/*

  Copyright (C) 2005-2006 ST Microelectronics, Inc.  All Rights Reserved.

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

  Contact information:  ST Microelectronics, Inc., 
  address, or:

  http://www.st.com

*/

/* ====================================================================
 *
 * Module: mul_cost_model.cxx
 *
 * Description:
 *
 * This module provides the cost model for multiplications (target dependent)
 *
 * Interface exported to wn_lower is:
 *
 * MUL_COST_MODEL_T *Get_Mul_Cost_Model(TYPE_ID rtype)
 *
 *   Returns the cost model of a multiplication, which depends on the target.
 *   For STxP70, it also depends on the availability of the mp instruction.
 *
 * ==================================================================== */


#include "defs.h"
#include "config_TARG.h"// For Enable_Mx
#include "wn_lower_mul.h"



/* Effective costs, rougthly:
 * mul cycles: half the bit length
 * add/shift/sub cycles: 1 for 32 bits, 3 for 64 bits.
 * cnst cycles: 1 for 32/64 bits
 * mul size: cost of a call (4) for 32/64 bits
 * add/mul/shift size: 1 for 32 bits, 4 for 64 bits.
 * cnst size: 1 for 32/64 bits
 */
   
static MUL_COST_MODEL_T mul_cost_64 = {
  /* Mul cycle cost for 64 bits. */
  { 32, 4, 3, 3, 1 },
  /* Mul size cost for 64 bits. */
  { 4, 4, 4, 4, 1 }};
static MUL_COST_MODEL_T mul_cost_32 = {
  /* Mul cycle cost for 32 bits. */
  { 16, 1, 1, 1, 1 },
  /* Mul size cost for 32 bits. */
  { 4, 1, 1, 1, 1 }};

/*
 * Get_Mul_Cost_Model()
 * Returns the cost model for the given type.
 * Returns NULL if no cost is available.
 */
MUL_COST_MODEL_T *
Get_Mul_Cost_Model(TYPE_ID rtype)
{
  if (MTYPE_byte_size(rtype) <= 4) {
    return &mul_cost_32;
  }
  if (MTYPE_byte_size(rtype) <= 8) return &mul_cost_64;
  return NULL;
}
  
