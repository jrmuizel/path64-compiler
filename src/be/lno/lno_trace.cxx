/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#include <cstdarg>
#include <iostream>
#include "lno_trace.h"

using namespace std;

//-----------------------------------------------------------------------
// NAME: LNO_TraceEvents
// FUNCTION: Keeps output fromat for LNO events
//-----------------------------------------------------------------------
static LNO_TraceEventDescr_t LNO_TraceEvents[] =
{
    {"ZERO_EVENT", ""}, /*LNO_INIT_EVENT*/
    {"PREFETCH", "genereted %d instructions"}, /*LNO_PREFETCH_EVENT*/
    {"INVARIANTS", "expressions moved to line %d"}, /*LNO_INVARIANT_EVENT*/ 
    {"VECTORIZE", "%s %s"}, /*LNO_VECTORIZE_EVENT*/
    {"SCALAR_EXPAND", "scalar %s %s %s in loop %s expanded"}, /* LNO_SCALAR_EXPAND_EVENT */
    {"3DTILE", "3D Tile loop %s -> (%s, %s, %s)"}, /* LNO_3DTILE_EVENT */
    {"2DTILE", "2D Tile loop %s -> (%s, %s)"}, /* LNO_2DTILE_EVENT */
    {"REVERSE", "loop reversed (index %s)"}, /*LNO_REVERSE_EVENT*/
    {"TRANSPOSE", "transposing array %s"}, /*LNO_TRANSPOSE_EVENT*/
    {"LEGO_SKEWING", "loop %s skewed"}, /*LNO_LEGO_SKEWING_EVENT*/
    {"VINTR_FIS", "%s %s"}, /*LNO_VINTR_FIS_EVENT*/
    {"CSE", "%d statements with %d common_invariants have been factorized"}, /*LNO_CSE_EVENT*/ 
    {"SUBST_STMT", "%s substituting %d occurences of %s"}, /*LNO_SUBST_STMT_EVENT*/
    {"SUBST", "forward substituting %s"}, /*LNO_SUBST_ARRAY_EVENT*/
    {"FIZ_FUSE",  "try fuse loops at lines %d and %d, level %d with result %s"}, /*LNO_FIZ_FUSE_EVENT*/
    {"MODEL", "loop number = %d -> cycle est %lf (before cache) %lf (after cache) %lf cache cycles  %lf overhead cycles "}, /*LNO_MODEL_EVENT*/
    {"FISSION", "fission %s %s"}, /*LNO_FISSION_EVENT*/
    {"INTERCHANGE", "%s"}, /*LNO_INTERCHANGE_EVENT*/
    {"SMALL_TRIPS", "%s"}, /*LNO_SMALL_TRIPS_EVENT*/
    {"SMALL_TRIPS", "%s at lines %d and %d"}, /*LNO_SMALL_TRIPS_UNIFY_EVENT*/
    {"SPLIT_TILES", "%s"}, /*LNO_SPLIT_TILES_EVENT*/
    {"PEELING", "%s peeling of %d iterations"}, /*LNO_PEELING_EVENT*/
    {"FUSION", "loops at lines %d and %d level %d %s"}, /*LNO_FUSION_EVENT*/
    {"INNER_FISSION", "%s"}, /*LNO_INNER_FISSION_EVENT*/
    {"HMB", "%s"}, /*LNO_HMB_EVENT*/
    {"OUTER_FUSION", "loops at lines %d and %d levels %d and %d %s"}, /*LNO_OUTER_FUSION_EVENT*/
    {"UNSWITCH", "%s"}, /*LNO_UNSWITCH_EVENT*/
    {"REDN_COND", "bad def for %s at line %d"}, /*LNO_REDN_COND_EVENT*/
    {"SNL_TRANSFORM", "%s"}, /*LNO_SNL_TRANSFORM_EVENT*/
    {"ARA", "%s"}, /*LNO_ARA_EVENT*/
    {"FIS_GTHR", "%s"}, /*LNO_FIS_GTHR_EVENT*/
    {"SX_INFO", "%s"}, /*LNO_SX_INFO_EVENT*/
    {"SNL_DIST", "%s"}, /*LNO_SNL_DIST_EVENT*/
    {"INV_FACTOR", "%s"}, /*LNO_INV_FACTOR_EVENT*/
    {"PARALLEL", "%s"}, /*LNO_PARALLEL_EVENT*/
    {"ACCESS", "%s"}, /*LNO_ACCESS_EVENT*/
    {"MAX_EVENT", ""}    /*LNO_MAX_EVENT*/
};  

#define LNO_TraceGetEventFormat( event)  LNO_TraceEvents[event].event_format
#define LNO_TraceGetEventName( event) LNO_TraceEvents[event].event_name

//-----------------------------------------------------------------------
// NAME: LNO_PrintEvent
// FUNCTION: Print an infromative message on the event
// using JSON format
// {"phase":"event_name", "file":"src_file:, "func":"proc_name", "line":src_line, "message":"event_message"}
//-----------------------------------------------------------------------

static void
LNO_PrintEvent( const char *event_name,                  
                char *src_file,                 
                INT  src_line,        
                char *proc_name,           
                const char *event_msg)
{
    cout << "{";
    cout << "\"phase\" : " << "\"" << event_name << "\", ";
    cout << "\"file\" : " << "\"" << src_file << "\", " ;
    cout << "\"func\" : " << "\"" << proc_name << "\", ";
    cout << "\"line\" : " << src_line << ", ";
    cout << "\"message\" : " << "\"" << event_msg << "\"";
    cout << "}"<<  endl;
    return;
} 

//-----------------------------------------------------------------------
// NAME: LNO_Trace
// FUNCTION: Create an infromative message on the event
// using prespecified format for each event described in LNO_TraceEvents
// uses src_file, src_line, proc_name, and va_list to create a standardized outptut
//-----------------------------------------------------------------------
void
LNO_Trace( LNO_TraceEvent_t event,             
           char * src_file,
           INT src_line,      
           char * proc_name,         
                  ...)                             
{
    char buf[512];
    va_list ap;
    
    va_start( ap, proc_name);
    vsprintf( buf, LNO_TraceGetEventFormat( event), ap);
    LNO_PrintEvent( LNO_TraceGetEventName( event),
                    src_file,
                    src_line,
                    proc_name,
                    buf);
    va_end( ap);
    return;
} 

