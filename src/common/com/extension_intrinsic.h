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

#include "extension_pattern_rec.h"
#include "wintrinsic.h"
#include "wn.h"
#include <list>

/** 
 * Get INTRINSIC idx from intrinsic name.
 */
INTRINSIC EXTENSION_INTRINSIC_From_Name(const char * name);

/** 
 * add couple (intrinsic name, INTRINSIC) to map.
 */
void EXTENSION_add_INTRINSIC_to_Map(const char* c, INTRINSIC i);


/**
 * Function that generates an intrinsic OP from extension code generation.
 * Note: This function should only be called from BETARG_Create_Intrinsic_from_OP()
 * Return the corresponding whirl node in case of success, return NULL otherwise.
 */
WN *Create_Intrinsic_from_OP(INTRINSIC intrnidx, int nbkids, WN *kids[],
                             INT nboutputs,  ST* outputs[],
                             TYPE_ID dsttype, WN** new_stmts,
			     bool* modified);

/** 
 * Initialize pattern_rec
 * 
 */
void
BETARG_Init_Pattern_Rec(void);

/**
 * Function that generates an intrinsic OP from extension code generation.
 * Return the corresponding whirl node in case of success, return NULL otherwise.
 */
WN *BETARG_Create_Intrinsic_from_OP(INTRINSIC intrnidx, int nbkids, WN *kids[],
				    TYPE_ID dsttype, WN** new_stmts,
				    bool* modified);

/** 
 * retrun the value of the extension option mask (takes into account
 * presence of pragmas modifying the Enable_Extension_Native_Support value)
 * 
 * 
 * @return the mask value
 */
INT32 ext_lower_get_local_ext_gen_mask();


typedef std::list<recog_rule*> ListOfRules;
typedef ListOfRules::const_iterator ListOfRulesCIt;
extern ListOfRules*  Get_rules_per_opcode(OPCODE opc);
extern ListOfRules*  Get_rules_per_intrinsic(INTRINSIC opc);
extern ListOfRules*  Get_Extension_pattern_rules();
