/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

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

//
// Generate an ISA containing the instructions specified.
/////////////////////////////////////////////////////////
// The instructions are listed by category. The different categories of
// instructions are:
//
//   1. Integer instructions
//   2. FP instructions
//   3. Simulated instructions
//   4. Dummy instructions
//
// Within each category, the instructions are in alphabetical order.
// This arrangement of instructions matches the order in the ISA manual.
/////////////////////////////////////
//
//  $Revision: 1.4 $
//  $Date: 05/03/02 15:27:55-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/isa/mips/SCCS/s.isa.cxx $

#include <stddef.h>
#include "isa_gen.h"

main ()
{
  ISA_Create ("MIPS", 
	"lb",
	"lbu",
	"lh",
	"lhu",
	"lw",
	"lwl",
	"lwr",
	"sb",
	"sh",
	"sw",
	"swl",
	"swr",
	"ll",
	"sc",
	"sync",
	"lwu",
	"ld",
	"ldl",
	"ldr",
	"lld",
	"sd",
	"sdl",
	"sdr",
	"scd",
	"pref",
	"prefx",
	"add",
	"addi",
	"addiu",
	"addu",
	"div",
	"divu",
	"mul",
	"mult",
	"multu",
	"slt",
	"slti",
	"sltiu",
	"sltu",
	"sub",
	"subu",
	"dadd",
	"daddi",
	"daddiu",
	"daddu",
	"ddiv",
	"ddivu",
	"dmult",
	"dmultu",
	"dsub",
	"dsubu",
	"and",
	"andi",
	"lui",
	"nor",
	"or",
	"ori",
	"xor",
	"xori",
	"clo",
	"clz",
	"dclo",
	"dclz",
	"mfhi",
	"mflo",
	"mthi",
	"mtlo",
	"movf",
	"movn",
	"movt",
	"movz",
	"sll",
	"sllv",
	"sra",
	"srav",
	"srl",
	"srlv",
	"dsll",
	"dsll32",
	"dsllv",
	"dsra",
	"dsra32",
	"dsrav",
	"dsrl",
	"dsrl32",
	"dsrlv",
	"beq",
	"bgez",
	"bgezal",
	"bgtz",
	"blez",
	"bltz",
	"bltzal",
	"bne",
	"j",
	"jal",
	"jalr",
	"jr",
	"break",
	"syscall",
	"teq",
	"teqi",
	"tge",
	"tgei",
	"tgeiu",
	"tgeu",
	"tlt",
	"tlti",
	"tltiu",
	"tltu",
	"tne",
	"tnei",
	"lwc1",
	"ldc1",
	"lwxc1",
	"ldxc1",
	"swc1",
	"sdc1",
	"swxc1",
	"sdxc1",
	"abs.s",
	"abs.d",
	"add.s",
	"add.d",
	"c.f.s",
	"c.f.d",
	"c.t.s",
	"c.t.d",
	"c.un.s",
	"c.un.d",
	"c.or.s",
	"c.or.d",
	"c.eq.s",
	"c.eq.d",
	"c.neq.s",
	"c.neq.d",
	"c.ueq.s",
	"c.ueq.d",
	"c.ogl.s",
	"c.ogl.d",
	"c.olt.s",
	"c.olt.d",
	"c.uge.s",
	"c.uge.d",
	"c.ult.s",
	"c.ult.d",
	"c.oge.s",
	"c.oge.d",
	"c.ole.s",
	"c.ole.d",
	"c.ugt.s",
	"c.ugt.d",
	"c.ule.s",
	"c.ule.d",
	"c.ogt.s",
	"c.ogt.d",
	"c.sf.s",
	"c.sf.d",
	"c.st.s",
	"c.st.d",
	"c.ngle.s",
	"c.ngle.d",
	"c.gle.s",
	"c.gle.d",
	"c.seq.s",
	"c.seq.d",
	"c.sne.s",
	"c.sne.d",
	"c.ngl.s",
	"c.ngl.d",
	"c.gl.s",
	"c.gl.d",
	"c.lt.s",
	"c.lt.d",
	"c.nlt.s",
	"c.nlt.d",
	"c.nge.s",
	"c.nge.d",
	"c.ge.s",
	"c.ge.d",
	"c.le.s",
	"c.le.d",
	"c.nle.s",
	"c.nle.d",
	"c.ngt.s",
	"c.ngt.d",
	"c.gt.s",
	"c.gt.d",
	"div.s",
	"div.d",
	"mul.s",
	"mul.d",
	"neg.s",
	"neg.d",
	"sub.s",
	"sub.d",
	"sqrt.s",
	"sqrt.d",
	"madd.s",
	"madd.d",
	"msub.s",
	"msub.d",
	"nmadd.s",
	"nmadd.d",
	"nmsub.s",
	"nmsub.d",
	"recip.s",
	"recip.d",
	"rsqrt.s",
	"rsqrt.d",
	"cfc1",
	"ctc1",
	"mfc1",
	"mtc1",
	"dmfc1",
	"dmtc1",
	"mov.s",
	"mov.d",
	"movf.s",
	"movf.d",
	"movn.s",
	"movn.d",
	"movt.s",
	"movt.d",
	"movz.s",
	"movz.d",
	"cvt.d.s",
	"cvt.d.w",
	"cvt.d.l",
	"cvt.l.s",
	"cvt.l.d",
	"cvt.s.d",
	"cvt.s.w",
	"cvt.s.l",
	"cvt.w.s",
	"cvt.w.d",
	"ceil.w.s",
	"ceil.w.d",
	"ceil.l.s",
	"ceil.l.d",
	"floor.w.s",
	"floor.w.d",
	"floor.l.s",
	"floor.l.d",
	"round.w.s",
	"round.w.d",
	"round.l.s",
	"round.l.d",
	"trunc.w.s",
	"trunc.w.d",
	"trunc.l.s",
	"trunc.l.d",
	"bc1f",
	"bc1t",

	// MIPS32/MIPS64 Release 2
	"dsbh",
	"dshd",
	"seb",
	"seh",
	"wsbh",
	"dext",
	"dextm",
	"dextu",
	"dins",
	"dinsm",
	"dinsu",
	"drotr",
	"drotr32",
	"drotrv",
	"ext",
	"ins",
	"rotr",
	"rotrv",
	"alnv.ps",
	"movn.ps",
	"movz.ps",
	"abs.ps",
	"c.eq.ps",
	"c.f.ps",
	"c.ge.ps",
	"c.gl.ps",
	"c.gle.ps",
	"c.gt.ps",
	"c.le.ps",
	"c.lt.ps",
	"c.neq.ps",
	"c.nge.ps",
	"c.ngl.ps",
	"c.ngle.ps",
	"c.ngt.ps",
	"c.nle.ps",
	"c.nlt.ps",
	"c.oge.ps",
	"c.ogl.ps",
	"c.ogt.ps",
	"c.ole.ps",
	"c.olt.ps",
	"c.or.ps",
	"c.seq.ps",
	"c.sf.ps",
	"c.sne.ps",
	"c.st.ps",
	"c.t.ps",
	"c.ueq.ps",
	"c.uge.ps",
	"c.ugt.ps",
	"c.ule.ps",
	"c.ult.ps",
	"c.un.ps",
	"cvt.ps.s",
	"cvt.s.pl",
	"cvt.s.pu",
	"mov.ps",
	"movf.ps",
	"movt.ps",
	"neg.ps",
	"pll.ps",
	"plu.ps",
	"pul.ps",
	"puu.ps",
	"add.ps",
	"madd.ps",
	"msub.ps",
	"mul.ps",
	"nmadd.ps",
	"nmsub.ps",
	"sub.ps",

	// MIPS32/MIPS64 Release 2 instructions not used by compiler:
	// di
	// ei
	// rdpgpr
	// wrpgpr
	// rdhwr
	// synci

	// SiCortex IceT Instructions
	// IceT integer instructions
	"lbx",
	"ldx",
	"lhx",
	"lwx",
	"madd",
	"maddu",
	"msub",
	"msubu",
	"dmadd",
	"dmaddu",
	"dmsub",
	"dmsubu",
	"dmulg",
	"dmulgu",
	"mfacx",
	"mtacx",
	"dperm",
	"pop",
	"dpop",
	// IceT FP instructions
	"luxc1",
	"suxc1",
	"addred.ps",
	"mulred.ps",
	"ma.nlupuu.ps",
	"ma.plunuu.ps",
	"ma.plupll.ps",
	"ma.puupul.ps",
	"recipit1.d",
	"recipit1.s",
	"recipit1.ps",
	"recipit2.d",
	"recipit2.s",
	"recipit2.ps",
	"rsqrtit1.d",
	"rsqrtit1.s",
	"rsqrtit1.ps",
	"rsqrtit2.d",
	"rsqrtit2.s",
	"rsqrtit2.ps",
	"mfhc1",
	"mthc1",
	"bc2any2f",
	"bc2any2t",
	// IceT other instructions
	"perfctl",
	"tlbwir",
	"tlbwrr",

        // And pseudo-opcodes
	"sext",
	"asm",
	"intrncall",
	"spadjust",
	"mov_hilo",
        "mov_fcc",

	// Dummy instructions
	"begin_pregtn",
	"end_pregtn",
	"bwd_bar",
	"fwd_bar",
	"dfixup",
	"ffixup",
	"ifixup",
	"label",
	"nop",
	"noop",
        "swp_start",
        "swp_stop",
	NULL);
}
