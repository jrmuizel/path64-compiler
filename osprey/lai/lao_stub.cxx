#include <stdio.h>

// Do not redefine these types

#define _INT8_T
#define _INTPTR_T
#define _UINTPTR_T

#include "bb.h"
#include "cg_region.h"
#include "freq.h"
#include "label_util.h"
#include "op.h"
#include "tag.h"
#include "tn_map.h"
#include "gtn_universe.h"
#include "gtn_tn_set.h"

#include "cg_dep_graph.h"

#include "erglob.h"
#include "tracing.h"

#include "stdint.h"
#include "stdbool.h"

#include "lao_stub.h"

extern "C" {

  // Because this and operator are C++ keywords

#define this     THIS
#define operator OPERATOR

  // To prevent name space conflict

#define BasicBlock LIR_NODE
#define TempName   LIR_OPERAND
#define Operation  LIR_OPERATION

#include "LIR2.h"

  // Missing definition in LAO
typedef void * LIR_REGION;

#undef Operation
#undef TempName
#undef BasicBlock

#undef operator
#undef this

}

// Array to map TOP operators to LIR operators
static Operator TOP2LAO[TOP_UNDEFINED];

// Memory pool local to the stub
static MEM_POOL MEM_lao_stub_pool;

// Hash table to map CGIR TNs to LAO TempNames.
static hTN_MAP TN2LIR_map;

static void
TOP2LAO_INIT() {

  TOP2LAO[TOP_add_i] = Operator_CODE_ADD_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_add_ii] = Operator_;
  TOP2LAO[TOP_add_r] = Operator_CODE_ADD_DEST_SRC1_SRC2;
  TOP2LAO[TOP_addcg] = Operator_CODE_ADDCG_DEST_BDEST_SRC1_SRC2_SCOND;
  TOP2LAO[TOP_and_i] = Operator_CODE_AND_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_and_ii] = Operator_;
  TOP2LAO[TOP_and_r] = Operator_CODE_AND_DEST_SRC1_SRC2;
  TOP2LAO[TOP_andc_i] = Operator_CODE_ANDC_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_andc_ii] = Operator_;
  TOP2LAO[TOP_andc_r] = Operator_CODE_ANDC_DEST_SRC1_SRC2;
  TOP2LAO[TOP_andl_i_b] = Operator_CODE_ANDL_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_andl_i_r] = Operator_CODE_ANDL_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_andl_ii_b] = Operator_;
  TOP2LAO[TOP_andl_ii_r] = Operator_;
  TOP2LAO[TOP_andl_r_b] = Operator_CODE_ANDL_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_andl_r_r] = Operator_CODE_ANDL_DEST_SRC1_SRC2;
  TOP2LAO[TOP_asm] = Operator_CODE_ASM15_DEST_SRC1_SRC2;
  TOP2LAO[TOP_begin_pregtn] = Operator_;
  TOP2LAO[TOP_br] = Operator_CODE_BR_BCOND_BTARG;
  TOP2LAO[TOP_break] = Operator_CODE_BREAK;
  TOP2LAO[TOP_brf] = Operator_CODE_BRF_BCOND_BTARG;
  TOP2LAO[TOP_bswap_r] = Operator_CODE_BSWAP_IDEST_SRC1;
  TOP2LAO[TOP_bwd_bar] = Operator_;
  TOP2LAO[TOP_call] = Operator_CODE_CALL_BTARG;
  TOP2LAO[TOP_cmpeq_i_b] = Operator_CODE_CMPEQ_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpeq_i_r] = Operator_CODE_CMPEQ_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpeq_ii_b] = Operator_;
  TOP2LAO[TOP_cmpeq_ii_r] = Operator_;
  TOP2LAO[TOP_cmpeq_r_b] = Operator_CODE_CMPEQ_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpeq_r_r] = Operator_CODE_CMPEQ_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpge_i_b] = Operator_CODE_CMPGE_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpge_i_r] = Operator_CODE_CMPGE_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpge_ii_b] = Operator_;
  TOP2LAO[TOP_cmpge_ii_r] = Operator_;
  TOP2LAO[TOP_cmpge_r_b] = Operator_CODE_CMPGE_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpge_r_r] = Operator_CODE_CMPGE_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpgeu_i_b] = Operator_CODE_CMPGEU_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpgeu_i_r] = Operator_CODE_CMPGEU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpgeu_ii_b] = Operator_;
  TOP2LAO[TOP_cmpgeu_ii_r] = Operator_;
  TOP2LAO[TOP_cmpgeu_r_b] = Operator_CODE_CMPGEU_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpgeu_r_r] = Operator_CODE_CMPGEU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpgt_i_b] = Operator_CODE_CMPGT_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpgt_i_r] = Operator_CODE_CMPGT_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpgt_ii_b] = Operator_;
  TOP2LAO[TOP_cmpgt_ii_r] = Operator_;
  TOP2LAO[TOP_cmpgt_r_b] = Operator_CODE_CMPGT_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpgt_r_r] = Operator_CODE_CMPGT_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpgtu_i_b] = Operator_CODE_CMPGTU_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpgtu_i_r] = Operator_CODE_CMPGTU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpgtu_ii_b] = Operator_;
  TOP2LAO[TOP_cmpgtu_ii_r] = Operator_;
  TOP2LAO[TOP_cmpgtu_r_b] = Operator_CODE_CMPGTU_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpgtu_r_r] = Operator_CODE_CMPGTU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmple_i_b] = Operator_CODE_CMPLE_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmple_i_r] = Operator_CODE_CMPLE_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmple_ii_b] = Operator_;
  TOP2LAO[TOP_cmple_ii_r] = Operator_;
  TOP2LAO[TOP_cmple_r_b] = Operator_CODE_CMPLE_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmple_r_r] = Operator_CODE_CMPLE_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpleu_i_b] = Operator_CODE_CMPLEU_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpleu_i_r] = Operator_CODE_CMPLEU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpleu_ii_b] = Operator_;
  TOP2LAO[TOP_cmpleu_ii_r] = Operator_;
  TOP2LAO[TOP_cmpleu_r_b] = Operator_CODE_CMPLEU_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpleu_r_r] = Operator_CODE_CMPLEU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmplt_i_b] = Operator_CODE_CMPLT_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmplt_i_r] = Operator_CODE_CMPLT_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmplt_ii_b] = Operator_;
  TOP2LAO[TOP_cmplt_ii_r] = Operator_;
  TOP2LAO[TOP_cmplt_r_b] = Operator_CODE_CMPLT_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmplt_r_r] = Operator_CODE_CMPLT_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpltu_i_b] = Operator_CODE_CMPLTU_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpltu_i_r] = Operator_CODE_CMPLTU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpltu_ii_b] = Operator_;
  TOP2LAO[TOP_cmpltu_ii_r] = Operator_;
  TOP2LAO[TOP_cmpltu_r_b] = Operator_CODE_CMPLTU_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpltu_r_r] = Operator_CODE_CMPLTU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpne_i_b] = Operator_CODE_CMPNE_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpne_i_r] = Operator_CODE_CMPNE_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_cmpne_ii_b] = Operator_;
  TOP2LAO[TOP_cmpne_ii_r] = Operator_;
  TOP2LAO[TOP_cmpne_r_b] = Operator_CODE_CMPNE_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_cmpne_r_r] = Operator_CODE_CMPNE_DEST_SRC1_SRC2;
  TOP2LAO[TOP_copy_br] = Operator_;
  TOP2LAO[TOP_dfixup] = Operator_;
  TOP2LAO[TOP_divs] = Operator_CODE_DIVS_DEST_BDEST_SRC1_SRC2_SCOND;
  TOP2LAO[TOP_end_pregtn] = Operator_;
  TOP2LAO[TOP_ffixup] = Operator_;
  TOP2LAO[TOP_fwd_bar] = Operator_;
  TOP2LAO[TOP_goto] = Operator_CODE_GOTO_BTARG;
  TOP2LAO[TOP_icall] = Operator_CODE_ICALL;
  TOP2LAO[TOP_ifixup] = Operator_;
  TOP2LAO[TOP_igoto] = Operator_CODE_IGOTO;
  TOP2LAO[TOP_imml] = Operator_CODE_IMML_IMM;
  TOP2LAO[TOP_immr] = Operator_CODE_IMMR_IMM;
  TOP2LAO[TOP_intrncall] = Operator_;
  TOP2LAO[TOP_label] = Operator_;
  TOP2LAO[TOP_ldb_d_i] = Operator_CODE_LDBD_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldb_d_ii] = Operator_;
  TOP2LAO[TOP_ldb_i] = Operator_CODE_LDB_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldb_ii] = Operator_;
  TOP2LAO[TOP_ldbu_d_i] = Operator_CODE_LDBUD_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldbu_d_ii] = Operator_;
  TOP2LAO[TOP_ldbu_i] = Operator_CODE_LDBU_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldbu_ii] = Operator_;
  TOP2LAO[TOP_ldh_d_i] = Operator_CODE_LDHD_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldh_d_ii] = Operator_;
  TOP2LAO[TOP_ldh_i] = Operator_CODE_LDH_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldh_ii] = Operator_;
  TOP2LAO[TOP_ldhu_d_i] = Operator_CODE_LDHUD_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldhu_d_ii] = Operator_;
  TOP2LAO[TOP_ldhu_i] = Operator_CODE_LDHU_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldhu_ii] = Operator_;
  TOP2LAO[TOP_ldw_d_i] = Operator_CODE_LDWD_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldw_d_ii] = Operator_;
  TOP2LAO[TOP_ldw_i] = Operator_CODE_LDW_IDESTL_ISRC2_SRC1;
  TOP2LAO[TOP_ldw_ii] = Operator_;
  TOP2LAO[TOP_max_i] = Operator_CODE_MAX_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_max_ii] = Operator_;
  TOP2LAO[TOP_max_r] = Operator_CODE_MAX_DEST_SRC1_SRC2;
  TOP2LAO[TOP_maxu_i] = Operator_CODE_MAXU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_maxu_ii] = Operator_;
  TOP2LAO[TOP_maxu_r] = Operator_CODE_MAXU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_mfb] = Operator_CODE_MFB_IDEST_SCOND;
  TOP2LAO[TOP_min_i] = Operator_CODE_MIN_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_min_ii] = Operator_;
  TOP2LAO[TOP_min_r] = Operator_CODE_MIN_DEST_SRC1_SRC2;
  TOP2LAO[TOP_minu_i] = Operator_CODE_MINU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_minu_ii] = Operator_;
  TOP2LAO[TOP_minu_r] = Operator_CODE_MINU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_mov_i] = Operator_CODE_MOV_IDEST_ISRC2;
  TOP2LAO[TOP_mov_ii] = Operator_;
  TOP2LAO[TOP_mov_r] = Operator_CODE_MOV_DEST_SRC2;
  TOP2LAO[TOP_mtb] = Operator_CODE_MTB_BDEST_SRC1;
  TOP2LAO[TOP_mulh_i] = Operator_CODE_MULH_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulh_ii] = Operator_;
  TOP2LAO[TOP_mulh_r] = Operator_CODE_MULH_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mulhh_i] = Operator_CODE_MULHH_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulhh_ii] = Operator_;
  TOP2LAO[TOP_mulhh_r] = Operator_CODE_MULHH_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mulhhu_i] = Operator_CODE_MULHHU_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulhhu_ii] = Operator_;
  TOP2LAO[TOP_mulhhu_r] = Operator_CODE_MULHHU_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mulhs_i] = Operator_CODE_MULHS_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulhs_ii] = Operator_;
  TOP2LAO[TOP_mulhs_r] = Operator_CODE_MULHS_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mulhu_i] = Operator_CODE_MULHU_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulhu_ii] = Operator_;
  TOP2LAO[TOP_mulhu_r] = Operator_CODE_MULHU_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mull_i] = Operator_CODE_MULL_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mull_ii] = Operator_;
  TOP2LAO[TOP_mull_r] = Operator_CODE_MULL_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mullh_i] = Operator_CODE_MULLH_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mullh_ii] = Operator_;
  TOP2LAO[TOP_mullh_r] = Operator_CODE_MULLH_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mullhu_i] = Operator_CODE_MULLHU_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mullhu_ii] = Operator_;
  TOP2LAO[TOP_mullhu_r] = Operator_CODE_MULLHU_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mulll_i] = Operator_CODE_MULLL_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulll_ii] = Operator_;
  TOP2LAO[TOP_mulll_r] = Operator_CODE_MULLL_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mulllu_i] = Operator_CODE_MULLLU_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mulllu_ii] = Operator_;
  TOP2LAO[TOP_mulllu_r] = Operator_CODE_MULLLU_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_mullu_i] = Operator_CODE_MULLU_IDESTL_SRC1_ISRC2;
  TOP2LAO[TOP_mullu_ii] = Operator_;
  TOP2LAO[TOP_mullu_r] = Operator_CODE_MULLU_DESTL_SRC1_SRC2;
  TOP2LAO[TOP_nandl_i_b] = Operator_CODE_NANDL_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_nandl_i_r] = Operator_CODE_NANDL_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_nandl_ii_b] = Operator_;
  TOP2LAO[TOP_nandl_ii_r] = Operator_;
  TOP2LAO[TOP_nandl_r_b] = Operator_CODE_NANDL_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_nandl_r_r] = Operator_CODE_NANDL_DEST_SRC1_SRC2;
  TOP2LAO[TOP_noop] = Operator_;
  TOP2LAO[TOP_nop] = Operator_CODE_NOP;
  TOP2LAO[TOP_norl_i_b] = Operator_CODE_NORL_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_norl_i_r] = Operator_CODE_NORL_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_norl_ii_b] = Operator_;
  TOP2LAO[TOP_norl_ii_r] = Operator_;
  TOP2LAO[TOP_norl_r_b] = Operator_CODE_NORL_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_norl_r_r] = Operator_CODE_NORL_DEST_SRC1_SRC2;
  TOP2LAO[TOP_or_i] = Operator_CODE_OR_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_or_ii] = Operator_;
  TOP2LAO[TOP_or_r] = Operator_CODE_OR_DEST_SRC1_SRC2;
  TOP2LAO[TOP_orc_i] = Operator_CODE_ORC_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_orc_ii] = Operator_;
  TOP2LAO[TOP_orc_r] = Operator_CODE_ORC_DEST_SRC1_SRC2;
  TOP2LAO[TOP_orl_i_b] = Operator_CODE_ORL_IBDEST_SRC1_ISRC2;
  TOP2LAO[TOP_orl_i_r] = Operator_CODE_ORL_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_orl_ii_b] = Operator_;
  TOP2LAO[TOP_orl_ii_r] = Operator_;
  TOP2LAO[TOP_orl_r_b] = Operator_CODE_ORL_BDEST_SRC1_SRC2;
  TOP2LAO[TOP_orl_r_r] = Operator_CODE_ORL_DEST_SRC1_SRC2;
  TOP2LAO[TOP_pft] = Operator_CODE_PFT_ISRC2_SRC1;
  TOP2LAO[TOP_phi] = Operator_;
  TOP2LAO[TOP_prgadd] = Operator_CODE_PRGADD_ISRC2_SRC1;
  TOP2LAO[TOP_prgins] = Operator_CODE_PRGINS;
  TOP2LAO[TOP_prgset] = Operator_CODE_PRGSET_ISRC2_SRC1;
  TOP2LAO[TOP_psi] = Operator_;
  TOP2LAO[TOP_return] = Operator_;
  TOP2LAO[TOP_rfi] = Operator_CODE_RFI;
  TOP2LAO[TOP_sbrk] = Operator_CODE_SBRK;
  TOP2LAO[TOP_sh1add_i] = Operator_CODE_SH1ADD_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_sh1add_ii] = Operator_;
  TOP2LAO[TOP_sh1add_r] = Operator_CODE_SH1ADD_DEST_SRC1_SRC2;
  TOP2LAO[TOP_sh2add_i] = Operator_CODE_SH2ADD_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_sh2add_ii] = Operator_;
  TOP2LAO[TOP_sh2add_r] = Operator_CODE_SH2ADD_DEST_SRC1_SRC2;
  TOP2LAO[TOP_sh3add_i] = Operator_CODE_SH3ADD_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_sh3add_ii] = Operator_;
  TOP2LAO[TOP_sh3add_r] = Operator_CODE_SH3ADD_DEST_SRC1_SRC2;
  TOP2LAO[TOP_sh4add_i] = Operator_CODE_SH4ADD_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_sh4add_ii] = Operator_;
  TOP2LAO[TOP_sh4add_r] = Operator_CODE_SH4ADD_DEST_SRC1_SRC2;
  TOP2LAO[TOP_shl_i] = Operator_CODE_SHL_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_shl_ii] = Operator_;
  TOP2LAO[TOP_shl_r] = Operator_CODE_SHL_DEST_SRC1_SRC2;
  TOP2LAO[TOP_shr_i] = Operator_CODE_SHR_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_shr_ii] = Operator_;
  TOP2LAO[TOP_shr_r] = Operator_CODE_SHR_DEST_SRC1_SRC2;
  TOP2LAO[TOP_shru_i] = Operator_CODE_SHRU_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_shru_ii] = Operator_;
  TOP2LAO[TOP_shru_r] = Operator_CODE_SHRU_DEST_SRC1_SRC2;
  TOP2LAO[TOP_slct_i] = Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRC2;
  TOP2LAO[TOP_slct_ii] = Operator_;
  TOP2LAO[TOP_slct_r] = Operator_CODE_SLCT_DEST_SCOND_SRC1_SRC2;
  TOP2LAO[TOP_slctf_i] = Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRC2;
  TOP2LAO[TOP_slctf_ii] = Operator_;
  TOP2LAO[TOP_slctf_r] = Operator_CODE_SLCTF_DEST_SCOND_SRC1_SRC2;
  TOP2LAO[TOP_spadjust] = Operator_;
  TOP2LAO[TOP_stb_i] = Operator_CODE_STB_ISRC2_SRC1_SRC2;
  TOP2LAO[TOP_stb_ii] = Operator_;
  TOP2LAO[TOP_sth_i] = Operator_CODE_STH_ISRC2_SRC1_SRC2;
  TOP2LAO[TOP_sth_ii] = Operator_;
  TOP2LAO[TOP_stw_i] = Operator_CODE_STW_ISRC2_SRC1_SRC2;
  TOP2LAO[TOP_stw_ii] = Operator_;
  TOP2LAO[TOP_sub_i] = Operator_CODE_SUB_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_sub_ii] = Operator_;
  TOP2LAO[TOP_sub_r] = Operator_CODE_SUB_DEST_SRC1_SRC2;
  TOP2LAO[TOP_sxtb_r] = Operator_CODE_SXTB_IDEST_SRC1;
  TOP2LAO[TOP_sxth_r] = Operator_CODE_SXTH_IDEST_SRC1;
  TOP2LAO[TOP_sync] = Operator_CODE_SYNC;
  TOP2LAO[TOP_syscall] = Operator_CODE_SYSCALL;
  TOP2LAO[TOP_xor_i] = Operator_CODE_XOR_IDEST_SRC1_ISRC2;
  TOP2LAO[TOP_xor_ii] = Operator_;
  TOP2LAO[TOP_xor_r] = Operator_CODE_XOR_DEST_SRC1_SRC2;
}

static void
TOP2LAO_FINI() {
}

static Operator
TOP_CGIR2LAO (TOP topcode) {
  return TOP2LAO[topcode];
}

static void
LAOstub_INIT() {

  MEM_POOL_Initialize ( &MEM_lao_stub_pool , "lao_stub_pool", false );
  TN2LIR_map = hTN_MAP_Create(&MEM_lao_stub_pool);

  TOP2LAO_INIT();
}

static void
LAOstub_FINI() {

  TOP2LAO_FINI();

  TN2LIR_map = NULL;
  MEM_POOL_Delete  ( &MEM_lao_stub_pool );
}

/* -----------------------------------------------------------------------
 * Functions to convert CGIR/TN to LIR2/LIR_OPERAND
 * -----------------------------------------------------------------------
 */

static LIR_OPERAND
TN_convert2LIR_reg ( TN *tn, int regindex, int regclass ) {
  LIR_OPERAND lirTN;

  Is_True(hTN_MAP_Get(TN2LIR_map, tn) == NULL, ("LAO TempName creation interface cannot be called twize with same TN."));

  if (TN_is_dedicated(tn))
    lirTN = Interface_makeDedicatedTempName(interface, tn, (Register)TN_register(tn));
  else
    lirTN = Interface_makePseudoRegTempName(interface, tn, (RegGroup)regclass);

  return lirTN;
}

static LIR_OPERAND
TN_convert2LIR_imm ( TN *tn, int immediate, int64_t value ) {
  LIR_OPERAND lirTN;

  Is_True(hTN_MAP_Get(TN2LIR_map, tn) == NULL, ("LAO TempName creation interface cannot be called twize with same TN."));

  lirTN = Interface_makeAbsoluteTempName(interface, tn, (Immediate)immediate, value);

  return lirTN;
}

static LIR_OPERAND
TN_convert2LIR_sym ( TN *tn, ST *var, int64_t offset ) {
  LIR_OPERAND lirTN;

  Is_True(hTN_MAP_Get(TN2LIR_map, tn) == NULL, ("LAO TempName creation interface cannot be called twize with same TN."));

  lirTN = Interface_makeSymbolTempName(interface, tn, (Immediate)0,
				       Interface_makeSymbol(interface, ST_name_idx(var), ST_name(var)),
				       offset);

  return lirTN;
}

static LIR_OPERAND
TN_convert2LIR_lab ( TN *tn, LABEL_IDX label, int64_t offset ) {
  LIR_OPERAND lirTN;

  Is_True(hTN_MAP_Get(TN2LIR_map, tn) == NULL, ("LAO TempName creation interface cannot be called twize with same TN."));

  lirTN = Interface_makeLabelTempName(interface, tn, (Immediate)0,
				      Interface_makeLabel(interface, label, LABEL_name(label)),
				      offset);

  return lirTN;
}

static LIR_OPERAND
TN_convert2TempName ( TN *tn ) {

  LIR_OPERAND lirTN;

  // First, check if a lirTN already exits for this TN.

  if ((lirTN = (LIR_OPERAND)hTN_MAP_Get(TN2LIR_map, tn)) != NULL)
    return lirTN;

  if (TN_is_constant(tn)) {

    if ( TN_has_value(tn))
      lirTN = TN_convert2LIR_imm (tn, 0/* immediate */, TN_value(tn));

    else if (TN_is_enum(tn)) {
      Is_True(FALSE, ("TN_is_enum not implemented."));
      //	  Interface_makeAbsoluteTempName(interface, tn, NULL, TN_enum(tn));
    }

    else if (TN_is_label(tn))
      lirTN = TN_convert2LIR_lab (tn, TN_label(tn), TN_offset(tn));

    else if (TN_is_tag(tn))
      lirTN = TN_convert2LIR_lab (tn, TN_label(tn), 0);

    else if (TN_is_symbol(tn))
      lirTN = TN_convert2LIR_sym (tn, TN_var(tn), TN_offset(tn));

    else {
      ErrMsg (EC_Unimplemented, "sPrint_TN: illegal constant TN");
    }
  }
  else {  /* register TN */
    lirTN = TN_convert2LIR_reg ( tn, TN_number(tn), TN_register_class(tn));
  }

  hTN_MAP_Set(TN2LIR_map, tn, (void *)lirTN);

  return lirTN;
}
static void
LIROP_addArgument ( LIR_OPERATION lirOP, TN *tn ) {
  LIR_OPERAND lirTN;

  lirTN = TN_convert2TempName(tn);
  Interface_appendOperationArgument(interface, lirOP, lirTN);
}

static void
LIROP_addResult ( LIR_OPERATION lirOP, TN *tn ) {
  LIR_OPERAND lirTN;

  lirTN = TN_convert2TempName(tn);
  Interface_appendOperationResult(interface, lirOP, lirTN);
}


/* -----------------------------------------------------------------------
 * Functions to convert CGIR/BB to/from LIR2/LIR_NODE
 * -----------------------------------------------------------------------
 */

typedef struct bbmap_ {
  BB *cgir;
  LIR_NODE lir2;
} BBmap_, *BBmap;



static BBmap_ *BB_map = NULL;
static int BB_map_SIZE = 0;

/* -----------------------------------------------------------------------
 * Associates a cgirBB to a lirBB through the BB_map array. BB_map is
 * an array of BB_map_, terminated by an element with NULL values.
 * -----------------------------------------------------------------------
 */
static void
BBmap_add ( BB * cgirBB, LIR_NODE lirBB ) {
  BBmap bbmap;
  int count = 1;

  if (BB_map_SIZE != 0) {
    for (bbmap = BB_map; bbmap->cgir; bbmap++, count ++);
  }

  Is_True((count <= BB_map_SIZE) || (BB_map_SIZE == 0), ("Inconsistent BB_map count"));

  if (count >= BB_map_SIZE) {
    int newSIZE = BB_map_SIZE + 10;
    BBmap newMap;
    newMap = TYPE_MEM_POOL_ALLOC_N(BBmap_, &MEM_lao_stub_pool, newSIZE);
    if (BB_map != NULL) {
      memcpy(newMap, BB_map, sizeof(BBmap_)*count);
      MEM_POOL_FREE(&MEM_lao_stub_pool, BB_map);
    }
    BB_map = newMap;
  }

  BB_map[count-1].cgir = cgirBB;
  BB_map[count-1].lir2 = lirBB;
  BB_map[count].cgir = NULL;
  BB_map[count].lir2 = NULL;
}

/* -----------------------------------------------------------------------
 * Return the CGIR/LAO node associated with the given LAO/CGIR node
 * -----------------------------------------------------------------------
 */
static LIR_NODE
BB_CGIR2LAO ( BB * node ) {
  BBmap bbmap;

  for (bbmap = BB_map; bbmap->cgir; bbmap++) {
    if (bbmap->cgir == node)
      return bbmap->lir2;
  }

  return NULL;
}

static BB *
BB_LAO2CGIR ( LIR_NODE node ) {
  BBmap bbmap;

  for (bbmap = BB_map; bbmap->cgir; bbmap++) {
    if (bbmap->lir2 == node)
      return bbmap->cgir;
  }

  Is_True(FALSE, ("Could not find CGIR BB for BasicBlock:%p", node));
}

static LIR_NODE
BB_convert2LIR ( BB *bb ) {
  LIR_NODE lirBB;
  int i;

  fprintf(TFile, "Starting BB_convert2LIR for %d\n", BB_id(bb));

  lirBB = Interface_makeBasicBlock ( interface, bb );

  BBmap_add(bb, lirBB);

  // Now, fill the basic blocks with labels, operations and branches

  if (BB_has_label(bb)) {
    ANNOTATION *ant;
    for (ant = ANNOT_First(BB_annotations(bb), ANNOT_LABEL);
	 ant != NULL;
	 ant = ANNOT_Next(ant, ANNOT_LABEL)) {
      LABEL_IDX label = ANNOT_label(ant);
      Interface_appendBasicBlockLabel(interface, lirBB, Interface_makeLabel(interface, label, LABEL_name(label)));
    }
  }

  const OP *op;
  FOR_ALL_BB_OPs (bb, op) {
    LIR_OPERATION lirOP;

    lirOP = Interface_makeOperation(interface, (void *)op, TOP_CGIR2LAO(OP_code(op)),
				    OP_opnds(op), OP_results(op));

    Interface_appendBasicBlockOperation(interface, lirBB, lirOP);

    for (i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op, i);
      LIROP_addArgument(lirOP, tn);
    }

    for (i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op, i);
      LIROP_addResult(lirOP, tn);
    }
  }
#if 0
  // Set live-in and live-out information on basic blocks.
  for (TN *tn = GTN_SET_Choose(BB_live_in(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_in(bb), tn)) {
    Interface_setLiveIn(interface, lirBB, TN_convert2TempName(tn));
  }

  for (TN *tn = GTN_SET_Choose(BB_live_out(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_out(bb), tn)) {
    Interface_setLiveOut(interface, lirBB, TN_convert2TempName(tn));
  }
#endif
  fprintf(TFile, "Completed BB_convert2LIR for %d\n", BB_id(bb));
}

static bool
BB_inRegion ( BB *regionBBs[], BB *bb ) {
  int i;

  for (i = 0; regionBBs[i]; i++) {
    if (regionBBs[i] == bb)
      return true;
  }

  return false;
}

static void
BB_LIRcreateEdges ( BB *src, BB *dst ) {
  LIR_NODE srcLIR, dstLIR;

  srcLIR = BB_CGIR2LAO(src);
  dstLIR = BB_CGIR2LAO(dst);

  Is_True((src != NULL) && (dst != NULL), ("BB_map internal ERROR."));

  Interface_makeControlArc(interface, srcLIR, dstLIR);
}

static LIR_REGION
REGION_convert2LIR ( BB ** entryBBs, BB ** exitBBs, BB ** regionBBs ) {
  int i;

  // First, create LIR basic blocks, and mark entry and exit nodes.

  for (i = 0; regionBBs[i]; i++) {
    BB_convert2LIR ( regionBBs[i] );
  }

  for (i = 0; entryBBs[i]; i++) {
    Interface_setEntry(interface, BB_CGIR2LAO(entryBBs[i]));
  }

  for (i = 0; exitBBs[i]; i++) {
    Interface_setExit(interface, BB_CGIR2LAO(exitBBs[i]));
  }

  // Then, create the control-flow edges between them.
  BBLIST *bl;
  for (i = 0; regionBBs[i]; i++) {
    BB *src, *dst;
    src = regionBBs[i];
    FOR_ALL_BB_SUCCS (src, bl) {
      dst = BBLIST_item(bl);

      if (BB_inRegion(regionBBs, dst))
	BB_LIRcreateEdges ( src, dst );
    }
  }

  return NULL;
}

static void LAOS_printBB (BB *bp);

bool LAO_scheduleRegion ( BB ** entryBBs, BB ** exitBBs, BB ** regionBBs , LAO_SWP_ACTION action ) {
  int i;
  BB *bb;
  LIR_REGION lir_region;
  int status;
  
  fprintf(TFile, "---- Before LAO schedule region ----\n");
  fprintf(TFile, "---- Begin trace regionBBs ----\n");
  for (i = 0; regionBBs[i]; i ++) {
    bb = regionBBs[i];
    LAOS_printBB(bb);
  }    
  fprintf(TFile, "---- End trace regionBBs ----\n");

  LIR2_INIT();
  LAOstub_INIT();

  lir_region = REGION_convert2LIR(entryBBs, exitBBs, regionBBs);

  //  status = REGION_schedule(lir_region);

  if (status == 0) {
    //    REGION_convert2CGIR(lir_region);
  }

  LAOstub_FINI();
  LIR2_FINI();
  
  fprintf(TFile, "---- After LAO schedule region ----\n");
  return (status == 0);
}

bool Perform_SWP ( LOOP_DESCR * loop , LAO_SWP_ACTION action )
{

  BB *bb;
  BB **entryBBs, **exitBBs, **regionBBs;
  bool res;

  fprintf(TFile, "---- Before LAO schedule loop ----\n");
  fprintf(TFile, "     ----- LOOP id %2d -----\n", BB_id(LOOP_DESCR_loophead(loop)));
  fprintf(TFile, "           ----------\n");

  FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(loop), bb) LAOS_printBB(bb);

  switch (action) {
  case SINGLE_BB_DOLOOP_SWP:
  case SINGLE_BB_DOLOOP_UNROLL:

    entryBBs  = (BB **)alloca(sizeof(BB *)*2);
    exitBBs   = (BB **)alloca(sizeof(BB *)*2);
    regionBBs = (BB **)alloca(sizeof(BB *)*2);

    entryBBs[0] = exitBBs[0] = regionBBs[0] = LOOP_DESCR_loophead(loop);
    entryBBs[1] = exitBBs[1] = regionBBs[1] = NULL;
      
    res = LAO_scheduleRegion ( entryBBs, exitBBs, regionBBs, action );

    return res;

  case SINGLE_BB_WHILELOOP_SWP:
    return false;
  case SINGLE_BB_WHILELOOP_UNROLL:
    return false;
  case MULTI_BB_DOLOOP:
    return false;
  case NO_LOOP_OPT:
    return false;
  default:
    Is_True(FALSE, ("unknown loop opt action."));
  }

  return TRUE;
}


/* -----------------------------------------------------------------------
 * Utility routines to print CGIR structures
 * -----------------------------------------------------------------------
 */
void LAOS_printBB_Header (BB *bp)
{
  BBLIST *bl;
  INT16 i;
  ANNOTATION *annot = ANNOT_Get(BB_annotations(bp), ANNOT_LOOPINFO);
  BOOL freqs = FREQ_Frequencies_Computed();

  if ( BB_entry(bp) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bp), ANNOT_ENTRYINFO);
    ENTRYINFO *ent = ANNOT_entryinfo (ant);
    OP *sp_adj = BB_entry_sp_adj_op(bp);
    Is_True ((sp_adj == ENTRYINFO_sp_adj(ent)),("bad sp_adj"));

    fprintf ( TFile, "Entrypoint: %s\t Starting Line %d\n",
	      ST_name(ENTRYINFO_name(ent)),
	      Srcpos_To_Line(ENTRYINFO_srcpos(ent)));

    if (sp_adj) {
      OP *op;
      BOOL found_sp_adj = FALSE;
      fprintf ( TFile, "SP entry adj: " );
      Print_OP_No_SrcLine (sp_adj);
      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( TFile, "******** ERROR ******** sp adjust not found in entry block\n");
    }
  }

  if ( BB_exit(bp) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bp), ANNOT_EXITINFO);
    EXITINFO *exit = ANNOT_exitinfo (ant);
    OP *sp_adj = BB_exit_sp_adj_op(bp);
    Is_True ((sp_adj == EXITINFO_sp_adj(exit)),("bad sp_adj"));

    if (sp_adj) {
      OP *op;
      BOOL found_sp_adj = FALSE;
      fprintf ( TFile, "SP exit adj: " );
      Print_OP_No_SrcLine (sp_adj);

      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( TFile, "******** ERROR ******** sp adjust not found in exit block\n");
    }
  }

  fprintf ( TFile, "    BB %d, flags 0x%04x",
	    BB_id(bp), BB_flag(bp) );

  if (freqs || BB_freq_fb_based(bp))
    fprintf(TFile, ", freq %g (%s)", BB_freq(bp),
	    BB_freq_fb_based(bp) ? "feedback" : "heuristic");
  
  if (BB_unreachable(bp)) fprintf ( TFile, ", Unreachable");
  if (BB_entry(bp))	fprintf ( TFile, ", Entry" );
  if (BB_handler(bp))	fprintf ( TFile, ", Handler" );
  if (BB_asm(bp)) 	fprintf ( TFile, ", Asm" );

  if (BB_exit(bp)) {
    if (BB_call(bp))	fprintf ( TFile, ", Tail-call" );
    else		fprintf ( TFile, ", Exit" );
  } else if (BB_call(bp)) fprintf ( TFile, ", Call" );

  if (BB_rid(bp)) {
    INT exits;
    RID *rid = BB_rid(bp);
    CGRIN *cgrin = RID_cginfo(rid);
    if (cgrin) {
      if (bp == CGRIN_entry(cgrin)) {
	fprintf ( TFile, ", Region-entry " );
      }
      exits = RID_num_exits(rid);
      for (i = 0; i < exits; ++i) {
	if (bp == CGRIN_exit_i(cgrin, i)) {
	  fprintf ( TFile, ", Region-exit[%d]", i );
	}
      }
    }
  }

  fprintf ( TFile, "\n");

  if (annot)
    Print_LOOPINFO(ANNOT_loopinfo(annot));

  if (BB_loop_head_bb(bp)) {
    if (BB_loophead(bp)) {
      if (!annot) {
	fprintf(TFile, "\tHead of loop body line %d\n", BB_Loop_Lineno(bp));
      }
    } else {
      BB *head = BB_loop_head_bb(bp);
      fprintf(TFile,
	      "\tPart of loop body starting at line %d with head BB:%d\n",
	      BB_Loop_Lineno(head), BB_id(head));
    }
  }

  if (BB_unrollings(bp) > 1)
    fprintf(TFile, "\tUnrolled %d times%s\n", BB_unrollings(bp),
	    BB_unrolled_fully(bp) ? " (fully)" : "");

  if ( BB_rid(bp) )
    RID_Fprint( TFile, BB_rid(bp) );

  fprintf ( TFile, "\tpred" );
  FOR_ALL_BB_PREDS (bp, bl) {
    fprintf ( TFile, " %d", BB_id(BBLIST_item(bl)));
  }

  fprintf ( TFile, "\n\tsucc%s", freqs ? " (w/probs)" : "" );
  FOR_ALL_BB_SUCCS (bp, bl) {
    fprintf ( TFile, " %d",
	      BB_id(BBLIST_item(bl)));
    if (freqs) fprintf(TFile, "(%g)", BBLIST_prob(bl));
  }
  fprintf ( TFile, "\n" );

  if (BB_has_label(bp)) {
    ANNOTATION *ant;
    fprintf(TFile, "\tLabel");
    for (ant = ANNOT_First(BB_annotations(bp), ANNOT_LABEL);
	 ant != NULL;
	 ant = ANNOT_Next(ant, ANNOT_LABEL))
      {
	INT eh_labs = 0;
	LABEL_IDX label = ANNOT_label(ant);
	fprintf (TFile," %s", LABEL_name(label));
	FmtAssert((Get_Label_BB(label) == bp),
		  (" Inconsistent ST for BB:%2d label", BB_id(bp)));
	switch (LABEL_kind(Label_Table[label])) {
	case LKIND_BEGIN_EH_RANGE:
	  fprintf (TFile,"%cbegin_eh_range", eh_labs++ ? ' ' : '(');
	  break;
	case LKIND_END_EH_RANGE:
	  fprintf (TFile,"%cend_eh_range", eh_labs++ ? ' ' : '(');
	  break;
	}
	if (eh_labs)
	  fprintf (TFile,")");
      }
    fprintf(TFile, "\n");
  }

  return;
}

void LAOS_printTN ( const TN *tn )
{

  if (TN_is_constant(tn)) {
    if ( TN_has_value(tn)) {
      fprintf ( TFile, "(0x%llx)", TN_value(tn) );
      if (TN_size(tn) == 4 && 
	  TN_value(tn) >> 32 != 0 &&
	  TN_value(tn) >> 31 != -1)
	fprintf ( TFile, "!!! TN_value=0x%llx is too big to fit in a word",
		  TN_value(tn));
    }
    else if (TN_is_enum(tn)) {
      fprintf ( TFile, "(enum:%s)", ISA_ECV_Name(TN_enum(tn)) );
    }
    else if ( TN_is_label(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      INT64 offset = TN_offset(tn);
      BB *targetBB;
      if ( offset == 0 ) {
	fprintf ( TFile, "(lab:%s)", name );
      }
      else {
	fprintf ( TFile, "(lab:%s+%lld)", name, offset );
      }
      targetBB = Get_Label_BB(lab);
      if (targetBB != NULL)
	fprintf(TFile, " --> %d", BB_id(targetBB));
    } 
    else if ( TN_is_tag(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      fprintf ( TFile, "(tag:%s)", name );
    }
    else if ( TN_is_symbol(tn) ) {
      ST *var = TN_var(tn);

      fprintf ( TFile, "(sym" );
      fprintf ( TFile, TN_RELOCS_Name(TN_relocs(tn)) );

      if (ST_class(var) == CLASS_CONST)
      	fprintf ( TFile, ":%s)", Targ_Print(NULL, ST_tcon_val(var)));
      else
      	fprintf ( TFile, ":%s%+lld)", ST_name(var), TN_offset(tn) );
    } 
    else {
      ErrMsg (EC_Unimplemented, "sPrint_TN: illegal constant TN");
    }
  }
  else {  /* register TN */
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
	fprintf ( TFile, "%s", 
		  REGISTER_name(TN_register_class(tn), TN_register(tn)));
      } else {
	fprintf ( TFile, "(%d,%d)", TN_register_class(tn), TN_register(tn));
      }
    }
    else if (TN_is_global_reg(tn)) {
      fprintf ( TFile, "G%d", TN_number(tn) );
    }
    else {
      fprintf ( TFile, "T%d", TN_number(tn) );
    }
    if (TN_is_save_reg(tn)) {
      fprintf ( TFile, "(sv:%s)", 
		REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
  }
}

void LAOS_printOP ( const OP *op )
{
  int i;

  if (OP_has_tag(op)) {
    LABEL_IDX tag = Get_OP_Tag(op);
    fprintf (TFile, "<tag %s>: ", LABEL_name(tag));
  }

  fprintf(TFile, "%s", TOP_Name(OP_code(op)));

  if ( OP_variant(op) != 0 ) {
    fprintf ( TFile, "(%x)", OP_variant(op));
  }

  if (OP_results(op) == 0)
    fprintf(TFile, " void");

  else for (i = 0; i < OP_results(op); i++) {
    fprintf(TFile, "%s", (i == 0 ? " " : ", "));
    LAOS_printTN(OP_result(op,i));
  }

  fprintf(TFile, " =");

  for (i=0; i<OP_opnds(op); i++) {
    fprintf(TFile, "%s", (i == 0 ? " " : ", "));
    TN *tn = OP_opnd(op,i);
    LAOS_printTN(tn);
    if (OP_Defs_TN(op, tn)) fprintf(TFile, "<def>");
  }
}

void LAOS_printOPs ( const OP *op )
{
  for ( ; op; op = OP_next(op)) {
    fprintf(TFile, "\t");
    LAOS_printOP(op);
    fprintf(TFile, "       \t#line[%4d]", Srcpos_To_Line(OP_srcpos(op)));
    fprintf(TFile, "\n");
  }
}

void LAOS_printBB (BB *bp)
{
  LAOS_printBB_Header (bp );
  if (BB_first_op(bp))	LAOS_printOPs (BB_first_op(bp));
}

typedef struct OP_list {
  OP *op;
  struct OP_list *next;
} OP_list;

static OP_list * OP_list_new(OP_list *head)
{
  OP_list * elem;

  elem = (OP_list *)malloc(sizeof(OP_list));
  elem->next = head;
  head = elem;
  return head;
}

void LAOS_printAlias()
{
  OP_list *memops = NULL, *elt1, *elt2;
  BB *bp;
  OP *op;
  BOOL alias, identical;

  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    for (op = BB_first_op(bp); op; op = OP_next(op)) {
      if (OP_memory(op)) {
	memops = OP_list_new(memops);
	memops->op = op;
      }
    }
  }

  for (elt1 = memops; elt1; elt1 = elt1->next) {
    fprintf(TFile, "<Alias>"); LAOS_printOP(elt1->op); fprintf(TFile, "\n");
    for (elt2 = memops; elt2 != elt1; elt2 = elt2->next) {
      fprintf(TFile, "\t<with>"); LAOS_printOP(elt2->op); fprintf(TFile, "\t");
      alias = CG_DEP_Mem_Ops_Alias(elt1->op, elt2->op, &identical);
      if (!alias)          fprintf(TFile, "NO-ALIAS");
      else if (!identical) fprintf(TFile, "   ALIAS");
      else                 fprintf(TFile, "IDENTICAL");
      fprintf(TFile, "</with>\n");
    }
    fprintf(TFile, "</Alias>\n");
  }
}

void LAOS_printCGIR()
{
  BB *bp;

  fprintf(TFile, "--------CFG Begin--------\n");
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    LAOS_printBB ( bp );
    fprintf ( TFile,"\n" );
  }

  //  LAOS_printAlias();

  // Print live-analysis information

  fprintf(TFile, "-------- CFG End --------\n");
}
