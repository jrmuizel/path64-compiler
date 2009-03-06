/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* ====================================================================
 * ====================================================================
 *
 * Module: config_elf_targ.cxx
 *
 * Description:
 *
 * ELF configuration specific to the target machine/system.
 *
 * ====================================================================
 * ====================================================================
 */

#include <elf.h>
#include <sys/elf_whirl.h>
#include "defs.h"
#include "erglob.h"
#include "tracing.h"
#include "config_elf_targ.h"

#ifdef TARG_ST
// [CL] include ST200 specific values
#include "targ_elf.h"
#include "config_target.h"
#include "config.h"
#endif

/* ====================================================================
 *   Config_Target_From_ELF
 *
 *   Based on the flags in the ELF header <ehdr>, return whether 
 *   is 64bit and the isa level.
 * ====================================================================
 */
void 
Config_Target_From_ELF (
  Elf64_Word e_flags, 
  BOOL *is_64bit, 
  INT *isa
)
{
#ifndef TARG_ST
  *is_64bit = (e_flags & EF_IRIX_ABI64);
  *isa = 1;
#else
  // [CL] check coherency between user options and file flags
  *is_64bit = FALSE;
  *isa = (INT)Target_ISA;

  switch(e_flags & ELF_ARM_CORE_MASK) {
  case ELF_ARM_CORE_armv5:
    if ((Target != TARGET_armv5) && (Target != TARGET_UNDEF)) {
      ErrMsg(EC_Conf_Targ, "processor", Targ_Name(Target), Targ_Name(TARGET_armv5));
    }
    if (Target_ABI == ABI_UNDEF) {
      Target = TARGET_armv5;
    }
    break;
  case ELF_ARM_CORE_armv6:
    if ((Target != TARGET_armv6) && (Target != TARGET_UNDEF)) {
      ErrMsg(EC_Conf_Targ, "processor", Targ_Name(Target), Targ_Name(TARGET_armv6));
    }
    if (Target_ABI == ABI_UNDEF) {
      Target = TARGET_armv6;
    }
    break;
  default:
    ErrMsg(EC_Conf_Targ, "processor", Targ_Name(Target), "undefined/unsupported");
    Target = TARGET_UNDEF;
    break;
  }

  switch(e_flags & ELF_ARM_ABI_MASK) {
  case ELF_ARM_ABI_VER1:
    if ((Target_ABI != ABI_ARM_ver1) &&
	(Target_ABI != ABI_UNDEF)) {
      ErrMsg(EC_Conf_Targ, "abi", Abi_Name(Target_ABI), Abi_Name(ABI_ARM_ver1));
    }
    if (Target_ABI == ABI_UNDEF) { 
      Target_ABI = ABI_ARM_ver1;
    }
    break;
  case ELF_ARM_ABI_VER2:
    if ((Target_ABI != ABI_ARM_ver2) &&
	(Target_ABI != ABI_UNDEF)) {
      ErrMsg(EC_Conf_Targ, "abi", Abi_Name(Target_ABI), Abi_Name(ABI_ARM_ver2));
    }
    if (Target_ABI == ABI_UNDEF) { 
      Target_ABI = ABI_ARM_ver2;
    }
    break;
  default:
    ErrMsg(EC_Conf_Targ, "abi", Abi_Name(Target_ABI), "undefined/unsupported");
    Target_ABI = ABI_UNDEF;
    break;
  }

  switch (e_flags & ELF_ARM_CODEGEN_MASK) {
  case ELF_ARM_CODEGEN_CPIC:
    if ( (!Gen_PIC_Call_Shared && Gen_PIC_Call_Shared_Set) ||
	 (Gen_PIC_Shared && Gen_PIC_Shared_Set) ) {
      if (!No_Shared_Warning) {
	ErrMsg(EC_Conf_CodeGen, "-call_shared");
      }
    }
    break;
  }
#endif
}

/* ====================================================================
 *   Config_ELF_From_Target
 *
 *   Return the ELF specific flags for the current target ABI and 
 *   ISA subset.
 * ====================================================================
 */
Elf32_Word 
Config_ELF_From_Target (
  BOOL is_64bit, 
  BOOL old_abi, 
  INT isa
)
{
  Elf32_Word e_flags = 0;

#ifndef TARG_ST
  if (is_64bit) e_flags |= EF_IRIX_ABI64;
#else
  // [CL] setup ELF flags according to user options

  if (isa != (INT)TARGET_ISA_armv5 &&
      isa != (INT)TARGET_ISA_armv6) {
      // [HK]
//        ErrMsg ( EC_Inv_TARG, "isa", Isa_Name((enum TARGET_ISA)isa) );
      ErrMsg ( EC_Inv_TARG, "isa", Isa_Name((TARGET_ISA)isa) );
  }

  // Handle ABI
  switch(Target_ABI) {
  case ABI_ARM_ver1:
    e_flags |= ELF_ARM_ABI_VER1; break;
  case ABI_ARM_ver2:
    e_flags |= ELF_ARM_ABI_VER2; break;
  default:
    e_flags |= ELF_ARM_ABI_UNDEF; break;
  }

  // Skip Mode (User, Kernel)

  // Handle Core
  switch(Target) {
  case TARGET_armv5:
    e_flags |= ELF_ARM_CORE_armv5; break;
  case TARGET_armv6:
    e_flags |= ELF_ARM_CORE_armv6; break;
  default:
    e_flags |= ELF_ARM_CORE_UNDEF; break;
  }

  // Handle Code Generation Model
  if (Gen_PIC_Call_Shared) {
    e_flags |= ELF_ARM_CODEGEN_CPIC;
  }
#endif

  return e_flags;
}

/* ====================================================================
 *   Get_Elf_Target_Machine
 * ====================================================================
 */
Elf32_Half 
Get_Elf_Target_Machine (void)
{
  // [CL] this ID would need to be sync'ed with binutils
  return EM_ARM;
}

/* =====================================================================
 *   ELF_WHIRL_has_subset_section
 *
 *   Returns TRUE if WHIRL files generated in IPA process (ipl) contains
 *   a specific section (.WHIRL.subset) containing target specific
 *   information on subsets.
 *
 *   Subset section isn't implemented yet for ST200.
 * =====================================================================
 */
BOOL
ELF_WHIRL_has_subset_section (void)
{
    return FALSE;
}

/* ======================================================================
 *   ELF_WHIRL_subset_info_buf
 *
 *   Returns a buffer that contains subset information for ELF WHIRL
 *   file.
 *
 *   Input:  pool (MEM_POOL*)  memory pool where buffer is to be allocated.
 *   Output: buf  (char**)     buffer address or NULL.
 *           size (Elf64_Word) buffer size or 0.
 *
 *   Should never be called.
 * =====================================================================
 */
void
ELF_WHIRL_subset_info_buf (MEM_POOL *pool, char **buf, Elf64_Word *size)
{
     *buf  = NULL;
     *size = (Elf64_Word)0;

      FmtAssert(FALSE,
               ("Routine ELF_WHIRL_subset_info_buf should never be called"));
      return;
}

/* ======================================================================
 *   ELF_WHIRL_check_subset
 *
 *   Check that a subset section in correct.
 *
 *   Should never be called.
 * =====================================================================
 */
BOOL
ELF_WHIRL_check_subset (char *base, Elf64_Word size)
{
    FmtAssert(FALSE,
             ("Routine ELF_WHIRL_check_subset should never be called"));
    return TRUE;
}


