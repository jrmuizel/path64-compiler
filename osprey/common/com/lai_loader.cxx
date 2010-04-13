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
#include "extension_include.h"
#include "loader.h"
#include "erglob.h"
#include "errors.h"
#include "config.h"
#include "tracing.h"
#include "intrn_info.h"
#include <stdlib.h>

#ifdef BACK_END
#include "dyn_isa_api_access.h"
#undef inline
#include "symtab_idx.h"
#include "targ_sim.h"
#include "isa_loader.h"
#include "lai_loader_api.h"
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
  char* asm_stmt_behavior;
  char** asm_stmt_results;
  char** asm_stmt_operands;
  int    asm_stmt_cycles;
  int    asm_stmt_size;
  long long flags;
} extension_intrinsic_info_t;

static extension_intrinsic_info_t *extension_INTRINSIC_table = (extension_intrinsic_info_t*)NULL;

// Keep access to extension info table
static Lai_Loader_Info_t *global_ext_info_table;

/*
 * Return the base PREG number for a register class
 */
INT EXTENSION_get_Min_Offset(ISA_REGISTER_CLASS rc) {
  FmtAssert((extension_RegClass_To_Preg_Min_Offset_table!=NULL), ("Unexpected NULL extension_RegClass_To_Preg_Min_Offset_table"));
  if (EXTENSION_Is_Extension_REGISTER_CLASS(rc)) {
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
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    while (ext < (global_ext_info_table->nb_ext-1) && 
	   id >= global_ext_info_table->base_INTRINSIC[ext+1]) {
      ext++;
    }
    return (global_ext_info_table->handler_tab[ext].extname);
  }

  return (NULL);
}

/*
 * Return the name of the extension containing the specified TOP id,
 * or NULL if not found
 */
const char *EXTENSION_Get_Extension_Name_From_TOP(TOP id) {
  int ext=0;
  if (EXTENSION_Is_Extension_TOP(id)) {
    while (ext < (global_ext_info_table->nb_ext-1) && 
	   id >= global_ext_info_table->base_TOP[ext+1]) {
      ext++;
    }
    return (global_ext_info_table->handler_tab[ext].extname);
  }

  return (NULL);
}

/*
 * Return the name of the extension containing the specified REGISTER CLASS rc,
 * or NULL if not found
 */
const char *EXTENSION_Get_Extension_Name_From_REGISTER_CLASS(ISA_REGISTER_CLASS rc) {
  int ext=0;
  if (EXTENSION_Is_Extension_REGISTER_CLASS(rc)) {
    while (ext < (global_ext_info_table->nb_ext-1) && 
	   rc >= global_ext_info_table->base_REGISTER_CLASS[ext+1]) {
      ext++;
    }
    return (global_ext_info_table->handler_tab[ext].extname);
  }

  return (NULL);
}

/*
 * Return the TOP corresponding to the INTRINSIC identifier given in parameter
 * Return TOP_UNDEFINED if the intrinsic id is invalid
 */
TOP EXTENSION_INTRINSIC_to_TOP(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id) &&
      (is_DYN_INTRN_TOP(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]))
      ) {
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
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_TOP(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
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
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_COMPOSE(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
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
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_PARTIAL_COMPOSE(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
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
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_PARTIAL_EXTRACT(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
  }
  return (false);
}

/*
 * Return true if the INTRINSIC identified by 'id' is a
 * convert_to_pixel INTRINSIC, 
 * meaning that it is a dummy intrinsic for the compiler that should
 * be replaced by a simple copy.
 * false otherwise.
 */
bool
EXTENSION_Is_Convert_To_Pixel_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_CONVERT_TO_PIXEL(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
  }
  return false;
}

/*
 * Return true if the INTRINSIC identified by 'id' is a
 * convert_from_pixel INTRINSIC, 
 * meaning that it is a dummy intrinsic for the compiler that should
 * be replaced by a simple copy.
 * false otherwise.
 */
bool
EXTENSION_Is_Convert_From_Pixel_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_CONVERT_FROM_PIXEL(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
  }
  return false;
}

/*
 * Return true if the INTRINSIC identified by 'id' is a
 * convert_to_ctype INTRINSIC, 
 * meaning that it is a dummy intrinsic for the compiler that should
 * be replaced by moves to core register.
 * false otherwise.
 */
bool
EXTENSION_Is_Convert_To_CType_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_CONVERT_TO_CTYPE(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
  }
  return false;
}

/*
 * Return true if the INTRINSIC identified by 'id' is a
 * convert_from_ctype INTRINSIC, 
 * meaning that it is a dummy intrinsic for the compiler that should
 * be replaced by moves from core registers to extension regisers.
 * false otherwise.
 */
bool
EXTENSION_Is_Convert_From_CType_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_CONVERT_FROM_CTYPE(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
  }
  return false;
}

/** 
 * Return true if the INTRINSIC identified by 'id' is a meta  INTRINSIC,
 * meaning that it is defined as an asm-stmt.
 * false otherwise.
 * 
 * @param id : intrinsic id.  
 * 
 * @return 
 */
bool EXTENSION_Is_Meta_INTRINSIC(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    return (is_DYN_INTRN_WHIRLNODE_META(extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)]));
  }
  return (false);
}

/** 
 * return the asm cost (in cycles) corresponding to the meta intrinsic.
 * 
 * @param id : meta intrinsic id.  
 * 
 * @return 
 */
int EXTENSION_Get_Meta_INTRINSIC_Asm_Cycles(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    int itrnidx = id - (INTRINSIC_STATIC_COUNT+1);
    if (is_DYN_INTRN_WHIRLNODE_META(extension_INTRINSIC_table[itrnidx])) {
      return extension_INTRINSIC_table[itrnidx].asm_stmt_cycles;
    }
  }
  return -1;
}

/** 
 * return the asm cost (in size) corresponding to the meta intrinsic.
 * 
 * @param id : meta intrinsic id.  
 * 
 * @return 
 */
int EXTENSION_Get_Meta_INTRINSIC_Asm_Size(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    int itrnidx = id - (INTRINSIC_STATIC_COUNT+1);
    if (is_DYN_INTRN_WHIRLNODE_META(extension_INTRINSIC_table[itrnidx])) {
      return extension_INTRINSIC_table[itrnidx].asm_stmt_size;
    }
  }
  return -1;
}

/** 
 * return the require options flags corresponding to the intrinsic.
 * 
 * @param id : extension intrinsic id.  
 * 
 * @return 
 */
long long EXTENSION_Get_INTRINSIC_Flags(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    int itrnidx = id - (INTRINSIC_STATIC_COUNT+1);
    return extension_INTRINSIC_table[itrnidx].flags;
  }
  return -1;
}

/** 
 * return the asm behavior corresponding to the meta intrinsic.
 * 
 * @param id : meta intrinsic id.  
 * 
 * @return 
 */
char* EXTENSION_Get_Meta_INTRINSIC_Asm_Behavior(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    int itrnidx = id - (INTRINSIC_STATIC_COUNT+1);
    if (is_DYN_INTRN_WHIRLNODE_META(extension_INTRINSIC_table[itrnidx])) {
      return extension_INTRINSIC_table[itrnidx].asm_stmt_behavior;
    }
  }
  return NULL;
}

/** 
 * return an array of results (+temporaries) used in the asm_behavior
 * for meta intrinsic.
 *
 * elements are described using the asm-stmt syntax. 
 * array is finished by a NULL element.
 *
 * @param id : meta intrinsic id.  
 * 
 * @return 
 */
char** EXTENSION_Get_Meta_INTRINSIC_Asm_Results(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    int itrnidx = id - (INTRINSIC_STATIC_COUNT+1);
    if (is_DYN_INTRN_WHIRLNODE_META(extension_INTRINSIC_table[itrnidx])) {
      return extension_INTRINSIC_table[itrnidx].asm_stmt_results;
    }
  }
  return NULL;
}

/** 
 * return an array of operands used in the asm_behavior
 * for meta intrinsic.
 *
 * elements are described using the asm-stmt syntax. 
 * array is finished by a NULL element.
 * 
 * @param id : meta intrinsic id.  
 * 
 * @return 
 */
char** EXTENSION_Get_Meta_INTRINSIC_Asm_Operands(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    int itrnidx = id - (INTRINSIC_STATIC_COUNT+1);
    if (is_DYN_INTRN_WHIRLNODE_META(extension_INTRINSIC_table[itrnidx])) {
    return extension_INTRINSIC_table[itrnidx].asm_stmt_operands;  
    }
  }
  return NULL;
}


/*
 * Return the index associated with current INTRINSIC, which must be either a 
 * partial compose or a partial extract. This index is used to know which part
 * of the SIMD data should beextracted/updated.
 */
int EXTENSION_Get_ComposeExtract_Index(INTRINSIC id) {
  FmtAssert((extension_INTRINSIC_table!=NULL), ("Unexpected NULL extension_INTRINSIC_table"));
  if (EXTENSION_Is_Extension_INTRINSIC(id)) {
    extension_intrinsic_info_t *info;
    info = &extension_INTRINSIC_table[id - (INTRINSIC_STATIC_COUNT+1)];
    
    FmtAssert((is_DYN_INTRN_PARTIAL_EXTRACT(*info) ||
               is_DYN_INTRN_PARTIAL_COMPOSE(*info)),
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
  if (EXTENSION_Is_Extension_REGISTER_CLASS(rc)) {
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
TYPE_ID Get_Extension_MTYPE_From_Name(const char *name) {
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
  const EXTENSION_Regclass_Info *rc_info;
  if (!EXTENSION_Is_Extension_REGISTER_CLASS(rc)) {
    return size; // Non-extension register class
  }

  rc_info  = EXTENSION_get_REGISTER_CLASS_info(rc);
  if (rc_info == NULL) { // Register class info not defined for SFR classes
    return size;
  }
  min_size = ISA_REGISTER_CLASS_INFO_Bit_Size(ISA_REGISTER_CLASS_Info(rc)) / 8;

  while (size >= min_size &&
	 (TOP_UNDEFINED == rc_info->get_load_TOP(/* reg size  */size,
						 /* base_type*/AM_BASE_DEFAULT,
						 /* offs_is_imm */true,
						 /* offs_is_incr*/true, 0)) &&
	 (TOP_UNDEFINED == rc_info->get_store_TOP(/* reg size  */size,
						  /* base_type*/AM_BASE_DEFAULT,
						  /* offs_is_imm  */true,
						  /* offs_is_incr */true, 0))) {
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
 * This function initializes all lai specific information for extension
 * description.
 * This mainly covers the targinfo description for register class,
 * TOP, operands, ...
 * ===========================================================================*/
bool Lai_Initialize_Extension_Loader(const int ext_inter_count, const Extension_dll_t *ext_inter_tab) {
  int ext, j;
  Lai_Loader_Info_t *ext_info;
  // Local heap memory
  MEM_POOL lai_loader_pool;
  char err_msg[512];

  const int nb_ext = Get_Extension_dll_handler_count( );
  FmtAssert( (nb_ext==ext_inter_count), ("Inconsistent count of Extension_dll_t VS count of Extension_dll_handle_t"));
  if (nb_ext==0) {
    return TRUE;
  }

  if (!Load_Lai_Loader_Info()) {
    return FALSE;
  }

  MEM_POOL_Initialize(&lai_loader_pool, "lai loader temporary memory", FALSE);
  MEM_POOL_Push(&lai_loader_pool);

  ext_info = Get_Lai_Loader_Info_Table();
  global_ext_info_table = ext_info;

  if (ext_info->trace_on) {
    fprintf(TFile,"%s Running Targinfo Extension Loader\n%s", DBar, DBar);
  }

  // ==========================================================================
  // 
  // The following table are not standard targinfo information but are required
  // to support user-defined extensions
  // 
  // ==========================================================================

  // Initialize INTRINSIC to OP table
  // --------------------------------
  for (ext=0; ext < nb_ext; ext++) {

    // Note: this rely on the order used in loader.cxx to load intrinsics.
    ext_info->base_INTRINSIC[ext] = ext_inter_tab[ext].base_intrinsics;

    if (ext_info->trace_on) {
      fprintf(TFile, "   -> First TOP id               (if any) : %d\n", ext_info->base_TOP[ext]);
      fprintf(TFile, "   -> First INTRINSIC id         (if any) : %d\n", ext_info->base_INTRINSIC[ext]);
      fprintf(TFile, "   -> First Literal class id     (if any) : %d\n", ext_info->base_LIT_CLASS[ext]);
      fprintf(TFile, "   -> First ENUM class id        (if any) : %d\n", ext_info->base_ENUM_CLASS[ext]);
      fprintf(TFile, "   -> First Register class id    (if any) : %d\n", ext_info->base_REGISTER_CLASS[ext]);
      fprintf(TFile, "   -> First Register subclass id (if any) : %d\n", ext_info->base_REGISTER_SUBCLASS[ext]);
      fprintf(TFile, "   -> First PREG id              (if any) : %d\n", ext_info->base_PREG[ext]);
    }
  }

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
      int intrn_in_ext = ext_inter_tab[ext].hooks->get_builtins_count();
      if (intrn_in_ext > 0) {
	if (ext_info->trace_on) {
	  fprintf(TFile, "  Extension %d: First intrinsic %d\n", ext, ext_info->base_INTRINSIC[ext]);
	}
	intrn_isa_info = &extension_INTRINSIC_table[ext_info->base_INTRINSIC[ext]-(INTRINSIC_STATIC_COUNT+1)];
	intrn_tab = ext_inter_tab[ext].hooks->get_builtins();
	for (j=0; j<intrn_in_ext; j++) {
	  intrn_isa_info->type = intrn_tab[j].type;

          intrn_isa_info->asm_stmt_behavior = NULL;
          intrn_isa_info->asm_stmt_results = NULL;
          intrn_isa_info->asm_stmt_operands = NULL;
          intrn_isa_info->asm_stmt_cycles = -1;
          intrn_isa_info->asm_stmt_size = -1;
          intrn_isa_info->flags = EXTOPT_none;

          if (is_DYN_INTRN_TOP(*intrn_isa_info)) {
	    intrn_isa_info->u1.top = (TOP)(ext_info->base_TOP[ext] + intrn_tab[j].u1.local_TOP_id);
	  } else if (is_DYN_INTRN_WHIRLNODE_META(*intrn_isa_info)) {
            FmtAssert((intrn_tab[j].wn_table!=NULL),
                      ("intrn_tab[j].wn_table must not be NULL for DYN_INTRN_WHIRLNODE_META"));
            intrn_isa_info->asm_stmt_behavior =
              intrn_tab[j].wn_table->asm_stmt_behavior;
            intrn_isa_info->asm_stmt_results =
              intrn_tab[j].wn_table->asm_stmt_results;
            intrn_isa_info->asm_stmt_operands =
              intrn_tab[j].wn_table->asm_stmt_operands;
            intrn_isa_info->asm_stmt_cycles = 
              intrn_tab[j].wn_table->cycles;
            intrn_isa_info->asm_stmt_size = 
              intrn_tab[j].wn_table->size;
            intrn_isa_info->flags =
              intrn_tab[j].wn_table->flags;
          } else {
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
      int count = ext_inter_tab[ext].hooks->get_modes_count();
      const extension_machine_types_t* modes = ext_inter_tab[ext].hooks->get_modes();
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

  return TRUE;
}


/*
 * TODO: This function is yet to be implemented and called. 
 *  Do we really need it?
 */
void Lai_Cleanup_Extension_Loader() {
}

#endif  /* defined (BACK_END) */

