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

/****************************************************************************
 ****************************************************************************/

/* SI_RRW -- A resource reservation word */
TARGINFO_EXPORTED
SI_RRW SI_RRW_Initial(void)
{
  return SI_RRW_initializer;
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
	 >> (sizeof(SI_RESOURCE_ID_SET) * 8 - SI_resource_count);
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

TARGINFO_EXPORTED
INT
TSI_Result_Available_Time( TOP top, INT result_index )
{
  return SI_top_si[(INT) top]->result_available_times[result_index];
}

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

