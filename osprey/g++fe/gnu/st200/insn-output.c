/* Generated automatically by the program `genoutput'
   from the machine description file `md'.  */

#include "config.h"
#include "system.h"
#include "flags.h"
#include "ggc.h"
#include "rtl.h"
#include "expr.h"
#include "insn-codes.h"
#include "tm_p.h"
#include "function.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"

#include "conditions.h"
#include "insn-attr.h"

#include "recog.h"

#include "toplev.h"
#include "output.h"

static const char * const output_0[] = {
  "mov %0 = %1",
  "ldw %0 = %1",
  "mov %0 = %1",
  "ldw %0 = %1",
  "stw %0 = %1",
  "mov %0 = %1",
};

static const char * const output_1[] = {
  "mov %0 = %1",
  "ldh %0 = %1",
  "sth %0 = %1",
  "mov %0 = %1",
};

static const char * const output_2[] = {
  "mov %0 = %1",
  "ldb %0 = %1",
  "stb %0 = %1",
  "mov %0 = %1",
};

static const char * const output_3[] = {
  "mov %0 = %1",
  "ldw %0 = %1",
  "stw %0 = %1",
  "mov %0 = %1",
};

static const char *output_20 PARAMS ((rtx *, rtx));

static const char *
output_20 (operands, insn)
     rtx *operands ATTRIBUTE_UNUSED;
     rtx insn ATTRIBUTE_UNUSED;
{

{
  switch (INTVAL (operands [2])) {
    case  2:   return "sh1add %0 = %1, %3";
    case  4:   return "sh2add %0 = %1, %3";
    case  8:   return "sh3add %0 = %1, %3";
    case 16:   return "sh4add %0 = %1, %3";
    default:   abort();
  }
}
}

static const char *output_24 PARAMS ((rtx *, rtx));

static const char *
output_24 (operands, insn)
     rtx *operands ATTRIBUTE_UNUSED;
     rtx insn ATTRIBUTE_UNUSED;
{

{
  if (FIT_UI16 (INTVAL (operands [2])))  return "mullu %0 = %1, %2";
  if (FIT_SI16 (INTVAL (operands [2])))  return "mull %0 = %1, %2";
  if (FIT_HI16 (INTVAL (operands [2])))  return "mulhs %0 = %1, %2";
  abort();
}
}

static const char * const output_39[] = {
  "sxth %0 = %1",
  "ldh %0 = %1",
};

static const char * const output_40[] = {
  "sxtb %0 = %1",
  "ldb %0 = %1",
};

static const char * const output_42[] = {
  "and %0 = %1, 255",
  "ldbu %0 = %1",
};


extern int nonimmediate_operand PARAMS ((rtx, enum machine_mode));
extern int general_operand PARAMS ((rtx, enum machine_mode));
extern int register_operand PARAMS ((rtx, enum machine_mode));
extern int nonmemory_operand PARAMS ((rtx, enum machine_mode));
extern int lx_shXadd_operand PARAMS ((rtx, enum machine_mode));
extern int const_int_operand PARAMS ((rtx, enum machine_mode));
extern int memory_operand PARAMS ((rtx, enum machine_mode));
extern int comparison_operator PARAMS ((rtx, enum machine_mode));



static const struct insn_operand_data operand_data[] = 
{
  {
    0,
    "",
    VOIDmode,
    0,
    0
  },
  {
    nonimmediate_operand,
    "=l,l,r,r,m,r",
    SImode,
    0,
    1
  },
  {
    general_operand,
    "r,m,lr,m,lr,i",
    SImode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "=rl,r,m,r",
    HImode,
    0,
    1
  },
  {
    general_operand,
    "rl,m,r,i",
    HImode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "=rl,r,m,r",
    QImode,
    0,
    1
  },
  {
    general_operand,
    "rl,m,r,i",
    QImode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "=r,r,m,r",
    SFmode,
    0,
    1
  },
  {
    general_operand,
    "r,m,r,i",
    SFmode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "=r,r,m,r",
    DImode,
    0,
    1
  },
  {
    general_operand,
    "r,m,r,i",
    DImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "%r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "b",
    CCmode,
    0,
    1
  },
  {
    register_operand,
    "=b",
    CCmode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "%r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    lx_shXadd_operand,
    "",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "%r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    const_int_operand,
    "i",
    VOIDmode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    HImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "%r",
    HImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    HImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    HImode,
    0,
    1
  },
  {
    const_int_operand,
    "i",
    VOIDmode,
    0,
    1
  },
  {
    register_operand,
    "=r,r",
    SImode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "r,m",
    HImode,
    0,
    1
  },
  {
    register_operand,
    "=r,r",
    SImode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "r,m",
    QImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    memory_operand,
    "m",
    HImode,
    0,
    1
  },
  {
    register_operand,
    "=b",
    CCmode,
    0,
    1
  },
  {
    comparison_operator,
    "",
    CCmode,
    0,
    0
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=b",
    CC_BRmode,
    0,
    1
  },
  {
    comparison_operator,
    "",
    CC_BRmode,
    0,
    0
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    comparison_operator,
    "",
    SImode,
    0,
    0
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "b",
    CCmode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "b",
    CC_BRmode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "b",
    CCmode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "b",
    CC_BRmode,
    0,
    1
  },
  {
    0,
    "",
    VOIDmode,
    0,
    1
  },
  {
    register_operand,
    "l",
    SImode,
    0,
    1
  },
  {
    0,
    "",
    VOIDmode,
    0,
    1
  },
  {
    nonmemory_operand,
    "li",
    SImode,
    0,
    1
  },
  {
    0,
    "",
    VOIDmode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    VOIDmode,
    0,
    1
  },
  {
    nonmemory_operand,
    "li",
    SImode,
    0,
    1
  },
  {
    0,
    "",
    VOIDmode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "%r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    lx_shXadd_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "r",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "ri",
    SImode,
    0,
    1
  },
  {
    general_operand,
    "",
    SImode,
    0,
    1
  },
  {
    general_operand,
    "",
    SImode,
    0,
    1
  },
  {
    general_operand,
    "",
    HImode,
    0,
    1
  },
  {
    general_operand,
    "",
    HImode,
    0,
    1
  },
  {
    general_operand,
    "",
    QImode,
    0,
    1
  },
  {
    general_operand,
    "",
    QImode,
    0,
    1
  },
  {
    general_operand,
    "",
    SFmode,
    0,
    1
  },
  {
    general_operand,
    "",
    SFmode,
    0,
    1
  },
  {
    register_operand,
    "",
    DImode,
    0,
    1
  },
  {
    register_operand,
    "",
    DImode,
    0,
    1
  },
  {
    register_operand,
    "",
    DImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    const_int_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    nonimmediate_operand,
    "",
    HImode,
    0,
    1
  },
  {
    register_operand,
    "",
    DFmode,
    0,
    1
  },
  {
    register_operand,
    "",
    DFmode,
    0,
    1
  },
  {
    register_operand,
    "",
    SFmode,
    0,
    1
  },
  {
    register_operand,
    "",
    SFmode,
    0,
    1
  },
  {
    register_operand,
    "=b",
    CCmode,
    0,
    1
  },
  {
    0,
    "b",
    CCmode,
    0,
    1
  },
  {
    register_operand,
    "=&r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "=b",
    CC_BRmode,
    0,
    1
  },
  {
    0,
    "b",
    CC_BRmode,
    0,
    1
  },
  {
    register_operand,
    "=&r",
    SImode,
    0,
    1
  },
  {
    0,
    "=b",
    CC_BRmode,
    0,
    1
  },
  {
    register_operand,
    "b",
    CC_BRmode,
    0,
    1
  },
  {
    register_operand,
    "=&r",
    SImode,
    0,
    1
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    comparison_operator,
    "",
    CCmode,
    0,
    0
  },
  {
    register_operand,
    "",
    SImode,
    0,
    1
  },
  {
    nonmemory_operand,
    "",
    SImode,
    0,
    1
  },
  {
    0,
    "",
    SImode,
    0,
    1
  },
  {
    0,
    "",
    VOIDmode,
    0,
    1
  },
  {
    register_operand,
    "",
    VOIDmode,
    0,
    1
  },
  {
    0,
    "",
    SImode,
    0,
    1
  },
  {
    0,
    "",
    VOIDmode,
    0,
    1
  },
};



const struct insn_data insn_data[] = 
{
  {
    "*movsi_internal",
    (const PTR) output_0,
    0,
    &operand_data[1],
    2,
    0,
    6,
    2
  },
  {
    "*movhi_internal",
    (const PTR) output_1,
    0,
    &operand_data[3],
    2,
    0,
    4,
    2
  },
  {
    "*movqi_internal",
    (const PTR) output_2,
    0,
    &operand_data[5],
    2,
    0,
    4,
    2
  },
  {
    "*movsf_internal",
    (const PTR) output_3,
    0,
    &operand_data[7],
    2,
    0,
    4,
    2
  },
  {
    "*movdi_internal",
    "#",
    0,
    &operand_data[9],
    2,
    0,
    4,
    1
  },
  {
    "addcg_insn",
    "addcg %0, %4 = %1, %2, %3",
    (insn_gen_fn) gen_addcg_insn,
    &operand_data[11],
    5,
    3,
    1,
    1
  },
  {
    "addsi3",
    "add %0 = %1, %2",
    (insn_gen_fn) gen_addsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "subsi3",
    "sub %0 = %1, %2",
    (insn_gen_fn) gen_subsi3,
    &operand_data[19],
    3,
    0,
    1,
    1
  },
  {
    "sminsi3",
    "min %0 = %1, %2",
    (insn_gen_fn) gen_sminsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "smaxsi3",
    "max %0 = %1, %2",
    (insn_gen_fn) gen_smaxsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "uminsi3",
    "minu %0 = %1, %2",
    (insn_gen_fn) gen_uminsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "umaxsi3",
    "maxu %0 = %1, %2",
    (insn_gen_fn) gen_umaxsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "andsi3",
    "and %0 = %1, %2",
    (insn_gen_fn) gen_andsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "iorsi3",
    "or %0 = %1, %2",
    (insn_gen_fn) gen_iorsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "xorsi3",
    "xor %0 = %1, %2",
    (insn_gen_fn) gen_xorsi3,
    &operand_data[16],
    3,
    0,
    1,
    1
  },
  {
    "ashlsi3",
    "shl %0 = %1, %2",
    (insn_gen_fn) gen_ashlsi3,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "ashrsi3",
    "shr %0 = %1, %2",
    (insn_gen_fn) gen_ashrsi3,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "lshrsi3",
    "shru %0 = %1, %2",
    (insn_gen_fn) gen_lshrsi3,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "negsi2",
    "sub %0 = $r0.0, %1",
    (insn_gen_fn) gen_negsi2,
    &operand_data[19],
    2,
    0,
    1,
    1
  },
  {
    "one_cmplsi2",
    "orc %0 = %1, $r0.0",
    (insn_gen_fn) gen_one_cmplsi2,
    &operand_data[19],
    2,
    0,
    1,
    1
  },
  {
    "*shXadd_insn",
    (const PTR) output_20,
    0,
    &operand_data[25],
    4,
    0,
    1,
    3
  },
  {
    "*andc_insn",
    "andc %0 = %1, %2",
    0,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "*orc_insn",
    "orc %0 = %1, %2",
    0,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "dummy_mulsi3",
    "#",
    (insn_gen_fn) gen_dummy_mulsi3,
    &operand_data[29],
    5,
    0,
    1,
    1
  },
  {
    "*mul_si_const_hi",
    (const PTR) output_24,
    0,
    &operand_data[34],
    3,
    0,
    1,
    3
  },
  {
    "*mulhs_insn",
    "mulhs %0 = %1, %2",
    0,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "*mullu_insn1",
    "mullu %0 = %1, %2",
    0,
    &operand_data[22],
    3,
    0,
    1,
    1
  },
  {
    "*mullu_insn2",
    "mullu %0 = %2, %1",
    0,
    &operand_data[37],
    3,
    0,
    1,
    1
  },
  {
    "*mull_insn",
    "mull %0 = %2, %1",
    0,
    &operand_data[37],
    3,
    0,
    1,
    1
  },
  {
    "*mulh_insn",
    "mulh %0 = %2, %1",
    0,
    &operand_data[19],
    3,
    0,
    1,
    1
  },
  {
    "*mulhu_insn",
    "mulhu %0 = %2, %1",
    0,
    &operand_data[19],
    3,
    0,
    1,
    1
  },
  {
    "*mulll_insn1",
    "mulll %0 = %1, %2",
    0,
    &operand_data[40],
    3,
    0,
    1,
    1
  },
  {
    "*mulll_insn2",
    "mulll %0 = %1, %2",
    0,
    &operand_data[43],
    3,
    0,
    1,
    1
  },
  {
    "*mulllu_insn1",
    "mulllu %0 = %1, %2",
    0,
    &operand_data[40],
    3,
    0,
    1,
    1
  },
  {
    "*mulllu_insn2",
    "mulllu %0 = %1, %2",
    0,
    &operand_data[43],
    3,
    0,
    1,
    1
  },
  {
    "*mullh_insn",
    "mullh %0 = %1, %2",
    0,
    &operand_data[37],
    3,
    0,
    1,
    1
  },
  {
    "*mullhu_insn",
    "mullhu %0 = %1, %2",
    0,
    &operand_data[37],
    3,
    0,
    1,
    1
  },
  {
    "*mulhh_insn",
    "mulhh %0 = %1, %2",
    0,
    &operand_data[19],
    3,
    0,
    1,
    1
  },
  {
    "*mulhhu_insn",
    "mulhhu %0 = %1, %2",
    0,
    &operand_data[19],
    3,
    0,
    1,
    1
  },
  {
    "extendhisi2",
    (const PTR) output_39,
    (insn_gen_fn) gen_extendhisi2,
    &operand_data[46],
    2,
    0,
    2,
    2
  },
  {
    "extendqisi2",
    (const PTR) output_40,
    (insn_gen_fn) gen_extendqisi2,
    &operand_data[48],
    2,
    0,
    2,
    2
  },
  {
    "ldhu_insn",
    "ldhu %0 = %1",
    (insn_gen_fn) gen_ldhu_insn,
    &operand_data[50],
    2,
    0,
    1,
    1
  },
  {
    "zero_extendhiqi2",
    (const PTR) output_42,
    (insn_gen_fn) gen_zero_extendhiqi2,
    &operand_data[48],
    2,
    0,
    2,
    2
  },
  {
    "cmpcc_insn",
    "cmp%j1 %0 = %2, %3",
    (insn_gen_fn) gen_cmpcc_insn,
    &operand_data[52],
    4,
    0,
    1,
    1
  },
  {
    "cmpcc_br_insn",
    "cmp%j1 %0 = %2, %3",
    (insn_gen_fn) gen_cmpcc_br_insn,
    &operand_data[56],
    4,
    0,
    1,
    1
  },
  {
    "*cmpsi_insn",
    "cmp%j1 %0 = %2, %3",
    0,
    &operand_data[60],
    4,
    0,
    1,
    1
  },
  {
    "mfb_cc_insn",
    "mfb %0 = %1",
    (insn_gen_fn) gen_mfb_cc_insn,
    &operand_data[64],
    2,
    0,
    1,
    1
  },
  {
    "mfb_cc_br_insn",
    "mfb %0 = %1",
    (insn_gen_fn) gen_mfb_cc_br_insn,
    &operand_data[66],
    2,
    0,
    1,
    1
  },
  {
    "*slct_insn",
    "slct %0 = %1, %2, %3",
    0,
    &operand_data[68],
    4,
    0,
    1,
    1
  },
  {
    "slctf_insn",
    "slctf %0 = %1, %2, %3",
    (insn_gen_fn) gen_slctf_insn,
    &operand_data[68],
    4,
    0,
    1,
    1
  },
  {
    "*branch_true",
    "br %0, %1",
    0,
    &operand_data[72],
    2,
    0,
    1,
    1
  },
  {
    "*branch_false",
    "brf %0, %1",
    0,
    &operand_data[72],
    2,
    0,
    1,
    1
  },
  {
    "*branch_false_rev",
    "brf %0, %1",
    0,
    &operand_data[72],
    2,
    0,
    1,
    1
  },
  {
    "*branch_true_rev",
    "br %0, %1",
    0,
    &operand_data[72],
    2,
    0,
    1,
    1
  },
  {
    "jump",
    "goto %0",
    (insn_gen_fn) gen_jump,
    &operand_data[73],
    1,
    0,
    0,
    1
  },
  {
    "indirect_jump",
    "goto %0",
    (insn_gen_fn) gen_indirect_jump,
    &operand_data[74],
    1,
    0,
    1,
    1
  },
  {
    "tablejump",
    "goto %0",
    (insn_gen_fn) gen_tablejump,
    &operand_data[74],
    2,
    0,
    1,
    1
  },
  {
    "*call_internal",
    "call $r0.63 = %0",
    0,
    &operand_data[76],
    2,
    0,
    1,
    1
  },
  {
    "*call_value_internal",
    "call $r0.63 = %1",
    0,
    &operand_data[78],
    3,
    0,
    1,
    1
  },
  {
    "return",
    "return $r0.63",
    (insn_gen_fn) gen_return,
    &operand_data[0],
    0,
    0,
    0,
    1
  },
  {
    "nop",
    "nop",
    (insn_gen_fn) gen_nop,
    &operand_data[0],
    0,
    0,
    0,
    1
  },
  {
    "bundle_barrier",
    ";;",
    (insn_gen_fn) gen_bundle_barrier,
    &operand_data[0],
    0,
    0,
    0,
    1
  },
  {
    "*reload_addsi3",
    "#",
    0,
    &operand_data[81],
    4,
    0,
    1,
    1
  },
  {
    "*reload_shXadd_insn",
    "#",
    0,
    &operand_data[85],
    5,
    0,
    1,
    1
  },
  {
    "movsi",
    0,
    (insn_gen_fn) gen_movsi,
    &operand_data[90],
    2,
    0,
    0,
    0
  },
  {
    "movhi",
    0,
    (insn_gen_fn) gen_movhi,
    &operand_data[92],
    2,
    0,
    0,
    0
  },
  {
    "movqi",
    0,
    (insn_gen_fn) gen_movqi,
    &operand_data[94],
    2,
    0,
    0,
    0
  },
  {
    "movsf",
    0,
    (insn_gen_fn) gen_movsf,
    &operand_data[96],
    2,
    0,
    0,
    0
  },
  {
    "movsf+1",
    0,
    0,
    &operand_data[9],
    2,
    0,
    0,
    0
  },
  {
    "adddi3",
    0,
    (insn_gen_fn) gen_adddi3,
    &operand_data[98],
    3,
    0,
    0,
    0
  },
  {
    "abssi2",
    0,
    (insn_gen_fn) gen_abssi2,
    &operand_data[32],
    2,
    3,
    0,
    0
  },
  {
    "mulsi3",
    0,
    (insn_gen_fn) gen_mulsi3,
    &operand_data[101],
    3,
    6,
    0,
    0
  },
  {
    "mulsi3+1",
    0,
    0,
    &operand_data[104],
    5,
    0,
    0,
    0
  },
  {
    "zero_extendhisi2-1",
    0,
    0,
    &operand_data[101],
    5,
    0,
    0,
    0
  },
  {
    "zero_extendhisi2",
    0,
    (insn_gen_fn) gen_zero_extendhisi2,
    &operand_data[108],
    2,
    0,
    0,
    0
  },
  {
    "absdf2",
    0,
    (insn_gen_fn) gen_absdf2,
    &operand_data[110],
    2,
    0,
    0,
    0
  },
  {
    "abssf2",
    0,
    (insn_gen_fn) gen_abssf2,
    &operand_data[112],
    2,
    0,
    0,
    0
  },
  {
    "negsf2",
    0,
    (insn_gen_fn) gen_negsf2,
    &operand_data[112],
    2,
    0,
    0,
    0
  },
  {
    "negdf2",
    0,
    (insn_gen_fn) gen_negdf2,
    &operand_data[110],
    2,
    0,
    0,
    0
  },
  {
    "cmpsi",
    0,
    (insn_gen_fn) gen_cmpsi,
    &operand_data[102],
    2,
    0,
    0,
    0
  },
  {
    "reload_incc",
    0,
    (insn_gen_fn) gen_reload_incc,
    &operand_data[114],
    3,
    0,
    1,
    0
  },
  {
    "reload_incc_br",
    0,
    (insn_gen_fn) gen_reload_incc_br,
    &operand_data[117],
    3,
    0,
    1,
    0
  },
  {
    "reload_outcc",
    0,
    (insn_gen_fn) gen_reload_outcc,
    &operand_data[120],
    3,
    0,
    1,
    0
  },
  {
    "reload_outcc_br",
    0,
    (insn_gen_fn) gen_reload_outcc_br,
    &operand_data[120],
    3,
    0,
    1,
    0
  },
  {
    "movsicc",
    0,
    (insn_gen_fn) gen_movsicc,
    &operand_data[123],
    4,
    4,
    0,
    0
  },
  {
    "seq",
    0,
    (insn_gen_fn) gen_seq,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sne",
    0,
    (insn_gen_fn) gen_sne,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sgt",
    0,
    (insn_gen_fn) gen_sgt,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sgtu",
    0,
    (insn_gen_fn) gen_sgtu,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "slt",
    0,
    (insn_gen_fn) gen_slt,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sltu",
    0,
    (insn_gen_fn) gen_sltu,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sge",
    0,
    (insn_gen_fn) gen_sge,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sgeu",
    0,
    (insn_gen_fn) gen_sgeu,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sle",
    0,
    (insn_gen_fn) gen_sle,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "sleu",
    0,
    (insn_gen_fn) gen_sleu,
    &operand_data[32],
    1,
    2,
    0,
    0
  },
  {
    "beq",
    0,
    (insn_gen_fn) gen_beq,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bne",
    0,
    (insn_gen_fn) gen_bne,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bgt",
    0,
    (insn_gen_fn) gen_bgt,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bgtu",
    0,
    (insn_gen_fn) gen_bgtu,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "blt",
    0,
    (insn_gen_fn) gen_blt,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bltu",
    0,
    (insn_gen_fn) gen_bltu,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bge",
    0,
    (insn_gen_fn) gen_bge,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bgeu",
    0,
    (insn_gen_fn) gen_bgeu,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "ble",
    0,
    (insn_gen_fn) gen_ble,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "bleu",
    0,
    (insn_gen_fn) gen_bleu,
    &operand_data[73],
    1,
    4,
    0,
    0
  },
  {
    "call",
    0,
    (insn_gen_fn) gen_call,
    &operand_data[127],
    2,
    0,
    0,
    0
  },
  {
    "call_value",
    0,
    (insn_gen_fn) gen_call_value,
    &operand_data[129],
    3,
    0,
    0,
    0
  },
  {
    "prologue",
    0,
    (insn_gen_fn) gen_prologue,
    &operand_data[0],
    0,
    0,
    0,
    0
  },
  {
    "epilogue",
    0,
    (insn_gen_fn) gen_epilogue,
    &operand_data[0],
    0,
    0,
    0,
    0
  },
  {
    "epilogue+1",
    0,
    0,
    &operand_data[81],
    4,
    0,
    0,
    0
  },
  {
    "epilogue+2",
    0,
    0,
    &operand_data[85],
    5,
    0,
    0,
    0
  },
};


const char *
get_insn_name (code)
     int code;
{
  return insn_data[code].name;
}
