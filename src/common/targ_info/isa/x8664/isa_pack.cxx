/*
 * Copyright (C) 2008 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007 QLogic Corporation.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
// Group TOPS with similar packing format together. 
/////////////////////////////////////////////////////////
// The instructions are listed by category. The different categories of
// instructions are:
//
//   1. ALU
//   2. Integer
//   3. Memory
//   4. Branch
//   5. Float
//
// Within each Pack_Type instructions are listed in the order as shown
// in the IA-64 instructions formats manual
/////////////////////////////////////

#include <stddef.h>
#include "topcode.h"
#include "isa_pack_gen.h"
 
main()
{
  ISA_Pack_Begin("x8664", 32);

  ISA_PACK_TYPE p1 = ISA_Pack_Type_Create("p1");
  Result(0, 12, 4);
  Operand(0, 0, 16, 4);
  Operand(1, 0, 20, 4);
  Instruction_Pack_Group(p1,
			 TOP_addxxx32,	0x000000ff,
			 TOP_addxxx64,	0x000000ff,
			 TOP_subxxx32,	0x000000ff,
			 TOP_subxxx64,	0x000000ff,
			 TOP_xorxxx8,	0x000000ff,
			 TOP_xorxxx16,	0x000000ff,
			 TOP_xorxxx32,	0x000000ff,
			 TOP_xorxxx64,	0x000000ff,
			 TOP_orxxx8,	0x000000ff,
			 TOP_orxxx16,	0x000000ff,
			 TOP_orxxx32,	0x000000ff,
			 TOP_orxxx64,	0x000000ff,
			 TOP_andxxx8,	0x000000ff,
			 TOP_andxxx16,	0x000000ff,
			 TOP_andxxx32,	0x000000ff,
			 TOP_andxxx64,	0x000000ff,
			 TOP_divxxxss,	0x000000ff,
			 TOP_divxxxsd,	0x000000ff,
			 TOP_addxxxss,	0x000000ff,
			 TOP_addxxxsd,	0x000000ff,
			 TOP_subxxxss,	0x000000ff,
			 TOP_subxxxsd,	0x000000ff,
			 TOP_mulxxxsd,	0x000000ff,
			 TOP_mulxxxss,	0x000000ff,
			 TOP_cmpxxx8,	0x000000ff,
			 TOP_cmpi8,	0x000000ff,
			 TOP_cmpxi8,	0x000000ff,
			 TOP_cmpxxi8,	0x000000ff,
			 TOP_cmpxxxi8,	0x000000ff,
			 TOP_cmpi16,	0x000000ff,
			 TOP_cmpxi16,	0x000000ff,
			 TOP_cmpxxi16,	0x000000ff,
			 TOP_cmpxxxi16,	0x000000ff,
			 TOP_cmpxi32,	0x000000ff,
			 TOP_cmpxxi32,	0x000000ff,
			 TOP_cmpxxxi32,	0x000000ff,
			 TOP_cmpxi64,	0x000000ff,
			 TOP_cmpxxi64,	0x000000ff,
			 TOP_cmpxxxi64,	0x000000ff,
			 TOP_cmpxr8,	0x000000ff,
			 TOP_cmpxxr8,	0x000000ff,
			 TOP_cmpxxxr8,	0x000000ff,
			 TOP_cmpxr16,	0x000000ff,
			 TOP_cmpxxr16,	0x000000ff,
			 TOP_cmpxxxr16,	0x000000ff,
			 TOP_cmpxr32,	0x000000ff,
			 TOP_cmpxxr32,	0x000000ff,
			 TOP_cmpxxxr32,	0x000000ff,
			 TOP_cmpxr64,	0x000000ff,
			 TOP_cmpxxr64,	0x000000ff,
			 TOP_cmpxxxr64,	0x000000ff,
			 TOP_cmpxxx16,	0x000000ff,
			 TOP_cmpxxx32,	0x000000ff,
			 TOP_cmpxxx64,	0x000000ff,
			 TOP_testxxx32,	0x000000ff,
			 TOP_testxxx64,	0x000000ff,
		    TOP_nop,	0x000000ff,
		    TOP_andnps,	0x000000ff,
		    TOP_andnpd,	0x000000ff,
		    TOP_cmpss,	0x000000ff,
		    TOP_cmpsd,	0x000000ff,
		    TOP_cmpps,	0x000000ff,
		    TOP_cmppd,	0x000000ff,
		    TOP_cmpneqps, 0x000000ff,
		    TOP_add32,	0x000000ff,
		    TOP_adc32,	0x000000ff,
		    TOP_add64,	0x000000ff,
		    TOP_addx32,	0x000000ff,
		    TOP_addx64,	0x000000ff,
		    TOP_addxx32,0x000000ff,
		    TOP_addxx64,0x000000ff,
		    TOP_addi32,	0x000000ff,
		    TOP_adci32,	0x000000ff,
		    TOP_addi64,	0x000000ff,
		    TOP_inc8,   0x000000ff,
		    TOP_inc16,  0x000000ff,
		    TOP_inc32,  0x000000ff,
		    TOP_inc64,  0x000000ff,
		    TOP_dec8,   0x000000ff,
		    TOP_dec16,  0x000000ff,
		    TOP_dec32,  0x000000ff,
		    TOP_dec64,  0x000000ff,
		    TOP_zero32,   0x000000ff,
		    TOP_zero64,   0x000000ff,
		    TOP_xzero32,   0x000000ff,
		    TOP_xzero64,   0x000000ff,
		    TOP_xzero128v32,   0x000000ff,
		    TOP_xzero128v64,   0x000000ff,
		    TOP_mul128v16,	0x000000ff,
		    TOP_add128v8,	0x000000ff,
		    TOP_addx128v8,	0x000000ff,
		    TOP_addxx128v8,	0x000000ff,
		    TOP_addxxx128v8,	0x000000ff,
		    TOP_add128v16,	0x000000ff,
		    TOP_addx128v16,	0x000000ff,
		    TOP_addxx128v16,	0x000000ff,
		    TOP_addxxx128v16,	0x000000ff,
		    TOP_add128v32,	0x000000ff,
		    TOP_addx128v32,	0x000000ff,
		    TOP_addxx128v32,	0x000000ff,
		    TOP_addxxx128v32,	0x000000ff,
		    TOP_add128v64,	0x000000ff,
		    TOP_addx128v64,	0x000000ff,
		    TOP_addxx128v64,	0x000000ff,
		    TOP_addxxx128v64,	0x000000ff,
		    TOP_fadd128v32,	0x000000ff,
		    TOP_faddx128v32,	0x000000ff,
		    TOP_faddxx128v32,	0x000000ff,
		    TOP_faddxxx128v32,	0x000000ff,
		    TOP_fadd128v64,	0x000000ff,
		    TOP_faddx128v64,	0x000000ff,
		    TOP_faddxx128v64,	0x000000ff,
		    TOP_faddxxx128v64,	0x000000ff,
		    TOP_faddsub128v32,	0x000000ff,
		    TOP_faddsubx128v32,	0x000000ff,
		    TOP_faddsubxx128v32,	0x000000ff,
		    TOP_faddsubxxx128v32,	0x000000ff,
		    TOP_faddsub128v64,	0x000000ff,
		    TOP_faddsubx128v64,	0x000000ff,
		    TOP_faddsubxx128v64,	0x000000ff,
		    TOP_faddsubxxx128v64,	0x000000ff,
		    TOP_fhadd128v32,	0x000000ff,
		    TOP_fhaddx128v32,	0x000000ff,
		    TOP_fhaddxx128v32,	0x000000ff,
		    TOP_fhaddxxx128v32,	0x000000ff,
		    TOP_fhadd128v64,	0x000000ff,
		    TOP_fhaddx128v64,	0x000000ff,
		    TOP_fhaddxx128v64,	0x000000ff,
		    TOP_fhaddxxx128v64,	0x000000ff,
		    TOP_and128v8,	0x000000ff,
		    TOP_andx128v8,	0x000000ff,
		    TOP_andxx128v8,	0x000000ff,
		    TOP_andxxx128v8,	0x000000ff,
		    TOP_and128v16,	0x000000ff,
		    TOP_andx128v16,	0x000000ff,
		    TOP_andxx128v16,	0x000000ff,
		    TOP_andxxx128v16,	0x000000ff,
		    TOP_and128v32,	0x000000ff,
		    TOP_andx128v32,	0x000000ff,
		    TOP_andxx128v32,	0x000000ff,
		    TOP_andxxx128v32,	0x000000ff,
		    TOP_and128v64,	0x000000ff,
		    TOP_andx128v64,	0x000000ff,
		    TOP_andxx128v64,	0x000000ff,
		    TOP_andxxx128v64,	0x000000ff,
		    TOP_andn128v8,	0x000000ff,
		    TOP_andnx128v8,	0x000000ff,
		    TOP_andnxx128v8,	0x000000ff,
		    TOP_andnxxx128v8,	0x000000ff,
		    TOP_andn128v16,	0x000000ff,
		    TOP_andnx128v16,	0x000000ff,
		    TOP_andnxx128v16,	0x000000ff,
		    TOP_andnxxx128v16,	0x000000ff,
		    TOP_andn128v32,	0x000000ff,
		    TOP_andnx128v32,	0x000000ff,
		    TOP_andnxx128v32,	0x000000ff,
		    TOP_andnxxx128v32,	0x000000ff,
		    TOP_andn128v64,	0x000000ff,
		    TOP_andnx128v64,	0x000000ff,
		    TOP_andnxx128v64,	0x000000ff,
		    TOP_andnxxx128v64,	0x000000ff,
		    TOP_fand128v32,	0x000000ff,
		    TOP_fandx128v32,	0x000000ff,
		    TOP_fandxx128v32,	0x000000ff,
		    TOP_fandxxx128v32,	0x000000ff,
		    TOP_fand128v64,	0x000000ff,
		    TOP_fandx128v64,	0x000000ff,
		    TOP_fandxx128v64,	0x000000ff,
		    TOP_fandxxx128v64,	0x000000ff,
		    TOP_fandn128v64,	0x000000ff,
		    TOP_fandnx128v64,	0x000000ff,
		    TOP_fandnxx128v64,	0x000000ff,
		    TOP_fandnxxx128v64,	0x000000ff,
		    TOP_or128v8,	0x000000ff,
		    TOP_orx128v8,	0x000000ff,
		    TOP_orxx128v8,	0x000000ff,
		    TOP_orxxx128v8,	0x000000ff,
		    TOP_or128v16,	0x000000ff,
		    TOP_orx128v16,	0x000000ff,
		    TOP_orxx128v16,	0x000000ff,
		    TOP_orxxx128v16,	0x000000ff,
		    TOP_or128v32,	0x000000ff,
		    TOP_orx128v32,	0x000000ff,
		    TOP_orxx128v32,	0x000000ff,
		    TOP_orxxx128v32,	0x000000ff,
		    TOP_or128v64,	0x000000ff,
		    TOP_orx128v64,	0x000000ff,
		    TOP_orxx128v64,	0x000000ff,
		    TOP_orxxx128v64,	0x000000ff,
		    TOP_for128v32,	0x000000ff,
		    TOP_forx128v32,	0x000000ff,
		    TOP_forxx128v32,	0x000000ff,
		    TOP_forxxx128v32,	0x000000ff,
		    TOP_for128v64,	0x000000ff,
		    TOP_forx128v64,	0x000000ff,
		    TOP_forxx128v64,	0x000000ff,
		    TOP_forxxx128v64,	0x000000ff,
		    TOP_xor128v8,	0x000000ff,
		    TOP_xorx128v8,	0x000000ff,
		    TOP_xorxx128v8,	0x000000ff,
		    TOP_xorxxx128v8,	0x000000ff,
		    TOP_xor128v16,	0x000000ff,
		    TOP_xorx128v16,	0x000000ff,
		    TOP_xorxx128v16,	0x000000ff,
		    TOP_xorxxx128v16,	0x000000ff,
		    TOP_xor128v32,	0x000000ff,
		    TOP_xorx128v32,	0x000000ff,
		    TOP_xorxx128v32,	0x000000ff,
		    TOP_xorxxx128v32,	0x000000ff,
		    TOP_xor128v64,	0x000000ff,
		    TOP_xorx128v64,	0x000000ff,
		    TOP_xorxx128v64,	0x000000ff,
		    TOP_xorxxx128v64,	0x000000ff,
		    TOP_fxor128v32,	0x000000ff,
		    TOP_fxorx128v32,	0x000000ff,
		    TOP_fxorxx128v32,	0x000000ff,
		    TOP_fxorxxx128v32,	0x000000ff,
		    TOP_fxor128v64,	0x000000ff,			 
		    TOP_fxorx128v64,	0x000000ff,			 
		    TOP_fxorxx128v64,	0x000000ff,			 
		    TOP_fxorxxx128v64,	0x000000ff,			 
		    TOP_xorps,	        0x000000ff,
		    TOP_xorpd,	        0x000000ff,
		    TOP_orps,	        0x000000ff,
		    TOP_orpd,	        0x000000ff,
		    TOP_andps,	        0x000000ff,
		    TOP_andpd,	        0x000000ff,
		    TOP_fmax128v32,	0x000000ff,
		    TOP_fmaxx128v32,	0x000000ff,
		    TOP_fmaxxx128v32,	0x000000ff,
		    TOP_fmaxxxx128v32,	0x000000ff,
		    TOP_fmax128v64,	0x000000ff,
		    TOP_fmaxx128v64,	0x000000ff,
		    TOP_fmaxxx128v64,	0x000000ff,
		    TOP_fmaxxxx128v64,	0x000000ff,
		    TOP_fmin128v32,	0x000000ff,
		    TOP_fminx128v32,	0x000000ff,
		    TOP_fminxx128v32,	0x000000ff,
		    TOP_fminxxx128v32,	0x000000ff,
		    TOP_fmin128v64,	0x000000ff,
		    TOP_fminx128v64,	0x000000ff,
		    TOP_fminxx128v64,	0x000000ff,
		    TOP_fminxxx128v64,	0x000000ff,
		    TOP_fdiv128v32,	0x000000ff,
		    TOP_fdivx128v32,	0x000000ff,
		    TOP_fdivxx128v32,	0x000000ff,
		    TOP_fdivxxx128v32,	0x000000ff,
		    TOP_fdiv128v64,	0x000000ff,
		    TOP_fdivx128v64,	0x000000ff,
		    TOP_fdivxx128v64,	0x000000ff,
		    TOP_fdivxxx128v64,	0x000000ff,
		    TOP_fmul128v32,	0x000000ff,
		    TOP_fmulx128v32,	0x000000ff,
		    TOP_fmulxx128v32,	0x000000ff,
		    TOP_fmulxxx128v32,	0x000000ff,
		    TOP_fmul128v64,	0x000000ff,
		    TOP_fmulx128v64,	0x000000ff,
		    TOP_fmulxx128v64,	0x000000ff,
		    TOP_fmulxxx128v64,	0x000000ff,
		    TOP_frcp128v32,	0x000000ff,
		    TOP_frsqrt128v32,	0x000000ff,
		    TOP_fsqrt128v32,	0x000000ff,
		    TOP_fsqrt128v64,	0x000000ff,
		    TOP_cmpgt128v8,     0x000000ff,
		    TOP_cmpgt128v16,    0x000000ff,
		    TOP_cmpgt128v32,    0x000000ff,
		    TOP_cmpeq128v8,     0x000000ff,
		    TOP_cmpeq128v16,    0x000000ff,
		    TOP_cmpeq128v32,    0x000000ff,
		    TOP_cmpgtx128v8,     0x000000ff,
		    TOP_cmpgtx128v16,    0x000000ff,
		    TOP_cmpgtx128v32,    0x000000ff,
		    TOP_cmpeqx128v8,     0x000000ff,
		    TOP_cmpeqx128v16,    0x000000ff,
		    TOP_cmpeqx128v32,    0x000000ff,
		    TOP_cmpgtxx128v8,     0x000000ff,
		    TOP_cmpgtxx128v16,    0x000000ff,
		    TOP_cmpgtxx128v32,    0x000000ff,
		    TOP_cmpeqxx128v8,     0x000000ff,
		    TOP_cmpeqxx128v16,    0x000000ff,
		    TOP_cmpeqxx128v32,    0x000000ff,
		    TOP_cmpgtxxx128v8,     0x000000ff,
		    TOP_cmpgtxxx128v16,    0x000000ff,
		    TOP_cmpgtxxx128v32,    0x000000ff,
		    TOP_cmpeqxxx128v8,     0x000000ff,
		    TOP_cmpeqxxx128v16,    0x000000ff,
		    TOP_cmpeqxxx128v32,    0x000000ff,
		    TOP_max128v8,         0x000000ff,
		    TOP_max128v16,        0x000000ff,
		    TOP_min128v8,         0x000000ff,
		    TOP_min128v16,        0x000000ff,
		    TOP_maxx128v8,         0x000000ff,
		    TOP_maxx128v16,        0x000000ff,
		    TOP_minx128v8,         0x000000ff,
		    TOP_minx128v16,        0x000000ff,
		    TOP_maxxx128v8,         0x000000ff,
		    TOP_maxxx128v16,        0x000000ff,
		    TOP_minxx128v8,         0x000000ff,
		    TOP_minxx128v16,        0x000000ff,
		    TOP_maxxxx128v8,         0x000000ff,
		    TOP_maxxxx128v16,        0x000000ff,
		    TOP_minxxx128v8,         0x000000ff,
		    TOP_minxxx128v16,        0x000000ff,
		    TOP_and8,	0x000000ff,
		    TOP_and16,	0x000000ff,
		    TOP_andx8,	0x000000ff,
		    TOP_andx16,	0x000000ff,
		    TOP_andx32,	0x000000ff,
		    TOP_andx64,	0x000000ff,
		    TOP_andxx8,	0x000000ff,
		    TOP_andxx16,0x000000ff,
		    TOP_andxx32,0x000000ff,
		    TOP_andxx64,0x000000ff,
		    TOP_and32,	0x000000ff,
		    TOP_and64,	0x000000ff,
		    TOP_andi32,	0x000000ff,
		    TOP_andi64,	0x000000ff,
		    TOP_call,	0x000000ff,
		    TOP_icall,	0x000000ff,
		    TOP_icallx,	0x000000ff,
		    TOP_icallxx,	0x000000ff,
		    TOP_icallxxx,	0x000000ff,
		    TOP_cmp8,	0x000000ff,
		    TOP_cmpx8,	0x000000ff,
		    TOP_cmpxx8,	0x000000ff,
		    TOP_cmpxx32,	0x000000ff,
		    TOP_cmpxx64,	0x000000ff,
		    TOP_cmp16,	0x000000ff,
		    TOP_cmpx16,	0x000000ff,
		    TOP_cmpxx16,0x000000ff,
		    TOP_cmp32,	0x000000ff,
		    TOP_cmpx32,	0x000000ff,
		    TOP_cmpxx32,0x000000ff,
		    TOP_cmp64,	0x000000ff,
		    TOP_cmpx64,	0x000000ff,
		    TOP_cmpi32,	0x000000ff,
		    TOP_cmpi64,	0x000000ff,
		    TOP_cmovb,	0x000000ff,
		    TOP_cmovae,	0x000000ff,
		    TOP_cmovp,	0x000000ff,
		    TOP_cmovnp,	0x000000ff,
		    TOP_cmove,	0x000000ff,
		    TOP_cmovne,	0x000000ff,
		    TOP_cmovbe,	0x000000ff,
		    TOP_cmova,	0x000000ff,
		    TOP_cmovl,	0x000000ff,
		    TOP_cmovge,	0x000000ff,
		    TOP_cmovle,	0x000000ff,
		    TOP_cmovg,	0x000000ff,
		    TOP_cmovs,	0x000000ff,
		    TOP_cmovns,	0x000000ff,
		    TOP_div32,	0x000000ff,
		    TOP_div64,	0x000000ff,
		    TOP_idiv32,	0x000000ff,
		    TOP_idiv64,	0x000000ff,
		    TOP_imul32,	0x000000ff,
		    TOP_imul64,	0x000000ff,
		    TOP_imuli32,0x000000ff,
		    TOP_imuli64,0x000000ff,
		    TOP_imulx64,0x000000ff,
		    TOP_imulx32,0x000000ff,
		    TOP_enter,	0x000000ff,
		    TOP_jb,	0x000000ff,
		    TOP_jae,	0x000000ff,
		    TOP_jp,	0x000000ff,
		    TOP_jnp,	0x000000ff,
		    TOP_je,	0x000000ff,
		    TOP_jne,	0x000000ff,
		    TOP_jbe,	0x000000ff,
		    TOP_ja,	0x000000ff,
		    TOP_jge,	0x000000ff,
		    TOP_jl,	0x000000ff,
		    TOP_jle,	0x000000ff,
		    TOP_jg,	0x000000ff,
		    TOP_jcxz,	0x000000ff,
		    TOP_jecxz,	0x000000ff,
		    TOP_jrcxz,	0x000000ff,
		    TOP_js,	0x000000ff,
		    TOP_jns,	0x000000ff,
		    TOP_jmp,	0x000000ff,
		    TOP_ijmp,	0x000000ff,
		    TOP_ijmpx,	0x000000ff,
		    TOP_ijmpxx,	0x000000ff,
		    TOP_ijmpxxx,	0x000000ff,
		    TOP_lea32,	0x000000ff,
		    TOP_lea64,	0x000000ff,
		    TOP_leax32,	0x000000ff,
		    TOP_leax64,	0x000000ff,
		    TOP_leaxx32,0x000000ff,
		    TOP_leaxx64,0x000000ff,
		    TOP_cltd,0x000000ff,
		    TOP_cqto,0x000000ff,
		    TOP_leave,	0x000000ff,
		    TOP_ld8_32,	0x000000ff,
		    TOP_ldx8_32,	0x000000ff,
		    TOP_ldxx8_32,	0x000000ff,
		    TOP_ldu8_32,	0x000000ff,
		    TOP_ldxu8_32,	0x000000ff,
		    TOP_ldxxu8_32,	0x000000ff,
		    TOP_ld16_32,	0x000000ff,
		    TOP_ldx16_32,	0x000000ff,
		    TOP_ldxx16_32,	0x000000ff,
		    TOP_ldu16_32,	0x000000ff,
		    TOP_ldxu16_32,	0x000000ff,
		    TOP_ldxxu16_32,	0x000000ff,
		    TOP_ld8_64,	0x000000ff,
		    TOP_ldx8_64,	0x000000ff,
		    TOP_ldxx8_64,	0x000000ff,
		    TOP_ld8_64_off,	0x000000ff,
		    TOP_ldu8_64,	0x000000ff,
		    TOP_ldxu8_64,	0x000000ff,
		    TOP_ldxxu8_64,	0x000000ff,
		    TOP_ldu8_64_off,	0x000000ff,
		    TOP_ld16_64,	0x000000ff,
		    TOP_ldx16_64,	0x000000ff,
		    TOP_ldxx16_64,	0x000000ff,
		    TOP_ld16_64_off,	0x000000ff,
		    TOP_ldu16_64,	0x000000ff,
		    TOP_ldxu16_64,	0x000000ff,
		    TOP_ldxxu16_64,	0x000000ff,
		    TOP_ldu16_64_off,	0x000000ff,
		    TOP_ld32,	0x000000ff,
		    TOP_ldx32,	0x000000ff,
		    TOP_ldxx32,	0x000000ff,
		    TOP_ld32_64,	0x000000ff,
		    TOP_ldx32_64,	0x000000ff,
		    TOP_ldxx32_64,	0x000000ff,
		    TOP_ld64,	0x000000ff,
		    TOP_ldx64,	0x000000ff,
		    TOP_ldxx64,	0x000000ff,
		    TOP_ldc32,	0x000000ff,
		    TOP_ldc64,	0x000000ff,
		    TOP_movabsq,0x000000ff,
		    TOP_maxsd,	0x000000ff,
		    TOP_maxss,	0x000000ff,
		    TOP_minsd,	0x000000ff,
		    TOP_minss,	0x000000ff,
		    TOP_mov32,	0x000000ff,
		    TOP_mov64,	0x000000ff,
		    TOP_movsbl,	0x000000ff,
		    TOP_movzbl,	0x000000ff,
		    TOP_movswl,	0x000000ff,
		    TOP_movzwl,	0x000000ff,
		    TOP_movsbq,	0x000000ff,
		    TOP_movzbq,	0x000000ff,
		    TOP_movswq,	0x000000ff,
		    TOP_movzwq,	0x000000ff,
		    TOP_movslq,	0x000000ff,
		    TOP_movzlq,	0x000000ff,
		    TOP_mul32,  0x000000ff,
		    TOP_mulx64, 0x000000ff,
		    TOP_neg32,	0x000000ff,
		    TOP_neg64,	0x000000ff,
		    TOP_not32,	0x000000ff,
		    TOP_not64,	0x000000ff,
		    TOP_or8,	0x000000ff,
		    TOP_or16,	0x000000ff,
		    TOP_orx8,	0x000000ff,
		    TOP_orx16,	0x000000ff,
		    TOP_orx32,	0x000000ff,
		    TOP_orx64,	0x000000ff,
		    TOP_orxx8,	0x000000ff,
		    TOP_orxx16,	0x000000ff,
		    TOP_orxx32,	0x000000ff,
		    TOP_orxx64,	0x000000ff,
		    TOP_or32,	0x000000ff,
		    TOP_or64,	0x000000ff,
		    TOP_ori32,	0x000000ff,
		    TOP_ori64,	0x000000ff,
		    TOP_prefetch,	0x000000ff,
		    TOP_prefetchw,	0x000000ff,
		    TOP_prefetcht0,	0x000000ff,
		    TOP_prefetcht1,	0x000000ff,
		    TOP_prefetchnta,    0x000000ff,
		    TOP_prefetchx,	0x000000ff,
		    TOP_prefetchwx,	0x000000ff,
		    TOP_prefetcht0x,	0x000000ff,
		    TOP_prefetcht1x,	0x000000ff,
		    TOP_prefetchntax,   0x000000ff,
		    TOP_prefetchxx,	0x000000ff,
		    TOP_prefetchwxx,	0x000000ff,
		    TOP_prefetcht0xx,	0x000000ff,
		    TOP_prefetcht1xx,	0x000000ff,
		    TOP_prefetchntaxx,  0x000000ff,
		    TOP_popl,	0x000000ff,
		    TOP_popq,	0x000000ff,
		    TOP_pushl,	0x000000ff,
		    TOP_pushq,	0x000000ff,
		    TOP_ret,	0x000000ff,
		    TOP_reti,	0x000000ff,
		    TOP_ror8,	0x000000ff,
		    TOP_ror16,	0x000000ff,
		    TOP_ror32,	0x000000ff,
		    TOP_ror64,	0x000000ff,
		    TOP_rori8,	0x000000ff,
		    TOP_rori16,	0x000000ff,
		    TOP_rori32,	0x000000ff,
		    TOP_rori64,	0x000000ff,
		    TOP_rol8,	0x000000ff,
		    TOP_rol16,	0x000000ff,
		    TOP_rol32,	0x000000ff,
		    TOP_rol64,	0x000000ff,
		    TOP_roli8,	0x000000ff,
		    TOP_roli16,	0x000000ff,
		    TOP_roli32,	0x000000ff,
		    TOP_roli64,	0x000000ff,
		    TOP_setb,	0x000000ff,
		    TOP_setae,	0x000000ff,
		    TOP_setp,	0x000000ff,
		    TOP_setnp,	0x000000ff,
		    TOP_sete,	0x000000ff,
		    TOP_setne,	0x000000ff,
		    TOP_setbe,	0x000000ff,
		    TOP_seta,	0x000000ff,
		    TOP_setl,	0x000000ff,
		    TOP_setge,	0x000000ff,
		    TOP_setle,	0x000000ff,
		    TOP_setg,	0x000000ff,
		    TOP_store8,	0x000000ff,
		    TOP_storex8,	0x000000ff,
		    TOP_storexx8,	0x000000ff,
		    TOP_store16,	0x000000ff,
		    TOP_storex16,	0x000000ff,
		    TOP_storexx16,	0x000000ff,
		    TOP_store32,	0x000000ff,
		    TOP_store64,	0x000000ff,
		    TOP_storex32,	0x000000ff,
		    TOP_storex64,	0x000000ff,
		    TOP_storexx32,	0x000000ff,
		    TOP_storexx64,	0x000000ff,
		    TOP_storenti32,	0x000000ff,
		    TOP_storentix32,	0x000000ff,
		    TOP_storentixx32,	0x000000ff,
		    TOP_storenti64,	0x000000ff,
		    TOP_storentix64,	0x000000ff,
		    TOP_storentixx64,	0x000000ff,
		    TOP_sar32,	0x000000ff,
		    TOP_sar64,	0x000000ff,
		    TOP_sari32,	0x000000ff,
		    TOP_sari64,	0x000000ff,
		    TOP_shl32,	0x000000ff,
		    TOP_shld32,	0x000000ff,
		    TOP_shldi32,	0x000000ff,
		    TOP_shrd32,	0x000000ff,
		    TOP_shrdi32,	0x000000ff,
		    TOP_shl64,	0x000000ff,
		    TOP_shli32,	0x000000ff,
		    TOP_shli64,	0x000000ff,
		    TOP_shr32,	0x000000ff,
		    TOP_shr64,	0x000000ff,
		    TOP_shri32,	0x000000ff,
		    TOP_shri64,	0x000000ff,
		    TOP_sub32,	0x000000ff,
		    TOP_sbb32,	0x000000ff,
		    TOP_sub64,	0x000000ff,
		    TOP_subx32,	0x000000ff,
		    TOP_subx64,	0x000000ff,
		    TOP_subxx32,0x000000ff,
		    TOP_subxx64,0x000000ff,
		    TOP_subi32,	0x000000ff,
		    TOP_sbbi32,	0x000000ff,
		    TOP_subi64,	0x000000ff,
		    TOP_sub128v8,	0x000000ff,
		    TOP_subx128v8,	0x000000ff,
		    TOP_subxx128v8,	0x000000ff,
		    TOP_subxxx128v8,	0x000000ff,
		    TOP_sub128v16,	0x000000ff,
		    TOP_subx128v16,	0x000000ff,
		    TOP_subxx128v16,	0x000000ff,
		    TOP_subxxx128v16,	0x000000ff,
		    TOP_sub128v32,	0x000000ff,
		    TOP_subx128v32,	0x000000ff,
		    TOP_subxx128v32,	0x000000ff,
		    TOP_subxxx128v32,	0x000000ff,
		    TOP_sub128v64,	0x000000ff,
		    TOP_subx128v64,	0x000000ff,
		    TOP_subxx128v64,	0x000000ff,
		    TOP_subxxx128v64,	0x000000ff,
		    TOP_fsub128v32,	0x000000ff,
		    TOP_fsubx128v32,	0x000000ff,
		    TOP_fsubxx128v32,	0x000000ff,
		    TOP_fsubxxx128v32,	0x000000ff,
		    TOP_fsub128v64,	0x000000ff,
		    TOP_fsubx128v64,	0x000000ff,
		    TOP_fsubxx128v64,	0x000000ff,
		    TOP_fsubxxx128v64,	0x000000ff,
		    TOP_fhsub128v32,	0x000000ff,
		    TOP_fhsubx128v32,	0x000000ff,
		    TOP_fhsubxx128v32,	0x000000ff,
		    TOP_fhsubxxx128v32,	0x000000ff,
		    TOP_fhsub128v64,	0x000000ff,
		    TOP_fhsubx128v64,	0x000000ff,
		    TOP_fhsubxx128v64,	0x000000ff,
		    TOP_fhsubxxx128v64,	0x000000ff,
		    TOP_fmovsldup,      0x000000ff,
		    TOP_fmovsldupx,     0x000000ff,
		    TOP_fmovsldupxx,    0x000000ff,
		    TOP_fmovsldupxxx,   0x000000ff,
		    TOP_fmovshdup,      0x000000ff,
		    TOP_fmovshdupx,     0x000000ff,
		    TOP_fmovshdupxx,    0x000000ff,
		    TOP_fmovshdupxxx,   0x000000ff,
		    TOP_fmovddup,      0x000000ff,
		    TOP_fmovddupx,     0x000000ff,
		    TOP_fmovddupxx,    0x000000ff,
		    TOP_fmovddupxxx,   0x000000ff,
		    TOP_test32,	0x000000ff,
		    TOP_testx32,	0x000000ff,
		    TOP_testxx32,	0x000000ff,
		    TOP_testx64,	0x000000ff,
		    TOP_testxx64,	0x000000ff,
		    TOP_test64,	0x000000ff,
		    TOP_testi32,	0x000000ff,
		    TOP_testi64,	0x000000ff,
		    TOP_xor8,	0x000000ff,
		    TOP_xor16,	0x000000ff,
		    TOP_xorx8,	0x000000ff,
		    TOP_xorx16,	0x000000ff,
		    TOP_xorx32,	0x000000ff,
		    TOP_xorx64,	0x000000ff,
		    TOP_xorxx8,	0x000000ff,
		    TOP_xorxx16,0x000000ff,
		    TOP_xorxx32,0x000000ff,
		    TOP_xorxx64,0x000000ff,
		    TOP_xor32,	0x000000ff,
		    TOP_xor64,	0x000000ff,
		    TOP_xori32,	0x000000ff,
		    TOP_xori64,	0x000000ff,
		    TOP_addsd,	0x000000ff,
		    TOP_addss,	0x000000ff,
		    TOP_addxsd,	0x000000ff,
		    TOP_addxss,	0x000000ff,
		    TOP_addxxsd,0x000000ff,
		    TOP_addxxss,0x000000ff,
		    TOP_divsd,	0x000000ff,
		    TOP_divxsd,	0x000000ff,
		    TOP_divxxsd,	0x000000ff,
		    TOP_divss,	0x000000ff,
		    TOP_divxss,	0x000000ff,
		    TOP_divxxss,	0x000000ff,
		    TOP_mulsd,	0x000000ff,
		    TOP_mulss,	0x000000ff,
		    TOP_mulxsd,	0x000000ff,
		    TOP_mulxss,	0x000000ff,
		    TOP_mulxxsd,	0x000000ff,
		    TOP_mulxxss,	0x000000ff,
		    TOP_rcpss,	0x000000ff,
		    TOP_sqrtsd,	0x000000ff,
		    TOP_sqrtss,	0x000000ff,
		    TOP_rsqrtss,	0x000000ff,
		    TOP_cvtdq2pd,       0x000000ff,
		    TOP_cvtdq2ps,       0x000000ff,
		    TOP_cvtps2pd,       0x000000ff,
		    TOP_cvtpd2ps,       0x000000ff,
		    TOP_cvtss2sd,	0x000000ff,
		    TOP_cvtsd2ss,	0x000000ff,
		    TOP_cvtss2si,	0x000000ff,
		    TOP_cvtsd2si,	0x000000ff,
		    TOP_cvtss2siq,	0x000000ff,
		    TOP_cvtsd2siq,	0x000000ff,
		    TOP_cvttss2si,	0x000000ff,
		    TOP_cvttsd2si,	0x000000ff,
		    TOP_cvttss2siq,	0x000000ff,
		    TOP_cvttsd2siq,	0x000000ff,
		    TOP_cvtsi2sd,	0x000000ff,
		    TOP_cvtsi2ss,	0x000000ff,
		    TOP_cvtsi2sdq,	0x000000ff,
		    TOP_cvtsi2ssq,	0x000000ff,
		    TOP_cvtps2dq,      0x000000ff,
		    TOP_cvttps2dq,      0x000000ff,
		    TOP_cvtpd2dq,      0x000000ff,
		    TOP_cvttpd2dq,      0x000000ff,
		    TOP_cvtdq2pd_x,     0x000000ff,
		    TOP_cvtdq2ps_x,     0x000000ff,
		    TOP_cvtps2pd_x,     0x000000ff,
		    TOP_cvtpd2ps_x,     0x000000ff,
		    TOP_cvttpd2dq_x,    0x000000ff,
		    TOP_cvtps2dq_x,    0x000000ff,			 
		    TOP_cvttps2dq_x,    0x000000ff,			 
		    TOP_cvtdq2pd_xx,     0x000000ff,
		    TOP_cvtdq2ps_xx,     0x000000ff,
		    TOP_cvtps2pd_xx,     0x000000ff,
		    TOP_cvtpd2ps_xx,     0x000000ff,
		    TOP_cvttpd2dq_xx,    0x000000ff,
		    TOP_cvtps2dq_xx,    0x000000ff,			 
		    TOP_cvttps2dq_xx,    0x000000ff,			 
		    TOP_cvtdq2pd_xxx,     0x000000ff,
		    TOP_cvtdq2ps_xxx,     0x000000ff,
		    TOP_cvtps2pd_xxx,     0x000000ff,
		    TOP_cvtpd2ps_xxx,     0x000000ff,
		    TOP_cvttpd2dq_xxx,    0x000000ff,
		    TOP_cvtps2dq_xxx,    0x000000ff,			 
		    TOP_cvttps2dq_xxx,    0x000000ff,			 
		    TOP_cvtpi2ps,    0x000000ff,			 
		    TOP_cvtps2pi,    0x000000ff,			 
		    TOP_cvttps2pi,    0x000000ff,			 
		    TOP_cvtpi2pd,    0x000000ff,			 
		    TOP_cvtpd2pi,    0x000000ff,			 
		    TOP_cvttpd2pi,    0x000000ff,			 
		    TOP_lddqa,	0x000000ff,
		    TOP_stdqa,	0x000000ff,
		    TOP_ldapd,  0x000000ff,
		    TOP_ldaps,  0x000000ff,
		    TOP_ldupd,  0x000000ff,
		    TOP_ldups,  0x000000ff,
		    TOP_stapd,  0x000000ff,
		    TOP_staps,  0x000000ff,
		    TOP_stupd,  0x000000ff,
		    TOP_stups,  0x000000ff,
		    TOP_stntpd, 0x000000ff,
		    TOP_stntps, 0x000000ff,
		    TOP_storenti128, 0x000000ff,
		    TOP_storent64_fm, 0x000000ff,
		    TOP_lddqu,	0x000000ff,
		    TOP_stdqu,	0x000000ff,
		    TOP_ldlpd,	0x000000ff,
		    TOP_stlpd,	0x000000ff,
		    TOP_storelpd,	0x000000ff,
		    TOP_ldlps,	0x000000ff,
		    TOP_stlps,	0x000000ff,
		    TOP_ldhpd,	0x000000ff,
		    TOP_sthpd,	0x000000ff,
		    TOP_ldhps,	0x000000ff,
		    TOP_sthps,	0x000000ff,
		    TOP_lddqax,	0x000000ff,
		    TOP_stdqax,	0x000000ff,
		    TOP_ldapdx,  0x000000ff,
		    TOP_ldapsx,  0x000000ff,
		    TOP_stapdx,  0x000000ff,
		    TOP_stapsx,  0x000000ff,
		    TOP_stntpdx,0x000000ff,
		    TOP_stntpsx,0x000000ff,
		    TOP_lddqux,	0x000000ff,
		    TOP_stdqux,	0x000000ff,
		    TOP_ldlpdx,	0x000000ff,
		    TOP_stlpdx,	0x000000ff,
		    TOP_ldlpsx,	0x000000ff,
		    TOP_stlpsx,	0x000000ff,
		    TOP_ldhpdx,	0x000000ff,
		    TOP_sthpdx,	0x000000ff,
		    TOP_ldhpsx,	0x000000ff,
		    TOP_sthpsx,	0x000000ff,
		    TOP_lddqaxx,	0x000000ff,
		    TOP_stdqaxx,	0x000000ff,
		    TOP_ldapdxx,  0x000000ff,
		    TOP_ldapsxx,  0x000000ff,
		    TOP_stapdxx,  0x000000ff,
		    TOP_stapsxx,  0x000000ff,
		    TOP_stntpdxx,       0x000000ff,
		    TOP_stntpsxx,       0x000000ff,
		    TOP_lddquxx,	0x000000ff,
		    TOP_stdquxx,	0x000000ff,
		    TOP_ldlpdxx,	0x000000ff,
		    TOP_stlpdxx,	0x000000ff,
		    TOP_ldlpsxx,	0x000000ff,
		    TOP_stlpsxx,	0x000000ff,
		    TOP_ldhpdxx,	0x000000ff,
		    TOP_sthpdxx,	0x000000ff,
		    TOP_ldhpsxx,	0x000000ff,
		    TOP_sthpsxx,	0x000000ff,
		    TOP_ldsd_n32,	0x000000ff,
		    TOP_ldss_n32,	0x000000ff,
		    TOP_ldsd,	0x000000ff,
		    TOP_ldss,	0x000000ff,
		    TOP_ldsdx,	0x000000ff,
		    TOP_ldsdxx,	0x000000ff,
		    TOP_ldssx,	0x000000ff,
		    TOP_ldssxx,	0x000000ff,
		    TOP_movg2x64,	0x000000ff,
		    TOP_movg2x,	0x000000ff,
		    TOP_movx2g64,	0x000000ff,
		    TOP_movx2g,	0x000000ff,
		    TOP_movsd,	0x000000ff,
		    TOP_movss,	0x000000ff,
		    TOP_movdq,  0x000000ff,
		    TOP_movapd, 0x000000ff,
		    TOP_movaps, 0x000000ff,
		    TOP_movq2dq, 0x000000ff,
		    TOP_movdq2q, 0x000000ff,
		    TOP_subsd,	0x000000ff,
		    TOP_subss,	0x000000ff,
		    TOP_subxsd,	0x000000ff,
		    TOP_subxss,	0x000000ff,
		    TOP_subxxsd,0x000000ff,
		    TOP_subxxss,0x000000ff,
		    TOP_stss_n32,	0x000000ff,
		    TOP_stsd_n32,	0x000000ff,
		    TOP_stss,	0x000000ff,
		    TOP_stsd,	0x000000ff,
		    TOP_stntss, 0x000000ff,
		    TOP_stntsd, 0x000000ff,
		    TOP_stssx,	0x000000ff,
		    TOP_stssxx,	0x000000ff,
		    TOP_stsdx,	0x000000ff,
		    TOP_stsdxx,	0x000000ff,
		    TOP_stntssx,  0x000000ff,
		    TOP_stntssxx, 0x000000ff,
		    TOP_stntsdx,  0x000000ff,
		    TOP_stntsdxx, 0x000000ff,
		    TOP_comisd,	0x000000ff,
		    TOP_comixsd,	0x000000ff,
		    TOP_comixxsd,	0x000000ff,
		    TOP_comixxxsd,	0x000000ff,
		    TOP_comiss,	        0x000000ff,
		    TOP_comixss,	0x000000ff,
		    TOP_comixxss,	0x000000ff,
		    TOP_comixxxss,	0x000000ff,
		    TOP_unpckhpd,       0x000000ff,
		    TOP_unpckhps,       0x000000ff,
		    TOP_unpcklpd,       0x000000ff,
		    TOP_unpcklps,       0x000000ff,
		    TOP_punpcklwd,	0x000000ff,
		    TOP_punpcklbw,	0x000000ff,
		    TOP_punpckldq,	0x000000ff,
		    TOP_pshufd,		0x000000ff,
		    TOP_pshufw,		0x000000ff,
		    TOP_pshuflw,	0x000000ff,
		    TOP_pshufhw,	0x000000ff,
		    TOP_shufpd,         0x000000ff,
		    TOP_shufps,         0x000000ff,
		    TOP_movhlps,        0x000000ff,
		    TOP_movlhps,        0x000000ff,
		    TOP_psrldq,         0x000000ff,
		    TOP_psrlq128v64,    0x000000ff,
		    TOP_pslldq,         0x000000ff,
		    TOP_psllw,          0x000000ff,
		    TOP_pslld,          0x000000ff,
		    TOP_psllq,          0x000000ff,
		    TOP_psrlw,          0x000000ff,
		    TOP_psrld,          0x000000ff,
		    TOP_psrlq,          0x000000ff,
		    TOP_psraw,          0x000000ff,
		    TOP_psrad,          0x000000ff,
  		    TOP_psllw_mmx,      0x000000ff,
  		    TOP_pslld_mmx,      0x000000ff,
  		    TOP_psrlw_mmx,      0x000000ff,
  		    TOP_psrld_mmx,      0x000000ff,
  		    TOP_psraw_mmx,      0x000000ff,
  		    TOP_psrad_mmx,      0x000000ff,
  		    TOP_pand_mmx,       0x000000ff,
  		    TOP_pandn_mmx,      0x000000ff,
            TOP_pand128,        0x000000ff,
            TOP_pandn128,       0x000000ff,
  		    TOP_por_mmx,        0x000000ff,
  		    TOP_pxor_mmx,       0x000000ff,
		    TOP_mfence,         0x000000ff,
		    TOP_lfence,         0x000000ff,
		    TOP_sfence,         0x000000ff,
		    TOP_ld32_64_off,    0x000000ff,
		    TOP_ld64_off,       0x000000ff,
		    TOP_store64_off,    0x000000ff,
		    TOP_ld8_32_n32,     0x000000ff,
		    TOP_ldu8_32_n32,    0x000000ff,
		    TOP_ld16_32_n32,    0x000000ff,
		    TOP_ldu16_32_n32,   0x000000ff,
		    TOP_ld32_n32,       0x000000ff,
		    TOP_store8_n32,     0x000000ff,
		    TOP_store16_n32,    0x000000ff,
		    TOP_store32_n32,    0x000000ff,
		    TOP_fadd,     0x000000ff,
		    TOP_faddp,    0x000000ff,
		    TOP_fsub,     0x000000ff,
		    TOP_fsubp,    0x000000ff,
		    TOP_fsubr,    0x000000ff,
		    TOP_fsubrp,   0x000000ff,
		    TOP_fld,      0x000000ff,
		    TOP_flds,     0x000000ff,
		    TOP_flds_n32, 0x000000ff,
		    TOP_fldl,     0x000000ff,
		    TOP_fldl_n32, 0x000000ff,
		    TOP_fldt,     0x000000ff,
		    TOP_fldt_n32, 0x000000ff,
		    TOP_fstp,     0x000000ff,
		    TOP_fstps,    0x000000ff,
		    TOP_fstps_n32,0x000000ff,
		    TOP_fstpl,    0x000000ff,
		    TOP_fstpl_n32,0x000000ff,
		    TOP_fst,    0x000000ff,
		    TOP_fsts,    0x000000ff,
		    TOP_fsts_n32,0x000000ff,
		    TOP_fstl,    0x000000ff,
		    TOP_fstl_n32,0x000000ff,
		    TOP_fstpt,    0x000000ff,
		    TOP_fstpt_n32,0x000000ff,
		    TOP_fxch,     0x000000ff,
		    TOP_fmov,     0x000000ff,
		    TOP_fmul,     0x000000ff,
		    TOP_fmulp,    0x000000ff,
		    TOP_fdiv,     0x000000ff,
		    TOP_fdivp,    0x000000ff,
		    TOP_fdivr,    0x000000ff,
		    TOP_fdivrp,   0x000000ff,
		    TOP_fucomi,   0x000000ff,
		    TOP_fucomip,  0x000000ff,
		    TOP_fchs,     0x000000ff,
		    TOP_frndint,  0x000000ff,
		    TOP_fnstcw,   0x000000ff,
		    TOP_fldcw,    0x000000ff,
		    TOP_fistps,   0x000000ff,
		    TOP_fistpl,   0x000000ff,
		    TOP_fists,    0x000000ff,
		    TOP_fistl,    0x000000ff,
		    TOP_fistpll,  0x000000ff,
		    TOP_fisttps,  0x000000ff,
		    TOP_fisttpl,  0x000000ff,
		    TOP_fisttpll, 0x000000ff,
		    TOP_filds,    0x000000ff,
		    TOP_fildl,    0x000000ff,
		    TOP_fildll,   0x000000ff,
		    TOP_fabs,     0x000000ff,
		    TOP_fsqrt,    0x000000ff,
		    TOP_fcmovb,   0x000000ff,
		    TOP_fcmovbe,  0x000000ff,
		    TOP_fcmovnb,  0x000000ff,
		    TOP_fcmovnbe, 0x000000ff,
		    TOP_fcmove,   0x000000ff,
		    TOP_fcmovne,  0x000000ff,
		    TOP_fcmovu,   0x000000ff,
		    TOP_fcmovnu,  0x000000ff,
		    TOP_fldz,     0x000000ff,
		    TOP_fcos,     0x000000ff,
		    TOP_fsin,     0x000000ff,
		    TOP_subus128v16,	0x000000ff,
		    TOP_ldaps_n32,	0x000000ff,
		    TOP_ldups_n32,	0x000000ff,
		    TOP_ldapd_n32,	0x000000ff,
		    TOP_ldupd_n32,	0x000000ff,
		    TOP_lddqa_n32,	0x000000ff,
		    TOP_lddqu_n32,	0x000000ff,
		    TOP_ldlps_n32,	0x000000ff,
		    TOP_ldhps_n32,	0x000000ff,
		    TOP_ldlpd_n32,	0x000000ff,
		    TOP_ldhpd_n32,	0x000000ff,
		    TOP_staps_n32,	0x000000ff,
		    TOP_stapd_n32,	0x000000ff,
		    TOP_stdqa_n32,	0x000000ff,
		    TOP_stdqu_n32,	0x000000ff,
		    TOP_stlps_n32,	0x000000ff,
		    TOP_sthps_n32,	0x000000ff,
		    TOP_stlpd_n32,	0x000000ff,
		    TOP_sthpd_n32,	0x000000ff,
		    TOP_store8_abs,	0x000000ff,
		    TOP_store16_abs,	0x000000ff,
		    TOP_store32_abs,	0x000000ff,
		    TOP_store64_abs,	0x000000ff,
		    TOP_ld8_abs,	0x000000ff,
		    TOP_ld16_abs,	0x000000ff,
		    TOP_ld32_abs,	0x000000ff,
		    TOP_ld64_abs,	0x000000ff,
		    TOP_ld32_gs_seg_off,	0x000000ff,
		    TOP_ld64_fs_seg_off,	0x000000ff,
		    TOP_lock_add8,      0x000000ff,
		    TOP_lock_add16,     0x000000ff,
		    TOP_lock_add32,     0x000000ff,
		    TOP_lock_adc32,     0x000000ff,
		    TOP_lock_add64,     0x000000ff,
		    TOP_lock_xadd8,     0x000000ff,
		    TOP_lock_xadd16,    0x000000ff,
		    TOP_lock_xadd32,    0x000000ff,
		    TOP_lock_xadd64,    0x000000ff,
		    TOP_cvtsd2ss_x,	0x000000ff,
		    TOP_cvtsd2ss_xx,	0x000000ff,
		    TOP_cvtsd2ss_xxx,	0x000000ff,
		    TOP_cvtsi2sd_x,	0x000000ff,
		    TOP_cvtsi2sd_xx,	0x000000ff,
		    TOP_cvtsi2sd_xxx,	0x000000ff,
		    TOP_cvtsi2ss_x,	0x000000ff,
		    TOP_cvtsi2ss_xx,	0x000000ff,
		    TOP_cvtsi2ss_xxx,	0x000000ff,
		    TOP_cvtsi2sdq_x,	0x000000ff,
		    TOP_cvtsi2sdq_xx,	0x000000ff,
		    TOP_cvtsi2sdq_xxx,	0x000000ff,
		    TOP_cvtsi2ssq_x,	0x000000ff,
		    TOP_cvtsi2ssq_xx,	0x000000ff,
		    TOP_cvtsi2ssq_xxx,	0x000000ff,
		    TOP_lock_cmpxchg8,  0x000000ff,
		    TOP_lock_cmpxchg16, 0x000000ff,
		    TOP_lock_cmpxchg32, 0x000000ff,
		    TOP_lock_cmpxchg64, 0x000000ff,
		    TOP_lock_and8,  0x000000ff,
		    TOP_lock_and16, 0x000000ff,
		    TOP_lock_and32, 0x000000ff,
		    TOP_lock_and64, 0x000000ff,
		    TOP_lock_or8,  0x000000ff,
		    TOP_lock_or16, 0x000000ff,
		    TOP_lock_or32, 0x000000ff,
		    TOP_lock_or64, 0x000000ff,
		    TOP_lock_xor8,  0x000000ff,
		    TOP_lock_xor16, 0x000000ff,
		    TOP_lock_xor32, 0x000000ff,
		    TOP_lock_xor64, 0x000000ff,
		    TOP_lock_sub8,  0x000000ff,
		    TOP_lock_sub16, 0x000000ff,
		    TOP_lock_sub32, 0x000000ff,
		    TOP_lock_sub64, 0x000000ff,
		    TOP_bsf32, 0x000000ff,
		    TOP_bsf64, 0x000000ff,
		    TOP_bsr32, 0x000000ff,
		    TOP_bsr64, 0x000000ff,
		    TOP_ld64_2m, 0x000000ff,
		    TOP_ld64_2m_n32, 0x000000ff,
		    TOP_store64_fm, 0x000000ff,
		    TOP_store64_fm_n32, 0x000000ff,
		    TOP_ld64_2sse, 0x000000ff,
		    TOP_ld64_2sse_n32, 0x000000ff,
		    TOP_store64_fsse, 0x000000ff,
		    TOP_store64_fsse_n32, 0x000000ff,
		    TOP_mov64_m, 0x000000ff,
		    TOP_add64v8, 0x000000ff,
		    TOP_add64v16, 0x000000ff,
		    TOP_add64v32, 0x000000ff,
		    TOP_sub64v8, 0x000000ff,
		    TOP_sub64v16, 0x000000ff,
		    TOP_sub64v32, 0x000000ff,
		    TOP_paddsb, 0x000000ff,
		    TOP_paddsw, 0x000000ff,
		    TOP_paddq, 0x000000ff,
		    TOP_psubsb, 0x000000ff,
		    TOP_psubsw, 0x000000ff,
		    TOP_paddusb, 0x000000ff,
		    TOP_paddusw, 0x000000ff,
		    TOP_psubusb, 0x000000ff,
		    TOP_psubusw, 0x000000ff,
		    TOP_pmullw, 0x000000ff,
		    TOP_pmulhw, 0x000000ff,
		    TOP_pcmpeqb, 0x000000ff,
		    TOP_pcmpeqw, 0x000000ff,
		    TOP_pcmpeqd, 0x000000ff,
		    TOP_pcmpgtb, 0x000000ff,
		    TOP_pcmpgtw, 0x000000ff,
		    TOP_pcmpgtd, 0x000000ff,
		    TOP_punpckhbw, 0x000000ff,
		    TOP_punpckhwd, 0x000000ff,
		    TOP_punpckhdq, 0x000000ff,
		    TOP_punpckl64v8, 0x000000ff,
		    TOP_punpckl64v16, 0x000000ff,
		    TOP_punpckl64v32, 0x000000ff,
		    TOP_packsswb, 0x000000ff,
		    TOP_packssdw, 0x000000ff,
		    TOP_packuswb, 0x000000ff,
		    TOP_packsswb128, 0x000000ff,
				TOP_packssdw128, 0x000000ff,
				TOP_packuswb128, 0x000000ff,
				TOP_punpckhbw128, 0x000000ff,
				TOP_punpckhwd128,0x000000ff,
				TOP_punpckhdq128, 0x000000ff,
				TOP_punpckhqdq, 0x000000ff,
				TOP_punpcklbw128, 0x000000ff,
				TOP_punpcklwd128, 0x000000ff,
				TOP_punpckldq128, 0x000000ff,
				TOP_punpcklqdq, 0x000000ff,
		    TOP_pmulhuw, 0x000000ff,
		    TOP_pmaddwd, 0x000000ff,
		    TOP_pavgb, 0x000000ff,
		    TOP_pavgw, 0x000000ff,
		    TOP_psadbw, 0x000000ff,
		    TOP_max64v8, 0x000000ff,
		    TOP_max64v16, 0x000000ff,
		    TOP_min64v8, 0x000000ff,
		    TOP_min64v16, 0x000000ff,
		    TOP_pextrw64, 0x000000ff,
		    TOP_pextrw128, 0x000000ff,
		    TOP_pinsrw64, 0x000000ff,
		    TOP_pinsrw128, 0x000000ff,
		    TOP_pmovmskb, 0x000000ff,
		    TOP_pmovmskb128, 0x000000ff,
		    TOP_cmpeqps, 0x000000ff,
		    TOP_cmpeqpd, 0x000000ff,
		    TOP_cmpltpd, 0x000000ff,
		    TOP_cmplepd, 0x000000ff,
		    TOP_cmpgtpd, 0x000000ff,
		    TOP_cmpgepd, 0x000000ff,
		    TOP_cmpneqpd, 0x000000ff,
		    TOP_cmpnltpd, 0x000000ff,
		    TOP_cmpnlepd, 0x000000ff,
		    TOP_cmpngtpd, 0x000000ff,
		    TOP_cmpngepd, 0x000000ff,
		    TOP_cmpordpd, 0x000000ff,
		    TOP_cmpunordpd, 0x000000ff,
		    TOP_cmpltps, 0x000000ff,
		    TOP_cmpleps, 0x000000ff,
		    TOP_cmpunordps, 0x000000ff,
		    TOP_cmpnltps, 0x000000ff,
		    TOP_cmpnleps, 0x000000ff,
		    TOP_cmpordps, 0x000000ff,
		    TOP_cmpeqss, 0x000000ff,
		    TOP_cmpltss, 0x000000ff,
		    TOP_cmpless, 0x000000ff,
		    TOP_cmpunordss, 0x000000ff,
		    TOP_cmpneqss, 0x000000ff,
		    TOP_cmpnltss, 0x000000ff,
		    TOP_cmpnless, 0x000000ff,
		    TOP_cmpordss, 0x000000ff,
		    TOP_cmpeqsd, 0x000000ff,
		    TOP_cmpltsd, 0x000000ff,
		    TOP_cmplesd, 0x000000ff,
		    TOP_cmpunordsd, 0x000000ff,
		    TOP_cmpneqsd, 0x000000ff,
		    TOP_cmpnltsd, 0x000000ff,
		    TOP_cmpnlesd, 0x000000ff,
		    TOP_cmpordsd, 0x000000ff,
		    TOP_emms, 0x000000ff,
		    TOP_stmxcsr, 0x000000ff,
		    TOP_ldmxcsr, 0x000000ff,
		    TOP_movi32_2m, 0x000000ff,
		    TOP_movi64_2m, 0x000000ff,
		    TOP_movm_2i32, 0x000000ff,
		    TOP_movm_2i64, 0x000000ff,
		    TOP_pshufw64v16, 0x000000ff,
		    TOP_movmskps, 0x000000ff,
		    TOP_movmskpd, 0x000000ff,
		    TOP_maskmovdqu, 0x000000ff,
		    TOP_maskmovq, 0x000000ff,
            TOP_extrq,   0x000000ff,
            TOP_insertq,   0x000000ff,
            TOP_pcmpistri, 0x000000ff,
            TOP_pcmpistrm, 0x000000ff,        
            TOP_pcmpestri, 0x000000ff,
            TOP_pcmpestrm, 0x000000ff,
            TOP_xchgx8, 0x000000ff,
            TOP_xchgx16, 0x000000ff,
            TOP_xchgx32, 0x000000ff,
            TOP_xchgx64, 0x000000ff,
		    TOP_UNDEFINED);

  ISA_Pack_End();
}
