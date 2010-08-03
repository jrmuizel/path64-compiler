/* 

  Copyright (C) 2007 ST Microelectronics, Inc.  All Rights Reserved. 

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

/* Assembly Intermediate Representation
 * STxP70 target specific header file
 */
#ifndef __TARG_AIR_H__
#define __TARG_AIR_H__

#include "topcode.h"

#include "air.h"
#include "air_services.h"

#ifdef __cplusplus
extern "C" {
#endif

extern INT AIR_target_init(void);
extern INT AIR_target_end(void);

#ifdef TARG_STxP70
extern int stxp70_pre_print_fct ( AIR_Print * );
#define DISASM_DECODE_pre_print_fct stxp70_pre_print_fct
#endif

#ifdef __cplusplus
}
#endif
#endif /* __TARG_AIR_H__*/
