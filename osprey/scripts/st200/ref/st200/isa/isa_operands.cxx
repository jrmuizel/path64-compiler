//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! 
// Group TOPS with similar operands/results format. 
///////////////////////////////////////////////////////// 
// Within each category, the instructions are arranged roughly in order 
// of increasing numbers of operands. 
///////////////////////////////////// 


#include <stddef.h> 
#include "topcode.h" 
#include "isa_operands_gen.h" 

main() 
{ 

  OPERAND_VALUE_TYPE bcond, scond, bdest, ibdest, dest, dest2, idest, idest2, src1, src2, lr; 
  OPERAND_VALUE_TYPE btarg, isrc2, s9, imm, pcrel; 

  /* ------------------------------------------------------
   *   Following built-in use types must be specified: 
   *     1. base operand use for TOP_load, TOP_store; 
   *     2. offset operand use for TOP_load, TOP_store; 
   *     3. storeval operand use for TOP_store; 
   *     4. condition operand use for TOP_select; 
   * 
   *   Following built-in use types may be specified: 
   *     5. implicit operand use for TOPs when the operand is implicitely used; 
   * 
   *   Here you can specify any additional operand uses. 
   * ------------------------------------------------------
   */
  OPERAND_USE_TYPE 
        base, 
        offset, 
        storeval, 
        condition, 
        predicate, 
        target, 
        postincr, 
        uniq_res, 
        opnd1, 
        opnd2; 

  ISA_Operands_Begin("st200"); 
  /* Create the register operand types: */ 
  bcond = ISA_Reg_Opnd_Type_Create("bcond", 
                ISA_REGISTER_CLASS_branch, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                1, UNSIGNED, INVALID); 
  scond = ISA_Reg_Opnd_Type_Create("scond", 
                ISA_REGISTER_CLASS_branch, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                1, UNSIGNED, INVALID); 
  bdest = ISA_Reg_Opnd_Type_Create("bdest", 
                ISA_REGISTER_CLASS_branch, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                1, UNSIGNED, INVALID); 
  ibdest = ISA_Reg_Opnd_Type_Create("ibdest", 
                ISA_REGISTER_CLASS_branch, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                1, UNSIGNED, INVALID); 
  dest = ISA_Reg_Opnd_Type_Create("dest", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                32, SIGNED, INVALID); 
  dest2 = ISA_Reg_Opnd_Type_Create("dest2", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_no_lr, 
                32, SIGNED, INVALID); 
  idest = ISA_Reg_Opnd_Type_Create("idest", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                32, SIGNED, INVALID); 
  idest2 = ISA_Reg_Opnd_Type_Create("idest2", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_no_lr, 
                32, SIGNED, INVALID); 
  src1 = ISA_Reg_Opnd_Type_Create("src1", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                32, SIGNED, INVALID); 
  src2 = ISA_Reg_Opnd_Type_Create("src2", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_UNDEFINED, 
                32, SIGNED, INVALID); 
  lr = ISA_Reg_Opnd_Type_Create("lr", 
                ISA_REGISTER_CLASS_integer, 
                ISA_REGISTER_SUBCLASS_lr, 
                32, UNSIGNED, INVALID); 

  /* Create the literal operand types: */ 
  btarg = ISA_Lit_Opnd_Type_Create("btarg", 23, SIGNED, LC_s23); 
  isrc2 = ISA_Lit_Opnd_Type_Create("isrc2", 32, SIGNED, LC_s32); 
  s9 = ISA_Lit_Opnd_Type_Create("s9", 9, SIGNED, LC_s9); 
  imm = ISA_Lit_Opnd_Type_Create("imm", 23, SIGNED, LC_s23); 
  pcrel = ISA_Lit_Opnd_Type_Create("pcrel", 32, PCREL, LC_s32); 


  /* Create the operand uses: */ 

  base = Create_Operand_Use("base"); 
  offset = Create_Operand_Use("offset"); 
  storeval = Create_Operand_Use("storeval"); 
  condition = Create_Operand_Use("condition"); 
  predicate = Create_Operand_Use("predicate"); 
  target = Create_Operand_Use("target"); 
  postincr = Create_Operand_Use("postincr"); 
  uniq_res = Create_Operand_Use("uniq_res"); 
  opnd1 = Create_Operand_Use("opnd1"); 
  opnd2 = Create_Operand_Use("opnd2"); 

  /* ====================================== */ 
  Instruction_Group("O_Branch", 
		 TOP_br, 
		 TOP_brf, 
		 TOP_UNDEFINED); 

  Operand (0, bcond, condition); 
  Operand (1, btarg, target); 

  /* ====================================== */ 
  Instruction_Group("O_Imm", 
		 TOP_imml, 
		 TOP_immr, 
		 TOP_UNDEFINED); 

  Operand (0, imm); 

  /* ====================================== */ 
  Instruction_Group("O_Store", 
		 TOP_stb_i, 
		 TOP_stb_ii, 
		 TOP_sth_i, 
		 TOP_sth_ii, 
		 TOP_stw_i, 
		 TOP_stw_ii, 
		 TOP_prginspg_i, 
		 TOP_prginspg_ii, 
		 TOP_pswset_i, 
		 TOP_pswset_ii, 
		 TOP_pswclr_i, 
		 TOP_pswclr_ii, 
		 TOP_UNDEFINED); 

  Operand (0, isrc2, offset); 
  Operand (1, src1, base); 
  Operand (2, src2, storeval); 

  /* ====================================== */ 
  Instruction_Group("O_Sync", 
		 TOP_sync, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_SysOp", 
		 TOP_break, 
		 TOP_prgins, 
		 TOP_sbrk, 
		 TOP_syscall, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_asm", 
		 TOP_asm_0, 
		 TOP_asm_1, 
		 TOP_asm_2, 
		 TOP_asm_3, 
		 TOP_asm_4, 
		 TOP_asm_5, 
		 TOP_asm_6, 
		 TOP_asm_7, 
		 TOP_asm_8, 
		 TOP_asm_9, 
		 TOP_asm_10, 
		 TOP_asm_11, 
		 TOP_asm_12, 
		 TOP_asm_13, 
		 TOP_asm_14, 
		 TOP_asm_15, 
		 TOP_asm_16, 
		 TOP_asm_17, 
		 TOP_asm_18, 
		 TOP_asm_19, 
		 TOP_asm_20, 
		 TOP_asm_21, 
		 TOP_asm_22, 
		 TOP_asm_23, 
		 TOP_asm_24, 
		 TOP_asm_25, 
		 TOP_asm_26, 
		 TOP_asm_27, 
		 TOP_asm_28, 
		 TOP_asm_29, 
		 TOP_asm_30, 
		 TOP_asm_31, 
		 TOP_asm, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_barrier", 
		 TOP_bwd_bar, 
		 TOP_fwd_bar, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_cache", 
		 TOP_pft_i, 
		 TOP_pft_ii, 
		 TOP_prgadd_i, 
		 TOP_prgadd_ii, 
		 TOP_prgset_i, 
		 TOP_prgset_ii, 
		 TOP_UNDEFINED); 

  Operand (0, isrc2, offset); 
  Operand (1, src1, base); 

  /* ====================================== */ 
  Instruction_Group("O_ijump", 
		 TOP_igoto, 
		 TOP_return, 
		 TOP_UNDEFINED); 

  Operand (0, lr); 

  /* ====================================== */ 
  Instruction_Group("O_intrncall", 
		 TOP_intrncall, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_jump", 
		 TOP_goto, 
		 TOP_UNDEFINED); 

  Operand (0, btarg, target); 

  /* ====================================== */ 
  Instruction_Group("O_label", 
		 TOP_label, 
		 TOP_UNDEFINED); 

  Operand (0, pcrel); 

  /* ====================================== */ 
  Instruction_Group("O_noop", 
		 TOP_noop, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_nop", 
		 TOP_nop, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_pregtn", 
		 TOP_begin_pregtn, 
		 TOP_end_pregtn, 
		 TOP_UNDEFINED); 

  Operand (0, src1); 
  Operand (1, isrc2); 

  /* ====================================== */ 
  Instruction_Group("O_rfi", 
		 TOP_rfi, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_ssa", 
		 TOP_phi, 
		 TOP_psi, 
		 TOP_UNDEFINED); 


  /* ====================================== */ 
  Instruction_Group("O_Cmp3R_Br", 
		 TOP_andl_r_b, 
		 TOP_cmpeq_r_b, 
		 TOP_cmpge_r_b, 
		 TOP_cmpgeu_r_b, 
		 TOP_cmpgt_r_b, 
		 TOP_cmpgtu_r_b, 
		 TOP_cmple_r_b, 
		 TOP_cmpleu_r_b, 
		 TOP_cmplt_r_b, 
		 TOP_cmpltu_r_b, 
		 TOP_cmpne_r_b, 
		 TOP_nandl_r_b, 
		 TOP_norl_r_b, 
		 TOP_orl_r_b, 
		 TOP_UNDEFINED); 

  Result (0, bdest); 
  Operand (0, src1, opnd1); 
  Operand (1, src2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_mtb", 
		 TOP_mtb, 
		 TOP_UNDEFINED); 

  Result (0, bdest); 
  Operand (0, src1); 

  /* ====================================== */ 
  Instruction_Group("O_cgen", 
		 TOP_addcg, 
		 TOP_divs, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Result (1, bdest); 
  Operand (0, src1); 
  Operand (1, src2); 
  Operand (2, bcond); 

  /* ====================================== */ 
  Instruction_Group("O_Int3R", 
		 TOP_mulh_r, 
		 TOP_mulhh_r, 
		 TOP_mulhhs_r, 
		 TOP_mulhhu_r, 
		 TOP_mulhs_r, 
		 TOP_mulhu_r, 
		 TOP_mull_r, 
		 TOP_mullh_r, 
		 TOP_mullhu_r, 
		 TOP_mullhus_r, 
		 TOP_mulll_r, 
		 TOP_mulllu_r, 
		 TOP_mullu_r, 
		 TOP_UNDEFINED); 

  Result (0, dest2); 
  Operand (0, src1, opnd1); 
  Operand (1, src2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Load", 
		 TOP_ldb_i, 
		 TOP_ldb_ii, 
		 TOP_ldb_d_i, 
		 TOP_ldb_d_ii, 
		 TOP_ldbu_i, 
		 TOP_ldbu_ii, 
		 TOP_ldbu_d_i, 
		 TOP_ldbu_d_ii, 
		 TOP_ldh_i, 
		 TOP_ldh_ii, 
		 TOP_ldh_d_i, 
		 TOP_ldh_d_ii, 
		 TOP_ldhu_i, 
		 TOP_ldhu_ii, 
		 TOP_ldhu_d_i, 
		 TOP_ldhu_d_ii, 
		 TOP_UNDEFINED); 

  Result (0, dest2); 
  Operand (0, isrc2, offset); 
  Operand (1, src1, base); 

  /* ====================================== */ 
  Instruction_Group("O_Mul64R", 
		 TOP_mul32_r, 
		 TOP_mul64h_r, 
		 TOP_mul64hu_r, 
		 TOP_mulfrac_r, 
		 TOP_UNDEFINED); 

  Result (0, dest2); 
  Operand (0, src1, opnd1); 
  Operand (1, src2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Cmp3R_Reg", 
		 TOP_andl_r_r, 
		 TOP_cmpeq_r_r, 
		 TOP_cmpge_r_r, 
		 TOP_cmpgeu_r_r, 
		 TOP_cmpgt_r_r, 
		 TOP_cmpgtu_r_r, 
		 TOP_cmple_r_r, 
		 TOP_cmpleu_r_r, 
		 TOP_cmplt_r_r, 
		 TOP_cmpltu_r_r, 
		 TOP_cmpne_r_r, 
		 TOP_nandl_r_r, 
		 TOP_norl_r_r, 
		 TOP_orl_r_r, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Operand (0, src1, opnd1); 
  Operand (1, src2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Int3R", 
		 TOP_add_r, 
		 TOP_and_r, 
		 TOP_andc_r, 
		 TOP_max_r, 
		 TOP_maxu_r, 
		 TOP_min_r, 
		 TOP_minu_r, 
		 TOP_or_r, 
		 TOP_orc_r, 
		 TOP_sh1add_r, 
		 TOP_sh2add_r, 
		 TOP_sh3add_r, 
		 TOP_sh4add_r, 
		 TOP_shl_r, 
		 TOP_shr_r, 
		 TOP_shru_r, 
		 TOP_sub_r, 
		 TOP_xor_r, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Operand (0, src1, opnd1); 
  Operand (1, src2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Load", 
		 TOP_ldw_i, 
		 TOP_ldw_ii, 
		 TOP_ldw_d_i, 
		 TOP_ldw_d_ii, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Operand (0, isrc2, offset); 
  Operand (1, src1, base); 

  /* ====================================== */ 
  Instruction_Group("O_Monadic", 
		 TOP_bswap_r, 
		 TOP_clz_r, 
		 TOP_sxtb_r, 
		 TOP_sxth_r, 
		 TOP_zxth_r, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Operand (0, src2, opnd1); 

  /* ====================================== */ 
  Instruction_Group("O_SelectR", 
		 TOP_slct_r, 
		 TOP_slctf_r, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Operand (0, bcond, condition); 
  Operand (1, src1, opnd1); 
  Operand (2, src2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_fixup", 
		 TOP_dfixup, 
		 TOP_ffixup, 
		 TOP_ifixup, 
		 TOP_UNDEFINED); 

  Result (0, dest); 

  /* ====================================== */ 
  Instruction_Group("O_move", 
		 TOP_mov_r, 
		 TOP_UNDEFINED); 

  Result (0, dest); 
  Operand (0, src2, opnd1); 

  /* ====================================== */ 
  Instruction_Group("O_Cmp3I_Br", 
		 TOP_andl_i_b, 
		 TOP_andl_ii_b, 
		 TOP_cmpeq_i_b, 
		 TOP_cmpeq_ii_b, 
		 TOP_cmpge_i_b, 
		 TOP_cmpge_ii_b, 
		 TOP_cmpgeu_i_b, 
		 TOP_cmpgeu_ii_b, 
		 TOP_cmpgt_i_b, 
		 TOP_cmpgt_ii_b, 
		 TOP_cmpgtu_i_b, 
		 TOP_cmpgtu_ii_b, 
		 TOP_cmple_i_b, 
		 TOP_cmple_ii_b, 
		 TOP_cmpleu_i_b, 
		 TOP_cmpleu_ii_b, 
		 TOP_cmplt_i_b, 
		 TOP_cmplt_ii_b, 
		 TOP_cmpltu_i_b, 
		 TOP_cmpltu_ii_b, 
		 TOP_cmpne_i_b, 
		 TOP_cmpne_ii_b, 
		 TOP_nandl_i_b, 
		 TOP_nandl_ii_b, 
		 TOP_norl_i_b, 
		 TOP_norl_ii_b, 
		 TOP_orl_i_b, 
		 TOP_orl_ii_b, 
		 TOP_UNDEFINED); 

  Result (0, ibdest); 
  Operand (0, src1, opnd1); 
  Operand (1, isrc2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Int3I", 
		 TOP_mulh_i, 
		 TOP_mulh_ii, 
		 TOP_mulhh_i, 
		 TOP_mulhh_ii, 
		 TOP_mulhhs_i, 
		 TOP_mulhhs_ii, 
		 TOP_mulhhu_i, 
		 TOP_mulhhu_ii, 
		 TOP_mulhs_i, 
		 TOP_mulhs_ii, 
		 TOP_mulhu_i, 
		 TOP_mulhu_ii, 
		 TOP_mull_i, 
		 TOP_mull_ii, 
		 TOP_mullh_i, 
		 TOP_mullh_ii, 
		 TOP_mullhu_i, 
		 TOP_mullhu_ii, 
		 TOP_mullhus_i, 
		 TOP_mullhus_ii, 
		 TOP_mulll_i, 
		 TOP_mulll_ii, 
		 TOP_mulllu_i, 
		 TOP_mulllu_ii, 
		 TOP_mullu_i, 
		 TOP_mullu_ii, 
		 TOP_UNDEFINED); 

  Result (0, idest2); 
  Operand (0, src1, opnd1); 
  Operand (1, isrc2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Mul64I", 
		 TOP_mul32_i, 
		 TOP_mul32_ii, 
		 TOP_mul64h_i, 
		 TOP_mul64h_ii, 
		 TOP_mul64hu_i, 
		 TOP_mul64hu_ii, 
		 TOP_mulfrac_i, 
		 TOP_mulfrac_ii, 
		 TOP_UNDEFINED); 

  Result (0, idest2); 
  Operand (0, src1, opnd1); 
  Operand (1, isrc2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Cmp3I_Reg", 
		 TOP_andl_i_r, 
		 TOP_andl_ii_r, 
		 TOP_cmpeq_i_r, 
		 TOP_cmpeq_ii_r, 
		 TOP_cmpge_i_r, 
		 TOP_cmpge_ii_r, 
		 TOP_cmpgeu_i_r, 
		 TOP_cmpgeu_ii_r, 
		 TOP_cmpgt_i_r, 
		 TOP_cmpgt_ii_r, 
		 TOP_cmpgtu_i_r, 
		 TOP_cmpgtu_ii_r, 
		 TOP_cmple_i_r, 
		 TOP_cmple_ii_r, 
		 TOP_cmpleu_i_r, 
		 TOP_cmpleu_ii_r, 
		 TOP_cmplt_i_r, 
		 TOP_cmplt_ii_r, 
		 TOP_cmpltu_i_r, 
		 TOP_cmpltu_ii_r, 
		 TOP_cmpne_i_r, 
		 TOP_cmpne_ii_r, 
		 TOP_nandl_i_r, 
		 TOP_nandl_ii_r, 
		 TOP_norl_i_r, 
		 TOP_norl_ii_r, 
		 TOP_orl_i_r, 
		 TOP_orl_ii_r, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, src1, opnd1); 
  Operand (1, isrc2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Int3I", 
		 TOP_sub_i, 
		 TOP_sub_ii, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, isrc2, opnd1); 
  Operand (1, src1, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_Int3I", 
		 TOP_add_i, 
		 TOP_add_ii, 
		 TOP_and_i, 
		 TOP_and_ii, 
		 TOP_andc_i, 
		 TOP_andc_ii, 
		 TOP_max_i, 
		 TOP_max_ii, 
		 TOP_maxu_i, 
		 TOP_maxu_ii, 
		 TOP_min_i, 
		 TOP_min_ii, 
		 TOP_minu_i, 
		 TOP_minu_ii, 
		 TOP_or_i, 
		 TOP_or_ii, 
		 TOP_orc_i, 
		 TOP_orc_ii, 
		 TOP_sh1add_i, 
		 TOP_sh1add_ii, 
		 TOP_sh2add_i, 
		 TOP_sh2add_ii, 
		 TOP_sh3add_i, 
		 TOP_sh3add_ii, 
		 TOP_sh4add_i, 
		 TOP_sh4add_ii, 
		 TOP_shl_i, 
		 TOP_shl_ii, 
		 TOP_shr_i, 
		 TOP_shr_ii, 
		 TOP_shru_i, 
		 TOP_shru_ii, 
		 TOP_xor_i, 
		 TOP_xor_ii, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, src1, opnd1); 
  Operand (1, isrc2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_SelectI", 
		 TOP_slct_i, 
		 TOP_slct_ii, 
		 TOP_slctf_i, 
		 TOP_slctf_ii, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, bcond, condition); 
  Operand (1, src1, opnd1); 
  Operand (2, isrc2, opnd2); 

  /* ====================================== */ 
  Instruction_Group("O_mfb", 
		 TOP_mfb, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, scond); 

  /* ====================================== */ 
  Instruction_Group("O_move", 
		 TOP_mov_i, 
		 TOP_mov_ii, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, isrc2, opnd1); 

  /* ====================================== */ 
  Instruction_Group("O_spadjust", 
		 TOP_spadjust, 
		 TOP_UNDEFINED); 

  Result (0, idest); 
  Operand (0, src1); 
  Operand (1, isrc2); 

  /* ====================================== */ 
  Instruction_Group("O_call", 
		 TOP_call, 
		 TOP_UNDEFINED); 

  Result (0, lr); 
  Operand (0, btarg); 

  /* ====================================== */ 
  Instruction_Group("O_getpc", 
		 TOP_getpc, 
		 TOP_UNDEFINED); 

  Result (0, lr); 
  Operand (0, btarg); 

  /* ====================================== */ 
  Instruction_Group("O_icall", 
		 TOP_icall, 
		 TOP_UNDEFINED); 

  Result (0, lr); 
  Operand (0, lr); 



  ISA_Operands_End(); 
  return 0; 
} 
