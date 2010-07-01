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


#if !defined(__FreeBSD__)
#include <alloca.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#include "ti_si.h"
#include "ti_errors.h"
#include "targ_isa_subset.h"
#include "targ_isa_bundle.h"
#include "ti_bundle.h"


#ifdef TARG_ST
/* ====================================================================
 *  TI_BUNDLE_initialize
 *  Thid function aim to propagate targinfo value pre-computed in case 
 *  of reentrance in targinfo
 * ====================================================================
 */
void TI_BUNDLE_initialize(int max_slot) {
	Isa_Max_Slots=max_slot;
}
#endif

/* ====================================================================
 *
 *  TI_BUNDLE_Has_Property
 *
 *  See interface description
 *
 * ====================================================================
 */
BOOL TI_BUNDLE_Has_Property( 
  TI_BUNDLE *bundle,
  ISA_EXEC_UNIT_PROPERTY property,
  INT *error
)
{
  INT i;

  *error = TI_RC_OKAY;

  if (bundle == NULL) {
    sprintf(TI_errmsg, "ISA_BUNDLE_INFO is empty");
    *error = TI_RC_ERROR;
    return FALSE;
  }

  for (i = 0; i < TI_BUNDLE_slot_count(bundle); i++) {
    if (TI_BUNDLE_exec_property(bundle,i) & property) 
      return TRUE;
  }
  
  return FALSE;
}
    
/* ====================================================================
 *
 *  TI_BUNDLE_Is_Full
 *
 *  See interface description
 *
 * ====================================================================
 */
BOOL TI_BUNDLE_Is_Full( 
  TI_BUNDLE *bundle,
  INT *error
)
{
  INT i;

  *error = TI_RC_OKAY;

  if (bundle == NULL) {
    sprintf(TI_errmsg, "ISA_BUNDLE_INFO is empty");
    *error = TI_RC_ERROR;
    return FALSE;
  }

  for (i = 0; i < TI_BUNDLE_slot_count(bundle); i++) {
    if (!TI_BUNDLE_slot_filled(bundle, i)) 
      return FALSE;
  }
  
  return TRUE;
}

/* ====================================================================
 *
 *  TI_BUNDLE_Is_Empty
 *
 *  See interface description
 *
 * ====================================================================
 */
BOOL TI_BUNDLE_Is_Empty( 
  TI_BUNDLE *bundle,
  INT *error
)
{
  INT i;

  *error = TI_RC_OKAY;

  if (bundle == NULL) {
    sprintf(TI_errmsg, "ISA_BUNDLE_INFO is empty");
    *error = TI_RC_ERROR;
    return FALSE;
  }

  for (i = 0; i < TI_BUNDLE_slot_count(bundle); i++) {
    if (TI_BUNDLE_slot_filled(bundle, i)) 
      return FALSE;
  }
  
  return TRUE;
}

/* ====================================================================
 *
 *  TI_BUNDLE_Return_Template
 *
 *  See interface description
 *
 * ====================================================================
 */
INT TI_BUNDLE_Return_Template( 
  TI_BUNDLE *bundle
)
{
  return TI_BUNDLE_pack_code(bundle);
}

/* ====================================================================
 *
 *  TI_BUNDLE_Clear
 *
 *  See interface description
 *
 * ====================================================================
 */
void TI_BUNDLE_Clear( 
  TI_BUNDLE *bundle
)
{
 #ifdef TARG_ST
  ISA_EXEC_MASK slot_mask = { 0 };
#endif
  INT i;
  for (i = 0; i < ISA_MAX_SLOTS; i++) {
    TI_BUNDLE_slot_filled(bundle, i) =  0;
    TI_BUNDLE_exec_property(bundle, i) = 0;
    TI_BUNDLE_stop_bit(bundle, i) = 0; 
  }

  Set_TI_BUNDLE_pack_code(bundle, 0);
  Set_TI_BUNDLE_stop_mask(bundle, 0x0);
#ifdef TARG_ST
  Set_TI_BUNDLE_slot_mask(bundle, slot_mask);
#else
  Set_TI_BUNDLE_slot_mask(bundle, 0x0);
#endif
  Set_TI_BUNDLE_slot_count(bundle, ISA_MAX_SLOTS);

}

/* ====================================================================
 *
 *  TI_BUNDLE_Slot_Available
 *
 *  See interface description
 *
 * ====================================================================
 */
BOOL TI_BUNDLE_Slot_Available(
  TI_BUNDLE  *bundle,
  ISA_EXEC_UNIT_PROPERTY property,
  INT        slot
)
{
  INT i, j;
  ISA_EXEC_UNIT_PROPERTY slot_prop;
  BOOL match;

  /* if <slot> is exceeds the bundle slot capacity, return FALSE */
  if (slot >= ISA_MAX_SLOTS) return FALSE;
  
  /* Loop through the bundles unless either a match is found or withdrawn. 
     TODO: Access it through hierarchical (or state) tables */

/* an instruction is compatible with a template slot if its EXEC_UNIT_PROPERTY
   is a superset of the slot's EXEC_UNIT_PROPERTY.  As a special case, consider
   instructions with an EXEC_UNIT_PROPERTY of 0 to always match */

#define SLOTS_COMPATIBLE(_prop, _bundle_index, _slot_index) \
 ((_prop == 0) || \
  ((_prop & ISA_EXEC_Slot_Prop(_bundle_index, _slot_index)) == \
   ISA_EXEC_Slot_Prop(_bundle_index, _slot_index)))
  
  /* Loop through the bundle and slot info. */  
  for (i = 0; i < ISA_MAX_BUNDLES; ++i) {
    match = TRUE;
    for (j = 0; j < ISA_MAX_SLOTS; ++j) {
      slot_prop = (j == slot) ? property : TI_BUNDLE_exec_property(bundle, j);
      if ((j == slot) || TI_BUNDLE_slot_filled(bundle, j) ||
	  TI_BUNDLE_stop_bit(bundle, j)) {
	  if (!SLOTS_COMPATIBLE(slot_prop, i, j) ||
		  (TI_BUNDLE_stop_bit(bundle, j) != ISA_EXEC_Stop(i, j))) {
	    match = FALSE;
	    break;
	}
      }
    }
    if (match) {
      Set_TI_BUNDLE_pack_code(bundle, i);
      return TRUE;
    }
  }
  return FALSE;
}

/* ====================================================================
 *
 *  TI_BUNDLE_Stop_Bit_Available
 *
 *  See interface description
 *
 * ====================================================================
 */
BOOL TI_BUNDLE_Stop_Bit_Available(
  TI_BUNDLE  *bundle,
  INT        slot
)
{
  INT i, j;
  ISA_EXEC_UNIT_PROPERTY slot_prop;
  BOOL stop_bit_value;
  BOOL match;

  /* if <slot> is exceeds the bundle slot capacity, return FALSE */
  if (slot >= ISA_MAX_SLOTS) return FALSE;

  /* Loop through the bundle and slot info. */  
  for (i = 0; i < ISA_MAX_BUNDLES; ++i) {
    match = TRUE;
    for (j = 0; j < ISA_MAX_SLOTS; ++j) {
      stop_bit_value = (j == slot) ? TRUE : TI_BUNDLE_stop_bit(bundle, j);
      if (ISA_EXEC_Stop(i, j) != stop_bit_value) {
	  /* always look at stop bits */
	  match = FALSE;
	  break;
      } else if (TI_BUNDLE_slot_filled(bundle, j)) {
	  /* only look at exec props if slot is filled */
	  slot_prop = TI_BUNDLE_exec_property(bundle, j);
	  if (!SLOTS_COMPATIBLE(slot_prop, i, j)) {
	      match = FALSE;
	      break;
	  }
      }
    }
    if (match) {
      Set_TI_BUNDLE_pack_code(bundle, i);
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef TARG_ST
/* ====================================================================
 *
 *  TI_BUNDLE_Set_Slot_Mask_Property
 *
 *  See interface description
 *
 * ====================================================================
 */
ISA_EXEC_MASK TI_BUNDLE_Set_Slot_Mask_Property(
  ISA_EXEC_MASK slot_mask,
  INT slot,
  ISA_EXEC_UNIT_PROPERTY property)
{
  UINT shift = ISA_TAG_SHIFT * (ISA_MAX_SLOTS - slot - 1);
  UINT word = shift / 64;
  UINT w_shift = shift - (word * 64);

  slot_mask.v[word] |= ((UINT64)property << w_shift);
  if (w_shift > 0) {
    slot_mask.v[word+1] |= ((UINT64)property >> (64 - w_shift));
  }
  return slot_mask;
}

/* ====================================================================
 *
 *  TI_BUNDLE_Get_Slot_Mask_Property
 *
 *  See interface description
 *
 * ====================================================================
 */
ISA_EXEC_UNIT_PROPERTY TI_BUNDLE_Get_Slot_Mask_Property(
  ISA_EXEC_MASK slot_mask,
  INT slot)
{
  UINT64 r;
  UINT shift = ISA_TAG_SHIFT * (ISA_MAX_SLOTS - slot - 1);
  UINT word = shift / 64;
  UINT w_shift = shift - (word * 64);
  r = slot_mask.v[word] >> w_shift;
  if (w_shift > 0) {
    r |= slot_mask.v[word+1] << (64 - w_shift);
  }
  return (ISA_EXEC_UNIT_PROPERTY)(r & ((1 << ISA_TAG_SHIFT) - 1));
}
#endif

/* ====================================================================
 *
 *  TI_BUNDLE_Reserve_Slot
 *
 *  See interface description
 *
 * ====================================================================
 */
void TI_BUNDLE_Reserve_Slot(
  TI_BUNDLE  *bundle,
  INT slot,
  ISA_EXEC_UNIT_PROPERTY property
)
{

  TI_BUNDLE_slot_filled(bundle, slot) = TRUE;
  Set_TI_BUNDLE_exec_property(bundle, slot, property);
#ifdef TARG_ST
  {
    ISA_EXEC_MASK slot_mask;
    slot_mask = TI_BUNDLE_slot_mask(bundle);
    slot_mask = TI_BUNDLE_Set_Slot_Mask_Property(slot_mask, slot, property);
    Set_TI_BUNDLE_slot_mask(bundle, slot_mask);
  }
#else
  TI_BUNDLE_slot_mask(bundle) |= property << 
    (ISA_TAG_SHIFT * (ISA_MAX_SLOTS - slot - 1));
#endif
}

/* ====================================================================
 *
 *  TI_BUNDLE_Reserve_Stop_Bit
 *
 *  See interface description
 *
 * ====================================================================
 */
void TI_BUNDLE_Reserve_Stop_Bit(
  TI_BUNDLE  *bundle,
  INT slot
)
{
  if (slot < 0 || slot > ISA_MAX_SLOTS) {
    fprintf(stderr,"TI_BUNDLE_Reserve_Stop_Bit: slot value not legal value \n");
    assert(FALSE);
  }

  Set_TI_BUNDLE_stop_bit(bundle, slot, TRUE);
}

/* ====================================================================
 *
 *  TI_BUNDLE_Unreserve_Stop_Bit
 *
 *  See interface description
 *
 * ====================================================================
 */
void TI_BUNDLE_Unreserve_Stop_Bit(
  TI_BUNDLE  *bundle,
  INT slot
)
{
  if (slot < 0 || slot > ISA_MAX_SLOTS) {
    fprintf(stderr,"TI_BUNDLE_Reserve_Stop_Bit: slot value not legal value \n");
    assert(FALSE);
  }

  Set_TI_BUNDLE_stop_bit(bundle, slot, FALSE);
}
