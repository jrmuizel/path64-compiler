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

/* Macros to check if a TN is a particular dedicated register. */
#define TN_is_zero_reg(r)  (FALSE)
#define TN_is_sp_reg(r)	   (TN_register_and_class(r) == CLASS_AND_REG_sp)
#define TN_is_gp_reg(r)	   (TN_register_and_class(r) == CLASS_AND_REG_gp)
#define TN_is_fp_reg(r)	   (TN_register_and_class(r) == CLASS_AND_REG_fp)
#define TN_is_ep_reg(r)    (FALSE)
#define TN_is_ra_reg(r)	   (TN_register_and_class(r) == CLASS_AND_REG_ra)
#define TN_is_pfs_reg(r)   (FALSE)
#define TN_is_lc_reg(r)    (TN_register_and_class(r) == CLASS_AND_REG_lc)
#define TN_is_ec_reg(r)    (FALSE)
#define TN_is_true_pred(r) (TN_register_and_class(r) == CLASS_AND_REG_true)
#define TN_is_fzero_reg(r) (FALSE)
#define TN_is_fone_reg(r)  (FALSE)

/* ====================================================================
 * ====================================================================
 *                  Define the TN_relocs values 
 * ====================================================================
 * ====================================================================
 */

/* the following represent various relocations on symbolic values.  
 * The values are represented by a symbol table element.
 */
#define  TN_RELOC_NONE	      0x00
#define  TN_RELOC_NEG         0x01
#define  TN_RELOC_GPREL16     0x02	/* gp-relative reference to symbol */
#define  TN_RELOC_LOW16	      0x03	/* Bits 0..15 of symbolic val */
#define  TN_RELOC_HIGH16      0x04	/* Bits 16..31 of symbol */
#define  TN_RELOC_HIGHER      0x05	/* Bits 32..47 of symbol */
#define  TN_RELOC_HIGHEST     0x06	/* Bits 48..63 of symbol */
#define  TN_RELOC_GOT_DISP    0x07
#define  TN_RELOC_GOT_PAGE    0x08
#define  TN_RELOC_GOT_OFST    0x09
#define  TN_RELOC_CALL16      0x0a
#define  TN_RELOC_GOT_HI16    0x0b
#define  TN_RELOC_GOT_LO16    0x0c
#define  TN_RELOC_CALL_HI16   0x0d
#define  TN_RELOC_CALL_LO16   0x0e
#define  TN_RELOC_GPSUB       0x0f	/* gp - sym (pic2 prolog) */
#define  TN_RELOC_LO_GPSUB    0x10
#define  TN_RELOC_HI_GPSUB    0x11
#define  TN_RELOC_GPIDENT     0x12      /* gp value (pic1 prolog) */
#define  TN_RELOC_LO_GPIDENT  0x13
#define  TN_RELOC_HI_GPIDENT  0x14

#define TN_RELOC_DA_0_14      0x15
#define TN_RELOC_DA_1_15      0x16
#define TN_RELOC_DA_2_16      0x17
#define TN_RELOC_DA_0_15      0x18
#define TN_RELOC_DA_16_31     0x19
#define TN_RELOC_DA_1_16      0x20
#define TN_RELOC_DA_17_32     0x21
#define TN_RELOC_DA_2_17      0x22
#define TN_RELOC_DA_18_33     0x23

  /*
  TN_RELOC_IA_IMM14	= 0x20,
  TN_RELOC_IA_IMM22	= 0x21,
  TN_RELOC_IA_PCREL	= 0x22,
  TN_RELOC_IA_GPREL22	= 0x23,
  TN_RELOC_IA_LTOFF22	= 0x24,
  TN_RELOC_IA_LTOFF_FPTR= 0x25,
				
  TN_RELOC_IA32_ALL 	= 0x40,	
  */

typedef struct {
  char *name;
} TN_RELOCS_INFO;
extern const TN_RELOCS_INFO TN_RELOCS_info[];


/* Define the TN_relocs access functions: */
#define TN_is_reloc_gprel16(r)		(TN_relocs(r) == TN_RELOC_GPREL16)
#define Set_TN_is_reloc_gprel16(r)	Set_TN_relocs(r,TN_RELOC_GPREL16)
#define TN_is_reloc_low16(r)		(TN_relocs(r) == TN_RELOC_LOW16)
#define Set_TN_is_reloc_low16(r)	Set_TN_relocs(r,TN_RELOC_LOW16)
#define TN_is_reloc_high16(r)		(TN_relocs(r) == TN_RELOC_HIGH16)
#define Set_TN_is_reloc_high16(r)	Set_TN_relocs(r,TN_RELOC_HIGH16)
#define TN_is_reloc_higher(r)		(TN_relocs(r) == TN_RELOC_HIGHER)
#define Set_TN_is_reloc_higher(r)	Set_TN_relocs(r,TN_RELOC_HIGHER)
#define TN_is_reloc_highest(r)		(TN_relocs(r) == TN_RELOC_HIGHEST)
#define Set_TN_is_reloc_highest(r)	Set_TN_relocs(r,TN_RELOC_HIGHEST)
#define TN_is_reloc_got_disp(r)		(TN_relocs(r) == TN_RELOC_GOT_DISP)
#define Set_TN_is_reloc_got_disp(r)	Set_TN_relocs(r,TN_RELOC_GOT_DISP)
#define TN_is_reloc_got_page(r)		(TN_relocs(r) == TN_RELOC_GOT_PAGE)
#define Set_TN_is_reloc_got_page(r)	Set_TN_relocs(r,TN_RELOC_GOT_PAGE)
#define TN_is_reloc_got_ofst(r)		(TN_relocs(r) == TN_RELOC_GOT_OFST)
#define Set_TN_is_reloc_got_ofst(r)	Set_TN_relocs(r,TN_RELOC_GOT_OFST)
#define TN_is_reloc_call16(r)		(TN_relocs(r) == TN_RELOC_CALL16)
#define Set_TN_is_reloc_call16(r)	Set_TN_relocs(r,TN_RELOC_CALL16)
#define TN_is_reloc_got_hi16(r)		(TN_relocs(r) == TN_RELOC_GOT_HI16)
#define Set_TN_is_reloc_got_hi16(r)	Set_TN_relocs(r,TN_RELOC_GOT_HI16)
#define TN_is_reloc_got_lo16(r)		(TN_relocs(r) == TN_RELOC_GOT_LO16)
#define Set_TN_is_reloc_got_lo16(r)	Set_TN_relocs(r,TN_RELOC_GOT_LO16)
#define TN_is_reloc_call_hi16(r)	(TN_relocs(r) == TN_RELOC_CALL_HI16)
#define Set_TN_is_reloc_call_hi16(r)	Set_TN_relocs(r,TN_RELOC_CALL_HI16)
#define TN_is_reloc_call_lo16(r)	(TN_relocs(r) == TN_RELOC_CALL_LO16)
#define Set_TN_is_reloc_call_lo16(r)	Set_TN_relocs(r,TN_RELOC_CALL_LO16)
#define TN_is_reloc_neg(r)		(TN_relocs(r) == TN_RELOC_NEG)
#define Set_TN_is_reloc_neg(r)		Set_TN_relocs(r,TN_RELOC_NEG)
#define TN_is_reloc_gpsub(r)		(TN_relocs(r) == TN_RELOC_GPSUB)
#define Set_TN_is_reloc_gpsub(r)	Set_TN_relocs(r,TN_RELOC_GPSUB)
#define TN_is_reloc_lo_gpsub(r)		(TN_relocs(r) == TN_RELOC_LO_GPSUB)
#define Set_TN_is_reloc_lo_gpsub(r)	Set_TN_relocs(r,TN_RELOC_LO_GPSUB)
#define TN_is_reloc_hi_gpsub(r)		(TN_relocs(r) == TN_RELOC_HI_GPSUB)
#define Set_TN_is_reloc_hi_gpsub(r)	Set_TN_relocs(r,TN_RELOC_HI_GPSUB)
#define TN_is_reloc_gpident(r)		(TN_relocs(r) == TN_RELOC_GPIDENT)
#define Set_TN_is_reloc_gpident(r)	Set_TN_relocs(r,TN_RELOC_GPIDENT)
#define TN_is_reloc_lo_gpident(r)	(TN_relocs(r) == TN_RELOC_LO_GPIDENT)
#define Set_TN_is_reloc_lo_gpident(r)	Set_TN_relocs(r,TN_RELOC_LO_GPIDENT)
#define TN_is_reloc_hi_gpident(r)	(TN_relocs(r) == TN_RELOC_HI_GPIDENT)
#define Set_TN_is_reloc_hi_gpident(r)	Set_TN_relocs(r,TN_RELOC_HI_GPIDENT)

inline const char * TN_RELOCS_Name (mUINT8 rc)
{
  return TN_RELOCS_info[rc].name;
}

// ---------------------------------------------------------------------
inline BOOL TN_is_const_reg(const TN *r)
{
  return (TN_is_register(r) && 
	  TN_is_dedicated(r) &&
	  (TN_is_true_pred(r)));
}
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
inline TN*
Gen_Predicate_TN()
{
  return Build_RCLASS_TN(ISA_REGISTER_CLASS_guard);
}
// ---------------------------------------------------------------------
inline BOOL TN_is_fcc_register (const TN *tn)
{
  return FALSE;
}

extern void TN_format_sp_relative (TN *t, INT64 base_ofst, vstring *buf);
extern void TN_format_reloc (TN *t, vstring *buf);
extern BOOL TN_Use_Base_ST_For_Reloc (INT reloc, ST *st);
extern INT  TN_Relocs_In_Asm (TN *t, ST *st, vstring *buf, INT64 *val);

#endif /* tn_targ_INCLUDED */
