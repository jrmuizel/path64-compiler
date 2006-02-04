/* Generated automatically by the program `genopinit'
from the machine description file `md'.  */

#include "config.h"
#include "system.h"
#include "rtl.h"
#include "flags.h"
#include "insn-config.h"
#include "recog.h"
#include "expr.h"
#include "optabs.h"
#include "reload.h"

void
init_all_optabs ()
{
  add_optab->handlers[(int) SImode].insn_code = CODE_FOR_addsi3;
  sub_optab->handlers[(int) SImode].insn_code = CODE_FOR_subsi3;
  smin_optab->handlers[(int) SImode].insn_code = CODE_FOR_sminsi3;
  smax_optab->handlers[(int) SImode].insn_code = CODE_FOR_smaxsi3;
  umin_optab->handlers[(int) SImode].insn_code = CODE_FOR_uminsi3;
  umax_optab->handlers[(int) SImode].insn_code = CODE_FOR_umaxsi3;
  and_optab->handlers[(int) SImode].insn_code = CODE_FOR_andsi3;
  ior_optab->handlers[(int) SImode].insn_code = CODE_FOR_iorsi3;
  xor_optab->handlers[(int) SImode].insn_code = CODE_FOR_xorsi3;
  ashl_optab->handlers[(int) SImode].insn_code = CODE_FOR_ashlsi3;
  ashr_optab->handlers[(int) SImode].insn_code = CODE_FOR_ashrsi3;
  lshr_optab->handlers[(int) SImode].insn_code = CODE_FOR_lshrsi3;
  neg_optab->handlers[(int) SImode].insn_code = CODE_FOR_negsi2;
  one_cmpl_optab->handlers[(int) SImode].insn_code = CODE_FOR_one_cmplsi2;
  extendtab[(int) SImode][(int) HImode][0] = CODE_FOR_extendhisi2;
  extendtab[(int) SImode][(int) QImode][0] = CODE_FOR_extendqisi2;
  extendtab[(int) QImode][(int) HImode][1] = CODE_FOR_zero_extendhiqi2;
  mov_optab->handlers[(int) SImode].insn_code = CODE_FOR_movsi;
  mov_optab->handlers[(int) HImode].insn_code = CODE_FOR_movhi;
  mov_optab->handlers[(int) QImode].insn_code = CODE_FOR_movqi;
  mov_optab->handlers[(int) SFmode].insn_code = CODE_FOR_movsf;
  add_optab->handlers[(int) DImode].insn_code = CODE_FOR_adddi3;
  abs_optab->handlers[(int) SImode].insn_code = CODE_FOR_abssi2;
  smul_optab->handlers[(int) SImode].insn_code = CODE_FOR_mulsi3;
  extendtab[(int) SImode][(int) HImode][1] = CODE_FOR_zero_extendhisi2;
  absv_optab->handlers[(int) (int) DFmode].insn_code =
    abs_optab->handlers[(int) (int) DFmode].insn_code = CODE_FOR_absdf2;
  absv_optab->handlers[(int) (int) SFmode].insn_code =
    abs_optab->handlers[(int) (int) SFmode].insn_code = CODE_FOR_abssf2;
  negv_optab->handlers[(int) (int) SFmode].insn_code =
    neg_optab->handlers[(int) (int) SFmode].insn_code = CODE_FOR_negsf2;
  negv_optab->handlers[(int) (int) DFmode].insn_code =
    neg_optab->handlers[(int) (int) DFmode].insn_code = CODE_FOR_negdf2;
  cmp_optab->handlers[(int) SImode].insn_code = CODE_FOR_cmpsi;
  reload_in_optab[(int) CCmode] = CODE_FOR_reload_incc;
  reload_in_optab[(int) CC_BRmode] = CODE_FOR_reload_incc_br;
  reload_out_optab[(int) CCmode] = CODE_FOR_reload_outcc;
  reload_out_optab[(int) CC_BRmode] = CODE_FOR_reload_outcc_br;
  movcc_gen_code[(int) SImode] = CODE_FOR_movsicc;
  setcc_gen_code[(int) EQ] = CODE_FOR_seq;
  setcc_gen_code[(int) NE] = CODE_FOR_sne;
  setcc_gen_code[(int) GT] = CODE_FOR_sgt;
  setcc_gen_code[(int) GTU] = CODE_FOR_sgtu;
  setcc_gen_code[(int) LT] = CODE_FOR_slt;
  setcc_gen_code[(int) LTU] = CODE_FOR_sltu;
  setcc_gen_code[(int) GE] = CODE_FOR_sge;
  setcc_gen_code[(int) GEU] = CODE_FOR_sgeu;
  setcc_gen_code[(int) LE] = CODE_FOR_sle;
  setcc_gen_code[(int) LEU] = CODE_FOR_sleu;
  bcc_gen_fctn[(int) EQ] = gen_beq;
  bcc_gen_fctn[(int) NE] = gen_bne;
  bcc_gen_fctn[(int) GT] = gen_bgt;
  bcc_gen_fctn[(int) GTU] = gen_bgtu;
  bcc_gen_fctn[(int) LT] = gen_blt;
  bcc_gen_fctn[(int) LTU] = gen_bltu;
  bcc_gen_fctn[(int) GE] = gen_bge;
  bcc_gen_fctn[(int) GEU] = gen_bgeu;
  bcc_gen_fctn[(int) LE] = gen_ble;
  bcc_gen_fctn[(int) LEU] = gen_bleu;
}
