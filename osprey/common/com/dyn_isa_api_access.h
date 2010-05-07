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
 * This modules defines the common API used to access the ISA description of extensions.
 * It is compatible with the successives library API revisions and insure the migration
 * to the latest one, hiding the implemention details to the other parts of thecompiler.
 */
#ifndef DYN_ISA_API_ACCESS_H
#define DYN_ISA_API_ACCESS_H

#include "dyn_isa_api.h"
#ifdef inline
#undef inline // Might be redefined by "dyn_isa_api.h"->"topcode.h"
#endif

// Return TRUE if the specified revision is compatible with current compiler,
// FALSE otherwise.
BOOL EXTENSION_Is_Supported_ISA_Revision(INT rev);

// ========================================================================
// List of compatible API revisions for ISA part of the library description
// ========================================================================
#define    NB_SUPPORTED_ISA_REV 13
#define    EXT_ISA_API_20070126        (20070126)
#define    EXT_ISA_API_20070615        (20070615)
#define    EXT_ISA_API_20070924        (20070924)
#define    EXT_ISA_API_20080307        (20080307)
#define    EXT_ISA_API_20081010        (20081010)
#define    EXT_ISA_API_20090126        (20090126)
#define    EXT_ISA_API_20090408        (20090408)
#define    EXT_ISA_API_20090416        (20090416)
#define    EXT_ISA_API_20090727        (20090727)
#define    EXT_ISA_API_20090915        (20090915)
#define    EXT_ISA_API_20100114        (20100114)
#define    EXT_ISA_API_20100120        (20100120)
#define    EXT_ISA_API_20100426        (20100426)


// #############################################################################
// ##
// ## Class: EXTENSION_Regclass_Info
// ## Descr: Define API used to access some register class specific information
// ##        of the extension
// ##
// #############################################################################
class EXTENSION_Regclass_Info {
 public:
  // Constructor/destructor
  EXTENSION_Regclass_Info();
  EXTENSION_Regclass_Info(const extension_regclass_t* input_rc_info, INT rev_number);
  ~EXTENSION_Regclass_Info();

  // Copy constructor
  EXTENSION_Regclass_Info(const EXTENSION_Regclass_Info &);

  // Copy operator
  EXTENSION_Regclass_Info&  operator=(const EXTENSION_Regclass_Info &rc_access);

  // Memory accesses
  TOP get_load_TOP (INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr, INT mpixel_size) const;
  TOP get_store_TOP(INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr, INT mpixel_size) const;
 
  // Move within same extension
  inline TOP get_move_X2X_TOP               (INT size) const;
 
  // Move from/to core with a 8, 16 or 32bits extension reg
  // (size must be 1, 2 or 4)
  inline TOP get_move_R2X_TOP               (INT size) const;
  inline TOP get_move_X2R_TOP               (INT size) const;

  // Compose extension reg value with 32bits data chunks
  inline TOP get_insert_and_zeroext_R2X_TOP (INT size) const;
  inline TOP get_rotate_and_insert_R2X_TOP  (INT size) const;

  // Compose extension reg value with 2x32bits data chunks
  inline TOP get_insert_and_zeroext_RR2X_TOP(INT size) const;
  inline TOP get_rotate_and_insert_RR2X_TOP (INT size) const;
 
  // Extract extension reg value by 32bits data chunks
  inline TOP get_extract_X2R_TOP            (INT size) const;
  inline TOP get_extract_and_rotate_X2R_TOP (INT size) const;

  // Clear top
  inline TOP get_clear_TOP                  (INT size) const;

  // Zero/Sign extend to direct parent register level
  // (X size is specified, P size is half X size)
  inline TOP get_zeroext_P2X_TOP            (INT xsize) const;
  inline TOP get_signext_P2X_TOP            (INT xsize) const;

  // Zero/Sign extend starting at a given 32bits chunk
  inline TOP get_zeroext_X_TOP              (INT size) const;
  inline TOP get_signext_X_TOP              (INT size) const;
  
  // Virtual Compose/Extract/Widemove
  inline TOP get_simulated_compose_TOP (INT from_size, INT to_size) const;
  inline TOP get_simulated_extract_TOP (INT from_size, INT to_size) const;
  inline TOP get_simulated_widemove_TOP(INT size) const;

 private:
  extension_regclass_t       *own_rc_info;
  const extension_regclass_t *rc_info;
  int revision_number;
};


// #############################################################################
// ##
// ## Class: EXTENSION_SI_Info
// ## Descr: Define API used to access Scheduling Information (SI) for a single
// ##        a single architecture of the extension
// ##
// #############################################################################
class EXTENSION_SI_Info {
 public:
  // Constructor/destructor
  EXTENSION_SI_Info();  
  EXTENSION_SI_Info(const extension_si_t *input_si_info);
  ~EXTENSION_SI_Info();

  // Copy constructor
  EXTENSION_SI_Info(const EXTENSION_SI_Info &);

  // Copy operator
  EXTENSION_SI_Info&  operator= (const EXTENSION_SI_Info &si_access) {
    si_info = si_access.si_info;
    return (*this);
  }

  // Name of the processor it is associated to
  inline const char *                 get_processor_name(void) const;

  // Resources
  inline INT                          get_SI_resource_count(void) const;
  inline SI_RESOURCE * TI_SI_CONST*   get_SI_resources_tab (void) const;

  // Resource access masks
  inline TI_SI_CONST SI_RRW           get_SI_RRW_initializer (void) const;
  inline TI_SI_CONST SI_RRW           get_SI_RRW_overuse_mask(void) const;

  // Issue slots
  inline INT                          get_SI_issue_slot_count(void) const;
  inline SI_ISSUE_SLOT * TI_SI_CONST* get_SI_issue_slots_tab (void) const;

  // SI table
  inline SI ** TI_SI_CONST            get_SI_ID_si_tab   (void) const;
  inline mUINT32                      get_SI_ID_si_tab_sz(void) const;

  // Extension TOP to SI table
  inline SI ** TI_SI_CONST            get_SI_top_si_tab(void) const;

 private:
  extension_si_t       *own_si_info;
  const extension_si_t *si_info;
};


// #############################################################################
// ##
// ## Class: EXTENSION_ISA_Info
// ## Descr: Define API used to access ISA description for given extension
// ##
// #############################################################################
class EXTENSION_ISA_Info {
 public:
  EXTENSION_ISA_Info(const ISA_EXT_Interface_t* new_isa_ext);
  ~EXTENSION_ISA_Info();

  /* Interface identification ----------------------------------------------- */
  inline mUINT32       get_initial_revision(void) const;
  
  /* Handling dynamic TOPs -------------------------------------------------- */
  inline const mUINT32 get_TOP_count    (void)  const;
  inline const char**  get_TOP_name_tab (void)  const;
  inline int           belong_TOP_to_ext(TOP)   const;
  inline TOP           get_TOP_local    (TOP)   const;
  inline void          set_TOP_base_offset(INT);
  inline TOP           get_TOP_AM_automod_variant(TOP,BOOL,BOOL,ISA_REGISTER_CLASS) const;
  inline void          set_TOP_UNDEFINED (TOP);
  
  /* Handling TOP properties ------------------------------------------------ */
  inline const EXTENSION_ISA_PROPERTY_FLAGS* get_TOP_prop_tab                (void) const;
  inline const mUINT16*                  get_TOP_mem_bytes_tab               (void) const;
  inline const mUINT16*                  get_TOP_mem_align_tab               (void) const;
  /* ...attribute table used for compatibility check/remapping                */
  inline const EXTENSION_ISA_PROPERTIES_ATTRIBUTE*
                                         get_ISA_PROPERTIES_attribute_tab    (void) const;
  inline const mUINT32                   get_ISA_PROPERTIES_attribute_tab_sz (void) const;

  /* Handling assembly printing --------------------------------------------- */
  inline const ISA_PRINT_INFO*           get_ISA_PRINT_info_tab              (void) const;
  inline const mUINT32                   get_ISA_PRINT_info_tab_sz           (void) const;
  inline const mUINT16*                  get_ISA_PRINT_info_index_tab        (void) const;
  inline const char**                    get_ISA_PRINT_name_tab              (void) const;

  inline const void*                     get_ISA_PRINT_info_pre_20080307_tab (void) const;
  
  /* Handling literal classes ----------------------------------------------- */
  inline const ISA_LIT_CLASS_INFO*       get_ISA_LIT_CLASS_info_tab          (void) const;
  inline const mUINT32                   get_ISA_LIT_CLASS_info_tab_sz       (void) const;
  inline const mUINT32                   get_ISA_LIT_CLASS_static_max        (void) const;
  // Following is new from EXT_ISA_API_20090416
               void                      set_ISA_LIT_CLASS_offset            (mUINT32 offs) const;
  
  /* Handling operands ------------------------------------------------------ */
  inline const ISA_OPERAND_VALTYP*       get_ISA_OPERAND_operand_types_tab   (void) const;
  inline const mUINT32                   get_ISA_OPERAND_operand_types_tab_sz(void) const;
  inline const ISA_OPERAND_INFO*         get_ISA_OPERAND_info_tab            (void) const;
  inline const mUINT32                   get_ISA_OPERAND_info_tab_sz         (void) const;
  inline const mUINT16*                  get_ISA_OPERAND_info_index_tab      (void) const;
  /* ...tables used for compatibility check/remapping                         */
  inline const ISA_OPERAND_USE_ATTRIBUTE*get_ISA_OPERAND_USE_attribute_tab   (void) const;
  inline const mUINT32                   get_ISA_OPERAND_USE_attribute_tab_sz(void) const;
  /* ...access to core reg class & subclass names used in ext                 */
  inline const char * const *         get_ISA_OPERAND_static_rclass_tab      (void) const;
  inline const mUINT32                get_ISA_OPERAND_static_rclass_tab_sz   (void) const;
  inline const char * const *         get_ISA_OPERAND_static_rsubclass_tab   (void) const;
  inline const mUINT32                get_ISA_OPERAND_static_rsubclass_tab_sz(void) const;
  
  /* Handling ISA subset ---------------------------------------------------- */
  inline const char* const*              get_ISA_SUBSET_tab                  (void) const;
  inline const mUINT32                   get_ISA_SUBSET_tab_sz               (void) const;
  inline const unsigned char**           get_ISA_SUBSET_op_tab               (void) const;
  
  /* Handling register classes ---------------------------------------------- */
  inline const ISA_REGISTER_CLASS_INFO*  get_ISA_REGISTER_CLASS_tab          (void) const;
  inline const mUINT32                   get_ISA_REGISTER_CLASS_tab_sz       (void) const;
  inline const mUINT8*                   get_ISA_REGISTER_CLASS_index_tab    (void) const;
  inline const ISA_REGISTER_SUBCLASS_INFO*get_ISA_REGISTER_SUBCLASS_tab      (void) const;
  inline const mUINT32                   get_ISA_REGISTER_SUBCLASS_tab_sz    (void) const;
  /* ...association btwn reg classes and injected TOPs                        */
 private:
  // Overridden function, now private
  inline const extension_regclass_t*     get_REGISTER_CLASS_info_tab         (void) const;
 public:  
  inline EXTENSION_Regclass_Info        *get_Regclass_Info_tab               (void) const;
  // Two following are new from EXT_ISA_API_20090416
               void                      set_ISA_REGISTER_CLASS_offset       (mUINT32 offs) const;
               void                      set_ISA_REGISTER_SUBCLASS_offset    (mUINT32 offs) const;

  /* Handling ABI properties ------------------------------------------------ */
  inline const ABI_PROPERTIES*           get_ABI_PROPERTIES_tab              (void) const;
  /* ...attribute table used for compatibility check/remapping                */
  inline const ABI_PROPERTIES_ATTRIBUTE* get_ABI_PROPERTIES_attribute_tab    (void) const;
  inline const mUINT32                   get_ABI_PROPERTIES_attribute_tab_sz (void) const;
  
  /* Handling extension information ----------------------------------------- */
  inline const char*                     get_DW_DEBUG_EXT_name               (void) const;
  inline const char*                     get_DW_DEBUG_EXT_reloc_string       (void) const;
  inline const EXTENSION_REG_DESC*       get_DW_DEBUG_EXT_reg_tab            (void) const;
  
  /* Handling scheduling information ---------------------------------------- */
 private:
  inline const extension_si_t*           get_scheduling_info_tab             (void) const;
 public:
  inline EXTENSION_SI_Info*              get_Scheduling_Info_Access_tab      (void) const;
  inline const mUINT32                   get_scheduling_info_tab_sz          (void) const;
  
  /* Handling enumerations -------------------------------------------------- */
  inline const ISA_ENUM_CLASS_INFO*      get_ISA_ENUM_CLASS_INFO_tab         (void) const;
  inline const mUINT32                   get_ISA_ENUM_CLASS_INFO_tab_sz      (void) const;
  inline const ISA_ENUM_CLASS_VALUE_INFO*get_ISA_ENUM_CLASS_INFO_VALUE_tab   (void) const;
  inline const mUINT32                   get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz(void) const;
  
  /* Handling bundling information ------------------------------------------ */
  inline const ISA_EXEC_UNIT_PROPERTY*   get_ISA_EXEC_unit_prop_tab          (void) const;
  inline const ISA_EXEC_UNIT_SLOTS*      get_ISA_EXEC_unit_slots_tab         (void) const;
  inline const mUINT8*                   get_ISA_BUNDLE_slot_count_tab       (void) const;
  
  /* Handling TOP variants -------------------------------------------------- */
  inline const ISA_VARIANT_INFO*         get_ISA_VARIANT_INFO_tab            (void) const;
  /* ...attribute table used for compatibility check/remapping                */
  inline const ISA_VARIANT_ATTRIBUTE*    get_ISA_VARIANT_attribute_tab       (void) const;
  inline const mUINT32                   get_ISA_VARIANT_attribute_tab_sz    (void) const;
  
  /* Handling hazards -------------------------------------------------------- */
  inline const mUINT8*                   get_ISA_HAZARDS_index_tab           (void) const;
  
  /* Handling ISA parse ----------------------------------------------------- */
  inline const PARSER_GetParserT*        get_ISA_PARSE_tab                   (void) const;
  inline const mUINT32                   get_ISA_PARSE_tab_sz                (void) const;

  /* Handling ISA pack ------------------------------------------------------ */
  inline const ISA_PACK_OPND_INFO*       get_ISA_PACK_OPND_info_tab          (void) const;
  inline const mUINT32                   get_ISA_PACK_OPND_info_tab_sz       (void) const;
  inline const mUINT16*                  get_ISA_PACK_OPND_info_index_tab    (void) const;
  inline const ISA_PACK_INFO*            get_ISA_PACK_info_tab               (void) const;
  inline const mUINT8*                   get_ISA_PACK_inst_words_tab         (void) const;
  inline const ISA_PACK_ADJ_INFO*        get_ISA_PACK_adj_info_tab           (void) const;
  inline const mUINT32                   get_ISA_PACK_adj_info_tab_sz        (void) const;
  inline const mUINT8*                   get_ISA_PACK_adj_info_index_tab     (void) const;

  /* Handling ISA decode ---------------------------------------------------- */
  inline const ISA_DECODE_DECODING**     get_ISA_DECODE_decoding_tab         (void) const;
  inline const mUINT32                   get_ISA_DECODE_decoding_tab_sz      (void) const;

  /* Handling ISA relocs ---------------------------------------------------- */
  inline       ISA_RELOC_INFO*           get_ISA_RELOC_info_tab              (void) const;
  inline const mUINT32                   get_ISA_RELOC_info_tab_sz           (void) const;
  inline       ISA_RELOC_SUBSET_INFO*    get_ISA_RELOC_SUBSET_info_tab       (void) const;
  inline const mUINT32                   get_ISA_RELOC_SUBSET_info_tab_sz    (void) const;
  inline const ISA_SUBSET                get_ISA_RELOC_max_static_virtual_id_core_subset (void) const;
               void                      set_ISA_RELOC_dynamic_reloc_offset  (mUINT32 offset) const;
  // Two following are new from EXT_ISA_API_20100120
  inline       ISA_RELOC_VARIANT_INFO*   get_ISA_RELOC_variant_info_tab      (void) const;
  inline const mUINT32                   get_ISA_RELOC_variant_info_tab_sz   (void) const;

  /* Handling ISA binutils -------------------------------------------------- */
  inline       ISA_BINUTILS_INFO*        get_ISA_BINUTILS_info_tab           (void) const;
  inline       mUINT32                   get_ISA_BINUTILS_info_tab_sz        (void) const;

 private:
  ISA_EXT_Interface_t           * own_isa_ext;
  const ISA_EXT_Interface_t     * isa_ext;
  const ISA_REGISTER_CLASS_INFO * overridden_ISA_REGISTER_CLASS_tab;
  const ISA_OPERAND_VALTYP      * overridden_ISA_OPERAND_operand_types_tab;
  const ISA_EXEC_UNIT_PROPERTY  * overridden_ISA_EXEC_unit_prop_tab;
  const ISA_EXEC_UNIT_SLOTS     * overridden_ISA_EXEC_unit_slots_tab;
  const mUINT8                  * overridden_ISA_BUNDLE_slot_count_tab;
  const ISA_PRINT_INFO          * overridden_ISA_PRINT_info_tab;
  const ISA_LIT_CLASS_INFO      * overridden_ISA_LIT_CLASS_info_tab;
  const ISA_OPERAND_INFO        * overridden_ISA_OPERAND_info_tab;
  EXTENSION_Regclass_Info       * regclass_access_tab;
  EXTENSION_SI_Info             * si_access_tab;
  PARSER_GetParserT const       * overridden_ISA_PARSE_tab;
  mUINT32                         overridden_ISA_PARSE_tab_sz;
  const ISA_PACK_OPND_INFO      * overridden_ISA_PACK_OPND_info_tab;
  mUINT32                         overridden_ISA_PACK_OPND_info_tab_sz;
  const mUINT16                 * overridden_ISA_PACK_OPND_info_index_tab;
  const ISA_PACK_INFO           * overridden_ISA_PACK_info_tab;
  const mUINT8                  * overridden_ISA_PACK_inst_words_tab;
  const ISA_PACK_ADJ_INFO       * overridden_ISA_PACK_adj_info_tab;
  mUINT32                         overridden_ISA_PACK_adj_info_tab_sz;
  const mUINT8                  * overridden_ISA_PACK_adj_info_index_tab;
  const ISA_DECODE_DECODING    ** overridden_ISA_DECODE_decoding_tab;
  mUINT32                         overridden_ISA_DECODE_decoding_tab_sz;
  ISA_RELOC_INFO                * overridden_ISA_RELOC_info_tab;
  mUINT32                         overridden_ISA_RELOC_info_tab_sz;
  ISA_RELOC_SUBSET_INFO         * overridden_ISA_RELOC_SUBSET_info_tab;
  mUINT32                         overridden_ISA_RELOC_SUBSET_info_tab_sz;
  ISA_SUBSET                      overridden_ISA_RELOC_max_static_virtual_id_core_subset;
  ISA_BINUTILS_INFO             * overridden_ISA_BINUTILS_info_tab;
  mUINT32                         overridden_ISA_BINUTILS_info_tab_sz;
  ISA_RELOC_VARIANT_INFO        * overridden_ISA_RELOC_variant_info_tab;
  mUINT32                         overridden_ISA_RELOC_variant_info_tab_sz;
};


// #############################################################################
// ##
// ## Inlined functions for "EXTENSION_Regclass_Info"
// ##
// #############################################################################
// Move within same extension
inline TOP
EXTENSION_Regclass_Info::get_move_X2X_TOP               (INT size) const {
  return       (rc_info->get_move_X2X_TOP(size));
}

// Move from/to core with a 8, 16 or 32bits extension reg
// (size must be 1, 2 or 4)
inline TOP
EXTENSION_Regclass_Info::get_move_R2X_TOP               (INT size) const {
  return       (rc_info->get_move_R2X_TOP(size));
}
inline TOP
EXTENSION_Regclass_Info::get_move_X2R_TOP               (INT size) const {
  return       (rc_info->get_move_X2R_TOP(size));
}

// Compose extension reg value with 32bits data chunks
inline TOP
EXTENSION_Regclass_Info::get_insert_and_zeroext_R2X_TOP (INT size) const {
  return       (rc_info->get_insert_and_zeroext_R2X_TOP(size));
}
inline TOP
EXTENSION_Regclass_Info::get_rotate_and_insert_R2X_TOP  (INT size) const {
  return       (rc_info->get_rotate_and_insert_R2X_TOP(size));
}

// Compose extension reg value with 2x32bits data chunks
inline TOP
EXTENSION_Regclass_Info::get_insert_and_zeroext_RR2X_TOP(INT size) const {
  return       (rc_info->get_insert_and_zeroext_RR2X_TOP(size));
}
inline TOP
EXTENSION_Regclass_Info::get_rotate_and_insert_RR2X_TOP (INT size) const {
  return       (rc_info->get_rotate_and_insert_RR2X_TOP(size));
}

// Extract extension reg value by 32bits data chunks
inline TOP
EXTENSION_Regclass_Info::get_extract_X2R_TOP            (INT size) const {
  return       (rc_info->get_extract_X2R_TOP(size));
}
inline TOP
EXTENSION_Regclass_Info::get_extract_and_rotate_X2R_TOP (INT size) const {
  return       (rc_info->get_extract_and_rotate_X2R_TOP(size));
}

// Clear top
inline TOP
EXTENSION_Regclass_Info::get_clear_TOP                  (INT size) const {
  return       (rc_info->get_clear_TOP(size));
}

// Zero/Sign extend to direct parent register level
// (X size is specified, P size is half X size)
inline TOP
EXTENSION_Regclass_Info::get_zeroext_P2X_TOP            (INT xsize) const {
  return       (rc_info->get_zeroext_P2X_TOP(xsize));
}
inline TOP
EXTENSION_Regclass_Info::get_signext_P2X_TOP            (INT xsize) const {
  return       (rc_info->get_signext_P2X_TOP(xsize));
}

// Zero/Sign extend starting at a given 32bits chunk
inline TOP
EXTENSION_Regclass_Info::get_zeroext_X_TOP              (INT size) const {
  return       (rc_info->get_zeroext_X_TOP(size));
}
inline TOP
EXTENSION_Regclass_Info::get_signext_X_TOP              (INT size) const {
  return       (rc_info->get_signext_X_TOP(size));
}

// Virtual Compose/Extract/Widemove
inline TOP
EXTENSION_Regclass_Info::get_simulated_compose_TOP (INT from_size, INT to_size) const {
  return       (rc_info->get_simulated_compose_TOP(from_size, to_size));
}
inline TOP
EXTENSION_Regclass_Info::get_simulated_extract_TOP (INT from_size, INT to_size) const {
  return       (rc_info->get_simulated_extract_TOP(from_size, to_size));
}
inline TOP
EXTENSION_Regclass_Info::get_simulated_widemove_TOP(INT size) const {
  return       (rc_info->get_simulated_widemove_TOP(size));
}


// #############################################################################
// ##
// ## EXTENSION_SI_Info inlined functions
// ##
// #############################################################################

// Name of the processor it is associated to
inline const char *
EXTENSION_SI_Info::get_processor_name(void) const {
  return (si_info->get_processor_name());
}

// Resources
inline INT
EXTENSION_SI_Info::get_SI_resource_count(void) const {
  return (si_info->get_SI_resource_count());
}
inline SI_RESOURCE * TI_SI_CONST*
EXTENSION_SI_Info::get_SI_resources_tab (void) const {
  return (si_info->get_SI_resources_tab());
}

// Resource access masks
inline TI_SI_CONST SI_RRW
EXTENSION_SI_Info::get_SI_RRW_initializer (void) const {
  return (si_info->get_SI_RRW_initializer());
}
inline TI_SI_CONST SI_RRW
EXTENSION_SI_Info::get_SI_RRW_overuse_mask(void) const {
  return (si_info->get_SI_RRW_overuse_mask());
}

// Issue slots
inline INT
EXTENSION_SI_Info::get_SI_issue_slot_count(void) const {
  return (si_info->get_SI_issue_slot_count());
}
inline SI_ISSUE_SLOT * TI_SI_CONST*
EXTENSION_SI_Info::get_SI_issue_slots_tab (void) const {
  return (si_info->get_SI_issue_slots_tab());
}

// SI table
inline SI ** TI_SI_CONST
EXTENSION_SI_Info::get_SI_ID_si_tab   (void) const {
  return (si_info->get_SI_ID_si_tab());
}
inline mUINT32
EXTENSION_SI_Info::get_SI_ID_si_tab_sz(void) const {
  return (si_info->get_SI_ID_si_tab_sz());
}

// Extension TOP to SI table
inline SI ** TI_SI_CONST
EXTENSION_SI_Info::get_SI_top_si_tab(void) const {
  return (si_info->get_SI_top_si_tab());
}



// #############################################################################
// ##
// ## EXTENSION_ISA_Info inlined functions
// ##
// #############################################################################

/* Interface identification ----------------------------------------------- */
inline mUINT32
EXTENSION_ISA_Info::get_initial_revision (void) const {        /* Magic number                */
  return      (isa_ext->magic);
}

/* Handling dynamic TOPs -------------------------------------------------- */
const mUINT32
EXTENSION_ISA_Info::get_TOP_count    (void) const { /* Return the number of dynamic TOPs*/ 
  return  (isa_ext->get_TOP_count());
}
const char**
EXTENSION_ISA_Info::get_TOP_name_tab (void) const {/* Return TOP name table            */
  return  (isa_ext->get_TOP_name_tab());
}
int
EXTENSION_ISA_Info::belong_TOP_to_ext(TOP top) const {/* Whether a TOP belongs to ext.    */
  return  (isa_ext->belong_TOP_to_ext(top));
}
TOP
EXTENSION_ISA_Info::get_TOP_local    (TOP top) const {/* Return local TOP value.          */
  return  (isa_ext->get_TOP_local(top));
}
void
EXTENSION_ISA_Info::set_TOP_base_offset(INT offs) {/*Set extension base TOP offset    */
  return  (isa_ext->set_TOP_base_offset(offs));
}
TOP
EXTENSION_ISA_Info::get_TOP_AM_automod_variant(TOP top, BOOL post_mod, BOOL inc_mod, ISA_REGISTER_CLASS rc) const {
  return  (isa_ext->get_TOP_AM_automod_variant(top, post_mod, inc_mod, rc));
}
/* Return TOP automod variant       */
void
EXTENSION_ISA_Info::set_TOP_UNDEFINED(TOP top) {/* Set TOP_UNDEFINED value.         */
  return  (isa_ext->set_TOP_UNDEFINED(top));
}

/* Handling TOP properties ------------------------------------------------ */
const EXTENSION_ISA_PROPERTY_FLAGS*
EXTENSION_ISA_Info::get_TOP_prop_tab() const {
  return  (isa_ext->get_TOP_prop_tab());
}

const mUINT16*
EXTENSION_ISA_Info::get_TOP_mem_bytes_tab() const {
  return  (isa_ext->get_TOP_mem_bytes_tab());
}

const mUINT16*
EXTENSION_ISA_Info::get_TOP_mem_align_tab() const {
  return  (isa_ext->get_TOP_mem_align_tab());
}

/* ...attribute table used for compatibility check/remapping                */
const EXTENSION_ISA_PROPERTIES_ATTRIBUTE*
EXTENSION_ISA_Info::get_ISA_PROPERTIES_attribute_tab() const {
  return  (isa_ext->get_ISA_PROPERTIES_attribute_tab());
}

const mUINT32
EXTENSION_ISA_Info::get_ISA_PROPERTIES_attribute_tab_sz() const {
  return  (isa_ext->get_ISA_PROPERTIES_attribute_tab_sz());
}

/* Handling assembly printing --------------------------------------------- */
const ISA_PRINT_INFO*
EXTENSION_ISA_Info::get_ISA_PRINT_info_tab              (void) const {
  return  (overridden_ISA_PRINT_info_tab);
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_PRINT_info_tab_sz           (void) const {
  return  (isa_ext->get_ISA_PRINT_info_tab_sz());
}
const mUINT16*
EXTENSION_ISA_Info::get_ISA_PRINT_info_index_tab        (void) const {
  return  (isa_ext->get_ISA_PRINT_info_index_tab());
}
const char**
EXTENSION_ISA_Info::get_ISA_PRINT_name_tab              (void) const {
  return  (isa_ext->get_ISA_PRINT_name_tab());
}

/* Handling literal classes ----------------------------------------------- */
const ISA_LIT_CLASS_INFO*
EXTENSION_ISA_Info::get_ISA_LIT_CLASS_info_tab          (void) const {
  return  (overridden_ISA_LIT_CLASS_info_tab);
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_LIT_CLASS_info_tab_sz       (void) const {
  return  (isa_ext->get_ISA_LIT_CLASS_info_tab_sz());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_LIT_CLASS_static_max        (void) const {
  return  (isa_ext->get_ISA_LIT_CLASS_static_max());
}

/* Handling operands ------------------------------------------------------ */
const ISA_OPERAND_VALTYP*
EXTENSION_ISA_Info::get_ISA_OPERAND_operand_types_tab   (void) const {
  return  (overridden_ISA_OPERAND_operand_types_tab);
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_OPERAND_operand_types_tab_sz(void) const {
  return  (isa_ext->get_ISA_OPERAND_operand_types_tab_sz());
}
const ISA_OPERAND_INFO*
EXTENSION_ISA_Info::get_ISA_OPERAND_info_tab            (void) const {
  return  (overridden_ISA_OPERAND_info_tab);
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_OPERAND_info_tab_sz         (void) const {
  return  (isa_ext->get_ISA_OPERAND_info_tab_sz());
}
const mUINT16*
EXTENSION_ISA_Info::get_ISA_OPERAND_info_index_tab      (void) const {
  return  (isa_ext->get_ISA_OPERAND_info_index_tab());
}
/* ...tables used for compatibility check/remapping                         */
const ISA_OPERAND_USE_ATTRIBUTE*
EXTENSION_ISA_Info::get_ISA_OPERAND_USE_attribute_tab   (void) const {
  return  (isa_ext->get_ISA_OPERAND_USE_attribute_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_OPERAND_USE_attribute_tab_sz(void) const {
  return  (isa_ext->get_ISA_OPERAND_USE_attribute_tab_sz());
}
/* ...access to core reg class & subclass names used in ext                 */
const char * const *
EXTENSION_ISA_Info::get_ISA_OPERAND_static_rclass_tab      (void) const {
  return  (isa_ext->get_ISA_OPERAND_static_rclass_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_OPERAND_static_rclass_tab_sz   (void) const {
  return  (isa_ext->get_ISA_OPERAND_static_rclass_tab_sz());
}
const char * const *
EXTENSION_ISA_Info::get_ISA_OPERAND_static_rsubclass_tab   (void) const {
  return  (isa_ext->get_ISA_OPERAND_static_rsubclass_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_OPERAND_static_rsubclass_tab_sz(void) const {
  return  (isa_ext->get_ISA_OPERAND_static_rsubclass_tab_sz());
}
  
/* Handling ISA subset ---------------------------------------------------- */
const char* const*
EXTENSION_ISA_Info::get_ISA_SUBSET_tab                  (void) const {
  return  (isa_ext->get_ISA_SUBSET_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_SUBSET_tab_sz               (void) const {
  return  (isa_ext->get_ISA_SUBSET_tab_sz());
}
const unsigned char**
EXTENSION_ISA_Info::get_ISA_SUBSET_op_tab               (void) const {
  return  (isa_ext->get_ISA_SUBSET_op_tab());
}

/* Handling register classes ---------------------------------------------- */
const ISA_REGISTER_CLASS_INFO*
EXTENSION_ISA_Info::get_ISA_REGISTER_CLASS_tab          (void) const {
  return    (overridden_ISA_REGISTER_CLASS_tab);
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_REGISTER_CLASS_tab_sz       (void) const {
  return  (isa_ext->get_ISA_REGISTER_CLASS_tab_sz());
}
const mUINT8*
EXTENSION_ISA_Info::get_ISA_REGISTER_CLASS_index_tab    (void) const {
  return  (isa_ext->get_ISA_REGISTER_CLASS_index_tab());
}
const ISA_REGISTER_SUBCLASS_INFO*
EXTENSION_ISA_Info::get_ISA_REGISTER_SUBCLASS_tab      (void) const {
  return  (isa_ext->get_ISA_REGISTER_SUBCLASS_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_REGISTER_SUBCLASS_tab_sz    (void) const {
  return  (isa_ext->get_ISA_REGISTER_SUBCLASS_tab_sz());
}
/* ...association btwn reg classes and injected TOPs                        */
inline EXTENSION_Regclass_Info*
EXTENSION_ISA_Info::get_Regclass_Info_tab               (void) const {
  return  (regclass_access_tab);
}
const extension_regclass_t*
EXTENSION_ISA_Info::get_REGISTER_CLASS_info_tab         (void) const {
  return  (isa_ext->get_REGISTER_CLASS_info_tab());
}

/* Handling ABI properties ------------------------------------------------ */
const ABI_PROPERTIES*
EXTENSION_ISA_Info::get_ABI_PROPERTIES_tab              (void) const {
  return  (isa_ext->get_ABI_PROPERTIES_tab());
}
/* ...attribute table used for compatibility check/remapping                */
const ABI_PROPERTIES_ATTRIBUTE*
EXTENSION_ISA_Info::get_ABI_PROPERTIES_attribute_tab    (void) const {
  return  (isa_ext->get_ABI_PROPERTIES_attribute_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ABI_PROPERTIES_attribute_tab_sz (void) const {
  return  (isa_ext->get_ABI_PROPERTIES_attribute_tab_sz());
}

/* Handling extension information ----------------------------------------- */
const char*
EXTENSION_ISA_Info::get_DW_DEBUG_EXT_name               (void) const {
  return  (isa_ext->get_DW_DEBUG_EXT_name());
}
const char*
EXTENSION_ISA_Info::get_DW_DEBUG_EXT_reloc_string       (void) const {
  return  (isa_ext->get_DW_DEBUG_EXT_reloc_string());
}
const EXTENSION_REG_DESC*
EXTENSION_ISA_Info::get_DW_DEBUG_EXT_reg_tab            (void) const {
  return  (isa_ext->get_DW_DEBUG_EXT_reg_tab());
}

/* Handling scheduling information ---------------------------------------- */
const extension_si_t*
EXTENSION_ISA_Info::get_scheduling_info_tab             (void) const {
  return  (isa_ext->get_scheduling_info_tab());
}
inline EXTENSION_SI_Info*
EXTENSION_ISA_Info::get_Scheduling_Info_Access_tab      (void) const {
  return  (si_access_tab);
}
const mUINT32
EXTENSION_ISA_Info::get_scheduling_info_tab_sz          (void) const {
  return  (isa_ext->get_scheduling_info_tab_sz());
}

/* Handling enumerations -------------------------------------------------- */
const ISA_ENUM_CLASS_INFO*
EXTENSION_ISA_Info::get_ISA_ENUM_CLASS_INFO_tab         (void) const {
  return  (isa_ext->get_ISA_ENUM_CLASS_INFO_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_ENUM_CLASS_INFO_tab_sz      (void) const {
  return  (isa_ext->get_ISA_ENUM_CLASS_INFO_tab_sz());
}
const ISA_ENUM_CLASS_VALUE_INFO*
EXTENSION_ISA_Info::get_ISA_ENUM_CLASS_INFO_VALUE_tab   (void) const {
  return  (isa_ext->get_ISA_ENUM_CLASS_INFO_VALUE_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz(void) const {
  return  (isa_ext->get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz());
}

/* Handling bundling information ------------------------------------------ */
const ISA_EXEC_UNIT_PROPERTY*
EXTENSION_ISA_Info::get_ISA_EXEC_unit_prop_tab          (void) const {
  return  (overridden_ISA_EXEC_unit_prop_tab);
}

const ISA_EXEC_UNIT_SLOTS*
EXTENSION_ISA_Info::get_ISA_EXEC_unit_slots_tab         (void) const {
  return (overridden_ISA_EXEC_unit_slots_tab);
}

const mUINT8*
EXTENSION_ISA_Info::get_ISA_BUNDLE_slot_count_tab         (void) const {
  return (overridden_ISA_BUNDLE_slot_count_tab);
}

/* Handling TOP variants -------------------------------------------------- */
const ISA_VARIANT_INFO*
EXTENSION_ISA_Info::get_ISA_VARIANT_INFO_tab            (void) const {
  return  (isa_ext->get_ISA_VARIANT_INFO_tab());
}
/* ...attribute table used for compatibility check/remapping                */
const ISA_VARIANT_ATTRIBUTE*
EXTENSION_ISA_Info::get_ISA_VARIANT_attribute_tab       (void) const {
  return  (isa_ext->get_ISA_VARIANT_attribute_tab());
}
const mUINT32
EXTENSION_ISA_Info::get_ISA_VARIANT_attribute_tab_sz    (void) const {
  return  (isa_ext->get_ISA_VARIANT_attribute_tab_sz());
}

/* Handling hazards -------------------------------------------------------- */
inline const mUINT8*
EXTENSION_ISA_Info::get_ISA_HAZARDS_index_tab           (void) const {
  return  (isa_ext->get_ISA_HAZARDS_index_tab());
}

/* Handling ISA parse ----------------------------------------------------- */
inline const PARSER_GetParserT*
EXTENSION_ISA_Info::get_ISA_PARSE_tab                   (void) const {
  return  (overridden_ISA_PARSE_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_PARSE_tab_sz                (void) const {
  return  (overridden_ISA_PARSE_tab_sz);
}

/* Handling ISA pack ------------------------------------------------------ */
inline const ISA_PACK_OPND_INFO*
EXTENSION_ISA_Info::get_ISA_PACK_OPND_info_tab          (void) const {
  return (overridden_ISA_PACK_OPND_info_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_PACK_OPND_info_tab_sz       (void) const {
  return (overridden_ISA_PACK_OPND_info_tab_sz);
}
inline const mUINT16*
EXTENSION_ISA_Info::get_ISA_PACK_OPND_info_index_tab    (void) const {
  return (overridden_ISA_PACK_OPND_info_index_tab);
}
inline const ISA_PACK_INFO*
EXTENSION_ISA_Info::get_ISA_PACK_info_tab               (void) const {
  return (overridden_ISA_PACK_info_tab);
}
inline const mUINT8*
EXTENSION_ISA_Info::get_ISA_PACK_inst_words_tab         (void) const {
  return  (overridden_ISA_PACK_inst_words_tab);
}
inline const ISA_PACK_ADJ_INFO*
EXTENSION_ISA_Info::get_ISA_PACK_adj_info_tab           (void) const {
  return  (overridden_ISA_PACK_adj_info_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_PACK_adj_info_tab_sz        (void) const {
  return  (overridden_ISA_PACK_adj_info_tab_sz);
}
inline const mUINT8*
EXTENSION_ISA_Info::get_ISA_PACK_adj_info_index_tab     (void) const {
  return  (overridden_ISA_PACK_adj_info_index_tab);
}

/* Handling ISA decode ---------------------------------------------------- */
inline const ISA_DECODE_DECODING**
EXTENSION_ISA_Info::get_ISA_DECODE_decoding_tab         (void) const {
  return (overridden_ISA_DECODE_decoding_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_DECODE_decoding_tab_sz      (void) const {
  return (overridden_ISA_DECODE_decoding_tab_sz);
}

/* Handling ISA relocs ---------------------------------------------------- */
inline ISA_RELOC_INFO*
EXTENSION_ISA_Info::get_ISA_RELOC_info_tab              (void) const {
  return (overridden_ISA_RELOC_info_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_RELOC_info_tab_sz           (void) const {
  return (overridden_ISA_RELOC_info_tab_sz);
}
inline ISA_RELOC_SUBSET_INFO*
EXTENSION_ISA_Info::get_ISA_RELOC_SUBSET_info_tab       (void) const {
  return (overridden_ISA_RELOC_SUBSET_info_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_RELOC_SUBSET_info_tab_sz    (void) const {
  return (overridden_ISA_RELOC_SUBSET_info_tab_sz);
}
inline const ISA_SUBSET
EXTENSION_ISA_Info::get_ISA_RELOC_max_static_virtual_id_core_subset (void) const {
  return (overridden_ISA_RELOC_max_static_virtual_id_core_subset);
}
inline ISA_RELOC_VARIANT_INFO*
EXTENSION_ISA_Info::get_ISA_RELOC_variant_info_tab      (void) const {
  return (overridden_ISA_RELOC_variant_info_tab);
}
inline const mUINT32
EXTENSION_ISA_Info::get_ISA_RELOC_variant_info_tab_sz   (void) const {
  return (overridden_ISA_RELOC_variant_info_tab_sz);
}

/* Handling ISA binutils -------------------------------------------------- */
inline ISA_BINUTILS_INFO*
EXTENSION_ISA_Info::get_ISA_BINUTILS_info_tab           (void) const {
  return (overridden_ISA_BINUTILS_info_tab);
}
inline mUINT32
EXTENSION_ISA_Info::get_ISA_BINUTILS_info_tab_sz        (void) const {
  return (overridden_ISA_BINUTILS_info_tab_sz);
}

typedef struct mystruct {
  ISA_EXEC_UNIT_PROPERTY oldval;
  ISA_EXEC_UNIT_PROPERTY newval;
} extension_unit_property_pairing;

BE_EXPORTED extension_unit_property_pairing* CGTARG_get_Extension_Unit_Map(INT32, INT32*);


#endif /* DYN_ISA_API_ACCESS_H */
