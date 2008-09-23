/*
  Copyright (C) 2008, STMicroelectronics Inc.  All Rights Reserved.

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

#include "wintrinsic.h"
#include "wn.h"

/** 
 * Get INTRINSIC idx from intrinsic name.
 */
INTRINSIC EXTENSION_INTRINSIC_From_Name(const char * name);

/** 
 * add couple (intrinsic name, INTRINSIC) to map.
 */
void EXTENSION_add_INTRINSIC_to_Map(const char* c, INTRINSIC i);


/** 
 * Initialize pattern_rec
 */
void init_pattern_rec(void);
/**
 * main function of pattern_rec. Check the set of target specific
 * patterns in the WN tree. 
 */
INTRINSIC targ_pattern_rec(WN *tree, INT *nboperands,  WN *kids[]);
