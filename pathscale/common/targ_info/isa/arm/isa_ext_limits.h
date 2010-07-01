/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/
//
// This file defines the scope of the extensions supported by the compiler
//
#ifndef _EXTENSION_LIMITS_H_INCLUDED_
#define _EXTENSION_LIMITS_H_INCLUDED_

#define EL_MAX_EXT             (8)  /* Maximum number of extension              */
#define EL_MAX_SIMD_RC_PER_EXT (2)  /* Maximum SIMD register classes per ext    */
#define EL_MAX_SFR_RC_PER_EXT  (1)  /* Maximum special register classes per ext */
#define EL_MAX_SUBRC_PER_RC    (10) /* Maximum subclasses defined per SIMD class
									 *   Considering RC defines the Q level, 
									 *   the following subclasses are needed:
									 *   - 1 for X level
									 *   - 1 for P level
									 *   - 2 for X_P<0..1> level (not used)
									 *   - 4 for X_Q<0..3> level (for TOP multi)
									 *   - 2 for P_Q<0..1> level (for TOP multi)
									 */

#define EXTENSION_NB_REGISTER_MAX           (64)
#define EXTENSION_NB_REGISTER_CLASS_MAX     (EL_MAX_EXT * (EL_MAX_SIMD_RC_PER_EXT + EL_MAX_SFR_RC_PER_EXT))
#define EXTENSION_NB_REGISTER_SUBCLASS_MAX  (EL_MAX_EXT * EL_MAX_SIMD_RC_PER_EXT * EL_MAX_SUBRC_PER_RC)
#define EXTENSION_NB_OPERAND_MAX            (27) /* Max operand count is defined by following read:
						  * - 8 SFRs,
						  * - 4 extension registers * 4 for MULTI version = 16,
						  * - 2 GPRs read
						  * - 1 Memory operand
						  * NOTE: It might be slightly overestimated considering
						  *       that memory are currently not real operands and
						  *       that GPR accesses might replace extension register
						  *       accesses.
						  */
#define EXTENSION_NB_RESULT_MAX             (26) /* Max result count is defined by following write:
						  * - 8 SFRs,
						  * - 4 extension registers * 4 for MULTI version = 16,
						  * - 1 GPRs read
						  * - 1 Memory operand
						  * NOTE: It might be slightly overestimated considering
						  *       that memory are currently not real operands and
						  *       that GPR accesses might replace extension register
						  *       accesses.
						  */
#endif
