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

#ifndef LNO_TRACE
#include "defs.h"
#include "glob.h"

typedef enum{
  LNO_INIT_EVENT=0,
  LNO_PREFETCH_EVENT,
  LNO_INVARIANT_EVENT,
  LNO_VECTORIZE_EVENT,
  LNO_SCALAR_EXPAND_EVENT,
  LNO_3DTILE_EVENT,
  LNO_2DTILE_EVENT,
  LNO_REVERSE_EVENT,
  LNO_TRANSPOSE_EVENT,
  LNO_LEGO_SKEWING_EVENT,
  LNO_VINTR_FIS_EVENT,
  LNO_CSE_EVENT,
  LNO_SUBST_STMT_EVENT,
  LNO_SUBST_ARRAY_EVENT,
  LNO_FIZ_FUSE_EVENT,
  LNO_MODEL_EVENT,
  LNO_FISSION_EVENT,
  LNO_INTERCHANGE_EVENT,
  LNO_SMALL_TRIPS_EVENT,
  LNO_SMALL_TRIPS_UNIFY_EVENT,
  LNO_SPLIT_TILES_EVENT,
  LNO_PEELING_EVENT,
  LNO_FUSION_EVENT,
  LNO_INNER_FISSION_EVENT,
  LNO_HMB_EVENT,
  LNO_OUTER_FUSION_EVENT,
  LNO_UNSWITCH_EVENT,
  LNO_REDN_COND_EVENT,
  LNO_SNL_TRANSFORM_EVENT,
  LNO_ARA_EVENT,
  LNO_FIS_GTHR_EVENT,
  LNO_SX_INFO_EVENT,
  LNO_SNL_DIST_EVENT,
  LNO_INV_FACTOR_EVENT,
  LNO_PARALLEL_EVENT,
  LNO_ACCESS_EVENT,
  LNO_MAX_EVENT
} LNO_TraceEvent_t;

typedef struct{
  const char * event_name;
  const char * event_format;
} LNO_TraceEventDescr_t;

extern void
LNO_Trace( LNO_TraceEvent_t event,             
           char * src_file,
           INT   src_line,      
           char * proc_name,         
                  ...);  
#endif /*!LNO_TRACE*/
