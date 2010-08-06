/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

/*
 * API to get information about extension
 */
#ifndef EXT_INFO_H
#define EXT_INFO_H

#define INVALID_EXTENSION_ID (0)
/*
 * Bit used to activate traces of extension support
 */
#define TRACE_EXTENSION_LOADER_MASK        0x0001 /* Trace of extension loader */
#define TRACE_EXTENSION_REG_PLACEMENT_MASK 0x0002 /* Trace of register placement
						     at whirl level for native 
						     support */

BE_EXPORTED extern INT32 EXTENSION_Get_ExtensionId_From_ExtensionName(const char *extname);

/* Return TRUE if conversion from src to tgt is allowed, FALSE otherwise.
 * At least one of the type is expected to be an extension one. */
BE_EXPORTED extern BOOL EXTENSION_Are_Equivalent_Mtype(TYPE_ID src_ty, TYPE_ID tgt_ty);

BE_EXPORTED extern INTRINSIC EXTENSION_Get_Convert_Intrinsic(TYPE_ID src_ty, TYPE_ID tgt_ty, BOOL ignore_sign=TRUE);

BE_EXPORTED extern TYPE_ID EXTENSION_Get_Equivalent_Mtype(TYPE_ID ext_ty);

BE_EXPORTED extern INTRINSIC EXTENSION_Get_CLR_Intrinsic(TYPE_ID ty);

BE_EXPORTED extern INTRINSIC EXTENSION_Get_Convert_From_U32_Intrinsic(TYPE_ID ty);

BE_EXPORTED extern BOOL EXTENSION_Is_ExtGen_Enabled_For_Intrinsic(INTRINSIC intrn, long long flags);

BE_EXPORTED extern long long
EXTENSION_Get_ExtOption_Flag_By_Name(int extid, const char* optionname);

BE_EXPORTED extern BOOL EXTENSION_Is_Defined(const char *extname);
BE_EXPORTED extern int EXTENSION_Get_ExtensionRank_From_Intrinsic(INTRINSIC);

/*
 * Return TRUE if native codegen is enabled for at least one extension
 */
BE_EXPORTED extern BOOL EXTENSION_Has_ExtGen_Enabled();

/*
 * Return TRUE if the parameter intrinsic Id belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_INTRINSIC(INTRINSIC id) {
  return (id > INTRINSIC_STATIC_COUNT && id <= INTRINSIC_COUNT);
}


#include <vector>
typedef std::vector<INTRINSIC> INTRINSIC_Vector_t;
#define OPCODE_MAPPED_ON_CORE INTRINSIC_INVALID
BE_EXPORTED extern INTRINSIC_Vector_t* Get_Intrinsic_from_OPCODE(OPCODE opc);
BE_EXPORTED extern void Add_Intrinsic_for_OPCODE(OPCODE opc, INTRINSIC intrn);


#endif /* EXT_INFO_H */
