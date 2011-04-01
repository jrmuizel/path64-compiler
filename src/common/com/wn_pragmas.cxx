/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#ifdef USE_PCH
#include "common_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop

#include "defs.h"
#include "wn_pragmas.h"
WN_PRAGMA_DESC WN_pragmas[] = 
{
  { PUSER_NULL,	WN_PRAGMA_SCOPE_UNKNOWN, NULL }, /* WN_PRAGMA_UNDEFINED */

  { (WN_PRAGMA_USERS) (PUSER_IPA|PUSER_WOPT), WN_PRAGMA_SCOPE_ON, 	"INLINE_BODY_START" },
  { (WN_PRAGMA_USERS) (PUSER_IPA|PUSER_WOPT), WN_PRAGMA_SCOPE_OFF, 	"INLINE_BODY_END" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, 	"INLINE_DEPTH" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, 	"INLINE_LOOPLEVEL" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL, "AGGRESSIVE_INNER_LOOP_FISSION" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_POINT, 	"FISSION" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL, "FISSIONABLE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL, "FUSE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL, "FUSEABLE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"NO_FISSION" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"NO_FUSION" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"INTERCHANGE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"NO_INTERCHANGE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"BLOCKING_SIZE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"NO_BLOCKING" },
  { PUSER_CG, WN_PRAGMA_SCOPE_WN, 	"UNROLL" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"BLOCKABLE" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"PREFETCH" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"PREFETCH_MANUAL" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"PREFETCH_REF" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"PREFETCH_REF_DISABLE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_WN, 	"IVDEP" },

  { (WN_PRAGMA_USERS) (PUSER_IPA|PUSER_LNO|PUSER_WOPT|PUSER_CG),
	WN_PRAGMA_SCOPE_PU,"OPTIONS" },
  { (WN_PRAGMA_USERS) (PUSER_IPA|PUSER_LNO|PUSER_WOPT|PUSER_CG),
	WN_PRAGMA_SCOPE_PU,"OPAQUE_REGION"},

  { PUSER_CG, WN_PRAGMA_SCOPE_POINT, 	"FREQUENCY" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_WN, 	"DISTRIBUTE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_WN, 	"REDISTRIBUTE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_WN, 	"DISTRIBUTE_RESHAPE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_WN,      "DYNAMIC" },

  { PUSER_WOPT, WN_PRAGMA_SCOPE_SPECIAL, "ACCESSED_ID" },

  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"PFOR_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"ENTER_GATE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"EXIT_GATE" },

  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,	"BARRIER" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"CHUNKSIZE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,	"COPYIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"CRITICAL_SECTION_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"CRITICAL_SECTION_END" },
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,	"DOACROSS" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"IF" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"LASTLOCAL" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"LOCAL" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"MPSCHEDTYPE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"ORDERED" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"PARALLEL_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"PARALLEL_END" },
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,	"PARALLEL_DO" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"PDO_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"PDO_END" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"PSECTION_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"PSECTION_END" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"REDUCTION" },
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,	"SECTION" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"SHARED" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"SINGLE_PROCESS_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"SINGLE_PROCESS_END" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"ITERATE_VAR" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"ITERATE_INIT" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"ITERATE_COUNT" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"ITERATE_STEP" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"AFFINITY" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"DATA_AFFINITY" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"THREAD_AFFINITY" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"NUMTHREADS" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"NOWAIT" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"PAGE_PLACE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"ONTO" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"LASTTHREAD" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_POINT,	"NORECURRENCE"},
  { PUSER_LNO, WN_PRAGMA_SCOPE_POINT,	"NEXT_SCALAR"},

  { PUSER_PURPLE, (WN_PRAGMA_SCOPE) WN_PRAGMA_PURPLE_CONDITIONAL,	"PURPLE_CONDITIONAL" },
  { PUSER_PURPLE, (WN_PRAGMA_SCOPE) WN_PRAGMA_PURPLE_UNCONDITIONAL, "PURPLE_UNCONDITIONAL" },
  
  { PUSER_WOPT, WN_PRAGMA_SCOPE_PU,     "WOPT_FINISHED_OPTIMIZATION" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ARCLIMIT" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_CONCURRENTIZE" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, "KAP_INLINE_FILE" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, "KAP_INLINE_PU" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_LIMIT" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_MINCONCURRENT" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_NOCONCURRENTIZE" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, "KAP_NOINLINE_FILE" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, "KAP_NOINLINE_PU" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_OPTIMIZE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ROUNDOFF" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_SCALAR_OPTIMIZE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_CTHRESHOLD" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_EACH_INVARIANT_IF_GROWTH" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_MAX_INVARIANT_IF_GROWTH" },
  { PUSER_CG,  WN_PRAGMA_SCOPE_PU, "KAP_STORAGE_ORDER"},

  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_BOUNDS_VIOLATIONS" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NOBOUNDS_VIOLATIONS" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_CONCURRENT_CALL" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_DO" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_DOPREFER" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_EQUIVALENCE_HAZARD" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NOEQUIVALENCE_HAZARD" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_LAST_VALUE_NEEDED" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NOLAST_VALUE_NEEDED" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_PERMUTATION" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NORECURRENCE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_RELATION" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NOSYNC" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_TEMPORARIES_FOR_CONSTANT_ARGUMENTS" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NOTEMPORARIES_FOR_CONSTANT_ARGUMENTS" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_ARGUMENT_ALIASING" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_BENIGN" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_DEPENDENCE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_FREQUENCY" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_IGNORE_ANY_DEPENDENCE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_IGNORE_ASSUMED_DEPENDENCE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NO_ARGUMENT_ALIASING" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NO_CONCURRENT_CALL" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_NO_INTERCHANGE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_USE_COMPRESS" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_USE_EXPAND" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_USE_CONTROLLED_STORE" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_USE_GATHER" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_ASSERT_USE_SCATTER" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "KAP_OPTIONS" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "PREAMBLE_END" },

  { PUSER_W2F, WN_PRAGMA_SCOPE_PU, "FLIST_SKIP_BEGIN" },
  { PUSER_W2F, WN_PRAGMA_SCOPE_PU, "FLIST_SKIP_END" },
  { PUSER_W2C, WN_PRAGMA_SCOPE_PU, "CLIST_SKIP_BEGIN" },
  { PUSER_W2C, WN_PRAGMA_SCOPE_PU, "CLIST_SKIP_END" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "FILL_SYMBOL" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_PU, "ALIGN_SYMBOL" },

  { PUSER_MP, WN_PRAGMA_SCOPE_ON,  "INDEPENDENT_BEGIN" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF, "INDEPENDENT_END" },

  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, "KAP_OPTION_INLINE" },
  { PUSER_IPA, WN_PRAGMA_SCOPE_PU, "KAP_OPTION_NOINLINE" },

  { PUSER_MP, WN_PRAGMA_SCOPE_WN,      "_CRI_IVDEP" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_NOVECTOR" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_NOVSEARCH" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_PREFERVECTOR" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_SHORTLOOP" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_CASE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_ENDCASE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_COMMON" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,      "_CRI_GUARD" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,     "_CRI_ENDGUARD" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_ENDLOOP" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,      "_CRI_PARALLEL" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,     "_CRI_ENDPARALLEL" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_PREFERTASK" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_TASKCOMMON" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_TASKLOOP" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_SHARED" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_PRIVATE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_VALUE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_DEFAULTS" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_MAXCPUS" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_SAVELAST" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_CHUNKSIZE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_NUMCHUNKS" },

  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_TASK" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_NOTASK" },
  { PUSER_CG, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_ALIGN" },
  { PUSER_CG, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_BL" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "_CRI_CNCALL" },

  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL, "MPNUM" },
  { PUSER_MP, (WN_PRAGMA_SCOPE) WN_PRAGMA_COPYIN_BOUND,  "COPYIN_BOUND" },
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL, "SYNC_DOACROSS" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"DEFAULT" },
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,	"FIRSTPRIVATE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"MASTER" },
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"ORDERED" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"END_ORDERED" },
  { PUSER_MP, WN_PRAGMA_SCOPE_WN,	"ATOMIC" },
  { PUSER_MP, WN_PRAGMA_SCOPE_WN,	"ORDERED_LOWER_BOUND" },
  { PUSER_MP, WN_PRAGMA_SCOPE_WN,	"ORDERED_STRIDE" },
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"END_MARKER" },
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,    "PARALLEL_SECTIONS" },

  { PUSER_REGION, WN_PRAGMA_SCOPE_POINT, "START_STMT_CLUMP" },
  { PUSER_REGION, WN_PRAGMA_SCOPE_POINT, "END_STMT_CLUMP" },

  { PUSER_LNO, WN_PRAGMA_SCOPE_WN, 	"TYPE_OF_RESHAPED_ARRAY" },

  { PUSER_CG, WN_PRAGMA_SCOPE_WN,       "ASM_CONSTRAINT"},
  { PUSER_CG, WN_PRAGMA_SCOPE_WN,       "ASM_CLOBBER"},

#ifdef KEY
  { PUSER_LNO, WN_PRAGMA_SCOPE_SPECIAL,	"FORALL" },
#endif
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,    "COPYPRIVATE" }, /* by jhs, 02.9.3 */
  { PUSER_MP, WN_PRAGMA_SCOPE_POINT,	"PARALLEL_WORKSHARE" }, /* by jhs, 04.3.10 */
  { PUSER_MP, WN_PRAGMA_SCOPE_ON,	"PWORKSHARE_BEGIN" }, /* by jhs, 04.3.10 */
  { PUSER_MP, WN_PRAGMA_SCOPE_OFF,	"PWORKSHARE_END" }, /* by jhs, 04.3.10 */
  { PUSER_MP, WN_PRAGMA_SCOPE_SPECIAL,  "THREADPRIVATE" }, /* by jhs, 02.9.18 */
  { PUSER_NULL,	WN_PRAGMA_SCOPE_UNKNOWN, NULL }	/* MAX_WN_PRAGMA */
};

const char *
WN_Pragma_Name(INT pragma_id)
{
  return WN_pragmas[pragma_id].name;
}


