/*
  Copyright (C) 2002-2009, STMicroelectronics, All Rights Reserved.

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

/* This file holds definitions specific to the ARM ELF ABI. */

#ifndef _TARG_ELF_H
#define _TARG_ELF_H

#include <elf.h>

/* (pp) core */
#define _ELF_ARM_CORE_BIT	        (8)                      /* 1st bit position in byte */
#define ELF_ARM_CORE_MASK	        (0xF<<_ELF_ARM_CORE_BIT)      /* mask */
#define ELF_ARM_CORE_armv5	        (0x0<<_ELF_ARM_CORE_BIT)
#define ELF_ARM_CORE_armv6	        (0x1<<_ELF_ARM_CORE_BIT)
#define ELF_ARM_CORE_UNDEF	        (0x5<<_ELF_ARM_CORE_BIT)
#define _ELF_ARM_CHECK_CORE(m)          ((m&ELF_ARM_CORE_MASK)==m)

/* (pp) cut */
#define _ELF_ARM_CUT_BIT	(16)                             /* 1st bit position in byte */
#define ELF_ARM_CUT_MASK	(0xF<<_ELF_ARM_CUT_BIT)        /* mask */
#define ELF_ARM_CUT_0	(0x0<<_ELF_ARM_CUT_BIT)
#define ELF_ARM_CUT_1	(0x1<<_ELF_ARM_CUT_BIT)
#define ELF_ARM_CUT_2	(0x2<<_ELF_ARM_CUT_BIT)
#define ELF_ARM_CUT_3	(0x3<<_ELF_ARM_CUT_BIT)
#define ELF_ARM_CUT_4	(0x4<<_ELF_ARM_CUT_BIT)
#define ELF_ARM_CUT_5	(0x5<<_ELF_ARM_CUT_BIT)
#define ELF_ARM_CUT_UNDEF	(0x6<<_ELF_ARM_CUT_BIT)
#define _ELF_ARM_CHECK_CUT(m) ((m&ELF_ARM_CUT_MASK)==m)

/* (pp) abi */
/* [JV] This is aligned with elf spec. */
#define _ELF_ARM_ABI_BIT	(12)                           /* 1st bit position in byte */
#define ELF_ARM_ABI_MASK	(0xF<<_ELF_ARM_ABI_BIT)       /* mask */
#define ELF_ARM_ABI_NO	        (0x0<<_ELF_ARM_ABI_BIT)
#define ELF_ARM_ABI_VER1	(0x1<<_ELF_ARM_ABI_BIT)
#define ELF_ARM_ABI_VER2	(0x2<<_ELF_ARM_ABI_BIT)
#define ELF_ARM_ABI_UNDEF	(0x3<<_ELF_ARM_ABI_BIT)
#define ELF_ARM_ABI_RELOC_EMBED (0x4<<_ELF_ARM_ABI_BIT)
#define _ELF_ARM_CHECK_ABI(m) ((m&ELF_ARM_ABI_MASK)==m)

/* [CL] code generation model */
#define _ELF_ARM_CODEGEN_BIT (24)
#define ELF_ARM_CODEGEN_MASK        (0x7<<_ELF_ARM_CODEGEN_BIT)
#define ELF_ARM_CODEGEN_CPIC        (0x2<<_ELF_ARM_CODEGEN_BIT)
#define ELF_ARM_CODEGEN_RELOCATABLE (0x3<<_ELF_ARM_CODEGEN_BIT)
#define _ELF_ARM_CHECK_CODEGEN(m)   ((m&ELF_ARM_CODEGEN_MASK)==m)

/* Already defined in elf.h and defined to 40 */
/*#define EM_ARM 0x64 */

#endif
