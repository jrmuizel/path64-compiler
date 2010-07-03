/*
  Copyright (C) 2001, STMicroelectronics, All Rights Reserved.

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


//-*-c++-*-
// =======================================================================
// =======================================================================
//
//  Module: cg_affirm.h
//
//  Description:
//  ============

//
// =======================================================================
// ======================================================================= */

#ifdef TARG_ST
#include "wn.h"
#include "bb.h"
#include "op.h"
#include "range_analysis.h"
#ifndef CG_AFFIRM_INCLUDED
#define CG_AFFIRM_INCLUDED

// FdF 20080305: Add a map to associate a WN_AFFIRM node to the TN(s)
// defined on an operation.

/* Affirm opt
   0x1 : Affirm for trip count optimization
   0x2 : Affirm for alignment on loop head
*/
enum AFFIRM_OPT {
  AFFIRM_TRIP_COUNT			= 0x1,
  AFFIRM_ALIGN				= 0x2
};
extern INT32 CG_affirm_opt;

extern void OP_Set_Affirm(OP *op, WN *wn_affirm);
extern WN * OP_Get_Affirm(const OP *op);
inline BOOL OP_Is_Affirm(const OP *op) { return OP_Get_Affirm(op) != NULL; }
extern void OP_Affirm_delete_map();


extern BOOL BB_Insert_Affirm_for_modulo(BB *bb, TN *tn, INT base, INT bias);
extern BOOL OPS_Insert_Affirm_for_modulo(OPS *ops, TN *tn, INT base, INT bias);
extern BOOL Get_Affirm_modulo(TN *tn, BB *bb, INT* base, INT* bias);
extern BOOL Get_Affirm_modulo(WN *wn_affirm, WN** wn_modulo, INT* base, INT* bias);

extern BOOL Generate_Affirm(RangeAnalysis *range_analysis, BB *bb);
#endif /* CG_AFFIRM_INCLUDED */

#endif /* TARG_ST */
