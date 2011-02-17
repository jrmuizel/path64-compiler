/*
 * Copyright 2009 SiCortex, Inc.
 */

/*
  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.

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

/*
 *
 * MIPS scheduling information for twc9a on SiCortex twc9a
 *
 * For documentation on the opcodes and pipelines, see
 * SiCortex IceT Process Core Specification
 *
 */

#include "si_gen.h"
#include "targ_isa_subset.h"
#include "topcode.h"

static RESOURCE res_Issue, res_EX, res_FP, res_MP;
static RESOURCE res_MC, res_ID, res_IM, res_FS;

int
main (int argc, char *argv[])
{
  Machine("twc9a", ISA_SUBSET_MIPS4, argc, argv);

  res_Issue = RESOURCE_Create ("Issue", 2);
  res_EX = RESOURCE_Create ("ExecutionPipe", 1);
  res_FP = RESOURCE_Create ("FloatPipe", 1);
  res_MP = RESOURCE_Create ("MemoryPipe", 1);
  res_MC = RESOURCE_Create ("MemoryCacheInstr", 1);
  res_ID = RESOURCE_Create ("IntegerDivide", 1);
  res_IM = RESOURCE_Create ("IntegerMultiply", 1);
  res_FS = RESOURCE_Create ("FloatSqrtDivide", 1);

  /* instruction categories and units */

  // Instruction Summary
  //	TOP_abs_d         	Pipef__Rep01__Lat02
  //	TOP_abs_ps        	Pipef__Rep01__Lat02
  //	TOP_abs_s         	Pipef__Rep01__Lat02
  //	TOP_add           	Pipee__Rep01__Lat01
  //	TOP_add_d         	Pipef__Rep01__Lat06
  //	TOP_add_ps        	Pipef__Rep01__Lat06
  //	TOP_add_s         	Pipef__Rep01__Lat06
  //	TOP_addi          	Pipee__Rep01__Lat01
  //	TOP_addiu         	Pipee__Rep01__Lat01
  //	TOP_addred_ps     	Pipef__Rep01__Lat06
  //	TOP_addu          	Pipee__Rep01__Lat01
  //	TOP_alnv_ps       	Pipeef__Rep01__Lat02
  //	TOP_and           	Pipee__Rep01__Lat01
  //	TOP_andi          	Pipee__Rep01__Lat01
  //	TOP_bc1f          	Pipee__Rep01__Lat01_branch
  //	TOP_bc1t          	Pipee__Rep01__Lat01_branch
  //	TOP_bc2any2f      	Pipee__Rep01__Lat01_branch
  //	TOP_bc2any2t      	Pipee__Rep01__Lat01_branch
  //	TOP_beq           	Pipee__Rep01__Lat01_branch
  //	TOP_bgez          	Pipee__Rep01__Lat01_branch
  //	TOP_bgezal        	Pipee__Rep01__Lat01_branch
  //	TOP_bgtz          	Pipee__Rep01__Lat01_branch
  //	TOP_blez          	Pipee__Rep01__Lat01_branch
  //	TOP_bltz          	Pipee__Rep01__Lat01_branch
  //	TOP_bltzal        	Pipee__Rep01__Lat01_branch
  //	TOP_bne           	Pipee__Rep01__Lat01_branch
  //	TOP_break         	Pipee__Rep01__Lat01
  //	TOP_c_eq_d        	Pipef__Rep01__Lat02
  //	TOP_c_eq_ps       	Pipef__Rep01__Lat02
  //	TOP_c_eq_s        	Pipef__Rep01__Lat02
  //	TOP_c_f_d         	Pipef__Rep01__Lat02
  //	TOP_c_f_ps        	Pipef__Rep01__Lat02
  //	TOP_c_f_s         	Pipef__Rep01__Lat02
  //	TOP_c_ge_d        	Pipef__Rep01__Lat02
  //	TOP_c_ge_ps       	Pipef__Rep01__Lat02
  //	TOP_c_ge_s        	Pipef__Rep01__Lat02
  //	TOP_c_gl_d        	Pipef__Rep01__Lat02
  //	TOP_c_gl_ps       	Pipef__Rep01__Lat02
  //	TOP_c_gl_s        	Pipef__Rep01__Lat02
  //	TOP_c_gle_d       	Pipef__Rep01__Lat02
  //	TOP_c_gle_ps      	Pipef__Rep01__Lat02
  //	TOP_c_gle_s       	Pipef__Rep01__Lat02
  //	TOP_c_gt_d        	Pipef__Rep01__Lat02
  //	TOP_c_gt_ps       	Pipef__Rep01__Lat02
  //	TOP_c_gt_s        	Pipef__Rep01__Lat02
  //	TOP_c_le_d        	Pipef__Rep01__Lat02
  //	TOP_c_le_ps       	Pipef__Rep01__Lat02
  //	TOP_c_le_s        	Pipef__Rep01__Lat02
  //	TOP_c_lt_d        	Pipef__Rep01__Lat02
  //	TOP_c_lt_ps       	Pipef__Rep01__Lat02
  //	TOP_c_lt_s        	Pipef__Rep01__Lat02
  //	TOP_c_neq_d       	Pipef__Rep01__Lat02
  //	TOP_c_neq_ps      	Pipef__Rep01__Lat02
  //	TOP_c_neq_s       	Pipef__Rep01__Lat02
  //	TOP_c_nge_d       	Pipef__Rep01__Lat02
  //	TOP_c_nge_ps      	Pipef__Rep01__Lat02
  //	TOP_c_nge_s       	Pipef__Rep01__Lat02
  //	TOP_c_ngl_d       	Pipef__Rep01__Lat02
  //	TOP_c_ngl_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ngl_s       	Pipef__Rep01__Lat02
  //	TOP_c_ngle_d      	Pipef__Rep01__Lat02
  //	TOP_c_ngle_ps     	Pipef__Rep01__Lat02
  //	TOP_c_ngle_s      	Pipef__Rep01__Lat02
  //	TOP_c_ngt_d       	Pipef__Rep01__Lat02
  //	TOP_c_ngt_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ngt_s       	Pipef__Rep01__Lat02
  //	TOP_c_nle_d       	Pipef__Rep01__Lat02
  //	TOP_c_nle_ps      	Pipef__Rep01__Lat02
  //	TOP_c_nle_s       	Pipef__Rep01__Lat02
  //	TOP_c_nlt_d       	Pipef__Rep01__Lat02
  //	TOP_c_nlt_ps      	Pipef__Rep01__Lat02
  //	TOP_c_nlt_s       	Pipef__Rep01__Lat02
  //	TOP_c_oge_d       	Pipef__Rep01__Lat02
  //	TOP_c_oge_ps      	Pipef__Rep01__Lat02
  //	TOP_c_oge_s       	Pipef__Rep01__Lat02
  //	TOP_c_ogl_d       	Pipef__Rep01__Lat02
  //	TOP_c_ogl_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ogl_s       	Pipef__Rep01__Lat02
  //	TOP_c_ogt_d       	Pipef__Rep01__Lat02
  //	TOP_c_ogt_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ogt_s       	Pipef__Rep01__Lat02
  //	TOP_c_ole_d       	Pipef__Rep01__Lat02
  //	TOP_c_ole_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ole_s       	Pipef__Rep01__Lat02
  //	TOP_c_olt_d       	Pipef__Rep01__Lat02
  //	TOP_c_olt_ps      	Pipef__Rep01__Lat02
  //	TOP_c_olt_s       	Pipef__Rep01__Lat02
  //	TOP_c_or_d        	Pipef__Rep01__Lat02
  //	TOP_c_or_ps       	Pipef__Rep01__Lat02
  //	TOP_c_or_s        	Pipef__Rep01__Lat02
  //	TOP_c_seq_d       	Pipef__Rep01__Lat02
  //	TOP_c_seq_ps      	Pipef__Rep01__Lat02
  //	TOP_c_seq_s       	Pipef__Rep01__Lat02
  //	TOP_c_sf_d        	Pipef__Rep01__Lat02
  //	TOP_c_sf_ps       	Pipef__Rep01__Lat02
  //	TOP_c_sf_s        	Pipef__Rep01__Lat02
  //	TOP_c_sne_d       	Pipef__Rep01__Lat02
  //	TOP_c_sne_ps      	Pipef__Rep01__Lat02
  //	TOP_c_sne_s       	Pipef__Rep01__Lat02
  //	TOP_c_st_d        	Pipef__Rep01__Lat02
  //	TOP_c_st_ps       	Pipef__Rep01__Lat02
  //	TOP_c_st_s        	Pipef__Rep01__Lat02
  //	TOP_c_t_d         	Pipef__Rep01__Lat02
  //	TOP_c_t_ps        	Pipef__Rep01__Lat02
  //	TOP_c_t_s         	Pipef__Rep01__Lat02
  //	TOP_c_ueq_d       	Pipef__Rep01__Lat02
  //	TOP_c_ueq_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ueq_s       	Pipef__Rep01__Lat02
  //	TOP_c_uge_d       	Pipef__Rep01__Lat02
  //	TOP_c_uge_ps      	Pipef__Rep01__Lat02
  //	TOP_c_uge_s       	Pipef__Rep01__Lat02
  //	TOP_c_ugt_d       	Pipef__Rep01__Lat02
  //	TOP_c_ugt_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ugt_s       	Pipef__Rep01__Lat02
  //	TOP_c_ule_d       	Pipef__Rep01__Lat02
  //	TOP_c_ule_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ule_s       	Pipef__Rep01__Lat02
  //	TOP_c_ult_d       	Pipef__Rep01__Lat02
  //	TOP_c_ult_ps      	Pipef__Rep01__Lat02
  //	TOP_c_ult_s       	Pipef__Rep01__Lat02
  //	TOP_c_un_d        	Pipef__Rep01__Lat02
  //	TOP_c_un_ps       	Pipef__Rep01__Lat02
  //	TOP_c_un_s        	Pipef__Rep01__Lat02
  //	TOP_cache         	Pipemc__Rep03__Lat01
  //	TOP_ceil_l_d      	Pipef__Rep01__Lat04
  //	TOP_ceil_l_s      	Pipef__Rep01__Lat04
  //	TOP_ceil_w_d      	Pipef__Rep01__Lat04
  //	TOP_ceil_w_s      	Pipef__Rep01__Lat04
  //	TOP_cfc1          	Pipeef__Rep01__Lat03
  //	TOP_clo           	Pipee__Rep01__Lat02
  //	TOP_clz           	Pipee__Rep01__Lat02
  //	TOP_ctc1          	Pipeef__Rep01__Lat01
  //	TOP_cvt_d_l       	Pipef__Rep01__Lat04
  //	TOP_cvt_d_s       	Pipef__Rep01__Lat04
  //	TOP_cvt_d_w       	Pipef__Rep01__Lat04
  //	TOP_cvt_l_d       	Pipef__Rep01__Lat04
  //	TOP_cvt_l_s       	Pipef__Rep01__Lat04
  //	TOP_cvt_ps_s      	Pipef__Rep01__Lat02
  //	TOP_cvt_s_d       	Pipef__Rep01__Lat04
  //	TOP_cvt_s_l       	Pipef__Rep01__Lat04
  //	TOP_cvt_s_pl      	Pipef__Rep01__Lat02
  //	TOP_cvt_s_pu      	Pipef__Rep01__Lat02
  //	TOP_cvt_s_w       	Pipef__Rep01__Lat04
  //	TOP_cvt_w_d       	Pipef__Rep01__Lat04
  //	TOP_cvt_w_s       	Pipef__Rep01__Lat04
  //	TOP_dadd          	Pipee__Rep01__Lat01
  //	TOP_daddi         	Pipee__Rep01__Lat01
  //	TOP_daddiu        	Pipee__Rep01__Lat01
  //	TOP_daddu         	Pipee__Rep01__Lat01
  //	TOP_dclo          	Pipee__Rep01__Lat02
  //	TOP_dclz          	Pipee__Rep01__Lat02
  //	TOP_ddiv          	Pipeed__Rep65__Lat69
  //	TOP_ddivu         	Pipeed__Rep65__Lat69
  //	TOP_deret         	Pipee__Rep01__Lat01
  //	TOP_dext          	Pipee__Rep01__Lat02
  //	TOP_dextm         	Pipee__Rep01__Lat02
  //	TOP_dextu         	Pipee__Rep01__Lat02
  //	TOP_di            	Pipee__Rep01__Lat03
  //	TOP_dins          	Pipee__Rep01__Lat02
  //	TOP_dinsm         	Pipee__Rep01__Lat02
  //	TOP_dinsu         	Pipee__Rep01__Lat02
  //	TOP_div           	Pipeed__Rep33__Lat37
  //	TOP_div_d         	Pipefs__Rep31__Lat35
  //	TOP_div_s         	Pipefs__Rep16__Lat20
  //	TOP_divu          	Pipeed__Rep33__Lat37
  //	TOP_dmadd         	Pipeeu__Rep02__Lat04
  //	TOP_dmaddu        	Pipeeu__Rep02__Lat04
  //	TOP_dmfc0         	Pipee__Rep01__Lat03
  //	TOP_dmfc1         	Pipeef__Rep01__Lat03
  //	TOP_dmsub         	Pipeeu__Rep02__Lat04
  //	TOP_dmsubu        	Pipeeu__Rep02__Lat04
  //	TOP_dmtc0         	Pipee__Rep01__Lat01
  //	TOP_dmtc1         	Pipeef__Rep01__Lat02
  //	TOP_dmulg         	Pipeeu__Rep02__Lat04
  //	TOP_dmulgu        	Pipeeu__Rep02__Lat04
  //	TOP_dmult         	Pipeeu__Rep02__Lat04
  //	TOP_dmultu        	Pipeeu__Rep02__Lat04
  //	TOP_dperm         	Pipeed__Rep01__Lat02
  //	TOP_dpop          	Pipee__Rep01__Lat02
  //	TOP_drotr         	Pipee__Rep01__Lat02
  //	TOP_drotr32       	Pipee__Rep01__Lat02
  //	TOP_drotrv        	Pipee__Rep01__Lat02
  //	TOP_dsbh          	Pipee__Rep01__Lat01
  //	TOP_dshd          	Pipee__Rep01__Lat01
  //	TOP_dsll          	Pipee__Rep01__Lat02
  //	TOP_dsll32        	Pipee__Rep01__Lat02
  //	TOP_dsllv         	Pipee__Rep01__Lat02
  //	TOP_dsra          	Pipee__Rep01__Lat02
  //	TOP_dsra32        	Pipee__Rep01__Lat02
  //	TOP_dsrav         	Pipee__Rep01__Lat02
  //	TOP_dsrl          	Pipee__Rep01__Lat02
  //	TOP_dsrl32        	Pipee__Rep01__Lat02
  //	TOP_dsrlv         	Pipee__Rep01__Lat02
  //	TOP_dsub          	Pipee__Rep01__Lat01
  //	TOP_dsubu         	Pipee__Rep01__Lat01
  //	TOP_ei            	Pipee__Rep01__Lat03
  //	TOP_eret          	Pipee__Rep01__Lat01
  //	TOP_ext           	Pipee__Rep01__Lat02
  //	TOP_floor_l_d     	Pipef__Rep01__Lat04
  //	TOP_floor_l_s     	Pipef__Rep01__Lat04
  //	TOP_floor_w_d     	Pipef__Rep01__Lat04
  //	TOP_floor_w_s     	Pipef__Rep01__Lat04
  //	TOP_ins           	Pipee__Rep01__Lat02
  //	TOP_j             	Pipee__Rep01__Lat01_branch
  //	TOP_jal           	Pipee__Rep01__Lat01_branch
  //	TOP_jalr          	Pipee__Rep01__Lat01_branch
  //	TOP_jalr_hb       	Pipee__Rep01__Lat01_branch
  //	TOP_jr            	Pipee__Rep01__Lat01_branch
  //	TOP_jr_hb         	Pipee__Rep01__Lat01_branch
  //	TOP_lb            	Pipem__Rep01__Lat04
  //	TOP_lbu           	Pipem__Rep01__Lat04
  //	TOP_lbx           	Pipem__Rep01__Lat04
  //	TOP_ld            	Pipem__Rep01__Lat03
  //	TOP_ldc1          	Pipem__Rep01__Lat03
  //	TOP_ldl           	Pipem__Rep01__Lat06
  //	TOP_ldr           	Pipem__Rep01__Lat06
  //	TOP_ldx           	Pipem__Rep01__Lat03
  //	TOP_ldxc1         	Pipem__Rep01__Lat03
  //	TOP_lh            	Pipem__Rep01__Lat04
  //	TOP_lhu           	Pipem__Rep01__Lat04
  //	TOP_lhx           	Pipem__Rep01__Lat04
  //	TOP_ll            	Pipem__Rep01__Lat03
  //	TOP_lld           	Pipem__Rep01__Lat03
  //	TOP_lui           	Pipee__Rep01__Lat01
  //	TOP_luxc1         	Pipem__Rep01__Lat03
  //	TOP_lw            	Pipem__Rep01__Lat03
  //	TOP_lwc1          	Pipem__Rep01__Lat03
  //	TOP_lwl           	Pipem__Rep01__Lat06
  //	TOP_lwr           	Pipem__Rep01__Lat06
  //	TOP_lwu           	Pipem__Rep01__Lat03
  //	TOP_lwx           	Pipem__Rep01__Lat03
  //	TOP_lwxc1         	Pipem__Rep01__Lat03
  //	TOP_ma_nlupuu_ps  	Pipef__Rep01__Lat06
  //	TOP_ma_plunuu_ps  	Pipef__Rep01__Lat06
  //	TOP_ma_plupll_ps  	Pipef__Rep01__Lat06
  //	TOP_ma_puupul_ps  	Pipef__Rep01__Lat06
  //	TOP_madd          	Pipeeu__Rep02__Lat04
  //	TOP_madd_d        	Pipef__Rep01__Lat06
  //	TOP_madd_ps       	Pipef__Rep01__Lat06
  //	TOP_madd_s        	Pipef__Rep01__Lat06
  //	TOP_maddu         	Pipeeu__Rep02__Lat04
  //	TOP_mfacx         	Pipee__Rep01__Lat04
  //	TOP_mfc0          	Pipee__Rep01__Lat03
  //	TOP_mfc1          	Pipeef__Rep01__Lat03
  //	TOP_mfhc1         	Pipeef__Rep01__Lat03
  //	TOP_mfhi          	Pipee__Rep01__Lat04
  //	TOP_mflo          	Pipee__Rep01__Lat04
  //	TOP_mov_d         	Pipef__Rep01__Lat02
  //	TOP_mov_ps        	Pipef__Rep01__Lat02
  //	TOP_mov_s         	Pipef__Rep01__Lat02
  //	TOP_movf          	Pipee__Rep01__Lat02
  //	TOP_movf_d        	Pipef__Rep01__Lat02
  //	TOP_movf_ps       	Pipef__Rep01__Lat02
  //	TOP_movf_s        	Pipef__Rep01__Lat02
  //	TOP_movn          	Pipee__Rep01__Lat02
  //	TOP_movn_d        	Pipeef__Rep01__Lat02
  //	TOP_movn_ps       	Pipeef__Rep01__Lat02
  //	TOP_movn_s        	Pipeef__Rep01__Lat02
  //	TOP_movt          	Pipee__Rep01__Lat02
  //	TOP_movt_d        	Pipef__Rep01__Lat02
  //	TOP_movt_ps       	Pipef__Rep01__Lat02
  //	TOP_movt_s        	Pipef__Rep01__Lat02
  //	TOP_movz          	Pipee__Rep01__Lat02
  //	TOP_movz_d        	Pipeef__Rep01__Lat02
  //	TOP_movz_ps       	Pipeef__Rep01__Lat02
  //	TOP_movz_s        	Pipeef__Rep01__Lat02
  //	TOP_msub          	Pipeeu__Rep02__Lat04
  //	TOP_msub_d        	Pipef__Rep01__Lat06
  //	TOP_msub_ps       	Pipef__Rep01__Lat06
  //	TOP_msub_s        	Pipef__Rep01__Lat06
  //	TOP_msubu         	Pipeeu__Rep02__Lat04
  //	TOP_mtacx         	Pipee__Rep01__Lat01
  //	TOP_mtc0          	Pipee__Rep01__Lat01
  //	TOP_mtc1          	Pipeef__Rep01__Lat02
  //	TOP_mthc1         	Pipeef__Rep01__Lat07
  //	TOP_mthi          	Pipee__Rep01__Lat01
  //	TOP_mtlo          	Pipee__Rep01__Lat01
  //	TOP_mul           	Pipeeu__Rep02__Lat04
  //	TOP_mul_d         	Pipef__Rep01__Lat06
  //	TOP_mul_ps        	Pipef__Rep01__Lat06
  //	TOP_mul_s         	Pipef__Rep01__Lat06
  //	TOP_mulred_ps     	Pipef__Rep01__Lat06
  //	TOP_mult          	Pipeeu__Rep02__Lat04
  //	TOP_multu         	Pipeeu__Rep02__Lat04
  //	TOP_neg_d         	Pipef__Rep01__Lat02
  //	TOP_neg_ps        	Pipef__Rep01__Lat02
  //	TOP_neg_s         	Pipef__Rep01__Lat02
  //	TOP_nmadd_d       	Pipef__Rep01__Lat06
  //	TOP_nmadd_ps      	Pipef__Rep01__Lat06
  //	TOP_nmadd_s       	Pipef__Rep01__Lat06
  //	TOP_nmsub_d       	Pipef__Rep01__Lat06
  //	TOP_nmsub_ps      	Pipef__Rep01__Lat06
  //	TOP_nmsub_s       	Pipef__Rep01__Lat06
  //	TOP_nor           	Pipee__Rep01__Lat01
  //	TOP_or            	Pipee__Rep01__Lat01
  //	TOP_ori           	Pipee__Rep01__Lat01
  //	TOP_perfctl       	Pipee__Rep01__Lat01
  //	TOP_pll_ps        	Pipef__Rep01__Lat02
  //	TOP_plu_ps        	Pipef__Rep01__Lat02
  //	TOP_pop           	Pipee__Rep01__Lat02
  //	TOP_pref          	Pipem__Rep01__Lat01
  //	TOP_prefx         	Pipem__Rep01__Lat01
  //	TOP_pul_ps        	Pipef__Rep01__Lat02
  //	TOP_puu_ps        	Pipef__Rep01__Lat02
  //	TOP_rdhwr         	Pipee__Rep01__Lat03
  //	TOP_rdpgpr        	Pipee__Rep01__Lat01
  //	TOP_recip_d       	Pipefs__Rep31__Lat35
  //	TOP_recip_s       	Pipefs__Rep16__Lat20
  //	TOP_recipit1_d    	Pipef__Rep01__Lat04
  //	TOP_recipit1_ps   	Pipef__Rep01__Lat04
  //	TOP_recipit1_s    	Pipef__Rep01__Lat04
  //	TOP_recipit2_d    	Pipef__Rep01__Lat06
  //	TOP_recipit2_ps   	Pipef__Rep01__Lat06
  //	TOP_recipit2_s    	Pipef__Rep01__Lat06
  //	TOP_rotr          	Pipee__Rep01__Lat02
  //	TOP_rotrv         	Pipee__Rep01__Lat02
  //	TOP_round_l_d     	Pipef__Rep01__Lat04
  //	TOP_round_l_s     	Pipef__Rep01__Lat04
  //	TOP_round_w_d     	Pipef__Rep01__Lat04
  //	TOP_round_w_s     	Pipef__Rep01__Lat04
  //	TOP_rsqrt_d       	Pipefs__Rep66__Lat70
  //	TOP_rsqrt_s       	Pipefs__Rep36__Lat40
  //	TOP_rsqrtit1_d    	Pipef__Rep01__Lat04
  //	TOP_rsqrtit1_ps   	Pipef__Rep01__Lat04
  //	TOP_rsqrtit1_s    	Pipef__Rep01__Lat04
  //	TOP_rsqrtit2_d    	Pipef__Rep01__Lat06
  //	TOP_rsqrtit2_ps   	Pipef__Rep01__Lat06
  //	TOP_rsqrtit2_s    	Pipef__Rep01__Lat06
  //	TOP_sb            	Pipem__Rep01__Lat01
  //	TOP_sc            	Pipem__Rep01__Lat04
  //	TOP_scd           	Pipem__Rep01__Lat04
  //	TOP_sd            	Pipem__Rep01__Lat01
  //	TOP_sdbbp         	Pipee__Rep01__Lat01
  //	TOP_sdc1          	Pipem__Rep01__Lat01
  //	TOP_sdl           	Pipem__Rep01__Lat01
  //	TOP_sdr           	Pipem__Rep01__Lat01
  //	TOP_sdxc1         	Pipem__Rep01__Lat01
  //	TOP_seb           	Pipee__Rep01__Lat01
  //	TOP_seh           	Pipee__Rep01__Lat01
  //	TOP_sext          	Pipee__Rep01__Lat02
  //	TOP_sh            	Pipem__Rep01__Lat01
  //	TOP_sll           	Pipee__Rep01__Lat02
  //	TOP_sllv          	Pipee__Rep01__Lat02
  //	TOP_slt           	Pipee__Rep01__Lat01
  //	TOP_slti          	Pipee__Rep01__Lat01
  //	TOP_sltiu         	Pipee__Rep01__Lat01
  //	TOP_sltu          	Pipee__Rep01__Lat01
  //	TOP_sqrt_d        	Pipefs__Rep31__Lat35
  //	TOP_sqrt_s        	Pipefs__Rep16__Lat20
  //	TOP_sra           	Pipee__Rep01__Lat02
  //	TOP_srav          	Pipee__Rep01__Lat02
  //	TOP_srl           	Pipee__Rep01__Lat02
  //	TOP_srlv          	Pipee__Rep01__Lat02
  //	TOP_sub           	Pipee__Rep01__Lat01
  //	TOP_sub_d         	Pipef__Rep01__Lat06
  //	TOP_sub_ps        	Pipef__Rep01__Lat06
  //	TOP_sub_s         	Pipef__Rep01__Lat06
  //	TOP_subu          	Pipee__Rep01__Lat01
  //	TOP_suxc1         	Pipem__Rep01__Lat01
  //	TOP_sw            	Pipem__Rep01__Lat01
  //	TOP_swc1          	Pipem__Rep01__Lat01
  //	TOP_swl           	Pipem__Rep01__Lat01
  //	TOP_swr           	Pipem__Rep01__Lat01
  //	TOP_swxc1         	Pipem__Rep01__Lat01
  //	TOP_sync          	Pipem__Rep01__Lat01
  //	TOP_synci         	Pipemc__Rep03__Lat01
  //	TOP_syscall       	Pipee__Rep01__Lat01
  //	TOP_teq           	Pipee__Rep01__Lat01
  //	TOP_teqi          	Pipee__Rep01__Lat01
  //	TOP_tge           	Pipee__Rep01__Lat01
  //	TOP_tgei          	Pipee__Rep01__Lat01
  //	TOP_tgeiu         	Pipee__Rep01__Lat01
  //	TOP_tgeu          	Pipee__Rep01__Lat01
  //	TOP_tlbp          	Pipemc__Rep04__Lat01
  //	TOP_tlbr          	Pipemc__Rep04__Lat01
  //	TOP_tlbwi         	Pipemc__Rep04__Lat01
  //	TOP_tlbwir        	Pipemc__Rep04__Lat01
  //	TOP_tlbwr         	Pipemc__Rep04__Lat01
  //	TOP_tlbwrr        	Pipemc__Rep04__Lat01
  //	TOP_tlt           	Pipee__Rep01__Lat01
  //	TOP_tlti          	Pipee__Rep01__Lat01
  //	TOP_tltiu         	Pipee__Rep01__Lat01
  //	TOP_tltu          	Pipee__Rep01__Lat01
  //	TOP_tne           	Pipee__Rep01__Lat01
  //	TOP_tnei          	Pipee__Rep01__Lat01
  //	TOP_trunc_l_d     	Pipef__Rep01__Lat04
  //	TOP_trunc_l_s     	Pipef__Rep01__Lat04
  //	TOP_trunc_w_d     	Pipef__Rep01__Lat04
  //	TOP_trunc_w_s     	Pipef__Rep01__Lat04
  //	TOP_wait          	Pipee__Rep01__Lat01
  //	TOP_wrpgpr        	Pipee__Rep01__Lat01
  //	TOP_wsbh          	Pipee__Rep01__Lat01
  //	TOP_xor           	Pipee__Rep01__Lat01
  //	TOP_xori          	Pipee__Rep01__Lat01

  Instruction_Group ("Pipee__Rep01__Lat01",
			  TOP_add,
			  TOP_addi,
			  TOP_addiu,
			  TOP_addu,
			  TOP_and,
			  TOP_andi,
			  TOP_break,
			  TOP_dadd,
			  TOP_daddi,
			  TOP_daddiu,
			  TOP_daddu,
		     // TOP_deret,
		     // TOP_dmtc0,
			  TOP_dsbh,
			  TOP_dshd,
			  TOP_dsub,
			  TOP_dsubu,
		     // TOP_eret,
			  TOP_lui,
			  TOP_mtacx,
		     // TOP_mtc0,
			  TOP_mthi,
			  TOP_mtlo,
			  TOP_nor,
			  TOP_or,
			  TOP_ori,
			  TOP_perfctl,
		     // TOP_rdpgpr,
		     // TOP_sdbbp,
			  TOP_seb,
			  TOP_seh,
			  TOP_slt,
			  TOP_slti,
			  TOP_sltiu,
			  TOP_sltu,
			  TOP_sub,
			  TOP_subu,
			  TOP_syscall,
			  TOP_teq,
			  TOP_teqi,
			  TOP_tge,
			  TOP_tgei,
			  TOP_tgeiu,
			  TOP_tgeu,
			  TOP_tlt,
			  TOP_tlti,
			  TOP_tltiu,
			  TOP_tltu,
			  TOP_tne,
			  TOP_tnei,
		     // TOP_wait,
		     // TOP_wrpgpr,
			  TOP_wsbh,
			  TOP_xor,
			  TOP_xori,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipee__Rep01__Lat01_branch",
			  TOP_bc1f,
			  TOP_bc1t,
			  TOP_bc2any2f,
			  TOP_bc2any2t,
			  TOP_beq,
			  TOP_bgez,
			  TOP_bgezal,
			  TOP_bgtz,
			  TOP_blez,
			  TOP_bltz,
			  TOP_bltzal,
			  TOP_bne,
			  TOP_j,
			  TOP_jal,
			  TOP_jalr,
		     // TOP_jalr_hb,
			  TOP_jr,
		     // TOP_jr_hb,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipee__Rep01__Lat02",
			  TOP_clo,
			  TOP_clz,
			  TOP_dclo,
			  TOP_dclz,
			  TOP_dext,
			  TOP_dextm,
			  TOP_dextu,
			  TOP_dins,
			  TOP_dinsm,
			  TOP_dinsu,
			  TOP_dpop,
			  TOP_drotr,
			  TOP_drotr32,
			  TOP_drotrv,
			  TOP_dsll,
			  TOP_dsll32,
			  TOP_dsllv,
			  TOP_dsra,
			  TOP_dsra32,
			  TOP_dsrav,
			  TOP_dsrl,
			  TOP_dsrl32,
			  TOP_dsrlv,
			  TOP_ext,
			  TOP_ins,
			  TOP_movf,
			  TOP_movn,
			  TOP_movt,
			  TOP_movz,
			  TOP_pop,
			  TOP_rotr,
			  TOP_rotrv,
			  TOP_sext,
			  TOP_sll,
			  TOP_sllv,
			  TOP_sra,
			  TOP_srav,
			  TOP_srl,
			  TOP_srlv,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipee__Rep01__Lat03",
		     // TOP_di,
		     // TOP_dmfc0,
		     // TOP_ei,
		     // TOP_mfc0,
		     // TOP_rdhwr,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (3);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipee__Rep01__Lat04",
			  TOP_mfacx,
			  TOP_mfhi,
			  TOP_mflo,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipeed__Rep01__Lat02",
			  TOP_dperm,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_ID, 0);

  Instruction_Group ("Pipeed__Rep33__Lat37",
			  TOP_div,
			  TOP_divu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (37);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_ID, 0);
  Resource_Requirement (res_ID, 1);
  Resource_Requirement (res_ID, 2);
  Resource_Requirement (res_ID, 3);
  Resource_Requirement (res_ID, 4);
  Resource_Requirement (res_ID, 5);
  Resource_Requirement (res_ID, 6);
  Resource_Requirement (res_ID, 7);
  Resource_Requirement (res_ID, 8);
  Resource_Requirement (res_ID, 9);
  Resource_Requirement (res_ID, 10);
  Resource_Requirement (res_ID, 11);
  Resource_Requirement (res_ID, 12);
  Resource_Requirement (res_ID, 13);
  Resource_Requirement (res_ID, 14);
  Resource_Requirement (res_ID, 15);
  Resource_Requirement (res_ID, 16);
  Resource_Requirement (res_ID, 17);
  Resource_Requirement (res_ID, 18);
  Resource_Requirement (res_ID, 19);
  Resource_Requirement (res_ID, 20);
  Resource_Requirement (res_ID, 21);
  Resource_Requirement (res_ID, 22);
  Resource_Requirement (res_ID, 23);
  Resource_Requirement (res_ID, 24);
  Resource_Requirement (res_ID, 25);
  Resource_Requirement (res_ID, 26);
  Resource_Requirement (res_ID, 27);
  Resource_Requirement (res_ID, 28);
  Resource_Requirement (res_ID, 29);
  Resource_Requirement (res_ID, 30);
  Resource_Requirement (res_ID, 31);
  Resource_Requirement (res_ID, 32);

  Instruction_Group ("Pipeed__Rep65__Lat69",
			  TOP_ddiv,
			  TOP_ddivu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (69);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_ID, 0);
  Resource_Requirement (res_ID, 1);
  Resource_Requirement (res_ID, 2);
  Resource_Requirement (res_ID, 3);
  Resource_Requirement (res_ID, 4);
  Resource_Requirement (res_ID, 5);
  Resource_Requirement (res_ID, 6);
  Resource_Requirement (res_ID, 7);
  Resource_Requirement (res_ID, 8);
  Resource_Requirement (res_ID, 9);
  Resource_Requirement (res_ID, 10);
  Resource_Requirement (res_ID, 11);
  Resource_Requirement (res_ID, 12);
  Resource_Requirement (res_ID, 13);
  Resource_Requirement (res_ID, 14);
  Resource_Requirement (res_ID, 15);
  Resource_Requirement (res_ID, 16);
  Resource_Requirement (res_ID, 17);
  Resource_Requirement (res_ID, 18);
  Resource_Requirement (res_ID, 19);
  Resource_Requirement (res_ID, 20);
  Resource_Requirement (res_ID, 21);
  Resource_Requirement (res_ID, 22);
  Resource_Requirement (res_ID, 23);
  Resource_Requirement (res_ID, 24);
  Resource_Requirement (res_ID, 25);
  Resource_Requirement (res_ID, 26);
  Resource_Requirement (res_ID, 27);
  Resource_Requirement (res_ID, 28);
  Resource_Requirement (res_ID, 29);
  Resource_Requirement (res_ID, 30);
  Resource_Requirement (res_ID, 31);
  Resource_Requirement (res_ID, 32);
  Resource_Requirement (res_ID, 33);
  Resource_Requirement (res_ID, 34);
  Resource_Requirement (res_ID, 35);
  Resource_Requirement (res_ID, 36);
  Resource_Requirement (res_ID, 37);
  Resource_Requirement (res_ID, 38);
  Resource_Requirement (res_ID, 39);
  Resource_Requirement (res_ID, 40);
  Resource_Requirement (res_ID, 41);
  Resource_Requirement (res_ID, 42);
  Resource_Requirement (res_ID, 43);
  Resource_Requirement (res_ID, 44);
  Resource_Requirement (res_ID, 45);
  Resource_Requirement (res_ID, 46);
  Resource_Requirement (res_ID, 47);
  Resource_Requirement (res_ID, 48);
  Resource_Requirement (res_ID, 49);
  Resource_Requirement (res_ID, 50);
  Resource_Requirement (res_ID, 51);
  Resource_Requirement (res_ID, 52);
  Resource_Requirement (res_ID, 53);
  Resource_Requirement (res_ID, 54);
  Resource_Requirement (res_ID, 55);
  Resource_Requirement (res_ID, 56);
  Resource_Requirement (res_ID, 57);
  Resource_Requirement (res_ID, 58);
  Resource_Requirement (res_ID, 59);
  Resource_Requirement (res_ID, 60);
  Resource_Requirement (res_ID, 61);
  Resource_Requirement (res_ID, 62);
  Resource_Requirement (res_ID, 63);
  Resource_Requirement (res_ID, 64);

  Instruction_Group ("Pipeef__Rep01__Lat01",
			  TOP_ctc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeef__Rep01__Lat02",
			  TOP_alnv_ps,
			  TOP_dmtc1,
			  TOP_movn_d,
			  TOP_movn_ps,
			  TOP_movn_s,
			  TOP_movz_d,
			  TOP_movz_ps,
			  TOP_movz_s,
			  TOP_mtc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeef__Rep01__Lat03",
			  TOP_cfc1,
			  TOP_dmfc1,
			  TOP_mfc1,
			  TOP_mfhc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (3);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeef__Rep01__Lat07",
			  TOP_mthc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (7);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeeu__Rep02__Lat04",
			  TOP_dmadd,
			  TOP_dmaddu,
			  TOP_dmsub,
			  TOP_dmsubu,
			  TOP_dmulg,
			  TOP_dmulgu,
			  TOP_dmult,
			  TOP_dmultu,
			  TOP_madd,
			  TOP_maddu,
			  TOP_msub,
			  TOP_msubu,
			  TOP_mul,
			  TOP_mult,
			  TOP_multu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_EX, 1);
  Resource_Requirement (res_IM, 0);
  Resource_Requirement (res_IM, 1);

  Instruction_Group ("Pipef__Rep01__Lat02",
			  TOP_abs_d,
			  TOP_abs_ps,
			  TOP_abs_s,
			  TOP_c_eq_d,
			  TOP_c_eq_ps,
			  TOP_c_eq_s,
			  TOP_c_f_d,
			  TOP_c_f_ps,
			  TOP_c_f_s,
			  TOP_c_ge_d,
			  TOP_c_ge_ps,
			  TOP_c_ge_s,
			  TOP_c_gl_d,
			  TOP_c_gl_ps,
			  TOP_c_gl_s,
			  TOP_c_gle_d,
			  TOP_c_gle_ps,
			  TOP_c_gle_s,
			  TOP_c_gt_d,
			  TOP_c_gt_ps,
			  TOP_c_gt_s,
			  TOP_c_le_d,
			  TOP_c_le_ps,
			  TOP_c_le_s,
			  TOP_c_lt_d,
			  TOP_c_lt_ps,
			  TOP_c_lt_s,
			  TOP_c_neq_d,
			  TOP_c_neq_ps,
			  TOP_c_neq_s,
			  TOP_c_nge_d,
			  TOP_c_nge_ps,
			  TOP_c_nge_s,
			  TOP_c_ngl_d,
			  TOP_c_ngl_ps,
			  TOP_c_ngl_s,
			  TOP_c_ngle_d,
			  TOP_c_ngle_ps,
			  TOP_c_ngle_s,
			  TOP_c_ngt_d,
			  TOP_c_ngt_ps,
			  TOP_c_ngt_s,
			  TOP_c_nle_d,
			  TOP_c_nle_ps,
			  TOP_c_nle_s,
			  TOP_c_nlt_d,
			  TOP_c_nlt_ps,
			  TOP_c_nlt_s,
			  TOP_c_oge_d,
			  TOP_c_oge_ps,
			  TOP_c_oge_s,
			  TOP_c_ogl_d,
			  TOP_c_ogl_ps,
			  TOP_c_ogl_s,
			  TOP_c_ogt_d,
			  TOP_c_ogt_ps,
			  TOP_c_ogt_s,
			  TOP_c_ole_d,
			  TOP_c_ole_ps,
			  TOP_c_ole_s,
			  TOP_c_olt_d,
			  TOP_c_olt_ps,
			  TOP_c_olt_s,
			  TOP_c_or_d,
			  TOP_c_or_ps,
			  TOP_c_or_s,
			  TOP_c_seq_d,
			  TOP_c_seq_ps,
			  TOP_c_seq_s,
			  TOP_c_sf_d,
			  TOP_c_sf_ps,
			  TOP_c_sf_s,
			  TOP_c_sne_d,
			  TOP_c_sne_ps,
			  TOP_c_sne_s,
			  TOP_c_st_d,
			  TOP_c_st_ps,
			  TOP_c_st_s,
			  TOP_c_t_d,
			  TOP_c_t_ps,
			  TOP_c_t_s,
			  TOP_c_ueq_d,
			  TOP_c_ueq_ps,
			  TOP_c_ueq_s,
			  TOP_c_uge_d,
			  TOP_c_uge_ps,
			  TOP_c_uge_s,
			  TOP_c_ugt_d,
			  TOP_c_ugt_ps,
			  TOP_c_ugt_s,
			  TOP_c_ule_d,
			  TOP_c_ule_ps,
			  TOP_c_ule_s,
			  TOP_c_ult_d,
			  TOP_c_ult_ps,
			  TOP_c_ult_s,
			  TOP_c_un_d,
			  TOP_c_un_ps,
			  TOP_c_un_s,
			  TOP_cvt_ps_s,
			  TOP_cvt_s_pl,
			  TOP_cvt_s_pu,
			  TOP_mov_d,
			  TOP_mov_ps,
			  TOP_mov_s,
			  TOP_movf_d,
			  TOP_movf_ps,
			  TOP_movf_s,
			  TOP_movt_d,
			  TOP_movt_ps,
			  TOP_movt_s,
			  TOP_neg_d,
			  TOP_neg_ps,
			  TOP_neg_s,
			  TOP_pll_ps,
			  TOP_plu_ps,
			  TOP_pul_ps,
			  TOP_puu_ps,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipef__Rep01__Lat04",
			  TOP_ceil_l_d,
			  TOP_ceil_l_s,
			  TOP_ceil_w_d,
			  TOP_ceil_w_s,
			  TOP_cvt_d_l,
			  TOP_cvt_d_s,
			  TOP_cvt_d_w,
			  TOP_cvt_l_d,
			  TOP_cvt_l_s,
			  TOP_cvt_s_d,
			  TOP_cvt_s_l,
			  TOP_cvt_s_w,
			  TOP_cvt_w_d,
			  TOP_cvt_w_s,
			  TOP_floor_l_d,
			  TOP_floor_l_s,
			  TOP_floor_w_d,
			  TOP_floor_w_s,
			  TOP_recipit1_d,
			  TOP_recipit1_ps,
			  TOP_recipit1_s,
			  TOP_round_l_d,
			  TOP_round_l_s,
			  TOP_round_w_d,
			  TOP_round_w_s,
			  TOP_rsqrtit1_d,
			  TOP_rsqrtit1_ps,
			  TOP_rsqrtit1_s,
			  TOP_trunc_l_d,
			  TOP_trunc_l_s,
			  TOP_trunc_w_d,
			  TOP_trunc_w_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipef__Rep01__Lat06",
			  TOP_add_d,
			  TOP_add_ps,
			  TOP_add_s,
			  TOP_addred_ps,
			  TOP_ma_nlupuu_ps,
			  TOP_ma_plunuu_ps,
			  TOP_ma_plupll_ps,
			  TOP_ma_puupul_ps,
			  TOP_madd_d,
			  TOP_madd_ps,
			  TOP_madd_s,
			  TOP_msub_d,
			  TOP_msub_ps,
			  TOP_msub_s,
			  TOP_mul_d,
			  TOP_mul_ps,
			  TOP_mul_s,
			  TOP_mulred_ps,
			  TOP_nmadd_d,
			  TOP_nmadd_ps,
			  TOP_nmadd_s,
			  TOP_nmsub_d,
			  TOP_nmsub_ps,
			  TOP_nmsub_s,
			  TOP_recipit2_d,
			  TOP_recipit2_ps,
			  TOP_recipit2_s,
			  TOP_rsqrtit2_d,
			  TOP_rsqrtit2_ps,
			  TOP_rsqrtit2_s,
			  TOP_sub_d,
			  TOP_sub_ps,
			  TOP_sub_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (6);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipefs__Rep16__Lat20",
			  TOP_div_s,
			  TOP_recip_s,
			  TOP_sqrt_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (20);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FS, 0);
  Resource_Requirement (res_FS, 1);
  Resource_Requirement (res_FS, 2);
  Resource_Requirement (res_FS, 3);
  Resource_Requirement (res_FS, 4);
  Resource_Requirement (res_FS, 5);
  Resource_Requirement (res_FS, 6);
  Resource_Requirement (res_FS, 7);
  Resource_Requirement (res_FS, 8);
  Resource_Requirement (res_FS, 9);
  Resource_Requirement (res_FS, 10);
  Resource_Requirement (res_FS, 11);
  Resource_Requirement (res_FS, 12);
  Resource_Requirement (res_FS, 13);
  Resource_Requirement (res_FS, 14);
  Resource_Requirement (res_FS, 15);

  Instruction_Group ("Pipefs__Rep31__Lat35",
			  TOP_div_d,
			  TOP_recip_d,
			  TOP_sqrt_d,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (35);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FS, 0);
  Resource_Requirement (res_FS, 1);
  Resource_Requirement (res_FS, 2);
  Resource_Requirement (res_FS, 3);
  Resource_Requirement (res_FS, 4);
  Resource_Requirement (res_FS, 5);
  Resource_Requirement (res_FS, 6);
  Resource_Requirement (res_FS, 7);
  Resource_Requirement (res_FS, 8);
  Resource_Requirement (res_FS, 9);
  Resource_Requirement (res_FS, 10);
  Resource_Requirement (res_FS, 11);
  Resource_Requirement (res_FS, 12);
  Resource_Requirement (res_FS, 13);
  Resource_Requirement (res_FS, 14);
  Resource_Requirement (res_FS, 15);
  Resource_Requirement (res_FS, 16);
  Resource_Requirement (res_FS, 17);
  Resource_Requirement (res_FS, 18);
  Resource_Requirement (res_FS, 19);
  Resource_Requirement (res_FS, 20);
  Resource_Requirement (res_FS, 21);
  Resource_Requirement (res_FS, 22);
  Resource_Requirement (res_FS, 23);
  Resource_Requirement (res_FS, 24);
  Resource_Requirement (res_FS, 25);
  Resource_Requirement (res_FS, 26);
  Resource_Requirement (res_FS, 27);
  Resource_Requirement (res_FS, 28);
  Resource_Requirement (res_FS, 29);
  Resource_Requirement (res_FS, 30);

  Instruction_Group ("Pipefs__Rep36__Lat40",
			  TOP_rsqrt_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (40);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FS, 0);
  Resource_Requirement (res_FS, 1);
  Resource_Requirement (res_FS, 2);
  Resource_Requirement (res_FS, 3);
  Resource_Requirement (res_FS, 4);
  Resource_Requirement (res_FS, 5);
  Resource_Requirement (res_FS, 6);
  Resource_Requirement (res_FS, 7);
  Resource_Requirement (res_FS, 8);
  Resource_Requirement (res_FS, 9);
  Resource_Requirement (res_FS, 10);
  Resource_Requirement (res_FS, 11);
  Resource_Requirement (res_FS, 12);
  Resource_Requirement (res_FS, 13);
  Resource_Requirement (res_FS, 14);
  Resource_Requirement (res_FS, 15);
  Resource_Requirement (res_FS, 16);
  Resource_Requirement (res_FS, 17);
  Resource_Requirement (res_FS, 18);
  Resource_Requirement (res_FS, 19);
  Resource_Requirement (res_FS, 20);
  Resource_Requirement (res_FS, 21);
  Resource_Requirement (res_FS, 22);
  Resource_Requirement (res_FS, 23);
  Resource_Requirement (res_FS, 24);
  Resource_Requirement (res_FS, 25);
  Resource_Requirement (res_FS, 26);
  Resource_Requirement (res_FS, 27);
  Resource_Requirement (res_FS, 28);
  Resource_Requirement (res_FS, 29);
  Resource_Requirement (res_FS, 30);
  Resource_Requirement (res_FS, 31);
  Resource_Requirement (res_FS, 32);
  Resource_Requirement (res_FS, 33);
  Resource_Requirement (res_FS, 34);
  Resource_Requirement (res_FS, 35);

  Instruction_Group ("Pipefs__Rep66__Lat70",
			  TOP_rsqrt_d,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (70);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FS, 0);
  Resource_Requirement (res_FS, 1);
  Resource_Requirement (res_FS, 2);
  Resource_Requirement (res_FS, 3);
  Resource_Requirement (res_FS, 4);
  Resource_Requirement (res_FS, 5);
  Resource_Requirement (res_FS, 6);
  Resource_Requirement (res_FS, 7);
  Resource_Requirement (res_FS, 8);
  Resource_Requirement (res_FS, 9);
  Resource_Requirement (res_FS, 10);
  Resource_Requirement (res_FS, 11);
  Resource_Requirement (res_FS, 12);
  Resource_Requirement (res_FS, 13);
  Resource_Requirement (res_FS, 14);
  Resource_Requirement (res_FS, 15);
  Resource_Requirement (res_FS, 16);
  Resource_Requirement (res_FS, 17);
  Resource_Requirement (res_FS, 18);
  Resource_Requirement (res_FS, 19);
  Resource_Requirement (res_FS, 20);
  Resource_Requirement (res_FS, 21);
  Resource_Requirement (res_FS, 22);
  Resource_Requirement (res_FS, 23);
  Resource_Requirement (res_FS, 24);
  Resource_Requirement (res_FS, 25);
  Resource_Requirement (res_FS, 26);
  Resource_Requirement (res_FS, 27);
  Resource_Requirement (res_FS, 28);
  Resource_Requirement (res_FS, 29);
  Resource_Requirement (res_FS, 30);
  Resource_Requirement (res_FS, 31);
  Resource_Requirement (res_FS, 32);
  Resource_Requirement (res_FS, 33);
  Resource_Requirement (res_FS, 34);
  Resource_Requirement (res_FS, 35);
  Resource_Requirement (res_FS, 36);
  Resource_Requirement (res_FS, 37);
  Resource_Requirement (res_FS, 38);
  Resource_Requirement (res_FS, 39);
  Resource_Requirement (res_FS, 40);
  Resource_Requirement (res_FS, 41);
  Resource_Requirement (res_FS, 42);
  Resource_Requirement (res_FS, 43);
  Resource_Requirement (res_FS, 44);
  Resource_Requirement (res_FS, 45);
  Resource_Requirement (res_FS, 46);
  Resource_Requirement (res_FS, 47);
  Resource_Requirement (res_FS, 48);
  Resource_Requirement (res_FS, 49);
  Resource_Requirement (res_FS, 50);
  Resource_Requirement (res_FS, 51);
  Resource_Requirement (res_FS, 52);
  Resource_Requirement (res_FS, 53);
  Resource_Requirement (res_FS, 54);
  Resource_Requirement (res_FS, 55);
  Resource_Requirement (res_FS, 56);
  Resource_Requirement (res_FS, 57);
  Resource_Requirement (res_FS, 58);
  Resource_Requirement (res_FS, 59);
  Resource_Requirement (res_FS, 60);
  Resource_Requirement (res_FS, 61);
  Resource_Requirement (res_FS, 62);
  Resource_Requirement (res_FS, 63);
  Resource_Requirement (res_FS, 64);
  Resource_Requirement (res_FS, 65);

  Instruction_Group ("Pipem__Rep01__Lat01",
			  TOP_pref,
			  TOP_prefx,
			  TOP_sb,
			  TOP_sd,
			  TOP_sdc1,
			  TOP_sdl,
			  TOP_sdr,
			  TOP_sdxc1,
			  TOP_sh,
			  TOP_suxc1,
			  TOP_sw,
			  TOP_swc1,
			  TOP_swl,
			  TOP_swr,
			  TOP_swxc1,
			  TOP_sync,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_MP, 0);

  Instruction_Group ("Pipem__Rep01__Lat03",
			  TOP_ld,
			  TOP_ldc1,
			  TOP_ldx,
			  TOP_ldxc1,
			  TOP_ll,
			  TOP_lld,
			  TOP_luxc1,
			  TOP_lw,
			  TOP_lwc1,
			  TOP_lwu,
			  TOP_lwx,
			  TOP_lwxc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (3);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_MP, 0);

  Instruction_Group ("Pipem__Rep01__Lat04",
			  TOP_lb,
			  TOP_lbu,
			  TOP_lbx,
			  TOP_lh,
			  TOP_lhu,
			  TOP_lhx,
			  TOP_sc,
			  TOP_scd,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_MP, 0);

  Instruction_Group ("Pipem__Rep01__Lat06",
			  TOP_ldl,
			  TOP_ldr,
			  TOP_lwl,
			  TOP_lwr,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (6);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_MP, 0);

  Instruction_Group ("Pipemc__Rep03__Lat01",
		     // TOP_cache,
		     // TOP_synci,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_MP, 0);
  Resource_Requirement (res_MC, 0);
  Resource_Requirement (res_MC, 1);
  Resource_Requirement (res_MC, 2);

  Instruction_Group ("Pipemc__Rep04__Lat01",
		     // TOP_tlbp,
		     // TOP_tlbr,
		     // TOP_tlbwi,
			  TOP_tlbwir,
		     // TOP_tlbwr,
			  TOP_tlbwrr,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_MP, 0);
  Resource_Requirement (res_MC, 0);
  Resource_Requirement (res_MC, 1);
  Resource_Requirement (res_MC, 2);
  Resource_Requirement (res_MC, 3);

    /* Dummy instructions */

  Instruction_Group ("Dummy",
			  TOP_asm,
			  TOP_intrncall,
			  TOP_spadjust,
			  TOP_begin_pregtn,
			  TOP_end_pregtn,
			  TOP_bwd_bar,
			  TOP_fwd_bar,
			  TOP_label,
			  TOP_nop,
			  TOP_noop,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(1);
  Resource_Requirement(res_Issue, 0);

    /* Fictitious instructions requiring no resource and zero time to execute.
     */

  Instruction_Group ("Dummy_no_res",
			  TOP_mov_hilo,
			  TOP_mov_fcc,
                          TOP_swp_start,
                          TOP_swp_stop,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(0);

  Machine_Done("twc9a.c");
}
