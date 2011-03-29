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
//  Module: cg_dud.h
//
//  Description:
//  ============
//
//  This module builds the def-use and use-def links on a single-entry
//  BB_REGION
//
//  Exported Classes:
//  =================
//
//  DUD_REGION
//    Implements the def-use and use-def on a
//    single-entry/multiple-exit region. The region can contain
//    control-flow and loops.
//
//  DUD_LIST
//    Represents a list of def-use or use-def links
//
// 
//  Exported Functions:
//  ===================
//
//  DUD_REGION *DUD_REGION::DUD_REGION(MEM_POOL *mem_pool)
//    Constructor for the DUD_REGION object.
//
//  DUD_REGION::Init(BB_REGION *bb_region, MEM_POOL *region_pool);
//    Computes the Def-Use/Use-Def links between TNs in a BB_REGION.
//
//  DUD_REGION *Build_DUD_info(BB_REGION *bb_region, MEM_POOL *region_pool);
//    Create a DUD_REGION object, build the def-use/use-def links in
//    the region, and return a DUD_REGION object if successful.
//
//  DUD_REGION::Trace_DUD()
//    Trace the Def-Use/Use-Def links
//
//  INT DUD_REGION::Get_Use_Def(OP *op, INT opnd, DUD_LIST &ud_list);
//    Returns in ud_list the list of use-def links from a given (op,
//    opnd) in the region.
//
//  INT DUD_REGION::Get_Def_Use(OP *op, INT res, DUD_LIST &du_list);
//    Returns in du_list the list of def-use links from a given (op,
//    res) in the region.
//
//  INT DUD_REGION::Get_opid(OP *op)
//    Returns the index in topological order of an operation in the
//    region. Returns 0 if op is not in the region.
//
//  OP *DUD_REGION::Begin_op()
//  OP *DUD_REGION::End_op()
//  OP *DUD_REGION::Next_op()
//    These functions are used to iterate on the operations in the
//    DUD_REGION, in topological order:
//      for (op = dudRegion.Begin_op(); op != dudRegion.End_op(); op = dudRegion.Next_op(op))
//
//  OP *DUD_REGION::Last_op()
//  OP *DUD_REGION::Prev_op()
//    These functions are used to iterate on the operations in the
//    DUD_REGION, in reverse topological order:
//      for (op = dudRegion.Last_op(); op != NULL; op = dudRegion.Prev_op(op))
//
//  INT DUD_LIST::size()
//    Returns the number of elements in a def-use or use-def list
//
//  OP *DUD_LIST::op(INT listIdx)
//    Returns the operation that is referenced by a DU or UD link at
//    index listIdx in the def-use or use-def list. Will return NULL
//    in case of a DU or UD link that refers to an operation outside
//    the region.
//
//  INT DUD_LIST::idx(INT listIdx)
//    Returns the index of the operand or result in the operation that
//    is referenced by a DU or UD link at index listIdx in the def-use
//    or use-def list.
//
// BOOL DUD_LIST::is_def(INT listIdx)
//    Returns whether DUD_LIST::idx(listIdx) returned an operand or a
//    result index in the operation that is referenced by a DU or UD
//    link at index listIdx in the def-use or use-def list.
//
// BOOL DUD_LIST::has_partial_def()
//    Returns TRUE if this is a Def-Use list and the list contains
//    some definitions. These definitions are actually predicated
//    definitions that introduces implicit uses.
//
// =======================================================================
// ======================================================================= */

