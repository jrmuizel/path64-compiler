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
//  Module: cg_ivs.h
//
//  Description:
//  ============

//  This module consists of Loop Induction Variables analysis and
//  optimization
//
// 
//  Exported Functions:
//  ===================
//
// LOOP_IVS::LOOP_IVS(MEM_POOL *mem_pool)
//    Constructor for a new LOOP_IVS instance
//
//  LOOP_IVS::~LOOP_IVS(void)
//    Destructor for a LOOP_IVS instance
//
//  void LOOP_IVS::Init( LOOP_DESCR *loop )
//    Initializes a LOOP_IVS instance for a loop with no internal
//    control, multiple exits and call instructions are allowded.
//
//  DefID_t OPND_IV_cycle(INT op_idx, INT opnd_idx);
//    Returns an ID for an Induction Variable for an operand that
//    belongs to an IV cycle. This ID is the last operation of the
//    loop that defines a TN that belongs to this IV cycle
//
//  DefID_t RES_IV_cycle(INT op_idx, INT res_idx);
//    Returns an ID for an Induction Variable for the defintion of a
//    TN that belongs to an IV cycle
//
//  INT OPND_IV_offset(INT op_idx, INT opnd_idx);
//    Returns the constant value from this use to the value of the IV
//    at the beginnning of a loop iteration.
//
//  INT IV_step(DefID_t iv_cycle);
//    Get the step of an IV
//
//  OP *IV_init(DefID_t iv_cycle);
//    Get the init operation outside of the loop of an IV
//
//  void Trace_IVs_Entries(const char *message);
//    Dump some traces for an LOOP_IVS instance.
//
// =======================================================================
// ======================================================================= */

