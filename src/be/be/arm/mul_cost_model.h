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
 * Module: mul_cost_model.h
 *
 * Description:
 *
 * This module provides the cost model for multiplications (target dependent)
 *
 * MUL_COST_MODEL_T *Get_Mul_Cost_Model(TYPE_ID rtype)
 *
 *   Returns the cost model of a multiplication, which depends on the target.
 *   For STxP70, it also depends on the availability of the mp instruction.
 *
 * ==================================================================== */

#ifndef mul_cost_model_INCLUDED
#define mul_cost_model_INCLUDED

#include "wn_lower_mul.h"

/* ====================================================================
 * Cost model of multiply
 * ====================================================================
 */

extern MUL_COST_MODEL_T *Get_Mul_Cost_Model(TYPE_ID rtype);


#endif /* mul_cost_model_INCLUDED */
