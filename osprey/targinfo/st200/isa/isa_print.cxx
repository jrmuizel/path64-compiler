//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! 
// Group TOPS with similar printing format. 
//////////////////////////////////////////////////


#include <stdio.h> 
#include <stddef.h> 
#include <string.h> 
#include <ctype.h> 
#include "topcode.h" 
#include "isa_print_gen.h" 

static const char *mnemonic_names[306] = {
  "add",	 /* TOP_add_r */ 
  "add",	 /* TOP_add_i */ 
  "add",	 /* TOP_add_ii */ 
  "addcg",	 /* TOP_addcg */ 
  "and",	 /* TOP_and_r */ 
  "and",	 /* TOP_and_i */ 
  "and",	 /* TOP_and_ii */ 
  "andc",	 /* TOP_andc_r */ 
  "andc",	 /* TOP_andc_i */ 
  "andc",	 /* TOP_andc_ii */ 
  "andl",	 /* TOP_andl_r_r */ 
  "andl",	 /* TOP_andl_r_b */ 
  "andl",	 /* TOP_andl_i_r */ 
  "andl",	 /* TOP_andl_ii_r */ 
  "andl",	 /* TOP_andl_i_b */ 
  "andl",	 /* TOP_andl_ii_b */ 
  "asm_0",	 /* TOP_asm_0 */ 
  "asm_1",	 /* TOP_asm_1 */ 
  "asm_2",	 /* TOP_asm_2 */ 
  "asm_3",	 /* TOP_asm_3 */ 
  "asm_4",	 /* TOP_asm_4 */ 
  "asm_5",	 /* TOP_asm_5 */ 
  "asm_6",	 /* TOP_asm_6 */ 
  "asm_7",	 /* TOP_asm_7 */ 
  "asm_8",	 /* TOP_asm_8 */ 
  "asm_9",	 /* TOP_asm_9 */ 
  "asm_10",	 /* TOP_asm_10 */ 
  "asm_11",	 /* TOP_asm_11 */ 
  "asm_12",	 /* TOP_asm_12 */ 
  "asm_13",	 /* TOP_asm_13 */ 
  "asm_14",	 /* TOP_asm_14 */ 
  "asm_15",	 /* TOP_asm_15 */ 
  "asm_16",	 /* TOP_asm_16 */ 
  "asm_17",	 /* TOP_asm_17 */ 
  "asm_18",	 /* TOP_asm_18 */ 
  "asm_19",	 /* TOP_asm_19 */ 
  "asm_20",	 /* TOP_asm_20 */ 
  "asm_21",	 /* TOP_asm_21 */ 
  "asm_22",	 /* TOP_asm_22 */ 
  "asm_23",	 /* TOP_asm_23 */ 
  "asm_24",	 /* TOP_asm_24 */ 
  "asm_25",	 /* TOP_asm_25 */ 
  "asm_26",	 /* TOP_asm_26 */ 
  "asm_27",	 /* TOP_asm_27 */ 
  "asm_28",	 /* TOP_asm_28 */ 
  "asm_29",	 /* TOP_asm_29 */ 
  "asm_30",	 /* TOP_asm_30 */ 
  "asm_31",	 /* TOP_asm_31 */ 
  "br",	 /* TOP_br */ 
  "break",	 /* TOP_break */ 
  "brf",	 /* TOP_brf */ 
  "bswap",	 /* TOP_bswap_r */ 
  "call",	 /* TOP_call */ 
  "call",	 /* TOP_icall */ 
  "clz",	 /* TOP_clz_r */ 
  "cmpeq",	 /* TOP_cmpeq_r_r */ 
  "cmpeq",	 /* TOP_cmpeq_r_b */ 
  "cmpeq",	 /* TOP_cmpeq_i_r */ 
  "cmpeq",	 /* TOP_cmpeq_ii_r */ 
  "cmpeq",	 /* TOP_cmpeq_i_b */ 
  "cmpeq",	 /* TOP_cmpeq_ii_b */ 
  "cmpge",	 /* TOP_cmpge_r_r */ 
  "cmpge",	 /* TOP_cmpge_r_b */ 
  "cmpge",	 /* TOP_cmpge_i_r */ 
  "cmpge",	 /* TOP_cmpge_ii_r */ 
  "cmpge",	 /* TOP_cmpge_i_b */ 
  "cmpge",	 /* TOP_cmpge_ii_b */ 
  "cmpgeu",	 /* TOP_cmpgeu_r_r */ 
  "cmpgeu",	 /* TOP_cmpgeu_r_b */ 
  "cmpgeu",	 /* TOP_cmpgeu_i_r */ 
  "cmpgeu",	 /* TOP_cmpgeu_ii_r */ 
  "cmpgeu",	 /* TOP_cmpgeu_i_b */ 
  "cmpgeu",	 /* TOP_cmpgeu_ii_b */ 
  "cmpgt",	 /* TOP_cmpgt_r_r */ 
  "cmpgt",	 /* TOP_cmpgt_r_b */ 
  "cmpgt",	 /* TOP_cmpgt_i_r */ 
  "cmpgt",	 /* TOP_cmpgt_ii_r */ 
  "cmpgt",	 /* TOP_cmpgt_i_b */ 
  "cmpgt",	 /* TOP_cmpgt_ii_b */ 
  "cmpgtu",	 /* TOP_cmpgtu_r_r */ 
  "cmpgtu",	 /* TOP_cmpgtu_r_b */ 
  "cmpgtu",	 /* TOP_cmpgtu_i_r */ 
  "cmpgtu",	 /* TOP_cmpgtu_ii_r */ 
  "cmpgtu",	 /* TOP_cmpgtu_i_b */ 
  "cmpgtu",	 /* TOP_cmpgtu_ii_b */ 
  "cmple",	 /* TOP_cmple_r_r */ 
  "cmple",	 /* TOP_cmple_r_b */ 
  "cmple",	 /* TOP_cmple_i_r */ 
  "cmple",	 /* TOP_cmple_ii_r */ 
  "cmple",	 /* TOP_cmple_i_b */ 
  "cmple",	 /* TOP_cmple_ii_b */ 
  "cmpleu",	 /* TOP_cmpleu_r_r */ 
  "cmpleu",	 /* TOP_cmpleu_r_b */ 
  "cmpleu",	 /* TOP_cmpleu_i_r */ 
  "cmpleu",	 /* TOP_cmpleu_ii_r */ 
  "cmpleu",	 /* TOP_cmpleu_i_b */ 
  "cmpleu",	 /* TOP_cmpleu_ii_b */ 
  "cmplt",	 /* TOP_cmplt_r_r */ 
  "cmplt",	 /* TOP_cmplt_r_b */ 
  "cmplt",	 /* TOP_cmplt_i_r */ 
  "cmplt",	 /* TOP_cmplt_ii_r */ 
  "cmplt",	 /* TOP_cmplt_i_b */ 
  "cmplt",	 /* TOP_cmplt_ii_b */ 
  "cmpltu",	 /* TOP_cmpltu_r_r */ 
  "cmpltu",	 /* TOP_cmpltu_r_b */ 
  "cmpltu",	 /* TOP_cmpltu_i_r */ 
  "cmpltu",	 /* TOP_cmpltu_ii_r */ 
  "cmpltu",	 /* TOP_cmpltu_i_b */ 
  "cmpltu",	 /* TOP_cmpltu_ii_b */ 
  "cmpne",	 /* TOP_cmpne_r_r */ 
  "cmpne",	 /* TOP_cmpne_r_b */ 
  "cmpne",	 /* TOP_cmpne_i_r */ 
  "cmpne",	 /* TOP_cmpne_ii_r */ 
  "cmpne",	 /* TOP_cmpne_i_b */ 
  "cmpne",	 /* TOP_cmpne_ii_b */ 
  "divs",	 /* TOP_divs */ 
  "goto",	 /* TOP_goto */ 
  "goto",	 /* TOP_igoto */ 
  "imml",	 /* TOP_imml */ 
  "immr",	 /* TOP_immr */ 
  "ldb",	 /* TOP_ldb_i */ 
  "ldb",	 /* TOP_ldb_ii */ 
  "ldb.d",	 /* TOP_ldb_d_i */ 
  "ldb.d",	 /* TOP_ldb_d_ii */ 
  "ldbu",	 /* TOP_ldbu_i */ 
  "ldbu",	 /* TOP_ldbu_ii */ 
  "ldbu.d",	 /* TOP_ldbu_d_i */ 
  "ldbu.d",	 /* TOP_ldbu_d_ii */ 
  "ldh",	 /* TOP_ldh_i */ 
  "ldh",	 /* TOP_ldh_ii */ 
  "ldh.d",	 /* TOP_ldh_d_i */ 
  "ldh.d",	 /* TOP_ldh_d_ii */ 
  "ldhu",	 /* TOP_ldhu_i */ 
  "ldhu",	 /* TOP_ldhu_ii */ 
  "ldhu.d",	 /* TOP_ldhu_d_i */ 
  "ldhu.d",	 /* TOP_ldhu_d_ii */ 
  "ldw",	 /* TOP_ldw_i */ 
  "ldw",	 /* TOP_ldw_ii */ 
  "ldw.d",	 /* TOP_ldw_d_i */ 
  "ldw.d",	 /* TOP_ldw_d_ii */ 
  "max",	 /* TOP_max_r */ 
  "max",	 /* TOP_max_i */ 
  "max",	 /* TOP_max_ii */ 
  "maxu",	 /* TOP_maxu_r */ 
  "maxu",	 /* TOP_maxu_i */ 
  "maxu",	 /* TOP_maxu_ii */ 
  "min",	 /* TOP_min_r */ 
  "min",	 /* TOP_min_i */ 
  "min",	 /* TOP_min_ii */ 
  "minu",	 /* TOP_minu_r */ 
  "minu",	 /* TOP_minu_i */ 
  "minu",	 /* TOP_minu_ii */ 
  "mulh",	 /* TOP_mulh_r */ 
  "mulh",	 /* TOP_mulh_i */ 
  "mulh",	 /* TOP_mulh_ii */ 
  "mulhh",	 /* TOP_mulhh_r */ 
  "mulhh",	 /* TOP_mulhh_i */ 
  "mulhh",	 /* TOP_mulhh_ii */ 
  "mulhhs",	 /* TOP_mulhhs_r */ 
  "mulhhs",	 /* TOP_mulhhs_i */ 
  "mulhhs",	 /* TOP_mulhhs_ii */ 
  "mulhhu",	 /* TOP_mulhhu_r */ 
  "mulhhu",	 /* TOP_mulhhu_i */ 
  "mulhhu",	 /* TOP_mulhhu_ii */ 
  "mulhs",	 /* TOP_mulhs_r */ 
  "mulhs",	 /* TOP_mulhs_i */ 
  "mulhs",	 /* TOP_mulhs_ii */ 
  "mulhu",	 /* TOP_mulhu_r */ 
  "mulhu",	 /* TOP_mulhu_i */ 
  "mulhu",	 /* TOP_mulhu_ii */ 
  "mull",	 /* TOP_mull_r */ 
  "mull",	 /* TOP_mull_i */ 
  "mull",	 /* TOP_mull_ii */ 
  "mullh",	 /* TOP_mullh_r */ 
  "mullh",	 /* TOP_mullh_i */ 
  "mullh",	 /* TOP_mullh_ii */ 
  "mullhu",	 /* TOP_mullhu_r */ 
  "mullhu",	 /* TOP_mullhu_i */ 
  "mullhu",	 /* TOP_mullhu_ii */ 
  "mullhus",	 /* TOP_mullhus_r */ 
  "mullhus",	 /* TOP_mullhus_i */ 
  "mullhus",	 /* TOP_mullhus_ii */ 
  "mulll",	 /* TOP_mulll_r */ 
  "mulll",	 /* TOP_mulll_i */ 
  "mulll",	 /* TOP_mulll_ii */ 
  "mulllu",	 /* TOP_mulllu_r */ 
  "mulllu",	 /* TOP_mulllu_i */ 
  "mulllu",	 /* TOP_mulllu_ii */ 
  "mullu",	 /* TOP_mullu_r */ 
  "mullu",	 /* TOP_mullu_i */ 
  "mullu",	 /* TOP_mullu_ii */ 
  "mul32",	 /* TOP_mul32_r */ 
  "mul32",	 /* TOP_mul32_i */ 
  "mul32",	 /* TOP_mul32_ii */ 
  "mul64h",	 /* TOP_mul64h_r */ 
  "mul64h",	 /* TOP_mul64h_i */ 
  "mul64h",	 /* TOP_mul64h_ii */ 
  "mul64hu",	 /* TOP_mul64hu_r */ 
  "mul64hu",	 /* TOP_mul64hu_i */ 
  "mul64hu",	 /* TOP_mul64hu_ii */ 
  "mulfrac",	 /* TOP_mulfrac_r */ 
  "mulfrac",	 /* TOP_mulfrac_i */ 
  "mulfrac",	 /* TOP_mulfrac_ii */ 
  "nandl",	 /* TOP_nandl_r_r */ 
  "nandl",	 /* TOP_nandl_r_b */ 
  "nandl",	 /* TOP_nandl_i_r */ 
  "nandl",	 /* TOP_nandl_ii_r */ 
  "nandl",	 /* TOP_nandl_i_b */ 
  "nandl",	 /* TOP_nandl_ii_b */ 
  "norl",	 /* TOP_norl_r_r */ 
  "norl",	 /* TOP_norl_r_b */ 
  "norl",	 /* TOP_norl_i_r */ 
  "norl",	 /* TOP_norl_ii_r */ 
  "norl",	 /* TOP_norl_i_b */ 
  "norl",	 /* TOP_norl_ii_b */ 
  "or",	 /* TOP_or_r */ 
  "or",	 /* TOP_or_i */ 
  "or",	 /* TOP_or_ii */ 
  "orc",	 /* TOP_orc_r */ 
  "orc",	 /* TOP_orc_i */ 
  "orc",	 /* TOP_orc_ii */ 
  "orl",	 /* TOP_orl_r_r */ 
  "orl",	 /* TOP_orl_r_b */ 
  "orl",	 /* TOP_orl_i_r */ 
  "orl",	 /* TOP_orl_ii_r */ 
  "orl",	 /* TOP_orl_i_b */ 
  "orl",	 /* TOP_orl_ii_b */ 
  "pft",	 /* TOP_pft_i */ 
  "pft",	 /* TOP_pft_ii */ 
  "prgadd",	 /* TOP_prgadd_i */ 
  "prgadd",	 /* TOP_prgadd_ii */ 
  "prgins",	 /* TOP_prgins */ 
  "prginspg",	 /* TOP_prginspg_i */ 
  "prginspg",	 /* TOP_prginspg_ii */ 
  "prgset",	 /* TOP_prgset_i */ 
  "prgset",	 /* TOP_prgset_ii */ 
  "pswset",	 /* TOP_pswset_r */ 
  "pswclr",	 /* TOP_pswclr_r */ 
  "rfi",	 /* TOP_rfi */ 
  "sbrk",	 /* TOP_sbrk */ 
  "sh1add",	 /* TOP_sh1add_r */ 
  "sh1add",	 /* TOP_sh1add_i */ 
  "sh1add",	 /* TOP_sh1add_ii */ 
  "sh2add",	 /* TOP_sh2add_r */ 
  "sh2add",	 /* TOP_sh2add_i */ 
  "sh2add",	 /* TOP_sh2add_ii */ 
  "sh3add",	 /* TOP_sh3add_r */ 
  "sh3add",	 /* TOP_sh3add_i */ 
  "sh3add",	 /* TOP_sh3add_ii */ 
  "sh4add",	 /* TOP_sh4add_r */ 
  "sh4add",	 /* TOP_sh4add_i */ 
  "sh4add",	 /* TOP_sh4add_ii */ 
  "shl",	 /* TOP_shl_r */ 
  "shl",	 /* TOP_shl_i */ 
  "shl",	 /* TOP_shl_ii */ 
  "shr",	 /* TOP_shr_r */ 
  "shr",	 /* TOP_shr_i */ 
  "shr",	 /* TOP_shr_ii */ 
  "shru",	 /* TOP_shru_r */ 
  "shru",	 /* TOP_shru_i */ 
  "shru",	 /* TOP_shru_ii */ 
  "slct",	 /* TOP_slct_r */ 
  "slct",	 /* TOP_slct_i */ 
  "slct",	 /* TOP_slct_ii */ 
  "slctf",	 /* TOP_slctf_r */ 
  "slctf",	 /* TOP_slctf_i */ 
  "slctf",	 /* TOP_slctf_ii */ 
  "stb",	 /* TOP_stb_i */ 
  "stb",	 /* TOP_stb_ii */ 
  "sth",	 /* TOP_sth_i */ 
  "sth",	 /* TOP_sth_ii */ 
  "stw",	 /* TOP_stw_i */ 
  "stw",	 /* TOP_stw_ii */ 
  "sub",	 /* TOP_sub_r */ 
  "sub",	 /* TOP_sub_i */ 
  "sub",	 /* TOP_sub_ii */ 
  "sxtb",	 /* TOP_sxtb_r */ 
  "sxth",	 /* TOP_sxth_r */ 
  "sync",	 /* TOP_sync */ 
  "syscall",	 /* TOP_syscall */ 
  "xor",	 /* TOP_xor_r */ 
  "xor",	 /* TOP_xor_i */ 
  "xor",	 /* TOP_xor_ii */ 
  "zxth",	 /* TOP_zxth_r */ 
  "nop",	 /* TOP_nop */ 
  "mov",	 /* TOP_mov_r */ 
  "mov",	 /* TOP_mov_i */ 
  "mov",	 /* TOP_mov_ii */ 
  "mtb",	 /* TOP_mtb */ 
  "mfb",	 /* TOP_mfb */ 
  "return",	 /* TOP_return */ 
  "asm",	 /* TOP_asm */
  "intrncall",	 /* TOP_intrncall */
  "spadjust",	 /* TOP_spadjust */
  "noop",	 /* TOP_noop */
  "getpc",	 /* TOP_getpc */
  "phi",	 /* TOP_phi */
  "psi",	 /* TOP_psi */
  "begin_pregtn",	 /* TOP_begin_pregtn */
  "end_pregtn",	 /* TOP_end_pregtn */
  "bwd_bar",	 /* TOP_bwd_bar */
  "fwd_bar",	 /* TOP_fwd_bar */
  "dfixup",	 /* TOP_dfixup */
  "ffixup",	 /* TOP_ffixup */
  "ifixup",	 /* TOP_ifixup */
  "label" 	 /* TOP_label */
};

static const char *asmname(TOP topcode) 
{ 
  return mnemonic_names[topcode]; 
} 

main() 
{ 
  ISA_Print_Begin("st200"); 

  Set_AsmName_Func(asmname); 

  Define_Macro("END_GROUP", ";;");	// end-of-group marker 
  Define_Macro("BEGIN_BUNDLE", "## {	 %s:");	// bundle introducer 
  Define_Macro("END_BUNDLE", "## };");	// bundle terminator 

  /* ================================= */ 
  ISA_PRINT_TYPE print_0; 
  print_0 = ISA_Print_Type_Create("print_0", "%s"); 
  Name(); 

  Instruction_Print_Group(print_0, 
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
		 TOP_break, 
		 TOP_prgins, 
		 TOP_rfi, 
		 TOP_sbrk, 
		 TOP_sync, 
		 TOP_syscall, 
		 TOP_nop, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_1; 
  print_1 = ISA_Print_Type_Create("print_1", "%s %s"); 
  Name(); 
  Operand(0); 

  Instruction_Print_Group(print_1, 
		 TOP_imml, 
		 TOP_immr, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_2; 
  print_2 = ISA_Print_Type_Create("print_2", "%s %s [ %s ]"); 
  Name(); 
  Operand(0); 
  Operand(1); 

  Instruction_Print_Group(print_2, 
		 TOP_pft_i, 
		 TOP_pft_ii, 
		 TOP_prgadd_i, 
		 TOP_prgadd_ii, 
		 TOP_prginspg_i, 
		 TOP_prginspg_ii, 
		 TOP_prgset_i, 
		 TOP_prgset_ii, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_3; 
  print_3 = ISA_Print_Type_Create("print_3", "%s %s [ %s ]  = %s"); 
  Name(); 
  Operand(0); 
  Operand(1); 
  Operand(2); 

  Instruction_Print_Group(print_3, 
		 TOP_stb_i, 
		 TOP_stb_ii, 
		 TOP_sth_i, 
		 TOP_sth_ii, 
		 TOP_stw_i, 
		 TOP_stw_ii, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_4; 
  print_4 = ISA_Print_Type_Create("print_4", "%s %s"); 
  Name(); 
  Operand(0); 

  Instruction_Print_Group(print_4, 
		 TOP_goto, 
		 TOP_igoto, 
		 TOP_return, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_5; 
  print_5 = ISA_Print_Type_Create("print_5", "%s %s , %s"); 
  Name(); 
  Operand(0); 
  Operand(1); 

  Instruction_Print_Group(print_5, 
		 TOP_br, 
		 TOP_brf, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_6; 
  print_6 = ISA_Print_Type_Create("print_6", "%s %s"); 
  Name(); 
  Operand(2); 

  Instruction_Print_Group(print_6, 
		 TOP_pswset_r, 
		 TOP_pswclr_r, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_7; 
  print_7 = ISA_Print_Type_Create("print_7", "%s %s , %s = %s , %s , %s"); 
  Name(); 
  Result(0); 
  Result(1); 
  Operand(0); 
  Operand(1); 
  Operand(2); 

  Instruction_Print_Group(print_7, 
		 TOP_addcg, 
		 TOP_divs, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_8; 
  print_8 = ISA_Print_Type_Create("print_8", "%s %s = %s [ %s ]"); 
  Name(); 
  Result(0); 
  Operand(0); 
  Operand(1); 

  Instruction_Print_Group(print_8, 
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
		 TOP_ldw_i, 
		 TOP_ldw_ii, 
		 TOP_ldw_d_i, 
		 TOP_ldw_d_ii, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_9; 
  print_9 = ISA_Print_Type_Create("print_9", "%s %s = %s"); 
  Name(); 
  Result(0); 
  Operand(0); 

  Instruction_Print_Group(print_9, 
		 TOP_bswap_r, 
		 TOP_call, 
		 TOP_icall, 
		 TOP_clz_r, 
		 TOP_sxtb_r, 
		 TOP_sxth_r, 
		 TOP_zxth_r, 
		 TOP_mov_r, 
		 TOP_mov_i, 
		 TOP_mov_ii, 
		 TOP_mtb, 
		 TOP_mfb, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_10; 
  print_10 = ISA_Print_Type_Create("print_10", "%s %s = %s , %s"); 
  Name(); 
  Result(0); 
  Operand(0); 
  Operand(1); 

  Instruction_Print_Group(print_10, 
		 TOP_andl_i_r, 
		 TOP_andl_ii_r, 
		 TOP_andl_i_b, 
		 TOP_andl_ii_b, 
		 TOP_cmpeq_i_r, 
		 TOP_cmpeq_ii_r, 
		 TOP_cmpeq_i_b, 
		 TOP_cmpeq_ii_b, 
		 TOP_cmpge_i_r, 
		 TOP_cmpge_ii_r, 
		 TOP_cmpge_i_b, 
		 TOP_cmpge_ii_b, 
		 TOP_cmpgeu_i_r, 
		 TOP_cmpgeu_ii_r, 
		 TOP_cmpgeu_i_b, 
		 TOP_cmpgeu_ii_b, 
		 TOP_cmpgt_i_r, 
		 TOP_cmpgt_ii_r, 
		 TOP_cmpgt_i_b, 
		 TOP_cmpgt_ii_b, 
		 TOP_cmpgtu_i_r, 
		 TOP_cmpgtu_ii_r, 
		 TOP_cmpgtu_i_b, 
		 TOP_cmpgtu_ii_b, 
		 TOP_cmple_i_r, 
		 TOP_cmple_ii_r, 
		 TOP_cmple_i_b, 
		 TOP_cmple_ii_b, 
		 TOP_cmpleu_i_r, 
		 TOP_cmpleu_ii_r, 
		 TOP_cmpleu_i_b, 
		 TOP_cmpleu_ii_b, 
		 TOP_cmplt_i_r, 
		 TOP_cmplt_ii_r, 
		 TOP_cmplt_i_b, 
		 TOP_cmplt_ii_b, 
		 TOP_cmpltu_i_r, 
		 TOP_cmpltu_ii_r, 
		 TOP_cmpltu_i_b, 
		 TOP_cmpltu_ii_b, 
		 TOP_cmpne_i_r, 
		 TOP_cmpne_ii_r, 
		 TOP_cmpne_i_b, 
		 TOP_cmpne_ii_b, 
		 TOP_nandl_i_r, 
		 TOP_nandl_ii_r, 
		 TOP_nandl_i_b, 
		 TOP_nandl_ii_b, 
		 TOP_norl_i_r, 
		 TOP_norl_ii_r, 
		 TOP_norl_i_b, 
		 TOP_norl_ii_b, 
		 TOP_orl_i_r, 
		 TOP_orl_ii_r, 
		 TOP_orl_i_b, 
		 TOP_orl_ii_b, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_11; 
  print_11 = ISA_Print_Type_Create("print_11", "%s %s = %s , %s"); 
  Name(); 
  Result(0); 
  Operand(0); 
  Operand(1); 

  Instruction_Print_Group(print_11, 
		 TOP_add_r, 
		 TOP_add_i, 
		 TOP_add_ii, 
		 TOP_and_r, 
		 TOP_and_i, 
		 TOP_and_ii, 
		 TOP_andc_r, 
		 TOP_andc_i, 
		 TOP_andc_ii, 
		 TOP_andl_r_r, 
		 TOP_andl_r_b, 
		 TOP_cmpeq_r_r, 
		 TOP_cmpeq_r_b, 
		 TOP_cmpge_r_r, 
		 TOP_cmpge_r_b, 
		 TOP_cmpgeu_r_r, 
		 TOP_cmpgeu_r_b, 
		 TOP_cmpgt_r_r, 
		 TOP_cmpgt_r_b, 
		 TOP_cmpgtu_r_r, 
		 TOP_cmpgtu_r_b, 
		 TOP_cmple_r_r, 
		 TOP_cmple_r_b, 
		 TOP_cmpleu_r_r, 
		 TOP_cmpleu_r_b, 
		 TOP_cmplt_r_r, 
		 TOP_cmplt_r_b, 
		 TOP_cmpltu_r_r, 
		 TOP_cmpltu_r_b, 
		 TOP_cmpne_r_r, 
		 TOP_cmpne_r_b, 
		 TOP_max_r, 
		 TOP_max_i, 
		 TOP_max_ii, 
		 TOP_maxu_r, 
		 TOP_maxu_i, 
		 TOP_maxu_ii, 
		 TOP_min_r, 
		 TOP_min_i, 
		 TOP_min_ii, 
		 TOP_minu_r, 
		 TOP_minu_i, 
		 TOP_minu_ii, 
		 TOP_mulh_r, 
		 TOP_mulh_i, 
		 TOP_mulh_ii, 
		 TOP_mulhh_r, 
		 TOP_mulhh_i, 
		 TOP_mulhh_ii, 
		 TOP_mulhhs_r, 
		 TOP_mulhhs_i, 
		 TOP_mulhhs_ii, 
		 TOP_mulhhu_r, 
		 TOP_mulhhu_i, 
		 TOP_mulhhu_ii, 
		 TOP_mulhs_r, 
		 TOP_mulhs_i, 
		 TOP_mulhs_ii, 
		 TOP_mulhu_r, 
		 TOP_mulhu_i, 
		 TOP_mulhu_ii, 
		 TOP_mull_r, 
		 TOP_mull_i, 
		 TOP_mull_ii, 
		 TOP_mullh_r, 
		 TOP_mullh_i, 
		 TOP_mullh_ii, 
		 TOP_mullhu_r, 
		 TOP_mullhu_i, 
		 TOP_mullhu_ii, 
		 TOP_mullhus_r, 
		 TOP_mullhus_i, 
		 TOP_mullhus_ii, 
		 TOP_mulll_r, 
		 TOP_mulll_i, 
		 TOP_mulll_ii, 
		 TOP_mulllu_r, 
		 TOP_mulllu_i, 
		 TOP_mulllu_ii, 
		 TOP_mullu_r, 
		 TOP_mullu_i, 
		 TOP_mullu_ii, 
		 TOP_mul32_r, 
		 TOP_mul32_i, 
		 TOP_mul32_ii, 
		 TOP_mul64h_r, 
		 TOP_mul64h_i, 
		 TOP_mul64h_ii, 
		 TOP_mul64hu_r, 
		 TOP_mul64hu_i, 
		 TOP_mul64hu_ii, 
		 TOP_mulfrac_r, 
		 TOP_mulfrac_i, 
		 TOP_mulfrac_ii, 
		 TOP_nandl_r_r, 
		 TOP_nandl_r_b, 
		 TOP_norl_r_r, 
		 TOP_norl_r_b, 
		 TOP_or_r, 
		 TOP_or_i, 
		 TOP_or_ii, 
		 TOP_orc_r, 
		 TOP_orc_i, 
		 TOP_orc_ii, 
		 TOP_orl_r_r, 
		 TOP_orl_r_b, 
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
		 TOP_shr_r, 
		 TOP_shr_i, 
		 TOP_shr_ii, 
		 TOP_shru_r, 
		 TOP_shru_i, 
		 TOP_shru_ii, 
		 TOP_sub_r, 
		 TOP_sub_i, 
		 TOP_sub_ii, 
		 TOP_xor_r, 
		 TOP_xor_i, 
		 TOP_xor_ii, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_12; 
  print_12 = ISA_Print_Type_Create("print_12", "%s %s = %s , %s , %s"); 
  Name(); 
  Result(0); 
  Operand(0); 
  Operand(1); 
  Operand(2); 

  Instruction_Print_Group(print_12, 
		 TOP_slct_i, 
		 TOP_slct_ii, 
		 TOP_slctf_i, 
		 TOP_slctf_ii, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_13; 
  print_13 = ISA_Print_Type_Create("print_13", "%s %s = %s , %s , %s"); 
  Name(); 
  Result(0); 
  Operand(0); 
  Operand(1); 
  Operand(2); 

  Instruction_Print_Group(print_13, 
		 TOP_slct_r, 
		 TOP_slctf_r, 
		 TOP_UNDEFINED); 



  ISA_Print_End(); 
  return 0; 
} 
