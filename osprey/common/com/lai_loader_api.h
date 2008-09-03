/*

Copyright (C) 2008 ST Microelectronics, Inc.  All Rights Reserved.

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

Contact information:  

*/
/* This file is in charge of providing extension specific feature */
/*
 * It contains extension initialization function for the Lai part of the compiler,
 * as well as interface functions used to access some extension specific information, 
 * nedded at Lai level
 */
#ifndef LAI_LOADER_API_H
#define LAI_LOADER_API_H

#ifdef TARG_ST

#ifdef BACK_END

#include "../gccfe/extension_include.h"
#include "dyn_isa_api_access.h"
#include "ext_info.h"

/* Initialization and cleanup functions */
void Lai_Initialize_Extension_Loader(int nb_ext, const struct Extension_dll *ext_tab);
void Lai_Cleanup_Extension_Loader();

/* Return a pointer to the description of the extension register class <rc> */
BE_EXPORTED extern const EXTENSION_Regclass_Info *EXTENSION_get_REGISTER_CLASS_info(ISA_REGISTER_CLASS rc);

/* Return the register class associated to <mtype> parameter */
BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_MTYPE_to_REGISTER_CLASS(TYPE_ID mtype);

/* Return the register subclass associated to <mtype> parameter */
BE_EXPORTED extern ISA_REGISTER_SUBCLASS EXTENSION_MTYPE_to_REGISTER_SUBCLASS(TYPE_ID mtype);

/* Return the MTYPE associated to registers of register class <rc> with size <size> */
BE_EXPORTED extern TYPE_ID EXTENSION_REGISTER_CLASS_to_MTYPE(ISA_REGISTER_CLASS rc, INT size);

/* Return the register class associated to <preg> */
BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_PREG_to_REGISTER_CLASS(PREG_NUM preg);

/* Return the register number of <preg> in its extension register class */
BE_EXPORTED extern int EXTENSION_PREG_to_REGISTER_NUM(PREG_NUM preg);

/*
 * Return the base PREG number for a register class
 */
BE_EXPORTED extern INT EXTENSION_get_Min_Offset(ISA_REGISTER_CLASS rc);

/*
 * Return TRUE if the parameter intrinsic Id belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_INTRINSIC(INTRINSIC id) {
  return (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT);
}

/*
 * Return TRUE if the parameter TOP Id belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_TOP(TOP id) {
  return (id >= TOP_static_count && id < TOP_count);
}

/*
 * Return TRUE if the parameter register class rc belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_REGISTER_CLASS(ISA_REGISTER_CLASS rc) {
  return (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX);
}

BE_EXPORTED extern const char *EXTENSION_Get_Extension_Name_From_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern const char *EXTENSION_Get_Extension_Name_From_TOP(TOP id);

BE_EXPORTED extern const char *EXTENSION_Get_Extension_Name_From_REGISTER_CLASS(ISA_REGISTER_CLASS rc);

BE_EXPORTED extern EXTENSION_ISA_Info *EXTENSION_Get_ISA_Info_From_TOP(TOP id);

BE_EXPORTED extern TOP  EXTENSION_INTRINSIC_to_TOP(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_TOP_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Compose_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Partial_Compose_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Partial_Extract_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Convert_To_Pixel_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Convert_From_Pixel_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Convert_To_CType_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Convert_From_CType_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern bool EXTENSION_Is_Meta_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern char* EXTENSION_Get_Meta_INTRINSIC_Asm_Behavior(INTRINSIC id);
BE_EXPORTED extern char** EXTENSION_Get_Meta_INTRINSIC_Asm_Results(INTRINSIC id);
BE_EXPORTED extern char** EXTENSION_Get_Meta_INTRINSIC_Asm_Operands(INTRINSIC id);
BE_EXPORTED extern int EXTENSION_Get_Meta_INTRINSIC_Asm_Cycles(INTRINSIC id);

BE_EXPORTED extern int EXTENSION_Get_Meta_INTRINSIC_Asm_Size(INTRINSIC id);

BE_EXPORTED extern int  EXTENSION_Get_ComposeExtract_Index(INTRINSIC id);

BE_EXPORTED extern TOP EXTENSION_TOP_AM_automod_variant(TOP top,
				     BOOL post_mod,
				     BOOL inc_mod,
				     ISA_REGISTER_CLASS regclass);


BE_EXPORTED extern TYPE_ID Get_Extension_MTYPE_From_Name(const char *name);

BE_EXPORTED extern bool EXTENSION_Is_Extension_Present(const char* name);
/* BE_EXPORTED extern int EXTENSION_get_Extension_Idx(const char* name); */

BE_EXPORTED extern INTRINSIC EXTENSION_INTRINSIC_From_Name(const char * name);


/**
 * Get the bit size of a register of the given sub class.
 *
 * @param  subclass 
 *
 * @pre    subclass is a valid subclass index
 * @post   true
 *
 * @return The number of bit of a register of given subclass
 */
BE_EXPORTED extern INT EXTENSION_get_REGISTER_SUBCLASS_bit_size(ISA_REGISTER_SUBCLASS subclass);

/**
 * Get the maximum bit size for given register class.
 * The maximum bit size corresponds to maximum bit size of the register
 * class and all its subclasses. For core registers, sub classes are not
 * checked
 *
 * @param  rc A register class
 *
 * @pre    rc is a valid register class index
 * @post   true
 *
 * @return The maximum bit size for given register class
 */
BE_EXPORTED extern INT EXTENSION_get_REGISTER_CLASS_max_bit_size(ISA_REGISTER_CLASS rc);

/**
 * Return the smallest alignment that will allow the generation of optimal
 * load and store sequences (minimum number of instructions) for the
 * specified register type (class + size).
 *
 * @param  rc is a register class
 * @param  size is the width of the register type (in bytes)
 *
 * @return The optimal alignment (in bytes)
 */
BE_EXPORTED extern INT EXTENSION_Get_REGISTER_CLASS_Optimal_Alignment(ISA_REGISTER_CLASS rc, INT size);

#endif /* BACK_END */

#endif /* TARG_ST */

#endif /* LAI_LOADER_API_H */

