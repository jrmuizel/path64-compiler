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
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../tn.h!!!!
 *
 * ====================================================================
 * ====================================================================
 */

/* ====================================================================
 *
 *   Description:
 *
 *   This module defines a number of functions used by the cg phase
 *   of the compiler. These are the target dependent interface 
 *   functions for handling the TNs.
 *
 *   Macros to check if a TN is a particular dedicated register:
 *
 *     TN_is_zero_reg(r)
 *     TN_is_sp_reg(r)
 *     TN_is_gp_reg(r)
 *     TN_is_fp_reg(r)
 *     TN_is_ra_reg(r)
 *     TN_is_lc_reg(r)
 *     TN_is_true_pred(r)
 *
 *   Exported Interface:
 *
 *   BOOL TN_is_const_reg(
 *                  const TN *tn)
 *
 *       Returns TRUE if the TN represents a hardwired registers.
 *
 *   BOOL TN_is_fcc_register (
 *                  const TN *tn)
 *
 *       Returns TRUE if the TN is a conditional code register.
 *
 *   TN *Gen_Predicate_TN ()
 *
 *       Generate TN to contain a general predicate value
 *
 * ====================================================================
 */

#ifndef tn_targ_INCLUDED
#define tn_targ_INCLUDED

#include "vstring.h"

/* Target-specific special registers: */


/* ====================================================================
 * ====================================================================
 *                  Define the TN_relocs values 
 * ====================================================================
 * ====================================================================
 */

/*
  [JV] For compatibility
 */
#define  TN_RELOC_NONE	          ISA_RELOC_UNDEFINED

#if 0

/* the following represent various relocations on symbolic values.  
 * The values are represented by a symbol table element.
 */
#define  TN_RELOC_NONE	          0x00
#define  TN_RELOC_GOT_DISP        0x01  /* equivalent to R_LX_GPREL */
#define  TN_RELOC_GOTOFF          0x02
#define  TN_RELOC_GOTOFF_FPTR     0x03
#define  TN_RELOC_GPIDENT         0x04
#define  TN_RELOC_GPSUB           0x05
#define  TN_RELOC_NEG_GOT_DISP    0x06  /* R_LX_NEG_GPREL */
#define  TN_RELOC_TPREL           0x07
#define  TN_RELOC_GOTOFF_TPREL    0x08
#define  TN_RELOC_GOTOFF_DTPLDM   0x09
#define  TN_RELOC_GOTOFF_DTPNDX   0x0a
#define  TN_RELOC_DTPREL          0x0b
#define  TN_RELOC_ABS32           0x0c
#define  TN_RELOC_JUMP24          0x0d
#define  TN_RELOC_CALL            0x0e
#define  TN_RELOC_OFFSET12        0x0f
#define  TN_RELOC_OFFSET12PC      0x10
#define  TN_RELOC_OFFSET8         0x11
#define  TN_RELOC_OFFSET8PC       0x12

/* Define the TN_relocs access functions: */
#define TN_is_reloc_none(r)             (TN_relocs(r) == TN_RELOC_NONE)
#define Set_TN_is_reloc_none(r)         Set_TN_relocs(r,TN_RELOC_NONE)
#define TN_is_reloc_got_disp(r)         (TN_relocs(r) == TN_RELOC_GOT_DISP)
#define Set_TN_is_reloc_got_disp(r)     Set_TN_relocs(r,TN_RELOC_GOT_DISP)
#define TN_is_reloc_gotoff_fptr(r)      (TN_relocs(r) == TN_RELOC_GOTOFF_FPTR)
#define Set_TN_is_reloc_gotoff_fptr(r)  Set_TN_relocs(r,TN_RELOC_GOTOFF_FPTR)
#define TN_is_reloc_neg_got_disp(r)     (TN_relocs(r) == TN_RELOC_NEG_GOT_DISP)
#define Set_TN_is_reloc_neg_got_disp(r) Set_TN_relocs(r,TN_RELOC_NEG_GOT_DISP)
#define TN_is_reloc_neg(r)              (0)
#define TN_is_reloc_tprel(r)            (TN_relocs(r) == TN_RELOC_TPREL)
#define Set_TN_is_reloc_tprel(r)        Set_TN_relocs(r, TN_RELOC_TPREL)
#define TN_is_reloc_gotoff_tprel(r)     (TN_relocs(r) == TN_RELOC_GOTOFF_TPREL)
#define Set_TN_is_reloc_gotoff_tprel(r) Set_TN_relocs(r, TN_RELOC_GOTOFF_TPREL)
#define TN_is_reloc_gotoff_dtpldm(r)    (TN_relocs(r) == TN_RELOC_GOTOFF_DTPLDM)
#define Set_TN_is_reloc_gotoff_dtpldm(r) Set_TN_relocs(r, TN_RELOC_GOTOFF_DTPLDM)
#define TN_is_reloc_gotoff_dtpndx(r)    (TN_relocs(r) == TN_RELOC_GOTOFF_DTPNDX)
#define Set_TN_is_reloc_gotoff_dtpndx(r) Set_TN_relocs(r, TN_RELOC_GOTOFF_DTPNDX)
#define TN_is_reloc_dtprel(r)           (TN_relocs(r) == TN_RELOC_DTPREL)
#define Set_TN_is_reloc_dtprel(r)       Set_TN_relocs(r, TN_RELOC_DTPREL)
#define TN_is_reloc_abs32(r)            (TN_relocs(r) == TN_RELOC_ABS32)
#define Set_TN_is_reloc_abs32(r)        Set_TN_relocs(r, TN_RELOC_ABS32)
#define TN_is_reloc_jump24(r)           (TN_relocs(r) == TN_RELOC_JUMP24)
#define Set_TN_is_reloc_jump24(r)       Set_TN_relocs(r, TN_RELOC_JUMP24)
#define TN_is_reloc_call(r)             (TN_relocs(r) == TN_RELOC_CALL)
#define Set_TN_is_reloc_call(r)         Set_TN_relocs(r, TN_RELOC_CALL)
#define TN_is_reloc_offset12(r)         (TN_relocs(r) == TN_RELOC_OFFSET12)
#define Set_TN_is_reloc_offset12(r)     Set_TN_relocs(r, TN_RELOC_OFFSET12)
#define TN_is_reloc_offset12pc(r)       (TN_relocs(r) == TN_RELOC_OFFSET12PC)
#define Set_TN_is_reloc_offset12pc(r)   Set_TN_relocs(r, TN_RELOC_OFFSET12PC)
#define TN_is_reloc_offset8(r)          (TN_relocs(r) == TN_RELOC_OFFSET8)
#define Set_TN_is_reloc_offset8(r)      Set_TN_relocs(r, TN_RELOC_OFFSET8)
#define TN_is_reloc_offset8pc(r)        (TN_relocs(r) == TN_RELOC_OFFSET8PC)
#define Set_TN_is_reloc_offset8pc(r)    Set_TN_relocs(r, TN_RELOC_OFFSET8PC)
#endif

// ---------------------------------------------------------------------
inline BOOL TN_is_fcc_register (const TN *tn)
{
  return FALSE;
}

#endif /* tn_targ_INCLUDED */
