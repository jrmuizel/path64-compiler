/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */
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


#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
static const char rcs_id[] = "$Source: /home/bos/bk/kpro64-pending/common/targ_info/access/SCCS/s.ti_init.c $ $Revision: 1.5 $";
#endif /* _KEEP_RCS_ID */

#if !defined(__FreeBSD__)
#include <alloca.h>
#endif
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "targ_isa_subset.h"
#include "targ_isa_hazards.h"
#include "targ_isa_registers.h"
#include "targ_abi_properties.h"
#include "targ_proc.h"
#include "dso.h"
#include "errors.h"

#include "ti_init.h"

#ifdef TARG_ST
static void * targ_handler = NULL;
#endif
#ifndef USE_WEAK_REFERENCES
const char * sanity_check_targ_so_name_p;
#define sanity_check_targ_so_name sanity_check_targ_so_name_p
#endif

#ifdef TARG_ST
#include <dlfcn.h>
#include "ti_si.h"
//#include "W_dlfcn.h"		    /* for sgidladd(), dlerror() */

/****************************************************************************
 ****************************************************************************/

typedef SI_RRW si_rrw_initial_t(void);
typedef SI_RRW si_rrw_has_overuse_t(SI_RRW);

SI_RRW SI_RRW_Initial(void)
{
  static si_rrw_initial_t* so_call = 0;
  if (!so_call && !(so_call = (si_rrw_initial_t*)dlsym(targ_handler, "SI_RRW_Initial")))
    return 0;
		   
  return so_call();
}

SI_RRW SI_RRW_Has_Overuse(SI_RRW word_with_reservations)
{
  static si_rrw_has_overuse_t* so_call= 0;

  if (!so_call && !(so_call = (si_rrw_has_overuse_t*)
		    dlsym(targ_handler, "SI_RRW_Has_Overuse")))
    return 0;

  return so_call(word_with_reservations);
}

/****************************************************************************
 ****************************************************************************/
typedef SI_RESOURCE_ID_SET si_resource_id_set_universe_t(void);

SI_RESOURCE_ID_SET SI_RESOURCE_ID_SET_Universe(void)
{
  static si_resource_id_set_universe_t* so_call= 0;

  if (!so_call && !(so_call = (si_resource_id_set_universe_t*)
		    dlsym(targ_handler, "SI_RESOURCE_ID_SET_Universe")))
    return 0;

  return so_call();
}

/****************************************************************************
 ****************************************************************************/
typedef const char * si_issue_slot_name_t (SI_ISSUE_SLOT*);
typedef int si_issue_slot_skew_t(SI_ISSUE_SLOT*);
typedef int si_issue_slot_avail_per_cycle_t(SI_ISSUE_SLOT*);
typedef void si_resource_id_set_max_avail_t( SI_RESOURCE_ID, INT);
typedef void void_func_t( void);
typedef int si_issue_slot_count_t(void);
typedef SI_ISSUE_SLOT * si_ith_issue_slot_t(UINT);

const char* SI_ISSUE_SLOT_Name( SI_ISSUE_SLOT* slot )
{
  static si_issue_slot_name_t * so_call= 0;

  if (!so_call && !(so_call = (si_issue_slot_name_t*)
		    dlsym(targ_handler, "SI_ISSUE_SLOT_Name")))
    return NULL;

  return so_call(slot);
}

INT SI_ISSUE_SLOT_Skew( SI_ISSUE_SLOT* slot )
{
  static si_issue_slot_skew_t* so_call= 0;

  if (!so_call && !(so_call = (si_issue_slot_skew_t*)
		    dlsym(targ_handler, "SI_ISSUE_SLOT_Skew")))
    return 0;

  return so_call(slot);
}

INT SI_ISSUE_SLOT_Avail_Per_Cycle( SI_ISSUE_SLOT* slot )
{
  static si_issue_slot_avail_per_cycle_t* so_call= 0;

  if (!so_call && !(so_call = (si_issue_slot_avail_per_cycle_t*)
		    dlsym(targ_handler, "SI_ISSUE_SLOT_Avail_Per_Cycle")))
    return 0;

  return so_call(slot);
}

INT SI_ISSUE_SLOT_Count(void)
{
  static si_issue_slot_count_t* so_call= 0;

  if (!so_call && !(so_call = (si_issue_slot_count_t*)
		    dlsym(targ_handler, "SI_ISSUE_SLOT_Count")))
    return 0;

  return so_call();
}

SI_ISSUE_SLOT* SI_Ith_Issue_Slot( UINT i )
{
  static si_ith_issue_slot_t* so_call= 0;

  if (!so_call && !(so_call = (si_ith_issue_slot_t*)
		    dlsym(targ_handler, "SI_Ith_Issue_Slot")))
    return NULL;

  return so_call(i);
}

typedef SI_RESOURCE* si_resource_total_resource_t (SI_RESOURCE_TOTAL*);
typedef SI_RESOURCE_ID si_resource_total_resource_id_t (SI_RESOURCE_TOTAL*);
typedef UINT si_resource_total_avail_per_cycle_t(SI_RESOURCE_TOTAL*);
typedef INT si_resource_total_total_used_t(SI_RESOURCE_TOTAL*);

/****************************************************************************
 ****************************************************************************/

SI_RESOURCE*
SI_RESOURCE_TOTAL_Resource( SI_RESOURCE_TOTAL* pair )
{
  static si_resource_total_resource_t* so_call=0;

  if (!so_call && !(so_call = (si_resource_total_resource_t*)
		    dlsym(targ_handler, "SI_RESOURCE_TOTAL_Resource")))
    return NULL;

  return so_call(pair);
}

SI_RESOURCE_ID SI_RESOURCE_TOTAL_Resource_Id( SI_RESOURCE_TOTAL* pair )
{
  static si_resource_total_resource_id_t* so_call=0;

  if (!so_call && !(so_call = (si_resource_total_resource_id_t*)
		    dlsym(targ_handler, "SI_RESOURCE_TOTAL_Resource_Id")))
    return 0;

  return so_call(pair);
}

UINT SI_RESOURCE_TOTAL_Avail_Per_Cycle(SI_RESOURCE_TOTAL* pair)
{
  static si_resource_total_avail_per_cycle_t* so_call=0;

  if (!so_call && !(so_call = (si_resource_total_avail_per_cycle_t*)
		    dlsym(targ_handler, "SI_RESOURCE_TOTAL_Avail_Per_Cycle")))
    return 0;

  return so_call(pair);
}

INT SI_RESOURCE_TOTAL_Total_Used( SI_RESOURCE_TOTAL* pair )
{
  static si_resource_total_total_used_t* so_call=0;

  if (!so_call && !(so_call = (si_resource_total_total_used_t*)
		    dlsym(targ_handler, "SI_RESOURCE_TOTAL_Total_Used")))
    return 0;

  return so_call(pair);
}

/****************************************************************************
 ****************************************************************************/
typedef UINT si_rr_length_t (SI_RR);
typedef SI_RRW si_rr_cycle_rrw_t (SI_RR, UINT);

UINT SI_RR_Length( SI_RR req )
{
  static si_rr_length_t* so_call=0;

  if (!so_call && !(so_call = (si_rr_length_t*)
		    dlsym(targ_handler, "SI_RR_Length")))
    return 0;

  return so_call(req);
}

SI_RRW SI_RR_Cycle_RRW( SI_RR req, UINT cycle )
{
  static si_rr_cycle_rrw_t* so_call=0;

  if (!so_call && !(so_call = (si_rr_cycle_rrw_t*)
		    dlsym(targ_handler, "SI_RR_Cycle_RRW")))
    return 0;

  return so_call(req, cycle);
}

/****************************************************************************
 ****************************************************************************/
typedef SI_RR tsi_resource_requirement_t(TOP);
typedef SI_RR tsi_ii_resource_requirement_t(TOP, INT);
typedef const SI_RESOURCE_ID_SET* tsi_ii_cycle_resource_ids_used_t(TOP, INT);
typedef SI_BAD_II_SET tsi_bad_iis_t(TOP);
typedef SI_ID tsi_id_t(TOP);
typedef INT tsi_resource_total_vector_size_t(TOP);
typedef SI_RESOURCE_TOTAL * tsi_resource_total_vector_t(TOP);

SI_RR TSI_Resource_Requirement(TOP top)
{
  static tsi_resource_requirement_t* so_call=0;

  if (!so_call && !(so_call = (tsi_resource_requirement_t*)
		    dlsym(targ_handler, "TSI_Resource_Requirement")))
    return NULL;

  return so_call(top);
}

SI_RR TSI_II_Resource_Requirement(TOP top, INT ii)
{
  static tsi_ii_resource_requirement_t* so_call = 0;

  if (!so_call && !(so_call = (tsi_ii_resource_requirement_t*)
		    dlsym(targ_handler, "TSI_II_Resource_Requirement")))
    return NULL;

  return so_call(top, ii);
}

const SI_RESOURCE_ID_SET*
TSI_II_Cycle_Resource_Ids_Used( TOP opcode, INT ii )
{
  static tsi_ii_cycle_resource_ids_used_t* so_call=0;

  if (!so_call && !(so_call = (tsi_ii_cycle_resource_ids_used_t*)
		    dlsym(targ_handler, "TSI_II_Cycle_Resource_Ids_Used")))
    return NULL;

  return so_call (opcode, ii);
}

SI_BAD_II_SET TSI_Bad_IIs( TOP top )
{
  static tsi_bad_iis_t* so_call=0;

  if (!so_call)
    so_call = (tsi_bad_iis_t*) dlsym(targ_handler, "TSI_Bad_IIs");

  return so_call (top);
}

SI_ID TSI_Id( TOP top )
{
  static tsi_id_t* so_call=0;

  if (!so_call && !(so_call = (tsi_id_t*)
		    dlsym(targ_handler, "TSI_Id")))
    return 0;

  return so_call(top);
}

UINT TSI_Resource_Total_Vector_Size( TOP top )
{
  static tsi_resource_total_vector_size_t* so_call=0;

  if (!so_call && !(so_call = (tsi_resource_total_vector_size_t*)
		    dlsym(targ_handler, "TSI_Resource_Total_Vector_Size")))
    return 0;

  return so_call(top);
}

SI_RESOURCE_TOTAL* TSI_Resource_Total_Vector( TOP top )
{
  static tsi_resource_total_vector_t* so_call=0;

  if (!so_call && !(so_call = (tsi_resource_total_vector_t*)
		    dlsym(targ_handler, "TSI_Resource_Total_Vector")))
    return NULL;

  return so_call(top);
}

/****************************************************************************
 ****************************************************************************/

typedef const char* si_resource_name_t( SI_RESOURCE*);
typedef UINT si_resource_id_t( SI_RESOURCE*);
typedef UINT si_resource_avail_per_cycle_t( SI_RESOURCE*);
typedef UINT si_resource_word_index_t( SI_RESOURCE*);
typedef UINT si_resource_bit_index_t( SI_RESOURCE*);
typedef const char* si_resource_id_name_t( SI_RESOURCE_ID);
typedef UINT si_resource_id_avail_per_cycle_t( SI_RESOURCE_ID);


const char* SI_RESOURCE_Name( SI_RESOURCE* res )
{
  static si_resource_name_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_name_t*)
		     dlsym(targ_handler, "SI_RESOURCE_Name")))
    return NULL;

  return so_call (res);
}

UINT SI_RESOURCE_Id( SI_RESOURCE* res )
{
  static si_resource_id_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_id_t*)
		     dlsym(targ_handler, "SI_RESOURCE_Id")))
    return 0;

  return so_call (res);
}

UINT SI_RESOURCE_Avail_Per_Cycle( SI_RESOURCE* res )
{
  static si_resource_avail_per_cycle_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_avail_per_cycle_t*)
		     dlsym(targ_handler, "SI_RESOURCE_Avail_Per_Cycle")))
    return 0;

  return so_call (res);
}

UINT SI_RESOURCE_Word_Index( SI_RESOURCE* res )
{
  static si_resource_word_index_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_word_index_t*)
		     dlsym(targ_handler, "SI_RESOURCE_Word_Index")))
    return 0;

  return so_call (res);
}

UINT SI_RESOURCE_Bit_Index( SI_RESOURCE* res )
{
  static si_resource_bit_index_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_bit_index_t*)
		     dlsym(targ_handler, "SI_RESOURCE_Bit_Index")))
    return 0;

  return so_call (res);
}

const char* SI_RESOURCE_ID_Name( SI_RESOURCE_ID id )
{
  static si_resource_id_name_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_id_name_t*)
		     dlsym(targ_handler, "SI_RESOURCE_ID_Name")))
    return NULL;

  return  so_call (id);
}

UINT SI_RESOURCE_ID_Avail_Per_Cycle( SI_RESOURCE_ID id )
{
  static si_resource_id_avail_per_cycle_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_id_avail_per_cycle_t*)
		     dlsym(targ_handler, "SI_RESOURCE_ID_Avail_Per_Cycle")))
    return 0;

  return so_call (id);
}

void SI_RESOURCE_ID_Set_Max_Avail(SI_RESOURCE_ID id, INT max)
{
  static si_resource_id_set_max_avail_t* so_call = 0;
  if (! so_call && !(so_call = (si_resource_id_set_max_avail_t*)
		     dlsym(targ_handler, "SI_RESOURCE_ID_Set_Max_Avail")))
    return;
  
  return so_call (id, max);
}

void backup_resource_tables(void)
{
  static void_func_t* so_call = 0;
  if (! so_call && !(so_call = (void_func_t*)
		     dlsym(targ_handler, "backup_resource_tables")))
    return;
  
  return so_call ();
}
void restore_resource_tables(void)
{
  static void_func_t* so_call = 0;
  if (! so_call && !(so_call = (void_func_t*)
		     dlsym(targ_handler, "restore_resource_tables")))
    return;
  
  return so_call ();
}

/****************************************************************************
 ****************************************************************************/
INT TSI_Operand_Access_Time( TOP top, INT operand_index ) 
{
    typedef INT tsi_operand_access_time_t (TOP, INT)  ;
    static  tsi_operand_access_time_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_operand_access_time_t*)
		      dlsym(targ_handler, "TSI_Operand_Access_Time")))
	return 0;
    
    return so_call(top, operand_index);
}

#ifdef TARG_ST
void TSI_Set_Operand_Access_Time( TOP top, INT operand_index, INT tm ) 
{
    typedef void tsi_set_operand_access_time_t (TOP, INT, INT)  ;
    static  tsi_set_operand_access_time_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_set_operand_access_time_t*)
		      dlsym(targ_handler, "TSI_Set_Operand_Access_Time")))
	return;
    
    so_call(top, operand_index, tm);
}

BOOL TSI_Operand_Access_Times_Overridden ( TOP top )
{
    typedef BOOL tsi_operand_access_times_overridden_t (TOP)  ;
    static  tsi_operand_access_times_overridden_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_operand_access_times_overridden_t*)
		      dlsym(targ_handler, "TSI_Operand_Access_Times_Overridden")))
	return 0;
    
    return so_call(top);
}
#endif

INT TSI_Result_Available_Time( TOP top, INT result_index )
{
    typedef INT tsi_result_available_time_t ( TOP , INT ) ;
    static tsi_result_available_time_t*  so_call = 0 ;
    if (!so_call && !(so_call = (tsi_result_available_time_t*)
		      dlsym(targ_handler, "TSI_Result_Available_Time")))
	return 0;
    
    return so_call(top, result_index);
}

#ifdef TARG_ST
void TSI_Set_Result_Available_Time( TOP top, INT result_index, INT tm )
{
    typedef void tsi_set_result_available_time_t ( TOP , INT , INT ) ;
    static tsi_set_result_available_time_t*  so_call = 0 ;
    if (!so_call && !(so_call = (tsi_set_result_available_time_t*)
		      dlsym(targ_handler, "TSI_Set_Result_Available_Time")))
	return;
    
    so_call(top, result_index, tm);
}

BOOL TSI_Result_Available_Times_Overridden ( TOP top )
{
    typedef BOOL tsi_result_available_times_overridden_t (TOP)  ;
    static  tsi_result_available_times_overridden_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_result_available_times_overridden_t*)
		      dlsym(targ_handler, "TSI_Result_Available_Times_Overridden")))
	return 0;
    
    return so_call(top);
}
#endif

INT TSI_Load_Access_Time( TOP top )
{
    typedef INT tsi_load_access_time_t  (TOP ) ;
    static  tsi_load_access_time_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_load_access_time_t*)
		      dlsym(targ_handler, "TSI_Load_Access_Time")))
	return 0;
    
    return so_call(top);
}

INT TSI_Last_Issue_Cycle( TOP top ) 
{
    typedef INT tsi_last_issue_cycle_t (TOP ) ;
    static tsi_last_issue_cycle_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_last_issue_cycle_t *)
		      dlsym(targ_handler, "TSI_Last_Issue_Cycle")))
	return 0;
    
    return so_call(top);
}

INT TSI_Store_Available_Time( TOP top )
{
    typedef INT tsi_store_available_time_t (TOP ) ;
    static tsi_store_available_time_t* so_call = 0 ;
    if (!so_call && !(so_call = (tsi_store_available_time_t *)
		      dlsym(targ_handler, "TSI_Store_Available_Time")))
	return 0;
    
    return so_call(top);
}

/****************************************************************************
 Need read/write accesses to SI tables for Reconfigurability
 ****************************************************************************/

typedef SI * TI_SI_CONST * get_si_pp_t(void);
typedef void set_si_pp_t(SI * TI_SI_CONST *);
typedef void set_si_RRW_t(SI_RRW);
typedef SI_RRW get_si_RRW_t(void);

void Set_SI_resource_count(INT i)
{
    typedef void set_si_rc_t(INT);
    static set_si_rc_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_rc_t*)
		       dlsym(targ_handler, "Set_SI_resource_count"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_resource_count'!\n"));
	return ;
    }    

    so_call (i);
}

void Set_SI_resources(SI_RESOURCE * TI_SI_CONST* tab)
{
    typedef void set_si_res_t(SI_RESOURCE * TI_SI_CONST *);
    static set_si_res_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_res_t*)
		       dlsym(targ_handler, "Set_SI_resources"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_resources'!\n"));
	return ;
    }    

    so_call (tab);
}

void Set_SI_RRW_initializer(SI_RRW i)
{
    static set_si_RRW_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_RRW_t*)
		       dlsym(targ_handler, "Set_SI_RRW_initializer"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_RRW_initializer'!\n"));
	return ;
    }    

    so_call (i);
}

void Set_SI_RRW_overuse_mask(SI_RRW i)
{
    static set_si_RRW_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_RRW_t*)
		       dlsym(targ_handler, "Set_SI_RRW_overuse_mask"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_RRW_overuse_mask'!\n"));
	return ;
    }    

    so_call (i);
}

void Set_SI_issue_slot_count(INT i)
{
    typedef void set_si_is_count_t(INT);
    static set_si_is_count_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_is_count_t*)
		       dlsym(targ_handler, "Set_SI_issue_slot_count"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_issue_slot_count'!\n"));
	return ;
    }    

    so_call (i);
}

void Set_SI_issue_slots(SI_ISSUE_SLOT * TI_SI_CONST* tab)
{
    typedef void set_si_is_t(SI_ISSUE_SLOT * TI_SI_CONST *);
    static set_si_is_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_is_t*)
		       dlsym(targ_handler, "Set_SI_issue_slots"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_issue_slots'!\n"));
	return ;
    }    

    so_call (tab);
}

void Set_SI_ID_si(SI * TI_SI_CONST * t)
{
    static set_si_pp_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_pp_t*)
		       dlsym(targ_handler, "Set_SI_ID_si"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_ID_si'!\n"));
	return ;
    }    

    so_call (t);
}

void Set_SI_top_si(SI * TI_SI_CONST * t)
{
    static set_si_pp_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_pp_t*)
		       dlsym(targ_handler, "Set_SI_top_si"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_top_si'!\n"));
	return ;
    }
    
    so_call (t);
}

void Set_SI_ID_count(INT i)
{
    typedef void set_si_id_count_t(INT);
    static set_si_id_count_t* so_call = 0;
    if (! so_call && !(so_call = (set_si_id_count_t*)
		       dlsym(targ_handler, "Set_SI_ID_count"))) {
	DevWarn(("Cannot retrieve symbol 'Set_SI_ID_count'!\n"));
	return ;
    }
    
    so_call (i);
}

INT Get_SI_resource_count(void)
{
    typedef INT get_si_rc_t(void);
    static get_si_rc_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_rc_t*)
		       dlsym(targ_handler, "Get_SI_resource_count"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_resource_count'!\n"));
	return 0;
    }
    
    return so_call ();
}

extern SI_RESOURCE * TI_SI_CONST* Get_SI_resources(void)
{
    typedef SI_RESOURCE * TI_SI_CONST* get_si_RES_pp_t(void);
    static get_si_RES_pp_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_RES_pp_t*)
		       dlsym(targ_handler, "Get_SI_resources"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_resources'!\n"));
	return 0;
    }
    
    return so_call ();    
}

extern TI_SI_CONST SI_RRW Get_SI_RRW_initializer(void)
{
    static get_si_RRW_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_RRW_t*)
		       dlsym(targ_handler, "Get_SI_RRW_initializer"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_RRW_initializer'!\n"));
	return 0;
    }
    
    return so_call ();    
}

extern TI_SI_CONST SI_RRW Get_SI_RRW_overuse_mask(void)
{
    static get_si_RRW_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_RRW_t*)
		       dlsym(targ_handler, "Get_SI_RRW_overuse_mask"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_RRW_overuse_mask'!\n"));
	return 0;
    }
    
    return so_call ();    
}

INT Get_SI_issue_slot_count(void)
{
    typedef INT get_si_is_count_t(void);
    static get_si_is_count_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_is_count_t*)
		       dlsym(targ_handler, "Get_SI_issue_slot_count"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_issue_slot_count'!\n"));
	return 0;
    }
    
    return so_call ();
}

extern SI_ISSUE_SLOT * TI_SI_CONST* Get_SI_issue_slots(void)
{
    typedef SI_ISSUE_SLOT * TI_SI_CONST* get_si_is_pp_t(void);
    static get_si_is_pp_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_is_pp_t*)
		       dlsym(targ_handler, "Get_SI_issue_slots"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_issue_slots'!\n"));
	return 0;
    }
    
    return so_call ();    
}

SI * TI_SI_CONST * Get_SI_ID_si(void)
{
    static get_si_pp_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_pp_t*)
		       dlsym(targ_handler, "Get_SI_ID_si"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_ID_si'!\n"));
	return 0;
    }
    
    return so_call ();
}

SI * TI_SI_CONST * Get_SI_top_si(void)
{
    static get_si_pp_t* so_call = 0;
    if (! so_call && !(so_call = (get_si_pp_t*)
		       dlsym(targ_handler, "Get_SI_top_si"))) {
	DevWarn(("Cannot retrieve symbol 'Get_SI_top_si'!\n"));
	return 0;
    }
    
    return so_call ();
}


/****************************************************************************
 ****************************************************************************/

typedef INT si_id_count_t(void);
typedef const SI_RESOURCE_ID_SET* si_id_ii_cycle_resource_ids_used_t(SI_ID, INT);

INT SI_ID_Count(void)
{
  static si_id_count_t* so_call = 0;
  if (! so_call && !(so_call = (si_id_count_t*)
		     dlsym(targ_handler, "SI_ID_Count")))
    return 0;

  return so_call ();
}

const SI_RESOURCE_ID_SET*
SI_ID_II_Cycle_Resource_Ids_Used( SI_ID id, INT ii )
{
  static si_id_ii_cycle_resource_ids_used_t* so_call = 0;
  if (! so_call && !(so_call = (si_id_ii_cycle_resource_ids_used_t*)
		     dlsym(targ_handler, "SI_ID_II_Cycle_Resource_Ids_Used")))
    return NULL;

  return so_call (id, ii);
}

int SI_resource_count;

/****************************************************************************
 ****************************************************************************/
/*
 * SI_Resource_count information is stored both in targ_so library and in 
 * back-end executable (for fast access)
 * Here, we retrieve the information from the library and store it in
 * SI_resource_count variable.
 */
static void
Update_SI_Resource_Count(void)
{    
    SI_resource_count = Get_SI_resource_count();
}
#endif

/* ====================================================================
 *
 *  TI_Initialize
 *
 *  See interface description
 *
 * ====================================================================
 */
#ifdef TARG_ST
void*
TI_Initialize(ABI_PROPERTIES_ABI tabi, 
              ISA_SUBSET tisa, 
              PROCESSOR tproc, 
              INT opt_subsets_count, 
              const ISA_SUBSET *opt_subsets, 
              char *tpath)
#else
void
TI_Initialize(ABI_PROPERTIES_ABI tabi, 
              ISA_SUBSET tisa, 
              PROCESSOR tproc, 
              char *tpath)
#endif
{
  static BOOL initialized;

  if ( !initialized ) {
    INT                i;
    const char        *targ_name     = PROCESSOR_Name(tproc);
    INT                targ_name_len = strlen(targ_name);
    char              *targ_so_name  = alloca(targ_name_len + sizeof(".so"));

    for (i = 0; i < targ_name_len; i++) {
      targ_so_name[i] = tolower(targ_name[i]);
    }
    strcpy(targ_so_name + targ_name_len, ".so");

#ifdef TARG_ST
    targ_handler = load_so(targ_so_name, tpath, FALSE /*verbose*/);
#else
    load_so(targ_so_name, tpath, FALSE /*verbose*/);
#endif

#ifdef TARG_ST
     // Retrieve SI_resource_count value from library
    Update_SI_Resource_Count();
    
    ISA_SUBSET_LIST_Add(ISA_SUBSET_List, tisa);
    for (i = 0; i < opt_subsets_count; i++) {
      ISA_SUBSET_LIST_Add(ISA_SUBSET_List, opt_subsets[i]);
    }
#else
    ISA_SUBSET_Value = tisa;
#endif
    PROCESSOR_Value = tproc;
    ABI_PROPERTIES_ABI_Value = tabi;

#ifdef TARG_ST
     // Reconfigurability
    extern void TI_Initialize_Extension_Loader();
    TI_Initialize_Extension_Loader();

    // Retrieve SI_resource_count value from library
    // (might have been updated by the extension loader)
    Update_SI_Resource_Count();
#endif
    ABI_PROPERTIES_Initialize();
    ISA_HAZARD_Initialize();
    ISA_REGISTER_Initialize();

#ifndef USE_WEAK_REFERENCES
    // For bug 13044, sanity check that we have loaded the proper information.
    FmtAssert (!strcmp(targ_so_name, sanity_check_targ_so_name),
     ("TI_Initialize did not load proper information from %s", targ_so_name));
#endif

    initialized = TRUE;
#ifdef TARG_ST
    return targ_handler;
#endif
  }
}

#ifndef USE_WEAK_REFERENCES

#include "ti_si_types.h"

const SI * const * SI_top_si_p;
const SI * const * SI_ID_si_p;
const int * SI_ID_count_p;
const SI_ISSUE_SLOT * const * SI_issue_slots_p;
const int * SI_issue_slot_count_p;
const SI_RESOURCE * const * SI_resources_p;
const int * SI_resource_count_p;
const SI_RRW * SI_RRW_initializer_p;
const SI_RRW * SI_RRW_overuse_mask_p;

#endif
