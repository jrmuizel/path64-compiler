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
/* This file is in charge of providing extension specific feature */
/*
 * It contains the data-structure used by the extension dlls to 
 * describe the low-level part of user-defined extensions
 */
#ifndef DYN_ISA_API_H
#define DYN_ISA_API_H

#ifdef TARG_ST

/* 
 * In order to avoid any mangling problem, we have defined a pure C interface.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

  //#ifdef BACK_END
#include "topcode.h"                     /* For definition of basic types.   */
#include "targ_proc.h"
#include "targ_isa_properties.h"
#include "targ_isa_registers.h"
#include "targ_isa_print.h"
#include "targ_isa_bundle.h"
#include "targ_isa_lits.h"
#include "targ_isa_operands.h"
#include "targ_abi_properties.h"
#include "targ_isa_enums.h"
#include "targ_isa_bundle.h"
#include "targ_isa_hazards.h"
#include "targ_isa_decode.h"
#include "targ_isa_binutils.h"
#include "targ_isa_relocs.h"
#include "ti_si.h"
#include "targ_isa_variants.h"
#include "air.h"
#include "parser.h"
  //#endif

#ifndef DLL_EXPORTED
#define DLL_EXPORTED 
#endif


#define MAGIC_NUMBER_EXT_ISA_API   20100426  /* Magic number. Interface checking */

/* ===========================================
 * Base register available for addressing mode
 * =========================================== */
typedef enum {
  AM_BASE_DEFAULT,  /* base is a Standard register */
  AM_BASE_GP,       /* base is the Global Pointer  */
  AM_BASE_SP,       /* base is the Stack Pointer   */
} AM_Base_Reg_Type;


/* ====================================
 * Extension register class description
 * ==================================== */
struct extension_regclass
{
  // Memory accesses
  TOP (*get_load_TOP) (INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr, INT mpixel_size);
  TOP (*get_store_TOP)(INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr, INT mpixel_size);
 
  // Move within same extension
  TOP (*get_move_X2X_TOP)               (INT size);
 
  // Move from/to core with a 8, 16 or 32bits extension reg
  // (size must be 1, 2 or 4)
  TOP (*get_move_R2X_TOP)               (INT size);
  TOP (*get_move_X2R_TOP)               (INT size);

  // Compose extension reg value with 32bits data chunks
  TOP (*get_insert_and_zeroext_R2X_TOP) (INT size);
  TOP (*get_rotate_and_insert_R2X_TOP)  (INT size);

  // Compose extension reg value with 2x32bits data chunks
  TOP (*get_insert_and_zeroext_RR2X_TOP)(INT size);
  TOP (*get_rotate_and_insert_RR2X_TOP) (INT size);
 
  // Extract extension reg value by 32bits data chunks
  TOP (*get_extract_X2R_TOP)            (INT size);
  TOP (*get_extract_and_rotate_X2R_TOP) (INT size);

  // Clear top
  TOP (*get_clear_TOP)                  (INT size);

  // Zero/Sign extend to direct parent register level
  // (X size is specified, P size is half X size)
  TOP (*get_zeroext_P2X_TOP)            (INT xsize);
  TOP (*get_signext_P2X_TOP)            (INT xsize);

  // Zero/Sign extend starting at a given 32bits chunk
  TOP (*get_zeroext_X_TOP)              (INT size);
  TOP (*get_signext_X_TOP)              (INT size);
  
  // Virtual Compose/Extract/Widemove
  TOP (*get_simulated_compose_TOP) (INT from_size, INT to_size);
  TOP (*get_simulated_extract_TOP) (INT from_size, INT to_size);
  TOP (*get_simulated_widemove_TOP)(INT size); 

};

typedef struct extension_regclass extension_regclass_t;


/* ================================================
 * Scheduling information for a single architecture
 * ================================================ */
struct extension_si
{
  // Name of the processor it is associated to
  const char *                 (*get_processor_name)(void);

  // Resources
  INT                          (*get_SI_resource_count)(void);
  SI_RESOURCE * TI_SI_CONST*   (*get_SI_resources_tab) (void);

  // Resource access masks
  TI_SI_CONST SI_RRW           (*get_SI_RRW_initializer) (void);
  TI_SI_CONST SI_RRW           (*get_SI_RRW_overuse_mask)(void);

  // Issue slots
  INT                          (*get_SI_issue_slot_count)(void);
  SI_ISSUE_SLOT * TI_SI_CONST* (*get_SI_issue_slots_tab) (void);

  // SI table
  SI ** TI_SI_CONST            (*get_SI_ID_si_tab)   (void);
  mUINT32                      (*get_SI_ID_si_tab_sz)(void);

  // Extension TOP to SI table
  SI ** TI_SI_CONST            (*get_SI_top_si_tab)(void);
};

typedef struct extension_si extension_si_t;


/* ========================================
 * Complete ISA description of an extension
 * ======================================== */
struct ISA_EXT_Interface {

 /* Interface identification ----------------------------------------------- */
 mUINT32    magic;                       /* Magic number                     */

 /* Handling dynamic TOPs -------------------------------------------------- */
 const mUINT32(*get_TOP_count)    (void);/* Return the number of dynamic TOPs*/ 
 const char** (*get_TOP_name_tab) (void);/* Return TOP name table            */
 int          (*belong_TOP_to_ext)(TOP) ;/* Whether a TOP belongs to ext.    */
 TOP          (*get_TOP_local)    (TOP) ;/* Return local TOP value.          */
 void         (*set_TOP_base_offset)(INT);/*Set extension base TOP offset    */
 TOP          (*get_TOP_AM_automod_variant)(TOP,BOOL,BOOL,ISA_REGISTER_CLASS);
                                         /* Return TOP automod variant       */
 void         (*set_TOP_UNDEFINED) (TOP);/* Set TOP_UNDEFINED value.         */

 /* Handling TOP properties ------------------------------------------------ */
 const EXTENSION_ISA_PROPERTY_FLAGS* (*get_TOP_prop_tab)                (void);
 const mUINT16*                  (*get_TOP_mem_bytes_tab)               (void);
 const mUINT16*                  (*get_TOP_mem_align_tab)               (void);
 /* ...attribute table used for compatibility check/remapping                */
 const EXTENSION_ISA_PROPERTIES_ATTRIBUTE* (*get_ISA_PROPERTIES_attribute_tab)(void);
 const mUINT32                   (*get_ISA_PROPERTIES_attribute_tab_sz) (void);

 /* Handling assembly printing --------------------------------------------- */
 const ISA_PRINT_INFO*           (*get_ISA_PRINT_info_tab)              (void);
 const mUINT32                   (*get_ISA_PRINT_info_tab_sz)           (void);
 const mUINT16*                  (*get_ISA_PRINT_info_index_tab)        (void);
 const char**                    (*get_ISA_PRINT_name_tab)              (void);

 /* Handling literal classes ----------------------------------------------- */
 const ISA_LIT_CLASS_INFO*       (*get_ISA_LIT_CLASS_info_tab)          (void);
 const mUINT32                   (*get_ISA_LIT_CLASS_info_tab_sz)       (void);
 const mUINT32                   (*get_ISA_LIT_CLASS_static_max)        (void);

 /* Handling operands ------------------------------------------------------ */
 const ISA_OPERAND_VALTYP*       (*get_ISA_OPERAND_operand_types_tab)   (void);
 const mUINT32                   (*get_ISA_OPERAND_operand_types_tab_sz)(void);
 const ISA_OPERAND_INFO*         (*get_ISA_OPERAND_info_tab)            (void);
 const mUINT32                   (*get_ISA_OPERAND_info_tab_sz)         (void);
 const mUINT16*                  (*get_ISA_OPERAND_info_index_tab)      (void);
 const mINT8*                    (*DEPRECATED_FUNC1)(void); /*NOTE: no more used, but kept to simplify backward compatibility*/
  /* ...tables used for compatibility check/remapping                         */
 const ISA_OPERAND_USE_ATTRIBUTE*(*get_ISA_OPERAND_USE_attribute_tab)   (void);
 const mUINT32                   (*get_ISA_OPERAND_USE_attribute_tab_sz)(void);
 /* ...access to core reg class & subclass names used in ext                 */
 const char * const *         (*get_ISA_OPERAND_static_rclass_tab)      (void);
 const mUINT32                (*get_ISA_OPERAND_static_rclass_tab_sz)   (void);
 const char * const *         (*get_ISA_OPERAND_static_rsubclass_tab)   (void);
 const mUINT32                (*get_ISA_OPERAND_static_rsubclass_tab_sz)(void);

 /* Handling ISA subset ---------------------------------------------------- */
 const char* const*              (*get_ISA_SUBSET_tab)                  (void);
 const mUINT32                   (*get_ISA_SUBSET_tab_sz)               (void);
 const unsigned char**           (*get_ISA_SUBSET_op_tab)               (void);

 /* Handling register classes ---------------------------------------------- */
 const ISA_REGISTER_CLASS_INFO*  (*get_ISA_REGISTER_CLASS_tab)          (void);
 const mUINT32                   (*get_ISA_REGISTER_CLASS_tab_sz)       (void);
 const mUINT8*                   (*get_ISA_REGISTER_CLASS_index_tab)    (void);
 const ISA_REGISTER_SUBCLASS_INFO*(*get_ISA_REGISTER_SUBCLASS_tab)      (void);
 const mUINT32                   (*get_ISA_REGISTER_SUBCLASS_tab_sz)    (void);
 /* ...association btwn reg classes and injected TOPs                        */
 const extension_regclass_t*     (*get_REGISTER_CLASS_info_tab)         (void);

 /* Handling ABI properties ------------------------------------------------ */
 const ABI_PROPERTIES*           (*get_ABI_PROPERTIES_tab)              (void);
 /* ...attribute table used for compatibility check/remapping                */
 const ABI_PROPERTIES_ATTRIBUTE* (*get_ABI_PROPERTIES_attribute_tab)    (void);
 const mUINT32                   (*get_ABI_PROPERTIES_attribute_tab_sz) (void);

 /* Handling extension information ----------------------------------------- */
 const char*                     (*get_DW_DEBUG_EXT_name)               (void);
 const char*                     (*get_DW_DEBUG_EXT_reloc_string)       (void);
 const EXTENSION_REG_DESC*       (*get_DW_DEBUG_EXT_reg_tab)            (void);

 /* Handling scheduling information ---------------------------------------- */
 const extension_si_t*           (*get_scheduling_info_tab)             (void);
 const mUINT32                   (*get_scheduling_info_tab_sz)          (void);

 /* Handling enumerations -------------------------------------------------- */
 const ISA_ENUM_CLASS_INFO*      (*get_ISA_ENUM_CLASS_INFO_tab)         (void);
 const mUINT32                   (*get_ISA_ENUM_CLASS_INFO_tab_sz)      (void);
 const ISA_ENUM_CLASS_VALUE_INFO*(*get_ISA_ENUM_CLASS_INFO_VALUE_tab)   (void);
 const mUINT32                   (*get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz)(void);

 /* Handling bundling information ------------------------------------------ */
 const ISA_EXEC_UNIT_PROPERTY*   (*get_ISA_EXEC_unit_prop_tab)          (void);

 /* Handling TOP variants -------------------------------------------------- */
 const ISA_VARIANT_INFO*         (*get_ISA_VARIANT_INFO_tab)            (void);
 /* ...attribute table used for compatibility check/remapping                */
 const ISA_VARIANT_ATTRIBUTE*    (*get_ISA_VARIANT_attribute_tab)       (void);
 const mUINT32                   (*get_ISA_VARIANT_attribute_tab_sz)    (void);

 /* Handling hazards -------------------------------------------------------- */
 const mUINT8*                   (*get_ISA_HAZARDS_index_tab)           (void);

 /* Handling bundling information: Added here for backward compatibility issue*/
 const ISA_EXEC_UNIT_SLOTS*      (*get_ISA_EXEC_unit_slots_tab)         (void);
 const mUINT8*                   (*get_ISA_BUNDLE_slot_count_tab)       (void);

 /* Handling parser structure: Added for GBU binutils */
 const PARSER_GetParserT*        (*get_ISA_PARSE_tab)                   (void);
 const mUINT32                   (*get_ISA_PARSE_tab_sz)                (void);

 /* Handling pack information: Added for GBU binutils ---------------------- */
 const ISA_PACK_OPND_INFO*       (*get_ISA_PACK_OPND_info_tab)          (void);
 const mUINT32                   (*get_ISA_PACK_OPND_info_tab_sz)       (void);
 const mUINT16*                  (*get_ISA_PACK_OPND_info_index_tab)    (void);
 const ISA_PACK_INFO*            (*get_ISA_PACK_info_tab)               (void);
 const mUINT8*                   (*get_ISA_PACK_inst_words_tab)         (void);
 const ISA_PACK_ADJ_INFO*        (*get_ISA_PACK_adj_info_tab)           (void);
 const mUINT32                   (*get_ISA_PACK_adj_info_tab_sz)        (void);
 const mUINT8*                   (*get_ISA_PACK_adj_info_index_tab)     (void);

 /* Handling decoding information: Added for GBU binutils ------------------ */
 const ISA_DECODE_DECODING**     (*get_ISA_DECODE_decoding_tab)         (void);
 const mUINT32                   (*get_ISA_DECODE_decoding_tab_sz)      (void);

 /* Handling ISA relocs ---------------------------------------------------- */
       ISA_RELOC_INFO*           (*get_ISA_RELOC_info_tab)              (void);
 const mUINT32                   (*get_ISA_RELOC_info_tab_sz)           (void);
       ISA_RELOC_SUBSET_INFO*    (*get_ISA_RELOC_SUBSET_info_tab)       (void);
 const mUINT32                   (*get_ISA_RELOC_SUBSET_info_tab_sz)    (void);
 const ISA_SUBSET                (*get_ISA_RELOC_max_static_virtual_id_core_subset)           (void);
       void                      (*set_ISA_RELOC_dynamic_reloc_offset)  (mUINT32 offset);

 /* Handling binutils information: Added for GBU binutils ------------------ */
       ISA_BINUTILS_INFO*        (*get_ISA_BINUTILS_info_tab)           (void);
       mUINT32                   (*get_ISA_BINUTILS_info_tab_sz)        (void);

 /* Handling register classes - Added in REV_20090416 ---------------------- */
       void                      (*set_ISA_REGISTER_CLASS_offset)       (mUINT32); /* Set extension register class offset    */
       void                      (*set_ISA_REGISTER_SUBCLASS_offset)    (mUINT32); /* Set extension register subclass offset */

 /* Handling literal classes - Added in REV_20090416 ----------------------- */
       void                      (*set_ISA_LIT_CLASS_offset)            (mUINT32); /* Set extension literal class offset     */

 /* Handling Relocation variants - Added in REV_20100120 ------------------- */
       ISA_RELOC_VARIANT_INFO*   (*get_ISA_RELOC_variant_info_tab)      (void);
 const mUINT32                   (*get_ISA_RELOC_variant_info_tab_sz)   (void);
};

typedef struct ISA_EXT_Interface ISA_EXT_Interface_t;

/* Function that returns an instance for the dll*/
typedef const ISA_EXT_Interface_t *(*get_isa_extension_instance_t)(void);
DLL_EXPORTED extern const ISA_EXT_Interface_t   *get_isa_extension_instance();


#ifdef __cplusplus
}                /* extern "C" */
#endif

#endif /* TARG_ST */

#endif /* DYN_ISA_API_H */

