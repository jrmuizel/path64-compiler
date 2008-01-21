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

/* Initialization functions */
void Lai_Initialize_Extension_Loader(int nb_ext, const struct Extension_dll *ext_tab);
void Lai_Cleanup_Extension_Loader();

/* Access functions to extension specific information */
BE_EXPORTED extern const EXTENSION_Regclass_Info *EXTENSION_get_REGISTER_CLASS_info(ISA_REGISTER_CLASS rc);

BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_MTYPE_to_REGISTER_CLASS(TYPE_ID mtype);

//TB: association mtype register subclass
BE_EXPORTED extern ISA_REGISTER_SUBCLASS EXTENSION_MTYPE_to_REGISTER_SUBCLASS(TYPE_ID mtype);

BE_EXPORTED extern TYPE_ID EXTENSION_REGISTER_CLASS_to_MTYPE(ISA_REGISTER_CLASS rc, INT size);

//TB map between preg and register in the extension
BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_PREG_to_REGISTER_CLASS(PREG_NUM preg);

//TB map between preg and register number in the extension register file
BE_EXPORTED extern int EXTENSION_PREG_to_REGISTER_NUM(PREG_NUM preg);

/*
 * TB: Return a the base PREG number for a register class
 */
BE_EXPORTED extern INT EXTENSION_get_Min_Offset(ISA_REGISTER_CLASS rc);

/*
 * Return TRUE if the parameter intrinsic Id belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_INTRINSIC(INTRINSIC id) {
  return (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT);
}

/*
 * Return TRUE if the parameter register class rc belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_REGISTER_CLASS(ISA_REGISTER_CLASS rc) {
  return (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX);
}

BE_EXPORTED extern BOOL EXTENSION_Is_Extension_REGISTER_CLASS(ISA_REGISTER_CLASS rc);

BE_EXPORTED extern BOOL EXTENSION_Is_Extension_INTRINSIC(INTRINSIC id);

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

BE_EXPORTED extern int  EXTENSION_Get_ComposeExtract_Index(INTRINSIC id);

BE_EXPORTED extern TOP EXTENSION_TOP_AM_automod_variant(TOP top,
				     BOOL post_mod,
				     BOOL inc_mod,
				     ISA_REGISTER_CLASS regclass);

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

