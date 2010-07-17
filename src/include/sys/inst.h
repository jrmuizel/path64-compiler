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


#ifndef __SYS_INST_H__
#define __SYS_INST_H__
#ident "$Revision$"

/*
 * inst.h -- instruction format defines
 */


/* for backward compatibility */
#define bb_ma_format ma_format

#ifndef _MIPSEL
union mips_instruction {
	unsigned word;
	unsigned char byte[4];
	struct {
		unsigned opcode : 6;
		unsigned target : 26;
	} j_format;
	struct {
		unsigned opcode : 6;
		unsigned rs : 5;
		unsigned rt : 5;
		signed simmediate : 16;
	} i_format;
	struct {
		unsigned opcode : 6;
		unsigned rs : 5;
		unsigned rt : 5;
		unsigned uimmediate : 16;
	} u_format;
	struct {			/* new R4K cacheop format */
		unsigned opcode : 6;
		unsigned rs : 5;
		unsigned c_op : 3;
		unsigned cache : 2;
		unsigned simmediate : 16;
	} c_format;
	struct {
		unsigned opcode : 6;
		unsigned rs : 5;
		unsigned rt : 5;
		unsigned rd : 5;
		unsigned re : 5;
		unsigned func : 6;
	} r_format;
	struct {
		unsigned opcode : 6;
		unsigned : 1;
		unsigned fmt : 4;
		unsigned rt : 5;
		unsigned rd : 5;
		unsigned re : 5;
		unsigned func : 6;
	} f_format;
        struct {
                unsigned opcode : 6;
                unsigned fr : 5;
                unsigned ft : 5;
                unsigned fs : 5;
                unsigned fd : 5;
                unsigned func : 3;
                unsigned fmt : 3;
        } ma_format;
	struct {
		unsigned opcode : 6;
		unsigned rs : 5;
                unsigned ft : 5;
                unsigned fs : 5;
                unsigned fd : 5;
		unsigned func : 6;
	} ps_format;
	struct {
		unsigned opcode : 6;
		unsigned sel : 4;
		unsigned fmt : 1;
		unsigned vt : 5;
		unsigned vs : 5;
		unsigned vd : 5;
		unsigned func : 6;
	} v_format;
		
};
#endif

#ifdef _MIPSEL
union mips_instruction {
	unsigned word;
	unsigned char byte[4];
	struct {
		unsigned target : 26;
		unsigned opcode : 6;
	} j_format;
	struct {
		signed simmediate : 16;
		unsigned rt : 5;
		unsigned rs : 5;
		unsigned opcode : 6;
	} i_format;
	struct {
		unsigned uimmediate : 16;
		unsigned rt : 5;
		unsigned rs : 5;
		unsigned opcode : 6;
	} u_format;
	struct {
		unsigned simmediate : 16;
		unsigned cache : 2;
		unsigned c_op : 3;
		unsigned rs : 5;
		unsigned opcode : 6;
	} c_format;
	struct {
		unsigned func : 6;
		unsigned re : 5;
		unsigned rd : 5;
		unsigned rt : 5;
		unsigned rs : 5;
		unsigned opcode : 6;
	} r_format;
	struct {
		unsigned func : 6;
		unsigned re : 5;
		unsigned rd : 5;
		unsigned rt : 5;
		unsigned fmt : 4;
		unsigned : 1;
		unsigned opcode : 6;
	} f_format;
        struct {
                unsigned fmt : 3;
                unsigned func : 3;
                unsigned fd : 5;
                unsigned fs : 5;
                unsigned ft : 5;
                unsigned fr : 5;
                unsigned opcode : 6;
        } ma_format;
	struct {
		unsigned func : 6;
		unsigned fd : 5;
		unsigned fs : 5;
		unsigned ft : 5;
		unsigned rs : 5;
		unsigned opcode : 6;
	} ps_format;
	struct {
		unsigned func : 6;
		unsigned vd : 5;
		unsigned vs : 5;
		unsigned vt : 5;
		unsigned fmt : 1;
		unsigned sel : 4;
		unsigned opcode : 6;
	} v_format;
};
#endif

/* major opcodes */
#define spec_op		0x00
#define bcond_op	0x01
#define j_op		0x02
#define jal_op		0x03

#define beq_op		0x04
#define bne_op		0x05
#define blez_op		0x06
#define bgtz_op		0x07

#define addi_op		0x08
#define addiu_op	0x09
#define slti_op		0x0A
#define sltiu_op	0x0B

#define andi_op		0x0C
#define ori_op		0x0D
#define xori_op		0x0E
#define lui_op		0x0F

#define cop0_op		0x10
#define cop1_op		0x11
#define cop2_op		0x12
#define cop1x_op        0x13

#define beql_op		0x14
#define bnel_op		0x15
#define blezl_op	0x16
#define bgtzl_op	0x17

#define daddi_op        0x18
#define daddiu_op       0x19
#define ldl_op          0x1A
#define ldr_op          0x1B

#define lb_op		0x20
#define lh_op		0x21
#define lwl_op		0x22
#define lw_op		0x23

#define lbu_op		0x24
#define lhu_op		0x25
#define lwr_op		0x26
#define lwu_op		0x27

#define sb_op		0x28
#define sh_op		0x29
#define swl_op		0x2A
#define sw_op		0x2B

#define sdl_op		0x2C
#define sdr_op		0x2D
#define swr_op		0x2E
#define cache_op	0x2F

#define ll_op		0x30
#define lwc1_op		0x31
#define lwc2_op		0x32
#define pref_op		0x33

#define lld_op		0x34
#define ldc1_op		0x35
#define ldc2_op		0x36
#define	ld_op		0x37

#define sc_op		0x38
#define swc1_op		0x39
#define swc2_op		0x3A

#define scd_op		0x3C
#define sdc1_op		0x3D
#define sdc2_op		0x3E
#define	sd_op		0x3F

/* func field of spec opcode */
#define sll_op		0x00
#define movc_op         0x01
#define srl_op		0x02
#define sra_op		0x03

#define sllv_op		0x04
#define srlv_op		0x06
#define srav_op		0x07

#define jr_op		0x08
#define jalr_op		0x09
#define movz_op         0x0A
#define movn_op         0x0B

#define syscall_op	0x0C
#define break_op	0x0D
#define spim_op		0x0E
#define sync_op		0x0F

#define mfhi_op		0x10
#define mthi_op		0x11
#define mflo_op		0x12
#define mtlo_op		0x13

#define dsllv_op	0x14
#define dsrlv_op	0x16
#define dsrav_op	0x17

#define mult_op		0x18
#define multu_op	0x19
#define div_op		0x1A
#define divu_op		0x1B

#define dmult_op	0x1C
#define dmultu_op	0x1D
#define ddiv_op		0x1E
#define ddivu_op	0x1F

#define add_op		0x20
#define addu_op		0x21
#define sub_op		0x22
#define subu_op		0x23

#define and_op		0x24
#define or_op		0x25
#define xor_op		0x26
#define nor_op		0x27

#define slt_op		0x2A
#define sltu_op		0x2B

#define dadd_op		0x2C
#define daddu_op	0x2D
#define dsub_op		0x2E
#define dsubu_op	0x2F

#define tge_op		0x30
#define tgeu_op		0x31
#define tlt_op		0x32
#define tltu_op		0x33

#define teq_op		0x34
#define tne_op		0x36

#define dsll_op         0x38
#define dsrl_op         0x3A
#define dsra_op         0x3B

#define dsll32_op       0x3C
#define dsrl32_op       0x3E
#define dsra32_op       0x3F

/* rt field of bcond opcodes */
#define bltz_op		0x00
#define bgez_op		0x01
#define bltzl_op	0x02
#define bgezl_op	0x03

#define spimi_op	0x04

#define tgei_op		0x08
#define tgeiu_op	0x09
#define tlti_op		0x0A
#define tltiu_op	0x0B

#define teqi_op		0x0C
#define tnei_op		0x0E

#define bltzal_op	0x10
#define bgezal_op	0x11
#define bltzall_op	0x12
#define bgezall_op	0x13

/* rs field of cop opcodes */
#define mfc_op		0x00
#define dmfc_op		0x01
#define cfc_op		0x02
#define mtc_op		0x04
#define dmtc_op		0x05
#define ctc_op		0x06
#define bc_op		0x08
#define cop_op		0x10
#define copm_op         0x18

/* func field of cop0 COI opcodes */
#define tlbr_op		0x01
#define tlbwi_op	0x02
#define tlbwr_op	0x06
#define tlbp_op		0x08
#define rfe_op		0x10
#define eret_op		0x18

/* func field of cop0 COM opcodes */
#define tlbr1_op	0x01
#define tlbw_op         0x02
#define tlbp1_op	0x08
#define dctr_op         0x09
#define dctw_op         0x0A

/* fmt field of cop1 opcodes */
#define	s_fmt		0
#define	d_fmt		1
#define	e_fmt		2
#define q_fmt		3
#define	w_fmt		4
#define l_fmt		5
#define ps_fmt		6

/* func field of cop1/{s,d,w} opcodes */
#define fadd_op		0x00
#define fsub_op		0x01
#define fmul_op		0x02
#define fdiv_op		0x03

#define fsqrt_op	0x04
#define fabs_op		0x05
#define fmov_op		0x06
#define fneg_op		0x07

#define froundl_op	0x08
#define ftruncl_op	0x09
#define fceill_op	0x0a
#define ffloorl_op	0x0b

#define fround_op	0x0c
#define ftrunc_op	0x0d
#define fceil_op	0x0e
#define ffloor_op	0x0f

#define fmovc_op        0x11
#define fmovz_op        0x12
#define fmovn_op        0x13

#define frecip_op       0x15
#define frsqrt_op       0x16

#define fcvts_op	0x20
#define fcvtspl_op	0x20
#define fcvtd_op	0x21
#define fcvte_op	0x22

#define fcvtw_op	0x24
#define fcvtl_op	0x25
#define fcvtps_op	0x26
#define fcvtspu_op	0x28
#define fpll_op		0x2c
#define fplu_op		0x2d
#define fpul_op		0x2e
#define fpuu_op		0x2f

#define fcmp_op		0x30

/* func field of cop1x opcodes */

#define lwxc1_op        0x00
#define ldxc1_op        0x01
#define luxc1_op        0x05
#define swxc1_op        0x08
#define sdxc1_op        0x09
#define suxc1_op        0x0d
#define pfetch_op       0x0f
#define alnv_ps_op	0x1c
#define madd_s_op       0x20
#define madd_d_op       0x21
#define madd_e_op       0x22
#define madd_ps_op	0x26
#define msub_s_op       0x28
#define msub_d_op       0x29
#define msub_e_op       0x2a
#define msub_ps_op      0x2e
#define nmadd_s_op      0x30
#define nmadd_d_op      0x31
#define nmadd_e_op      0x32
#define nmadd_ps_op	0x36
#define nmsub_s_op      0x38
#define nmsub_d_op      0x39
#define nmsub_e_op      0x3a
#define nmsub_ps_op	0x3e

/* func field of cop2 opcodes */
#define vmsgn_op	0x00
#define vcmp_eq_op 	0x01
#define vpickf_op	0x02
#define vpickt_op	0x03
#define vcmp_lt_op 	0x04
#define vcmp_le_op 	0x05
#define vmin_op		0x06
#define vmax_op		0x07
#define vsub_op		0x0a
#define vadd_op		0x0b
#define vand_op		0x0c
#define vxor_op		0x0d
#define vor_op		0x0e
#define vnor_op		0x0f
#define vsll_op		0x10
#define vsrl_op		0x12
#define vsra_op		0x13
#define valni_ob_op	0x18
#define valnv_ob_op	0x19
#define valni_qh_op	0x1a
#define valnv_qh_op	0x1b
#define vshfl_op	0x1f
#define vrnd_zu_op	0x20
#define vrnd_nau_op	0x21
#define vrnd_neu_op	0x22
#define vrnd_zs_op	0x24
#define vrnd_nas_op	0x25
#define vrnd_nes_op	0x26
#define vmul_op		0x30
#define vmuls_op	0x32
#define vmula_op	0x33
#define vsuba_op	0x36
#define vadda_op	0x37
#define vwac_op		0x3e
#define vrac_op		0x3f

/* format field of cop2 opcodes */
#define ob_fmt		0x0
#define qh_fmt		0x1

/* func field for mad opcodes (ma_format.func) */
#define madd_op         0x04
#define msub_op         0x05
#define nmadd_op        0x06
#define nmsub_op        0x07

/* obsolete opcodes */
#define cop3_op		0x13

#endif	/* __SYS_INST_H__ */
