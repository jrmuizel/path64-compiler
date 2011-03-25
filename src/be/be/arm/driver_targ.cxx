/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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


#include "defs.h"
#include "be_errors.h"
#include "config.h"
#include "config_targ.h"
#include "ti_init.h"

/* ====================================================================
 *   Initialize_Targ_Info
 * ====================================================================
 */

BE_EXPORTED void*
Initialize_Targ_Info (void)
{
  ABI_PROPERTIES_ABI abi;
  ISA_SUBSET isa;
  PROCESSOR proc;

  switch (Target_ABI) {
  case ABI_ARM_ver1:
    abi = ABI_PROPERTIES_ABI_arm_PIC;
    break;
  case ABI_ARM_ver2:
    abi = ABI_PROPERTIES_ABI_arm_PIC;
    break;
  default:
    FmtAssert(FALSE, ("targinfo doesn't handle abi: %d\n", Target_ABI));
  }

  switch (Target_ISA) {
  case TARGET_ISA_armv5:
    isa = ISA_SUBSET_armv5e;
    break;
  case TARGET_ISA_armv6:
    isa = ISA_SUBSET_armv6;
    break;
  default:
    FmtAssert(FALSE, ("targinfo doesn't handle isa: %s\n", Isa_Name(Target_ISA)));
  }

  switch (Target) {
  case TARGET_armv5:
    proc = PROCESSOR_armv5e;
    break;
  case TARGET_armv6:
    proc = PROCESSOR_armv6;
    break;
  default:
    FmtAssert(FALSE, ("targinfo doesn't handle target: %s\n", Targ_Name(Target)));
  }

  return TI_Initialize(abi, isa, proc, 0, NULL, Targ_Path);
}

