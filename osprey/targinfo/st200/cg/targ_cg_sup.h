/* -- This file is automatically generated -- */ 
/* 

  Copyright (C) 2002, 2004 ST Microelectronics, Inc.  All Rights Reserved. 

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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/ 

// AUTOMATICALLY GENERATED FROM MDS DATA BASE !!! 
/* ====================================================================
 * ====================================================================
 *
 * Header: cg_targsup.h
 *
 * Description:
 *
 * Supplementary routines for use by $target/cg/targ_cg.cxx
 *
 * ====================================================================
 * ====================================================================
 */

static TOP TOP_To_Multi_Table[TOP_count+1] = {
  TOP_addcg,
  TOP_addf_n,
  TOP_addpc_i,
  TOP_addpc_ii,
  TOP_add_r,
  TOP_add_i,
  TOP_add_ii,
  TOP_spadjust,
  TOP_andc_r,
  TOP_andc_i,
  TOP_andc_ii,
  TOP_andl_r_b,
  TOP_andl_i_b,
  TOP_andl_ii_b,
  TOP_andl_r_r,
  TOP_andl_i_r,
  TOP_andl_ii_r,
  TOP_and_r,
  TOP_and_i,
  TOP_and_ii,
  TOP_asm_0,
  TOP_asm_10,
  TOP_asm_11,
  TOP_asm_12,
  TOP_asm_13,
  TOP_asm_14,
  TOP_asm_15,
  TOP_asm_16_i,
  TOP_asm_16_ii,
  TOP_asm_17_i,
  TOP_asm_17_ii,
  TOP_asm_18_i,
  TOP_asm_18_ii,
  TOP_asm_19_i,
  TOP_asm_19_ii,
  TOP_asm_1,
  TOP_asm_20_i,
  TOP_asm_20_ii,
  TOP_asm_21_i,
  TOP_asm_21_ii,
  TOP_asm_22_i,
  TOP_asm_22_ii,
  TOP_asm_23_i,
  TOP_asm_23_ii,
  TOP_asm_24_i,
  TOP_asm_24_ii,
  TOP_asm_25_i,
  TOP_asm_25_ii,
  TOP_asm_26_i,
  TOP_asm_26_ii,
  TOP_asm_27_i,
  TOP_asm_27_ii,
  TOP_asm_28_i,
  TOP_asm_28_ii,
  TOP_asm_29_i,
  TOP_asm_29_ii,
  TOP_asm_2,
  TOP_asm_30_i,
  TOP_asm_30_ii,
  TOP_asm_31_i,
  TOP_asm_31_ii,
  TOP_asm_3,
  TOP_asm_4,
  TOP_asm_5,
  TOP_asm_6,
  TOP_asm_7,
  TOP_asm_8,
  TOP_asm_9,
  TOP_begin_pregtn,
  TOP_break,
  TOP_brf,
  TOP_br,
  TOP_bswap,
  TOP_bwd_bar,
  TOP_CALL,
  TOP_call,
  TOP_clz,
  TOP_cmpeqf_n_b,
  TOP_cmpeqf_n_r,
  TOP_cmpeq_r_b,
  TOP_cmpeq_i_b,
  TOP_cmpeq_ii_b,
  TOP_cmpeq_r_r,
  TOP_cmpeq_i_r,
  TOP_cmpeq_ii_r,
  TOP_cmpgef_n_b,
  TOP_cmpgef_n_r,
  TOP_cmpgeu_r_b,
  TOP_cmpgeu_i_b,
  TOP_cmpgeu_ii_b,
  TOP_cmpgeu_r_r,
  TOP_cmpgeu_i_r,
  TOP_cmpgeu_ii_r,
  TOP_cmpge_r_b,
  TOP_cmpge_i_b,
  TOP_cmpge_ii_b,
  TOP_cmpge_r_r,
  TOP_cmpge_i_r,
  TOP_cmpge_ii_r,
  TOP_cmpgtf_n_b,
  TOP_cmpgtf_n_r,
  TOP_cmpgtu_r_b,
  TOP_cmpgtu_i_b,
  TOP_cmpgtu_ii_b,
  TOP_cmpgtu_r_r,
  TOP_cmpgtu_i_r,
  TOP_cmpgtu_ii_r,
  TOP_cmpgt_r_b,
  TOP_cmpgt_i_b,
  TOP_cmpgt_ii_b,
  TOP_cmpgt_r_r,
  TOP_cmpgt_i_r,
  TOP_cmpgt_ii_r,
  TOP_cmplef_n_b,
  TOP_cmplef_n_r,
  TOP_cmpleu_r_b,
  TOP_cmpleu_i_b,
  TOP_cmpleu_ii_b,
  TOP_cmpleu_r_r,
  TOP_cmpleu_i_r,
  TOP_cmpleu_ii_r,
  TOP_cmple_r_b,
  TOP_cmple_i_b,
  TOP_cmple_ii_b,
  TOP_cmple_r_r,
  TOP_cmple_i_r,
  TOP_cmple_ii_r,
  TOP_cmpltf_n_b,
  TOP_cmpltf_n_r,
  TOP_cmpltu_r_b,
  TOP_cmpltu_i_b,
  TOP_cmpltu_ii_b,
  TOP_cmpltu_r_r,
  TOP_cmpltu_i_r,
  TOP_cmpltu_ii_r,
  TOP_cmplt_r_b,
  TOP_cmplt_i_b,
  TOP_cmplt_ii_b,
  TOP_cmplt_r_r,
  TOP_cmplt_i_r,
  TOP_cmplt_ii_r,
  TOP_cmpne_r_b,
  TOP_cmpne_i_b,
  TOP_cmpne_ii_b,
  TOP_cmpne_r_r,
  TOP_cmpne_i_r,
  TOP_cmpne_ii_r,
  TOP_composep,
  TOP_convfi_n,
  TOP_convif_n,
  TOP_COPY,
  TOP_divs,
  TOP_divu,
  TOP_div,
  TOP_end_pregtn,
  TOP_extractp,
  TOP_FALL,
  TOP_fwd_bar,
  TOP_getpc,
  TOP_asm,
  TOP_GOTO,
  TOP_goto,
  TOP_icall,
  TOP_idle,
  TOP_ifixup,
  TOP_igoto,
  TOP_intrncall,
  TOP_JUMP,
  TOP_KILL,
  TOP_label,
  TOP_ldbc_i,
  TOP_ldbc_ii,
  TOP_ldbuc_i,
  TOP_ldbuc_ii,
  TOP_ldbu_d_i,
  TOP_ldbu_d_ii,
  TOP_ldbu_i,
  TOP_ldbu_ii,
  TOP_ldb_d_i,
  TOP_ldb_d_ii,
  TOP_ldb_i,
  TOP_ldb_ii,
  TOP_ldhc_i,
  TOP_ldhc_ii,
  TOP_ldhuc_i,
  TOP_ldhuc_ii,
  TOP_ldhu_d_i,
  TOP_ldhu_d_ii,
  TOP_ldhu_i,
  TOP_ldhu_ii,
  TOP_ldh_d_i,
  TOP_ldh_d_ii,
  TOP_ldh_i,
  TOP_ldh_ii,
  TOP_multi_ldlc_i,
  TOP_multi_ldlc_ii,
  TOP_multi_ldl_i,
  TOP_multi_ldl_ii,
  TOP_ldwc_i,
  TOP_ldwc_ii,
  TOP_ldwl,
  TOP_ldw_d_i,
  TOP_ldw_d_ii,
  TOP_ldw_i,
  TOP_ldw_ii,
  TOP_LINK,
  TOP_LOOP,
  TOP_maxu_r,
  TOP_maxu_i,
  TOP_maxu_ii,
  TOP_max_r,
  TOP_max_i,
  TOP_max_ii,
  TOP_mfb,
  TOP_minu_r,
  TOP_minu_i,
  TOP_minu_ii,
  TOP_min_r,
  TOP_min_i,
  TOP_min_ii,
  TOP_movc,
  TOP_movcf,
  TOP_movp,
  TOP_mov_r,
  TOP_mov_i,
  TOP_mov_ii,
  TOP_mtb,
  TOP_mul32_r,
  TOP_mul32_i,
  TOP_mul32_ii,
  TOP_mul64hu_r,
  TOP_mul64hu_i,
  TOP_mul64hu_ii,
  TOP_mul64h_r,
  TOP_mul64h_i,
  TOP_mul64h_ii,
  TOP_mulfrac_r,
  TOP_mulfrac_i,
  TOP_mulfrac_ii,
  TOP_mulf_n,
  TOP_mulhhs_r,
  TOP_mulhhs_i,
  TOP_mulhhs_ii,
  TOP_mulhhu_r,
  TOP_mulhhu_i,
  TOP_mulhhu_ii,
  TOP_mulhh_r,
  TOP_mulhh_i,
  TOP_mulhh_ii,
  TOP_mulhs_r,
  TOP_mulhs_i,
  TOP_mulhs_ii,
  TOP_mulhu_r,
  TOP_mulhu_i,
  TOP_mulhu_ii,
  TOP_mulh_r,
  TOP_mulh_i,
  TOP_mulh_ii,
  TOP_mullhus_r,
  TOP_mullhus_i,
  TOP_mullhus_ii,
  TOP_mullhu_r,
  TOP_mullhu_i,
  TOP_mullhu_ii,
  TOP_mullh_r,
  TOP_mullh_i,
  TOP_mullh_ii,
  TOP_mulllu_r,
  TOP_mulllu_i,
  TOP_mulllu_ii,
  TOP_mulll_r,
  TOP_mulll_i,
  TOP_mulll_ii,
  TOP_mullu_r,
  TOP_mullu_i,
  TOP_mullu_ii,
  TOP_mull_r,
  TOP_mull_i,
  TOP_mull_ii,
  TOP_multi_ldlc_i,
  TOP_multi_ldlc_ii,
  TOP_multi_ldl_i,
  TOP_multi_ldl_ii,
  TOP_multi_stlc_i,
  TOP_multi_stlc_ii,
  TOP_multi_stl_i,
  TOP_multi_stl_ii,
  TOP_nandl_r_b,
  TOP_nandl_i_b,
  TOP_nandl_ii_b,
  TOP_nandl_r_r,
  TOP_nandl_i_r,
  TOP_nandl_ii_r,
  TOP_noop,
  TOP_nop,
  TOP_norl_r_b,
  TOP_norl_i_b,
  TOP_norl_ii_b,
  TOP_norl_r_r,
  TOP_norl_i_r,
  TOP_norl_ii_r,
  TOP_orc_r,
  TOP_orc_i,
  TOP_orc_ii,
  TOP_orl_r_b,
  TOP_orl_i_b,
  TOP_orl_ii_b,
  TOP_orl_r_r,
  TOP_orl_i_r,
  TOP_orl_ii_r,
  TOP_or_r,
  TOP_or_i,
  TOP_or_ii,
  TOP_pftc_i,
  TOP_pftc_ii,
  TOP_pft_i,
  TOP_pft_ii,
  TOP_phi,
  TOP_prgadd_i,
  TOP_prgadd_ii,
  TOP_prgins,
  TOP_prginsadd_i,
  TOP_prginsadd_ii,
  TOP_prginspg_i,
  TOP_prginspg_ii,
  TOP_prgset_i,
  TOP_prgset_ii,
  TOP_psi,
  TOP_pswclr,
  TOP_pswset,
  TOP_pushregs,
  TOP_remu,
  TOP_rem,
  TOP_RETURN,
  TOP_return,
  TOP_rfi,
  TOP_sbrk,
  TOP_st235_sbrk,
  TOP_sh1add_r,
  TOP_sh1add_i,
  TOP_sh1add_ii,
  TOP_sh2add_r,
  TOP_sh2add_i,
  TOP_sh2add_ii,
  TOP_sh3add_r,
  TOP_sh3add_i,
  TOP_sh3add_ii,
  TOP_sh4add_r,
  TOP_sh4add_i,
  TOP_sh4add_ii,
  TOP_shl_r,
  TOP_shl_i,
  TOP_shl_ii,
  TOP_shru_r,
  TOP_shru_i,
  TOP_shru_ii,
  TOP_shr_r,
  TOP_shr_i,
  TOP_shr_ii,
  TOP_SIGMA,
  TOP_slctf_r,
  TOP_slctf_i,
  TOP_slctf_ii,
  TOP_slct_r,
  TOP_slct_i,
  TOP_slct_ii,
  TOP_stbc_i,
  TOP_stbc_ii,
  TOP_stb_i,
  TOP_stb_ii,
  TOP_sthc_i,
  TOP_sthc_ii,
  TOP_sth_i,
  TOP_sth_ii,
  TOP_multi_stlc_i,
  TOP_multi_stlc_ii,
  TOP_multi_stl_i,
  TOP_multi_stl_ii,
  TOP_stwc_i,
  TOP_stwc_ii,
  TOP_stwl,
  TOP_stw_i,
  TOP_stw_ii,
  TOP_subf_n,
  TOP_sub_r,
  TOP_sub_i,
  TOP_sub_ii,
  TOP_sxtb,
  TOP_sxth,
  TOP_sync,
  TOP_syncins,
  TOP_syscall,
  TOP_st235_syscall,
  TOP_wmb,
  TOP_xor_r,
  TOP_xor_i,
  TOP_xor_ii,
  TOP_zxtb,
  TOP_zxth
};

static TOP TOP_From_Multi_Table[TOP_count+1] = {
  TOP_addcg,
  TOP_addf_n,
  TOP_addpc_i,
  TOP_addpc_ii,
  TOP_add_r,
  TOP_add_i,
  TOP_add_ii,
  TOP_spadjust,
  TOP_andc_r,
  TOP_andc_i,
  TOP_andc_ii,
  TOP_andl_r_b,
  TOP_andl_i_b,
  TOP_andl_ii_b,
  TOP_andl_r_r,
  TOP_andl_i_r,
  TOP_andl_ii_r,
  TOP_and_r,
  TOP_and_i,
  TOP_and_ii,
  TOP_asm_0,
  TOP_asm_10,
  TOP_asm_11,
  TOP_asm_12,
  TOP_asm_13,
  TOP_asm_14,
  TOP_asm_15,
  TOP_asm_16_i,
  TOP_asm_16_ii,
  TOP_asm_17_i,
  TOP_asm_17_ii,
  TOP_asm_18_i,
  TOP_asm_18_ii,
  TOP_asm_19_i,
  TOP_asm_19_ii,
  TOP_asm_1,
  TOP_asm_20_i,
  TOP_asm_20_ii,
  TOP_asm_21_i,
  TOP_asm_21_ii,
  TOP_asm_22_i,
  TOP_asm_22_ii,
  TOP_asm_23_i,
  TOP_asm_23_ii,
  TOP_asm_24_i,
  TOP_asm_24_ii,
  TOP_asm_25_i,
  TOP_asm_25_ii,
  TOP_asm_26_i,
  TOP_asm_26_ii,
  TOP_asm_27_i,
  TOP_asm_27_ii,
  TOP_asm_28_i,
  TOP_asm_28_ii,
  TOP_asm_29_i,
  TOP_asm_29_ii,
  TOP_asm_2,
  TOP_asm_30_i,
  TOP_asm_30_ii,
  TOP_asm_31_i,
  TOP_asm_31_ii,
  TOP_asm_3,
  TOP_asm_4,
  TOP_asm_5,
  TOP_asm_6,
  TOP_asm_7,
  TOP_asm_8,
  TOP_asm_9,
  TOP_begin_pregtn,
  TOP_break,
  TOP_brf,
  TOP_br,
  TOP_bswap,
  TOP_bwd_bar,
  TOP_CALL,
  TOP_call,
  TOP_clz,
  TOP_cmpeqf_n_b,
  TOP_cmpeqf_n_r,
  TOP_cmpeq_r_b,
  TOP_cmpeq_i_b,
  TOP_cmpeq_ii_b,
  TOP_cmpeq_r_r,
  TOP_cmpeq_i_r,
  TOP_cmpeq_ii_r,
  TOP_cmpgef_n_b,
  TOP_cmpgef_n_r,
  TOP_cmpgeu_r_b,
  TOP_cmpgeu_i_b,
  TOP_cmpgeu_ii_b,
  TOP_cmpgeu_r_r,
  TOP_cmpgeu_i_r,
  TOP_cmpgeu_ii_r,
  TOP_cmpge_r_b,
  TOP_cmpge_i_b,
  TOP_cmpge_ii_b,
  TOP_cmpge_r_r,
  TOP_cmpge_i_r,
  TOP_cmpge_ii_r,
  TOP_cmpgtf_n_b,
  TOP_cmpgtf_n_r,
  TOP_cmpgtu_r_b,
  TOP_cmpgtu_i_b,
  TOP_cmpgtu_ii_b,
  TOP_cmpgtu_r_r,
  TOP_cmpgtu_i_r,
  TOP_cmpgtu_ii_r,
  TOP_cmpgt_r_b,
  TOP_cmpgt_i_b,
  TOP_cmpgt_ii_b,
  TOP_cmpgt_r_r,
  TOP_cmpgt_i_r,
  TOP_cmpgt_ii_r,
  TOP_cmplef_n_b,
  TOP_cmplef_n_r,
  TOP_cmpleu_r_b,
  TOP_cmpleu_i_b,
  TOP_cmpleu_ii_b,
  TOP_cmpleu_r_r,
  TOP_cmpleu_i_r,
  TOP_cmpleu_ii_r,
  TOP_cmple_r_b,
  TOP_cmple_i_b,
  TOP_cmple_ii_b,
  TOP_cmple_r_r,
  TOP_cmple_i_r,
  TOP_cmple_ii_r,
  TOP_cmpltf_n_b,
  TOP_cmpltf_n_r,
  TOP_cmpltu_r_b,
  TOP_cmpltu_i_b,
  TOP_cmpltu_ii_b,
  TOP_cmpltu_r_r,
  TOP_cmpltu_i_r,
  TOP_cmpltu_ii_r,
  TOP_cmplt_r_b,
  TOP_cmplt_i_b,
  TOP_cmplt_ii_b,
  TOP_cmplt_r_r,
  TOP_cmplt_i_r,
  TOP_cmplt_ii_r,
  TOP_cmpne_r_b,
  TOP_cmpne_i_b,
  TOP_cmpne_ii_b,
  TOP_cmpne_r_r,
  TOP_cmpne_i_r,
  TOP_cmpne_ii_r,
  TOP_composep,
  TOP_convfi_n,
  TOP_convif_n,
  TOP_COPY,
  TOP_divs,
  TOP_divu,
  TOP_div,
  TOP_end_pregtn,
  TOP_extractp,
  TOP_FALL,
  TOP_fwd_bar,
  TOP_getpc,
  TOP_asm,
  TOP_GOTO,
  TOP_goto,
  TOP_icall,
  TOP_idle,
  TOP_ifixup,
  TOP_igoto,
  TOP_intrncall,
  TOP_JUMP,
  TOP_KILL,
  TOP_label,
  TOP_ldbc_i,
  TOP_ldbc_ii,
  TOP_ldbuc_i,
  TOP_ldbuc_ii,
  TOP_ldbu_d_i,
  TOP_ldbu_d_ii,
  TOP_ldbu_i,
  TOP_ldbu_ii,
  TOP_ldb_d_i,
  TOP_ldb_d_ii,
  TOP_ldb_i,
  TOP_ldb_ii,
  TOP_ldhc_i,
  TOP_ldhc_ii,
  TOP_ldhuc_i,
  TOP_ldhuc_ii,
  TOP_ldhu_d_i,
  TOP_ldhu_d_ii,
  TOP_ldhu_i,
  TOP_ldhu_ii,
  TOP_ldh_d_i,
  TOP_ldh_d_ii,
  TOP_ldh_i,
  TOP_ldh_ii,
  TOP_ldlc_i,
  TOP_ldlc_ii,
  TOP_ldl_i,
  TOP_ldl_ii,
  TOP_ldwc_i,
  TOP_ldwc_ii,
  TOP_ldwl,
  TOP_ldw_d_i,
  TOP_ldw_d_ii,
  TOP_ldw_i,
  TOP_ldw_ii,
  TOP_LINK,
  TOP_LOOP,
  TOP_maxu_r,
  TOP_maxu_i,
  TOP_maxu_ii,
  TOP_max_r,
  TOP_max_i,
  TOP_max_ii,
  TOP_mfb,
  TOP_minu_r,
  TOP_minu_i,
  TOP_minu_ii,
  TOP_min_r,
  TOP_min_i,
  TOP_min_ii,
  TOP_movc,
  TOP_movcf,
  TOP_movp,
  TOP_mov_r,
  TOP_mov_i,
  TOP_mov_ii,
  TOP_mtb,
  TOP_mul32_r,
  TOP_mul32_i,
  TOP_mul32_ii,
  TOP_mul64hu_r,
  TOP_mul64hu_i,
  TOP_mul64hu_ii,
  TOP_mul64h_r,
  TOP_mul64h_i,
  TOP_mul64h_ii,
  TOP_mulfrac_r,
  TOP_mulfrac_i,
  TOP_mulfrac_ii,
  TOP_mulf_n,
  TOP_mulhhs_r,
  TOP_mulhhs_i,
  TOP_mulhhs_ii,
  TOP_mulhhu_r,
  TOP_mulhhu_i,
  TOP_mulhhu_ii,
  TOP_mulhh_r,
  TOP_mulhh_i,
  TOP_mulhh_ii,
  TOP_mulhs_r,
  TOP_mulhs_i,
  TOP_mulhs_ii,
  TOP_mulhu_r,
  TOP_mulhu_i,
  TOP_mulhu_ii,
  TOP_mulh_r,
  TOP_mulh_i,
  TOP_mulh_ii,
  TOP_mullhus_r,
  TOP_mullhus_i,
  TOP_mullhus_ii,
  TOP_mullhu_r,
  TOP_mullhu_i,
  TOP_mullhu_ii,
  TOP_mullh_r,
  TOP_mullh_i,
  TOP_mullh_ii,
  TOP_mulllu_r,
  TOP_mulllu_i,
  TOP_mulllu_ii,
  TOP_mulll_r,
  TOP_mulll_i,
  TOP_mulll_ii,
  TOP_mullu_r,
  TOP_mullu_i,
  TOP_mullu_ii,
  TOP_mull_r,
  TOP_mull_i,
  TOP_mull_ii,
  TOP_ldlc_i,
  TOP_ldlc_ii,
  TOP_ldl_i,
  TOP_ldl_ii,
  TOP_stlc_i,
  TOP_stlc_ii,
  TOP_stl_i,
  TOP_stl_ii,
  TOP_nandl_r_b,
  TOP_nandl_i_b,
  TOP_nandl_ii_b,
  TOP_nandl_r_r,
  TOP_nandl_i_r,
  TOP_nandl_ii_r,
  TOP_noop,
  TOP_nop,
  TOP_norl_r_b,
  TOP_norl_i_b,
  TOP_norl_ii_b,
  TOP_norl_r_r,
  TOP_norl_i_r,
  TOP_norl_ii_r,
  TOP_orc_r,
  TOP_orc_i,
  TOP_orc_ii,
  TOP_orl_r_b,
  TOP_orl_i_b,
  TOP_orl_ii_b,
  TOP_orl_r_r,
  TOP_orl_i_r,
  TOP_orl_ii_r,
  TOP_or_r,
  TOP_or_i,
  TOP_or_ii,
  TOP_pftc_i,
  TOP_pftc_ii,
  TOP_pft_i,
  TOP_pft_ii,
  TOP_phi,
  TOP_prgadd_i,
  TOP_prgadd_ii,
  TOP_prgins,
  TOP_prginsadd_i,
  TOP_prginsadd_ii,
  TOP_prginspg_i,
  TOP_prginspg_ii,
  TOP_prgset_i,
  TOP_prgset_ii,
  TOP_psi,
  TOP_pswclr,
  TOP_pswset,
  TOP_pushregs,
  TOP_remu,
  TOP_rem,
  TOP_RETURN,
  TOP_return,
  TOP_rfi,
  TOP_sbrk,
  TOP_st235_sbrk,
  TOP_sh1add_r,
  TOP_sh1add_i,
  TOP_sh1add_ii,
  TOP_sh2add_r,
  TOP_sh2add_i,
  TOP_sh2add_ii,
  TOP_sh3add_r,
  TOP_sh3add_i,
  TOP_sh3add_ii,
  TOP_sh4add_r,
  TOP_sh4add_i,
  TOP_sh4add_ii,
  TOP_shl_r,
  TOP_shl_i,
  TOP_shl_ii,
  TOP_shru_r,
  TOP_shru_i,
  TOP_shru_ii,
  TOP_shr_r,
  TOP_shr_i,
  TOP_shr_ii,
  TOP_SIGMA,
  TOP_slctf_r,
  TOP_slctf_i,
  TOP_slctf_ii,
  TOP_slct_r,
  TOP_slct_i,
  TOP_slct_ii,
  TOP_stbc_i,
  TOP_stbc_ii,
  TOP_stb_i,
  TOP_stb_ii,
  TOP_sthc_i,
  TOP_sthc_ii,
  TOP_sth_i,
  TOP_sth_ii,
  TOP_stlc_i,
  TOP_stlc_ii,
  TOP_stl_i,
  TOP_stl_ii,
  TOP_stwc_i,
  TOP_stwc_ii,
  TOP_stwl,
  TOP_stw_i,
  TOP_stw_ii,
  TOP_subf_n,
  TOP_sub_r,
  TOP_sub_i,
  TOP_sub_ii,
  TOP_sxtb,
  TOP_sxth,
  TOP_sync,
  TOP_syncins,
  TOP_syscall,
  TOP_st235_syscall,
  TOP_wmb,
  TOP_xor_r,
  TOP_xor_i,
  TOP_xor_ii,
  TOP_zxtb,
  TOP_zxth
};

TOP
CGTARG_TOP_To_Multi (
  TOP top
)
{
    return TOP_To_Multi_Table[top];
}

TOP
CGTARG_TOP_From_Multi (
  TOP top
)
{
    return TOP_From_Multi_Table[top];
}

