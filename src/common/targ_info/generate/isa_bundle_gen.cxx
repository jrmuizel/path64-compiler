/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

   Special thanks goes to SGI for their continued support to open source

*/


// isa_bundle_gen.cxx
/////////////////////////////////////
//
//  Gneerate an interface for specifying template encoding instructions 
//  within a bundle.
//
/////////////////////////////////////
//  $Revision: 1.6 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_bundle_gen.cxx $

#if !defined(__FreeBSD__)
#include <alloca.h>
#endif
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <list>
#include <vector>
#include <map>
#include "topcode.h"
#include "targ_isa_properties.h"
#include "gen_util.h"
#include "isa_bundle_gen.h"

#ifdef TARG_ST
#ifndef DYNAMIC_CODE_GEN
#include "topcode.h"
#else
#include "dyn_isa_topcode.h"
#endif

#include "targ_isa_properties.h"
#include "gen_util.h"
#include "isa_bundle_gen.h"

// Need isa_subset to compute the max slot number allowed by isa subset
#include "targ_isa_subset.h"
// Need isa_pack to compute the slot number used by instructions
#include "targ_isa_pack.h"

#ifdef TARG_ST
#define MAX_SLOTS 32	  // max # of slots the generator can handle
#define MAX_EXEC_UNITS 64 // max # of exec units the generator can handle
#define MAX_TEMPLATES  32 // max # of templates the generator can handle
#else
#define MAX_SLOTS 4	// max # of slots the generator can handle
#define TAG_SHIFT 12    // max # of bits required to encode all the
                        // execution property types.
#endif

// In following loops, we iterate on the number of
// TOP. This number differs whether we generate
// static or dynamic TOPs.
#ifndef DYNAMIC_CODE_GEN
static TOP TOP_count_limit = TOP_static_count;
#else
static TOP TOP_count_limit = TOP_dyn_count;
#endif

/* ====================================================================
 *
 * This section handles bundle packing data types
 *
 * ====================================================================
 */

typedef enum {
  END		= 0,		// end of list marker
  FTEMPLATE	= 1,		// template + m => template m
  FSLOT		= FTEMPLATE+MAX_TEMPLATES,		// slot+n => slot n
} PACK_COMP_TYPE;

#define MAX_PACK_COMPS (FSLOT+MAX_SLOTS)

#ifdef TARG_ST
static const char * pack_comp_type_name[MAX_PACK_COMPS] = {
  "END",
  "TEMPLATE"
};
#else
static const char * const pack_comp_type_name[] = {
  "END",
  "TEMPLATE",
  "SLOT0",
  "SLOT1",
  "SLOT2",
  "SLOT3"
};
#endif

#ifdef TARG_ST
static const char * pack_comp_name[MAX_PACK_COMPS] = {
  "ISA_BUNDLE_PACK_COMP_end",
  "ISA_BUNDLE_PACK_COMP_template"
};
#else
static const char * const pack_comp_name[] = {
  "ISA_BUNDLE_PACK_COMP_end",
  "ISA_BUNDLE_PACK_COMP_template",
  "ISA_BUNDLE_PACK_COMP_slot+0",
  "ISA_BUNDLE_PACK_COMP_slot+1",
  "ISA_BUNDLE_PACK_COMP_slot+2",
  "ISA_BUNDLE_PACK_COMP_slot+3",
  "ISA_BUNDLE_PACK_COMP_slot+4",
  "ISA_BUNDLE_PACK_COMP_slot+5",
  "ISA_BUNDLE_PACK_COMP_slot+6",
  "ISA_BUNDLE_PACK_COMP_slot+7",
};
#endif

typedef struct {
  int comp_pos;
  int bundle_pos;
  int width;
} BUNDLE_FIELD;

typedef struct bundle_pack_info{
  const char *name;
  ISA_BUNDLE_PACK_ENDIAN endian;
  int bundle_size;
  int pack_index;
  BUNDLE_FIELD ftemplate[MAX_TEMPLATES];
  std::vector<BUNDLE_FIELD*> fslot[MAX_SLOTS];
} bundle_pack_info_t;

static ISA_BUNDLE_PACK_INFO current_bundle_pack_info = NULL;
static int current_template_index = 0;
static int current_pack_index = 0;
static std::list <ISA_BUNDLE_PACK_INFO> bundle_packs;

/* ====================================================================
 *
 * This section handles bundle scheduling data types
 *
 * ====================================================================
 */

struct isa_exec_unit_type {
  const char *name; 	// Name given for documentation and debugging
  int bit_position;	// bit position in flag word
  std::vector <bool> members; // set of opcodes that have this property
  ISA_EXEC_UNIT_TYPE base_unit; // base exec unit type (or null if base)
};

struct isa_bundle_type {
  const char *name;
  const char *asm_name;
#ifdef TARG_ST
  int bias, base;
#endif
  int slot_count;
  ISA_EXEC_UNIT_TYPE slot[MAX_SLOTS];
  bool stop_bit[MAX_SLOTS];
  int pack_index;
  unsigned long long pack_code;
  ISA_BUNDLE_PACK_INFO bundle_pack_info;
  // this mask can be used to check whether this bundle definition
  // is available for a given architecture.
  int valid_bundle_archi_mask;
};

#ifdef TARG_ST
struct slot_mask_type {
  long long val[MAX_SLOTS * MAX_EXEC_UNITS];
};
#endif

static int isa_exec_property_count = 0; 

static int num_bundles = 0;
static int max_slots = 0;
static int max_templates = 0;
static int max_bundle_bits;
static std::list <ISA_EXEC_UNIT_TYPE> all_exec_types; 
static std::list <ISA_BUNDLE_TYPE> all_bundles; 
static ISA_EXEC_UNIT_TYPE current_exec_type_desc;
static ISA_BUNDLE_TYPE current_bundle_desc;

// Vector indexed by isa_subset id and gives corresponding max slot number.
static std::vector<int> isa_subset_slots;

// Vector indexed by top id and gives corresponding used slot number.
static std::vector<int> top_slots;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the bundling properties. The interface is",
  " *   divided into two pieces: scheduling, and packing. The scheduling",
  " *   interface exports the following:",
  " *",
  " *   const INT ISA_BUNDLE_MAX_SLOTS",
  " *       An integer constant that indicates the maximum number of",
  " *       slots in all bundles what ever the subset. It can be used",
  " *       to fix array size.",
  " *",
  " *   const INT ISA_MAX_SLOTS",
  " *       An integer constant that indicates the maximum number of",
  " *       slots in given active subset bundles (activated by ISA_SUBSET_List).",
  " *",
  " *   const INT ISA_TAG_SHIFT",
  " *       Maximum number of bits required to encode all the execution",
  " *       property types.",
  " *",
  " *   typedef mUINTxx ISA_EXEC_UNIT_PROPERTY",
  " *       A single-bit mask of representing an execution unit.",
  " *",
  " *   typedef mUINT8 ISA_EXEC_UNIT_SLOTS",
  " *       Represents slots number for a given top.",
  " *",
  " *       The names have the form ISA_EXEC_PROPERTY_xxx",
  " *       where 'xxx' is replaced with the EXEC_UNIT_PROPERTY name.",
  " *",
  " *   typedef (enum) ISA_EXEC_UNIT",
  " *       An enumeration of the execution units.",
  " *",
  " *       The names have the form ISA_EXEC_xxx",
  " *       where 'xxx' is replaced with the EXEC_UNIT_PROPERTY name.",
  " *",
  " *       The values of ISA_EXEC_UNIT and ISA_EXEC_UNIT_PROPERTY are",
  " *       related in that the bit-mask value of an ISA_EXEC_UNIT_PROPERTY",
  " *       is equal to 2**ISA_EXEC_UNIT.",
  " *",
  " *   const INT ISA_EXEC_MAX",
  " *       The highest value ISA_EXEC_UNIT value.",
  " *",
  " *   BOOL ISA_EXEC_PROPERTY_is_xxx(TOP t) ",
  " *       Returns TRUE if EXEC_PROPERTY_is_xxx matches <t>'s property.",
  " *",
  " *   ISA_EXEC_UNIT_PROPERTY ISA_EXEC_Unit_Prop(TOP topcode)",
  " *       Returns exec_unit_property for the instruction specified",
  " *       by <topcode>.",
  " *",
  " *   ISA_EXEC_UNIT_SLOTS ISA_EXEC_Unit_Slots(TOP topcode)",
  " *       Returns slot number of the given top. I.e. a single instruction can be splitted",
  " *       on several slots.",
  " *       by <topcode>.",
  " *",
  " *   UINT32 ISA_EXEC_Stop_Mask(INT bundle)",
  " *       Return stop_mask for <bundle>.",
  " *",
  " *   UINT8 ISA_EXEC_Pack_Index(INT bundle)",
  " *       Return stop_mask for <bundle>.",
  " *",
  " *   UINT64 ISA_EXEC_Pack_Code(INT bundle)",
  " *       Return pack_code for <bundle>.",
  " *",
  " *   ISA_BUNDLE_INFO ISA_EXEC_Bundle_Info(INT index)",
  " *       Return isa_bundle_info specified by <index>. ",
  " *",
  " *   ISA_EXEC_UNIT_PROPERTY ISA_EXEC_Slot_Prop(INT bundle, INT slot_index)",
  " *       Return exec_unit_property for the slot position <slot_index>",
  " *       in <bundle>.",
  " *",
#ifdef TARG_ST
  " *   INT ISA_EXEC_Slot_Count(INT bundle)",
  " *       Return the number of slots in <bundle>.",
  " *",
  " *   ISA_EXEC_MASK ISA_EXEC_Slot_Mask(INT bundle)",
#else
  " *   UINT64 ISA_EXEC_Slot_Mask(INT bundle)",
#endif
  " *       Return slot_mask for <bundle>.",
  " *",
  " *   BOOL ISA_EXEC_Stop(INT bundle, INT slot_index)",
  " *       Return stop bit for the slot position <slot_index> in <bundle>.",
  " *",
  " *   ISA_EXEC_UNIT ISA_EXEC_Unit(INT bundle, INT slot_index)",
  " *       Return the execution unit slot position <slot_index> in <bundle>.",
  " *",
  " *   const char *ISA_EXEC_Name(INT bundle)",
  " *       Return the name for <bundle>.",
  " *",
  " *   const char *ISA_EXEC_AsmName(INT bundle)",
  " *       Return the assembly language name for <bundle>.",
  " *",
  " * ====================================================================",
  " *",
  " *   The packing interface exports the following:",
  " *",
  " *   typedef ISA_BUNDLE",
  " *       A type large enough to hold a bundle. This type will always",
  " *       be a struct containing an array of either 32-, or 64-bit",
  " *       unsigned integers.",
  " *",
  " *   typedef (enum) ISA_BUNDLE_PACK_COMP",
  " *       An enumeration of the bundle components to be packed.",
  " *",
  " *   const INT ISA_BUNDLE_PACK_COMP_MAX",
  " *       The maximum number of components to be packed for a bundle.",
  " *",
  " *   typedef (struct) ISA_BUNDLE_PACK_INFO",
  " *       Describes how a the components of a bundle are packed.",
  " *       The contents are private.",
  " *",
  " *   const ISA_BUNDLE_PACK_INFO *ISA_BUNDLE_Pack_Info(void)",
  " *       Returns a pointer to the first packing component.",
  " *       Increment the returned pointer to access any additional packing",
  " *       components for the bundle. A component of ISA_PACK_COMP_end",
  " *       marks the end.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_Comp(const ISA_BUNDLE_PACK_INFO *info)",
  " *       Identifies the bundle component to be packed.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_Index(const ISA_BUNDLE_PACK_INFO *info)",
  " *       The index of the bundle word containing the component.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_Index is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_CompPos(const ISA_BUNDLE_PACK_INFO *info)",
  " *       The offset, in bits, to the start of the component in the",
  " *       component value.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_CompPos is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_BundlePos(const ISA_BUNDLE_PACK_INFO *info)",
  " *       The offset, in bits, to the start of the component in the",
  " *       bundle word.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_BundlePos is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   UINT64 ISA_BUNDLE_PACK_INFO_Mask(const ISA_BUNDLE_PACK_INFO *info)",
  " *       A bit mask that is as wide as the bundle component being",
  " *       packed. The mask is shifted to match the field in the",
  " *       bundle word.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_Mask is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   INT ISA_BUNDLE_Pack_Info_Index(ISA_BUNDLE_PACK_COMP comp)",
  " *       Index into bundle packing info array (see ISA_BUNDLE_Pack_Info)",
  " *       to the start of the info for the component <comp>. If there",
  " *       is no packing info for <comp>, the index is for the 'end'",
  " *       component.",
  " *",
  " * ====================================================================",
  " *   Management of dynamic extensions",
  " *",
  " *   The assumption that has been made so far, is that extensions don't",
  " *   modify the bundling and packing properties. Adding new TOP don't",
  " *   modify the description of bundles (in particular, we assume",
  " *   that it doesn't introduce new bundles). The only thing that has",
  " *   to be done, is to specify in which bundles each TOP can enter.",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

/* ====================================================================
 *
 * This section handles bundle scheduling
 *
 * ====================================================================
 */

/////////////////////////////////////
ISA_EXEC_UNIT_TYPE ISA_Exec_Unit_Type_Create ( const char* name,
					       ISA_EXEC_UNIT_TYPE base_unit )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_EXEC_UNIT_TYPE cur_type = new isa_exec_unit_type;

  cur_type->name = name;
  cur_type->bit_position = isa_exec_property_count++;
  cur_type->members = std::vector<bool> (TOP_count_limit, false);
  cur_type->base_unit = base_unit;

  current_exec_type_desc = cur_type;
  all_exec_types.push_back (current_exec_type_desc);
  return cur_type;
}

/////////////////////////////////////
void Instruction_Exec_Unit_Group(ISA_EXEC_UNIT_TYPE unit_type, ... )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  TOP opcode;

  // Last TOP in variable length enumeration is not the
  // same if we are in the static case or in the dynamic one.
  TOP TOP_limit = Is_Static_Code() ? TOP_UNDEFINED : static_cast<TOP>(-1);

  if (!current_exec_type_desc->name) {
    fprintf(stderr,"### Error: no execution unit type name specified for %s\n",
                   current_exec_type_desc->name);
    exit(EXIT_FAILURE);
  }
 
  va_start(ap, unit_type);
  while ( (opcode = static_cast<TOP>(va_arg(ap, int))) != TOP_limit) {
      unit_type->members[(int)opcode] = true;
  }
  va_end(ap);  
}

/////////////////////////////////////
void ISA_BUNDLE_Dyn_Set_Slot_Count ( int slot_count ) 
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int isa_subset;
  
  if (Is_Static_Code()) {
    fprintf(stderr, "### Error: ISA_BUNDLE_Dyn_Set_Slot_Count reserved for dynamic reconfigurability\n");
    exit(EXIT_FAILURE);
  }
  for(isa_subset = ISA_SUBSET_MIN; isa_subset <= ISA_SUBSET_MAX; isa_subset++) {
    isa_subset_slots[isa_subset] = slot_count;
  }
}

/////////////////////////////////////
void ISA_Bundle_Type_Create (const char* name, const char* asm_name, 
			     int no_slots, ISA_BUNDLE_PACK_INFO bundle_pack_info,
                             unsigned long long pattern, int bundle_archi_mask
 )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int i;
  ISA_BUNDLE_TYPE cur_type = new isa_bundle_type;
  cur_type->name = name;
  cur_type->asm_name = asm_name;
#ifdef TARG_ST
  cur_type->bias = 0;
  cur_type->base = 8;
  cur_type->valid_bundle_archi_mask = bundle_archi_mask;
#endif
  cur_type->slot_count = no_slots;
  cur_type->pack_code = pattern;
  cur_type->pack_index = bundle_pack_info->pack_index;
  cur_type->bundle_pack_info = bundle_pack_info;
  for (i = 0; i < no_slots; ++i) cur_type->stop_bit[i] = false;

  current_bundle_desc = cur_type;
  all_bundles.push_back (current_bundle_desc);

  if (no_slots > max_slots) max_slots = no_slots;
  ++num_bundles;
}

//
// Update isa_subset max slot number
//
static void
update_isa_subset_slots( ISA_EXEC_UNIT_TYPE type, ISA_BUNDLE_TYPE bundle_type ) {

  int top;

  for(top = 0; top < type->members.size(); top++) {
    if(type->members[top]) {
      int isa_subset;
      for(isa_subset = ISA_SUBSET_MIN; isa_subset <= ISA_SUBSET_MAX; isa_subset++) {
	if(ISA_SUBSET_Member(isa_subset,top)) {
	  if(bundle_type->slot_count > isa_subset_slots[isa_subset]) {
	    isa_subset_slots[isa_subset] = bundle_type->slot_count;
	  }
	}
      }
    }
  }
}

/////////////////////////////////////
void Slot (int slot_index, ISA_EXEC_UNIT_TYPE type)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (slot_index > current_bundle_desc->slot_count) {
    fprintf(stderr, "### Error: slot index (%d) exceeds %d\n",
		    slot_index, current_bundle_desc->slot_count);
    exit(EXIT_FAILURE);
  }

  if (!type) {
    fprintf(stderr, "### Error: slot type have non NULL value \n");
    exit(EXIT_FAILURE);
  }

  current_bundle_desc->slot[slot_index] = type;

  update_isa_subset_slots(type,current_bundle_desc);
}

#ifdef TARG_ST
/////////////////////////////////////
void Alignment (int bias, int base)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  current_bundle_desc->bias = bias;
  current_bundle_desc->base = base;
}
#endif

/////////////////////////////////////
void Stop (int slot_index)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (slot_index > current_bundle_desc->slot_count) {
    fprintf(stderr, "### Error: slot index (%d) exceeds %d\n",
		    slot_index, current_bundle_desc->slot_count);
    exit(EXIT_FAILURE);
  }

  current_bundle_desc->stop_bit[slot_index] = true;
}

#ifdef TARG_ST
static slot_mask_type slot_mask_shift_or (slot_mask_type slot_mask,
					  unsigned long long value, int shift)
{
  int word = shift / 64;
  int w_shift = shift - (word * 64);
  slot_mask.val[word] |= value << w_shift;
  if (w_shift > 0) {
    slot_mask.val[word+1] |= value >> (64 - w_shift);
  }
  return slot_mask;
}
#endif
 
// Track warning to print them only once.
std::map<int,ISA_EXEC_UNIT_TYPE> coding_size_warnings;

//
// Returns the exec unit coding size
//
static int
get_exec_unit_size( ISA_EXEC_UNIT_TYPE type ) {

  int top;
  int exec_unit_size = 0;
  for(top = 0; top < type->members.size(); top++) {
    if(type->members[top]) {
      int top_size = ISA_PACK_INST_WORD_SIZE * ISA_PACK_Inst_Words(top);
      if(exec_unit_size != 0 && top_size != 0 && exec_unit_size != top_size) {
          std::map<int,ISA_EXEC_UNIT_TYPE>::iterator warning;
	if(!((warning = coding_size_warnings.find(top)) != coding_size_warnings.end() &&
	     warning->second == type)) {
	  fprintf(stderr,"### Warning: Not all instructions have the same coding size in exec unit %s (%s: %d <-> TOP_%s: %d)\n",
		  type->name, type->name, exec_unit_size, TOP_Name(top), top_size);
	}
	coding_size_warnings[top] = type;
      }
      else {
	if(top_size != 0) {
	  exec_unit_size = top_size;
	}
      }
    }
  }

  return exec_unit_size;
}

//
// Returns the exec unit coding size
//
static int
get_slot_size( int slot, ISA_BUNDLE_PACK_INFO bundle_pack_info ) {
  if(!(slot < MAX_SLOTS)) {
    fprintf(stderr,"### Error: slot %d higher than MAX_SLOTS (%d)\n",
	    slot,MAX_SLOTS);
  }

  int slot_size = 0;
  for(std::vector<BUNDLE_FIELD*>::iterator bf = bundle_pack_info->fslot[slot].begin();
      bf != bundle_pack_info->fslot[slot].end(); bf++) {
    slot_size += (*bf)->width;
  }

  slot_size = slot_size & 7 ? (slot_size & ~7) + 8 : slot_size; 

  return slot_size;
}

//
// Updates top slots present in the exec unit

static void
update_top_slots( ISA_EXEC_UNIT_TYPE type, int slots, ISA_BUNDLE_TYPE bundle_type ) {

  int top;
  int last_top = -1;

  for(top = 0; top < type->members.size(); top++) {
    if(type->members[top]) {
      if(top_slots[top]) {
	if(top_slots[top] != slots) {
	  if(last_top != -1) {
	    fprintf(stderr,"### Error: missmatch on TOP slot number on bundle pack used by bundle type %s (TOP_%s: %d <-> TOP_%s: %d)\n",
		    bundle_type->name, TOP_Name(last_top), top_slots[last_top],  TOP_Name(top), top_slots[top]);
	  }
	  else {
	    fprintf(stderr,"### Error: missmatch on TOP slot number on bundle pack used by bundle type %s (TOP_%s: %d <-> %d)\n",
		    bundle_type->name, TOP_Name(top), top_slots[top],slots);
	  }
	  exit(-1);
	}
	else {
	  last_top = top;
	}
      }
      top_slots[top] = slots;
    }
  }
}

static bool
subset_has_tops(ISA_SUBSET isa_subset, ISA_BUNDLE_TYPE bundle_type)
{
  for(int slot = 0; slot < bundle_type->slot_count; slot++) {
    ISA_EXEC_UNIT_TYPE unit_type = bundle_type->slot[slot];
    int top;

    for(top = 0; top < unit_type->members.size(); top++) {
      if(!TOP_is_dummy(top) &&
	 !TOP_is_simulated(top) &&
	 unit_type->members[top] && ISA_SUBSET_Member(isa_subset,top)) {
	return true;
      }
    }
  }

  return false;
}

//
// Update top_slots vector that contains the necessary number of slot
// by top.
//
static void
get_top_slots( void )
{
    std::list<ISA_BUNDLE_TYPE>::iterator bundle_type;

  if(all_bundles.size() == 0) {
    // If no bundle packing specified, consider one slot per top
    int top;
    for(top = 0; top < TOP_count_limit; top++) {
      top_slots[top] = 1;
    }
    return;
  }

  for (bundle_type = all_bundles.begin();
       bundle_type != all_bundles.end(); ++bundle_type) {
    ISA_BUNDLE_TYPE current_bundle_type = *bundle_type;
    ISA_BUNDLE_PACK_INFO bundle_pack_info = current_bundle_type->bundle_pack_info;
    int exec_unit_size = 0;
    ISA_EXEC_UNIT_TYPE current_exec_unit;
    int slot_nb;
    int slot;

    for(slot = 0; slot < current_bundle_type->slot_count; slot++) {
      ISA_EXEC_UNIT_TYPE exec_unit = current_bundle_type->slot[slot];
      if(exec_unit_size == 0) {
	slot_nb = 0;
	exec_unit_size = get_exec_unit_size(exec_unit);
	current_exec_unit = exec_unit;
      }
      else {
	if(current_exec_unit != exec_unit) {
	  fprintf(stderr,"### Error: Bundle: %s: unsupported not contiguous multiple slots for exec unit %s <-> %s\n",
		  current_bundle_type->name,current_exec_unit->name,exec_unit->name);
	  exit(-1);
	}
      }

      int slot_size = get_slot_size( slot, bundle_pack_info );
      exec_unit_size -= slot_size;

      if(exec_unit_size < 0) {
	fprintf(stderr,"### Error: Bundle %s: missmatch between exec unit %s size and template slot sizes\n",
		current_bundle_type->name,current_exec_unit->name);
	exit(-1);
      }

      slot_nb++;

      if(exec_unit_size == 0) {
	update_top_slots(current_exec_unit,slot_nb,current_bundle_type);
      }
    }
  }
}

/////////////////////////////////////
static void Emit_Bundle_Scheduling(FILE *hfile, FILE *cfile, FILE *efile)
/////////////////////////////////////
//  Emit the bundle scheduling interface.
/////////////////////////////////////
{
    std::list<ISA_EXEC_UNIT_TYPE>::iterator iei;
    std::list<ISA_BUNDLE_TYPE>::iterator ibi;
  int i;
  bool gen_static_code = Is_Static_Code();   // "static" or "dynamic" TOP.
  const char *tabname;

  //  const char *isa_exec_type_format = "  %3d,  /* %s: ";
  const char *info_index_type;
#ifdef TARG_ST
  const int tag_shift = isa_exec_property_count;
  const int slot_mask_words = ((tag_shift * max_slots) + 63) / 64;
#endif

  get_top_slots();

  int index = 0;
  for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++index,
								  ++iei) {
  }

  char *int_suffix;
  // select the ISA_EXEC_unit_prop based on the number of exec info types.
  if (index <= 8) {
    info_index_type = "mUINT8";
    int_suffix = const_cast<char*>("");
  } else if (index <= 16) {
    info_index_type = "mUINT16";
    int_suffix = const_cast<char*>("");
  } else if (index <= 32) {
    info_index_type = "mUINT32";
    int_suffix = const_cast<char*>("U");
  } else {
    assert (index <= 64);
    info_index_type = "mUINT64";
    int_suffix = const_cast<char*>("ULL");
  }

  
  if(gen_static_code) {
    fprintf (hfile, "BE_EXPORTED extern int Isa_Max_Slots;\n");
    fprintf (hfile, "BE_EXPORTED extern int get_Max_Slot();\n");
    fprintf (hfile, "#define ISA_MAX_SLOTS (Isa_Max_Slots == -1 ? (Isa_Max_Slots = get_Max_Slot()) : Isa_Max_Slots)\n");
    fprintf (hfile, "#define ISA_BUNDLE_MAX_SLOTS (%d)\n", max_slots);
#ifdef TARG_ST
    fprintf (hfile, "#define ISA_TAG_SHIFT (%d)\n", tag_shift);
    fprintf (hfile, "#define ISA_SLOT_MASK_WORDS (%d)\n", slot_mask_words);
#else
    fprintf (hfile, "#define ISA_TAG_SHIFT (%d)\n", TAG_SHIFT);
#endif
#ifdef TARG_STxP70
    // TEMPO PATCH: Max inst Bytes in v3 must be equal to 4.
    // This value should not be shared between v3 and v4.
    fprintf (hfile, "BE_EXPORTED extern int Isa_Max_Inst_Bytes;\n");
    fprintf (hfile, "BE_EXPORTED extern int get_Max_Inst_Bytes();\n");
    fprintf (hfile, "#define ISA_MAX_INST_BYTES (Isa_Max_Inst_Bytes == -1? (Isa_Max_Inst_Bytes = get_Max_Inst_Bytes()) : Isa_Max_Inst_Bytes)\n");

    fprintf (cfile,"\nint Isa_Max_Inst_Bytes = -1;\n");
    fprintf (cfile,
	     "\nint get_Max_Inst_Bytes(void)\n"
	     "{\n"
	     "  int isa_subset;\n"
	     "  for(isa_subset = ISA_SUBSET_MIN; isa_subset <= ISA_SUBSET_MAX; isa_subset++) {\n"
	     "    if(ISA_SUBSET_LIST_Contains(ISA_SUBSET_List,isa_subset)) {\n"
	     "      if(!strncmp(\"stxp70_v3\", ISA_SUBSET_Name(isa_subset), 9)) {\n"
	     "        return 4; /* Hardcoded value for v3 */\n"
	     "      }\n"
	     "    }\n"
	     "  }\n"
	     "  return %d /* Computed value for v4 */;\n"
	     "}\n", max_bundle_bits/8);
#else
    fprintf (hfile, "#define ISA_MAX_INST_BYTES (%d)\n", max_bundle_bits/8);
#endif
    
    fprintf (hfile, "\ntypedef %s ISA_EXEC_UNIT_PROPERTY;\n",
             info_index_type);

    fprintf (hfile, "\n/* Number of slots used by exec unit */\n");
    fprintf (hfile, "typedef mUINT8 ISA_EXEC_UNIT_SLOTS;\n");

    fprintf (hfile, "\n");
    for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
      ISA_EXEC_UNIT_TYPE curr_exec_type = *iei;
      fprintf (hfile, "#define ISA_EXEC_PROPERTY_%-15s (" PRINTF_LONGLONG_FORMAT( "0x", "", "x" ) "%s)\n",
                       curr_exec_type->name,
                       (1ULL << curr_exec_type->bit_position), int_suffix);
    }

    fprintf (hfile, "\ntypedef enum {\n");
    for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
      ISA_EXEC_UNIT_TYPE curr_exec_type = *iei;
      fprintf (hfile, "  ISA_EXEC_%-15s = %d,\n",
                      curr_exec_type->name,
                      curr_exec_type->bit_position);
    }
    fprintf (hfile, "  ISA_EXEC_%-15s = %d\n"
                    "} ISA_EXEC_UNIT;\n",
                    "MAX", isa_exec_property_count - 1);

#ifdef TARG_ST
    fprintf (hfile, "\ntypedef struct {\n"
                    "  mUINT64 v[%d];\n"
                    "} ISA_EXEC_MASK;\n",
                    slot_mask_words);
#endif

    fprintf (hfile, "\ntypedef struct {\n"
                    "  const char *name;\n"
                    "  const char *asm_name;\n"
#ifdef TARG_ST
                   "  int bias, base;\n"
#endif
                   "  int slot_count;\n"
                   "  ISA_EXEC_UNIT_PROPERTY slot[%d];\n"
                   "  mBOOL stop[%d];\n"
                   "  mUINT8 unit[%d];\n"
                   "  mUINT8 stop_mask;\n"
	           "  mUINT8 pack_index;\n"
                   "  mUINT64 pack_code;\n"
#ifdef TARG_ST
                   "  ISA_EXEC_MASK slot_mask;\n"
                   "  mUINT32 valid_bundle_archi_mask;\n"
#else
                   "  mUINT64 slot_mask;\n"
#endif
                   "} ISA_BUNDLE_INFO;\n",
                   max_slots ? max_slots : 1,
                   max_slots ? max_slots : 1,
                   max_slots ? max_slots : 1);
  }                                  // if(gen_static_code)

  ///////////////////////////////////////////////////////////
  // Building ISA_BUNDLE_info table
  ///////////////////////////////////////////////////////////

  if(gen_static_code) {
  fprintf(efile, "ISA_BUNDLE_info\n");

  fprintf(cfile, "\nBE_EXPORTED const ISA_BUNDLE_INFO ISA_BUNDLE_info[] = {\n");

#ifndef TARG_ST
  int slot_mask_digits = ((TAG_SHIFT * max_slots) + 3) / 4;
#endif
  for (ibi = all_bundles.begin(); ibi != all_bundles.end(); ++ibi) {
    ISA_BUNDLE_TYPE curr_exec_type = *ibi;
#ifdef TARG_ST
    fprintf (cfile,
	     " {\n"
	     "    \"%s\", /* name */\n"
	     "    \"%s\", /* asm_name */\n"
	     "    %d, /* bias */\n"
	     "    %d, /* base */\n"
	     "    %d, /* slot_count */\n", 
	     curr_exec_type->name, 
	     curr_exec_type->asm_name, 
	     curr_exec_type->bias,
	     curr_exec_type->base,
	     curr_exec_type->slot_count);
#else
    fprintf (cfile, " {\n    \"%s\",%*s \"%s\",%*s %d,", 
		    curr_exec_type->name, 
		    13 - strlen(curr_exec_type->name), "",
		    curr_exec_type->asm_name, 
		    8 - strlen(curr_exec_type->asm_name), "",
		    curr_exec_type->slot_count);
#endif

#ifdef TARG_ST
    slot_mask_type slot_mask = { 0 };
#else
    unsigned long long slot_mask = 0;
#endif
    unsigned int stop_mask = 0;
    fprintf (cfile, "\n    {");
    for (i = 0; i < curr_exec_type->slot_count; i++) {
      unsigned int flag_value = 1 << curr_exec_type->slot[i]->bit_position;
      int shift_count = max_slots - i - 1;
#ifdef TARG_ST
      slot_mask = slot_mask_shift_or(slot_mask, flag_value, tag_shift * shift_count);
#else
      slot_mask |= ((unsigned long long)flag_value << (TAG_SHIFT * shift_count));
#endif
      stop_mask |= (curr_exec_type->stop_bit[i] << shift_count);
      fprintf (cfile, " %2d /* %7s */,", 
		      flag_value,
		      curr_exec_type->slot[i]->name);
    }
    fprintf (cfile, " },");

    fprintf (cfile, "\n    {");
    for (i = 0; i < max_slots; i++) {
      fprintf (cfile, " %5s,", curr_exec_type->stop_bit[i] ? "TRUE" : "FALSE");
    }
    fprintf (cfile, " }, /* stop */");

    fprintf (cfile, "\n    {");
    for (i = 0; i < curr_exec_type->slot_count; i++) {
      ISA_EXEC_UNIT_TYPE unit_type = curr_exec_type->slot[i];
      if (unit_type->base_unit) unit_type = unit_type->base_unit;
      fprintf (cfile, " ISA_EXEC_%5s,", unit_type->name);
    }
    fprintf (cfile, " },\n");
    fprintf(cfile, "    0x%1x, /* stop_mask */\n", stop_mask);
    fprintf(cfile, "    %2d, /* pack_index */\n", curr_exec_type->pack_index);
    fprintf(cfile, "    " PRINTF_LONGLONG_HEXA ", /* pack_code */\n", curr_exec_type->pack_code);
#ifdef TARG_ST
    fprintf(cfile, "    { " PRINTF_LONGLONG_HEXA "", slot_mask.val[0]);
    for (i = 1; i < slot_mask_words; i++) {
      fprintf(cfile, ", " PRINTF_LONGLONG_HEXA "", slot_mask.val[i]);
    }
    fprintf(cfile, " }\n /* slot_mask */     , %d /* valid_bundle_archi_mask */},\n", curr_exec_type->valid_bundle_archi_mask);

#else
    fprintf(cfile, " " PRINTF_LONGLONG_FORMAT( "0x", "0*", "xULL" ) "\n  },\n", slot_mask_digits, slot_mask);
#endif
  }
#ifdef TARG_ST
  fprintf (cfile, "  {\n    \"template_MAX\", \"\", 0, 1, 0,\n    { -1 /* ??????? */");
#else
  fprintf (cfile, "  {\n    \"template_MAX\", \"\", -1,\n    { -1 /* ??????? */");
#endif
  for (i = 1; i < max_slots; ++i) fprintf (cfile, ", -1 /* ??????? */");
  fprintf (cfile, ",},\n    { FALSE");
  for (i = 1; i < max_slots; ++i) fprintf (cfile, ", FALSE");
#ifdef TARG_ST
  fprintf (cfile, "},\n    { -1 /* ??????? */");
  for (i = 1; i < max_slots; ++i) fprintf (cfile, ", -1 /* ??????? */");
  fprintf (cfile, "},\n     -1, 0x0, 0x0ULL, { 0x00000000 }\n  }\n};\n");
#else
  fprintf (cfile, ",},\n    -1, 0x0, 0x%0*x\n  }\n};\n", slot_mask_digits, 0);
#endif
  fprintf(hfile,"\n#define ISA_MAX_BUNDLES %d\n",num_bundles);

  }               // gen_static_code

  /////////////////////////////////////////////////////////
  // Building ISA_EXEC_unit_prop table
  
  tabname = gen_static_code ? "ISA_EXEC_unit_prop_static" :
                              "ISA_EXEC_unit_prop_dynamic";

  fprintf (cfile, "\nstatic const ISA_EXEC_UNIT_PROPERTY %s[%d] = {\n",
	  tabname,TOP_count_limit);
  for (unsigned int top = 0; top < TOP_count_limit; ++top) {
    bool is_first   = true;

    fprintf(cfile,"  /* %-30s */   ",TOP_Name((TOP)top));

    for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
      ISA_EXEC_UNIT_TYPE exec_type = *iei;
      if (exec_type->members[top]) {

        if(!is_first)
          fprintf(cfile," | ");   // Add bitwise or operator.
        else
          is_first = false;

        fprintf(cfile,"ISA_EXEC_PROPERTY_%-15s",exec_type->name);
      }
    }

    if(true==is_first)           // No property detected
      fprintf(cfile,"0x0");

    fprintf(cfile, ",\n");       // Add comma to prepare next table item

  }
  fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(cfile,
      "\n"
      "const ISA_EXEC_UNIT_PROPERTY *ISA_EXEC_unit_prop = %s;\n"
      "\n",
      tabname);

    fprintf(hfile, 
      "\n"
      "BE_EXPORTED extern const ISA_EXEC_UNIT_PROPERTY *ISA_EXEC_unit_prop;\n"
      "\n");

    fprintf (efile, "ISA_EXEC_unit_prop\n");
  }
  else {
    fprintf(cfile,
    "\n"
    "const ISA_EXEC_UNIT_PROPERTY *dyn_get_ISA_EXEC_unit_prop_tab ( void )\n"
    "{  return %s;\n"
    "}\n"
    "\n",
    tabname);

    fprintf(hfile,
    "\n"
    "extern const ISA_EXEC_UNIT_PROPERTY *dyn_get_ISA_EXEC_unit_prop_tab(void);\n"
     );
  }                              // if(gen_static_code) then else

  // End of table ISA_EXEC_Unit_prop
  /////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////
  // Building ISA_EXEC_unit_slots table
  
  tabname = gen_static_code ? "ISA_EXEC_unit_slots_static" :
                              "ISA_EXEC_unit_slots_dynamic";

  fprintf (cfile, "\nstatic const ISA_EXEC_UNIT_SLOTS %s[%d] = {\n",
	  tabname,TOP_count_limit);
  for (unsigned int top = 0; top < TOP_count_limit; ++top) {
    bool found_top   = false;

    fprintf(cfile,"  /* %-30s */   ",TOP_Name((TOP)top));

    for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
      ISA_EXEC_UNIT_TYPE exec_type = *iei;
      if (exec_type->members[top]) {
	fprintf(cfile,"%d",top_slots[top]);
	found_top = true;
      }
    }

    if(!found_top) {
      // No slot needed
      fprintf(cfile,"0");
    }

    fprintf(cfile, ",\n");       // Add comma to prepare next table item

  }
  fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(cfile,
      "\n"
      "const ISA_EXEC_UNIT_SLOTS *ISA_EXEC_unit_slots = %s;\n"
      "\n",
      tabname);

    fprintf(hfile, 
      "\n"
      "BE_EXPORTED extern const ISA_EXEC_UNIT_SLOTS *ISA_EXEC_unit_slots;\n"
      "\n");

    fprintf (efile, "ISA_EXEC_unit_slots\n");
  }
  else {
    fprintf(cfile,
    "\n"
    "const ISA_EXEC_UNIT_SLOTS *dyn_get_ISA_EXEC_unit_slots_tab ( void )\n"
    "{  return %s;\n"
    "}\n"
    "\n",
    tabname);

    fprintf(hfile,
    "\n"
    "extern const ISA_EXEC_UNIT_SLOTS *dyn_get_ISA_EXEC_unit_slots_tab(void);\n"
     );
  }                              // if(gen_static_code) then else

  // End of table ISA_EXEC_Unit_slots
  /////////////////////////////////////////////////////////

  if(gen_static_code) {
    for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
      ISA_EXEC_UNIT_TYPE exec_type = *iei;
      fprintf(hfile,
             "#define EXEC_PROPERTY_is_%s(t)\t (ISA_EXEC_unit_prop[(INT)t] & ISA_EXEC_PROPERTY_%s)\n",
             exec_type->name, exec_type->name);
    }
  }                               // gen_static_code

  if(gen_static_code) {
  fprintf (hfile, "\ninline ISA_EXEC_UNIT_PROPERTY "
                   "ISA_EXEC_Unit_Prop(TOP topcode)\n"
                 "{\n"
                 "  return ISA_EXEC_unit_prop[(INT)topcode];\n"
                 "}\n");
		   
  fprintf (hfile, "\ninline ISA_EXEC_UNIT_SLOTS "
                   "ISA_EXEC_Unit_Slots(TOP topcode)\n"
                 "{\n"
                 "  return ISA_EXEC_unit_slots[(INT)topcode];\n"
                 "}\n");
		   
  fprintf (hfile, "\ninline ISA_BUNDLE_INFO "
                   "ISA_EXEC_Bundle_Info(INT index)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
                 "  return ISA_BUNDLE_info[index];\n"
                 "}\n");
		   
  fprintf (hfile, "\ninline ISA_EXEC_UNIT_PROPERTY "
                   "ISA_EXEC_Slot_Prop(INT bundle, INT slot_index)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->slot[slot_index];\n"
                 "}\n");

#ifdef TARG_ST
  fprintf (hfile, "\ninline INT "
                   "ISA_EXEC_BUNDLE_activated(INT bundle, PROCESSOR proc)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return (info->valid_bundle_archi_mask & (1<<proc));\n"
                 "}\n");

  fprintf (hfile, "\ninline INT "
                   "ISA_EXEC_Slot_Count(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->slot_count;\n"
                 "}\n");

  fprintf (hfile, "\ninline ISA_EXEC_MASK ");
#else
  fprintf (hfile, "\ninline UINT64 ");
#endif
  fprintf (hfile, "ISA_EXEC_Slot_Mask(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->slot_mask;\n"
                 "}\n");

  fprintf (hfile, "\ninline BOOL "
                   "ISA_EXEC_Stop(INT bundle, INT slot_index)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->stop[slot_index];\n"
                 "}\n");

  fprintf (hfile, "\ninline ISA_EXEC_UNIT "
                   "ISA_EXEC_Unit(INT bundle, INT slot_index)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return (ISA_EXEC_UNIT)info->unit[slot_index];\n"
                 "}\n");

  fprintf (hfile, "\ninline UINT32 "
                   "ISA_EXEC_Stop_Mask(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->stop_mask;\n"
                 "}\n");

  fprintf (hfile, "\ninline UINT8 "
                   "ISA_EXEC_Pack_Index(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->pack_index;\n"
                 "}\n");

  fprintf (hfile, "\ninline UINT64 "
                   "ISA_EXEC_Pack_Code(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->pack_code;\n"
                 "}\n");

  fprintf (hfile, "\ninline const char * "
                   "ISA_EXEC_Name(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->name;\n"
                 "}\n");

  fprintf (hfile, "\ninline const char * "
                   "ISA_EXEC_AsmName(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->asm_name;\n"
                 "}\n");
#ifdef TARG_ST
  fprintf (hfile, "\ninline INT "
                   "ISA_EXEC_Bias(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->bias;\n"
                 "}\n");
  fprintf (hfile, "\ninline INT "
                   "ISA_EXEC_Base(INT bundle)\n"
                 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->base;\n"
                 "}\n");
#endif
  }                           // gen_static_code

  return;
}


/* ====================================================================
 *
 * This section handles bundle packing
 *
 * ====================================================================
 */

/////////////////////////////////////
ISA_BUNDLE_PACK_INFO ISA_Bundle_Pack_Create (const char* name, ISA_BUNDLE_PACK_ENDIAN endian, int _bundle_size)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_BUNDLE_PACK_INFO bundle_pack_info = NULL;
  std::list<ISA_BUNDLE_PACK_INFO>::reverse_iterator bundle_pack_iter = bundle_packs.rbegin();
  bundle_pack_info = new(bundle_pack_info_t);
  memset(bundle_pack_info, 0, sizeof(bundle_pack_info_t));
  bundle_pack_info->name = name;
  bundle_pack_info->endian = endian;
  bundle_pack_info->bundle_size = _bundle_size;
  bundle_pack_info->pack_index = current_pack_index++;

  current_template_index = 0;
  current_bundle_pack_info = bundle_pack_info;
  bundle_packs.push_back(bundle_pack_info);
  return bundle_pack_info;
}

/////////////////////////////////////
void Pack_Template (int comp_pos, int bundle_pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (current_bundle_pack_info == NULL) {
    fprintf(stderr, "### Error: Missing call to ISA_Bundle_Pack_Create\n");
    exit(EXIT_FAILURE);
  }
  if (bundle_pos + width > max_bundle_bits) {
    fprintf(stderr, "### Error: Pack_Template: field exceeds max bundle boundaries %d (bundle pos = %d, width = %d\n",
	    max_bundle_bits,bundle_pos,width);
    exit(EXIT_FAILURE);
  }

  current_bundle_pack_info->ftemplate[current_template_index].comp_pos = comp_pos;
  current_bundle_pack_info->ftemplate[current_template_index].bundle_pos = bundle_pos;
  current_bundle_pack_info->ftemplate[current_template_index].width = width;

  current_template_index++;
  if(current_template_index > max_templates) {
    max_templates = current_template_index;
  }
}

/////////////////////////////////////
void Pack_Slot (int slot, int comp_pos, int bundle_pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (current_bundle_pack_info == NULL) {
    fprintf(stderr, "### Error: Missing call to ISA_Bundle_Pack_Create\n");
    exit(EXIT_FAILURE);
  }
  if (slot >= MAX_SLOTS) {
    fprintf(stderr, "### Error: slot (%d) exceeds %d\n", slot, MAX_SLOTS);
    exit(EXIT_FAILURE);
  }
  if (bundle_pos + width > max_bundle_bits) {
    fprintf(stderr, "### Error: Pack_Slot: field exceeds max bundle boundaries %d (bundle pos = %d, width = %d\n",
	    max_bundle_bits,bundle_pos,width);
    exit(EXIT_FAILURE);
  }

  BUNDLE_FIELD *bfield = new(BUNDLE_FIELD);
  bfield->comp_pos = comp_pos;
  bfield->bundle_pos = bundle_pos;
  bfield->width = width;

  current_bundle_pack_info->fslot[slot].push_back(bfield);
}

/////////////////////////////////////
static unsigned long long Mask64(int width)
/////////////////////////////////////
//  Return a bit-mask of size <width>.
/////////////////////////////////////
{
  if ((unsigned)width > 64U) {
    fprintf(stderr, "### Error: field width (%d) exceeds 64\n", width);
    exit(EXIT_FAILURE);
  } else if (width == 64) {
    return -1ULL;
  }
  return (1ULL << width) - 1;
}

/////////////////////////////////////
static ISA_BUNDLE_PACK_ENDIAN Host_Endian(void)
/////////////////////////////////////
//  Return the endian-ness of the host machine.
/////////////////////////////////////
{
  int i = 0x12;
  int lowbyte = *(char *)&i;
  assert(sizeof(int) > sizeof(char));
  return (lowbyte == 0x12) ? ISA_Bundle_Pack_Little_Endian
			   : ISA_Bundle_Pack_Big_Endian;
}

/////////////////////////////////////
static void Emit_Pack_Component(
  FILE *cfile,
  ISA_BUNDLE_PACK_INFO bundle_pack_info,
  BUNDLE_FIELD *field,
  int comp,
  int *first_comps,
  int *pack_index)
/////////////////////////////////////
//  Emit the packing info for a component. A single source specification
//  may result in multiple packing info entries depending on endian
//  specification and word boundary crossings.
/////////////////////////////////////
{
  if (first_comps[comp] < 0) first_comps[comp] = *pack_index;

  if (comp == END) {
    fprintf(stderr, "### Error: ISA_BUNDLE_COMP_end no more supported\n");
    exit(-1);
  }

  bool wrong_endian = (bundle_pack_info->endian != Host_Endian());
  int incr;
  int flip_mask;
  int comp_pos = field->comp_pos;
  int bundle_pos = field->bundle_pos;
  int width = field->width;
  int word_size = max_bundle_bits >= 64 ? 64 : (max_bundle_bits + 7) & ~7;
  if (wrong_endian) {
    incr = 8;
    flip_mask = (word_size - 1) & 070;
  } else {
    incr = word_size;
    flip_mask = 0;
  }
  do {
    int bundle_word_pos = (bundle_pos % word_size) ^ flip_mask;
    int index = bundle_pos / word_size;
    int b = bundle_pos % incr;
    int w = width;
    unsigned long long mask = Mask64(w) << bundle_pos;
    
    if (b + width > incr) w = incr - b;
    fprintf (cfile,
	     "  bundle->word[%d] =   (bundle->word[%d] & ~" PRINTF_LONGLONG_HEXA ")"
	     "  | (((val >> %d) << %d) & " PRINTF_LONGLONG_HEXA ");\n",
	     index, index, mask,
	     comp_pos, bundle_word_pos, mask);

    ++*pack_index;
    bundle_pos += w;
    comp_pos += w;
    width -= w;
  } while (width != 0);
}

/////////////////////////////////////
static void Emit_Unpack_Component(
  FILE *cfile,const char *spaces,
  ISA_BUNDLE_PACK_INFO bundle_pack_info,
  BUNDLE_FIELD *field,
  int comp)
/////////////////////////////////////
//  Emit the unpacking info for a component.
/////////////////////////////////////
{
  if (comp == END) {
    fprintf(stderr, "### Error: ISA_BUNDLE_COMP_end no more supported\n");
    exit(-1);
  }

  bool wrong_endian = (bundle_pack_info->endian != Host_Endian());
  int incr;
  int flip_mask;
  int comp_pos = field->comp_pos;
  int bundle_pos = field->bundle_pos;
  int width = field->width;
  int word_size = max_bundle_bits >= 64 ? 64 : (max_bundle_bits + 7) & ~7;
  if (wrong_endian) {
    incr = 8;
    flip_mask = (word_size - 1) & 070;
  } else {
    incr = word_size;
    flip_mask = 0;
  }
  do {
    int bundle_word_pos = (bundle_pos % word_size) ^ flip_mask;
    int index = bundle_pos / word_size;
    int b = bundle_pos % incr;
    int w = width;
    unsigned long long mask = Mask64(w) << bundle_pos;
    
    if (b + width > incr) w = incr - b;
    fprintf (cfile,"%s"
	     "  pack_code |=   ((bundle.word[%d] & " PRINTF_LONGLONG_HEXA ") >> %d) << %d;\n",
	     spaces,index, mask,bundle_word_pos,comp_pos);
      
    bundle_pos += w;
    comp_pos += w;
    width -= w;
  } while (width != 0);
}


/* Returns true if <bfield> has bits between <base> and <base>+<width>
 * <nbfield> if 
 */
static bool
has_bits_in_this_word(FILE *cfile, BUNDLE_FIELD *bfield, int base, int width, BUNDLE_FIELD *nbfield)
{
  if(base <= bfield->comp_pos  && bfield->comp_pos < (base + ISA_PACK_INST_WORD_SIZE)) {
    if(nbfield != NULL) {
      //      fprintf(cfile,"  /* 1: base = %d, end = %d */\n",base,base + ISA_PACK_INST_WORD_SIZE);
      nbfield->comp_pos -= base;
      if(nbfield->comp_pos + bfield->width > width) {
	nbfield->width -= (nbfield->comp_pos + bfield->width - width);
      }
    }
    return true;
  }
  if(base >= bfield->comp_pos && base < bfield->comp_pos + bfield->width) {
    if(nbfield != NULL) {
      //      fprintf(cfile,"  /* 2: base = %d, end = %d */\n",base,base + ISA_PACK_INST_WORD_SIZE);
      nbfield->width -= (base - bfield->comp_pos);
      if(nbfield->comp_pos + nbfield->width > base + width) {
	nbfield->width -= (nbfield->comp_pos + nbfield->width) - (base + width);
      }
      nbfield->bundle_pos += base - nbfield->comp_pos;
      nbfield->comp_pos = 0;
    }
    return true;
  }
  return false;
}

/////////////////////////////////////
static void Emit_Bundle_Packing(FILE *hfile, FILE *cfile, FILE *efile)
/////////////////////////////////////
//  Emit the bundle packing interface.
/////////////////////////////////////
{
  int i;
  int first_comps[MAX_PACK_COMPS];
  int max_pack_comps = FTEMPLATE + max_templates + max_slots;
  int word_size = max_bundle_bits >= 64 ? 64 : (max_bundle_bits + 7) & ~7;
  int pack_index = 0;
  int isa_subset;

  /////////////////////////////////////////////////////////
  // Building ISA_BUNDLE_slot_count table
  
  bool gen_static_code = Is_Static_Code();   // "static" or "dynamic" TOP.  
  const char *tabname = gen_static_code ? "ISA_BUNDLE_slot_count_static" :
                                    "ISA_BUNDLE_slot_count_dynamic";

  fprintf(cfile, "\nconst mUINT8 %s[] = {\n",tabname);

  for(isa_subset = ISA_SUBSET_MIN; isa_subset <= ISA_SUBSET_MAX; isa_subset++) {
    fprintf(cfile, "  %d, /* %s */\n",isa_subset_slots[isa_subset],ISA_SUBSET_Name(isa_subset));
  }
  fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(cfile,
	    "\n"
	    "const mUINT8 *ISA_BUNDLE_slot_count = %s;\n"
	    "\n",
	    tabname);

    fprintf (hfile, "\nBE_EXPORTED extern const mUINT8 *ISA_BUNDLE_slot_count;\n\n");
    fprintf (hfile, "BE_EXPORTED extern void set_Max_Slots(int nb);\n");

    fprintf(cfile,"\nint Isa_Max_Slots = -1;\n");
    fprintf(cfile, "void set_Max_Slots(int nb) { Isa_Max_Slots= nb; /* printf(\"set_Max_Slots %%d %%x\\n\",  Isa_Max_Slots, &Isa_Max_Slots); */ }\n");
    fprintf(cfile,
	    "\nint get_Max_Slot(void)\n"
	    "{\n"
            "   return Isa_Max_Slots;\n"
            "}\n");
    fprintf (efile, "ISA_BUNDLE_slot_count\n");
  }
  else {
    fprintf(cfile,
	    "\n"
	    "const mUINT8 *dyn_get_ISA_BUNDLE_slot_count_tab ( void )\n"
	    "{  return %s;\n"
	    "}\n"
	    "\n",
	    tabname);

    fprintf(hfile,
	    "\n"
	    "extern const mUINT8 *dyn_get_ISA_BUNDLE_slot_count_tab(void);\n"
	    );
  }                              // if(gen_static_code) then else
  
  // End of table ISA_BUNDLE_slot_count
  /////////////////////////////////////////////////////////


  /* Shunt routine in the dynamic case */
  if(Is_Dynamic_Code()) {
    if(bundle_packs.size() != 0) {
      fprintf(stderr,"### Error: current model for dynamic extensions does not allow to\n"
	      "###        modify bundle packing specification at extension level!\n");
      exit(EXIT_FAILURE);
    }
    return;
  }

  /* Dealing with the normal "static" case */
  if (bundle_packs.size() == 0) {
    fprintf(stderr, "### Error: no bundle packing specification!\n");
    exit(EXIT_FAILURE);
  }

#ifdef TARG_ST
  for (i = 0; i < MAX_PACK_COMPS; ++i) {
    first_comps[i] = -1;
    if (i >= FTEMPLATE && i < FTEMPLATE+max_templates) {
      char *buf = (char *)malloc(strlen("TEMPLATE000") + 1);
      sprintf (buf, "TEMPLATE%3d", i - FTEMPLATE);
      pack_comp_type_name[i] = buf;
      char *buf2 = (char *)malloc(strlen("ISA_BUNDLE_PACK_COMP_template+000") + 1);
      sprintf (buf2, "ISA_BUNDLE_PACK_COMP_template+%3d", i - FTEMPLATE);
      pack_comp_name[i] = buf2;
    }
    if (i >= FTEMPLATE+max_templates) {
      char *buf = (char *)malloc(strlen("SLOT000") + 1);
      sprintf (buf, "SLOT%3d", i - (FTEMPLATE+max_templates));
      pack_comp_type_name[i] = buf;
      char *buf2 = (char *)malloc(strlen("ISA_BUNDLE_PACK_COMP_slot+0000000") + 1);
      sprintf (buf2, "ISA_BUNDLE_PACK_COMP_slot+%7d", i - (FTEMPLATE+max_templates));
      pack_comp_name[i] = buf2;
    }
  }
#else
  for (i = 0; i < MAX_PACK_COMPS; ++i) first_comps[i] = -1;
#endif

  if (max_bundle_bits % word_size == 0) {
    fprintf(hfile, "\ntypedef struct {\n"
		 "  mUINT%d word[%d];\n"
		 "} ISA_BUNDLE;\n",
		 word_size, max_bundle_bits / word_size);
  } else {
    fprintf(hfile, "\ntypedef struct {\n"
		 "  mUINT%d word[%d];\n"
		 "} ISA_BUNDLE;\n",
		 word_size, max_bundle_bits / word_size + 1);
  }

#if 1  
  fprintf(hfile, "\ntypedef void (*ISA_BUNDLE_PACK_T)(mINT32 bundle_id, ISA_BUNDLE *bundle, mUINT64 val);\n");
  fprintf(hfile,
	  "\ntypedef struct {\n"
	  "  ISA_BUNDLE_PACK_T pack_template;\n"
	  "  ISA_BUNDLE_PACK_T pack_slot[ISA_BUNDLE_MAX_SLOTS][ISA_PACK_MAX_INST_WORDS];\n"
	  "} ISA_BUNDLE_PACK_INFO;\n");

  fprintf(hfile, "\ntypedef enum {\n"
	"  %-30s = %d,  /* %s */\n"
	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d,  /* %s */\n"
	"} ISA_BUNDLE_PACK_COMP;\n",
	"ISA_BUNDLE_PACK_COMP_end", END, "End of list marker",
	"ISA_BUNDLE_PACK_COMP_template", FTEMPLATE, "Template => template",
	"ISA_BUNDLE_PACK_COMP_slot", FTEMPLATE+1, "SLOT+n => slot n",
        "ISA_BUNDLE_PACK_COMP_MAX", FTEMPLATE + max_slots, "Last component");

#else
  fprintf(hfile, "\ntypedef enum {\n"
	"  %-30s = %d,  /* %s */\n"
	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d,  /* %s */\n"
	"} ISA_BUNDLE_PACK_COMP;\n",
	"ISA_BUNDLE_PACK_COMP_end", END, "End of list marker",
	"ISA_BUNDLE_PACK_COMP_template", FTEMPLATE, "Template+n => template",
	"ISA_BUNDLE_PACK_COMP_slot", FTEMPLATE+max_templates, "SLOT+n => slot n",
        "ISA_BUNDLE_PACK_COMP_MAX", max_pack_comps-1, "Last component");

  fprintf(hfile, "\ntypedef struct {\n"
		"  mUINT8 comp;\n"
		"  mUINT8 index;\n"
  		"  mUINT8 comp_pos;\n"
  		"  mUINT8 bundle_pos;\n"
		"  mUINT64 mask;\n" 
		"} ISA_BUNDLE_PACK_INFO;\n");
#endif
  fprintf(efile, "ISA_BUNDLE_pack_info\n");

  pack_index = 0;
  std::list <ISA_BUNDLE_PACK_INFO>::iterator bundle_pack_info_iter;
  for(bundle_pack_info_iter = bundle_packs.begin(); 
      bundle_pack_info_iter != bundle_packs.end(); 
      bundle_pack_info_iter++) {
    ISA_BUNDLE_PACK_INFO bundle_pack_info = *bundle_pack_info_iter;

    fprintf(cfile,"\nvoid ISA_BUNDLE_Pack_Template_%s(mINT32 bundle_id, ISA_BUNDLE *bundle, mUINT64 val)\n{\n",
	    bundle_pack_info->name);

    for (i = 0; i < max_templates; ++i) {
      if (bundle_pack_info->ftemplate[i].width != 0) {
	Emit_Pack_Component(cfile,
			    bundle_pack_info,
			    &bundle_pack_info->ftemplate[i],
			    FTEMPLATE+i,
			    first_comps,
			    &pack_index);
      }
    }
    fprintf(cfile,"}\n");

    for (i = 0; i < max_slots; ++i) {
      for(int w = 0; w < ISA_PACK_MAX_INST_WORDS; w++) {
	int base = w * ISA_PACK_INST_WORD_SIZE;
        std::vector<BUNDLE_FIELD*>::iterator bf;
	bool is_entry = false;
	for(bf = bundle_pack_info->fslot[i].begin();
	    bf != bundle_pack_info->fslot[i].end(); bf++) {
	  BUNDLE_FIELD *bfield = *bf;
	  if(has_bits_in_this_word(NULL,bfield,base,ISA_PACK_INST_WORD_SIZE,NULL)) {
	    is_entry = true;
	  }
	}

	if(is_entry) {
	  fprintf(cfile,"\nvoid ISA_BUNDLE_Pack_Slot%d_Word%d_%s(mINT32 bundle_id, ISA_BUNDLE *bundle, mUINT64 val)\n{\n",
		  i, w, bundle_pack_info->name);
	
	  for(bf = bundle_pack_info->fslot[i].begin();
	      bf != bundle_pack_info->fslot[i].end(); bf++) {
	    BUNDLE_FIELD new_bf = **bf;
	    if(has_bits_in_this_word(cfile,*bf,base,ISA_PACK_INST_WORD_SIZE,&new_bf)) {
	      Emit_Pack_Component(cfile,
				  bundle_pack_info,
				  &new_bf,
				  FTEMPLATE+max_templates+i,
				  first_comps,
				  &pack_index);
	    }
	  }
	  fprintf(cfile,"}\n");
	}
      }
    }
  }

  fprintf(hfile, "\ninline const ISA_BUNDLE_PACK_INFO *ISA_BUNDLE_Pack_Info(INT bundle)\n"
		 "{\n"
		 "  BE_EXPORTED extern const ISA_BUNDLE_PACK_INFO ISA_BUNDLE_pack_info[];\n"
		 "  return &ISA_BUNDLE_pack_info[bundle];\n"
		 "}\n");

  fprintf(cfile, "\nconst ISA_BUNDLE_PACK_INFO ISA_BUNDLE_pack_info[] = {\n");
  for(bundle_pack_info_iter = bundle_packs.begin(); 
      bundle_pack_info_iter != bundle_packs.end(); 
      bundle_pack_info_iter++) {
    ISA_BUNDLE_PACK_INFO bundle_pack_info = *bundle_pack_info_iter;
    fprintf(cfile,"  { ISA_BUNDLE_Pack_Template_%s, {\n",
	    bundle_pack_info->name);
    for (i = 0; i < max_slots; ++i) {
      fprintf(cfile,"      { ");
      for(int w = 0; w < ISA_PACK_MAX_INST_WORDS; w++) {
	int base = w * ISA_PACK_INST_WORD_SIZE;
        std::vector<BUNDLE_FIELD*>::iterator bf;
	bool is_entry = false;
	for(bf = bundle_pack_info->fslot[i].begin();
	    bf != bundle_pack_info->fslot[i].end(); bf++) {
	  BUNDLE_FIELD *bfield = *bf;
	  if(has_bits_in_this_word(NULL,bfield,base,ISA_PACK_INST_WORD_SIZE,NULL)) {
	    is_entry = true;
	  }
	}
	if(is_entry) {
	  fprintf(cfile,"ISA_BUNDLE_Pack_Slot%d_Word%d_%s, ",
		  i, w, bundle_pack_info->name);
	}
	else {
	  fprintf(cfile,"NULL, ");
	}
      }
      fprintf(cfile,"},\n");
    }
    fprintf(cfile,"    },\n  },\n");
  }
  fprintf(cfile,"};\n");

  // Unpack routine

  std::map<int,std::map<ISA_BUNDLE_PACK_INFO,ISA_BUNDLE_TYPE> > bundle_types;
  for(std::list <ISA_BUNDLE_TYPE>::iterator bundle_iter = all_bundles.begin();
      bundle_iter != all_bundles.end(); bundle_iter++) {
    ISA_BUNDLE_TYPE bundle_type = *bundle_iter;
    bundle_types[bundle_type->bundle_pack_info->bundle_size][bundle_type->bundle_pack_info] = bundle_type;
  }

  if(Is_Static_Code()) {
    fprintf(hfile,"\n\n/* Unpack the bundle template given through <bundle> arg. It returns\n"
	    " * the number of slots or -1 if errors. <slots> contains recognized slots.\n"
	    " * <slots_size> gives the size for each recognized.\n"
	    " */\n");
    fprintf(hfile,"typedef mINT32 (*ISA_BUNDLE_Unpack_Template_T)(ISA_BUNDLE bundle, ISA_PACK_INST *slots, mUINT32 *slots_size);\n");
    fprintf(hfile,"BE_EXPORTED extern ISA_BUNDLE_Unpack_Template_T ISA_BUNDLE_Unpack_Template[];\n");
  }

  for(isa_subset = ISA_SUBSET_MIN; isa_subset <= ISA_SUBSET_MAX; isa_subset++) {
    fprintf(cfile,"/* See interface for description. */\n");
    fprintf(cfile,"static mINT32 ISA_BUNDLE_Unpack_Template_%s(ISA_BUNDLE bundle, ISA_PACK_INST *slots, mUINT32 *slots_size)\n{\n",
	    ISA_SUBSET_Name(isa_subset));
    fprintf(cfile,"  mUINT64 pack_code = 0;\n");
    fprintf(cfile,"  mINT32 slot_count = -1;\n\n");

    for(std::map<int,std::map<ISA_BUNDLE_PACK_INFO,ISA_BUNDLE_TYPE> >::reverse_iterator bundle_iter = bundle_types.rbegin();
	bundle_iter != bundle_types.rend(); bundle_iter++) {
      for(std::map<ISA_BUNDLE_PACK_INFO,ISA_BUNDLE_TYPE>::iterator bundle_iter2 = bundle_iter->second.begin();
	  bundle_iter2 != bundle_iter->second.end(); bundle_iter2++) {
	ISA_BUNDLE_PACK_INFO bundle_pack_info = bundle_iter2->first;
	ISA_BUNDLE_TYPE bundle_type = bundle_iter2->second;

	if(!subset_has_tops(isa_subset,bundle_type)) continue;

	fprintf(cfile,"/* Bundle size = %d, Bundle info: %s */\n",bundle_pack_info->bundle_size, bundle_pack_info->name);
	fprintf(cfile,"  pack_code = 0;\n");
	for (i = 0; i < max_templates; ++i) {
	  if (bundle_pack_info->ftemplate[i].width != 0) {
	    Emit_Unpack_Component(cfile,"",
				  bundle_pack_info,
				  &bundle_pack_info->ftemplate[i],
				  FTEMPLATE+i);
	  }
	}
	fprintf(cfile,"  if(pack_code == " PRINTF_LONGLONG_HEXA "",bundle_type->pack_code);
	fprintf(cfile,") {\n");
	fprintf(cfile,"    slot_count = %d;\n",bundle_type->slot_count);

	for (int slot = 0; slot < bundle_type->slot_count; slot++) {
	  fprintf(cfile,"    pack_code = 0;\n");
          std::vector<BUNDLE_FIELD*>::iterator bf;
	  for(bf = bundle_pack_info->fslot[slot].begin();
	      bf != bundle_pack_info->fslot[slot].end(); bf++) {
	    BUNDLE_FIELD *bfield = *bf;
	    Emit_Unpack_Component(cfile,"  ",
				  bundle_pack_info,
				  bfield,
				  FTEMPLATE+max_templates+slot);
	  }
	  fprintf(cfile,"    slots[%d] = pack_code;\n",slot);
	  fprintf(cfile,"    slots_size[%d] = %d;\n",slot,get_slot_size( slot, bundle_pack_info ));
	}
	fprintf(cfile,"    return slot_count;\n");
	fprintf(cfile,"  }\n");
      }
    }

    fprintf(cfile,"  return slot_count;\n");
    fprintf(cfile,"}\n\n");

  }

  fprintf(cfile,"ISA_BUNDLE_Unpack_Template_T ISA_BUNDLE_Unpack_Template[] = {\n");
  for(isa_subset = ISA_SUBSET_MIN; isa_subset <= ISA_SUBSET_MAX; isa_subset++) {
    fprintf(cfile,"  ISA_BUNDLE_Unpack_Template_%s,\n",
	    ISA_SUBSET_Name(isa_subset));
  }
  fprintf(cfile,"};\n");


#if 0
  fprintf(hfile, "\ninline INT ISA_BUNDLE_Pack_Info_Index(ISA_BUNDLE_PACK_COMP comp)\n"
		 "{\n"
		 "  BE_EXPORTED extern const mUINT8 ISA_BUNDLE_pack_info_index[%d];\n"
		 "  return ISA_BUNDLE_pack_info_index[(INT)comp];\n"
		 "}\n",
#ifdef TARG_ST
		 max_pack_comps);
#else
		 MAX_PACK_COMPS);
#endif
#endif
}

/* ====================================================================
 *
 * This section handles the common interfaces
 *
 * ====================================================================
 */

/////////////////////////////////////
void ISA_Bundle_Begin( const char* /* name */, int max_bundle_width )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  isa_subset_slots = std::vector<int> (ISA_SUBSET_MAX+1, 1);
  top_slots = std::vector<int> (TOP_count_limit, 0);
  max_bundle_bits = max_bundle_width;
}

/////////////////////////////////////
void ISA_Bundle_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  // Whether we generate code for an extension
  // or for the core.
  bool gen_static_code = Is_Static_Code();

  const char* const extname = gen_static_code ? NULL: Get_Extension_Name();
  const char* const fname_h = Gen_Build_Filename (FNAME_TARG_ISA_BUNDLE,
                                                  extname,
                                                  gen_util_file_type_hfile);
  const char* const fname_c = Gen_Build_Filename (FNAME_TARG_ISA_BUNDLE,
                                                  extname,
                                                  gen_util_file_type_cfile);
  const char* const fname_e = Gen_Build_Filename (FNAME_TARG_ISA_BUNDLE,
                                                  extname,
                                                  gen_util_file_type_efile);

  FILE* hfile = Gen_Open_File_Handle(fname_h,"w");
  FILE* cfile = Gen_Open_File_Handle(fname_c,"w");
  FILE* efile = gen_static_code ? Gen_Open_File_Handle(fname_e,"w") : NULL;

  const char*  fname_top_h;  // file name of (dynamic) topcode header file
  const char*  fname_pack_h; // file name of (dynamic) isa_pack header file
  const char*  fname_bun_h;  // file name of (dynamic) bundle  header file
  const char*  basename;
  GEN_UTIL_FILE_TYPE filetype;

#ifdef DYNAMIC_CODE_GEN
   /* For dynamic extensions, we need to initialize tables
      that describe register files in a similar way to what
      is done by dynamic loader. Indeed, following code require
      that routines

        ISA_PACK_inst_words

      return correct results.
    */
   ISA_PACK_Initialize_Stub();
#endif

  filetype   = gen_static_code ? gen_util_file_type_hfile:
                                 gen_util_file_type_dyn_hfile;

  basename   = gen_static_code ? FNAME_TOPCODE : FNAME_ISA_TOPCODE;
  fname_top_h= Gen_Build_Filename(basename,extname,filetype); 
 
  basename    = gen_static_code ? FNAME_TARG_ISA_PACK : FNAME_ISA_PACK;
  fname_pack_h= Gen_Build_Filename(basename,extname,filetype); 
 
  basename   = gen_static_code ? FNAME_TARG_ISA_BUNDLE : FNAME_ISA_BUNDLE;
  fname_bun_h= Gen_Build_Filename(basename,extname,filetype);

  fprintf(cfile,"#include <stdio.h>\n");

  fprintf(hfile,"#include \"%s\"\n\n",fname_top_h);
  fprintf(hfile,"#include \"%s\"\n\n",fname_pack_h);
  fprintf(cfile,"#include \"%s\"\n\n",fname_bun_h);

  if(gen_static_code) {
    fprintf(cfile,"#include \"targ_isa_subset.h\"\n\n");
  }
  fprintf(hfile,"#include \"targ_proc.h\"\n\n");
    

  Emit_Header  (hfile, FNAME_TARG_ISA_BUNDLE, interface,extname);
  Emit_C_Header(cfile);           // emit "C" directive

  Emit_Bundle_Scheduling(hfile, cfile, efile);
  fprintf(hfile, "\f");
  fprintf(cfile, "\f");
  Emit_Bundle_Packing(hfile, cfile, efile);

  Emit_Footer  (hfile);
  Emit_C_Footer(cfile);

  // Closing all file handlers.
  Gen_Close_File_Handle(cfile,fname_c);
  Gen_Close_File_Handle(hfile,fname_h);
  if(efile)
    Gen_Close_File_Handle(efile,fname_e);

  // Memory deallocation
  Gen_Free_Filename(const_cast<char*> (fname_c));
  Gen_Free_Filename(const_cast<char*> (fname_h));
  Gen_Free_Filename(const_cast<char*> (fname_e));

  Gen_Free_Filename(const_cast<char*> (fname_top_h));
  Gen_Free_Filename(const_cast<char*> (fname_bun_h));

  return;
}
#else
#define MAX_SLOTS 3	// max # of slots the generator can handle
#define TAG_SHIFT 12    // max # of bits required to encode all the
                        // execution property types.

struct isa_exec_unit_type {
  const char *name; 	// Name given for documentation and debugging
  int bit_position;	// bit position in flag word
  std::vector<bool> members; // set of opcodes that have this property
  ISA_EXEC_UNIT_TYPE base_unit; // base exec unit type (or null if base)
};

struct isa_bundle_type {
  const char *name;
  const char *asm_name;
  int slot_count;
  ISA_EXEC_UNIT_TYPE slot[MAX_SLOTS];
  bool stop_bit[MAX_SLOTS];
  unsigned int pack_code;
};

static int isa_exec_property_count = 0; 

static int num_bundles = 0;
static int max_slots = 0;
static int bundle_bits;
static std::list<ISA_EXEC_UNIT_TYPE> all_exec_types; 
static std::list<ISA_BUNDLE_TYPE> all_bundles; 
static ISA_EXEC_UNIT_TYPE current_exec_type_desc;
static ISA_BUNDLE_TYPE current_bundle_desc;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the bundling properties. The interface is",
  " *   divided into two pieces: scheduling, and packing. The scheduling",
  " *   interface exports the following:",
  " *",
  " *   const INT ISA_MAX_SLOTS",
  " *       An integer constant that indicates the maximum number of",
  " *       slots in a bundle.",
  " *",
  " *   const INT ISA_TAG_SHIFT",
  " *       Maximum number of bits required to encode all the execution",
  " *       property types.",
  " *",
  " *   typedef mUINTxx ISA_EXEC_UNIT_PROPERTY",
  " *       A single-bit mask of representing an execution unit.",
  " *",
  " *       The names have the form ISA_EXEC_PROPERTY_xxx",
  " *       where 'xxx' is replaced with the EXEC_UNIT_PROPERTY name.",
  " *",
  " *   typedef (enum) ISA_EXEC_UNIT",
  " *       An enumeration of the execution units.",
  " *",
  " *       The names have the form ISA_EXEC_xxx",
  " *       where 'xxx' is replaced with the EXEC_UNIT_PROPERTY name.",
  " *",
  " *       The values of ISA_EXEC_UNIT and ISA_EXEC_UNIT_PROPERTY are",
  " *       related in that the bit-mask value of an ISA_EXEC_UNIT_PROPERTY",
  " *       is equal to 2**ISA_EXEC_UNIT.",
  " *",
  " *   const INT ISA_EXEC_MAX",
  " *       The highest value ISA_EXEC_UNIT value.",
  " *",
  " *   BOOL ISA_EXEC_PROPERTY_is_xxx(TOP t) ",
  " *       Returns TRUE if EXEC_PROPERTY_is_xxx matches <t>'s property.",
  " *",
  " *   ISA_EXEC_UNIT_PROPERTY ISA_EXEC_Unit_Prop(TOP topcode)",
  " *       Returns exec_unit_property for the instruction specified",
  " *       by <topcode>.",
  " *",
  " *   ISA_BUNDLE_INFO ISA_EXEC_Bundle_Info(INT index)",
  " *       Return isa_bundle_info specified by <index>. ",
  " *",
  " *   ISA_EXEC_UNIT_PROPERTY ISA_EXEC_Slot_Prop(INT bundle, INT slot_index)",
  " *       Return exec_unit_property for the slot position <slot_index>",
  " *       in <bundle>.",
  " *",
  " *   UINT64 ISA_EXEC_Slot_Mask(INT bundle)",
  " *       Return slot_mask for <bundle>.",
  " *",
  " *   BOOL ISA_EXEC_Stop(INT bundle, INT slot_index)",
  " *       Return stop bit for the slot position <slot_index> in <bundle>.",
  " *",
  " *   ISA_EXEC_UNIT ISA_EXEC_Unit(INT bundle, INT slot_index)",
  " *       Return the execution unit slot position <slot_index> in <bundle>.",
  " *",
  " *   UINT32 ISA_EXEC_Stop_Mask(INT bundle)",
  " *       Return stop_mask for <bundle>.",
  " *",
  " *   const char *ISA_EXEC_Name(INT bundle)",
  " *       Return the name for <bundle>.",
  " *",
  " *   const char *ISA_EXEC_AsmName(INT bundle)",
  " *       Return the assembly language name for <bundle>.",
  " *",
  " * ====================================================================",
  " *",
  " *   The packing interface exports the following:",
  " *",
  " *   typedef ISA_BUNDLE",
  " *       A type large enough to hold a bundle. This type will always",
  " *       be a struct containing an array of either 32-, or 64-bit",
  " *       unsigned integers.",
  " *",
  " *   typedef (enum) ISA_BUNDLE_PACK_COMP",
  " *       An enumeration of the bundle components to be packed.",
  " *",
  " *   const INT ISA_BUNDLE_PACK_COMP_MAX",
  " *       The maximum number of components to be packed for a bundle.",
  " *",
  " *   typedef (struct) ISA_BUNDLE_PACK_INFO",
  " *       Describes how a the components of a bundle are packed.",
  " *       The contents are private.",
  " *",
  " *   const ISA_BUNDLE_PACK_INFO *ISA_BUNDLE_Pack_Info(void)",
  " *       Returns a pointer to the first packing component.",
  " *       Increment the returned pointer to access any additional packing",
  " *       components for the bundle. A component of ISA_PACK_COMP_end",
  " *       marks the end.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_Comp(const ISA_BUNDLE_PACK_INFO *info)",
  " *       Identifies the bundle component to be packed.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_Index(const ISA_BUNDLE_PACK_INFO *info)",
  " *       The index of the bundle word containing the component.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_Index is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_CompPos(const ISA_BUNDLE_PACK_INFO *info)",
  " *       The offset, in bits, to the start of the component in the",
  " *       component value.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_CompPos is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   INT ISA_BUNDLE_PACK_INFO_BundlePos(const ISA_BUNDLE_PACK_INFO *info)",
  " *       The offset, in bits, to the start of the component in the",
  " *       bundle word.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_BundlePos is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   UINT64 ISA_BUNDLE_PACK_INFO_Mask(const ISA_BUNDLE_PACK_INFO *info)",
  " *       A bit mask that is as wide as the bundle component being",
  " *       packed. The mask is shifted to match the field in the",
  " *       bundle word.",
  " *",
  " *       ISA_BUNDLE_PACK_INFO_Mask is meaningless for ISA_BUNDLE_PACK_COMP_end.",
  " *",
  " *   INT ISA_BUNDLE_Pack_Info_Index(ISA_BUNDLE_PACK_COMP comp)",
  " *       Index into bundle packing info array (see ISA_BUNDLE_Pack_Info)",
  " *       to the start of the info for the component <comp>. If there",
  " *       is no packing info for <comp>, the index is for the 'end'",
  " *       component.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

/* ====================================================================
 *
 * This section handles bundle scheduling
 *
 * ====================================================================
 */

/////////////////////////////////////
ISA_EXEC_UNIT_TYPE ISA_Exec_Unit_Type_Create ( const char* name,
					       ISA_EXEC_UNIT_TYPE base_unit )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_EXEC_UNIT_TYPE cur_type = new isa_exec_unit_type;

  cur_type->name = name;
  cur_type->bit_position = isa_exec_property_count++;
  cur_type->members = std::vector<bool> (TOP_count, false);
  cur_type->base_unit = base_unit;

  current_exec_type_desc = cur_type;
  all_exec_types.push_back (current_exec_type_desc);
  return cur_type;
}

/////////////////////////////////////
void Instruction_Exec_Unit_Group(ISA_EXEC_UNIT_TYPE unit_type, ... )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  TOP opcode;

  if (!current_exec_type_desc->name) {
    fprintf(stderr,"### Error: no execution unit type name specified for %s\n",
                   current_exec_type_desc->name);
    exit(EXIT_FAILURE);
  }
 
  va_start(ap, unit_type);
  while ( (opcode = static_cast<TOP>(va_arg(ap, int))) != TOP_UNDEFINED) {
      unit_type->members[(int)opcode] = true;      
  }
  va_end(ap);  
}

/////////////////////////////////////
void ISA_Bundle_Type_Create (const char* name, const char* asm_name, 
			     int no_slots )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int i;
  ISA_BUNDLE_TYPE cur_type = new isa_bundle_type;
  cur_type->name = name;
  cur_type->asm_name = asm_name;
  cur_type->slot_count = no_slots;
  cur_type->pack_code = num_bundles;
  for (i = 0; i < no_slots; ++i) cur_type->stop_bit[i] = false;

  current_bundle_desc = cur_type;
  all_bundles.push_back (current_bundle_desc);

  if (no_slots > max_slots) max_slots = no_slots;
  ++num_bundles;
}

/////////////////////////////////////
void Slot (int slot_index, ISA_EXEC_UNIT_TYPE type)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (slot_index > current_bundle_desc->slot_count) {
    fprintf(stderr, "### Error: slot index (%d) exceeds %d\n",
		    slot_index, current_bundle_desc->slot_count);
    exit(EXIT_FAILURE);
  }

  if (!type) {
    fprintf(stderr, "### Error: slot type have non NULL value \n");
    exit(EXIT_FAILURE);
  }

  current_bundle_desc->slot[slot_index] = type;
}

/////////////////////////////////////
void Stop (int slot_index)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (slot_index > current_bundle_desc->slot_count) {
    fprintf(stderr, "### Error: slot index (%d) exceeds %d\n",
		    slot_index, current_bundle_desc->slot_count);
    exit(EXIT_FAILURE);
  }

  current_bundle_desc->stop_bit[slot_index] = true;
}

/////////////////////////////////////
static void Emit_Bundle_Scheduling(FILE *hfile, FILE *cfile, FILE *efile)
/////////////////////////////////////
//  Emit the bundle scheduling interface.
/////////////////////////////////////
{
  std::list<ISA_EXEC_UNIT_TYPE>::iterator iei;
  std::list<ISA_BUNDLE_TYPE>::iterator ibi;
  int i;

  const char * const isa_exec_type_format = "  %3llu,  /* %s: ";
  const char *info_index_type;

  int index = 0;
  for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++index,
								  ++iei) {
  }

  const char *int_suffix;
  // select the ISA_EXEC_unit_prop based on the number of exec info types.
  if (index <= 8) {
    info_index_type = "mUINT8";
    int_suffix = "";
  } else if (index <= 16) {
    info_index_type = "mUINT16";
    int_suffix = "";
  } else if (index <= 32) {
    info_index_type = "mUINT32";
    int_suffix = "U";
  } else {
    assert (index <= 64);
    info_index_type = "mUINT64";
    int_suffix = "ULL";
  }

  fprintf (hfile, "\n#define ISA_MAX_SLOTS (%d)\n", max_slots);
  fprintf (hfile, "#define ISA_TAG_SHIFT (%d)\n", TAG_SHIFT);

  fprintf (hfile, "\ntypedef %s ISA_EXEC_UNIT_PROPERTY;\n",
	   info_index_type);

  fprintf (hfile, "\n");
  for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
    ISA_EXEC_UNIT_TYPE curr_exec_type = *iei;
    fprintf (hfile, "#define ISA_EXEC_PROPERTY_%-15s (0x%llx%s)\n",
		    curr_exec_type->name,
		    (1ULL << curr_exec_type->bit_position), int_suffix);
  }

  fprintf (hfile, "\ntypedef enum {\n");
  for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
    ISA_EXEC_UNIT_TYPE curr_exec_type = *iei;
    fprintf (hfile, "  ISA_EXEC_%-15s = %d,\n",
		    curr_exec_type->name,
		    curr_exec_type->bit_position);
  }
  fprintf (hfile, "  ISA_EXEC_%-15s = %d\n"
		  "} ISA_EXEC_UNIT;\n",
		  "MAX", isa_exec_property_count - 1);

  fprintf (hfile, "\ntypedef struct {\n"
		  "  const char *name;\n"
		  "  const char *asm_name;\n"
		  "  int slot_count;\n"
		  "  ISA_EXEC_UNIT_PROPERTY slot[%d];\n"
		  "  mBOOL stop[%d];\n"
		  "  mUINT8 unit[%d];\n"
		  "  mUINT8 pack_code;\n"
		  "  mUINT8 stop_mask;\n"
		  "  mUINT64 slot_mask;\n"
		  "} ISA_BUNDLE_INFO;\n",
		  max_slots ? max_slots : 1,
		  max_slots ? max_slots : 1,
		  max_slots ? max_slots : 1);

  fprintf(efile, "ISA_BUNDLE_info\n");
  fprintf(cfile, "\nconst ISA_BUNDLE_INFO ISA_BUNDLE_info[] = {\n");

  int slot_mask_digits = ((TAG_SHIFT * max_slots) + 3) / 4;
  for (ibi = all_bundles.begin(); ibi != all_bundles.end(); ++ibi) {
    ISA_BUNDLE_TYPE curr_exec_type = *ibi;
    fprintf (cfile, " {\n    \"%s\",%*s \"%s\",%*s %d,", 
		    curr_exec_type->name, 
#ifdef KEY /* Mac port */
		    (int) (13 - strlen(curr_exec_type->name)), "",
		    curr_exec_type->asm_name, 
		    (int) (8 - strlen(curr_exec_type->asm_name)), "",
#else /* KEY Mac port */
		    13 - strlen(curr_exec_type->name), "",
		    curr_exec_type->asm_name, 
		    8 - strlen(curr_exec_type->asm_name), "",
#endif /* KEY Mac port */
		    curr_exec_type->slot_count);

    unsigned long long slot_mask = 0;
    unsigned int stop_mask = 0;
    fprintf (cfile, "\n    {");
    for (i = 0; i < curr_exec_type->slot_count; i++) {
      unsigned int flag_value = 1 << curr_exec_type->slot[i]->bit_position;
      int shift_count = max_slots - i - 1;
      slot_mask |= ((unsigned long long)flag_value << (TAG_SHIFT * shift_count));
      stop_mask |= (curr_exec_type->stop_bit[i] << shift_count);
      fprintf (cfile, " %2d /* %7s */,", 
		      flag_value,
		      curr_exec_type->slot[i]->name);
    }
    fprintf (cfile, " },");

    fprintf (cfile, "\n    {");
    for (i = 0; i < max_slots; i++) {
      fprintf (cfile, " %5s,", curr_exec_type->stop_bit[i] ? "TRUE" : "FALSE");
    }
    fprintf (cfile, " },");

    fprintf (cfile, "\n    {");
    for (i = 0; i < curr_exec_type->slot_count; i++) {
      ISA_EXEC_UNIT_TYPE unit_type = curr_exec_type->slot[i];
      if (unit_type->base_unit) unit_type = unit_type->base_unit;
      fprintf (cfile, " ISA_EXEC_%5s,", unit_type->name);
    }
    fprintf (cfile, " },");

    fprintf(cfile, "\n    %2d,", curr_exec_type->pack_code);
    fprintf(cfile, " 0x%1x,", stop_mask);
    fprintf(cfile, " 0x%0*llx\n  },\n", slot_mask_digits, slot_mask);
  }
  fprintf (cfile, "  {\n    \"template_MAX\", \"\", -1,\n    { -1 /* ??????? */");
  for (i = 1; i < max_slots; ++i) fprintf (cfile, ", -1 /* ??????? */");
  fprintf (cfile, ",},\n    { FALSE");
  for (i = 1; i < max_slots; ++i) fprintf (cfile, ", FALSE");
  fprintf (cfile, ",},\n    -1, 0x0, 0x%0*x\n  }\n};\n", slot_mask_digits, 0);

  fprintf(hfile,"\n#define ISA_MAX_BUNDLES %d\n",num_bundles);

  fprintf (efile, "ISA_EXEC_unit_prop\n");
  fprintf (cfile, "\nconst ISA_EXEC_UNIT_PROPERTY ISA_EXEC_unit_prop[%d] = {\n",
	  TOP_count);

  for (int top = 0; top < TOP_count; ++top) {
    unsigned long long flag_value = 0;

    for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
      ISA_EXEC_UNIT_TYPE exec_type = *iei;
      if (exec_type->members[top]) 
	flag_value |= (1ULL << exec_type->bit_position);
    }
    fprintf(cfile, 	isa_exec_type_format,
			flag_value,
			TOP_Name((TOP)top));
    for ( iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei ) {
      ISA_EXEC_UNIT_TYPE exec_type = *iei;
      if (exec_type->members[top]) 
	fprintf (cfile, " %s", exec_type->name);
    }
    fprintf (cfile, " */\n");
  }
  fprintf(cfile, "};\n");

  fprintf(hfile, "\nextern const ISA_EXEC_UNIT_PROPERTY ISA_EXEC_unit_prop[];\n");
  fprintf(hfile, "\n");
  for (iei = all_exec_types.begin(); iei != all_exec_types.end(); ++iei) {
    ISA_EXEC_UNIT_TYPE exec_type = *iei;
    fprintf(hfile,
             "#define EXEC_PROPERTY_is_%s(t)\t (ISA_EXEC_unit_prop[(INT)t] & ISA_EXEC_PROPERTY_%s)\n",
             exec_type->name, exec_type->name);
  }

  fprintf (hfile, "\ninline ISA_EXEC_UNIT_PROPERTY "
                   "ISA_EXEC_Unit_Prop(TOP topcode)\n"
                 "{\n"
                 "  return ISA_EXEC_unit_prop[(INT)topcode];\n"
                 "}\n");
		   
  fprintf (hfile, "\ninline ISA_BUNDLE_INFO "
                   "ISA_EXEC_Bundle_Info(INT index)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
                 "  return ISA_BUNDLE_info[index];\n"
                 "}\n");
		   
  fprintf (hfile, "\ninline ISA_EXEC_UNIT_PROPERTY "
                   "ISA_EXEC_Slot_Prop(INT bundle, INT slot_index)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->slot[slot_index];\n"
                 "}\n");

  fprintf (hfile, "\ninline UINT64 "
                   "ISA_EXEC_Slot_Mask(INT bundle)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->slot_mask;\n"
                 "}\n");

  fprintf (hfile, "\ninline BOOL "
                   "ISA_EXEC_Stop(INT bundle, INT slot_index)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->stop[slot_index];\n"
                 "}\n");

  fprintf (hfile, "\ninline ISA_EXEC_UNIT "
                   "ISA_EXEC_Unit(INT bundle, INT slot_index)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return (ISA_EXEC_UNIT)info->unit[slot_index];\n"
                 "}\n");

  fprintf (hfile, "\ninline UINT32 "
                   "ISA_EXEC_Stop_Mask(INT bundle)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->stop_mask;\n"
                 "}\n");

  fprintf (hfile, "\ninline const char * "
                   "ISA_EXEC_Name(INT bundle)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->name;\n"
                 "}\n");

  fprintf (hfile, "\ninline const char * "
                   "ISA_EXEC_AsmName(INT bundle)\n"
                 "{\n"
		 "  extern const ISA_BUNDLE_INFO ISA_BUNDLE_info[];\n"
		 "  const ISA_BUNDLE_INFO *info = ISA_BUNDLE_info + bundle;\n"
                 "  return info->asm_name;\n"
                 "}\n");
}

/* ====================================================================
 *
 * This section handles bundle packing
 *
 * ====================================================================
 */

typedef enum {
  END		= 0,		// end of list marker
  FTEMPLATE	= 1,		// template
  FSLOT		= 2,		// slot+n => slot n
} PACK_COMP_TYPE;

#define MAX_PACK_COMPS (FSLOT+MAX_SLOTS)

static const char * const pack_comp_type_name[] = {
  "END",
  "TEMPLATE",
  "SLOT0",
  "SLOT1",
  "SLOT2",
};

static const char * const pack_comp_name[] = {
  "ISA_BUNDLE_PACK_COMP_end",
  "ISA_BUNDLE_PACK_COMP_template",
  "ISA_BUNDLE_PACK_COMP_slot+0",
  "ISA_BUNDLE_PACK_COMP_slot+1",
  "ISA_BUNDLE_PACK_COMP_slot+2",
};

typedef struct {
  int comp_pos;
  int bundle_pos;
  int width;
} BUNDLE_FIELD;

typedef struct {
  ISA_BUNDLE_PACK_ENDIAN endian;
  BUNDLE_FIELD ftemplate;
  BUNDLE_FIELD fslot[MAX_SLOTS];
} BUNDLE_PACK_INFO;

static BUNDLE_PACK_INFO *bundle_pack_info = NULL;

/////////////////////////////////////
void ISA_Bundle_Pack_Create (ISA_BUNDLE_PACK_ENDIAN endian)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bundle_pack_info) {
    fprintf(stderr, "### Error: ISA_Bundle_Pack_Create called multiple times\n");
    exit(EXIT_FAILURE);
  }

  bundle_pack_info = new(BUNDLE_PACK_INFO);
  memset(bundle_pack_info, 0, sizeof(*bundle_pack_info));
  bundle_pack_info->endian = endian;
}

/////////////////////////////////////
void Pack_Template (int comp_pos, int bundle_pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bundle_pack_info == NULL) {
    fprintf(stderr, "### Error: Missing call to ISA_Bundle_Pack_Create\n");
    exit(EXIT_FAILURE);
  }
  if (bundle_pos + width > bundle_bits) {
    fprintf(stderr, "### Error: field exceeds bundle boundaries\n");
    exit(EXIT_FAILURE);
  }

  bundle_pack_info->ftemplate.comp_pos = comp_pos;
  bundle_pack_info->ftemplate.bundle_pos = bundle_pos;
  bundle_pack_info->ftemplate.width = width;
}

/////////////////////////////////////
void Pack_Slot (int slot, int comp_pos, int bundle_pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bundle_pack_info == NULL) {
    fprintf(stderr, "### Error: Missing call to ISA_Bundle_Pack_Create\n");
    exit(EXIT_FAILURE);
  }
  if (slot >= MAX_SLOTS) {
    fprintf(stderr, "### Error: slot (%d) exceeds %d\n", slot, MAX_SLOTS);
    exit(EXIT_FAILURE);
  }
  if (bundle_pos + width > bundle_bits) {
    fprintf(stderr, "### Error: field exceeds bundle boundaries\n");
    exit(EXIT_FAILURE);
  }

  bundle_pack_info->fslot[slot].comp_pos = comp_pos;
  bundle_pack_info->fslot[slot].bundle_pos = bundle_pos;
  bundle_pack_info->fslot[slot].width = width;
}

/////////////////////////////////////
static unsigned long long Mask64(int width)
/////////////////////////////////////
//  Return a bit-mask of size <width>.
/////////////////////////////////////
{
  if ((unsigned)width > 64U) {
    fprintf(stderr, "### Error: field width (%d) exceeds 64\n", width);
    exit(EXIT_FAILURE);
  } else if (width == 64) {
    return -1ULL;
  }
  return (1ULL << width) - 1;
}

/////////////////////////////////////
static ISA_BUNDLE_PACK_ENDIAN Host_Endian(void)
/////////////////////////////////////
//  Return the endian-ness of the host machine.
/////////////////////////////////////
{
  int i = 0x12;
  int lowbyte = *(char *)&i;
  assert(sizeof(int) > sizeof(char));
  return (lowbyte == 0x12) ? ISA_Bundle_Pack_Little_Endian
			   : ISA_Bundle_Pack_Big_Endian;
}

/////////////////////////////////////
static void Emit_Pack_Component(
  FILE *cfile,
  BUNDLE_FIELD *field,
  int comp,
  int *first_comps,
  int *pack_index)
/////////////////////////////////////
//  Emit the packing info for a component. A single source specification
//  may result in multiple packing info entries depending on endian
//  specification and word boundary crossings.
/////////////////////////////////////
{
  if (first_comps[comp] < 0) first_comps[comp] = *pack_index;

  if (comp == END) {
    fprintf (cfile, "  { %-30s, %2d, %2d, %2d,   %16lld },  /* %s */\n",
		    pack_comp_name[comp],
		    -1,
		    -1,
		    -1, 
		    -1LL,
		    pack_comp_type_name[comp]);
    ++*pack_index;
  } else {
    bool wrong_endian = (bundle_pack_info->endian != Host_Endian());
    int incr;
    int flip_mask;
    int comp_pos = field->comp_pos;
    int bundle_pos = field->bundle_pos;
    int width = field->width;
    int word_size = bundle_bits >= 64 ? 64 : (bundle_bits + 7) & ~7;
    if (wrong_endian) {
      incr = 8;
      flip_mask = (word_size - 1) & 070;
    } else {
      incr = word_size;
      flip_mask = 0;
    }
    do {
      int bundle_word_pos = (bundle_pos % word_size) ^ flip_mask;
      int index = bundle_pos / word_size;
      int b = bundle_pos % incr;
      int w = width;
      if (b + width > incr) w = incr - b;
      fprintf (cfile, "  { %-30s, %2d, %2d, %2d, 0x%016llx },  /* %s */\n",
		      pack_comp_name[comp],
		      index,
		      comp_pos,
		      bundle_word_pos,
		      Mask64(w) << bundle_word_pos,
		      pack_comp_type_name[comp]);
      ++*pack_index;
      bundle_pos += w;
      comp_pos += w;
      width -= w;
    } while (width != 0);
  }
}

/////////////////////////////////////
static void Emit_Bundle_Packing(FILE *hfile, FILE *cfile, FILE *efile)
/////////////////////////////////////
//  Emit the bundle packing interface.
/////////////////////////////////////
{
  int i;
  int first_comps[MAX_PACK_COMPS];
  int max_pack_comps = FSLOT + max_slots;
  int word_size = bundle_bits >= 64 ? 64 : (bundle_bits + 7) & ~7;
  int pack_index = 0;

  if (bundle_pack_info == NULL) {
    fprintf(stderr, "### Error: no bundle packing specification!\n");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < MAX_PACK_COMPS; ++i) first_comps[i] = -1;

  fprintf(hfile, "\ntypedef struct {\n"
		 "  mUINT%d word[%d];\n"
		 "} ISA_BUNDLE;\n",
		 word_size, bundle_bits / word_size);

  fprintf(hfile, "\ntypedef enum {\n"
	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d,  /* %s */\n"
   	"  %-30s = %d   /* %s */\n"
	"} ISA_BUNDLE_PACK_COMP;\n",
	"ISA_BUNDLE_PACK_COMP_end", END, "End of list marker",
	"ISA_BUNDLE_PACK_COMP_template", FTEMPLATE, "Template",
	"ISA_BUNDLE_PACK_COMP_slot", FSLOT, "SLOT+n => slot n",
        "ISA_BUNDLE_PACK_COMP_MAX", max_pack_comps-1, "Last component");

  fprintf(hfile, "\ntypedef struct {\n"
		"  mUINT8 comp;\n"
		"  mUINT8 index;\n"
  		"  mUINT8 comp_pos;\n"
  		"  mUINT8 bundle_pos;\n"
		"  UINT64 mask;\n" 
		"} ISA_BUNDLE_PACK_INFO;\n");

  fprintf(efile, "ISA_BUNDLE_pack_info\n");

  fprintf(cfile, "\nconst ISA_BUNDLE_PACK_INFO ISA_BUNDLE_pack_info[] = {\n");
  pack_index = 0;
  if (bundle_pack_info->ftemplate.width != 0) {
    Emit_Pack_Component(cfile,
			&bundle_pack_info->ftemplate,
			FTEMPLATE,
			first_comps,
			&pack_index);

  }
  for (i = 0; i < max_slots; ++i) {
    Emit_Pack_Component(cfile,
			&bundle_pack_info->fslot[i],
			FSLOT+i,
			first_comps,
			&pack_index);
  }
  Emit_Pack_Component(cfile,
		      NULL,
		      END,
		      first_comps,
		      &pack_index);

  fprintf (cfile, "};\n");

  fprintf(hfile, "\ninline const ISA_BUNDLE_PACK_INFO *ISA_BUNDLE_Pack_Info(void)\n"
		 "{\n"
		 "  extern const ISA_BUNDLE_PACK_INFO ISA_BUNDLE_pack_info[];\n"
		 "  return ISA_BUNDLE_pack_info;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_BUNDLE_PACK_INFO_Comp(const ISA_BUNDLE_PACK_INFO *info)\n"
		 "{\n"
		 "  return info->comp;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_BUNDLE_PACK_INFO_Index(const ISA_BUNDLE_PACK_INFO *info)\n"
		 "{\n"
		 "  return info->index;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_BUNDLE_PACK_INFO_CompPos(const ISA_BUNDLE_PACK_INFO *info)\n"
		 "{\n"
		 "  return info->comp_pos;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_BUNDLE_PACK_INFO_BundlePos(const ISA_BUNDLE_PACK_INFO *info)\n"
		 "{\n"
		 "  return info->bundle_pos;\n"
		 "}\n");

  fprintf(hfile, "\ninline UINT64 ISA_BUNDLE_PACK_INFO_Mask(const ISA_BUNDLE_PACK_INFO *info)\n"
		 "{\n"
		 "  return info->mask;\n"
		 "}\n");

  fprintf(efile, "ISA_BUNDLE_pack_info_index\n");

  fprintf(cfile, "\nconst mUINT8 ISA_BUNDLE_pack_info_index[%d] = {\n",
		 MAX_PACK_COMPS);
  for (i = 0; i < MAX_PACK_COMPS; ++i) {
    int index = first_comps[i];
    if (index < 0) index = first_comps[END];
    fprintf(cfile, "  %2d, /* %s */\n", index, pack_comp_name[i]);
  }
  fprintf(cfile, "};\n");

  fprintf(hfile, "\ninline INT ISA_BUNDLE_Pack_Info_Index(ISA_BUNDLE_PACK_COMP comp)\n"
		 "{\n"
		 "  extern const mUINT8 ISA_BUNDLE_pack_info_index[%d];\n"
		 "  return ISA_BUNDLE_pack_info_index[(INT)comp];\n"
		 "}\n",
		 MAX_PACK_COMPS);
}

/* ====================================================================
 *
 * This section handles the common interfaces
 *
 * ====================================================================
 */

/////////////////////////////////////
void ISA_Bundle_Begin( const char* /* name */, int bundle_width )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  bundle_bits = bundle_width;
}

/////////////////////////////////////
void ISA_Bundle_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{

#define FNAME "targ_isa_bundle"
  char buf[1000];
  sprintf (buf, "%s.h", FNAME);
  FILE* hfile = fopen(buf, "w");
  sprintf (buf, "%s.c", FNAME);
  FILE* cfile = fopen(buf, "w");
  sprintf (buf, "%s.Exported", FNAME);
  FILE* efile = fopen(buf, "w");

  fprintf(hfile, "#include \"topcode.h\"\n");
  fprintf(cfile,"#include \"%s.h\"\n\n", FNAME);

  sprintf (buf, "%s", FNAME);
  Emit_Header (hfile, buf, interface);

  Emit_Bundle_Scheduling(hfile, cfile, efile);
  fprintf(hfile, "\f");
  fprintf(cfile, "\f");
  Emit_Bundle_Packing(hfile, cfile, efile);
		   
  Emit_Footer (hfile);
}
#endif
