/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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

#include "dyn_isa_api_access.h"
#include "ext_info.h"

#include "isa_loader_api.h"

/* Initialization and cleanup functions */
bool Lai_Initialize_Extension_Loader(const int ext_inter_count, const struct Extension_dll *ext_inter_tab);
void Lai_Cleanup_Extension_Loader();

/* Return the register class associated to <mtype> parameter */
BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_MTYPE_to_REGISTER_CLASS(TYPE_ID mtype);

/* Return the register subclass associated to <mtype> parameter */
BE_EXPORTED extern ISA_REGISTER_SUBCLASS EXTENSION_MTYPE_to_REGISTER_SUBCLASS(TYPE_ID mtype);

/* Return the MTYPE associated to registers of register class <rc> with size <size> */
BE_EXPORTED extern TYPE_ID EXTENSION_REGISTER_CLASS_to_MTYPE(ISA_REGISTER_CLASS rc, INT size);

/* Return the register number of <preg> in its extension register class */
BE_EXPORTED extern int EXTENSION_PREG_to_REGISTER_NUM(PREG_NUM preg);

/*
 * Return the base PREG number for a register class
 */
BE_EXPORTED extern INT EXTENSION_get_Min_Offset(ISA_REGISTER_CLASS rc);

BE_EXPORTED extern const char *EXTENSION_Get_Extension_Name_From_INTRINSIC(INTRINSIC id);

BE_EXPORTED extern const char *EXTENSION_Get_Extension_Name_From_TOP(TOP id);

BE_EXPORTED extern const char *EXTENSION_Get_Extension_Name_From_REGISTER_CLASS(ISA_REGISTER_CLASS rc);

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

BE_EXPORTED extern long long EXTENSION_Get_INTRINSIC_Flags(INTRINSIC id);

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

