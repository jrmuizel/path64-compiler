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

#if HAVE_ALLOCA_H
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
#include "ti_si_types.h"

static const SI_SUMMARY *All_Si_Summaries[PROCESSOR_count];
const SI_SUMMARY *Cur_Si_Summary;


/* ====================================================================
 *
 *  TI_Initialize
 *
 *  See interface description
 *
 * ====================================================================
 */
void
TI_Initialize(ABI_PROPERTIES_ABI tabi, 
              ISA_SUBSET tisa, 
              PROCESSOR tproc, 
              char *tpath)
{
  if (All_Si_Summaries[tproc] == NULL) {
    const char *targ_name = PROCESSOR_Name(tproc);
	char *dso_name = alloca(strlen(targ_name) + sizeof(EXT_DSO));
    const SI_SUMMARY *(*SI_Get_Summary)(void);
    const SI_SUMMARY *summary;
    void *handle;

    strcpy(dso_name, targ_name);
    strcat(dso_name, EXT_DSO);

    handle = dso_load(dso_name, tpath, FALSE);

    SI_Get_Summary = dso_get_interface(handle, "SI_Get_Summary");
    summary = SI_Get_Summary();

    // For bug 13044, sanity check that we have loaded the proper information.
    FmtAssert(!strcmp(targ_name, summary->target_name),
     ("TI_Initialize did not load proper information for %s", targ_name));

    All_Si_Summaries[tproc] = summary;
  }

  Cur_Si_Summary = All_Si_Summaries[tproc];
  ISA_SUBSET_Value = tisa;
  PROCESSOR_Value = tproc;
  ABI_PROPERTIES_ABI_Value = tabi;

  ABI_PROPERTIES_Initialize();
  ISA_HAZARD_Initialize();
  ISA_REGISTER_Initialize();
}
