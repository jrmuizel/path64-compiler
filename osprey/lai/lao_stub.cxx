#include <stdio.h>
#include <vector.h>

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

#include "lao_stub.h"

extern "C" {

#define this THIS
#define operator OPERATOR
#define __STDBOOL_H__

#include "LIR.h"

#undef operator
#undef this

}

typedef list<BB*> BB_List;

// Test if a BB belongs to a BB_List.
static bool
LAO_inBB_List(BB_List& bb_list, BB *bb) {
  //
  BB_List::iterator bb_iter;
  for (bb_iter = bb_list.begin(); bb_iter != bb_list.end(); bb_iter++) {
    if (*bb_iter == bb) return true;
  }
  //
  return false;
}

static void
LAOS_printCGIR();

// Map CGIR TOP to LIR Operator.
static Operator TOP__Operator[TOP_UNDEFINED];

// Map CGIR ISA_ENUM_CLASS to LIR Modifier.
static Modifier IEC__Modifier[EC_MAX];

// Map CGIR Literal to LIR Immediate.
static Immediate LC__Immediate[LC_MAX];

// Map CGIR ISA_REGISTER_CLASS to LIR RegClass.
// WARNING! ISA_REGISTER_CLASS reaches ISA_REGISTER_CLASS_MAX
static RegClass IRC__RegClass[ISA_REGISTER_CLASS_MAX+1];

// Memory pool local to the stub
static MEM_POOL MEM_lao_pool;

// Variable used to skip multiple LAO_INIT / LAO_FINI calls
static int LAO_initialized = 0;

extern "C" {
#include <unistd.h>
}

typedef vector<BB*> BB_VECTOR;

/*--------------------------- LAO_INIT / LAO_FINI ----------------------------*/

// Initialization of the LAO, needs to be called once.
void
LAO_INIT() {
#ifndef LAO_EXPERIMENT
#endif //LAO_EXPERIMENT
  int dummy; fprintf(stderr, "LAO PID=%lld\n", (int64_t)getpid()); scanf("%d", &dummy);
  if (LAO_initialized++ == 0) {
    // initialize LIR
    LIR_INIT();
    // initialize TOP__Operator
    for (int i = 0; i < TOP_UNDEFINED; i++) TOP__Operator[i] = Operator_;
    TOP__Operator[TOP_add_i] = Operator_CODE_ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_add_ii] = Operator_CODE_ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_add_r] = Operator_CODE_ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_addcg] = Operator_CODE_ADDCG_DEST_BDEST_SRC1_SRC2_SCOND;
    TOP__Operator[TOP_and_i] = Operator_CODE_AND_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_and_ii] = Operator_CODE_AND_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_and_r] = Operator_CODE_AND_DEST_SRC1_SRC2;
    TOP__Operator[TOP_andc_i] = Operator_CODE_ANDC_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_andc_ii] = Operator_CODE_ANDC_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_andc_r] = Operator_CODE_ANDC_DEST_SRC1_SRC2;
    TOP__Operator[TOP_andl_i_b] = Operator_CODE_ANDL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_andl_ii_b] = Operator_CODE_ANDL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_andl_i_r] = Operator_CODE_ANDL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_andl_ii_r] = Operator_CODE_ANDL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_andl_r_b] = Operator_CODE_ANDL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_andl_r_r] = Operator_CODE_ANDL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_asm] = Operator_CODE_ASM15_DEST_SRC1_SRC2;
    TOP__Operator[TOP_begin_pregtn] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_br] = Operator_CODE_BR_BCOND_BTARG;
    TOP__Operator[TOP_break] = Operator_CODE_BREAK;
    TOP__Operator[TOP_brf] = Operator_CODE_BRF_BCOND_BTARG;
    TOP__Operator[TOP_bswap_r] = Operator_CODE_BSWAP_IDEST_SRC1;
    TOP__Operator[TOP_bwd_bar] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_call] = Operator_CODE_CALL_BTARG;
    TOP__Operator[TOP_cmpeq_i_b] = Operator_CODE_CMPEQ_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpeq_ii_b] = Operator_CODE_CMPEQ_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpeq_i_r] = Operator_CODE_CMPEQ_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpeq_ii_r] = Operator_CODE_CMPEQ_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpeq_r_b] = Operator_CODE_CMPEQ_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpeq_r_r] = Operator_CODE_CMPEQ_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpge_i_b] = Operator_CODE_CMPGE_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpge_ii_b] = Operator_CODE_CMPGE_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpge_i_r] = Operator_CODE_CMPGE_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpge_ii_r] = Operator_CODE_CMPGE_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpge_r_b] = Operator_CODE_CMPGE_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpge_r_r] = Operator_CODE_CMPGE_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgeu_i_b] = Operator_CODE_CMPGEU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgeu_ii_b] = Operator_CODE_CMPGEU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgeu_i_r] = Operator_CODE_CMPGEU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgeu_ii_r] = Operator_CODE_CMPGEU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgeu_r_b] = Operator_CODE_CMPGEU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgeu_r_r] = Operator_CODE_CMPGEU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgt_i_b] = Operator_CODE_CMPGT_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgt_ii_b] = Operator_CODE_CMPGT_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgt_i_r] = Operator_CODE_CMPGT_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgt_ii_r] = Operator_CODE_CMPGT_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgt_r_b] = Operator_CODE_CMPGT_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgt_r_r] = Operator_CODE_CMPGT_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgtu_i_b] = Operator_CODE_CMPGTU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgtu_ii_b] = Operator_CODE_CMPGTU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgtu_i_r] = Operator_CODE_CMPGTU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgtu_ii_r] = Operator_CODE_CMPGTU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgtu_r_b] = Operator_CODE_CMPGTU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgtu_r_r] = Operator_CODE_CMPGTU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmple_i_b] = Operator_CODE_CMPLE_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmple_ii_b] = Operator_CODE_CMPLE_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmple_i_r] = Operator_CODE_CMPLE_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmple_ii_r] = Operator_CODE_CMPLE_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmple_r_b] = Operator_CODE_CMPLE_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmple_r_r] = Operator_CODE_CMPLE_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpleu_i_b] = Operator_CODE_CMPLEU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpleu_ii_b] = Operator_CODE_CMPLEU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpleu_i_r] = Operator_CODE_CMPLEU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpleu_ii_r] = Operator_CODE_CMPLEU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpleu_r_b] = Operator_CODE_CMPLEU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpleu_r_r] = Operator_CODE_CMPLEU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmplt_i_b] = Operator_CODE_CMPLT_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmplt_ii_b] = Operator_CODE_CMPLT_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmplt_i_r] = Operator_CODE_CMPLT_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmplt_ii_r] = Operator_CODE_CMPLT_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmplt_r_b] = Operator_CODE_CMPLT_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmplt_r_r] = Operator_CODE_CMPLT_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpltu_i_b] = Operator_CODE_CMPLTU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpltu_ii_b] = Operator_CODE_CMPLTU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpltu_i_r] = Operator_CODE_CMPLTU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpltu_ii_r] = Operator_CODE_CMPLTU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpltu_r_b] = Operator_CODE_CMPLTU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpltu_r_r] = Operator_CODE_CMPLTU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpne_i_b] = Operator_CODE_CMPNE_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpne_ii_b] = Operator_CODE_CMPNE_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpne_i_r] = Operator_CODE_CMPNE_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpne_ii_r] = Operator_CODE_CMPNE_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpne_r_b] = Operator_CODE_CMPNE_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpne_r_r] = Operator_CODE_CMPNE_DEST_SRC1_SRC2;
    TOP__Operator[TOP_copy_br] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_dfixup] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_divs] = Operator_CODE_DIVS_DEST_BDEST_SRC1_SRC2_SCOND;
    TOP__Operator[TOP_end_pregtn] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_ffixup] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_fwd_bar] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_goto] = Operator_CODE_GOTO_BTARG;
    TOP__Operator[TOP_icall] = Operator_CODE_ICALL;
    TOP__Operator[TOP_ifixup] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_igoto] = Operator_CODE_IGOTO;
    TOP__Operator[TOP_imml] = Operator_CODE_IMML_IMM;
    TOP__Operator[TOP_immr] = Operator_CODE_IMMR_IMM;
    TOP__Operator[TOP_intrncall] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_label] = Operator_PSEUDO_LABEL;
    TOP__Operator[TOP_ldb_d_i] = Operator_CODE_LDBD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldb_d_ii] = Operator_CODE_LDBD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldb_i] = Operator_CODE_LDB_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldb_ii] = Operator_CODE_LDB_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldbu_d_i] = Operator_CODE_LDBUD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldbu_d_ii] = Operator_CODE_LDBUD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldbu_i] = Operator_CODE_LDBU_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldbu_ii] = Operator_CODE_LDBU_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldh_d_i] = Operator_CODE_LDHD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldh_d_ii] = Operator_CODE_LDHD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldh_i] = Operator_CODE_LDH_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldh_ii] = Operator_CODE_LDH_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldhu_d_i] = Operator_CODE_LDHUD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldhu_d_ii] = Operator_CODE_LDHUD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldhu_i] = Operator_CODE_LDHU_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldhu_ii] = Operator_CODE_LDHU_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldw_d_i] = Operator_CODE_LDWD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldw_d_ii] = Operator_CODE_LDWD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldw_i] = Operator_CODE_LDW_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldw_ii] = Operator_CODE_LDW_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_max_i] = Operator_CODE_MAX_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_max_ii] = Operator_CODE_MAX_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_max_r] = Operator_CODE_MAX_DEST_SRC1_SRC2;
    TOP__Operator[TOP_maxu_i] = Operator_CODE_MAXU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_maxu_ii] = Operator_CODE_MAXU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_maxu_r] = Operator_CODE_MAXU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_mfb] = Operator_CODE_MFB_IDEST_SCOND;
    TOP__Operator[TOP_min_i] = Operator_CODE_MIN_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_min_ii] = Operator_CODE_MIN_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_min_r] = Operator_CODE_MIN_DEST_SRC1_SRC2;
    TOP__Operator[TOP_minu_i] = Operator_CODE_MINU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_minu_ii] = Operator_CODE_MINU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_minu_r] = Operator_CODE_MINU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_mov_i] = Operator_CODE_MOV_IDEST_ISRC2;
    TOP__Operator[TOP_mov_ii] = Operator_CODE_MOV_IDEST_ISRCX;
    TOP__Operator[TOP_mov_r] = Operator_CODE_MOV_DEST_SRC2;
    TOP__Operator[TOP_mtb] = Operator_CODE_MTB_BDEST_SRC1;
    TOP__Operator[TOP_mulh_i] = Operator_CODE_MULH_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulh_ii] = Operator_CODE_MULH_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulh_r] = Operator_CODE_MULH_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhh_i] = Operator_CODE_MULHH_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhh_ii] = Operator_CODE_MULHH_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhh_r] = Operator_CODE_MULHH_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhhu_i] = Operator_CODE_MULHHU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhhu_ii] = Operator_CODE_MULHHU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhhu_r] = Operator_CODE_MULHHU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhs_i] = Operator_CODE_MULHS_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhs_ii] = Operator_CODE_MULHS_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhs_r] = Operator_CODE_MULHS_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhu_i] = Operator_CODE_MULHU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhu_ii] = Operator_CODE_MULHU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhu_r] = Operator_CODE_MULHU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mull_i] = Operator_CODE_MULL_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mull_ii] = Operator_CODE_MULL_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mull_r] = Operator_CODE_MULL_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mullh_i] = Operator_CODE_MULLH_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mullh_ii] = Operator_CODE_MULLH_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mullh_r] = Operator_CODE_MULLH_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mullhu_i] = Operator_CODE_MULLHU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mullhu_ii] = Operator_CODE_MULLHU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mullhu_r] = Operator_CODE_MULLHU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulll_i] = Operator_CODE_MULLL_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulll_ii] = Operator_CODE_MULLL_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulll_r] = Operator_CODE_MULLL_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulllu_i] = Operator_CODE_MULLLU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulllu_ii] = Operator_CODE_MULLLU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulllu_r] = Operator_CODE_MULLLU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mullu_i] = Operator_CODE_MULLU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mullu_ii] = Operator_CODE_MULLU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mullu_r] = Operator_CODE_MULLU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_nandl_i_b] = Operator_CODE_NANDL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_nandl_ii_b] = Operator_CODE_NANDL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_nandl_i_r] = Operator_CODE_NANDL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_nandl_ii_r] = Operator_CODE_NANDL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_nandl_r_b] = Operator_CODE_NANDL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_nandl_r_r] = Operator_CODE_NANDL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_noop] = Operator_PSEUDO_NOP;
    TOP__Operator[TOP_nop] = Operator_CODE_NOP;
    TOP__Operator[TOP_norl_i_b] = Operator_CODE_NORL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_norl_ii_b] = Operator_CODE_NORL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_norl_i_r] = Operator_CODE_NORL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_norl_ii_r] = Operator_CODE_NORL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_norl_r_b] = Operator_CODE_NORL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_norl_r_r] = Operator_CODE_NORL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_or_i] = Operator_CODE_OR_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_or_ii] = Operator_CODE_OR_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_or_r] = Operator_CODE_OR_DEST_SRC1_SRC2;
    TOP__Operator[TOP_orc_i] = Operator_CODE_ORC_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_orc_ii] = Operator_CODE_ORC_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_orc_r] = Operator_CODE_ORC_DEST_SRC1_SRC2;
    TOP__Operator[TOP_orl_i_b] = Operator_CODE_ORL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_orl_ii_b] = Operator_CODE_ORL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_orl_i_r] = Operator_CODE_ORL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_orl_ii_r] = Operator_CODE_ORL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_orl_r_b] = Operator_CODE_ORL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_orl_r_r] = Operator_CODE_ORL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_pft] = Operator_CODE_PFT_ISRC2_SRC1;
    TOP__Operator[TOP_phi] = Operator_PSEUDO_PHI;
    TOP__Operator[TOP_prgadd] = Operator_CODE_PRGADD_ISRC2_SRC1;
    TOP__Operator[TOP_prgins] = Operator_CODE_PRGINS;
    TOP__Operator[TOP_prgset] = Operator_CODE_PRGSET_ISRC2_SRC1;
    TOP__Operator[TOP_psi] = Operator_PSEUDO_PSI;
    TOP__Operator[TOP_return] = Operator_MACRO_RETURN;
    TOP__Operator[TOP_rfi] = Operator_CODE_RFI;
    TOP__Operator[TOP_sbrk] = Operator_CODE_SBRK;
    TOP__Operator[TOP_sh1add_i] = Operator_CODE_SH1ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh1add_ii] = Operator_CODE_SH1ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh1add_r] = Operator_CODE_SH1ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sh2add_i] = Operator_CODE_SH2ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh2add_ii] = Operator_CODE_SH2ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh2add_r] = Operator_CODE_SH2ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sh3add_i] = Operator_CODE_SH3ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh3add_ii] = Operator_CODE_SH3ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh3add_r] = Operator_CODE_SH3ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sh4add_i] = Operator_CODE_SH4ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh4add_ii] = Operator_CODE_SH4ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh4add_r] = Operator_CODE_SH4ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_shl_i] = Operator_CODE_SHL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_shl_ii] = Operator_CODE_SHL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_shl_r] = Operator_CODE_SHL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_shr_i] = Operator_CODE_SHR_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_shr_ii] = Operator_CODE_SHR_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_shr_r] = Operator_CODE_SHR_DEST_SRC1_SRC2;
    TOP__Operator[TOP_shru_i] = Operator_CODE_SHRU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_shru_ii] = Operator_CODE_SHRU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_shru_r] = Operator_CODE_SHRU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_slct_i] = Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRC2;
    TOP__Operator[TOP_slct_ii] = Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRCX;
    TOP__Operator[TOP_slct_r] = Operator_CODE_SLCT_DEST_SCOND_SRC1_SRC2;
    TOP__Operator[TOP_slctf_i] = Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRC2;
    TOP__Operator[TOP_slctf_ii] = Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRCX;
    TOP__Operator[TOP_slctf_r] = Operator_CODE_SLCTF_DEST_SCOND_SRC1_SRC2;
    TOP__Operator[TOP_spadjust] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_stb_i] = Operator_CODE_STB_ISRC2_SRC1_SRC2;
    TOP__Operator[TOP_stb_ii] = Operator_CODE_STB_ISRCX_SRC1_SRC2;
    TOP__Operator[TOP_sth_i] = Operator_CODE_STH_ISRC2_SRC1_SRC2;
    TOP__Operator[TOP_sth_ii] = Operator_CODE_STH_ISRCX_SRC1_SRC2;
    TOP__Operator[TOP_stw_i] = Operator_CODE_STW_ISRC2_SRC1_SRC2;
    TOP__Operator[TOP_stw_ii] = Operator_CODE_STW_ISRCX_SRC1_SRC2;
    TOP__Operator[TOP_sub_i] = Operator_CODE_SUB_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sub_ii] = Operator_CODE_SUB_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sub_r] = Operator_CODE_SUB_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sxtb_r] = Operator_CODE_SXTB_IDEST_SRC1;
    TOP__Operator[TOP_sxth_r] = Operator_CODE_SXTH_IDEST_SRC1;
    TOP__Operator[TOP_sync] = Operator_CODE_SYNC;
    TOP__Operator[TOP_syscall] = Operator_CODE_SYSCALL;
    TOP__Operator[TOP_xor_i] = Operator_CODE_XOR_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_xor_ii] = Operator_CODE_XOR_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_xor_r] = Operator_CODE_XOR_DEST_SRC1_SRC2;
    // initialize IEC__Modifier
    for (int i = 0; i < EC_MAX; i++) IEC__Modifier[i] = Modifier_;
    // initialize LC__Immediate
    for (int i = 0; i < LC_MAX; i++) LC__Immediate[i] = Immediate_;
    LC__Immediate[0] = Immediate_I_signed_32_overflow_dont; // HACK ALERT
    LC__Immediate[LC_s32] = Immediate_I_signed_32_overflow_dont;
    LC__Immediate[LC_u32] = Immediate_I_signed_32_overflow_dont;
    LC__Immediate[LC_s23] = Immediate_I_signed_23_overflow_signed;
    LC__Immediate[LC_u23] = Immediate_I_unsigned_23_overflow_unsigned;
    LC__Immediate[LC_s9] = Immediate_I_signed_9_overflow_signed;
    // initialize IRC__RegClass
    for (int i = 0; i <= ISA_REGISTER_CLASS_MAX; i++) IRC__RegClass[i] = RegClass_;
    IRC__RegClass[ISA_REGISTER_CLASS_integer] = RegClass_GRC;
    IRC__RegClass[ISA_REGISTER_CLASS_branch] = RegClass_BRC;
    // initialize MEM_lao_pool
    MEM_POOL_Initialize ( &MEM_lao_pool, "lao_stub_pool", false );
  }
}

// Finalization of the LAO, needs to be called once.
void
LAO_FINI() {
  if (--LAO_initialized == 0) {
    // finalize MEM_lao_pool
    MEM_POOL_Delete  ( &MEM_lao_pool );
    // finalize LIR
    LIR_FINI();
  }
}

/*------------------------ LAO Conversion Fonctions --------------------------*/

// Convert CGIR TOP to LAO Operator.
static inline Operator
LAO_TOP_Operator(TOP top) {
  Operator lao_operator = TOP__Operator[top];
  Is_True(top >= 0 && top < TOP_UNDEFINED, ("TOPcode out of range"));
  Is_True(lao_operator != Operator_, ("Cannot map TOPcode to Operator"));
  return lao_operator;
}

// Convert CGIR ISA_ENUM_CLASS to LAO Modifier.
static inline Modifier
LAO_IEC_Modifier(ISA_ENUM_CLASS iec) {
  Modifier lao_modifier = IEC__Modifier[iec];
  Is_True(iec >= 0 && iec < EC_MAX, ("ISA_ENUM_CLASS out of range"));
  Is_True(lao_modifier != Modifier_, ("Cannot map ISA_ENUM_CLASS to Modifier"));
  return lao_modifier;
}

// Convert CGIR ISA_LIT_CLASS to LAO Immediate.
static inline Immediate
LAO_LC_Immediate(ISA_LIT_CLASS ilc) {
  Immediate lao_immediate = LC__Immediate[ilc];
  Is_True(ilc >= 0 && ilc < LC_MAX, ("ISA_LIT_CLASS out of range"));
  Is_True(lao_immediate != Immediate_, ("Cannot map ISA_LIT_CLASS to Immediate"));
  return lao_immediate;
}

// Convert CGIR ISA_REGISTER_CLASS to LAO RegClass.
static inline RegClass
LAO_IRC_RegClass(ISA_REGISTER_CLASS irc) {
  RegClass lao_regclass = IRC__RegClass[irc];
  Is_True(irc >= 0 && irc <= ISA_REGISTER_CLASS_MAX, ("ISA_REGISTER_CLASS out of range"));
  Is_True(lao_regclass != RegClass_, ("Cannot map ISA_REGISTER_CLASS to RegClass"));
  return lao_regclass;
}

// Convert CGIR CLASS_REG_PAIR to LAO Register.
static inline Register
LAO_CRP_Register(CLASS_REG_PAIR crp) {
  mREGISTER reg = CLASS_REG_PAIR_reg(crp);
  ISA_REGISTER_CLASS irc = CLASS_REG_PAIR_rclass(crp);
  RegClass regclass = LAO_IRC_RegClass(irc);
  Register lowreg = RegClass_getLowReg(regclass);
  return (Register)(lowreg + (reg - 1));
}

// Convert CGIR TN to LAO TempName.
static inline TempName
LAO_TN_TempName(TN *tn) {
  TempName tempname = Interface_getTempName(interface, tn);
  if (tempname == NULL) {
    if (TN_is_register(tn)) {
      if (TN_is_dedicated(tn)) {
	CLASS_REG_PAIR tn_crp = TN_class_reg(tn);
	tempname = Interface_makeDedicatedTempName(interface, tn, LAO_CRP_Register(tn_crp));
      } else {
	ISA_REGISTER_CLASS tn_irc = TN_register_class(tn);
	tempname = Interface_makePseudoRegTempName(interface, tn, LAO_IRC_RegClass(tn_irc));
      }
    } else if (TN_is_constant(tn)) {
      if (TN_has_value(tn)) {
	int64_t value = TN_value(tn);
	Immediate immediate = LAO_LC_Immediate((ISA_LIT_CLASS)0); // HACK ALERT
	tempname = Interface_makeAbsoluteTempName(interface, tn, immediate, value);
      } else if (TN_is_symbol(tn)) {
	ST *var = TN_var(tn);
	ST_IDX st_idx = ST_st_idx(*var);
	int64_t offset = TN_offset(tn);
	Immediate immediate = LAO_LC_Immediate((ISA_LIT_CLASS)0); // HACK ALERT
	Symbol symbol = Interface_makeSymbol(interface, st_idx, ST_name(st_idx));
	tempname = Interface_makeSymbolTempName(interface, tn, immediate, symbol, offset);
      } else if (TN_is_label(tn)) {
	LABEL_IDX label_idx = TN_label(tn);
	Immediate immediate = LAO_LC_Immediate((ISA_LIT_CLASS)0); // HACK ALERT
	Label label = Interface_makeLabel(interface, label_idx, LABEL_name(label_idx));
	tempname = Interface_makeLabelTempName(interface, tn, immediate, label);
	Is_True(TN_offset(tn) == 0, ("LAO requires zero offset from label."));
      } else if (TN_is_enum(tn)) {
	ISA_ENUM_CLASS_VALUE value = TN_enum(tn);
	Modifier modifier = LAO_IEC_Modifier((ISA_ENUM_CLASS)0);	// HACK ALERT
	tempname = Interface_makeModifierTempName(interface, tn, modifier, value);
      } else {
	Is_True(FALSE, ("Unknown constant TN type."));
      }
    } else {
      Is_True(FALSE, ("Unknown TN type."));
    }
    Is_True(tempname != NULL, ("tempname should not be NULL."));
  }
  return tempname;
}

// Convert CGIR OP to LAO Operation.
static Operation
LAO_OP_Operation(OP *op) {
  Operation operation = Interface_getOperation(interface, op);
  if (operation == NULL) {
    int argCount = OP_opnds(op);
    int resCount = OP_results(op);
    Operator OPERATOR = LAO_TOP_Operator(OP_code(op));
    operation = Interface_makeOperation(interface, op, OPERATOR, argCount, resCount);
    for (int i = 0; i < argCount; i++) {
      TempName tempname = LAO_TN_TempName(OP_opnd(op, i));
      Operation_appendArgument(operation, tempname);
    }
    for (int i = 0; i < resCount; i++) {
      TempName tempname = LAO_TN_TempName(OP_result(op, i));
      Operation_appendResult(operation, tempname);
    }
  }
  return operation;
}

// Convert CGIR BB to LAO BasicBlock.
static BasicBlock
LAO_BB_BasicBlock(BB *bb) {
  BasicBlock basicblock = Interface_getBasicBlock(interface, bb);
  if (basicblock == NULL) {
    basicblock = Interface_makeBasicBlock(interface, bb, BB_freq(bb));
    // the BasicBlock label(s)
    if (BB_has_label(bb)) {
      ANNOTATION *annot;
      for (annot = ANNOT_First(BB_annotations(bb), ANNOT_LABEL);
	   annot != NULL;
	   annot = ANNOT_Next(annot, ANNOT_LABEL)) {
	LABEL_IDX label_idx = ANNOT_label(annot);
	Label label = Interface_makeLabel(interface, label_idx, LABEL_name(label_idx));
	BasicBlock_appendLabel(basicblock, label);
      }
    }
    // the BasicBlock operations
    OP *op = NULL;
    FOR_ALL_BB_OPs(bb, op) {
      Operation operation = LAO_OP_Operation(op);
      BasicBlock_appendOperation(basicblock, operation);
    }
  }
  return basicblock;
}

/*-------------------------- LAO Utility Functions----------------------------*/

// Enter the control-flow arcs in the LAO.
static void
LAO_setControlArc(BasicBlockHandle handle, CodeRegion coderegion) {
  BBLIST *bblist = NULL;
  BB *bb = (BB *)BasicBlockHandle_pointer(handle);
  BasicBlock basicblock = BasicBlockHandle_basicblock(handle);
  FOR_ALL_BB_SUCCS(bb, bblist) {
    BB *succ_bb = BBLIST_item(bblist);
    BasicBlock succ_basicblock = Interface_getBasicBlock(interface, succ_bb);
    if (succ_basicblock != NULL) {
      float probability = BBLIST_prob(bblist);
      unsigned flags = BBLIST_flags(bblist);
      CodeRegion_makeControlArc(coderegion, basicblock, succ_basicblock, probability, flags);
    }
  }
}

// Enter the live-in information in the LAO.
static void
LAO_setLiveIn(BasicBlockHandle handle, CodeRegion coderegion) {
  BB *bb = (BB *)BasicBlockHandle_pointer(handle);
  BasicBlock basicblock = BasicBlockHandle_basicblock(handle);
  for (TN *tn = GTN_SET_Choose(BB_live_in(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_in(bb), tn)) {
    TempName tempname = LAO_TN_TempName(tn);
    CodeRegion_setLiveIn(coderegion, basicblock, tempname);
  }
}

// Enter the live-out information in the LAO.
static void
LAO_setLiveOut(BasicBlockHandle handle, CodeRegion coderegion) {
  BB *bb = (BB *)BasicBlockHandle_pointer(handle);
  BasicBlock basicblock = BasicBlockHandle_basicblock(handle);
  for (TN *tn = GTN_SET_Choose(BB_live_out(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_out(bb), tn)) {
    TempName tempname = LAO_TN_TempName(tn);
    CodeRegion_setLiveOut(coderegion, basicblock, tempname);
  }
}

// Make a LAO CodeRegion from the BB_VECTORs supplied.
static CodeRegion
LAO_makeCodeRegion(BB_List& entryBBs, BB_List& innerBBs, BB_List& exitBBs, CodeRegion_Type region_kind) {
  BB_List::iterator bb_iter;
  int bb_count = entryBBs.size() + innerBBs.size() + exitBBs.size();
  //
  CodeRegion coderegion = Interface_makeCodeRegion(interface, region_kind, bb_count);
  //
  // Create the LAO BasicBlocks.
  for (bb_iter = entryBBs.begin(); bb_iter != entryBBs.end(); bb_iter++) {
    LAO_BB_BasicBlock(*bb_iter);
  }
  for (bb_iter = innerBBs.begin(); bb_iter != innerBBs.end(); bb_iter++) {
    LAO_BB_BasicBlock(*bb_iter);
  }
  for (bb_iter = exitBBs.begin(); bb_iter != exitBBs.end(); bb_iter++) {
    LAO_BB_BasicBlock(*bb_iter);
  }
  //
  // Set the CodeRegion entry and exit BasicBlocks.
  for (bb_iter = entryBBs.begin(); bb_iter != entryBBs.end(); bb_iter++) {
    BasicBlock basicblock = Interface_getBasicBlock(interface, *bb_iter);
    CodeRegion_setEntry(coderegion, basicblock);
  }
  for (bb_iter = exitBBs.begin(); bb_iter != exitBBs.end(); bb_iter++) {
    BasicBlock basicblock = Interface_getBasicBlock(interface, *bb_iter);
    CodeRegion_setExit(coderegion, basicblock);
  }
  //
  // Add the control-flow arcs, the live-in, and the live-out.
  size_t size = bb_count * sizeof(BasicBlockHandle_);
  BasicBlockHandle_ *handles = (BasicBlockHandle_ *)alloca(size);
  Interface_fillBasicBlockHandles(interface, handles);
  for (int i = 0; i < bb_count; i++) {
    LAO_setControlArc(handles + i, coderegion);
    LAO_setLiveIn(handles + i, coderegion);
    LAO_setLiveOut(handles + i, coderegion);
  }
  //
  return coderegion;
}

// Enter the BB Memory dependences into the LAO.
static void
LAO_setMemoryDependences(BB* bb, LoopInfo loopinfo) {
  OP *op = NULL;
  FOR_ALL_BB_OPs(bb, op) {
    ARC_LIST *arcs = NULL;
    if (_CG_DEP_op_info(op)) {
      Operation orig_operation = Interface_getOperation(interface, op);
      for (arcs = OP_succs(op); arcs; arcs = ARC_LIST_rest(arcs)) {
	ARC *arc = ARC_LIST_first(arcs);
	CG_DEP_KIND kind = ARC_kind(arc);
	if (ARC_is_mem(arc)) {
	  bool definite = ARC_is_definite(arc);
	  int latency = ARC_latency(arc), omega = ARC_omega(arc);
	  OP *pred_op = ARC_pred(arc), *succ_op = ARC_succ(arc);
	  Is_True(pred_op == op, ("Error in LAO_setMemoryDependences"));
	  Operation dest_operation = Interface_getOperation(interface, succ_op);
	  LoopInfo_setMemoryDependence(loopinfo,
	      orig_operation, dest_operation, latency, omega, definite);
	  //CG_DEP_Trace_Arc(arc, TRUE, FALSE);
	}
      }
    } else fprintf(TFile, "<arc>   CG_DEP INFO is NULL\n");
  }
}

// Make a LAO LoopInfo from the BB_List supplied.
static LoopInfo
LAO_makeLoopInfo(BB_List& bb_list, unsigned lao_actions) {
  void *pointer = NULL;		// Parameter for CG_DEP_Delete_Graph.
  LoopInfo loopinfo = NULL;	// FIXME
  bool cyclic =
      lao_actions & LAO_LoopSchedule ||
      lao_actions & LAO_LoopPipeline;
  //  cyclic = false;	// HACK ALERT
  if (bb_list.size() == 1) {
    BB *bb = bb_list.front();
    CG_DEP_Compute_Graph(bb,
	false,	// assigned_regs
	cyclic,	// compute_cyclic
	false,	// memread_arcs
	true,	// memin_arcs
	false,	// control_arcs
	NULL);	// need_anti_out_dep
    LAO_setMemoryDependences(bb, loopinfo);
    pointer = bb;
  } else {
    if (cyclic) {
      cyclic = false;
      lao_actions &= ~LAO_LoopSchedule;
      lao_actions &= ~LAO_LoopPipeline;
      fprintf(TFile, "Cannot compute cyclic dependences on multi-BB loops\n");
    }
    CG_DEP_Compute_Region_Graph(bb_list,
	false,	// assigned_regs
	false,	// memread_arcs
	false);	// control_arcs
    BB_List::iterator bb_iter;
    for (bb_iter = bb_list.begin(); bb_iter != bb_list.end(); bb_iter++) {
      LAO_setMemoryDependences(*bb_iter, loopinfo);
    }
    pointer = &bb_list;
  }
  CG_DEP_Delete_Graph(pointer);
  return loopinfo;
}

/*----------------------- LAO Optimization Functions -------------------------*/

static void LAOS_printBB (BB *bp);

// Low-level LAO_optimize entry point.
static bool
LAO_optimize(BB_List &entryBBs, BB_List &innerBBs, BB_List &exitBBs, CodeRegion_Type region_kind, unsigned lao_actions) {
  bool result = false;
  //
  Interface_open(interface);
  //
  CodeRegion coderegion = LAO_makeCodeRegion(entryBBs, innerBBs, exitBBs, region_kind);
  //
  LoopInfo loopinfo = LAO_makeLoopInfo(innerBBs, lao_actions);
  //
  CodeRegion_pretty(coderegion, TFile);
  //
  result = CodeRegion_optimize(coderegion, lao_actions);
  //
  Interface_close(interface);
  //
  return result;
}

// Optimize a LOOP_DESCR through the LAO.
bool
LAO_optimize(LOOP_DESCR *loop, unsigned lao_actions) {
  BB_List entryBBs, innerBBs, exitBBs;
  bool result = false;
  //
#ifndef LAO_EXPERIMENT
  LAOS_printCGIR();
#endif //LAO_EXPERIMENT
  //
  if (BB_innermost(LOOP_DESCR_loophead(loop))) {
    // Create a prolog and epilog for the region when possible.
    CG_LOOP cg_loop(loop);
    // We need to call cg_loop.Build_CG_LOOP_Info() to perform
    // initialization of operations in the loop. This is required before calling
    // CG_DEP_Compute_Graph with cyclic = true.
    if (cg_loop.Single_BB()) cg_loop.Build_CG_LOOP_Info();
    //
    entryBBs.push_back(CG_LOOP_prolog);
    //
    // Enter the body blocks in linear order.
    BB *loop_head = LOOP_DESCR_loophead(loop);
    BB *loop_tail = LOOP_DESCR_Find_Unique_Tail(loop);
    //
    if (loop_tail != NULL) {
      //
      for (BB *bb = loop_head;
	   bb && BB_prev(bb) != loop_tail;
	   bb = BB_next(bb)) {
	if (BB_SET_MemberP(LOOP_DESCR_bbset(loop), bb)) {
	  innerBBs.push_back(bb);
	  //
	  BBLIST *succs = NULL;
	  FOR_ALL_BB_SUCCS(bb, succs) {
	    BB *succ = BBLIST_item(succs);
	    if (!BB_SET_MemberP(LOOP_DESCR_bbset(loop), succ)) {
	      // Ensure that a bb is not put twice in the exitBBs.
	      if (!LAO_inBB_List(exitBBs, succ)) exitBBs.push_back(succ);
	    }
	  }
	}
      }
      //
      // Call the main LAO_optimize entry point.
      result = LAO_optimize(entryBBs, innerBBs, exitBBs, CodeRegion_InnerLoop, lao_actions);
    }
  }
  //
  return result;
}

// Optimize a HB through the LAO.
bool
LAO_optimize(HB *hb, unsigned lao_actions) {
  BB_List entryBBs, innerBBs, exitBBs;
  bool result = false;
  //
  entryBBs.push_back(HB_Entry(hb));
  //
  BB *bb = NULL;
  FOR_ALL_BB_SET_members(HB_Blocks(hb), bb) {
    innerBBs.push_back(bb);
    //
    BBLIST *succs = NULL;
    FOR_ALL_BB_SUCCS(bb, succs) {
      BB *succ = BBLIST_item(succs);
      if (!HB_Contains_Block(hb, succ)) {
	if (!LAO_inBB_List(exitBBs, succ))
	  exitBBs.push_back(succ);
      }
    }
  }
  //
  result = LAO_optimize(entryBBs, innerBBs, exitBBs, CodeRegion_TraceBlocks, lao_actions);
  //
  return result;
}

/*-------------------------- CGIR Print Functions ----------------------------*/

typedef struct OP_list {
  OP *op;
  struct OP_list *next;
} OP_list;

static OP_list * OP_list_new(OP_list *head)
{
  OP_list * elem;
  //
  elem = (OP_list *)malloc(sizeof(OP_list));
  elem->next = head;
  head = elem;
  return head;
}

static void
LAOS_printTN ( const TN *tn )
{
  //
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
      //
      fprintf ( TFile, "(sym" );
      fprintf ( TFile, TN_RELOCS_Name(TN_relocs(tn)) );
      //
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

static void
LAOS_printOP ( const OP *op )
{
  int i;
  //
  if (OP_has_tag(op)) {
    LABEL_IDX tag = Get_OP_Tag(op);
    fprintf (TFile, "<tag %s>: ", LABEL_name(tag));
  }
  //
  fprintf(TFile, "%s", TOP_Name(OP_code(op)));
  //
  if ( OP_variant(op) != 0 ) {
    fprintf ( TFile, "(%x)", OP_variant(op));
  }
  //
  if (OP_results(op) == 0)
    fprintf(TFile, " void");
  //
  else for (i = 0; i < OP_results(op); i++) {
    fprintf(TFile, "%s", (i == 0 ? " " : ", "));
    LAOS_printTN(OP_result(op,i));
  }
  //
  fprintf(TFile, " =");
  //
  for (i=0; i<OP_opnds(op); i++) {
    fprintf(TFile, "%s", (i == 0 ? " " : ", "));
    TN *tn = OP_opnd(op,i);
    LAOS_printTN(tn);
    if (OP_Defs_TN(op, tn)) fprintf(TFile, "<def>");
  }
}

static void
LAOS_printOPs ( const OP *op )
{
  for ( ; op; op = OP_next(op)) {
    fprintf(TFile, "\t");
    LAOS_printOP(op);
    fprintf(TFile, "       \t#line[%4d]", Srcpos_To_Line(OP_srcpos(op)));
    fprintf(TFile, "\n");
  }
}

static void
LAOS_printBB_Header (BB *bp)
{
  BBLIST *bl;
  INT16 i;
  ANNOTATION *annot = ANNOT_Get(BB_annotations(bp), ANNOT_LOOPINFO);
  BOOL freqs = FREQ_Frequencies_Computed();
  //
  if ( BB_entry(bp) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bp), ANNOT_ENTRYINFO);
    ENTRYINFO *ent = ANNOT_entryinfo (ant);
    OP *sp_adj = BB_entry_sp_adj_op(bp);
    Is_True ((sp_adj == ENTRYINFO_sp_adj(ent)),("bad sp_adj"));
    //
    fprintf ( TFile, "Entrypoint: %s\t Starting Line %d\n",
	      ST_name(ENTRYINFO_name(ent)),
	      Srcpos_To_Line(ENTRYINFO_srcpos(ent)));
    //
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
  //
  if ( BB_exit(bp) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bp), ANNOT_EXITINFO);
    EXITINFO *exit = ANNOT_exitinfo (ant);
    OP *sp_adj = BB_exit_sp_adj_op(bp);
    Is_True ((sp_adj == EXITINFO_sp_adj(exit)),("bad sp_adj"));
    //
    if (sp_adj) {
      OP *op;
      BOOL found_sp_adj = FALSE;
      fprintf ( TFile, "SP exit adj: " );
      Print_OP_No_SrcLine (sp_adj);
      //
      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( TFile, "******** ERROR ******** sp adjust not found in exit block\n");
    }
  }
  //
  fprintf ( TFile, "    BB %d, flags 0x%04x",
	    BB_id(bp), BB_flag(bp) );
  //
  if (freqs || BB_freq_fb_based(bp))
    fprintf(TFile, ", freq %g (%s)", BB_freq(bp),
	    BB_freq_fb_based(bp) ? "feedback" : "heuristic");
  
  if (BB_unreachable(bp)) fprintf ( TFile, ", Unreachable");
  if (BB_entry(bp))	fprintf ( TFile, ", Entry" );
  if (BB_handler(bp))	fprintf ( TFile, ", Handler" );
  if (BB_asm(bp)) 	fprintf ( TFile, ", Asm" );
  //
  if (BB_exit(bp)) {
    if (BB_call(bp))	fprintf ( TFile, ", Tail-call" );
    else		fprintf ( TFile, ", Exit" );
  } else if (BB_call(bp)) fprintf ( TFile, ", Call" );
  //
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
  //
  fprintf ( TFile, "\n");
  //
  if (annot)
    Print_LOOPINFO(ANNOT_loopinfo(annot));
  //
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
  //
  if (BB_unrollings(bp) > 1)
    fprintf(TFile, "\tUnrolled %d times%s\n", BB_unrollings(bp),
	    BB_unrolled_fully(bp) ? " (fully)" : "");
  //
  if ( BB_rid(bp) )
    RID_Fprint( TFile, BB_rid(bp) );
  //
  fprintf ( TFile, "\tpred" );
  FOR_ALL_BB_PREDS (bp, bl) {
    fprintf ( TFile, " %d", BB_id(BBLIST_item(bl)));
  }
  //
  fprintf ( TFile, "\n\tsucc%s", freqs ? " (w/probs)" : "" );
  FOR_ALL_BB_SUCCS (bp, bl) {
    fprintf ( TFile, " %d",
	      BB_id(BBLIST_item(bl)));
    if (freqs) fprintf(TFile, "(%g)", BBLIST_prob(bl));
  }
  fprintf ( TFile, "\n" );
  //
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
  //
  return;
}

static void
LAOS_printBB (BB *bp)
{
  LAOS_printBB_Header (bp );
  if (BB_first_op(bp))	LAOS_printOPs (BB_first_op(bp));
}

static void
LAOS_printAlias()
{
  OP_list *memops = NULL, *elt1, *elt2;
  BB *bp;
  OP *op;
  BOOL alias, identical;
  //
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    for (op = BB_first_op(bp); op; op = OP_next(op)) {
      if (OP_memory(op)) {
	memops = OP_list_new(memops);
	memops->op = op;
      }
    }
  }
  //
  fprintf(TFile, "--------------- Begin Print Alias ---------------\n");
  //
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
  //
  fprintf(TFile, "---------------- End Print Alias ----------------\n");
}

static void
LAOS_printCGIR()
{
  BB *bp;
  //
  fprintf(TFile, "--------CFG Begin--------\n");
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    LAOS_printBB ( bp );
    fprintf ( TFile,"\n" );
  }
  //
  LAOS_printAlias();
  //
  // Print live-analysis information
  fprintf(TFile, "-------- CFG End --------\n");
}

/*-------------------------- Old LAO Entry Points ----------------------------*/

bool
LAO_scheduleRegion ( BB_List& entryBBs, BB_List& innerBBs, BB_List& exitBBs, CodeRegion_Type region_kind, LAO_SWP_ACTION action ) {
  int i;
  BB_List::iterator bb_iter;
  CodeRegion lir_region;
  int status;
  
  fprintf(TFile, "---- Before LAO schedule region ----\n");
  fprintf(TFile, "---- Begin trace regionBBs ----\n");
  for (bb_iter = innerBBs.begin(); bb_iter != innerBBs.end(); bb_iter++) {
    LAOS_printBB(*bb_iter);
  }    
  fprintf(TFile, "---- End trace regionBBs ----\n");
  //
  lir_region = LAO_makeCodeRegion(entryBBs, innerBBs, exitBBs, region_kind);
  //
  CodeRegion_pretty(lir_region, TFile);
  //
  //  status = REGION_schedule(lir_region);
  //
  if (status == 0) {
    //    REGION_convert2CGIR(lir_region);
  }
  //  
  fprintf(TFile, "---- After LAO schedule region ----\n");
  return (status == 0);
}

static void
LAOS_printCGIR(void);

bool Perform_SWP(CG_LOOP& cl, LAO_SWP_ACTION action) {
  LOOP_DESCR *loop = cl.Loop();
  BB *bb;
  BB_List entryBBs, innerBBs, exitBBs;
  bool res;
  //
  LAOS_printCGIR();
  //
  fprintf(TFile, "---- Before LAO schedule loop ----\n");
  fprintf(TFile, "     ------ LOOP id %2d ------\n", BB_id(LOOP_DESCR_loophead(loop)));
  fprintf(TFile, "            -----------\n");
  //
  int scan;
  scanf("%d", &scan);
  //
  FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(loop), bb) LAOS_printBB(bb);
  //
  entryBBs.push_back (CG_LOOP_prolog);
  innerBBs.push_back (CG_LOOP_prolog);
  innerBBs.push_back (LOOP_DESCR_loophead(loop));
  innerBBs.push_back (CG_LOOP_epilog);
  exitBBs.push_back  (CG_LOOP_epilog);
  //
  fprintf(TFile, "--------------- dpendence graph for %d ---------------\n", BB_id(LOOP_DESCR_loophead(loop)));
  // CG_DEP_Compute_Region_Graph
  CYCLIC_DEP_GRAPH cyclic_graph( LOOP_DESCR_loophead(loop), &MEM_lao_pool); 
  CG_DEP_Trace_Graph(LOOP_DESCR_loophead(loop));
  //
  res = LAO_scheduleRegion ( entryBBs, innerBBs, exitBBs, CodeRegion_InnerLoop, action );
  //
  return res;
}
