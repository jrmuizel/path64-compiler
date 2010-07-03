
/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

   Special thanks goes to SGI for their continued support to open source

*/

#ifndef __SGIDEFS_H__
#define __SGIDEFS_H__

#ident "$Revision$"

/*
 * sgidefs.h - SGI/MIPS basic software generation system constants & types
 *
 * This file MUST be includable in any language
 * and in the 'C' case must be ANSI compliant
 * In particular this means NO long long ...
 *
 * Constants and types defined here are to support
 * multi-isa (instruction set architecture) coding
 *
 * Each catagory has a define that the compilation system will set
 * based on the environment the compile is initiated in. Programs
 * can test the define using the manifest constants defined here.
 * (e.g. #if (_MIPS_FPSET == 16) )
 */

/*
 * Floating Point register set
 * Define:
 *	_MIPS_FPSET
 * Can take on the values 16 or 32
 */

/*
 * Instruction Set Architecture
 * Define:
 *	_MIPS_ISA
 */
#define _MIPS_ISA_MIPS1	1	/* R2/3K */
#define _MIPS_ISA_MIPS2	2	/* R4K/6K */
#define _MIPS_ISA_MIPS3	3	/* R4K */
#define _MIPS_ISA_MIPS4	4	/* TFP */

/*
 * Subprogram Interface Model
 * Define:
 *	_MIPS_SIM
 */
#define _MIPS_SIM_ABI32		1	/* MIPS MSIG calling convention */
#define _MIPS_SIM_NABI32	2	/* MIPS new 32-bit abi */
		/* NABI32 is 64bit calling convention but 32bit type sizes) */
#define _MIPS_SIM_ABI64		3	/* MIPS 64 calling convention */

#endif /* !__SGIDEFS_H__ */
