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
 * ====================================================================
 *
 *  TN utility routines which include target dependencies.
 *
 * ====================================================================
 * ====================================================================
 */

#include <elf.h>

#include "defs.h"
#include "config.h"
#include "erglob.h"
#include "xstats.h"
#include "tracing.h"

#include "strtab.h"
#include "tn.h"
#include "tn_list.h"
#include "ttype.h"

#include "const.h"
#include "targ_const.h"
#include "targ_sim.h"

#include "config_asm.h"

#include "em_elf.h"

/*
 * TN relocations info:
 */
const TN_RELOCS_INFO TN_RELOCS_info[] = {
  { "" },
  { "-" },
  { "#gprel16" },
  { "#lo#" },
  { "#hi" },
  { "#higher" },
  { "#highest" },
  { "#got_disp" },
  { "#got_page" },
  { "#got_ofst" },
  { "#call16" },
  { "#got_hi16" },
  { "#got_lo16" },
  { "#call_hi16" },
  { "#call_lo16" },
  { "#gpsub" },
  { "#lo_gpsub" },
  { "#hi_gpsub" },
  { "#gpident" },
  { "#lo_gpident" },
  { "#hi_gpident" },
  { "#da0to14" },
  { "#da1to15" },
  { "#da2to16" },
  { "#da0to15" },
  { "#da16to31" },
  { "#da1to16" },
  { "#da17to32" },
  { "#da2to17" },
  { "#da18to33" }
};

/* ====================================================================
 *   TN_format_sp_relative
 * ====================================================================
 */
void
TN_format_sp_relative (
  TN      *t,
  INT64   base_ofst,
  vstring *buf		/* A buffer to format it into */
)
{
  BOOL  hexfmt = FALSE;
  INT64 val = base_ofst + TN_offset(t);

  if (TN_is_reloc_neg(t)) {
    val = -val;
  }
  if ( TN_is_reloc_low16(t) ) {
    val = val & 0xffff;
    hexfmt = TRUE;
  } else if ( TN_is_reloc_high16(t) ) {
    val = ( ( val - (short)val ) >> 16) & 0xffff;
    hexfmt = TRUE;
  } else if ( TN_is_reloc_higher(t) ) {
    val = ( ( val + 0x80008000LL ) >> 32 ) & 0xffff;
    hexfmt = TRUE;
  } else if ( TN_is_reloc_highest(t) ) {
    val = ( ( val + 0x800080008000LL ) >> 48 ) & 0xffff;
    hexfmt = TRUE;
  }

  vstr_sprintf (buf, vstr_len(*buf), (hexfmt ? "0x%llx" : "%lld"), val );
  return;
}

/* ====================================================================
 *   TN_format_reloc
 * ====================================================================
 */
void
TN_format_reloc (
  TN *t,
  vstring *buf
)
{
  if (TN_is_reloc_neg(t)) {
    *buf = vstr_concat (*buf, "-");
  }
  return;
}

/* ====================================================================
 *   TN_Use_Base_ST_For_Reloc
 *
 *   whether to use the base st for the reloc
 * ====================================================================
 */
BOOL
TN_Use_Base_ST_For_Reloc (
  INT reloc, 
  ST *st
)
{
  return ST_is_export_local(st);
}

/* ====================================================================
 *   TN_Relocs_In_Asm
 * ====================================================================
 */
INT
TN_Relocs_In_Asm (
  TN *t, 
  ST *st, 
  vstring *buf, 
  INT64 *val
)
{
  INT paren = 1;	// num parens
  // only add in GP_DISP if based on gprel section
  // not if based on ipa-generated extern.
  if (ST_class(st) == CLASS_BLOCK && STB_section(st)) {
    *val -= GP_DISP;
  }
  switch (TN_relocs(t)) {
    case TN_RELOC_DA_0_14:
      *buf = vstr_concat (*buf, "%da0to14");
      break;
    case TN_RELOC_DA_1_15:
      *buf = vstr_concat (*buf, "%da1to15");
      break;
    case TN_RELOC_DA_2_16:
      *buf = vstr_concat (*buf, "%da2to16");
      break;
    case TN_RELOC_DA_0_15:
      *buf = vstr_concat (*buf, "%da0to15");
      break;
    case TN_RELOC_DA_16_31:
      *buf = vstr_concat (*buf, "%da16to31");
      break;
    case TN_RELOC_DA_1_16:
      *buf = vstr_concat (*buf, "%da1to16");
      break;
    case TN_RELOC_DA_17_32:
      *buf = vstr_concat (*buf, "%da17to32");
      break;
    case TN_RELOC_DA_2_17:
      *buf = vstr_concat (*buf, "%da2to17");
      break;
    case TN_RELOC_DA_18_33:
      *buf = vstr_concat (*buf, "%da18to33");
      break;
    default:
      #pragma mips_frequency_hint NEVER
      FmtAssert (FALSE, ("relocs_asm: illegal reloc TN"));
      /*NOTREACHED*/
  }
  /*
  *buf = vstr_concat (*buf, "(" );
  *buf = vstr_concat (*buf, ST_name(st));
  *buf = vstr_concat (*buf, Symbol_Name_Suffix);
  */
  return paren;
}
