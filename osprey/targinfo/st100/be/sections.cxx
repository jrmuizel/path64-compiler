/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

/* ====================================================================
 *
 *  Description:
 *
 *    This should be moved to be/com/ directory and should contain only
 *    target independent stuff. All target dependent things should go
 *    to targ_sections[c,h].
 * ====================================================================
 */

#include <elf.h>
#include "defs.h"
#include "stab.h"
#include "stblock.h"
#include "config_targ.h"
#include "data_layout.h"
#include "sections.h"
#include "targ_sections.h"
#include "targ_isa_bundle.h" // for ISA_INST_BYTES

// only called from Shorten_Section():
extern SECTION_IDX Corresponding_Short_Section (ST *st, SECTION_IDX sec);

SECTION Sections[_SEC_INDEX_MAX] = {
  {_SEC_UNKNOWN,NULL,
     0,
	0, 0, 
     0, ".unknown", 0},
  {_SEC_TEXT,	NULL,
     0|SHF_EXECINSTR|SHF_ALLOC,
	SHT_PROGBITS, ISA_INST_BYTES, 
     TEXT_SIZE, ELF_TEXT, 0},
  {_SEC_DATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     DATA_SIZE, ELF_DATA, 0},
  {_SEC_SDATA,	NULL,
     0|SHF_WRITE|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA_SIZE, SDATA_NAME, 0},
  {_SEC_RDATA,	NULL,
     0|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     RDATA_SIZE, ELF_RODATA, 0},
  {_SEC_SRDATA,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     INT32_MAX, MIPS_SRDATA, 0},
  {_SEC_LIT4,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 4, 
     LIT4_SIZE, LIT4_NAME, 0},
  {_SEC_LIT8,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 8, 
     LIT8_SIZE, LIT8_NAME, 0},
  {_SEC_LIT16,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 16, 
     LIT16_SIZE, LIT16_NAME, 0},
  {_SEC_BSS,	NULL,
     0|SHF_WRITE|SHF_ALLOC,
	SHT_NOBITS, 0, 
     BSS_SIZE, ELF_BSS, 0},
  {_SEC_SBSS,	NULL,
     0|SHF_WRITE|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_NOBITS, 0, 
     SBSS_SIZE, SBSS_NAME, 0},
  {_SEC_GOT,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     GOT_SIZE, ELF_GOT, 0},
  {_SEC_CPLINIT,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     CPLINIT_SIZE, CPLINIT_NAME, 0},

  // MIPS and IA64 specific sections
#if defined(TARG_MIPS) || defined(TARG_IA64)
  {_SEC_LDATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_PROGBITS, 0, 
     INT64_MAX, ".MIPS.ldata", 0},
#ifndef linux
  {_SEC_LBSS,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_NOBITS, 0, 
     INT64_MAX, MIPS_LBSS, 0},
#else
  // There is no MIPS_LBSS section on Linux, but we need a space holder
  {_SEC_LBSS,   NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif

#ifndef linux
  {_SEC_EH_REGION,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, MIPS_EH_REGION, 0},
  {_SEC_EH_REGION_SUPP,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, MIPS_EH_REGION_SUPP, 0},
#else
  // It's not yet clear what to do about the EH_REGION sections on Linux
  {_SEC_EH_REGION,      NULL,
     0,
        0, 0,
     0, ".unknown", 0},
  {_SEC_EH_REGION_SUPP, NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif
  {_SEC_DISTR_ARRAY,  NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0,
     INT64_MAX, "_MIPS_distr_array", 0},
  // I don't know what it is
  {_SEC_THREAD_PRIVATE_FUNCS,      NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif /* TARG_MIPS || TARG_IA64 */

#if defined(TARG_ST100)
  {_SEC_XSPACE,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     INT32_MAX, ".xspace", 0},
  {_SEC_YSPACE,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     INT32_MAX, ".yspace", 0},
  {_SEC_TDATA1,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     TDATA1_SIZE, ".tdata1", 0},
  {_SEC_TDATA2,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     TDATA2_SIZE, ".tdata2", 0},
  {_SEC_TDATA4,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     TDATA4_SIZE, ".tdata4", 0},
  {_SEC_SDATA1,	NULL,
     0|SHF_WRITE|SHF_ST100_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA1_SIZE, ".sdata1", 0},
  {_SEC_SDATA2,	NULL,
     0|SHF_WRITE|SHF_ST100_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA2_SIZE, ".sdata2", 0},
  {_SEC_SDATA4,	NULL,
     0|SHF_WRITE|SHF_ST100_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA4_SIZE, ".sdata4", 0},
#endif /* TARG_ST100 */
};

/* ====================================================================
 *   Shorten_Section
 *
 *   Given the section, is there a "Short" section and can we put
 *   this object into it?  Return the section we should put this
 *   object into. We also try to figure out if the object can be put
 *   into a merged section. 
 * ====================================================================
 */
SECTION_IDX
Shorten_Section ( 
  ST *st, 
  SECTION_IDX sec 
)
{
   INT64       size;
   SECTION_IDX newsec;
   BOOL        is_root_block = Is_Allocatable_Root_Block(st);

   // if we are not generating GP-relative addressing, return
   if (Gen_GP_Relative == FALSE) return sec;

   if (!is_root_block && ST_class(st) == CLASS_BLOCK) return sec;

   /* For PIC code, we can only put local or hidden objects into GP
      relative sections.  (unless is gprel)
   */
   if ((Gen_PIC_Shared || Gen_PIC_Call_Shared) && ST_is_preemptible(st)
   	&& ! (ST_class(st) == CLASS_VAR && ST_force_gprel(st)) )
   {
      return sec;
   }
#if 0
   newsec = Corresponding_Short_Section (st, sec);
   if (newsec == sec) return sec;	// won't be shortened
#endif

   size = ST_size(st);
   /* size = 0 implies that we don't really know the size. e.g. int a[]; */
   if (size == 0) return sec;

   if (ST_class(st) == CLASS_VAR && ST_not_gprel(st)) 
	return sec;	/* user says not gp-relative */

   if (size > Max_Sdata_Elt_Size) { 
	/* Object size is greater than the -G number.
	 * Unless user or previous phase has specified it to be GPREL,
	 * it can't be GP relative. */
	if (ST_class(st) != CLASS_VAR)
		return sec;
	/* else is CLASS_VAR */
	else if (!ST_gprel(st) && !ST_force_gprel(st))
		return sec;
   }

   if (Strings_Not_Gprelative && Is_String_Literal(st)) {
	/* don't put strings in a gp-relative section */
	return sec;
   }

   if ((ST_class(st) == CLASS_VAR || ST_class(st) == CLASS_CONST) && !ST_gprel(st)) 
   {
	/* Gspace specifies total amount of space we can use for things other
	 * than what ipa already allocated as gprel. */
	/* need to adjust size by possible alignment padding */
	size = MAX(size, Adjusted_Alignment(st));
	if (size <= Gspace_Available) {
		Gspace_Available -= size;
	}
	else {
	  /*
		if (Trace_Frame) fprintf(TFile, "<lay> not enough Gspace, so didn't assign %s to gprel section\n", ST_NAME(st));
		return sec;
	  */
	}
   }

   newsec = Corresponding_Short_Section (st, sec);

#if 0
   if (sec == _SEC_SDATA1 && ST_class(st) == CLASS_CONST) {
     /* by default put all short .rodata items into .srdata, unless 
      * we can put it into an appropriate merge section.
      */
     TCON tcon = STC_val(st);
     switch (TCON_ty (tcon)) {
       case MTYPE_F4:
       case MTYPE_I4:
       case MTYPE_U4:
	 newsec = _SEC_SDATA4;
	 break;
       case MTYPE_F8:
       case MTYPE_I8:
       case MTYPE_U8:
	 newsec = _SEC_SDATA4;
	 break;
       case MTYPE_FQ:
	 newsec = _SEC_SDATA4;
	 break;
     }
   }
#endif

   return newsec;
}

