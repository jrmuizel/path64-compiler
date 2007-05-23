/*
  Copyright (C) 2002, STMicroelectronics, All Rights Reserved.

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
*/
/*
  (Christian Bruel)
  This file is intended to provide the interface to the target information, 
  scheduling information in the library side.
  On the code generator side, a call to dlsym is made to those to initialize
  proper library interfaces.
  see ti_init.c. 
 */
#include "ti_si.h"
#ifdef TARG_ST
#include <stdlib.h>

static SI_RRW SI_RRW_initial_reservations;
#endif

/****************************************************************************
 ****************************************************************************/

/* SI_RRW -- A resource reservation word */
TARGINFO_EXPORTED
SI_RRW SI_RRW_Initial(void)
{
#ifdef TARG_ST
  return SI_RRW_initializer + SI_RRW_initial_reservations;
#else
  return SI_RRW_initializer;
#endif
}

TARGINFO_EXPORTED
SI_RRW SI_RRW_Has_Overuse( SI_RRW word_with_reservations )
{
  return (word_with_reservations & SI_RRW_overuse_mask) != 0;
}

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
SI_RESOURCE_ID_SET SI_RESOURCE_ID_SET_Universe(void)
{
  return    (SI_RESOURCE_ID_SET)-1
	 >> (sizeof(SI_RESOURCE_ID_SET) * 8 - TI_SI_resource_count);
}

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
const char* SI_ISSUE_SLOT_Name( SI_ISSUE_SLOT* slot )
{
  return slot->name;
}

TARGINFO_EXPORTED
INT SI_ISSUE_SLOT_Skew( SI_ISSUE_SLOT* slot )
{
  return slot->skew;
}

TARGINFO_EXPORTED
INT SI_ISSUE_SLOT_Avail_Per_Cycle( SI_ISSUE_SLOT* slot )
{
  return slot->avail_per_cycle;
}

TARGINFO_EXPORTED
INT SI_ISSUE_SLOT_Count(void)
{
  return SI_issue_slot_count;
}

TARGINFO_EXPORTED
SI_ISSUE_SLOT* SI_Ith_Issue_Slot( UINT i )
{
  return SI_issue_slots[i];
}

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
SI_RESOURCE*
SI_RESOURCE_TOTAL_Resource( SI_RESOURCE_TOTAL* pair )
{
  return pair->resource;
}

TARGINFO_EXPORTED
SI_RESOURCE_ID SI_RESOURCE_TOTAL_Resource_Id( SI_RESOURCE_TOTAL* pair )
{
  return SI_RESOURCE_Id(SI_RESOURCE_TOTAL_Resource(pair));
}

TARGINFO_EXPORTED
UINT SI_RESOURCE_TOTAL_Avail_Per_Cycle(SI_RESOURCE_TOTAL* pair)
{
  return SI_RESOURCE_Avail_Per_Cycle(SI_RESOURCE_TOTAL_Resource(pair));
}

TARGINFO_EXPORTED
INT SI_RESOURCE_TOTAL_Total_Used( SI_RESOURCE_TOTAL* pair )
{
  return pair->total_used;
}

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
UINT SI_RR_Length( SI_RR req )
{
  return (INT) req[0];
}

TARGINFO_EXPORTED
SI_RRW SI_RR_Cycle_RRW( SI_RR req, UINT cycle )
{
  /* TODO: make this compilable with and without defs.h 
  assert(cycle <= req[0]);
  */
  return req[cycle+1];
}

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
const char* TSI_Name( TOP top )
{
  return SI_top_si[(INT) top]->name;
}

TARGINFO_EXPORTED
SI_ID TSI_Id( TOP top )
{
  return SI_top_si[top]->id;
}

TARGINFO_EXPORTED
INT
TSI_Operand_Access_Time( TOP top, INT operand_index )
{
  return SI_top_si[(INT) top]->operand_access_times[operand_index];
}

#ifdef TARG_ST
TARGINFO_EXPORTED
BOOL
TSI_Operand_Access_Times_Overridden ( TOP top )
{
  TI_SI_CONST SI *si = SI_top_si[(INT) top];
  return si->operand_access_times_overridden;
}

TARGINFO_EXPORTED
void
TSI_Set_Operand_Access_Time ( TOP top, INT operand_index, INT tm )
{
  TI_SI_CONST SI *si = SI_top_si[(INT) top];
  // Care: si can be null if the operation does not exist on this
  // target.  We allow that case but silently ignore it, so that
  // command-line options can be shared across targets.
  if (si) {
    if (! si->operand_access_times_overridden) {
      // Cannot simply overwrite the current value, since it is
      // shared with other instructions. First make a duplicate.
      SI *new_si = (SI *)malloc (sizeof (SI));
      if (!new_si) return;
      memcpy (new_si, si, sizeof(SI));
      SI_top_si[(INT) top] = new_si;
      si = new_si;
      mUINT8 *new_access_times = (mUINT8 *)malloc (sizeof(mUINT8)
						   * si->n_opds);
      if (!new_access_times) return;
      memcpy (new_access_times, si->operand_access_times,
	      si->n_opds * sizeof (mUINT8));
      si->operand_access_times = new_access_times;
      si->operand_access_times_overridden = TRUE;
    }
    si->operand_access_times[operand_index] = tm;
  }
}

#endif

TARGINFO_EXPORTED
INT
TSI_Result_Available_Time( TOP top, INT result_index )
{
  return SI_top_si[(INT) top]->result_available_times[result_index];
}

#ifdef TARG_ST
TARGINFO_EXPORTED
BOOL TSI_Result_Available_Times_Overridden ( TOP top )
{
  TI_SI_CONST SI *si = SI_top_si[(INT) top];
  return si->result_available_times_overridden;
}

TARGINFO_EXPORTED
void
TSI_Set_Result_Available_Time( TOP top, INT result_index, INT tm )
{
  TI_SI_CONST SI *si = SI_top_si[(INT) top];
  // Care: si can be null if the operation does not exist on this
  // target.  We allow that case but silently ignore it, so that
  // command-line options can be shared across targets.
  if (si) {
    if (! si->result_available_times_overridden) {
      // Cannot simply overwrite the current value, since it is
      // shared with other instructions. First make a duplicate.
      SI *new_si = (SI *)malloc (sizeof (SI));
      if (!new_si) return;
      memcpy (new_si, si, sizeof(SI));
      SI_top_si[(INT) top] = new_si;
      si = new_si;
      mUINT8 *new_access_times = (mUINT8 *)malloc (sizeof(mUINT8)
						   * si->n_results);
      if (!new_access_times) return;
      memcpy (new_access_times, si->result_available_times,
	      si->n_results * sizeof (mUINT8));
      new_access_times[result_index] = tm;
      si->result_available_times_overridden = TRUE;
    }
    si->result_available_times[result_index] = tm;
  }
}

#endif

TARGINFO_EXPORTED
INT
TSI_Load_Access_Time( TOP top )
{
  return SI_top_si[(INT) top]->load_access_time;
}

TARGINFO_EXPORTED
INT
TSI_Last_Issue_Cycle( TOP top )
{
  return SI_top_si[(INT) top]->last_issue_cycle;
}

TARGINFO_EXPORTED
INT
TSI_Store_Available_Time( TOP top )
{
  return SI_top_si[(INT) top]->store_available_time;
}

TARGINFO_EXPORTED
SI_RR TSI_Resource_Requirement( TOP top )
{
  return SI_top_si[(INT) top]->rr;
}

TARGINFO_EXPORTED
SI_BAD_II_SET TSI_Bad_IIs( TOP top )
{
  return SI_top_si[(INT) top]->bad_iis;
}

TARGINFO_EXPORTED
SI_RR TSI_II_Resource_Requirement( TOP top, INT ii )
{
  SI* const info = SI_top_si[(INT) top];

  if ( ii > info->ii_info_size ) return info->rr;

  return info->ii_rr[ii - 1];
}

TARGINFO_EXPORTED
const SI_RESOURCE_ID_SET*
TSI_II_Cycle_Resource_Ids_Used( TOP opcode, INT ii )
{
  SI* const info = SI_top_si[(INT)opcode];
  if ( ii > info->ii_info_size ) return info->resources_used;

  return info->ii_resources_used[ii - 1];
}

TARGINFO_EXPORTED
UINT TSI_Valid_Issue_Slot_Count( TOP top )
{
  return SI_top_si[(INT) top]->valid_issue_slot_count;
}

TARGINFO_EXPORTED
SI_ISSUE_SLOT* TSI_Valid_Issue_Slots( TOP top, UINT i )
{
  return SI_top_si[(INT) top]->valid_issue_slots[i];
}

TARGINFO_EXPORTED
UINT TSI_Resource_Total_Vector_Size( TOP top )
{
  return SI_top_si[(INT) top]->resource_total_vector_size;
}

TARGINFO_EXPORTED
SI_RESOURCE_TOTAL* TSI_Resource_Total_Vector( TOP top )
{
  return SI_top_si[(INT) top]->resource_total_vector;
}

TARGINFO_EXPORTED
INT TSI_Write_Write_Interlock( TOP top )
{
  return SI_top_si[(INT) top]->write_write_interlock;
}

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
const char* SI_RESOURCE_Name( SI_RESOURCE* res )
{
  return res->name;
}

TARGINFO_EXPORTED
UINT SI_RESOURCE_Id( SI_RESOURCE* res )
{
  return res->id;
}

TARGINFO_EXPORTED
UINT SI_RESOURCE_Avail_Per_Cycle( SI_RESOURCE* res )
{
  return res->avail_per_cycle;
}

TARGINFO_EXPORTED
UINT SI_RESOURCE_Word_Index( SI_RESOURCE* res )
{
  return res->word_index;
}

TARGINFO_EXPORTED
UINT SI_RESOURCE_Bit_Index( SI_RESOURCE* res )
{
  return res->bit_index;
}

TARGINFO_EXPORTED
const char* SI_RESOURCE_ID_Name( SI_RESOURCE_ID id )
{
  return SI_RESOURCE_Name(SI_resources[id]);
}

TARGINFO_EXPORTED
UINT SI_RESOURCE_ID_Avail_Per_Cycle( SI_RESOURCE_ID id )
{
  return SI_RESOURCE_Avail_Per_Cycle(SI_resources[id]);
}

/****************************************************************************
 ****************************************************************************/
#ifdef TARG_ST
static INT
get_field_width(INT count) 
{
  INT i;
  INT field_width;
  for ( i = 31 ; i >= 0 ; --i ) {
    if ((( (INT) 1) << i) & count) {
      field_width = i + 2;
      break;
    }
  }
  return field_width;
}

void 
SI_RESOURCE_ID_Set_Max_Avail(SI_RESOURCE_ID id, INT max)
{
  INT avail_per_cycle = SI_resources[id]->avail_per_cycle;
  INT bit_index = SI_resources[id]->bit_index;
  INT pre_reserve =  avail_per_cycle - max;
  SI_RRW pre_requierement;
  SI_RRW pre_requierement_mask;
  INT r;
  INT si_id;
  if (pre_reserve <= 0) return;

  pre_requierement = ((SI_RRW)pre_reserve << bit_index);
  pre_requierement_mask = (((SI_RRW)-1) >> (sizeof(SI_RRW)*8 - get_field_width(avail_per_cycle)) << bit_index);
  SI_RRW_initial_reservations = 
    (SI_RRW_initial_reservations & ~pre_requierement_mask)
    | pre_requierement;

  /* Now fix SI informations that overflow the max resource. */
  for (si_id = 0; si_id < SI_ID_count; si_id++) {
    SI *si = SI_ID_si[si_id];
    SI_RESOURCE_ID_SET *set;
    if (si == (void *)0) continue;
    set = si->resources_used;
    if (set == (void *)0) continue;
    if (*set & (1<<id)) {
      for (r = 0; r < si->resource_total_vector_size; r++) {
	if (si->resource_total_vector[r].resource->id == id) {
         if (si->resource_total_vector[r].total_used > max) {
	    si->resource_total_vector[r].total_used = max;
	  }
	}
      }
      for (r = 0; r < si->rr[0]; r++) {
	SI_RRW reservation = si->rr[1+r];
	INT num = (reservation & pre_requierement_mask) >> bit_index;
	if (num > max) {
	  reservation = (reservation & ~pre_requierement_mask)
	    | (((SI_RRW)max << bit_index));
	  si->rr[1+r] = reservation;
	}
      }
    }
  }
}
#endif

/****************************************************************************
 ****************************************************************************/
TARGINFO_EXPORTED
INT SI_ID_Count(void)
{
  return SI_ID_count;
}

TARGINFO_EXPORTED
const SI_RESOURCE_ID_SET*
SI_ID_II_Cycle_Resource_Ids_Used( SI_ID id, INT ii )
{
  SI* const info = SI_ID_si[id];
  if ( ii > info->ii_info_size ) return info->resources_used;

  return info->ii_resources_used[ii - 1];
}

/****************************************************************************
 Need read/write accesses to SI tables for Reconfigurability
 ****************************************************************************/
#ifdef TARG_ST
TARGINFO_EXPORTED
extern void Set_SI_resource_count(INT i) {
  TI_SI_resource_count = i;
}

TARGINFO_EXPORTED
extern void Set_SI_resources(SI_RESOURCE * TI_SI_CONST* t) {
  SI_resources = t;
}

TARGINFO_EXPORTED
extern void Set_SI_RRW_initializer (TI_SI_CONST SI_RRW i) {
  SI_RRW_initializer = i;
}

TARGINFO_EXPORTED
extern void Set_SI_RRW_overuse_mask(TI_SI_CONST SI_RRW i) {
  SI_RRW_overuse_mask = i;
}

TARGINFO_EXPORTED
extern void Set_SI_issue_slot_count(INT i) {
  SI_issue_slot_count = i;
}

TARGINFO_EXPORTED
extern void Set_SI_issue_slots(SI_ISSUE_SLOT * TI_SI_CONST* t) {
  SI_issue_slots = t;
}

TARGINFO_EXPORTED
void Set_SI_ID_count(int v) {
  SI_ID_count = v;
}

TARGINFO_EXPORTED
void Set_SI_ID_si(SI * TI_SI_CONST * tab) {
  SI_ID_si = tab;
}

TARGINFO_EXPORTED
void Set_SI_top_si(SI * TI_SI_CONST * tab) {
  SI_top_si = tab;
}

TARGINFO_EXPORTED
extern INT Get_SI_resource_count(void) {
  return (TI_SI_resource_count);
}

TARGINFO_EXPORTED
extern SI_RESOURCE * TI_SI_CONST* Get_SI_resources(void) {
  return (SI_resources);
}

TARGINFO_EXPORTED
extern TI_SI_CONST SI_RRW Get_SI_RRW_initializer (void) {
  return (SI_RRW_initializer);
}

TARGINFO_EXPORTED
extern TI_SI_CONST SI_RRW Get_SI_RRW_overuse_mask(void) {
  return (SI_RRW_overuse_mask);
}

TARGINFO_EXPORTED
extern INT Get_SI_issue_slot_count(void) {
  return (SI_issue_slot_count);
}

TARGINFO_EXPORTED
extern SI_ISSUE_SLOT * TI_SI_CONST* Get_SI_issue_slots(void) {
  return (SI_issue_slots);
}

TARGINFO_EXPORTED
SI * TI_SI_CONST * Get_SI_ID_si(void) {
  return (SI_ID_si);
}

TARGINFO_EXPORTED
SI * TI_SI_CONST * Get_SI_top_si(void) {
  return (SI_top_si);
}
#endif

