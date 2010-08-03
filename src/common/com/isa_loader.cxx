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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/

#include "isa_loader.h"
#include "attribute_map_template.cxx"

/*------------------------------------------------------------------------------
 * API used by both Front-end and Back-end to load extension dlls
 * and initialize common structures (mtypes, intrinsics)
 */

#ifndef BINUTILS

// Table to save PREG -> reg class association
static ISA_REGISTER_CLASS *extension_Preg_To_RegClass_table;

// Table to save PREG -> reg number association
int *extension_Preg_To_RegNum_table;

// Table containing Rclass to preg min offset
INT *extension_RegClass_To_Preg_Min_Offset_table;

// Table containing maximum bitsize width of registers
// in a given extension register class, based on size
// of enclosed subclasses
INT *extension_RegClass_Max_Bitsize_table;

// Table containing maximum bitsize width of registers
// in a given extension register subclass
INT *extension_RegSubClass_Bitsize_table;


/**
 * @see isa_loader_api.h
 */
INT EXTENSION_get_REGISTER_CLASS_max_bit_size(ISA_REGISTER_CLASS rclass) {
  FmtAssert((rclass >  ISA_REGISTER_CLASS_STATIC_MAX &&
             rclass <= ISA_REGISTER_CLASS_MAX),
            ("EXTENSION_get_REGISTER_CLASS_max_bit_size(): non-extension class specified"));
  INT rc_idx = rclass - (ISA_REGISTER_CLASS_STATIC_MAX + 1);
  return extension_RegClass_Max_Bitsize_table[rc_idx];
}

/**
 * @see isa_loader_api.h
 */
INT EXTENSION_get_REGISTER_SUBCLASS_bit_size(ISA_REGISTER_SUBCLASS subclass) {
  INT size;
  FmtAssert((subclass >  ISA_REGISTER_SUBCLASS_STATIC_MAX &&
             subclass <= ISA_REGISTER_SUBCLASS_MAX),
            ("EXTENSION_get_REGISTER_SUBCLASS_bit_size(): non-extension subclass specified"));
  INT subrc_idx = subclass - (ISA_REGISTER_SUBCLASS_STATIC_MAX + 1);
  size = extension_RegSubClass_Bitsize_table[subrc_idx];
  if (size == -1) {
    // Rescue code in case no size was specified for subclass
    const ISA_REGISTER_SUBCLASS_INFO *subclassInfo = ISA_REGISTER_SUBCLASS_Info(subclass);
    ISA_REGISTER_CLASS rclass = ISA_REGISTER_SUBCLASS_INFO_Class(subclassInfo);
    return EXTENSION_get_REGISTER_CLASS_max_bit_size(rclass);
  }
  return size;
}

/*
 * Return the register class associated to the specified PREG number .
 */
ISA_REGISTER_CLASS EXTENSION_PREG_to_REGISTER_CLASS(PREG_NUM preg) {
  FmtAssert((extension_Preg_To_RegClass_table!=NULL),
			("Unexpected NULL extension_Preg_To_RegClass_table"));
  if (preg > Get_Static_Last_Dedicated_Preg_Offset() && preg  <= Get_Last_Dedicated_Preg_Offset_Func()) {
    return (extension_Preg_To_RegClass_table[preg - Get_Static_Last_Dedicated_Preg_Offset() - 1]);
  }
  FmtAssert((0),("Unexpected PREG out of extension bounds: %d", preg));
}
#endif

/*
 * Return a pointer to the newly allocated API used to access to the 
 * ISA description of the specified extension.
 */
EXTENSION_ISA_Info * Generate_EXTENSION_ISA_Info(const Extension_dll_handler_t *dll_instance, BOOL verbose) {
  INT i;
  EXTENSION_ISA_Info  *isa_ext_access = NULL;
  const ISA_EXT_Interface_t *isa_ext;
  get_isa_extension_instance_t get_instance;
  get_instance = (get_isa_extension_instance_t)Get_dll_Symbol(dll_instance, "get_isa_extension_instance");
  isa_ext = (*get_instance)();

  // Check ISA extension compatibility and initialize the access API
  INT magic = isa_ext->magic;
  if (EXTENSION_Is_Supported_ISA_Revision(magic)) {
    if (verbose) {
      get_extension_name_t get_extname = (get_extension_name_t)Get_dll_Symbol(dll_instance, "get_extension_name");
      const char *extname = (*get_extname)();
      fprintf(TFile, "  [Extension '%s'] ISA API revision: lib=%d, compiler=%d\n", extname, magic, MAGIC_NUMBER_EXT_ISA_API);
    }
    isa_ext_access = new EXTENSION_ISA_Info(isa_ext);
  }
  else {
    char err_msg[256];
    sprintf(err_msg,
	    "Incompatible ISA API revision: lib=%d, compiler=%d.\n",
	    magic, MAGIC_NUMBER_EXT_ISA_API);
    RaiseErrorIncompatibleLibrary(dll_instance->dllname, err_msg);
  }

  return (isa_ext_access);
}

/* ===========================================================================
 * Initialize ISA REgister classes tables from statically defined ones
 * concatenated with extension defined ones.
 * Initialized data are: 
 * - ISA_REGISTER_CLASS_info        (array [ISA_REGISTER_CLASS_MAX+1])
 *                                              UNDEFINED at offset 0
 * - ISA_REGISTER_CLASS_info_index  (array [ISA_REGISTER_CLASS_MAX+1])
 * - ISA_REGISTER_SUBCLASS_info     (array [ISA_REGISTER_SUBCLASS_MAX+1])
 * ===========================================================================*/
void Initialize_ISA_RegisterClasses(Lai_Loader_Info_t &ext_info) {
  int ext, j;
  int static_size;
  ISA_REGISTER_CLASS_INFO    *rc_tab;
  ISA_REGISTER_SUBCLASS_INFO *subrc_tab;
  mUINT8 *rc_index_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA Register classes\n");
  }

  int nb_added_rclass = ISA_REGISTER_CLASS_MAX - ISA_REGISTER_CLASS_STATIC_MAX;
  int nb_added_rsubclass = ISA_REGISTER_SUBCLASS_MAX - ISA_REGISTER_SUBCLASS_STATIC_MAX;
#ifndef BINUTILS
  int nb_added_preg = Get_Last_Dedicated_Preg_Offset_Func() - Get_Static_Last_Dedicated_Preg_Offset();
#endif

  if (nb_added_rclass == 0) {
    return;
  }

  // Resize register class table
  static_size = ISA_REGISTER_CLASS_STATIC_MAX + 1;
  rc_tab = TYPE_MEM_POOL_ALLOC_N(ISA_REGISTER_CLASS_INFO, Malloc_Mem_Pool,
				 (static_size + nb_added_rclass));
  memcpy(rc_tab, ISA_REGISTER_CLASS_info, static_size * sizeof(ISA_REGISTER_CLASS_INFO));
  
#ifndef BINUTILS
  extension_Preg_To_RegClass_table =  TYPE_MEM_POOL_ALLOC_N(ISA_REGISTER_CLASS,
						      Malloc_Mem_Pool,
						      nb_added_preg);
  extension_Preg_To_RegNum_table =  TYPE_MEM_POOL_ALLOC_N(int,
							  Malloc_Mem_Pool,
							  nb_added_preg);
  //TB: add a mapping between register class and Preg_Min_Offset for this class
  extension_RegClass_To_Preg_Min_Offset_table = TYPE_MEM_POOL_ALLOC_N(INT,
								      Malloc_Mem_Pool,
								      nb_added_rclass);

  extension_RegSubClass_Bitsize_table = TYPE_MEM_POOL_ALLOC_N(INT,
                                                              Malloc_Mem_Pool,
                                                              nb_added_rsubclass+1);
  for (j=0; j<nb_added_rsubclass; j++) {
    extension_RegSubClass_Bitsize_table[j] = -1;
  }

  extension_RegClass_Max_Bitsize_table = TYPE_MEM_POOL_ALLOC_N(INT,
                                                               Malloc_Mem_Pool,
                                                               nb_added_rclass+1);
  for (j=0; j<nb_added_rclass; j++) {
    extension_RegClass_Max_Bitsize_table[j] = -1;
  }

#endif

  // Resize register class index table
  // (it might be oversized, as several entries in register class table might
  //  represent different regset of the same real register class)
  rc_index_tab = TYPE_MEM_POOL_ALLOC_N(mUINT8, Malloc_Mem_Pool,
				       static_size + nb_added_rclass);
  memcpy(rc_index_tab, ISA_REGISTER_CLASS_info_index, static_size * sizeof(mUINT8));
  
  // Resize sub register class table
  if (nb_added_rsubclass > 0) {
    static_size   = ISA_REGISTER_SUBCLASS_STATIC_MAX + 1;
    subrc_tab = TYPE_MEM_POOL_ALLOC_N(ISA_REGISTER_SUBCLASS_INFO,
				      Malloc_Mem_Pool,
				      static_size + nb_added_rsubclass);
    memcpy(subrc_tab, ISA_REGISTER_SUBCLASS_info,
	   static_size * sizeof(ISA_REGISTER_SUBCLASS_INFO));
  }

  int  next_rc_index  = ISA_REGISTER_CLASS_INDEX_DYNAMIC_NEXT; // next value to be put in REGISTER_CLASS_index table
  int  next_rc_offset = ISA_REGISTER_CLASS_STATIC_MAX+1; // next REGISTER_CLASS_index table offset to fill
#ifndef BINUTILS
  int Local_Last_Dedicated_Preg_Offset = Get_Static_Last_Dedicated_Preg_Offset();
#endif
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int rc_in_ext    = ext_info.ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab_sz();
    int subrc_in_ext = (nb_added_rsubclass > 0) ? ext_info.ISA_tab[ext]->get_ISA_REGISTER_SUBCLASS_tab_sz() : 0;
#ifndef BINUTILS
    FmtAssert((Local_Last_Dedicated_Preg_Offset + 1 == ext_info.base_PREG[ext]),("Difference between base preg and conputed one for extension %d", ext));
#endif
    if (rc_in_ext > 0) {
      
      // Add register classes
      memcpy(&rc_tab[ext_info.base_REGISTER_CLASS[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab(),
	     rc_in_ext * sizeof(ISA_REGISTER_CLASS_INFO));

      /* Backward compatibility. We ignore the subset mask (isa_mask) for the register class.
	 Actually, we force it to be the principal currently active subset, such that the register
	 calss is always activated.
	 Anyway any loaded register class is active as soon as the extensiojn is active, 
	 i.e. this information is not used.
	 For instance on stxp70, the stxp70 RTK may generate either the mask (1<<ISA_SUBSET_stxp70) or
	 the mask (1<<ISA_SUBSET_stxp70_ext) while ISA_SUBSET_stxp70_ext is now obsolete. */
      for (int regcl = ext_info.base_REGISTER_CLASS[ext]; 
	   regcl < ext_info.base_REGISTER_CLASS[ext] + rc_in_ext; 
	   regcl++) {
        rc_tab[regcl].isa_mask = (1<<*(ISA_SUBSET_LIST_Begin(ISA_SUBSET_List)));
      }

#ifndef BINUTILS
      //TB: Update PREG with regclass
      for (int regcl = ext_info.base_REGISTER_CLASS[ext]; 
	   regcl < ext_info.base_REGISTER_CLASS[ext] + rc_in_ext; 
	   regcl++) {
	int first_isa_reg  = rc_tab[regcl].min_regnum;
	int last_isa_reg   = rc_tab[regcl].max_regnum;
	int register_count = last_isa_reg - first_isa_reg + 1;
	for (int k = Local_Last_Dedicated_Preg_Offset; 
	     k < Local_Last_Dedicated_Preg_Offset + register_count; 
	     k++) {
	  // Update PREG to register class array
	  extension_Preg_To_RegClass_table[k - Get_Static_Last_Dedicated_Preg_Offset() ] = regcl; 
	  extension_Preg_To_RegNum_table[k - Get_Static_Last_Dedicated_Preg_Offset() ] = first_isa_reg + (k - Local_Last_Dedicated_Preg_Offset) ; 
	}
	extension_RegClass_To_Preg_Min_Offset_table[regcl - ISA_REGISTER_CLASS_STATIC_MAX - 1] = Local_Last_Dedicated_Preg_Offset + 1;
	Local_Last_Dedicated_Preg_Offset += register_count;
      }
#endif
      // Add register subclasses
      if (subrc_in_ext > 0) {
	memcpy(&subrc_tab[ext_info.base_REGISTER_SUBCLASS[ext]],
	       ext_info.ISA_tab[ext]->get_ISA_REGISTER_SUBCLASS_tab(),
	       subrc_in_ext * sizeof(ISA_REGISTER_SUBCLASS_INFO));

	// ...Update reference to the register class
	for (j=0; j<subrc_in_ext; j++) {
	  subrc_tab[ext_info.base_REGISTER_SUBCLASS[ext]+j].rclass += ext_info.base_REGISTER_CLASS[ext];
	}
      }

      for (j=0; j<rc_in_ext; j++) {
	// Fill index table (one regclass can have several regsets)
	// TODO: if several entries of the ISA_REGISTER_CLASS_info table are
	//       associated to the same register class name,
	//       then ISA_REGISTER_CLASS_MAX should be decremented to reflect that.
	//   --> Fortunately, a check is done in isa_register_gen to forbid
	//       register class with multiple register sets.
	if ((j==0) ||
	    (strcmp(rc_tab[ext_info.base_REGISTER_CLASS[ext]+j-1].name,
		    rc_tab[ext_info.base_REGISTER_CLASS[ext]+j].name) != 0)) {
	  rc_index_tab[next_rc_offset++] = next_rc_index;
	}
	next_rc_index++;
      }
    }
  }

  // Update targinfo pointers
  ISA_REGISTER_CLASS_info       = rc_tab;
  if (nb_added_rsubclass > 0) {
    ISA_REGISTER_SUBCLASS_info  = subrc_tab;
  }
  ISA_REGISTER_CLASS_info_index = rc_index_tab;

#ifndef BINUTILS
  FmtAssert((Local_Last_Dedicated_Preg_Offset == Get_Last_Dedicated_Preg_Offset_Func()),
	    ("Computed max preg != Last_Dedicated_Preg_Offset"));
#endif
}

/* ===========================================================================
 * Initialize ABI PROPERTIES tables from statically defined ones concatenated
 * with extension defined ones. Initialized data are: 
 * - ABI_PROPERTIES_base_props     (array)
 *
 * ===========================================================================*/
void Initialize_ABI_Properties(Lai_Loader_Info_t &ext_info) {
  int ext;
  ABI_PROPERTIES *abi;
  int             abi_idx;
  
  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ABI properties\n");
  }

  if (ISA_REGISTER_CLASS_MAX == ISA_REGISTER_CLASS_STATIC_MAX) {
    return;  // No extension register classes
  }

  // Allocate new table
  abi = TYPE_MEM_POOL_ALLOC_N(ABI_PROPERTIES, Malloc_Mem_Pool,
			      (ABI_PROPERTIES_ABI_MAX+1));

  // For each ABI, allocate new tables, initialize them with the content
  // of the initial static tables and complete them extension specific
  // data
  // Warning, the model which has been adopted so far consider
  // that we define only one ABI extension for each extension.
  // As a result, we always have only one item in extension
  // ABI_PROPERTIES table for extension.
  for (abi_idx=0; abi_idx<=ABI_PROPERTIES_ABI_MAX; abi_idx++) {
    mUINT32     **reg_flags_tab;
    const char * const **reg_names_tab;
    const mUINT32 ext_abi_idx=0;     /* ABI index for extension. (Forced to 0). */

    reg_flags_tab = TYPE_MEM_POOL_ALLOC_N(mUINT32*, Malloc_Mem_Pool,
					  (ISA_REGISTER_CLASS_MAX+1));
    memcpy(reg_flags_tab, ABI_PROPERTIES_base_props[abi_idx].reg_flags,
	   (ISA_REGISTER_CLASS_STATIC_MAX+1) * sizeof(mUINT32*));
    abi[abi_idx].reg_flags = reg_flags_tab;

    reg_names_tab = TYPE_MEM_POOL_ALLOC_N(const char* const *, Malloc_Mem_Pool,
					  (ISA_REGISTER_CLASS_MAX+1));
    memcpy(reg_names_tab, ABI_PROPERTIES_base_props[abi_idx].reg_names,
	   (ISA_REGISTER_CLASS_STATIC_MAX+1) * sizeof(const char * const*));
    abi[abi_idx].reg_names = reg_names_tab;
    
    for (ext=0; ext<ext_info.nb_ext; ext++) {
      int nb_rc = ext_info.ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab_sz();
      if (nb_rc > 0) {
	const ABI_PROPERTIES *old_abi;
	old_abi = &ext_info.ISA_tab[ext]->get_ABI_PROPERTIES_tab()[ext_abi_idx];
	for (int rc = 0; rc < nb_rc; rc++) {
	  reg_flags_tab[ext_info.base_REGISTER_CLASS[ext]+rc] = old_abi->reg_flags[rc];
	  reg_names_tab[ext_info.base_REGISTER_CLASS[ext]+rc] = old_abi->reg_names[rc];
	}

	{ 
	  // Remap ABI properties attributes. 
	  // Core and Extension ABI properties identifiers are stored on mUINT32 as a mask (1<<id).
	  typedef ATTRIBUTE_MAP<mUINT32> CORE_ATTR_MAP;
	  typedef ATTRIBUTE_MAP<mUINT32> EXT_ATTR_MAP;
	  // The ABI properties mask for registers are stored both on mUINT32 in extension and core.
	  typedef ATTRIBUTE_REMAPPER<EXT_ATTR_MAP, CORE_ATTR_MAP, mUINT32, mUINT32 > EXT_ATTR_REMAPPER;
	  const NAME_VALUE<mUINT32> *core_attr_tab = (const NAME_VALUE<mUINT32> *)ABI_PROPERTIES_get_attribute_table();
	  const NAME_VALUE<mUINT32> *ext_attr_tab  = (const NAME_VALUE<mUINT32> *)ext_info.ISA_tab[ext]->get_ABI_PROPERTIES_attribute_tab();
	  INT    ext_attr_count = ext_info.ISA_tab[ext]->get_ABI_PROPERTIES_attribute_tab_sz();
	  CORE_ATTR_MAP core_attr_map(core_attr_tab, ABI_PROPERTIES_ATTRIBUTE_COUNT, CORE_ATTR_MAP::KIND_MASK);
	  EXT_ATTR_MAP ext_attr_map(ext_attr_tab, ext_attr_count, CORE_ATTR_MAP::KIND_MASK);
	  EXT_ATTR_REMAPPER remapper(ext_attr_map, core_attr_map);

	  for (int rc = 0; rc < nb_rc; rc++) {
	    mUINT32 *attr_walker = reg_flags_tab[ext_info.base_REGISTER_CLASS[ext]+rc];
	    for (int reg=0; reg<=ISA_REGISTER_MAX; reg++) {
	      if (!remapper.remapMask(attr_walker[reg], attr_walker[reg])) 
		RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, 
					      "Incompatible target description,"
					      "ABI_PROPERTY attribute name in Extension not found in Core.");
	    }
	  }
	}
      }
    }
  }
  
  // Update targinfo pointers
  ABI_PROPERTIES_base_props = abi;
  ABI_PROPERTIES_Initialize();
}

/* ===========================================================================
 * Initialize ISA Reloc from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_Decode_Decoding      (array)
 * ===========================================================================*/
void Initialize_ISA_Reloc(Lai_Loader_Info_t &ext_info, MEM_POOL &tmp_mempool) {
  ISA_RELOC_INFO * new_reloc_info;
  int ext_num_reloc_info;
  ISA_RELOC_VARIANT_INFO * new_reloc_variant_info;
  int ext_num_reloc_variant_info;
  ISA_RELOC_SUBSET_INFO * new_reloc_subset_info;
  int ext_num_reloc_subset_info;
  ISA_RELOC * new_reloc_virt;
  int max_reloc_virt_id;
  ISA_VIRTUAL_RELOC * subset_max_virtual_id;
  ISA_VIRTUAL_RELOC max_virtual_id;
  ISA_SUBSET subset;
  ISA_VIRTUAL_RELOC * new_reloc_virtual;
  int ext;
  int ext_optypes_num;
  ISA_OPERAND_VALTYP *ext_optypes_tab;
  int ext_optypes_nr;
  int ext_optypes_id;
  
  if (TOP_count == TOP_static_count) {        // No extension top: easy work.
    return;
  }
  
  subset_max_virtual_id = TYPE_MEM_POOL_ALLOC_N(ISA_VIRTUAL_RELOC,&tmp_mempool,ISA_SUBSET_count);
  max_virtual_id = 0;
  for (subset=ISA_SUBSET_MIN;subset<ISA_SUBSET_static_count;subset++) {
     subset_max_virtual_id[subset] = ISA_RELOC_Max_Virtual_Id(subset);
     if (subset_max_virtual_id[subset] > max_virtual_id) {
        max_virtual_id = subset_max_virtual_id[subset];
     }
  }
  
  // Count number of reloc_info to be dynamically added  
  ext_num_reloc_subset_info = ISA_SUBSET_static_count;
  ext_num_reloc_info = ISA_RELOC_STATIC_MAX+1;
  ext_num_reloc_variant_info = ISA_RELOC_STATIC_MAX+1;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
     ext_num_reloc_subset_info += ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab_sz();
     ext_num_reloc_info += ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab_sz();
     ext_num_reloc_variant_info += ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab_sz();
  }
  new_reloc_subset_info = TYPE_MEM_POOL_ALLOC_N(ISA_RELOC_SUBSET_INFO, Malloc_Mem_Pool, ext_num_reloc_subset_info);
  memcpy(new_reloc_subset_info, ISA_reloc_subset_tab, ISA_SUBSET_static_count * sizeof(ISA_RELOC_SUBSET_INFO));
  new_reloc_info = TYPE_MEM_POOL_ALLOC_N(ISA_RELOC_INFO, Malloc_Mem_Pool, ext_num_reloc_info);
  memcpy(new_reloc_info, ISA_RELOC_info, (ISA_RELOC_STATIC_MAX+1) * sizeof(ISA_RELOC_INFO));
  new_reloc_variant_info = TYPE_MEM_POOL_ALLOC_N(ISA_RELOC_VARIANT_INFO, Malloc_Mem_Pool, ext_num_reloc_variant_info);
  memcpy(new_reloc_variant_info, ISA_RELOC_variant_info, (ISA_RELOC_STATIC_MAX+1) * sizeof(ISA_RELOC_VARIANT_INFO));
  
  ext_num_reloc_subset_info = ISA_SUBSET_static_count;
  ext_num_reloc_info = ISA_RELOC_STATIC_MAX+1;
  ext_num_reloc_variant_info = ISA_RELOC_STATIC_MAX+1;
  ext_optypes_num = ISA_OPERAND_TYPES_STATIC_COUNT;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
     ISA_VIRTUAL_RELOC ext_virtual_id_base;
     ISA_RELOC_INFO * ext_reloc_info;
     ISA_RELOC_VARIANT_INFO * ext_reloc_variant_info;
     int ext_reloc_num;
     ISA_RELOC_SUBSET_INFO * ext_reloc_subset_info;
     int ext_reloc_subset_num;
     
     /* Setup extension base reloc number within ISA_RELOC_info table */
     if (NULL != ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab()) {
        ext_info.ISA_tab[ext]->set_ISA_RELOC_dynamic_reloc_offset(ext_num_reloc_info);
     }

     /* Renumbering of virtual relocation within ext RELOC_INFO */
     subset = ext_info.ISA_tab[ext]->get_ISA_RELOC_max_static_virtual_id_core_subset();
     ext_virtual_id_base = subset_max_virtual_id[subset]+1;
     ext_reloc_info = ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab();
     ext_reloc_variant_info = ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab();
     for (ext_reloc_num=0; ext_reloc_num<ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab_sz(); ext_reloc_num++) {
        ext_reloc_info[ext_reloc_num].virtual_id += ext_virtual_id_base;
     }
     for (ext_reloc_num=0; ext_reloc_num<ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab_sz(); ext_reloc_num++) {
        ext_reloc_variant_info[ext_reloc_num].reloc_prev_encoding += ext_num_reloc_variant_info;
        ext_reloc_variant_info[ext_reloc_num].reloc_next_encoding += ext_num_reloc_variant_info;
     }

     ext_reloc_subset_info = ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab();
     for (ext_reloc_subset_num=0; ext_reloc_subset_num<ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab_sz(); ext_reloc_subset_num++) {
        int subset_reloc_num;
	
        /* Setup extension subset max virtual id per subset */
        ext_reloc_subset_info[ext_reloc_subset_num].max_virtual_id += ext_virtual_id_base;

        /* Setup extension subset references from tab_subset_reloc to ISA_RELOC_info */
	for (subset_reloc_num=0; subset_reloc_num<ext_reloc_subset_info[ext_reloc_subset_num].nb_subset_relocs; subset_reloc_num++) {
	   ext_reloc_subset_info[ext_reloc_subset_num].tab_subset_reloc[subset_reloc_num] += ext_num_reloc_info;
	}
     }

     /* Align local max virtual id in associated subset for next ext concerned and keep trace of overall max virtual id */
     subset_max_virtual_id[subset] += ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab_sz();
     if (subset_max_virtual_id[subset] > max_virtual_id) {
        max_virtual_id = subset_max_virtual_id[subset];
     }

     /* Copy RELOC_SUBSET_INFO from ext into global one */
     if (NULL != ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab()) {
        memcpy(&new_reloc_subset_info[ext_num_reloc_subset_info], 
               ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab(), 
	       ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab_sz() * sizeof(ISA_RELOC_SUBSET_INFO));
     }

     /* Copy RELOC_INFO from ext into global one */
     if (NULL != ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab()) {
        memcpy(&new_reloc_info[ext_num_reloc_info], 
               ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab(), 
	       ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab_sz() * sizeof(ISA_RELOC_INFO));
     }

     /* Copy RELOC_VARIANT_INFO from ext into global one */
     if (NULL != ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab()) {
        memcpy(&new_reloc_variant_info[ext_num_reloc_info], 
               ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab(), 
	       ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab_sz() * sizeof(ISA_RELOC_VARIANT_INFO));
     }

     /* Now we have to perform renumbering of relocation ids within OPERAND_VALTYPE array */
     ext_optypes_tab = &ISA_OPERAND_operand_types[ext_optypes_num];
     ext_optypes_nr = ext_info.ISA_tab[ext]->get_ISA_OPERAND_operand_types_tab_sz();
     for (ext_optypes_id=0;ext_optypes_id<ext_optypes_nr;ext_optypes_id++) {
        int i;
	
        ext_optypes_tab[ext_optypes_id].default_reloc += ext_num_reloc_info;
	for (i=0;
	     i<ISA_OPERAND_VALTYP_Relocs(&ext_optypes_tab[ext_optypes_id]) && ISA_OPERAND_VALTYP_Is_Valid_Reloc(&ext_optypes_tab[ext_optypes_id],i);
	     i++) {
	  ext_optypes_tab[ext_optypes_id].reloc[i] += ext_num_reloc_info;
	}
     }
     
     /* Let's go to next extension
     ext_num_reloc_subset_info += ext_info.ISA_tab[ext]->get_ISA_RELOC_SUBSET_info_tab_sz();
     ext_num_reloc_info += ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab_sz();
     ext_num_reloc_variant_info += ext_info.ISA_tab[ext]->get_ISA_RELOC_variant_info_tab_sz();
     ext_optypes_num += ext_info.ISA_tab[ext]->get_ISA_OPERAND_operand_types_tab_sz();
     
     /* Add relocation number of extension to MAX */
     ISA_RELOC_MAX += ext_info.ISA_tab[ext]->get_ISA_RELOC_info_tab_sz();
  }
  
  new_reloc_virtual = TYPE_MEM_POOL_ALLOC_N(ISA_VIRTUAL_RELOC, Malloc_Mem_Pool, max_virtual_id*sizeof(ISA_VIRTUAL_RELOC));
  memset(new_reloc_virtual,ISA_RELOC_UNDEFINED,max_virtual_id);
  
  // Switch to extended ISA_RELOC
  ISA_reloc_subset_tab = new_reloc_subset_info;
  ISA_RELOC_info = new_reloc_info;
  ISA_RELOC_variant_info = new_reloc_variant_info;
  reloc_virtual_tab = new_reloc_virtual;
  sz_reloc_virtual_tab = max_virtual_id;
}

#ifdef BACK_END

// Associate regclass to regclass info
static const EXTENSION_Regclass_Info **extension_regclass_table = (const EXTENSION_Regclass_Info**)NULL;

/*
 * Return a pointer to the description of extension register class
 */
const EXTENSION_Regclass_Info *EXTENSION_get_REGISTER_CLASS_info(ISA_REGISTER_CLASS rc) {
  FmtAssert((extension_regclass_table!=NULL), ("Unexpected NULL extension_regclass_table"));
  if (EXTENSION_Is_Extension_REGISTER_CLASS(rc)) {
    return (extension_regclass_table[rc - (ISA_REGISTER_CLASS_STATIC_MAX+1)]);
  }
  return ((EXTENSION_Regclass_Info*)NULL);
}

/* ===========================================================================
 * Keep access to extension info table
 * ===========================================================================*/
static Lai_Loader_Info_t * global_Lai_Loader_Info_table;
Lai_Loader_Info_t * Get_Lai_Loader_Info_Table() { return global_Lai_Loader_Info_table; }

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
void Initialize_ISA_Top(Lai_Loader_Info_t &ext_info) {
  int ext;
  const char **TOP_names_tab;
  ISA_PROPERTY_FLAGS     *TOP_prop_tab;
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
  
  TOP_prop_tab = TYPE_MEM_POOL_ALLOC_N(ISA_PROPERTY_FLAGS, Malloc_Mem_Pool, (TOP_count+1));
  memcpy(TOP_prop_tab, ISA_PROPERTIES_flags, (TOP_static_count) * sizeof(ISA_PROPERTY_FLAGS));
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
      
      memcpy(&TOP_mem_bytes_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_TOP_mem_bytes_tab(),
	     top_in_ext * sizeof(mUINT16));
      
      memcpy(&TOP_mem_align_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_TOP_mem_align_tab(),
	     top_in_ext * sizeof(mUINT16));

      { 
	// Remap ISA properties attributes. 
	// Core ISA properties identifiers are stored on UINT64.
	// Extension ISA properties identifiers are stored on UINT64, the identifier 
	// is stored as a mask (1<<id) when the number of properties is <= 64,
	// or as the identifier value if count > 64.
	typedef ATTRIBUTE_MAP<mUINT64> CORE_ATTR_MAP;
	typedef ATTRIBUTE_MAP<mUINT64> EXT_ATTR_MAP;
	// The properties mask are stored both on UINT64 in extension and core.
	typedef ATTRIBUTE_REMAPPER<EXT_ATTR_MAP, CORE_ATTR_MAP, mUINT64, mUINT64 > EXT_ATTR_REMAPPER;
	const NAME_VALUE<mUINT64> *core_attr_tab = (const NAME_VALUE<mUINT64> *)ISA_PROPERTIES_get_attribute_table();
	const NAME_VALUE<mUINT64> *ext_attr_tab  = (const NAME_VALUE<mUINT64> *)ext_info.ISA_tab[ext]->get_ISA_PROPERTIES_attribute_tab();
	INT ext_attr_count = ext_info.ISA_tab[ext]->get_ISA_PROPERTIES_attribute_tab_sz();
	EXT_ATTR_MAP::map_kind ext_map_kind = ext_attr_count > 64 ? EXT_ATTR_MAP::KIND_VALUE: EXT_ATTR_MAP::KIND_MASK;
	/* Map of properties names to static ISA properties identifiers. */
	CORE_ATTR_MAP core_attr_map(core_attr_tab, ISA_PROPERTIES_ATTRIBUTE_COUNT);
	/* Map of extension properties to extension properties identifiers. */
	EXT_ATTR_MAP ext_attr_map(ext_attr_tab, ext_attr_count, ext_map_kind);
	/* Will remap from the EXTENSION_ISA_PROPERTY_FLAGS,  variable size mask of UINT64[ext_word_count], to
	 * ISA_PROPERTY_FLAGS, also a variable mask of UINT64[core_word_count]. */
	EXT_ATTR_REMAPPER remapper(ext_attr_map, core_attr_map);
	INT ext_word_count = (ext_attr_count + 63) / 64;
	INT core_word_count = (ISA_PROPERTIES_ATTRIBUTE_COUNT + 63) / 64;

	const mUINT64 *ext_mask_walker = (const mUINT64 *)ext_info.ISA_tab[ext]->get_TOP_prop_tab();
	mUINT64 *core_mask_walker = (mUINT64 *)&TOP_prop_tab[ext_info.base_TOP[ext]];
	for (int top = 0; top < top_in_ext; top++) {
	  if (!remapper.remapMaskPtr(ext_mask_walker, core_mask_walker, ext_word_count, core_word_count))
	    RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, 
					  "Incompatible target description,"
					  "ISA_PROPERTY attribute name in Extension not found in Core.");
	  ext_mask_walker += ext_word_count;
	  core_mask_walker += core_word_count;
	}
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
void Initialize_ISA_Print(Lai_Loader_Info_t &ext_info) {
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
void Initialize_ISA_Bundle(Lai_Loader_Info_t &ext_info) {
  int ext;
  ISA_EXEC_UNIT_PROPERTY *unit_prop_tab;
  ISA_EXEC_UNIT_SLOTS    *unit_slots_tab;
  mUINT8                 *bundle_slot_count_tab;

  if (ext_info.trace_on) {
    fprintf(TFile, "...Initialize extension ISA bundles\n");
  }

  if (TOP_count == TOP_static_count) {
    return; // No top in extension
  }


  int i;
  int subset_count = ISA_SUBSET_static_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    subset_count += ext_info.ISA_tab[ext]->get_ISA_SUBSET_tab_sz();
  }


  // Allocate new tables and fill it with the content
  // of the initial static table
  unit_prop_tab = TYPE_MEM_POOL_ALLOC_N(ISA_EXEC_UNIT_PROPERTY, Malloc_Mem_Pool, TOP_count);
  memcpy(unit_prop_tab, ISA_EXEC_unit_prop, TOP_static_count * sizeof(ISA_EXEC_UNIT_PROPERTY));
  unit_slots_tab = TYPE_MEM_POOL_ALLOC_N(ISA_EXEC_UNIT_SLOTS, Malloc_Mem_Pool, TOP_count);
  memcpy(unit_slots_tab, ISA_EXEC_unit_slots, TOP_static_count * sizeof(ISA_EXEC_UNIT_SLOTS));

  /* ISA_SUBSET_List initialized by Initialize_ISA_Subsets */
  bundle_slot_count_tab = TYPE_MEM_POOL_ALLOC_N(mUINT8, Malloc_Mem_Pool, subset_count);
  memcpy(bundle_slot_count_tab, ISA_BUNDLE_slot_count, ISA_SUBSET_static_count * sizeof(mUINT8));

  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    if (top_in_ext) {
      // Concatenate extension defined unit properties
      memcpy(&unit_prop_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_EXEC_unit_prop_tab(),
	     top_in_ext * sizeof(ISA_EXEC_UNIT_PROPERTY));

      // Concatenate extension defined unit slots
      memcpy(&unit_slots_tab[ext_info.base_TOP[ext]],
	     ext_info.ISA_tab[ext]->get_ISA_EXEC_unit_slots_tab(),
	     top_in_ext * sizeof(ISA_EXEC_UNIT_SLOTS));
    }

    int ext_num_subsets = ext_info.ISA_tab[ext]->get_ISA_SUBSET_tab_sz();
    subset_count = ISA_SUBSET_static_count;
    if(ext_num_subsets) {
      // Concatenate extension defined bundle slot count
      memcpy(&bundle_slot_count_tab[subset_count+ext],
	     ext_info.ISA_tab[ext]->get_ISA_BUNDLE_slot_count_tab(),
	     ext_num_subsets * sizeof(mUINT8));
      subset_count += ext_num_subsets;
    }
  }
  
  // Update targinfo pointers
  ISA_EXEC_unit_prop = unit_prop_tab;
  ISA_EXEC_unit_slots = unit_slots_tab;
  ISA_BUNDLE_slot_count = bundle_slot_count_tab;
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
 *       (Doing so will need update of the Operand initialization)
 *
 * NOTE: Currently, Core literal classes are not used by extension
 *       operands, there is then no remapping/coherency check done
 *       between the ones defined for the core and the ones embedded
 *       in extensions.
 * ===========================================================================*/
void Initialize_ISA_Literals(Lai_Loader_Info_t &ext_info) {
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
void Initialize_ISA_Enum(Lai_Loader_Info_t &ext_info) {
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
 *
 * Assumptions:
 * - Extension TOPs use only extension defined OPERAND_info, not core ones
 *   (not a problem if they are duplicated)
 * ===========================================================================*/
void Initialize_ISA_Operands(Lai_Loader_Info_t &ext_info, MEM_POOL        &tmp_mempool) {
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

    optypes_tab = TYPE_MEM_POOL_ALLOC_N(ISA_OPERAND_VALTYP, Malloc_Mem_Pool, nb_optypes);
    if (nb_optypes > ISA_OPERAND_TYPES_STATIC_COUNT) {
      memset(&optypes_tab[ISA_OPERAND_TYPES_STATIC_COUNT],0,
	     (nb_optypes-ISA_OPERAND_TYPES_STATIC_COUNT) * sizeof(ISA_OPERAND_VALTYP));
    }
    memcpy(optypes_tab, ISA_OPERAND_operand_types,
	   ISA_OPERAND_TYPES_STATIC_COUNT * sizeof(ISA_OPERAND_VALTYP));

    opinfo_tab = TYPE_MEM_POOL_ALLOC_N(ISA_OPERAND_INFO, Malloc_Mem_Pool, nb_opinfo);
    memcpy(opinfo_tab, ISA_OPERAND_info,
	   ISA_OPERAND_INFO_STATIC_COUNT * sizeof(ISA_OPERAND_INFO));

    opinfo_idx_tab = TYPE_MEM_POOL_ALLOC_N(mUINT16, Malloc_Mem_Pool, TOP_count);
    memcpy(opinfo_idx_tab, ISA_OPERAND_info_index, (TOP_static_count) * sizeof(mUINT16));

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
	      RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, err_msg);
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
	      RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, err_msg);
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

#ifndef BINUTILS
            INT rc_idx = tgt_op->rclass - (ISA_REGISTER_CLASS_STATIC_MAX + 1);
            if (extension_RegClass_Max_Bitsize_table[rc_idx] < src_op->size) {
              extension_RegClass_Max_Bitsize_table[rc_idx] = src_op->size;
            }
#endif

	    if (tgt_op->rsubclass != ISA_REGISTER_SUBCLASS_UNDEFINED) {
	      tgt_op->rsubclass += ext_info.base_REGISTER_SUBCLASS[ext] - (subrc_static_max_in_ext+1);

#ifndef BINUTILS
              if (src_op->size != -1) {
                INT subrc_idx = tgt_op->rsubclass - (ISA_REGISTER_SUBCLASS_STATIC_MAX + 1);
                extension_RegSubClass_Bitsize_table[subrc_idx] = src_op->size;
              }
#endif
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

      { 
	// Remap ISA OPERAND USE attributes into operand_info->ouse[k] operand info.
	// ISA_OPERAND_USE identifiers are stored as UINT32 mask (1<<id) in the
	// static ISA and in the extension ISA tables.
	// Final values in ouse[] and ruse[] arrays info are stored as mask of mUINT16.
	typedef ATTRIBUTE_MAP<UINT32> CORE_ATTR_MAP;
	typedef ATTRIBUTE_MAP<UINT32> EXT_ATTR_MAP;
	typedef ATTRIBUTE_REMAPPER<EXT_ATTR_MAP, CORE_ATTR_MAP, mUINT16, mUINT16 > EXT_ATTR_REMAPPER;
	const NAME_VALUE<UINT32> *core_attr_tab = (const NAME_VALUE<UINT32> *)ISA_OPERAND_USE_get_attribute_table();
	const NAME_VALUE<UINT32> *ext_attr_tab  = (const NAME_VALUE<UINT32> *)ext_info.ISA_tab[ext]->get_ISA_OPERAND_USE_attribute_tab();
	INT ext_attr_count = ext_info.ISA_tab[ext]->get_ISA_OPERAND_USE_attribute_tab_sz();
 	CORE_ATTR_MAP core_attr_map(core_attr_tab, ISA_OPERAND_USE_ATTRIBUTE_COUNT, CORE_ATTR_MAP::KIND_MASK);
	EXT_ATTR_MAP ext_attr_map(ext_attr_tab, ext_attr_count, EXT_ATTR_MAP::KIND_MASK);
	EXT_ATTR_REMAPPER remapper(ext_attr_map, core_attr_map);

	tgt_info = &opinfo_tab[ext_base_OPERAND_info[ext]];
	for (j=0; j<sz; j++, tgt_info++) {
	  // Update references to operand uses
	  for (k=0; k<tgt_info->opnds; k++) {
	    // Remap mUINT16 ouse info for each operand.
	    if (!remapper.remapMask(tgt_info->ouse[k], tgt_info->ouse[k]))
	      RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, 
					    "Incompatible target description,"
					    "OPERAND_USE attribute name in Extension not found in Core.");

	  }
	  for (k=0; k<tgt_info->results; k++) {
	    // Remap mUINT16 ruse info for each result.
	    if (!remapper.remapMask(tgt_info->ruse[k], tgt_info->ruse[k]))
	      RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, 
					    "Incompatible target description,"
					    "OPERAND_USE attribute name in Extension not found in Core.");
	  }
	}
      }

      // Update operand index info
      // .........................
      int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();

      const mUINT16 *ext_ii_tab = ext_info.ISA_tab[ext]->get_ISA_OPERAND_info_index_tab();
      mUINT16 base = ext_base_OPERAND_info[ext];

      for (j=0; j<top_in_ext; j++) {
	opinfo_idx_tab[ext_info.base_TOP[ext]+j] = base + ext_ii_tab[j];
      }
    }
    
    // Update targinfo pointer
    ISA_OPERAND_types_count   = nb_optypes;
    ISA_OPERAND_operand_types = optypes_tab;
    ISA_OPERAND_info          = opinfo_tab;
    ISA_OPERAND_info_index    = opinfo_idx_tab;
  }
}


/* ===========================================================================
 * Initialize ISA Subsets from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_SUBSET_op_tab   (array)
 *
 * ISA subsets are now represented as set of booleans(1/0), which simplifies
 * greatly the initialization procedure (ISA subsets were previously to be array
 * of bits).
 * ===========================================================================*/
void Initialize_ISA_Subsets(Lai_Loader_Info_t &ext_info) {
  int ext;
  int i;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    int ext_base_top = ext_info.base_TOP[ext];
    int ext_top_count = ext_info.ISA_tab[ext]->get_TOP_count();
    int ext_num_subsets = ext_info.ISA_tab[ext]->get_ISA_SUBSET_tab_sz();
    const unsigned char * const *ext_subsets_members = ext_info.ISA_tab[ext]->get_ISA_SUBSET_op_tab();
    const char * const *ext_subsets_names = ext_info.ISA_tab[ext]->get_ISA_SUBSET_tab();
    for (i = 0; i < ext_num_subsets; i++) {
      const char *subset_name = ext_subsets_names[i];
      const unsigned char *members = ext_subsets_members[i];
      ISA_SUBSET ext_subset;

      // Declare the extension subset.
      ext_subset = EXTENSION_ISA_SUBSET_New(subset_name);
      EXTENSION_ISA_SUBSET_Set_Members(ext_subset, ext_base_top, ext_top_count, members);

      // By default activate the extension subsets.
      ISA_SUBSET_LIST_Add(ISA_SUBSET_List, ext_subset);
    }
  }
}

/* ===========================================================================
 * Initialize ISA Parse from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_PARSE_tab   (array)
 *
 * ISA PARSE table is indexed with ISA_SUBSET_count.
 * ===========================================================================*/
void Initialize_ISA_Parse(Lai_Loader_Info_t &ext_info) {
  int ext;
  int i;
  int ext_num_parse = ISA_SUBSET_static_count;
  PARSER_GetParserT * extended_ISA_PARSE_tab = NULL;
  
  // Count number of parser to be dynamically added  
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    ext_num_parse += ext_info.ISA_tab[ext]->get_ISA_PARSE_tab_sz();
  }

  // Resize ISA_PARSE_tab and copy static part...
  extended_ISA_PARSE_tab = TYPE_MEM_POOL_ALLOC_N(PARSER_GetParserT, Malloc_Mem_Pool, ext_num_parse);
  memcpy(extended_ISA_PARSE_tab, ISA_PARSE_tab, ISA_SUBSET_static_count * sizeof(PARSER_GetParserT));

  // Then create dynamic part...
  ext_num_parse = ISA_SUBSET_static_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PARSE_tab()) {
       memcpy(&extended_ISA_PARSE_tab[ext_num_parse],
              ext_info.ISA_tab[ext]->get_ISA_PARSE_tab(),
              ext_info.ISA_tab[ext]->get_ISA_PARSE_tab_sz() * sizeof(PARSER_GetParserT));
    }
    ext_num_parse += ext_info.ISA_tab[ext]->get_ISA_PARSE_tab_sz();
  }

  // Switch to extended ISA_PARSE_tab  
  ISA_PARSE_tab = extended_ISA_PARSE_tab;
}

/* ===========================================================================
 * Initialize ISA Pack from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_PACK_OPND_info       (array)
 * - ISA_PACK_OPND_info_index (array)
 * - ISA_PACK_info            (array)
 * - ISA_PACK_inst_words      (array)
 * - ISA_PACK_adj_info        (array)
 * - ISA_PACK_adj_info_index  (array)
 * ===========================================================================*/
void Initialize_ISA_Pack(Lai_Loader_Info_t &ext_info) {

  ISA_PACK_OPND_INFO* new_pack_opnd_info;
  mUINT16*            new_pack_opnd_info_index;
  ISA_PACK_INFO*      new_pack_info;
  mUINT8 *            new_pack_inst_words;
  ISA_PACK_ADJ_INFO*  new_pack_adj_info;
  mUINT8*             new_pack_adj_info_index;
  int ext;
  int max_opnd_info;
  int max_adj_info;
  int renum_opnd_info;
  int renum_adj_info;

  if (TOP_count == TOP_static_count) {        // No extension top: easy work.
    return;
  }
  
  max_opnd_info = ISA_PACK_OPND_INFO_STATIC_MAX;
  max_adj_info  = ISA_PACK_ADJ_INFO_STATIC_MAX;
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    max_opnd_info += ext_info.ISA_tab[ext]->get_ISA_PACK_OPND_info_tab_sz();
    max_adj_info  += ext_info.ISA_tab[ext]->get_ISA_PACK_adj_info_tab_sz();
  }
  
  new_pack_opnd_info = TYPE_MEM_POOL_ALLOC_N(ISA_PACK_OPND_INFO,  
                                             Malloc_Mem_Pool, 
                                             max_opnd_info);
  new_pack_opnd_info_index = TYPE_MEM_POOL_ALLOC_N(mUINT16,  
                                              Malloc_Mem_Pool, 
                                              TOP_count);
  new_pack_info = TYPE_MEM_POOL_ALLOC_N(ISA_PACK_INFO,  
                                        Malloc_Mem_Pool, 
                                        TOP_count);
  new_pack_inst_words = TYPE_MEM_POOL_ALLOC_N(mUINT8,  
                                              Malloc_Mem_Pool, 
                                              TOP_count);
  new_pack_adj_info = TYPE_MEM_POOL_ALLOC_N(ISA_PACK_ADJ_INFO,  
                                            Malloc_Mem_Pool, 
                                            max_adj_info);
  new_pack_adj_info_index = TYPE_MEM_POOL_ALLOC_N(mUINT8,  
                                                  Malloc_Mem_Pool, 
                                                  TOP_count);
  // Copy of static part of tables.
  memcpy(new_pack_opnd_info, 
         ISA_PACK_OPND_info, 
         ISA_PACK_OPND_INFO_STATIC_MAX * sizeof(ISA_PACK_OPND_INFO));
  memcpy(new_pack_opnd_info_index, 
         ISA_PACK_OPND_info_index, 
         TOP_static_count * sizeof(mUINT16));
  memcpy(new_pack_info, 
         ISA_PACK_info, 
         TOP_static_count * sizeof(ISA_PACK_INFO));
  memcpy(new_pack_inst_words, 
         ISA_PACK_inst_words, 
         TOP_static_count * sizeof(mUINT8));
  memcpy(new_pack_adj_info, 
         ISA_PACK_adj_info, 
         ISA_PACK_ADJ_INFO_STATIC_MAX * sizeof(ISA_PACK_ADJ_INFO));
  memcpy(new_pack_adj_info_index, 
         ISA_PACK_adj_info_index, 
         TOP_static_count * sizeof(mUINT8));

  renum_opnd_info = ISA_PACK_OPND_INFO_STATIC_MAX;
  renum_adj_info  = ISA_PACK_ADJ_INFO_STATIC_MAX;
  for (ext=0; ext<ext_info.nb_ext; ext++) {
    int top_in_ext = ext_info.ISA_tab[ext]->get_TOP_count();
    int opnd_info_in_ext = ext_info.ISA_tab[ext]->get_ISA_PACK_OPND_info_tab_sz();
    int adj_info_in_ext = ext_info.ISA_tab[ext]->get_ISA_PACK_adj_info_tab_sz();
    int i;
    
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PACK_OPND_info_tab()) {
       memcpy(&new_pack_opnd_info[renum_opnd_info],
              ext_info.ISA_tab[ext]->get_ISA_PACK_OPND_info_tab(),
              opnd_info_in_ext * sizeof(ISA_PACK_OPND_INFO));
    }
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PACK_OPND_info_index_tab()) {
       memcpy(&new_pack_opnd_info_index[ext_info.base_TOP[ext]],
              ext_info.ISA_tab[ext]->get_ISA_PACK_OPND_info_index_tab(),
              top_in_ext * sizeof(mUINT16));
    }
    for (i=0;i<top_in_ext;i++) {
      new_pack_opnd_info_index[ext_info.base_TOP[ext]+i] += renum_opnd_info;
    }
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PACK_info_tab()) {
       memcpy(&new_pack_info[ext_info.base_TOP[ext]],
              ext_info.ISA_tab[ext]->get_ISA_PACK_info_tab(),
              top_in_ext * sizeof(ISA_PACK_INFO));
    }
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PACK_inst_words_tab()) {
       memcpy(&new_pack_inst_words[ext_info.base_TOP[ext]],
              ext_info.ISA_tab[ext]->get_ISA_PACK_inst_words_tab(),
              top_in_ext * sizeof(mUINT8));
    }
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PACK_adj_info_tab()) {
       memcpy(&new_pack_adj_info_index[renum_adj_info],
              ext_info.ISA_tab[ext]->get_ISA_PACK_adj_info_tab(),
              adj_info_in_ext * sizeof(ISA_PACK_ADJ_INFO));
    }
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_PACK_adj_info_index_tab()) {
       memcpy(&new_pack_adj_info_index[ext_info.base_TOP[ext]],
              ext_info.ISA_tab[ext]->get_ISA_PACK_adj_info_index_tab(),
              top_in_ext * sizeof(mUINT8));
    }
    for (i=0;i<top_in_ext;i++) {
      new_pack_adj_info_index[ext_info.base_TOP[ext]+i] += renum_adj_info;
    }
    renum_opnd_info += opnd_info_in_ext;
    renum_adj_info += adj_info_in_ext;
  }

  // Update pointers
  ISA_PACK_OPND_info       = new_pack_opnd_info;
  ISA_PACK_OPND_info_index = new_pack_opnd_info_index;
  ISA_PACK_info            = new_pack_info;
  ISA_PACK_inst_words      = new_pack_inst_words;
  ISA_PACK_adj_info        = new_pack_adj_info;
  ISA_PACK_adj_info_index  = new_pack_adj_info_index;
}

/* ===========================================================================
 * Initialize ISA Decode from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_Decode_Decoding      (array)
 * ===========================================================================*/
void Initialize_ISA_Decode(Lai_Loader_Info_t &ext_info) {
  int ext;
  const ISA_DECODE_DECODING ** new_decode_decoding;
  int ext_num_decode;
  
  if (TOP_count == TOP_static_count) {        // No extension top: easy work.
    return;
  }
  
  // Count number of decoder to be dynamically added  
  ext_num_decode = ISA_SUBSET_static_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    ext_num_decode += ext_info.ISA_tab[ext]->get_ISA_DECODE_decoding_tab_sz();
  }

  // Resize ISA_Decode_Decoding and copy static part...
  new_decode_decoding = TYPE_MEM_POOL_ALLOC_N(const ISA_DECODE_DECODING*, Malloc_Mem_Pool, ext_num_decode);
  memcpy(new_decode_decoding, ISA_Decode_Decoding, ISA_SUBSET_static_count * sizeof(ISA_DECODE_DECODING*));

  // Then create dynamic part...
  ext_num_decode = ISA_SUBSET_static_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_DECODE_decoding_tab()) {
       memcpy(&new_decode_decoding[ext_num_decode],ext_info.ISA_tab[ext]->get_ISA_DECODE_decoding_tab(),
              ext_info.ISA_tab[ext]->get_ISA_DECODE_decoding_tab_sz() * sizeof(ISA_DECODE_DECODING*));
    }
    ext_num_decode += ext_info.ISA_tab[ext]->get_ISA_DECODE_decoding_tab_sz();
  }

  // Switch to extended ISA_Decode_Decoding  
  ISA_Decode_Decoding = new_decode_decoding;
}

/* ===========================================================================
 * Initialize ISA Binutils from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_BINUTILS_xxx         (array)
 * ===========================================================================*/
void Initialize_ISA_Binutils(Lai_Loader_Info_t &ext_info) {
  int ext;
  int ext_num_binutils;
  ISA_BINUTILS_INFO * new_binutils_info;
  
  if (TOP_count == TOP_static_count) {        // No extension top: easy work.
    return;
  }
  
  // Count number of ISA_BINUTILS_INFO to be dynamically added  
  ext_num_binutils = ISA_SUBSET_static_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    ext_num_binutils += ext_info.ISA_tab[ext]->get_ISA_BINUTILS_info_tab_sz();
  }

  // Resize ISA_BINUTILS_info and copy static part...
  new_binutils_info = TYPE_MEM_POOL_ALLOC_N(ISA_BINUTILS_INFO, Malloc_Mem_Pool, ext_num_binutils);
  memcpy(new_binutils_info, ISA_BINUTILS_info, ISA_SUBSET_static_count * sizeof(ISA_BINUTILS_INFO));

  // Then create dynamic part...
  ext_num_binutils = ISA_SUBSET_static_count;
  for (ext=0; ext<ext_info.nb_ext; ext++) {      // Iterate on all extensions.
    if (NULL != ext_info.ISA_tab[ext]->get_ISA_BINUTILS_info_tab()) {
       memcpy(&new_binutils_info[ext_num_binutils],ext_info.ISA_tab[ext]->get_ISA_BINUTILS_info_tab(),
              ext_info.ISA_tab[ext]->get_ISA_BINUTILS_info_tab_sz() * sizeof(ISA_BINUTILS_INFO));
    }
    ext_num_binutils += ext_info.ISA_tab[ext]->get_ISA_BINUTILS_info_tab_sz();
  }

  // Switch to extended ISA_BINUTILS_info
  ISA_BINUTILS_info = new_binutils_info;
}

#ifndef BINUTILS
/* ===========================================================================
 * Initialize ISA Hazards from statically defined ones concatenated with
 * extension defined ones. Initialized data are:
 * - ISA_HAZARD_hazard_index   (array)
 *
 * ===========================================================================*/
void Initialize_ISA_Hazards(Lai_Loader_Info_t &ext_info) {

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
}
#endif

#ifndef BINUTILS
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
void Initialize_SI(Lai_Loader_Info_t &ext_info, MEM_POOL &tmp_mempool) {
  const EXTENSION_SI_Info ** ext_proc_si_info;
  int  ext, j;
  char err_msg[256];
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
  ext_proc_si_info          = TYPE_MEM_POOL_ALLOC_N(const EXTENSION_SI_Info*, &tmp_mempool, ext_info.nb_ext);
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
    // Find SI description for current processor (Always take the first one)
    ext_proc_si_info[ext] = &(ext_info.ISA_tab[ext]->get_Scheduling_Info_Access_tab()[0]);
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
      bool ok = true;

      // ...Check Resources
      int ext_res_count = ext_proc_si_info[ext]->get_SI_resource_count();
      SI_RESOURCE * const *ext_res_tab = ext_proc_si_info[ext]->get_SI_resources_tab();
      SI_RESOURCE * TI_SI_CONST* static_res_tab = Get_SI_resources();
      for (u = 0; (u<static_res_count) && (u<ext_res_count); u++) {
	if (strcmp(SI_RESOURCE_Name(ext_res_tab[u]), SI_RESOURCE_ID_Name(u))==0 ||
	    /* Backward compatibility. Allow First resource to be named Resource_stxp70_ext_ALL for STxP70 target. */
	    (u == 0 && (strcmp(proc_name, "stxp70_v3")==0 &&
			(strcmp(SI_RESOURCE_Name(ext_res_tab[u]), "Resource_stxp70_v3_ALL")==0 ||
                         strcmp(SI_RESOURCE_Name(ext_res_tab[u]), "Resource_stxp70_v3_ext_ALL")==0 ||
			 strcmp(SI_RESOURCE_Name(ext_res_tab[u]), "Resource_stxp70_ALL")==0 ||
			 strcmp(SI_RESOURCE_Name(ext_res_tab[u]), "Resource_stxp70_ext_ALL")==0)))) {
	  if ((SI_RESOURCE_Avail_Per_Cycle(ext_res_tab[u]) != SI_RESOURCE_Avail_Per_Cycle(static_res_tab[u])) ||
	      (SI_RESOURCE_Word_Index     (ext_res_tab[u]) != SI_RESOURCE_Word_Index     (static_res_tab[u])) ||
	      (SI_RESOURCE_Bit_Index      (ext_res_tab[u]) != SI_RESOURCE_Bit_Index      (static_res_tab[u]))
	      ) {
	    ok = false;
	    sprintf(err_msg,
		    "Extension incompatible with Core (resources). "
		    "Incompatible resource description %s",
		    SI_RESOURCE_ID_Name(u));
	    RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname,
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
	    (strncmp(ext_info.handler_tab[ext].extname,
		     &res_name[strlen(prefix)],
		     strlen(ext_info.handler_tab[ext].extname)) != 0)) {
	  ok = false;
	  sprintf(err_msg,
		  "Extension contains incompatible scheduling information"
		  " (unexpected resource '%s' at rank %d).",
		  res_name, u);
	  RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname,
					err_msg);
	}
      }

      // ...Check Slots
      if (ok) {
	int ext_islot_count = ext_proc_si_info[ext]->get_SI_issue_slot_count();
	if (ext_islot_count < static_islot_count) {
	  ok = false;
	  RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname,
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
		(strncmp(ext_info.handler_tab[ext].extname,
			 &slot_name[strlen(prefix)],
			 strlen(ext_info.handler_tab[ext].extname)) != 0)) {
	      ok = false;
	      sprintf(err_msg,
		      "Extension contains incompatible scheduling information"
		      " (unexpected instruction slot '%s' at rank %d).",
		      slot_name, u);
	      RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname,
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

      // Check if resource requirements can be encoded on a 64 bits word.
      // In order to remove this limitation, it would be required to
      // extend SI_RRW table (res_req#) to support multi word entries.
      // Note that the limitation is also present in RES_WORD structure
      // of the SI generator.
      if (res_tab[ext_base_res]->bit_index + nb_ext_bits > 63) {
        sprintf(err_msg,
                "Failed to load extension because maximum number"
                " of scheduling info resources has been reached");
        RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname,
                                      err_msg);
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
#endif

/* ===========================================================================
 * Initialize ISA_VARIANTS tables used by the code generator.
 * Initialized data are:
 * - ISA_VARIANT_infos    (array [TOP_Count + 1])
 * ===========================================================================*/
void Initialize_ISA_VARIANTS(Lai_Loader_Info_t &ext_info) {
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
      // Remap ISA VARIANT attributes.
      // Core and Extension variant identifiers are stored as mask (1<<id) on UINT64.
      // Variant mask on a top variant are stored as UINT64.
      typedef ATTRIBUTE_MAP<UINT64> CORE_ATTR_MAP;
      typedef ATTRIBUTE_MAP<UINT64> EXT_ATTR_MAP;
      typedef ATTRIBUTE_REMAPPER<EXT_ATTR_MAP, CORE_ATTR_MAP, UINT64, UINT64 > EXT_ATTR_REMAPPER;
      const NAME_VALUE<UINT64> *core_attr_tab = (const NAME_VALUE<UINT64> *)ISA_VARIANT_get_attribute_table();
      const NAME_VALUE<UINT64> *ext_attr_tab = (const NAME_VALUE<UINT64> *)ext_info.ISA_tab[ext]->get_ISA_VARIANT_attribute_tab();
      INT    ext_attr_count = ext_info.ISA_tab[ext]->get_ISA_VARIANT_attribute_tab_sz();
      CORE_ATTR_MAP core_attr_map(core_attr_tab, ISA_VARIANT_ATTRIBUTE_COUNT, CORE_ATTR_MAP::KIND_MASK);
      EXT_ATTR_MAP ext_attr_map(ext_attr_tab, ext_attr_count, EXT_ATTR_MAP::KIND_MASK);
      EXT_ATTR_REMAPPER remapper(ext_attr_map, core_attr_map);

      // Build extension variants entries
      for (top = 0; top < top_in_ext; top++) {
	int n_variants = ext_variant_tab[top].n_variants;
	variants_table[ext_base_TOP+top].n_variants = n_variants;
	variants_table[ext_base_TOP+top].variants = variant_ptr;
	for (var = 0; var < n_variants; var++) {
	  variant_ptr->top = ext_variant_tab[top].variants[var].top + ext_base_TOP; // convert to TOP global numbering
	  if (!remapper.remapMask(ext_variant_tab[top].variants[var].attributes, variant_ptr->attributes))
	    RaiseErrorIncompatibleLibrary(ext_info.handler_tab[ext].dllname, 
					  "Incompatible target description,"
					  "ISA_VARIANT attribute name in Extension not found in Core.");
	  variant_ptr++;
	}
      }
    }
  }

  // Update ISA_VARIANTS_infos table pointer.
  ISA_VARIANT_infos = variants_table;
}

#ifndef BINUTILS
/* ===========================================================================
 * Initialize DW_DEBUG tables used by the code generator for generation of
 * extension dwarf debug information
 * generator. Initialized data are:
 *   EXTENSION_description_tab         (array[EXTENSION_count])
 *   EXTENSION_description_reg_tab     (array[ISA_REGISTER_CLASS_MAX+1])
 * ===========================================================================*/
void Initialize_DW_DEBUG_info(Lai_Loader_Info_t &ext_info)
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
        new_tab_reg[rc2].dw_base_id = ext_tab[rc2 - rc].dw_base_id;
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
}
#endif

/* ===========================================================================
 * This function initializes all lai specific information for extension
 * description.
 * This mainly covers the targinfo description for register class,
 * TOP, operands, ...
 * ===========================================================================*/
bool Load_Lai_Loader_Info() {
  int ext, j;
  Lai_Loader_Info_t *ext_info;
  // Local heap memory
  MEM_POOL lai_loader_pool;
  char err_msg[512];

  const int nb_ext = Get_Extension_dll_handler_count( );
  if (nb_ext==0) {
    return TRUE;
  }

  MEM_POOL_Initialize(&lai_loader_pool, "lai loader temporary memory", FALSE);
  MEM_POOL_Push(&lai_loader_pool);

  // Allocate and initialize Lai specific extension table
  ext_info = TYPE_MEM_POOL_ALLOC_N(Lai_Loader_Info_t, Malloc_Mem_Pool, 1);
  global_Lai_Loader_Info_table = ext_info;

  ext_info->nb_ext    = nb_ext;
  ext_info->trace_on  = Get_Trace(TP_EXTENSION, TRACE_EXTENSION_LOADER_MASK);
  ext_info->handler_tab = Get_Extension_dll_handler_tab( );

  if (ext_info->trace_on) {
    fprintf(TFile,"%s Running RAW Targinfo Extension Loader\n%s", DBar, DBar);
  }

  // Base offset for each element of each extension
  ext_info->ISA_tab                = TYPE_MEM_POOL_ALLOC_N(EXTENSION_ISA_Info*, Malloc_Mem_Pool, nb_ext);
  ext_info->base_TOP               = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_INTRINSIC         = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext); // unused by raw targinfo loaders
  ext_info->base_LIT_CLASS         = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_ENUM_CLASS        = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_ENUM_CLASS_VALUE  = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_REGISTER_CLASS    = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
  ext_info->base_REGISTER_SUBCLASS = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
#ifndef BINUTILS
  ext_info->base_PREG              = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, nb_ext);
#endif
  
  // Compute cumulated bounds
  // ------------------------
  int nb_top    = TOP_static_count;
  int lc_max    = ISA_LC_STATIC_MAX;
  int nb_enum       = ISA_EC_STATIC_MAX;
  int nb_enum_value = ISA_ECV_STATIC_MAX;
  int rc_max    = ISA_REGISTER_CLASS_STATIC_MAX;
  int subrc_max = ISA_REGISTER_SUBCLASS_STATIC_MAX;
#ifndef BINUTILS
  int preg_max = Get_Static_Last_Dedicated_Preg_Offset();
#endif
  for (ext=0; ext < nb_ext; ext++) {

    // Get ISA Extension description accessor
    ext_info->ISA_tab[ext] = Generate_EXTENSION_ISA_Info(&(ext_info->handler_tab[ext]), ext_info->trace_on);

    ext_info->base_TOP[ext] = nb_top;
    nb_top += ext_info->ISA_tab[ext]->get_TOP_count();

    // Note: unused by raw targinfo loaders
    ext_info->base_INTRINSIC[ext] = 0;

    ext_info->base_LIT_CLASS[ext] = lc_max+1;
    ext_info->ISA_tab[ext]->set_ISA_LIT_CLASS_offset(ext_info->base_LIT_CLASS[ext]);
    lc_max += ext_info->ISA_tab[ext]->get_ISA_LIT_CLASS_info_tab_sz() - (ext_info->ISA_tab[ext]->get_ISA_LIT_CLASS_static_max()+1);
    
    ext_info->base_ENUM_CLASS[ext] = nb_enum;
    nb_enum += ext_info->ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_tab_sz();

    ext_info->base_ENUM_CLASS_VALUE[ext] = nb_enum_value;
    nb_enum_value += ext_info->ISA_tab[ext]->get_ISA_ENUM_CLASS_INFO_VALUE_tab_sz();
	
    ext_info->base_REGISTER_CLASS[ext] = rc_max+1;
    ext_info->ISA_tab[ext]->set_ISA_REGISTER_CLASS_offset(ext_info->base_REGISTER_CLASS[ext]);
    int rc_in_ext    = ext_info->ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab_sz();
    const ISA_REGISTER_CLASS_INFO *rc_tab = ext_info->ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab();
    rc_max  += rc_in_ext;
    
#ifndef BINUTILS
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
#endif

    ext_info->base_REGISTER_SUBCLASS[ext] = subrc_max+1;
    ext_info->ISA_tab[ext]->set_ISA_REGISTER_SUBCLASS_offset(ext_info->base_REGISTER_SUBCLASS[ext]);
    subrc_max += ext_info->ISA_tab[ext]->get_ISA_REGISTER_SUBCLASS_tab_sz();

    if (ext_info->trace_on) {
      fprintf(TFile, "   -> First TOP id               (if any) : %d\n", ext_info->base_TOP[ext]);
      fprintf(TFile, "   -> First INTRINSIC id         (if any) : %d\n", ext_info->base_INTRINSIC[ext]);
      fprintf(TFile, "   -> First Literal class id     (if any) : %d\n", ext_info->base_LIT_CLASS[ext]);
      fprintf(TFile, "   -> First ENUM class id        (if any) : %d\n", ext_info->base_ENUM_CLASS[ext]);
      fprintf(TFile, "   -> First Register class id    (if any) : %d\n", ext_info->base_REGISTER_CLASS[ext]);
      fprintf(TFile, "   -> First Register subclass id (if any) : %d\n", ext_info->base_REGISTER_SUBCLASS[ext]);
#ifndef BINUTILS
      fprintf(TFile, "   -> First PREG id              (if any) : %d\n", ext_info->base_PREG[ext]);
#endif
    }
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
#ifndef BINUTILS
  //TB: Reinitialize register class to preg offset
  if (preg_max != Get_Static_Last_Dedicated_Preg_Offset()) {
    Set_Last_Dedicated_Preg_Offset(preg_max);
    Reset_RegisterClass_To_Preg();
  }
#endif

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

#ifndef BINUTILS
  Initialize_ISA_Hazards(*ext_info);

  Initialize_SI(*ext_info, lai_loader_pool);
#endif

  Initialize_ISA_VARIANTS(*ext_info);

#ifndef BINUTILS
  Initialize_DW_DEBUG_info(*ext_info);
#endif
  Initialize_ISA_Parse(*ext_info);

  Initialize_ISA_Pack(*ext_info);

  Initialize_ISA_Decode(*ext_info);

  Initialize_ISA_Binutils(*ext_info);

  Initialize_ISA_Reloc(*ext_info, lai_loader_pool);

  // ==========================================================================
  // 
  // The following table are not standard targinfo information but are required
  // to support user-defined extensions
  // 
  // ==========================================================================

  // Initialize RC to regclass info table
  // ------------------------------------
  if (ext_info->trace_on) {
    fprintf(TFile, "...Initialize extension regclass info table\n");
  }
  if (ISA_REGISTER_CLASS_MAX > ISA_REGISTER_CLASS_STATIC_MAX) {
    int nb_added_rclass = ISA_REGISTER_CLASS_MAX - ISA_REGISTER_CLASS_STATIC_MAX;
    extension_regclass_table = TYPE_MEM_POOL_ALLOC_N(const EXTENSION_Regclass_Info*,
						     Malloc_Mem_Pool,
						     nb_added_rclass);
  
    for (ext=0; ext<nb_ext; ext++) {
      int rc_in_ext = ext_info->ISA_tab[ext]->get_ISA_REGISTER_CLASS_tab_sz();
      if (rc_in_ext > 0) {
	
	for (int local_rc=0; local_rc<rc_in_ext; local_rc++) {
	  int reg;
	  ISA_REGISTER_CLASS rc = local_rc + ext_info->base_REGISTER_CLASS[ext];
	  // Only allocatable regclasses have an entry in get_REGISTER_CLASS_info_tab.
	  // Non-allocatable one (typically SFR) are always the last one.
	  for (reg=0; reg<ISA_REGISTER_MAX && !ABI_PROPERTY_Is_allocatable(rc, reg); reg++) { ; }
	  if (reg < ISA_REGISTER_MAX) {
	    extension_regclass_table[rc-(ISA_REGISTER_CLASS_STATIC_MAX+1)] =
	      &(ext_info->ISA_tab[ext]->get_Regclass_Info_tab()[local_rc]);
	  }
	  else {
	    // Regclass not allocatable (typically SFR)
	    extension_regclass_table[rc-(ISA_REGISTER_CLASS_STATIC_MAX+1)] = NULL;
	  }
	}
      }
    }
  }

  if (ext_info->trace_on) {
    fprintf(TFile," Leaving RAW Targinfo Extension Loader\n%s", SBar);
  }
  
  // Cleanup
  MEM_POOL_Pop(&lai_loader_pool);
  MEM_POOL_Delete(&lai_loader_pool);

  return TRUE;
}

/*
 * Return the EXTENSION_ISA_Info associated to the parameter TOP,
 * or NULL if the TOP does not belong to an extension
 */
EXTENSION_ISA_Info *EXTENSION_Get_ISA_Info_From_TOP(TOP id) {
  int ext=0;
  if (EXTENSION_Is_Extension_TOP(id)) {
    while (ext < (global_Lai_Loader_Info_table->nb_ext-1) && 
	   id >= global_Lai_Loader_Info_table->base_TOP[ext+1]) {
      ext++;
    }
    return (global_Lai_Loader_Info_table->ISA_tab[ext]);
  }
  
  return (NULL);
}

#endif  /* defined (BACK_END) */

