//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! 
// Group TOPS with similar printing format. 
//////////////////////////////////////////////////


#include <stdio.h> 
#include <stddef.h> 
#include <string.h> 
#include <ctype.h> 
#include "topcode.h" 
#include "isa_print_gen.h" 

// Multiple topcodes map to the same assembly name. To disambiguate the 
// topcodes, we append a signature to the basename. To get the assembly 
// name we must strip off the suffix. 

static const char *asmname(TOP topcode) 
{ 
  int c; 
  int i; 
  const char *name = TOP_Name(topcode); 
  char buf[100]; 
  /* 
   * First handle simulated and dummy instructions: 
   */ 
  switch (topcode) { 
  case TOP_asm: return "asm"; 
  case TOP_intrncall: return "intrncall"; 
  case TOP_spadjust: return "spadjust"; 
  case TOP_copy_br: return "copy_br"; 
  case TOP_noop: return "noop"; 
  case TOP_begin_pregtn: return "begin_pregtn"; 
  case TOP_end_pregtn: return "end_pregtn"; 
  case TOP_bwd_bar: return "bwd_bar"; 
  case TOP_fwd_bar: return "fwd_bar"; 
  case TOP_dfixup: return "dfixup"; 
  case TOP_ffixup: return "ffixup"; 
  case TOP_ifixup: return "ifixup"; 
  case TOP_label: return "label"; 
  default: break; 
  } 

  for (i = 0; c != '\0'; ++i) { 
    c = name[i]; 
    if (c == '_') { 
      // if next is _i or _r, get out, else it's a _d 
      if (name[i+1] == 'd') { 
	buf[i] = '.'; 
	buf[i+1] = 'd'; 
        buf[i+3] = '\0'; 
      } 
      else { 
	buf[i] = '\0'; 
      } 
      break; 
    } 
    buf[i] = c; 
  } 

  return strdup(buf); 
} 

main() 
{ 
  ISA_Print_Begin("st200"); 

  Set_AsmName_Func(asmname); 

  Define_Macro("END_GROUP", ";;");	// end-of-group marker 
  Define_Macro("BEGIN_BUNDLE", "## bundle %s:");	// bundle introducer 
  Define_Macro("END_BUNDLE", ";;");	// bundle terminator 

  /* ================================= */ 
  ISA_PRINT_TYPE print_0; 
  print_0 = ISA_Print_Type_Create("print_0", "c0	%s LR"); 
  Name("%s"); 

  Instruction_Print_Group(print_0, 
		 TOP_igoto, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_1; 
  print_1 = ISA_Print_Type_Create("print_1", "c0	%s LR = %s"); 
  Name("%s"); 
  Operand(0, "%s"); 

  Instruction_Print_Group(print_1, 
		 TOP_call, 
		 TOP_goto, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_2; 
  print_2 = ISA_Print_Type_Create("print_2", "c0	%s %s = %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%d"); 

  Instruction_Print_Group(print_2, 
		 TOP_mov_i, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_3; 
  print_3 = ISA_Print_Type_Create("print_3", "c0	%s %s = %s, %s, %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%s"); 
  Operand(1, "%s"); 
  Operand(2, "%d"); 

  Instruction_Print_Group(print_3, 
		 TOP_slct_i, 
		 TOP_slctf_i, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_4; 
  print_4 = ISA_Print_Type_Create("print_4", "c0	%s %s = %s, %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%s"); 
  Operand(1, "%d"); 

  Instruction_Print_Group(print_4, 
		 TOP_add_i, 
		 TOP_sub_i, 
		 TOP_shl_i, 
		 TOP_shr_i, 
		 TOP_shru_i, 
		 TOP_sh1add_i, 
		 TOP_sh2add_i, 
		 TOP_sh3add_i, 
		 TOP_sh4add_i, 
		 TOP_and_i, 
		 TOP_andc_i, 
		 TOP_or_i, 
		 TOP_orc_i, 
		 TOP_xor_i, 
		 TOP_max_i, 
		 TOP_maxu_i, 
		 TOP_min_i, 
		 TOP_minu_i, 
		 TOP_mull_i, 
		 TOP_mullu_i, 
		 TOP_mulh_i, 
		 TOP_mulhu_i, 
		 TOP_mulll_i, 
		 TOP_mulllu_i, 
		 TOP_mullh_i, 
		 TOP_mullhu_i, 
		 TOP_mulhh_i, 
		 TOP_mulhhu_i, 
		 TOP_mulhs_i, 
		 TOP_cmpeq_i_r, 
		 TOP_cmpne_i_r, 
		 TOP_cmpge_i_r, 
		 TOP_cmpgeu_i_r, 
		 TOP_cmpgt_i_r, 
		 TOP_cmpgtu_i_r, 
		 TOP_cmple_i_r, 
		 TOP_cmpleu_i_r, 
		 TOP_cmplt_i_r, 
		 TOP_cmpltu_i_r, 
		 TOP_andl_i_r, 
		 TOP_nandl_i_r, 
		 TOP_orl_i_r, 
		 TOP_norl_i_r, 
		 TOP_cmpeq_i_b, 
		 TOP_cmpne_i_b, 
		 TOP_cmpge_i_b, 
		 TOP_cmpgeu_i_b, 
		 TOP_cmpgt_i_b, 
		 TOP_cmpgtu_i_b, 
		 TOP_cmple_i_b, 
		 TOP_cmpleu_i_b, 
		 TOP_cmplt_i_b, 
		 TOP_cmpltu_i_b, 
		 TOP_andl_i_b, 
		 TOP_nandl_i_b, 
		 TOP_orl_i_b, 
		 TOP_norl_i_b, 
		 TOP_send_i, 
		 TOP_recv_i, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_5; 
  print_5 = ISA_Print_Type_Create("print_5", "c0	%s"); 
  Name("%s"); 

  Instruction_Print_Group(print_5, 
		 TOP_rfi, 
		 TOP_nop, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_6; 
  print_6 = ISA_Print_Type_Create("print_6", "c0	%s %s"); 
  Name("%s"); 
  Operand(0, "%s"); 

  Instruction_Print_Group(print_6, 
		 TOP_return, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_7; 
  print_7 = ISA_Print_Type_Create("print_7", "c0	%s %s, %s"); 
  Name("%s"); 
  Operand(0, "%s"); 
  Operand(1, "%s"); 

  Instruction_Print_Group(print_7, 
		 TOP_br, 
		 TOP_brf, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_8; 
  print_8 = ISA_Print_Type_Create("print_8", "c0	%s %s [ %s ] = %s"); 
  Name("%s"); 
  Operand(0, "%d"); 
  Operand(1, "%s"); 
  Operand(2, "%s"); 

  Instruction_Print_Group(print_8, 
		 TOP_stw, 
		 TOP_sth, 
		 TOP_stb, 
		 TOP_pft, 
		 TOP_prgadd, 
		 TOP_prgset, 
		 TOP_sync, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_9; 
  print_9 = ISA_Print_Type_Create("print_9", "c0	%s LR = LR"); 
  Name("%s"); 

  Instruction_Print_Group(print_9, 
		 TOP_icall, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_10; 
  print_10 = ISA_Print_Type_Create("print_10", "c0	%s %s = %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%s"); 

  Instruction_Print_Group(print_10, 
		 TOP_mov_r, 
		 TOP_mtb, 
		 TOP_mfb, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_11; 
  print_11 = ISA_Print_Type_Create("print_11", "c0	%s %s = %s, %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%s"); 
  Operand(1, "%s"); 

  Instruction_Print_Group(print_11, 
		 TOP_add_r, 
		 TOP_sub_r, 
		 TOP_shl_r, 
		 TOP_shr_r, 
		 TOP_shru_r, 
		 TOP_sh1add_r, 
		 TOP_sh2add_r, 
		 TOP_sh3add_r, 
		 TOP_sh4add_r, 
		 TOP_and_r, 
		 TOP_andc_r, 
		 TOP_or_r, 
		 TOP_orc_r, 
		 TOP_xor_r, 
		 TOP_sxtb_r, 
		 TOP_sxth_r, 
		 TOP_max_r, 
		 TOP_maxu_r, 
		 TOP_min_r, 
		 TOP_minu_r, 
		 TOP_bswap_r, 
		 TOP_mull_r, 
		 TOP_mullu_r, 
		 TOP_mulh_r, 
		 TOP_mulhu_r, 
		 TOP_mulll_r, 
		 TOP_mulllu_r, 
		 TOP_mullh_r, 
		 TOP_mullhu_r, 
		 TOP_mulhh_r, 
		 TOP_mulhhu_r, 
		 TOP_mulhs_r, 
		 TOP_cmpeq_r_r, 
		 TOP_cmpne_r_r, 
		 TOP_cmpge_r_r, 
		 TOP_cmpgeu_r_r, 
		 TOP_cmpgt_r_r, 
		 TOP_cmpgtu_r_r, 
		 TOP_cmple_r_r, 
		 TOP_cmpleu_r_r, 
		 TOP_cmplt_r_r, 
		 TOP_cmpltu_r_r, 
		 TOP_andl_r_r, 
		 TOP_nandl_r_r, 
		 TOP_orl_r_r, 
		 TOP_norl_r_r, 
		 TOP_cmpeq_r_b, 
		 TOP_cmpne_r_b, 
		 TOP_cmpge_r_b, 
		 TOP_cmpgeu_r_b, 
		 TOP_cmpgt_r_b, 
		 TOP_cmpgtu_r_b, 
		 TOP_cmple_r_b, 
		 TOP_cmpleu_r_b, 
		 TOP_cmplt_r_b, 
		 TOP_cmpltu_r_b, 
		 TOP_andl_r_b, 
		 TOP_nandl_r_b, 
		 TOP_orl_r_b, 
		 TOP_norl_r_b, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_12; 
  print_12 = ISA_Print_Type_Create("print_12", "c0	%s %s = %s, %s, %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%s"); 
  Operand(1, "%s"); 
  Operand(2, "%s"); 

  Instruction_Print_Group(print_12, 
		 TOP_slct_r, 
		 TOP_slctf_r, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_13; 
  print_13 = ISA_Print_Type_Create("print_13", "c0	%s %s = %s [ %s ]"); 
  Name("%s"); 
  Result(0, "%s"); 
  Operand(0, "%d"); 
  Operand(1, "%s"); 

  Instruction_Print_Group(print_13, 
		 TOP_ldw, 
		 TOP_ldw_d, 
		 TOP_ldh, 
		 TOP_ldh_d, 
		 TOP_ldhu, 
		 TOP_ldhu_d, 
		 TOP_ldb, 
		 TOP_ldb_d, 
		 TOP_ldbu, 
		 TOP_ldbu_d, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_14; 
  print_14 = ISA_Print_Type_Create("print_14", "c0	%s %s, %s = %s, %s, %s"); 
  Name("%s"); 
  Result(0, "%s"); 
  Result(1, "%s"); 
  Operand(0, "%s"); 
  Operand(1, "%s"); 
  Operand(2, "%s"); 

  Instruction_Print_Group(print_14, 
		 TOP_addcg, 
		 TOP_divs, 
		 TOP_UNDEFINED); 

  /* ================================= */ 
  ISA_PRINT_TYPE print_15; 
  print_15 = ISA_Print_Type_Create("print_15", "c0	%s %s"); 
  Name("%s"); 
  Operand(0, "%d"); 

  Instruction_Print_Group(print_15, 
		 TOP_imml, 
		 TOP_immr, 
		 TOP_prgins, 
		 TOP_sbrk, 
		 TOP_syscall, 
		 TOP_break, 
		 TOP_UNDEFINED); 



  ISA_Print_End(); 
  return 0; 
} 
