/* 

  Copyright (C) 2006,2007 ST Microelectronics, Inc.  All Rights Reserved. 

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

//TB: put internal data type def common to the gcc loader and the lai
//loader


#ifndef LOADER_INTERNAL_H
#define LOADER_INTERNAL_H
#include "dyn_isa_api.h"

// ==================================================================
// This structure contains information shared by the various targinfo 
// initializer functions
// ==================================================================
struct Lai_Loader_Info {
  BOOL trace_on; // debug trace on?
  int  nb_ext;
  const extension_hooks_t   **hooks_tab;
  const ISA_EXT_Interface_t **ISA_tab;
  const Extension_dll_t      *dll_tab;

  // Base offsets for each element of each extension
  // (only those that need to be global are here)
  int  *base_TOP;
  int  *base_INTRINSIC;
  int  *base_LIT_CLASS;
  int  *base_ENUM_CLASS;
  int  *base_ENUM_CLASS_VALUE;
  int  *base_REGISTER_CLASS;
  int  *base_REGISTER_SUBCLASS;
  //TB: add support to extend PREG
  int  *base_PREG;
};

typedef struct Lai_Loader_Info Lai_Loader_Info_t;

// ===================
// Register class data
// ===================
extern const extension_regclass_t **extension_regclass_table;
//TB: tables to save PREG-> reg class association
//extern ISA_REGISTER_CLASS *extension_Preg_To_RegClass_table;
//TB: tables to save PREG-> reg number association
extern int *extension_Preg_To_RegNum_table;
//TB: rclAss tO preg min offset
extern INT *extension_RegClass_To_Preg_Min_Offset_table;
/*
 * Function called when an extension is incompatible with the core description
 */
extern void RaiseErrorIncompatibleLibrary(const char *name, const char *error_msg);

/* ===========================================================================
 * The following set of macros is used to factorize attribute remapping
 * algorithm used by several ISA targinfo components (ISA variants, ISA
 * properties, ISA operand uses and ABI properties).
 *
 * For all these components, each attribute is encoded as a power of two,
 * allowing the representation of a set of attributes by a simple bitfield.
 *
 * Note that macros are used instead of functions to support several
 * attribute encoding type (UINT64, int)
 * ===========================================================================*/
/* 
 * Compute remap table from extension to core. Result is store in implicitly
 * defined tables.
 * Parameters:
 *   in_EXT_NAME        : extension name
 *   in_COMPONENT_NAME  : targinfo component
 *   in_ATTR_TYPE       : type used of attribute
 *   in_ATTR_SUFFIX     : suffix for constant of current type attribute
 *   in_ATTR_FORMAT     : print format for current attribute type 
 *   in_CORE_ATTR_COUNT : number of core attributes
 *   in_CORE_ATTR_TAB   : table for core attributes
 *   in_EXT_ATTR_COUNT  : number of extension attributes
 *   in_EXT_ATTR_TAB    : table for extension attributes
 *   out_NB_REMAP       : number of remapped attributes
 */
#define COMPUTE_ATTR_REMAP_TABLE(in_EXT_NAME, in_COMPONENT_NAME,	\
				 in_ATTR_TYPE, in_ATTR_SUFFIX, in_ATTR_FORMAT, \
				 in_CORE_ATTR_COUNT, in_CORE_ATTR_TAB,	\
				 in_EXT_ATTR_COUNT, in_EXT_ATTR_TAB,	\
				 out_NB_REMAP				\
				 )					\
  /* 3 following variables are defined here and reused */		\
  /*  in REMAP_ATTR() and CLEANUP_ATTR_REMAP_TABLE()   */		\
  int          macro_nb_remap = 0;					\
  in_ATTR_TYPE macro_modified_attr = 0##in_ATTR_SUFFIX;			\
  struct { in_ATTR_TYPE old_attr; in_ATTR_TYPE new_attr; }		\
    macro_remap_tab[64];						\
  {									\
    int attr_rank, core_attr_rank;					\
    if (in_CORE_ATTR_COUNT < in_EXT_ATTR_COUNT) {			\
      RaiseErrorIncompatibleLibrary(in_EXT_NAME,			\
"Incompatible target description, too many TOP variants in extension"); \
    }									\
    for (attr_rank = 0; attr_rank < in_EXT_ATTR_COUNT; attr_rank++) {	\
      if ((in_EXT_ATTR_TAB[attr_rank].attribute == 0)			\
	  || ((in_EXT_ATTR_TAB[attr_rank].attribute ==			\
	       in_CORE_ATTR_TAB[attr_rank].attribute)			\
	      && (strcmp(in_EXT_ATTR_TAB[attr_rank].name,		\
			 in_CORE_ATTR_TAB[attr_rank].name)==0))) {	\
	continue; /* same attribute id, or unused in ext */		\
      }									\
      /* Search remap info for current attribute */			\
      for (core_attr_rank = 0; core_attr_rank < in_CORE_ATTR_COUNT; core_attr_rank++) { \
	if ((core_attr_rank!=attr_rank) &&				\
	    (strcmp(in_EXT_ATTR_TAB[attr_rank].name,			\
		    in_CORE_ATTR_TAB[core_attr_rank].name)==0)) {	\
	  if (in_EXT_ATTR_TAB[attr_rank].attribute !=			\
	      in_CORE_ATTR_TAB[core_attr_rank].attribute) {		\
	    macro_modified_attr |= in_EXT_ATTR_TAB[attr_rank].attribute; \
	    macro_remap_tab[macro_nb_remap].old_attr = in_EXT_ATTR_TAB[attr_rank].attribute; \
	    macro_remap_tab[macro_nb_remap].new_attr = in_CORE_ATTR_TAB[core_attr_rank].attribute; \
	    if (ext_info.trace_on) {					\
	      fprintf(TFile, "   Extension %s: Remap %s '%s' from id 0x"in_ATTR_FORMAT" to id 0x"in_ATTR_FORMAT"\n", \
		      in_EXT_NAME,					\
		      in_COMPONENT_NAME,				\
		      in_EXT_ATTR_TAB[attr_rank].name,			\
		      macro_remap_tab[macro_nb_remap].old_attr, macro_remap_tab[macro_nb_remap].new_attr); \
	    }								\
	    macro_nb_remap++;						\
	  }								\
	  break;							\
	}								\
      }									\
      if (core_attr_rank == in_CORE_ATTR_COUNT) {			\
	char err_msg[256];						\
	sprintf(err_msg,						\
"Incompatible target description, TOP variants '%s' not found in core", \
	 in_EXT_ATTR_TAB[attr_rank].name);				\
        RaiseErrorIncompatibleLibrary(in_EXT_NAME, err_msg);		\
      }									\
    }									\
    out_NB_REMAP = macro_nb_remap;					\
  }

/* 
 * Remap input attribute into ouput attribute using the previously computed
 * remap table.
 * Parameters:
 *   in_ATTR_TYPE   : type used of attribute
 *   in_ATTR_SUFFIX : suffix for constant of current type attribute
 *   in_ATTR        : input attributes
 *   out_ATTR       : output attributes
 */
#define REMAP_ATTR(in_ATTR_TYPE, in_ATTR_SUFFIX,			\
		   in_ATTR,						\
		   out_ATTR						\
		   )							\
  if ((macro_nb_remap > 0) && ((in_ATTR) & (macro_modified_attr)) != 0) { \
    /* Remap current attribute field */					\
    int attr_rank;							\
    in_ATTR_TYPE orig_attr = in_ATTR;					\
    in_ATTR_TYPE new_attr  = 0##in_ATTR_SUFFIX;				\
    for (attr_rank = 0; attr_rank < macro_nb_remap; attr_rank++) {	\
      if (orig_attr & macro_remap_tab[attr_rank].old_attr) {		\
	orig_attr &= ~(macro_remap_tab[attr_rank].old_attr);		\
	new_attr  |=   macro_remap_tab[attr_rank].new_attr;		\
      }									\
    }									\
    out_ATTR = (orig_attr | new_attr);					\
  }									\
  else {								\
    out_ATTR = in_ATTR;							\
  }

/* 
 * Cleanup remap table previously computed.
 * Currently empty
 */
#define CLEANUP_ATTR_REMAP_TABLE()		\
  
#endif /* LOADER_INTERNAL_H */
