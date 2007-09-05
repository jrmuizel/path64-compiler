/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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
 *
 * This file deals with compiler reconfigurability.
 * It is used to perform dynamic initialization of the targinfo description
 * used by LAI.
 * It supports extending the targinfo model, not shrinking it.
 *
 */
#include "../gccfe/extension_include.h"
#include "dyn_dll_api.h"
#include "loader.h"
#include "erglob.h"
#include "errors.h"
#include "config.h"
#include "tracing.h"
#include "intrn_info.h"
#include <stdlib.h>

#ifdef BACK_END
#include "dyn_isa_api.h"
#undef inline
#include "symtab_idx.h"
#include "targ_sim.h"
#include "lai_loader_api.h"
extern "C" {
#include "W_dlfcn.h"		    /* for sgidladd(), dlerror() */
};
#include "register_preg.h"

// ==========================================
// MType <-> Register Class Conversion tables
// ==========================================
static ISA_REGISTER_CLASS *extension_MTYPE_to_REGISTER_CLASS_table = (ISA_REGISTER_CLASS*)NULL;
static ISA_REGISTER_SUBCLASS *extension_MTYPE_to_REGISTER_SUBCLASS_table = (ISA_REGISTER_SUBCLASS*)NULL;
typedef struct {
  int      typeCount; 
  TYPE_ID *typeTab; // All mtypes associated to an extension register class, each with their specific size
} rc_to_mtype_info_t;
static rc_to_mtype_info_t *extension_REGISTER_CLASS_to_MTYPE_table = (rc_to_mtype_info_t*)NULL;


// ==============
// Intrinsic data
// ==============
typedef struct {
  DYN_INTRN_TYPE type;
  union {
    TOP top;
    int compose_extract_idx;
  } u1;
} extension_intrinsic_info_t;

static extension_intrinsic_info_t *extension_INTRINSIC_table = (extension_intrinsic_info_t*)NULL;


// Keep access to extension info table
static Lai_Loader_Info_t *global_ext_info_table;

/*
 * Return a pointer to the description of extension register class
 */
const extension_regclass_t *EXTENSION_get_REGISTER_CLASS_info(ISA_REGISTER_CLASS rc) {
  FmtAssert((extension_regclass_table!=NULL), ("Unexpected NULL extension_regclass_table"));
  if (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX) {
    return (extension_regclass_table[rc - (ISA_REGISTER_CLASS_STATIC_MAX+1)]);
  }
  return ((const extension_regclass_t*)NULL);
}

/*
 * Return the base PREG number for a register class
 */
INT EXTENSION_get_Min_Offset(ISA_REGISTER_CLASS rc) {
  FmtAssert((extension_RegClass_To_Preg_Min_Offset_table!=NULL), ("Unexpected NULL extension_RegClass_To_Preg_Min_Offset_table"));
  if (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX) {
    return (extension_RegClass_To_Preg_Min_Offset_table[rc - (ISA_REGISTER_CLASS_STATIC_MAX+1)]);
  }
  FmtAssert((0),("Unexpected Register Class, out of extension bounds: %d", rc));
  return 0;
}

/*
 * Return the name of the extension containing the specified INTRINSIC id,
 * or NULL if not found
 */
const char *EXTENSION_Get_Extension_Name_From_INTRINSIC(INTRINSIC id) {
  int ext=0;
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT) {
    while (ext < (global_ext_info_table->nb_ext-1) && 
	   id >= global_ext_info_table->base_INTRINSIC[ext+1]) {
      ext++;
    }
    return (global_ext_info_table->dll_tab[ext].extname);
  }

  return (NULL);
}

/*
 * Return the name of the extension containing the specified TOP id,
 * or NULL if not found
 */
const char *EXTENSION_Get_Extension_Name_From_TOP(TOP id) {
  int ext=0;
  if (id > TOP_static_count && id < TOP_count) {
    while (ext < (global_ext_info_table->nb_ext-1) && 
	   id >= global_ext_info_table->base_TOP[ext+1]) {
      ext++;
    }
    return (global_ext_info_table->dll_tab[ext].extname);
  }

  return (NULL);
}

/*
 * Return the name of the extension containing the specified REGISTER CLASS rc,
 * or NULL if not found
 */
const char *EXTENSION_Get_Extension_Name_From_REGISTER_CLASS(ISA_REGISTER_CLASS rc) {
  int ext=0;
  if (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX) {
    while (ext < (global_ext_info_table->nb_ext-1) && 
	   rc >= global_ext_info_table->base_REGISTER_CLASS[ext+1]) {
      ext++;
    }
    return (global_ext_info_table->dll_tab[ext].extname);
  }

  return (NULL);
}

/*
 * Return the TOP corresponding to the INTRINSIC identifier given in parameter
 * Return TOP_UNDEFINED if the intrinsic id is invalid
 */
TOP EXTENSION_INTRINSIC_to_TOP(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT &&
      (extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)].type == DYN_INTRN_TOP)) {
    return (extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)].u1.top);
  }
  return (TOP_UNDEFINED);
}


/*
 * Return true if the INTRINSIC identified by 'id' is associated with a TOP,
 * false otherwise.
 */
bool EXTENSION_Is_TOP_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT) {
    return (extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)].type==DYN_INTRN_TOP);
  }
  return (false);
}


/*
 * Return true if the INTRINSIC identified by 'id' is a full compose INTRINSIC,
 * meaning that it is used to fully initialized a SIMD data by subpart,
 * false otherwise.
 */
bool EXTENSION_Is_Compose_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT) {
    return (extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)].type==DYN_INTRN_COMPOSE);
  }
  return (false);
}


/*
 * Return true if the INTRINSIC identified by 'id' is a partial compose INTRINSIC,
 * meaning that it is used to update a part of a SIMD data,
 * false otherwise.
 */
bool EXTENSION_Is_Partial_Compose_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT) {
    return (extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)].type==DYN_INTRN_PARTIAL_COMPOSE);
  }
  return (false);
}


/*
 * Return true if the INTRINSIC identified by 'id' is an extract INTRINSIC,
 * meaning that it is used to extract a part of a SIMD data,
 * false otherwise.
 */
bool EXTENSION_Is_Partial_Extract_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT) {
    return (extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)].type==DYN_INTRN_PARTIAL_EXTRACT);
  }
  return (false);
}


/*
 * Return the index associated with current INTRINSIC, which must be either a 
 * partial compose or a partial extract. This index is used to know which part
 * of the SIMD data should beextracted/updated.
 */
int EXTENSION_Get_ComposeExtract_Index(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT) {
    extension_intrinsic_info_t *info;
    info = &extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)];
    FmtAssert((info->type==DYN_INTRN_PARTIAL_EXTRACT ||
	       info->type==DYN_INTRN_PARTIAL_COMPOSE),
	      ("Not a compose or extract intrinsic"));
    return (info->u1.compose_extract_idx);
  }
  return (0);
}

/*
 * Return the register number associated to the specified PREG number .
 */
int EXTENSION_PREG_to_REGISTER_NUM(PREG_NUM preg) {
  FmtAssert((extension_Preg_To_RegNum_table!=NULL),
			("Unexpected NULL extension_Preg_To_RegNum_table"));
  if (preg > Get_Static_Last_Dedicated_Preg_Offset() && preg  <= Last_Dedicated_Preg_Offset) {
    return (extension_Preg_To_RegNum_table[preg - Get_Static_Last_Dedicated_Preg_Offset() - 1]);
  }
  FmtAssert((0),("Unexpexted PREG out of extension bounds: %d", preg));
}

/*
 * Return the register class associated to the specified machine type, or
 * ISA_REGISTER_CLASS_UNDEFINED if not defined.
 */
ISA_REGISTER_CLASS EXTENSION_MTYPE_to_REGISTER_CLASS(TYPE_ID mtype) {
  FmtAssert((extension_MTYPE_to_REGISTER_CLASS_table!=NULL),
			("Unexpected NULL extension_MTYPE_to_REGISTER_CLASS_table"));
  if (mtype > MTYPE_STATIC_LAST && mtype < FIRST_COMPOSED_MTYPE) {
    return (extension_MTYPE_to_REGISTER_CLASS_table[mtype - (MTYPE_STATIC_LAST+1)]);
  }
  FmtAssert((0),("Unexpexted MTYPE: %d", mtype));
  return (ISA_REGISTER_CLASS_UNDEFINED);
}

/*
 * Return the unique register subclass associated to the specified machine type, or
 * ISA_REGISTER_CLASS_UNDEFINED if not defined.
 */
ISA_REGISTER_SUBCLASS EXTENSION_MTYPE_to_REGISTER_SUBCLASS(TYPE_ID mtype) {
  FmtAssert((extension_MTYPE_to_REGISTER_SUBCLASS_table!=NULL),
			("Unexpected NULL extension_MTYPE_to_REGISTER_SUBCLASS_table"));
  if (mtype > MTYPE_STATIC_LAST && mtype < FIRST_COMPOSED_MTYPE) {
    return (extension_MTYPE_to_REGISTER_SUBCLASS_table[mtype - (MTYPE_STATIC_LAST+1)]);
  }
  FmtAssert((0),("Unexpexted MTYPE: %d", mtype));
  return (ISA_REGISTER_SUBCLASS_UNDEFINED);
}

/*
 * Return the mtype associated to the specified register class and with the 
 * specified byte size, or MTYPE_UNKNOWN if either undefined or multiply
 * defined (several mtypes with same size associated to same rc).
 */
TYPE_ID EXTENSION_REGISTER_CLASS_to_MTYPE(ISA_REGISTER_CLASS rc, INT size) {
  int i;
  if (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX) {
    FmtAssert((extension_REGISTER_CLASS_to_MTYPE_table!=NULL),
	      ("Unexpected NULL extension_REGISTER_CLASS_to_MTYPE_table"));
    rc_to_mtype_info_t *info;
    TYPE_ID typ = MTYPE_UNKNOWN;
    info = &extension_REGISTER_CLASS_to_MTYPE_table[rc - (ISA_REGISTER_CLASS_STATIC_MAX+1)];
    for (i=0; i<info->typeCount; i++) {
      if (MTYPE_byte_size(info->typeTab[i]) == size) {
	if (typ != MTYPE_UNKNOWN) {
	  DevWarn("EXTENSION_REGISTER_CLASS_to_MTYPE(): "
		  "Several mtypes fit request: cannot disambiguate");
	  return (MTYPE_UNKNOWN);
	}
	typ = info->typeTab[i];
      }
    }
    return (typ);
  }
  return (MTYPE_UNKNOWN);
}

/*
 * Return the TYPE_ID corresponding to the extension machine type
 * identified by 'name' (MTYPE_UNKNOWN if undefined).
 */
static TYPE_ID Get_Extension_MTYPE_From_Name(const char *name) {
  mUINT32 i;
  for (i= MTYPE_STATIC_LAST + 1; i< FIRST_COMPOSED_MTYPE; i++) {
    if (!(strcmp(name, MTYPE_name(i)))) {
      return (i);
    }
  }
  return (MTYPE_UNKNOWN);
}


/*
 * For a given load/store TOP, return an equivalent TOP with an auto-modified
 * addressing mode, specified by the post_mod, pre_mod and regclass atttributes.
 * If it does not exist, TOP_UNDEFINED is returned.
 *
 * top      : initial TOP
 * post_mod : post-modified or pre-modified addressing mode
 * inc_mod  : increment or decrement addressing mode
 * regclass : is the offset to add given by value or register
 */
TOP EXTENSION_TOP_AM_automod_variant(TOP top,
				     BOOL post_mod,
				     BOOL inc_mod,
				     ISA_REGISTER_CLASS regclass) {
  int ext=0;

  if (ext >= global_ext_info_table->nb_ext) {
    return (TOP_UNDEFINED);
  }

  for (ext=0; ext < global_ext_info_table->nb_ext; ext++) {
    if (global_ext_info_table->ISA_tab[ext]->belong_TOP_to_ext(top)) {
      // Found the container extension
      return (global_ext_info_table->ISA_tab[ext]->get_TOP_AM_automod_variant(top, post_mod, inc_mod, regclass));
    }
  }

  return (TOP_UNDEFINED);
}

/**
 * @see lai_loader_api.h
 */
INT EXTENSION_Get_REGISTER_CLASS_Optimal_Alignment(ISA_REGISTER_CLASS rc, INT size) {
  INT min_size;
  const extension_regclass_t *rc_info;
  if (rc <= ISA_REGISTER_CLASS_STATIC_MAX || rc > ISA_REGISTER_CLASS_MAX) {
    return size; // Non-extension register class
  }

  rc_info  = EXTENSION_get_REGISTER_CLASS_info(rc);
  min_size = ISA_REGISTER_CLASS_INFO_Bit_Size(ISA_REGISTER_CLASS_Info(rc)) / 8;

  while (size >= min_size &&
	 (TOP_UNDEFINED == rc_info->get_load_TOP(/* reg size     */size,
						 /* base_type    */AM_BASE_DEFAULT,
						 /* offs_is_imm  */true,
						 /* offs_is_incr */true)) &&
	 (TOP_UNDEFINED == rc_info->get_store_TOP(/* reg size     */size,
						  /* base_type    */AM_BASE_DEFAULT,
						  /* offs_is_imm  */true,
						  /* offs_is_incr */true))) {
    size /= 2;
  }

  if (size >= min_size) {
    return size;
  } else {
    // No load/store available for current register class, core load/store will be used.
    return 4;
  }
}

/* ===========================================================================
 * Initialize ISA TOP tables from statically defined ones concatenated with
 * extension defined ones. Initialized data are: 
 * - ISA_generic_top_names         (array [TOP_count+1] UNDEFINED at end)
 * - ISA_PROPERTIES_flags          (array [TOP_count+1] UNDEFINED at end)
 * - ISA_PROPERTIES_TOP_mem_bytes  (array [TOP_count+1] UNDEFINED at end)
 * - ISA_PROPERTIES_TOP_mem_align  (array [TOP_count+1] UNDEFINED at end)
 *
 * Invoke call-back routines for base_TOP and TOP_UNDEFINED.
 * ===========================================================================*/
static void Initialize_ISA_Top(Lai_Loader_Info_t &ext_info) {
  int ext;
  const char **TOP_names_tab;
  mUINT64     *TOP_prop_tab;
  mUINT16     *TOP_mem_bytes_tab;
  mUINT16     *TOP_mem_align_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA TOPs\n");
  }

  if (TOP_count == TOP_static_count) {
    return;
  }
  
  // Allocate new tables for TOP related information and initialize with
  // the content of the initial static tables
  // Note: For all these tables, TOP_UNDEFINED value is kept at the end.
  TOP_names_tab = TYPE_MEM_POOL_ALLOC_N(const char*, Malloc_Mem_Pool, (TOP_count+1));
  memcpy(TOP_names_tab, ISA_generic_top_names, (TOP_static_count) * sizeof(const char *));
  TOP_names_tab[TOP_count] = ISA_generic_top_names[TOP_static_count]; // TOP_UNDEFINED
  
  TOP_prop_tab = TYPE_MEM_POOL_ALLOC_N(mUINT64, Malloc_Mem_Pool, (TOP_count+1));
  memcpy(TOP_prop_tab, ISA_PROPERTIES_flags, (TOP_static_count) * sizeof(mUINT64));
  TOP_prop_tab[TOP_count] = ISA_PROPERTIES_flags[TOP_static_count]; // TOP_UNDEFINED

  TOP_mem_bytes_tab = TYPE_MEM_POOL_ALLOC_N(mUINT16, Malloc_Mem_Pool, (TOP_count+1));
  memcpy(TOP_mem_bytes_tab, ISA_PROPERTIES_TOP_mem_bytes, (TOP_static_count) * sizeof(mUINT16));
  TOP_mem_bytes_tab[TOP_count] = ISA_PROPERTIES_TOP_mem_bytes[TOP_static_count]; // TOP_UNDEFINED
  
  TOP_mem_align_tab = TYPE_MEM_POOL_ALLOC_N(mUINT16, Malloc_Mem_Pool, (TOP_count+1));
  memcpy(TOP_mem_align_tab, ISA_PROPERTIES_TOP_mem_align, (TOP_static_count) * sizeof(mUINT16));
  TOP_mem_align_tab[TOP_count] = ISA_PROPERTIES_TOP_mem_align[TOP_static_count]; // TOP_UNDEFINED

  // Complete new tables with extension specific data
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      memcpy(&TOP_names_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_TOP_name_tab(),
	     top_in_ext * sizeof(const char *));
      
      memcpy(&TOP_prop_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_TOP_prop_tab(),
	     top_in_ext * sizeof(mUINT64));
      
      memcpy(&TOP_mem_bytes_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_TOP_mem_bytes_tab(),
	     top_in_ext * sizeof(mUINT16));
      
      memcpy(&TOP_mem_align_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_TOP_mem_align_tab(),
	     top_in_ext * sizeof(mUINT16));

      { // Remap ISA properties attributes if necessary
	INT    nb_remap;
	INT    ext_attr_count = ext_info.ISA_tab[ext]->get_ISA_PROPERTIES_attribute_tab_sz();
	const ISA_PROPERTIES_ATTRIBUTE *core_attr_tab = ISA_PROPERTIES_get_attribute_table();
	const ISA_PROPERTIES_ATTRIBUTE *ext_attr_tab  = ext_info.ISA_tab[ext]->get_ISA_PROPERTIES_attribute_tab();
	COMPUTE_ATTR_REMAP_TABLE ( /* in_EXT_NAME        */ ext_info.dll_tab[ext].dllname,
				   /* in_COMPONENT_NAME  */ "TOP properties",
				   /* in_ATTR_TYPE       */ UINT64,
				   /* in_ATTR_SUFFIX     */ ULL,
				   /* in_ATTR_FORMAT     */ "%llx",
				   /* in_CORE_ATTR_COUNT */ ISA_PROPERTIES_ATTRIBUTE_COUNT,
				   /* in_CORE_ATTR_TAB   */ core_attr_tab,
				   /* in_EXT_ATTR_COUNT  */ ext_attr_count,
				   /* in_EXT_ATTR_TAB    */ ext_attr_tab,
				   /* out_NB_REMAP       */ nb_remap
				   );
	if (nb_remap>0) {
	  mUINT64 *attr_walker = &TOP_prop_tab[ext_info.base_TOP[ext]];
	  for (int top=0; top<top_in_ext; top++) {
	    REMAP_ATTR ( /* in_ATTR_TYPE   */ UINT64,
			 /* in_ATTR_SUFFIX */ ULL,
			 /* in_ATTR        */ *attr_walker,
			 /* out_ATTR       */ *attr_walker
			 );
	    attr_walker++;
	  }
	}
	CLEANUP_ATTR_REMAP_TABLE();
      }

      // ...Update base index in dll
      ext_info.ISA_tab[ext]->set_TOP_base_offset(ext_info.base_TOP[ext]);

      // Initialize TOP_UNDEFINED dll private copy
      ext_info.ISA_tab[ext]->set_TOP_UNDEFINED(TOP_UNDEFINED);
    }
  }
  
  // Update targinfo pointers
  ISA_generic_top_names        = TOP_names_tab;
  ISA_PROPERTIES_flags         = TOP_prop_tab;
  ISA_PROPERTIES_TOP_mem_bytes = TOP_mem_bytes_tab;
  ISA_PROPERTIES_TOP_mem_align = TOP_mem_align_tab;
}


/* ===========================================================================
 * Initialize ISA Print tables from statically defined ones concatenated with
 * extension defined ones. Initialized data are: 
 * - ISA_PRINT_info_size    (int)
 * - ISA_PRINT_info         (array [ISA_PRINT_info_size])
 * - ISA_PRINT_info_index   (array [TOP_count])
 * - ISA_PRINT_asmname      (array [TOP_count+1], with UNDEFINED at the end)
 * ===========================================================================*/
static void Initialize_ISA_Print(Lai_Loader_Info_t &ext_info) {
  int ext, j;
  ISA_PRINT_INFO *info_tab;
  mUINT16        *info_index_tab;
  const char    **name_tab;
  int             base_PRINT_info;  // base offset for current extension

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA print\n");
  }

  if (TOP_count == TOP_static_count) {
    return; // No top in extension
  }

  // Compute total count of PRINT_info
  int nb_pi = ISA_PRINT_info_size;
  for (ext=0; ext < ext_info.nb_ext; ext++) {
    nb_pi += ext_info.ISA_tab[ext]->get_ISA_PRINT_info_tab_sz();
  }

  // Allocate new table and fill it with the content
  // of the initial static table
  info_index_tab = TYPE_MEM_POOL_ALLOC_N(mUINT16, Malloc_Mem_Pool, (TOP_count));
  memcpy(info_index_tab, ISA_PRINT_info_index, TOP_static_count * sizeof(mUINT16));
  
  name_tab = TYPE_MEM_POOL_ALLOC_N(const char*, Malloc_Mem_Pool, (TOP_count+1));
  memcpy(name_tab, ISA_PRINT_asmname, TOP_static_count * sizeof(const char *));
  name_tab[TOP_count] = ISA_PRINT_asmname[TOP_static_count]; // TOP_UNDEFINED
  
  info_tab = TYPE_MEM_POOL_ALLOC_N(ISA_PRINT_INFO, Malloc_Mem_Pool, nb_pi);
  memcpy(info_tab, ISA_PRINT_info, ISA_PRINT_info_static_size * sizeof(ISA_PRINT_INFO));

  base_PRINT_info = ISA_PRINT_info_size;
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    // Concatenate extension defined PRINT_info
    int pi_in_ext = ext_info.ISA_tab[ext]->get_ISA_PRINT_info_tab_sz();
    if (pi_in_ext > 0) {
      memcpy(&info_tab[base_PRINT_info],
	     ext_info.ISA_tab[ext]->get_ISA_PRINT_info_tab(),
	     pi_in_ext * sizeof(ISA_PRINT_INFO));
    }

    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      // Concatenate extension defined TOP name
      memcpy(&name_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_PRINT_name_tab(),
	     top_in_ext * sizeof(const char *));
      
      // Initialize association between TOP and PRINT_info
      // Note: the values need to be shifted
      const mUINT16 *ext_ii_tab = ext_info.ISA_tab[ext]->get_ISA_PRINT_info_index_tab();
      for (j=0; j<top_in_ext; j++) {
	if (ext_ii_tab[j]==0) {
	  // No output format for current operation
	  info_index_tab[ext_info.base_TOP[ext]+j] = 0;
	}
	else {
	  // Note: Subtract -1 to index because extension print format start at 1
	  info_index_tab[ext_info.base_TOP[ext]+j] = base_PRINT_info + ext_ii_tab[j] - 1;
	}
      }
    }
    base_PRINT_info += pi_in_ext;
  }
  
  // Update targinfo pointer
  ISA_PRINT_info_size  = nb_pi;
  ISA_PRINT_info       = info_tab;
  ISA_PRINT_info_index = info_index_tab;
  ISA_PRINT_asmname    = name_tab;
}


/* ===========================================================================
 * Initialize ISA Bundle tables from statically defined ones concatenated with
 * extension defined ones. Initialized data are: 
 * - ISA_EXEC_unit_prop    (array [TOP_count])
 *
 * Note: bundle extension support is very limited. Indeed, it is only possible
 *       to associate a bundle type to an instruction, it is not possible 
 *       define a new bundle type.
 *       TODO that, it will be required to extend the following table as well:
 *       - ISA_BUNDLE_pack_info_index
 *       - ISA_BUNDLE_pack_info
 *       - ISA_BUNDLE_info
 * ===========================================================================*/
static void Initialize_ISA_Bundle(Lai_Loader_Info_t &ext_info) {
  int ext;
  ISA_EXEC_UNIT_PROPERTY *unit_prop_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA bundles\n");
  }

  if (TOP_count == TOP_static_count) {
    return; // No top in extension
  }

  // Allocate new table and fill it with the content
  // of the initial static table
  unit_prop_tab = TYPE_MEM_POOL_ALLOC_N(ISA_EXEC_UNIT_PROPERTY, Malloc_Mem_Pool, TOP_count);
  memcpy(unit_prop_tab, ISA_EXEC_unit_prop, TOP_static_count * sizeof(ISA_EXEC_UNIT_PROPERTY));

  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      // Concatenate extension defined unit properties
      memcpy(&unit_prop_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_EXEC_unit_prop_tab(),
	     top_in_ext * sizeof(ISA_EXEC_UNIT_PROPERTY));
    }
  }
  
  // Update targinfo pointer
  ISA_EXEC_unit_prop = unit_prop_tab;
}


/* ===========================================================================
 * Initialize Literal classes from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_LIT_CLASS_info  (array [ISA_LC_MAX+1] UNDEFINED at 0)
 *
 * The following assumptions are made:
 * - Each extension LC table contains the static core LC followed by the
 *   extension specific ones.
 * 
 * TODO: Currently, extension specific LCs that are defined within
 *       several extensions are not merged in the final table.
 *       (Doing so will need update of the Operand intialization)
 *
 * NOTE: Currently, Core literal classes are not used by extension
 *       operands, there is then no remapping/coherency check done
 *       between the ones defined for the core and the ones embedded
 *       in extensions.
 * ===========================================================================*/
static void Initialize_ISA_Literals(Lai_Loader_Info_t &ext_info) {
  int ext;
  ISA_LIT_CLASS_INFO *lc_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA literals\n");
  }

  if (ISA_LC_MAX == ISA_LC_STATIC_MAX) {
    return; // No extension specific LC
  }
  
  // Allocate new table and fill it with the content
  // of the initial static table
  lc_tab = TYPE_MEM_POOL_ALLOC_N(ISA_LIT_CLASS_INFO, Malloc_Mem_Pool, ISA_LC_MAX+1);
  memcpy(lc_tab, ISA_LIT_CLASS_info, (ISA_LC_STATIC_MAX+1) * sizeof(ISA_LIT_CLASS_INFO));
  
  // Concatenate extension specific literal classes
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int static_max_in_ext = ext_info.ISA_tab[ext]->get_ISA_LIT_CLASS_static_max();
    int added = ext_info.ISA_tab[ext]->get_ISA_LIT_CLASS_info_tab_sz() - (static_max_in_ext+1);
    if (added>0) {
      memcpy(&lc_tab[ext_info.base_LIT_CLASS[ext]],
	     &(ext_info.ISA_tab[ext]->get_ISA_LIT_CLASS_info_tab()[static_max_in_ext+1]),
	     added * sizeof(ISA_LIT_CLASS_INFO));
    }
  }
    
  // Update targinfo pointer
  ISA_LIT_CLASS_info = lc_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "Summary of updated ISA literals information:\n");
    fprintf(TFile, "  -> Literal Max           : %8d (static: %d)\n",
	    ISA_LC_MAX, ISA_LC_STATIC_MAX);
  }
}


/* ===========================================================================
 * Initialize Enumeration classes from statically defined ones concatenated 
 * with extension defined ones. Initialized data are:
 * - ISA_ENUM_CLASS_info        (array [ISA_EC_MAX]  UNDEFINED at 0)
 * - ISA_ENUM_CLASS_VALUE_info  (array [ISA_ECV_MAX] UNDEFINED at 0)
 *
 * ===========================================================================*/
static void Initialize_ISA_Enum(Lai_Loader_Info_t &ext_info) {
  int ext, j;
  int base_ecv;
  ISA_ENUM_CLASS_INFO       *ec_tab;
  ISA_ENUM_CLASS_VALUE_INFO *ecv_tab;
  ISA_ENUM_CLASS_INFO       *ec_ptr;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA enumerations\n");
  }

  if (ISA_EC_MAX == ISA_EC_STATIC_MAX) {
    return; // No extension specific enum
  }
  
  // Allocate new table and fill it with the content
  // of the initial static table
  ec_tab = TYPE_MEM_POOL_ALLOC_N(ISA_ENUM_CLASS_INFO, Malloc_Mem_Pool, ISA_EC_MAX);
  memcpy(ec_tab, ISA_ENUM_CLASS_info, ISA_EC_STATIC_MAX * sizeof(ISA_ENUM_CLASS_INFO));

  ecv_tab = TYPE_MEM_POOL_ALLOC_N(ISA_ENUM_CLASS_VALUE_INFO, Malloc_Mem_Pool, ISA_ECV_MAX);
  memcpy(ecv_tab, ISA_ENUM_CLASS_VALUE_info, ISA_ECV_STATIC_MAX * sizeof(ISA_ENUM_CLASS_VALUE_INFO));
  
  // Concatenate extension specific Enumeration classes
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int ec_in_ext = ext_info.ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_tab_sz();
    if (ec_in_ext>0) {
      memcpy(&ec_tab[ext_info.base_ENUM_CLASS[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_tab(),
	     ec_in_ext * sizeof(ISA_ENUM_CLASS_INFO));
      
      // Renumber references to local ECV
      ec_ptr   = &ec_tab[ext_info.base_ENUM_CLASS[ext]];
      base_ecv = ext_info.base_ENUM_CLASS_VALUE[ext];
      for (j=0; j<ec_in_ext; j++) {
	ec_ptr->first += base_ecv;
	ec_ptr->last  += base_ecv;
	ec_ptr++;
      }

      // Concatenate extension ECV
      int ecv_in_ext = ext_info.ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz();
      memcpy(&ecv_tab[base_ecv],
	     ext_info.ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_VALUE_tab(),
	     ecv_in_ext * sizeof(ISA_ENUM_CLASS_VALUE_INFO));
    }
  }
    
  // Update targinfo pointer
  ISA_ENUM_CLASS_info       = ec_tab;
  ISA_ENUM_CLASS_VALUE_info = ecv_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "Summary of updated ISA enumeration information:\n");
    fprintf(TFile, "  -> Enumeration Max           : %8d (static: %d)\n",
	    ISA_EC_MAX, ISA_EC_STATIC_MAX);
    fprintf(TFile, "  -> Enumeration Value Max     : %8d (static: %d)\n",
	    ISA_ECV_MAX, ISA_ECV_STATIC_MAX);
  }
}


/* ===========================================================================
 * Initialize ISA Operands from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_OPERAND_operand_types   (array [ISA_OPERAND_TYPES_COUNT])
 * - ISA_OPERAND_info            (array [ISA_OPERAND_INFO_COUNT])
 * - ISA_OPERAND_info_index      (array [TOP_count])
 * - ISA_OPERAND_relocatable_opnd(array [TOP_count])
 *
 * Assumptions:
 * - Extension TOPs use only extension defined OPERAND_info, not core ones
 *   (not a problem if they are duplicated)
 * ===========================================================================*/
static void Initialize_ISA_Operands(Lai_Loader_Info_t &ext_info,
				    MEM_POOL        &tmp_mempool) {
  int ext, j, k;
  int *ext_base_OPERAND_TYPE; // base for additionnal operands types
  int *ext_base_OPERAND_info;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA operands\n");
  }

  // Compute base offset and maximum bounds
  ext_base_OPERAND_TYPE = TYPE_MEM_POOL_ALLOC_N(int, &tmp_mempool, ext_info.nb_ext);
  ext_base_OPERAND_info = TYPE_MEM_POOL_ALLOC_N(int, &tmp_mempool, ext_info.nb_ext);

  int nb_optypes = ISA_OPERAND_TYPES_STATIC_COUNT;
  int nb_opinfo  = ISA_OPERAND_INFO_STATIC_COUNT;
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    ext_base_OPERAND_TYPE[ext] = nb_optypes;
    nb_optypes += ext_info.ISA_tab[ext]->get_ISA_OPERAND_operand_types_tab_sz();

    ext_base_OPERAND_info[ext] = nb_opinfo;
    nb_opinfo += ext_info.ISA_tab[ext]->get_ISA_OPERAND_info_tab_sz();
  }
  

  if ((nb_optypes > ISA_OPERAND_TYPES_STATIC_COUNT) ||
      (nb_opinfo  > ISA_OPERAND_INFO_STATIC_COUNT )) {
    ISA_OPERAND_VALTYP *optypes_tab;
    ISA_OPERAND_INFO   *opinfo_tab;
    mUINT16            *opinfo_idx_tab;
    mINT8              *relocatable_opnd_tab;

    optypes_tab = TYPE_MEM_POOL_ALLOC_N(ISA_OPERAND_VALTYP, Malloc_Mem_Pool, nb_optypes);
    memcpy(optypes_tab, ISA_OPERAND_operand_types,
	   ISA_OPERAND_TYPES_STATIC_COUNT * sizeof(ISA_OPERAND_VALTYP));

    opinfo_tab = TYPE_MEM_POOL_ALLOC_N(ISA_OPERAND_INFO, Malloc_Mem_Pool, nb_opinfo);
    memcpy(opinfo_tab, ISA_OPERAND_info,
	   ISA_OPERAND_INFO_STATIC_COUNT * sizeof(ISA_OPERAND_INFO));

    opinfo_idx_tab = TYPE_MEM_POOL_ALLOC_N(mUINT16, Malloc_Mem_Pool, TOP_count);
    memcpy(opinfo_idx_tab, ISA_OPERAND_info_index, (TOP_static_count) * sizeof(mUINT16));

    relocatable_opnd_tab = TYPE_MEM_POOL_ALLOC_N(mINT8, Malloc_Mem_Pool, TOP_count);
    memcpy(relocatable_opnd_tab, ISA_OPERAND_relocatable_opnd, (TOP_static_count) * sizeof(mINT8));


    for (ext=0; ext<ext_info.nb_ext; ext++) {
      // Update operands types
      // .....................
      int added = ext_info.ISA_tab[ext]->get_ISA_OPERAND_operand_types_tab_sz();
      if (added>0) {
	const ISA_OPERAND_VALTYP *src_op;
	ISA_OPERAND_VALTYP       *tgt_op;
	int lc_static_max_in_ext = ext_info.ISA_tab[ext]->get_ISA_LIT_CLASS_static_max();
	int rc_static_max_in_ext = ext_info.ISA_tab[ext]->get_ISA_OPERAND_static_rclass_tab_sz()-1;
	int subrc_static_max_in_ext = ext_info.ISA_tab[ext]->get_ISA_OPERAND_static_rsubclass_tab_sz()-1;
	int rc_remap[ISA_REGISTER_CLASS_MAX_LIMIT];
	int subrc_remap[ISA_REGISTER_SUBCLASS_MAX_LIMIT];

	// Fill remap table for core REGISTER CLASSES used in current extension
	const char *const *rc_used;
	rc_used = ext_info.ISA_tab[ext]->get_ISA_OPERAND_static_rclass_tab();
	for (j=ISA_REGISTER_CLASS_MIN; j<=rc_static_max_in_ext; j++) {
	  if ((rc_used[j] == 0) ||
	      (strcmp(rc_used[j], ISA_REGISTER_CLASS_info[j].name) == 0)) {
	    rc_remap[j] = j;
	  }
	  else {
	    int k;
	    for (k=ISA_REGISTER_CLASS_MIN; k<=ISA_REGISTER_CLASS_STATIC_MAX; k++) {
	      if (strcmp(rc_used[j], ISA_REGISTER_CLASS_info[k].name) == 0) {
		rc_remap[j] = k; // Found 
		break;
	      }
	    }
	    if (k == ISA_REGISTER_CLASS_STATIC_MAX) {
	      char err_msg[256];
	      sprintf(err_msg,
		      "Incompatible target description, cannot find core register class %s",
		      rc_used[j]);
	      RaiseErrorIncompatibleLibrary(ext_info.dll_tab[ext].dllname, err_msg);
	    }
	  }
	}

	// Fill remap table for core REGISTER SUBCLASSES used in current extension
	const char *const *subrc_used;
	subrc_used = ext_info.ISA_tab[ext]->get_ISA_OPERAND_static_rsubclass_tab();
	for (j=ISA_REGISTER_SUBCLASS_MIN; j<=subrc_static_max_in_ext; j++) {
	  if ((subrc_used[j] == 0) ||
	      (strcmp(subrc_used[j], ISA_REGISTER_CLASS_info[j].name) == 0)) {
	    subrc_remap[j] = j;
	  }
	  else {
	    int k;
	    for (k=ISA_REGISTER_SUBCLASS_MIN; k<=ISA_REGISTER_SUBCLASS_STATIC_MAX; k++) {
	      if (strcmp(subrc_used[j], ISA_REGISTER_SUBCLASS_info[k].name) == 0) {
		subrc_remap[j] = k; // Found 
		break;
	      }
	    }
	    if (k == ISA_REGISTER_SUBCLASS_STATIC_MAX) {
	      char err_msg[256];
	      sprintf(err_msg,
		      "Incompatible target description, cannot find core register subclass %s",
		      subrc_used[j]);
	      RaiseErrorIncompatibleLibrary(ext_info.dll_tab[ext].dllname, err_msg);
	    }
	  }
	}

	src_op = ext_info.ISA_tab[ext]->get_ISA_OPERAND_operand_types_tab();
	tgt_op = &optypes_tab[ext_base_OPERAND_TYPE[ext]];
	for (j=0; j<added; j++, src_op++, tgt_op++) {
	  // Update regclasses info
	  tgt_op->rclass    = src_op->rclass;
	  tgt_op->rsubclass = src_op->rsubclass;
	  if (tgt_op->rclass > rc_static_max_in_ext) {
	    tgt_op->rclass += ext_info.base_REGISTER_CLASS[ext] - (rc_static_max_in_ext+1);
	    if (tgt_op->rsubclass != ISA_REGISTER_SUBCLASS_UNDEFINED) {
	      tgt_op->rsubclass += ext_info.base_REGISTER_SUBCLASS[ext] - (subrc_static_max_in_ext+1);
	    }
	  }
	  else if (tgt_op->rclass != ISA_REGISTER_CLASS_UNDEFINED) {
	    tgt_op->rclass = rc_remap[tgt_op->rclass];
	    if (tgt_op->rsubclass != ISA_REGISTER_SUBCLASS_UNDEFINED) {
	      tgt_op->rsubclass = subrc_remap[tgt_op->rsubclass];
	    }
	  }
	  // Update literals info
	  tgt_op->lclass = src_op->lclass;
	  if (tgt_op->lclass > lc_static_max_in_ext) {
	    tgt_op->lclass += ext_info.base_LIT_CLASS[ext] - (lc_static_max_in_ext+1);
	  }
	  else {
	    FmtAssert((tgt_op->lclass == ISA_LC_UNDEFINED),
		      ("Reuse of static literal classes in TOP operands not yet supported"));
	  }
	  // Update enum info
	  tgt_op->eclass = src_op->eclass;
	  if (tgt_op->eclass >= ISA_EC_STATIC_MAX) {
	    tgt_op->eclass += ext_info.base_ENUM_CLASS[ext] - ISA_EC_STATIC_MAX;
	  }
	  tgt_op->size   = src_op->size;
	  tgt_op->flags  = src_op->flags;
	}
      }

      // Update operand info
      // ...................
      int sz = ext_info.ISA_tab[ext]->get_ISA_OPERAND_info_tab_sz();
      const ISA_OPERAND_INFO *src_info;
      ISA_OPERAND_INFO       *tgt_info;
      src_info = ext_info.ISA_tab[ext]->get_ISA_OPERAND_info_tab();
      tgt_info = &opinfo_tab[ext_base_OPERAND_info[ext]];
      memcpy(tgt_info, src_info, sz * sizeof(ISA_OPERAND_INFO));
      for (j=0; j<sz; j++, tgt_info++) {
	// Update references to extension operand types
	for (k=0; k<tgt_info->opnds; k++) {
	  tgt_info->opnd[k] += ext_base_OPERAND_TYPE[ext];
	}
	for (k=0; k<tgt_info->results; k++) {
	  tgt_info->result[k] += ext_base_OPERAND_TYPE[ext];
	}
      }

      { // Remap ISA OPERAND USE attributes if necessary
	INT    nb_remap;
	INT    ext_attr_count = ext_info.ISA_tab[ext]->get_ISA_OPERAND_USE_attribute_tab_sz();
	const ISA_OPERAND_USE_ATTRIBUTE *core_attr_tab = ISA_OPERAND_USE_get_attribute_table();
	const ISA_OPERAND_USE_ATTRIBUTE *ext_attr_tab  = ext_info.ISA_tab[ext]->get_ISA_OPERAND_USE_attribute_tab();
	COMPUTE_ATTR_REMAP_TABLE ( /* in_EXT_NAME        */ ext_info.dll_tab[ext].dllname,
				   /* in_COMPONENT_NAME  */ "ISA OPERAND USE",
				   /* in_ATTR_TYPE       */ mUINT32,
				   /* in_ATTR_SUFFIX     */ UL,
				   /* in_ATTR_FORMAT     */ "%x",
				   /* in_CORE_ATTR_COUNT */ ISA_OPERAND_USE_ATTRIBUTE_COUNT,
				   /* in_CORE_ATTR_TAB   */ core_attr_tab,
				   /* in_EXT_ATTR_COUNT  */ ext_attr_count,
				   /* in_EXT_ATTR_TAB    */ ext_attr_tab,
				   /* out_NB_REMAP       */ nb_remap
				   );
	if (nb_remap>0) {
	  tgt_info = &opinfo_tab[ext_base_OPERAND_info[ext]];
	  for (j=0; j<sz; j++, tgt_info++) {
	    // Update references to operand uses
	    for (k=0; k<tgt_info->opnds; k++) {
	      mUINT32 val = tgt_info->ouse[k]; // tempo used because ouse tab is UINT16
	      REMAP_ATTR ( /* in_ATTR_TYPE   */ mUINT32,
			   /* in_ATTR_SUFFIX */ UL,
			   /* in_ATTR        */ val,
			   /* out_ATTR       */ val
			   );
	      tgt_info->ouse[k] = val;
	    }
	    for (k=0; k<tgt_info->results; k++) {
	      mUINT32 val = tgt_info->ruse[k]; // tempo used because ruse tab is UINT16
	      REMAP_ATTR ( /* in_ATTR_TYPE   */ mUINT32,
			   /* in_ATTR_SUFFIX */ UL,
			   /* in_ATTR        */ val,
			   /* out_ATTR       */ val
			   );
	      tgt_info->ruse[k] = val;
	    }
	  }
	}
	CLEANUP_ATTR_REMAP_TABLE();
      }

      // Update operand index info
      // .........................
      int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();

      const mUINT16 *ext_ii_tab = ext_info.ISA_tab[ext]->get_ISA_OPERAND_info_index_tab();
      mUINT16 base = ext_base_OPERAND_info[ext];

      for (j=0; j<top_in_ext; j++) {
	opinfo_idx_tab[ext_info.base_TOP[ext]+j] = base + ext_ii_tab[j];
      }

      // Update relocatable operand info
      // ...............................
      memcpy(&relocatable_opnd_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_OPERAND_relocatable_opnd_tab(),
	     top_in_ext * sizeof(mINT8));
    }
    
    // Update targinfo pointer
    ISA_OPERAND_types_count   = nb_optypes;
    ISA_OPERAND_operand_types = optypes_tab;
    ISA_OPERAND_info          = opinfo_tab;
    ISA_OPERAND_info_index    = opinfo_idx_tab;
    ISA_OPERAND_relocatable_opnd = relocatable_opnd_tab;
  }
}


/* ===========================================================================
 * Initialize ISA Subsets from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_SUBSET_opcode_table   (array)
 *
 * ISA subsets are now represented as set of booleans(1/0), which simplifies
 * greatly the initialization procedure (ISA subsets were previously to be array
 * of bits).
 * ===========================================================================*/
static void Initialize_ISA_Subsets(Lai_Loader_Info_t &ext_info) {

  unsigned const char **ext_subsets_tab;
  unsigned char        *new_subsets_tab[ISA_SUBSET_MAX+1];
  unsigned char        *new_subset_tab;
  int                   top_in_ext;          // Nb of top in an extensions.
  int                   ext;
  int                   subset;
  const unsigned int    sz_uchar = sizeof(unsigned char);

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA subsets\n");
  }

  if (TOP_count == TOP_static_count) {        // No extension top: easy work.
    return;
  }
 
  // Check that subsets are enumerated in the 
  // range [0...ISA_SUBSET_MAX,ISA_SUBSET_UNDEFINED]
  FmtAssert(ISA_SUBSET_MIN==0 && ISA_SUBSET_MAX+1==ISA_SUBSET_UNDEFINED,
            ("Error: uncoherent subset identifiers"));

  // Initialize static part of isa subset (including UNDEFINED subset).
  for (subset=ISA_SUBSET_MIN; subset<=ISA_SUBSET_UNDEFINED; subset++) {
    new_subset_tab = TYPE_MEM_POOL_ALLOC_N(unsigned char, 
                                           Malloc_Mem_Pool,
                                           TOP_count);
    memcpy(new_subset_tab,
           ISA_SUBSET_opcode_table[subset],
           TOP_static_count*sz_uchar);

    new_subsets_tab[subset]         = new_subset_tab;
    ISA_SUBSET_opcode_table[subset] = new_subset_tab;
  }

  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();

    // Checking internal index range
    if(ext==ext_info.nb_ext-1) {
      FmtAssert(top_in_ext+ext_info.base_TOP[ext]==(int)TOP_count,
         ("Internal error: bad index range for extension TOP"));
    } else {
      FmtAssert(ext_info.base_TOP[ext]+top_in_ext==ext_info.base_TOP[ext+1],
         ("Internal error: bad index range for extension TOP"));
    }

    if(0==top_in_ext) {                     // No top : cool!
      continue;
    }

    ext_subsets_tab = ext_info.ISA_tab[ext]->get_ISA_SUBSET_op_tab();

    // Check that we have one and only one extension subset
    FmtAssert(1==ext_info.ISA_tab[ext]->get_ISA_SUBSET_tab_sz(),
    ("Error: current model for extensions only define one ISA subset for each extension.\n"));


    // Table concatenation for all core subsets.
    // TODO: normally we should check whether current subset is
    // extensible or not.
    for (subset=ISA_SUBSET_MIN; subset<=ISA_SUBSET_MAX; subset++) {
      memcpy(new_subsets_tab[subset]+ext_info.base_TOP[ext],
             ext_subsets_tab[0],       // O is the only allowed index.
             top_in_ext*sz_uchar);
    }
  }

  return;
}

/* ===========================================================================
 * Initialize ISA Hazards from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_HAZARD_hazard_index   (array)
 *
 * ===========================================================================*/
static void Initialize_ISA_Hazards(Lai_Loader_Info_t &ext_info) {

  mUINT8 *new_hazards_index_tab;
  int ext;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA hazards\n");
  }

  if (TOP_count == TOP_static_count) {        // No extension top: easy work.
    return;
  }

  new_hazards_index_tab = TYPE_MEM_POOL_ALLOC_N(mUINT8,  
                                                Malloc_Mem_Pool, 
                                                TOP_count);
  // Concatenation of index tables.
  memcpy(new_hazards_index_tab, 
         ISA_HAZARD_hazard_index, 
         TOP_static_count * sizeof(mUINT8));

  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();

    memcpy(&new_hazards_index_tab[ext_info.base_TOP[ext]],
           ext_info.ISA_tab[ext]->get_ISA_HAZARDS_index_tab(),
           top_in_ext * sizeof(mUINT8));
  }

  // Update pointers
  ISA_HAZARD_hazard_index = new_hazards_index_tab;

  return;
}


/* ===========================================================================
 * Initialize Scheduling Information from statically defined ones concatenated
 * with extension defined ones. Initialized data are:
 * - SI_resource_count    (int)
 * - SI_resources         (array [SI_resource_count])
 * - SI_issue_slot_count  (int)
 * - SI_issue_slots       (array [SI_issue_slot_count])
 * - SI_RRW_initializer   (SI_RRW, scalar)
 * - SI_RRW_overuse_mask  (SI_RRW, scalar)
 * - SI_ID_count          (int)
 * - SI_ID_si             (array [SI_ID_count])
 * - SI_top_si            (array [TOP_count])
 * ===========================================================================*/
static void Initialize_SI(Lai_Loader_Info_t &ext_info,
			  MEM_POOL        &tmp_mempool) {
  const extension_si_t ** ext_proc_si_info;
  int  ext, j;
  // ...ptrs to newly allocated tables
  SI            **top_si_tab;
  SI            **si_tab;
  SI_RESOURCE   **res_tab;
  SI_ISSUE_SLOT **islot_tab;
  // ...count for statically defined elements
  int  static_si_count;
  int  static_res_count;
  int  static_islot_count;
  // ...counts after dynamic loading
  int  new_si_count;
  int  new_res_count;
  int  new_islot_count;
  // ...count for statically defined elements present in each extension
  int *static_res_count_in_ext;
  int *static_islot_count_in_ext;
  // ...base offsets to access elements of current extension within the new tables
  int  ext_base_si;
  int  ext_base_res;
  int  ext_base_islot;
  // ...nb of bits used by statically defined resources in SI_RRW bit arrays
  int  nb_static_bits;
  int *nb_static_bits_in_ext;
  // ...nb of bits used by all resources in SI_RRW bit arrays
  int  nb_total_bits;
  // ...mask containing all bits used by statically defined resources in SI_RRW arrays
  SI_RRW  mask_static_bits;
  SI_RRW *mask_static_bits_in_ext;
  // ...mask of statically defined resources (1 bit/resource)
  SI_RESOURCE_ID_SET  mask_static_res;
  SI_RESOURCE_ID_SET *mask_static_res_in_ext;
  // ...processor name 
  const char *proc_name = PROCESSOR_Name(PROCESSOR_Value);

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension Scheduling information\n");
  }

  if (TOP_count == TOP_static_count) {
    return; // No top in extension
  }

  // Allocate temporary memory for SI pointers
  ext_proc_si_info          = TYPE_MEM_POOL_ALLOC_N(const extension_si_t*, &tmp_mempool, ext_info.nb_ext);
  static_res_count_in_ext   = TYPE_MEM_POOL_ALLOC_N(int, &tmp_mempool, ext_info.nb_ext);
  static_islot_count_in_ext = TYPE_MEM_POOL_ALLOC_N(int, &tmp_mempool, ext_info.nb_ext);
  nb_static_bits_in_ext     = TYPE_MEM_POOL_ALLOC_N(int, &tmp_mempool, ext_info.nb_ext);
  mask_static_bits_in_ext   = TYPE_MEM_POOL_ALLOC_N(SI_RRW, &tmp_mempool, ext_info.nb_ext);
  mask_static_res_in_ext    = TYPE_MEM_POOL_ALLOC_N(SI_RESOURCE_ID_SET, &tmp_mempool, ext_info.nb_ext);

  // Retrieve SI info and compute total number of SI
  static_si_count    = SI_ID_Count();
  static_res_count   = Get_SI_resource_count();
  static_islot_count = Get_SI_issue_slot_count();
  new_si_count       = static_si_count;
  new_res_count      = static_res_count;
  new_islot_count    = static_islot_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    // Find SI description for current processor
    ext_proc_si_info[ext] = 0;
    // NOTE: The following code will probably be updated. We will consider that only a
    //       single scheduling description will be available for an extension
    for (j=0; j < (int)ext_info.ISA_tab[ext]->get_scheduling_info_tab_sz(); j++) {
      if ((j==0) /*patch to take the first si, even if the processor name is not the same */ ||
	  !strcmp(ext_info.ISA_tab[ext]->get_scheduling_info_tab()[j].get_processor_name(),
		  proc_name)) {
	ext_proc_si_info[ext] = &(ext_info.ISA_tab[ext]->get_scheduling_info_tab()[j]);
	break;
      }
    }
    FmtAssert((ext_proc_si_info[ext]!=NULL),
	      ("ERROR: Targinfo dynamic loading failed: Cannot locate scheduling"
	       " information for processor '%s' in current extension\n",
	       proc_name));

    /* Core/Extension compatibility check
     * Check that the extension resources are defined as follow:
     * - Resources [0..i] are defined in the core, in the same order,
     *                    with exactly the same description
     * - Resources [i+1..n] are extension private
     * Note: the core might contain more than i resources, they will be
     *       ignored by the extension (this allow addition of resources
     *       in the core without breaking compatibility  with  existing
     *       extension libraries)
     */
    {
      int u;
      char err_msg[256];
      bool ok = true;

      // ...Check Resources
      int ext_res_count = ext_proc_si_info[ext]->get_SI_resource_count();
      SI_RESOURCE * const *ext_res_tab = ext_proc_si_info[ext]->get_SI_resources_tab();
      SI_RESOURCE * TI_SI_CONST* static_res_tab = Get_SI_resources();
      for (u = 0; (u<static_res_count) && (u<ext_res_count); u++) {
	if (strcmp(SI_RESOURCE_Name(ext_res_tab[u]), SI_RESOURCE_ID_Name(u))==0) {
	  if ((SI_RESOURCE_Avail_Per_Cycle(ext_res_tab[u]) != SI_RESOURCE_Avail_Per_Cycle(static_res_tab[u])) ||
	      (SI_RESOURCE_Word_Index     (ext_res_tab[u]) != SI_RESOURCE_Word_Index     (static_res_tab[u])) ||
	      (SI_RESOURCE_Bit_Index      (ext_res_tab[u]) != SI_RESOURCE_Bit_Index      (static_res_tab[u]))
	      ) {
	    ok = false;
	    sprintf(err_msg,
		    "Extension incompatible with Core (resources). "
		    "Incompatible resource description %s",
		    SI_RESOURCE_ID_Name(u));
	    RaiseErrorIncompatibleLibrary(ext_info.dll_tab[ext].dllname,
					  err_msg);
	    break;
	  }
	}
	else {
	  // Reach the first extension private resource, coherency check below
	  break;
	}
      }
      static_res_count_in_ext[ext] = u;
      
      // All remaining resources must be specific to the extension
      // -> they must begin with the extension name prefixed by 'Resource_'
      for ( ; u<ext_res_count; u++) {
	const char prefix[]  = "Resource_";
	const char *res_name = SI_RESOURCE_Name(ext_res_tab[u]);
	if ((strncmp(prefix, res_name, strlen(prefix)) != 0) ||
	    (strncmp(ext_info.dll_tab[ext].extname,
		     &res_name[strlen(prefix)],
		     strlen(ext_info.dll_tab[ext].extname)) != 0)) {
	  ok = false;
	  sprintf(err_msg,
		  "Extension contains incompatible scheduling information"
		  " (unexpected resource '%s' at rank %d).",
		  res_name, u);
	  RaiseErrorIncompatibleLibrary(ext_info.dll_tab[ext].dllname,
					err_msg);
	}
      }

      // ...Check Slots
      if (ok) {
	int ext_islot_count = ext_proc_si_info[ext]->get_SI_issue_slot_count();
	if (ext_islot_count < static_islot_count) {
	  ok = false;
	  RaiseErrorIncompatibleLibrary(ext_info.dll_tab[ext].dllname,
					"ERROR: Too few slots in extension");
	}
	else {
	  SI_ISSUE_SLOT * const *ext_islot_tab = ext_proc_si_info[ext]->get_SI_issue_slots_tab();
	  for (u=0; u<ext_islot_count; u++) {
	    if (strcmp(SI_ISSUE_SLOT_Name(ext_islot_tab[u]), SI_ISSUE_SLOT_Name(SI_Ith_Issue_Slot(u)))==0) {
	    }
	    else {
	      // Reached the first extension private slot, coherency check below
	      break;
	    }
	  }
	  static_islot_count_in_ext[ext] = u;

	  // All remaining instruction slots must be specific to the extension
	  // -> they must begin with the extension name prefixed by 'ISlot_'
	  for ( ; u<ext_islot_count; u++) {
	    const char  prefix[]  = "Slot_";
	    const char *slot_name = SI_ISSUE_SLOT_Name(ext_islot_tab[u]);
	    if ((strncmp(prefix, slot_name, strlen(prefix)) != 0) ||
		(strncmp(ext_info.dll_tab[ext].extname,
			 &slot_name[strlen(prefix)],
			 strlen(ext_info.dll_tab[ext].extname)) != 0)) {
	      ok = false;
	      sprintf(err_msg,
		      "Extension contains incompatible scheduling information"
		      " (unexpected instruction slot '%s' at rank %d).",
		      slot_name, u);
	      RaiseErrorIncompatibleLibrary(ext_info.dll_tab[ext].dllname,
					    err_msg);
	    }
	  }
	}
      }

      FmtAssert(ok,("ERROR in SI description"));
    } // END of compatibility check
    
    new_si_count    +=  ext_proc_si_info[ext]->get_SI_ID_si_tab_sz();
    new_res_count   += (ext_proc_si_info[ext]->get_SI_resource_count() - static_res_count_in_ext[ext]);
    new_islot_count += (ext_proc_si_info[ext]->get_SI_issue_slot_count() - static_islot_count_in_ext[ext]);


    // Compute nb_static_bits_in_ext
    {
      SI_RESOURCE * const *ext_res_tab = ext_proc_si_info[ext]->get_SI_resources_tab();
      int nb_bits  = 0;
      SI_RRW tmask = 0ULL;
      for (j=0; j<static_res_count_in_ext[ext]; j++) {
	int nb_res = SI_RESOURCE_Avail_Per_Cycle(ext_res_tab[j]);
	while (nb_res>0) {
	  nb_res >>= 1;
	  nb_bits++;
	  tmask = (tmask<<1ULL)+1ULL;
	}
	// always one more bit used
	nb_bits++;
	tmask = (tmask<<1ULL)+1ULL;
     }
      
      nb_static_bits_in_ext[ext]   = nb_bits;
      mask_static_bits_in_ext[ext] = tmask;
      mask_static_res_in_ext[ext]  = (1ULL<<static_res_count_in_ext[ext]) - 1ULL;

      if (ext_info.trace_on) {
	fprintf(TFile, "nb static bit for ext: %d\n", nb_static_bits_in_ext[ext]);
      }
    }
  }

  // Allocate new tables and initialize static part
  top_si_tab = TYPE_MEM_POOL_ALLOC_N(SI*, Malloc_Mem_Pool, TOP_count);
  memcpy(top_si_tab, Get_SI_top_si(), TOP_static_count * sizeof(SI*));

  if (new_si_count > static_si_count) {
    si_tab = TYPE_MEM_POOL_ALLOC_N(SI*, Malloc_Mem_Pool, new_si_count);
    memcpy(si_tab, Get_SI_ID_si(), static_si_count * sizeof(SI*));
  }

  if (new_res_count > static_res_count) {
    SI_RESOURCE * TI_SI_CONST* static_res_tab;
    static_res_tab = Get_SI_resources();
    res_tab = TYPE_MEM_POOL_ALLOC_N(SI_RESOURCE*, Malloc_Mem_Pool, new_res_count);
    memcpy(res_tab, static_res_tab, static_res_count * sizeof(SI_RESOURCE*));

    // Compute masks and bits count for SI_RRW arrays
    nb_static_bits   = 0;
    mask_static_bits = 0ULL;
    SI_RRW mask = Get_SI_RRW_overuse_mask();
    while (mask) {
      mask = mask >> 1;
      nb_static_bits++;
      mask_static_bits = (mask_static_bits<<1ULL)+1ULL;
    }
    mask_static_res = (1ULL<<static_res_count) - 1ULL;
    nb_total_bits   = nb_static_bits;
  }

  if (new_islot_count > static_islot_count) {
    SI_ISSUE_SLOT * TI_SI_CONST* static_islot_tab;
    static_islot_tab = Get_SI_issue_slots();
    islot_tab = TYPE_MEM_POOL_ALLOC_N(SI_ISSUE_SLOT*, Malloc_Mem_Pool, new_islot_count);
    if (static_islot_count > 0) {
      memcpy(islot_tab, static_islot_tab, static_islot_count * sizeof(SI_ISSUE_SLOT*));
    }
  }

  // Concatenate extension specific SI
  ext_base_si    = static_si_count;
  ext_base_res   = static_res_count;
  ext_base_islot = static_islot_count;
  SI_RRW new_RRW_initializer  = Get_SI_RRW_initializer();
  SI_RRW new_RRW_overuse_mask = Get_SI_RRW_overuse_mask();
  for (ext=0; ext<ext_info.nb_ext; ext++) {

    int si_in_ext = ext_proc_si_info[ext]->get_SI_ID_si_tab_sz();
    if (si_in_ext) {
      memcpy(&si_tab[ext_base_si],
	     ext_proc_si_info[ext]->get_SI_ID_si_tab(),
	     (si_in_ext * sizeof(SI*)));
    }
    
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      memcpy(&top_si_tab[ext_info.base_TOP[ext]],
	     ext_proc_si_info[ext]->get_SI_top_si_tab(),
	     (top_in_ext * sizeof(SI*)));
    }

    int islot_in_ext = ext_proc_si_info[ext]->get_SI_issue_slot_count() - static_islot_count_in_ext[ext];
    if (islot_in_ext) {
      memcpy(&islot_tab[ext_base_islot],
	     &(ext_proc_si_info[ext]->get_SI_issue_slots_tab()[static_islot_count_in_ext[ext]]),
	     (islot_in_ext * sizeof(SI_ISSUE_SLOT*)));
    }

    int res_in_ext = (ext_proc_si_info[ext]->get_SI_resource_count() - static_res_count_in_ext[ext]);
    if (res_in_ext) {
      memcpy(&res_tab[ext_base_res],
	     &(ext_proc_si_info[ext]->get_SI_resources_tab()[static_res_count_in_ext[ext]]),
	     (res_in_ext * sizeof(SI_RESOURCE*)));

      // Shift index of extension resources
      for (j=0; j<res_in_ext; j++) {
	res_tab[ext_base_res+j]->bit_index += (nb_total_bits - nb_static_bits_in_ext[ext]);
      }

      // Update SI_RRW masks
      SI_RRW tmp;
      tmp = (ext_proc_si_info[ext]->get_SI_RRW_initializer() >> nb_static_bits_in_ext[ext]);
      tmp = tmp << nb_total_bits;
      new_RRW_initializer |= tmp;

      tmp = (ext_proc_si_info[ext]->get_SI_RRW_overuse_mask() >> nb_static_bits_in_ext[ext]);
      tmp = tmp << nb_total_bits;
      new_RRW_overuse_mask |= tmp;

      // Compute nb of bits used by current extension resources in SI_RRW bit arrays
      int nb_ext_bits = 0;
      SI_RRW mask = (ext_proc_si_info[ext]->get_SI_RRW_overuse_mask() >> nb_static_bits_in_ext[ext]);
      while (mask) {
	mask = mask >> 1;
	nb_ext_bits++;
      }

      // Update SI information
      for (j=0; j<si_in_ext; j++) {
	SI *current_si = si_tab[ext_base_si+j];
	current_si->id = ext_base_si+j;

	// Update resource requirement table:
	//   In SI_RRW & SI_RESOURCE_ID_SET bit arrays, references to statically
	//   defined resources must remain at the same position while references
	//   to extension specific resources must be shifted if resources from
	//   another extension has already been inserted.
	//
	// Example for SI_RESOURCE_ID_SET
	//   Extension1:     0001 1111
	//   Extension2:     0011 1111
	//                     \/ \__/
	//    Extensions have__/    \____ 4 statically defined resources
	//    respectively 1 and
	//    2 private resources
	//
	// The resulting bit array will be:
	//                   0111 1111
	//                    \/| \__/
	//        2 bits for _/ |   \____ 4 statically defined resources
	//        extension 2   \ 1 bit for extension 1
	//
	if (current_si->rr) {
	  int nb_cy = current_si->rr[0];
	  for (int cy=0; cy<nb_cy; cy++) {
	    SI_RRW tmp = current_si->rr[cy+1];
	    current_si->rr[cy+1] = (tmp & mask_static_bits_in_ext[ext]) |
	      ((tmp >> nb_static_bits_in_ext[ext]) << nb_total_bits);
	    
	    SI_RESOURCE_ID_SET tmp2 = current_si->resources_used[cy];
	    current_si->resources_used[cy] = (tmp2 & mask_static_res_in_ext[ext]) |
	      ((tmp2 >> static_res_count_in_ext[ext]) << ext_base_res);
	  }
	}

	// Update Issue slot tables
	// (same merging process as above)
	if (current_si->ii_info_size > 0) {
	  for (mUINT32 ii=0; ii<current_si->ii_info_size; ii++) {
	    if (current_si->ii_rr[ii]) {
	      int nb_cy = current_si->ii_rr[ii][0];
	      // TODO: check that current_si->ii_rr[ii] has not already been processed...
	      // it might be possible that the pointed table is referenced twice ...
	      for (int cy=0; cy<nb_cy; cy++) {
		SI_RRW tmp = current_si->ii_rr[ii][cy+1];
		current_si->ii_rr[ii][cy+1] = (tmp & mask_static_bits_in_ext[ext]) |
		  ((tmp >> nb_static_bits_in_ext[ext]) << nb_total_bits);
		
		SI_RESOURCE_ID_SET tmp2 = current_si->ii_resources_used[ii][cy];
		current_si->ii_resources_used[ii][cy] = (tmp2 & mask_static_res_in_ext[ext]) | 
		  ((tmp2 >> static_res_count_in_ext[ext]) << ext_base_res);
	      }
	    }
	  }
	}

	// Update Bad IIs:
	//  No action should be required here. However, this case has not
	//  been tested.
	if ((current_si->bad_iis.dw[0] != 0) || (current_si->bad_iis.dw[1] != 0)) {
	  if (ext_info.trace_on) {
	    fprintf(TFile, "Found an instruction with unexpected Bad IIs information "
	                   "during extension loading. Need to check if this information "
	                   "has to be updated here");
	    DevWarn("Found an instruction with unexpected Bad IIs information "
		    "during extension loading. Need to check if this information "
		    "has to be updated here");
          }
	}

	// Each SI description might contain a table of  pointer on resources.
	// Here we will insure that pointers to static resources always point
	// to the same resource object (Initially, they are duplicated in
	// static description and in each extension dll).
	if (current_si->resource_total_vector_size > 0) {
	  SI_RESOURCE * TI_SI_CONST* ext_res_tab;
	  ext_res_tab = ext_proc_si_info[ext]->get_SI_resources_tab();
	  for (int res=0; res<current_si->resource_total_vector_size; res++) {
	    int res_rank=0;
	    while (current_si->resource_total_vector[res].resource != ext_res_tab[res_rank]) {
	      res_rank++;
	    }
	    if (res_rank<static_res_count_in_ext[ext]) {
	      // Dealing with static resource, need to update the pointer
	      current_si->resource_total_vector[res].resource = res_tab[res_rank];
	    }
	  }
	}

	// Each SI description might contain a table of  pointer on issue slots.
	// Here we will insure that pointers to static issue slots always point
	// to the same resource object (Initially, they are duplicated in
	// static description and in each extension dll).
	if (current_si->valid_issue_slot_count > 0) {
	  SI_ISSUE_SLOT * TI_SI_CONST* ext_issue_slots_tab;
	  ext_issue_slots_tab = ext_proc_si_info[ext]->get_SI_issue_slots_tab();
	  for (int islot=0; islot<current_si->valid_issue_slot_count; islot++) {
	    int islot_rank=0;
	    while (current_si->valid_issue_slots[islot] != ext_issue_slots_tab[islot_rank]) {
	      islot_rank++;
	    }
	    if (islot_rank < static_islot_count_in_ext[ext]) {
	      // Dealing with static issue slot, need to update the pointer
	      current_si->valid_issue_slots[islot] = islot_tab[islot_rank];
	    }
	  }
	}
      } // forall si_in_ext
      
      ext_base_si    += si_in_ext;
      ext_base_res   += res_in_ext;
      ext_base_islot += islot_in_ext;
      nb_total_bits  += nb_ext_bits;
    }
  }
  
  // Update targinfo pointers and sizes
  if (new_res_count > static_res_count) {
    Set_SI_resource_count(new_res_count);
    Set_SI_resources(res_tab);
  }
  if (new_islot_count > static_islot_count) {
    Set_SI_issue_slot_count(new_islot_count);
    Set_SI_issue_slots(islot_tab);
  }
  Set_SI_RRW_initializer(new_RRW_initializer);
  Set_SI_RRW_overuse_mask(new_RRW_overuse_mask);
  Set_SI_ID_count(new_si_count);
  Set_SI_ID_si(si_tab);
  Set_SI_top_si(top_si_tab);


  if (ext_info.trace_on) {
    fprintf(TFile, "Summary of updated scheduling information:\n");
    fprintf(TFile, "  -> Resource count        : %8d (static: %d)\n",
	    new_res_count, static_res_count);
    fprintf(TFile, "  -> Issue Slot count      : %8d (static: %d)\n",
	    new_islot_count, static_islot_count);
    fprintf(TFile, "  -> Sched Info (SI) count : %8d (static: %d)\n",
	    new_si_count, static_si_count);
    fprintf(TFile, "  -> SI_RRW_initializer    : 0x%llx\n", new_RRW_initializer);
    fprintf(TFile, "  -> SI_RRW_overuse_mask   : 0x%llx\n", new_RRW_overuse_mask);

    fprintf(TFile, "  -> Resource table = \n");
    for (int k=0; k<new_res_count; k++) {
      fprintf(TFile, "     [%d] %s ", k, SI_RESOURCE_ID_Name(k));
      fprintf(TFile, "avail per cycle: %d ", SI_RESOURCE_Avail_Per_Cycle(Get_SI_resources()[k]));
      fprintf(TFile, "word index: %d ", SI_RESOURCE_Word_Index(Get_SI_resources()[k]));
      fprintf(TFile, "bit index: %d\n", SI_RESOURCE_Bit_Index(Get_SI_resources()[k]));
    }
  }
}


/* ===========================================================================
 * Initialize ISA_VARIANTS tables used by the code generator.
 * Initialized data are:
 * - ISA_VARIANT_infos    (array [TOP_Count + 1])
 * ===========================================================================*/
static void Initialize_ISA_VARIANTS(Lai_Loader_Info_t &ext_info) {
  int  ext, top, var;
  ISA_TOP_VARIANT *variant_list, *variant_ptr;
  ISA_VARIANT_INFO *variants_table;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension TOP Variant tables\n");
  }

  if (TOP_count == TOP_static_count) {
    return; // No top in extension
  }

  // Allocate new tables for all TOPs.
  variants_table= TYPE_MEM_POOL_ALLOC_N(ISA_VARIANT_INFO, Malloc_Mem_Pool, (TOP_count+1));
  // Initialize all top variants (plus TOP_UNDEFINED).
  memset (variants_table, 0, (TOP_count+1)*sizeof(ISA_VARIANT_INFO));
  
  // Count the total number of variants for all tops (static and dynamic variants included).
  int num_variants = 0;
  for (top = 0; top < TOP_static_count; top++) {
    num_variants += ISA_VARIANT_infos[top].n_variants;
  }
  for (ext = 0; ext < ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      const ISA_VARIANT_INFO *ext_variant_tab = ext_info.ISA_tab[ext]->get_ISA_VARIANT_INFO_tab();
      for (top = 0; top < top_in_ext; top++) {
	num_variants +=  ext_variant_tab[top].n_variants;
      }
    }
  }
  
  // Allocate the whole variant list.
  variant_list = TYPE_MEM_POOL_ALLOC_N(ISA_TOP_VARIANT, Malloc_Mem_Pool, num_variants);
  
  // Initialize the variants table to point into the variant list.
  variant_ptr = &variant_list[0];
  for (top = 0; top < TOP_static_count; top++) {
    int n_variants = ISA_VARIANT_infos[top].n_variants;
    variants_table[top].n_variants = n_variants;
    variants_table[top].variants = variant_ptr;
    for (var = 0; var < n_variants; var++) {
      variant_ptr->top = ISA_VARIANT_infos[top].variants[var].top;
      variant_ptr->attributes = ISA_VARIANT_infos[top].variants[var].attributes;
      variant_ptr++;
    }
  }
  for (ext = 0; ext < ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      INT ext_base_TOP = ext_info.base_TOP[ext];
      const ISA_VARIANT_INFO *ext_variant_tab = ext_info.ISA_tab[ext]->get_ISA_VARIANT_INFO_tab();
      // Compute remap table if extension variants and core variants
      //                     use different numbering
      INT    nb_remap;
      INT    ext_attr_count = ext_info.ISA_tab[ext]->get_ISA_VARIANT_attribute_tab_sz();
      const ISA_VARIANT_ATTRIBUTE *core_attr_tab = ISA_VARIANT_get_attribute_table();
      const ISA_VARIANT_ATTRIBUTE *ext_attr_tab  = ext_info.ISA_tab[ext]->get_ISA_VARIANT_attribute_tab();
      COMPUTE_ATTR_REMAP_TABLE ( /* in_EXT_NAME        */ ext_info.dll_tab[ext].dllname,
				 /* in_COMPONENT_NAME  */ "variant",
				 /* in_ATTR_TYPE       */ UINT64,
				 /* in_ATTR_SUFFIX     */ ULL,
				 /* in_ATTR_FORMAT     */ "%llx",
				 /* in_CORE_ATTR_COUNT */ ISA_VARIANT_ATTRIBUTE_COUNT,
				 /* in_CORE_ATTR_TAB   */ core_attr_tab,
				 /* in_EXT_ATTR_COUNT  */ ext_attr_count,
				 /* in_EXT_ATTR_TAB    */ ext_attr_tab,
				 /* out_NB_REMAP       */ nb_remap
				 );
      // Build extension variants entries
      for (top = 0; top < top_in_ext; top++) {
	int n_variants = ext_variant_tab[top].n_variants;
	variants_table[ext_base_TOP+top].n_variants = n_variants;
	variants_table[ext_base_TOP+top].variants = variant_ptr;
	for (var = 0; var < n_variants; var++) {
	  variant_ptr->top = ext_variant_tab[top].variants[var].top + ext_base_TOP; // convert to TOP global numbering
	  REMAP_ATTR ( /* in_ATTR_TYPE   */ UINT64,
		       /* in_ATTR_SUFFIX */ ULL,
		       /* in_ATTR        */ ext_variant_tab[top].variants[var].attributes,
		       /* out_ATTR       */ variant_ptr->attributes
		     );
	  variant_ptr++;
	}
      }
      CLEANUP_ATTR_REMAP_TABLE();
    }
  }

  // Update ISA_VARIANTS_infos table pointer.
  ISA_VARIANT_infos = variants_table;
}

/* ===========================================================================
 * Initialize DW_DEBUG tables used by the code generator for generation of
 * extension dwarf debug information
 * generator. Initialized data are:
 *   EXTENSION_description_tab         (array[EXTENSION_count])
 *   EXTENSION_description_reg_tab     (array[ISA_REGISTER_CLASS_MAX+1])
 * ===========================================================================*/
static void Initialize_DW_DEBUG_info(Lai_Loader_Info_t &ext_info)
{
   int          ext, rc, rc2;
   int          nb_added_rclass;          /* Number of added register classes */
   int          index;
   const char*  extname;
   const char*  reloc_string;

   EXTENSION_DESC     *new_tab;
   EXTENSION_REG_DESC *new_tab_reg;

   if (ext_info.trace_on) {
     fprintf(TFile,"Summary of extension DWARF debug information\n");
   }

   // TODO: see if sanity checks are OK and if we need more.
   if (TOP_count == TOP_static_count) {
    return; // No top in extension
   }

   EXTENSION_count += ext_info.nb_ext;

   new_tab = TYPE_MEM_POOL_ALLOC_N(
               EXTENSION_DESC,
               Malloc_Mem_Pool,
               EXTENSION_count);

   // Copy the static part of the table.
   if(EXTENSION_static_count!=0)
    { memcpy(new_tab,
             EXTENSION_description_tab,
             EXTENSION_static_count*sizeof(EXTENSION_DESC));
    }
   
   // Initialize dynamic part of the table.
   for (ext = 0; ext < ext_info.nb_ext; ext++) {
     extname      = ext_info.ISA_tab[ext]->get_DW_DEBUG_EXT_name();
     reloc_string = ext_info.ISA_tab[ext]->get_DW_DEBUG_EXT_reloc_string();

     index = EXTENSION_static_count + ext;

     // Copy first string
     new_tab[index].extname = TYPE_MEM_POOL_ALLOC_N(
                                char,
                                Malloc_Mem_Pool,
                                strlen(extname)+1);
     strcpy(const_cast<char*>(new_tab[index].extname),extname);

     // Copy second string. This argument can be NULL.
     if(NULL==reloc_string) {
       new_tab[index].reloc_string = (const char*)NULL;
     }
     else {
       new_tab[index].reloc_string = TYPE_MEM_POOL_ALLOC_N(
                                   char,Malloc_Mem_Pool,strlen(reloc_string)+1);
       strcpy(const_cast<char*>(new_tab[index].reloc_string),reloc_string);
    }

    // Activate extension.
    new_tab[index].enabled = 1;
   }

   nb_added_rclass = ISA_REGISTER_CLASS_MAX - ISA_REGISTER_CLASS_STATIC_MAX;

   // No additional register class: return.
   if(0==nb_added_rclass)
     return;

   new_tab_reg = TYPE_MEM_POOL_ALLOC_N(EXTENSION_REG_DESC,
                                       Malloc_Mem_Pool,
                                       ISA_REGISTER_CLASS_MAX+1);
   memcpy(new_tab_reg,
          EXTENSION_reg_description_tab,
          sizeof(EXTENSION_REG_DESC)*(ISA_REGISTER_CLASS_STATIC_MAX+1));

   rc = ISA_REGISTER_CLASS_STATIC_MAX+1;  // Index of first "dyn" reg class.

   for(ext=0;ext<ext_info.nb_ext;ext++) { // Loop on all extensions.

     const EXTENSION_REG_DESC* ext_tab ; // Reg. description table local to
                                         // a given extension.
     int nb_reg_class;                   // nb of register class for the
                                         // considered extension.

     ext_tab = ext_info.ISA_tab[ext]->get_DW_DEBUG_EXT_reg_tab();


     nb_reg_class = ext_info.ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab_sz();
     for(rc2=rc;rc2<rc+nb_reg_class;rc2++) {
        // Set extension internal id in new table.
        new_tab_reg[rc2].ext_id     = EXTENSION_static_count+ext;
 
        // Set register class base dwarf id register.
        new_tab_reg[rc2].dw_base_id = ext_tab->dw_base_id;
      }
     rc+=nb_reg_class;

   }

   // Pointer update.
   EXTENSION_description_tab     = new_tab;
   EXTENSION_reg_description_tab = new_tab_reg;

   if (ext_info.trace_on) {
     int rc;
     FILE *fhan = TFile;

     fprintf(fhan, "Summarizing dwarf register class information\n");
     fprintf(fhan, "Extension list: name - reloc_string\n");
     for(ext=0;ext<(int)EXTENSION_count;++ext) {
       fprintf(fhan,"---> %-15s --- %s\n",
               EXTENSION_description_tab[ext].extname,
               EXTENSION_description_tab[ext].reloc_string);
     } 
     fprintf(fhan, "Register classes: DW id, ext. id, ext. name\n");
     for(rc=0;rc<(int)(ISA_REGISTER_CLASS_MAX+1);rc++)
     { const ISA_REGISTER_CLASS_INFO *cinfo;
       int   internal_id;

       cinfo       = ISA_REGISTER_CLASS_Info((ISA_REGISTER_CLASS)rc);
       internal_id = EXTENSION_reg_description_tab[rc].ext_id;

       fprintf(fhan," ISA_REGISTER_CLASS_%-15s  ==> %3d, %3d, %s\n",
               ISA_REGISTER_CLASS_INFO_Name(cinfo),
               EXTENSION_reg_description_tab[rc].dw_base_id,
               internal_id,
               internal_id==-1?
                 "core" :
                 EXTENSION_description_tab[internal_id].extname);
     }
     fprintf(fhan,"\n");
   }

   return;
}



/* ===========================================================================
 * This function initializes all lai specific information for extension
 * description.
 * This mainly covers the targinfo description for register class,
 * TOP, operands, ...
 * ===========================================================================*/
void Lai_Initialize_Extension_Loader (int nb_ext, const Extension_dll_t *ext_inter_tab) {
  int ext, j;
  Lai_Loader_Info_t *ext_info;
  // Local heap memory
  MEM_POOL lai_loader_pool;
  char err_msg[512];

  if (nb_ext==0) {
    return;
  }

  MEM_POOL_Initialize(&lai_loader_pool, "lai loader temporary memory", FALSE);
  MEM_POOL_Push(&lai_loader_pool);

  const extension_hooks_t **ext_tab;
  ext_tab = TYPE_MEM_POOL_ALLOC_N(const extension_hooks_t*, Malloc_Mem_Pool, nb_ext);
  for (ext=0; ext<nb_ext; ext++) {
    ext_tab[ext] = ext_inter_tab[ext].hooks;
  }

  // Allocate and initialize Lai specific extension table
  ext_info = TYPE_MEM_POOL_ALLOC_N(Lai_Loader_Info_t, Malloc_Mem_Pool, 1);
  global_ext_info_table = ext_info;

  ext_info->nb_ext    = nb_ext;
  ext_info->hooks_tab = ext_tab;
  ext_info->trace_on  = Get_Trace(TP_EXTLOAD, 0xffffffff);
  ext_info->dll_tab   = ext_inter_tab;

  if (ext_info->trace_on) {
    fprintf(TFile,"%s Running Targinfo Extension Loader\n%s", DBar, DBar);
  }

  // Base offset for each element of each extension
  ext_info->ISA_tab                = TYPE_MEM_POOL_ALLOC_N(const ISA_EXT_Interface_t*, Malloc_Mem_Pool, nb_ext);
  ext_info->base_TOP               = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_INTRINSIC         = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_LIT_CLASS         = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_ENUM_CLASS        = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_ENUM_CLASS_VALUE  = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_REGISTER_CLASS    = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_REGISTER_SUBCLASS = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_PREG              = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  
  // Compute cumulated bounds
  // ------------------------
  int nb_top    = TOP_static_count;
  int lc_max    = ISA_LC_STATIC_MAX;
  int nb_enum       = ISA_EC_STATIC_MAX;
  int nb_enum_value = ISA_ECV_STATIC_MAX;
  int rc_max    = ISA_REGISTER_CLASS_STATIC_MAX;
  int subrc_max = ISA_REGISTER_SUBCLASS_STATIC_MAX;
  int preg_max = Get_Static_Last_Dedicated_Preg_Offset();
  for (ext=0; ext < nb_ext; ext++) {
    get_isa_extension_instance_t get_instance;
    get_instance = (get_isa_extension_instance_t)Get_dll_Symbol(&ext_inter_tab[ext], "get_isa_extension_instance");
    if (get_instance == NULL) {
      sprintf(err_msg, "Unable to find symbol get_isa_extension_instance (%s)", dlerror());
      RaiseErrorIncompatibleLibrary(ext_inter_tab[ext].dllname, err_msg);
    }
    ext_info->ISA_tab[ext] = (*get_instance)();
    
    /* Coherency check - magic number */
    if (MAGIC_NUMBER_EXT_ISA_API != ext_info->ISA_tab[ext]->magic) {
      char err_msg[256];
      sprintf(err_msg,
	      "Incompatible magic number (lib:0x%08x, API:0x%08x).",
	      ext_info->ISA_tab[ext]->magic, MAGIC_NUMBER_EXT_ISA_API);
      RaiseErrorIncompatibleLibrary(ext_inter_tab[ext].dllname, err_msg);
    }

    ext_info->base_TOP[ext] = nb_top;
    nb_top += ext_info->ISA_tab[ext]->get_TOP_count();

    // Note: this rely on the order used in loader.cxx to load intrinsics.
    ext_info->base_INTRINSIC[ext] = ext_inter_tab[ext].base_intrinsics;

    ext_info->base_LIT_CLASS[ext] = lc_max+1;
    lc_max += ext_info->ISA_tab[ext]->get_ISA_LIT_CLASS_info_tab_sz() - (ext_info->ISA_tab[ext]->get_ISA_LIT_CLASS_static_max()+1);
    
    ext_info->base_ENUM_CLASS[ext] = nb_enum;
    nb_enum += ext_info->ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_tab_sz();

    ext_info->base_ENUM_CLASS_VALUE[ext] = nb_enum_value;
    nb_enum_value += ext_info->ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz();
	
    ext_info->base_REGISTER_CLASS[ext] = rc_max+1;
    int rc_in_ext    = ext_info->ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab_sz();
    const ISA_REGISTER_CLASS_INFO *rc_tab = ext_info->ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab();
    rc_max  += rc_in_ext;
    
    //TB: Compute nb of registers to add to the core
    ext_info->base_PREG[ext] = preg_max+1;
    for (int regcl = 0; 
	 regcl < rc_in_ext; 
	 regcl++) {
      int first_isa_reg  = rc_tab[regcl].min_regnum;
      int last_isa_reg   = rc_tab[regcl].max_regnum;
      int register_count = last_isa_reg - first_isa_reg + 1;
      preg_max += register_count;
    }

    ext_info->base_REGISTER_SUBCLASS[ext] = subrc_max+1;
    subrc_max += ext_info->ISA_tab[ext]->get_ISA_REGISTER_SUBCLASS_tab_sz();
  }

  // Sanity checks
  FmtAssert((lc_max <= ISA_LC_MAX_LIMIT),
			("Too many literal classes defined by extensions"));
  FmtAssert((rc_max <= ISA_REGISTER_CLASS_MAX_LIMIT),
			("Too many register classes defined by extensions"));
  FmtAssert((subrc_max <= ISA_REGISTER_SUBCLASS_MAX_LIMIT),
			("Too many register subclasses defined by extensions"));

  // Update targinfo maximum bounds
  TOP_count     = nb_top;
  TOP_UNDEFINED = TOP_count;
  ISA_LC_MAX    = lc_max;
  ISA_EC_MAX    = nb_enum;
  ISA_ECV_MAX   = nb_enum_value;
  ISA_REGISTER_CLASS_MAX    = rc_max;
  ISA_REGISTER_SUBCLASS_MAX = subrc_max;
  ISA_REGISTER_CLASS_COUNT  = rc_max - ISA_REGISTER_CLASS_MIN + 1;
  //TB: Reinitialize register class to preg offset
  if (preg_max != Get_Static_Last_Dedicated_Preg_Offset()) {
    Set_Last_Dedicated_Preg_Offset(preg_max);
    Reset_RegisterClass_To_Preg();
  }

  // ==========================================================================
  //
  // Initialize the various targinfo part. Note that the order is relevant!!
  //
  // ==========================================================================

  Initialize_ISA_RegisterClasses(*ext_info);

  Initialize_ABI_Properties(*ext_info);

  Initialize_ISA_Top(*ext_info);

  Initialize_ISA_Print(*ext_info);

  Initialize_ISA_Bundle(*ext_info);

  Initialize_ISA_Literals(*ext_info);

  Initialize_ISA_Enum(*ext_info);

  Initialize_ISA_Operands(*ext_info, lai_loader_pool);

  Initialize_ISA_Subsets(*ext_info);

  Initialize_ISA_Hazards(*ext_info);

  Initialize_SI(*ext_info, lai_loader_pool);

  Initialize_ISA_VARIANTS(*ext_info);

  Initialize_DW_DEBUG_info(*ext_info);

  // ==========================================================================
  // 
  // The following table are not standard targinfo information but are required
  // to support user-defined extensions
  // 
  // ==========================================================================

  // Initialize INTRINSIC to OP table
  // --------------------------------
  if (ext_info->trace_on) {
	fprintf(TFile, "...Initialize INTRINSIC to OP table\n");
  }
  if (INTRINSIC_COUNT > INTRINSIC_STATIC_COUNT) {
    const extension_builtins_t* intrn_tab;
    extension_intrinsic_info_t* intrn_isa_info;
    extension_INTRINSIC_table = TYPE_MEM_POOL_ALLOC_N(extension_intrinsic_info_t,
						      Malloc_Mem_Pool,
						      ((INTRINSIC_COUNT-INTRINSIC_STATIC_COUNT) + 1));

    for (ext=0; ext<nb_ext; ext++) {
      int intrn_in_ext = ext_tab[ext]->get_builtins_count();
      if (intrn_in_ext > 0) {
	if (ext_info->trace_on) {
	  fprintf(TFile, "  Extension %d: First intrinsic %d\n", ext, ext_info->base_INTRINSIC[ext]);
	}
	intrn_isa_info = &extension_INTRINSIC_table[ext_info->base_INTRINSIC[ext]-(INTRINSIC_STATIC_COUNT+1)];
	intrn_tab = ext_tab[ext]->get_builtins();
	for (j=0; j<intrn_in_ext; j++) {
	  intrn_isa_info->type = intrn_tab[j].type;
	  if (intrn_isa_info->type == DYN_INTRN_TOP) {
	    intrn_isa_info->u1.top = (TOP)(ext_info->base_TOP[ext] + intrn_tab[j].u1.local_TOP_id);
	  }
	  else {
	    intrn_isa_info->u1.compose_extract_idx = intrn_tab[j].u1.compose_extract_idx;
	  }
	  intrn_isa_info++;
	}
      }
    }
  }


  // Initialize MTYPE from/to REGISTER CLASS table
  // ---------------------------------------------
  if (ext_info->trace_on) {
	fprintf(TFile, "...Initialize MTYPE from/to Register class table\n");
  }
  if (NB_PURE_DYNAMIC_MTYPES > 0) {
    extension_REGISTER_CLASS_to_MTYPE_table = TYPE_MEM_POOL_ALLOC_N(rc_to_mtype_info_t, Malloc_Mem_Pool, ((ISA_REGISTER_CLASS_MAX-ISA_REGISTER_CLASS_STATIC_MAX) + 1));
    extension_MTYPE_to_REGISTER_CLASS_table = TYPE_MEM_POOL_ALLOC_N(ISA_REGISTER_CLASS, Malloc_Mem_Pool, NB_PURE_DYNAMIC_MTYPES);
    extension_MTYPE_to_REGISTER_SUBCLASS_table = TYPE_MEM_POOL_ALLOC_N(ISA_REGISTER_SUBCLASS, Malloc_Mem_Pool, NB_PURE_DYNAMIC_MTYPES);

    for (j=0; j<(ISA_REGISTER_CLASS_MAX-ISA_REGISTER_CLASS_STATIC_MAX) + 1; j++) {
      extension_REGISTER_CLASS_to_MTYPE_table[j].typeCount = 0;
    }

    for (ext=0; ext<nb_ext; ext++) {
      int count = ext_tab[ext]->get_modes_count();
      const extension_machine_types_t* modes = ext_tab[ext]->get_modes();
      for (j=0; j<count; j++) {
	int     rc_id = ext_info->base_REGISTER_CLASS[ext] + modes[j].local_REGISTER_CLASS_id;
	int     rsc_id = (modes[j].local_REGISTER_SUBCLASS_id != -1 ) ? ext_info->base_REGISTER_SUBCLASS[ext] + modes[j].local_REGISTER_SUBCLASS_id : ISA_REGISTER_SUBCLASS_UNDEFINED;
	TYPE_ID mtype = Get_Extension_MTYPE_From_Name(modes[j].name);
	// ...regclass from mtype
	extension_MTYPE_to_REGISTER_CLASS_table[mtype - (MTYPE_STATIC_LAST + 1)] = rc_id;
	extension_MTYPE_to_REGISTER_SUBCLASS_table[mtype - (MTYPE_STATIC_LAST + 1)] = rsc_id;
	// ...mtype from regclass
	rc_to_mtype_info_t *info;
	info = &extension_REGISTER_CLASS_to_MTYPE_table[rc_id - (ISA_REGISTER_CLASS_STATIC_MAX+1)];
	if (info->typeCount == 0) {
	  info->typeTab = TYPE_MEM_POOL_ALLOC_N(TYPE_ID, Malloc_Mem_Pool, count/*slightly overestimated*/);
	}
	info->typeTab[info->typeCount] = mtype;
	info->typeCount++;
      }
    }
  }


  if (ext_info->trace_on) {
    fprintf(TFile," Leaving Targinfo Extension Loader\n%s", SBar);
  }
  
  // Cleanup
  MEM_POOL_Pop(&lai_loader_pool);
  MEM_POOL_Delete(&lai_loader_pool);
}


/*
 * TODO: This function is yet to be implemented and called. 
 *  Do we really need it?
 */
void Lai_Cleanup_Extension_Loader() {
}

/**
 * @see lai_loader_api.h
 */
INT EXTENSION_get_REGISTER_SUBCLASS_bit_size(ISA_REGISTER_SUBCLASS subclass)
{
    const ISA_REGISTER_SUBCLASS_INFO * subclassInfo =
        ISA_REGISTER_SUBCLASS_Info(subclass);
    ISA_REGISTER_CLASS rclass = ISA_REGISTER_SUBCLASS_INFO_Class(subclassInfo);
    const ISA_REGISTER_CLASS_INFO* rcInfo = ISA_REGISTER_CLASS_Info(rclass);
    INT size = ISA_REGISTER_CLASS_INFO_Bit_Size(rcInfo);

    if(rclass > ISA_REGISTER_CLASS_STATIC_MAX)
        {
            INT globalSize = size * (ISA_REGISTER_CLASS_INFO_Last_Reg(rcInfo) -
                                     ISA_REGISTER_CLASS_INFO_First_Reg(rcInfo)
                                     + 1);
            size = globalSize / ISA_REGISTER_SUBCLASS_INFO_Count(subclassInfo);
        }
    return size;
}

/**
 * @see lai_loader_api.h
 */
INT EXTENSION_get_REGISTER_CLASS_max_bit_size(ISA_REGISTER_CLASS rc)
{
    const ISA_REGISTER_CLASS_INFO* rcInfo = ISA_REGISTER_CLASS_Info(rc);
    INT size = ISA_REGISTER_CLASS_INFO_Bit_Size(rcInfo);

    if(rc > ISA_REGISTER_CLASS_STATIC_MAX)
        {
            INT globalSize = size * (ISA_REGISTER_CLASS_INFO_Last_Reg(rcInfo) -
                                     ISA_REGISTER_CLASS_INFO_First_Reg(rcInfo)
                                     + 1);
            ISA_REGISTER_SUBCLASS subclass;
            FOR_ALL_ISA_REGISTER_SUBCLASS(subclass)
            {
                const ISA_REGISTER_SUBCLASS_INFO * subclassInfo =
                    ISA_REGISTER_SUBCLASS_Info(subclass);
                if(rc == ISA_REGISTER_SUBCLASS_INFO_Class(subclassInfo))
                    {
                        INT tmpSize = globalSize /
                            ISA_REGISTER_SUBCLASS_INFO_Count(subclassInfo);
                        size = tmpSize > size? tmpSize: size;
                    }
            }
        }
    return size;
}

#endif  /* defined (BACK_END) */

