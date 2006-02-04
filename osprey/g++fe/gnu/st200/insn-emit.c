/* Generated automatically by the program `genemit'
from the machine description file `md'.  */

#include "config.h"
#include "system.h"
#include "rtl.h"
#include "tm_p.h"
#include "function.h"
#include "expr.h"
#include "optabs.h"
#include "real.h"
#include "flags.h"
#include "output.h"
#include "insn-config.h"
#include "hard-reg-set.h"
#include "recog.h"
#include "resource.h"
#include "reload.h"
#include "toplev.h"
#include "ggc.h"

#define FAIL return (end_sequence (), _val)
#define DONE return (_val = get_insns (), end_sequence (), _val)

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:495 */
rtx
gen_addcg_insn (operand0, operand1, operand2, operand3, operand4)
     rtx operand0;
     rtx operand1;
     rtx operand2;
     rtx operand3;
     rtx operand4;
{
  return gen_rtx_PARALLEL (VOIDmode,
	gen_rtvec (2,
		gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_TRUNCATE (SImode,
	gen_rtx_PLUS (DImode,
	gen_rtx_PLUS (DImode,
	gen_rtx_ZERO_EXTEND (DImode,
	operand1),
	gen_rtx_ZERO_EXTEND (DImode,
	operand2)),
	gen_rtx_NE (DImode,
	operand3,
	const0_rtx)))),
		gen_rtx_SET (VOIDmode,
	operand4,
	gen_rtx_NE (CCmode,
	gen_rtx_LSHIFTRT (DImode,
	gen_rtx_PLUS (DImode,
	gen_rtx_PLUS (DImode,
	gen_rtx_ZERO_EXTEND (DImode,
	operand1),
	gen_rtx_ZERO_EXTEND (DImode,
	operand2)),
	gen_rtx_NE (DImode,
	operand3,
	const0_rtx)),
	GEN_INT (32)),
	const0_rtx))));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:524 */
rtx
gen_addsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_PLUS (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:535 */
rtx
gen_subsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_MINUS (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:546 */
rtx
gen_sminsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_SMIN (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:557 */
rtx
gen_smaxsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_SMAX (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:568 */
rtx
gen_uminsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_UMIN (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:579 */
rtx
gen_umaxsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_UMAX (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:590 */
rtx
gen_andsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_AND (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:601 */
rtx
gen_iorsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_IOR (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:612 */
rtx
gen_xorsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_XOR (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:623 */
rtx
gen_ashlsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ASHIFT (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:634 */
rtx
gen_ashrsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ASHIFTRT (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:645 */
rtx
gen_lshrsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_LSHIFTRT (SImode,
	operand1,
	operand2));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:656 */
rtx
gen_negsi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NEG (SImode,
	operand1));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:679 */
rtx
gen_one_cmplsi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NOT (SImode,
	operand1));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:794 */
rtx
gen_dummy_mulsi3 (operand0, operand1, operand2, operand3, operand4)
     rtx operand0;
     rtx operand1;
     rtx operand2;
     rtx operand3;
     rtx operand4;
{
  return gen_rtx_PARALLEL (VOIDmode, gen_rtvec (3,
		gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_MULT (SImode,
	operand1,
	operand2)),
		gen_rtx_CLOBBER (VOIDmode,
	operand3),
		gen_rtx_CLOBBER (VOIDmode,
	operand4)));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1073 */
rtx
gen_extendhisi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_SIGN_EXTEND (SImode,
	operand1));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1085 */
rtx
gen_extendqisi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_SIGN_EXTEND (SImode,
	operand1));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1114 */
rtx
gen_ldhu_insn (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ZERO_EXTEND (SImode,
	operand1));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1124 */
rtx
gen_zero_extendhiqi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ZERO_EXTEND (SImode,
	operand1));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1309 */
rtx
gen_cmpcc_insn (operand0, operand1, operand2, operand3)
     rtx operand0;
     rtx operand1;
     rtx operand2;
     rtx operand3;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx (GET_CODE (operand1), CCmode,
		operand2,
		operand3));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1321 */
rtx
gen_cmpcc_br_insn (operand0, operand1, operand2, operand3)
     rtx operand0;
     rtx operand1;
     rtx operand2;
     rtx operand3;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx (GET_CODE (operand1), CC_BRmode,
		operand2,
		operand3));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1462 */
rtx
gen_mfb_cc_insn (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NE (SImode,
	operand1,
	const0_rtx));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1473 */
rtx
gen_mfb_cc_br_insn (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NE (SImode,
	operand1,
	const0_rtx));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1522 */
rtx
gen_slctf_insn (operand0, operand1, operand2, operand3)
     rtx operand0;
     rtx operand1;
     rtx operand2;
     rtx operand3;
{
  return gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_IF_THEN_ELSE (SImode,
	gen_rtx_EQ (CCmode,
	operand1,
	const0_rtx),
	operand2,
	operand3));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1909 */
rtx
gen_jump (operand0)
     rtx operand0;
{
  return gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_LABEL_REF (VOIDmode,
	operand0));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1918 */
rtx
gen_indirect_jump (operand0)
     rtx operand0;
{
  return gen_rtx_SET (VOIDmode,
	pc_rtx,
	operand0);
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1927 */
rtx
gen_tablejump (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  return gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2,
		gen_rtx_SET (VOIDmode,
	pc_rtx,
	operand0),
		gen_rtx_USE (VOIDmode,
	gen_rtx_LABEL_REF (VOIDmode,
	operand1))));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1991 */
rtx
gen_return ()
{
  return gen_rtx_PARALLEL (VOIDmode,
	gen_rtvec (2,
		gen_rtx_RETURN (VOIDmode),
		gen_rtx_USE (VOIDmode,
	gen_rtx_REG (SImode,
	63))));
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:2031 */
rtx
gen_nop ()
{
  return const0_rtx;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:2040 */
rtx
gen_bundle_barrier ()
{
  return gen_rtx_UNSPEC_VOLATILE (VOIDmode,
	gen_rtvec (1,
		const0_rtx),
	2);
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:265 */
rtx
gen_movsi (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;
{
     if (! reload_in_progress && ! reload_completed
         && GET_CODE (operands[0]) == MEM
         && GET_CODE (operands[1]) == MEM)
       operands[1] = force_reg (SImode, operands[1]);

    /* (operand == 0) substituted with register #0 */
    if (GET_CODE (operands[1]) == CONST_INT && INTVAL (operands [1]) == 0)
       operands[1] = gen_rtx_REG (SImode, 0);
   }
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:299 */
rtx
gen_movhi (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = force_reg (HImode, operands[1]);

 if (GET_CODE (operands[1]) == CONST_INT && INTVAL (operands [1]) == 0)
    operands[1] = gen_rtx_REG (HImode, 0);
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:330 */
rtx
gen_movqi (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = force_reg (QImode, operands[1]);

 if (GET_CODE (operands[1]) == CONST_INT && INTVAL (operands [1]) == 0)
    operands[1] = gen_rtx_REG (QImode, 0);
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:366 */
rtx
gen_movsf (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = force_reg (SFmode, operands[1]);
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:410 */
extern rtx gen_split_68 PARAMS ((rtx *));
rtx
gen_split_68 (operands)
      rtx *operands;
{
  rtx operand0;
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx operand4;
  rtx operand5;
  rtx _val = 0;
  start_sequence ();

{
  rtx seq;
  int need_clobber;
  int i;
			     
  if (! reload_in_progress && ! reload_completed
       && GET_CODE (operands[0]) == MEM
       && GET_CODE (operands[1]) == MEM)
       operands[1] = force_reg (DImode, operands[1]);

  start_sequence ();

  need_clobber = 0;
  for (i = 0; i < 2;  i++)
  {
     rtx xpart = operand_subword (operands[0], i, 0, DImode);
     rtx ypart = operand_subword (operands[1], i, 0, DImode);

     need_clobber |= (GET_CODE (xpart) == SUBREG);

      emit_move_insn (xpart, ypart);
   }

   seq = get_insns ();
   end_sequence ();

   /* Show the output dies here.  This is necessary for SUBREGs
      of pseudos since we cannot track their lifetimes correctly;
      hard regs shouldn't appear here except as return values.
       We never want to emit such a clobber after reload.  */
   if (operands[0] != operands[1]
    && ! (reload_in_progress || reload_completed)
    && need_clobber != 0)
    {
      emit_insn (gen_rtx_CLOBBER (VOIDmode, operands[0]));
    }

   emit_insn (seq);

   DONE;
}
  operand0 = operands[0];
  operand1 = operands[1];
  operand2 = operands[2];
  operand3 = operands[3];
  operand4 = operands[4];
  operand5 = operands[5];
  emit_insn (gen_rtx_SET (VOIDmode,
	operand2,
	operand3));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand4,
	operand5));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:459 */
rtx
gen_adddi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  rtx cc1_rtx  = gen_reg_rtx (CCmode);
  rtx cc2_rtx  = gen_reg_rtx (CCmode);
  rtx cc3_rtx  = gen_reg_rtx (CCmode);
  rtx si_0_rtx = gen_rtx_REG (SImode, 0);
  rtx ne_rtx   = gen_rtx (NE, VOIDmode, NULL_RTX, NULL_RTX);

  operands[0] = force_reg (DImode, operands[0]);
  operands[1] = force_reg (DImode, operands[1]);
  operands[2] = force_reg (DImode, operands[2]);

  if (REGNO(operands[0]) != REGNO(operands[1])) {
    emit_insn (gen_rtx_CLOBBER (VOIDmode, operands[0]));
  }
  emit_insn (gen_cmpcc_insn (cc1_rtx, ne_rtx, si_0_rtx, const0_rtx));
  emit_insn (gen_addcg_insn (gen_lowpart (SImode, operands[0]),
                             gen_lowpart (SImode, operands[1]),
                             gen_lowpart (SImode, operands[2]),
                             cc1_rtx, cc2_rtx));
  emit_insn (gen_addcg_insn (gen_highpart (SImode, operands[0]),
                             gen_highpart (SImode, operands[1]),
                             gen_highpart (SImode, operands[2]),
                             cc2_rtx, cc3_rtx));
  emit_move_insn(operands[0], operands[0]);
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_PLUS (DImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:666 */
rtx
gen_abssi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;
    operands[1] = operand1;

{
    operands[3] = gen_reg_rtx (SImode);
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand3,
	gen_rtx_NEG (SImode,
	operand1)));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_SMAX (SImode,
	operand1,
	operand3)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:770 */
rtx
gen_mulsi3 (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx operand3;
  rtx operand4;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[5];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  operands[3] = gen_reg_rtx (SImode);
  operands[4] = gen_reg_rtx (SImode);
  if (optimize > 1) {
    emit_insn (gen_dummy_mulsi3 (operands[0], operands[1], operands[2],
                                 operands[3], operands[4]));
    DONE;
  }
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
    operand4 = operands[4];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand3,
	gen_rtx_MULT (SImode,
	gen_rtx_AND (SImode,
	operand2,
	GEN_INT (-65536)),
	operand1)));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand4,
	gen_rtx_MULT (SImode,
	operand1,
	gen_rtx_AND (SImode,
	operand2,
	GEN_INT (65535)))));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_PLUS (SImode,
	operand3,
	operand4)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:809 */
extern rtx gen_split_72 PARAMS ((rtx *));
rtx
gen_split_72 (operands)
      rtx *operands;
{
  rtx operand0;
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();

  operand0 = operands[0];
  operand1 = operands[1];
  operand2 = operands[2];
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_MULT (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:829 */
extern rtx gen_split_73 PARAMS ((rtx *));
rtx
gen_split_73 (operands)
      rtx *operands;
{
  rtx operand0;
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx operand4;
  rtx operand5;
  rtx _val = 0;
  start_sequence ();

{
  if (GET_CODE (operands [2]) == CONST_INT) {
    int a, b;
    a = INTVAL (operands [2]);
    b = (short) a;
    operands [5] = GEN_INT ((FORCE_LONG_IMM (b)) ? a : b);
  } else
    operands [5] = operands [2];
}
  operand0 = operands[0];
  operand1 = operands[1];
  operand2 = operands[2];
  operand3 = operands[3];
  operand4 = operands[4];
  operand5 = operands[5];
  emit_insn (gen_rtx_SET (VOIDmode,
	operand3,
	gen_rtx_MULT (SImode,
	gen_rtx_AND (SImode,
	operand2,
	GEN_INT (-65536)),
	operand1)));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand4,
	gen_rtx_MULT (SImode,
	copy_rtx (operand1),
	gen_rtx_AND (SImode,
	operand5,
	GEN_INT (65535)))));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_PLUS (SImode,
	copy_rtx (operand3),
	copy_rtx (operand4))));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1097 */
rtx
gen_zero_extendhisi2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
    if (MEM_P (operands[1]))
      emit_insn (gen_ldhu_insn (operands[0], operands[1]));
    else {
      rtx op = gen_lowpart (SImode, operands[1]);
      emit_insn (gen_andsi3 (operands[0], op, GEN_INT (0xffff)));
    }
    DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ZERO_EXTEND (SImode,
	operand1)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1173 */
rtx
gen_absdf2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  rtx target, result, insns;

  start_sequence ();
  target = operand_subword (operands[0], !TARGET_BIG_ENDIAN, 1, DFmode);
  result = expand_binop (SImode, and_optab,
			 operand_subword_force (operands[1],
                                                !TARGET_BIG_ENDIAN, DFmode),
			 GEN_INT(0x7fffffff), target, 0, OPTAB_WIDEN);

  if (result == 0)
    abort ();

  if (result != target)
    emit_move_insn (result, target);

  emit_move_insn (operand_subword (operands[0], TARGET_BIG_ENDIAN, 1, DFmode),
		  operand_subword_force (operands[1],
                                         TARGET_BIG_ENDIAN, DFmode));

  insns = get_insns ();
  end_sequence ();

  emit_no_conflict_block (insns, operands[0], operands[1], 0, 0);
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ABS (DFmode,
	operand1)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1205 */
rtx
gen_abssf2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  rtx result;
  rtx target;

  target = operand_subword_force (operands[0], 0, SFmode);
  result = expand_binop (SImode, and_optab,
			 operand_subword_force (operands[1], 0, SFmode),
			 GEN_INT(0x7fffffff), target, 0, OPTAB_WIDEN);
  if (result == 0)
    abort ();

  if (result != target)
    emit_move_insn (result, target);

  /* Make a place for REG_EQUAL.  */
  emit_move_insn (operands[0], operands[0]);
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_ABS (SFmode,
	operand1)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1230 */
rtx
gen_negsf2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  rtx result;
  rtx target;

  target = operand_subword_force (operands[0], 0, SFmode);
  result = expand_binop (SImode, xor_optab,
			 operand_subword_force (operands[1], 0, SFmode),
			 GEN_INT(0x80000000), target, 0, OPTAB_WIDEN);
  if (result == 0)
    abort ();

  if (result != target)
    emit_move_insn (result, target);

  /* Make a place for REG_EQUAL.  */
  emit_move_insn (operands[0], operands[0]);
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NEG (SFmode,
	operand1)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1254 */
rtx
gen_negdf2 (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  rtx target, result, insns;

  start_sequence ();
  target = operand_subword (operands[0], !TARGET_BIG_ENDIAN, 1, DFmode);

  result = expand_binop (SImode, xor_optab,
			 operand_subword_force (operands[1],
                                                !TARGET_BIG_ENDIAN, DFmode),
			 GEN_INT(0x80000000), target, 0, OPTAB_WIDEN);

  if (result == 0)
    abort ();

  if (result != target)
    emit_move_insn (result, target);

  emit_move_insn (operand_subword (operands[0], TARGET_BIG_ENDIAN, 1, DFmode),
		  operand_subword_force (operands[1],
                                         TARGET_BIG_ENDIAN, DFmode));

  insns = get_insns ();
  end_sequence ();

  emit_no_conflict_block (insns, operands[0], operands[1], 0, 0);
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NEG (DFmode,
	operand1)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1295 */
rtx
gen_cmpsi (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  lx_compare_op0 = operands[0];
  lx_compare_op1 = operands[1];
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	cc0_rtx,
	gen_rtx_COMPARE (CCmode,
	operand0,
	operand1)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1376 */
rtx
gen_reload_incc (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  rtx src, ne_rtx;
  ne_rtx = gen_rtx (NE, VOIDmode, NULL_RTX, NULL_RTX);
  if (GET_CODE(operands[1]) == MEM)
    src = change_address (operands[1], SImode, NULL_RTX);
  else if (GET_CODE (operands[1]) == REG || GET_CODE (operands[1]) == SUBREG)
    src = gen_rtx (REG, SImode, true_regnum (operands[1]));
  else
    src = operands[1];
  emit_insn(gen_move_insn (operands[2], src));
  emit_insn(gen_cmpcc_insn (operands[0], ne_rtx, operands[2], const0_rtx));
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  emit_insn (gen_rtx_CLOBBER (VOIDmode,
	operand2));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1398 */
rtx
gen_reload_incc_br (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  rtx src, ne_rtx;
  ne_rtx = gen_rtx (NE, VOIDmode, NULL_RTX, NULL_RTX);
  if (GET_CODE(operands[1]) == MEM)
    src = change_address (operands[1], SImode, NULL_RTX);
  else if (GET_CODE (operands[1]) == REG || GET_CODE (operands[1]) == SUBREG)
    src = gen_rtx (REG, SImode, true_regnum (operands[1]));
  else
    src = operands[1];
  emit_insn(gen_move_insn (operands[2], src));
  emit_insn(gen_cmpcc_br_insn (operands[0], ne_rtx, operands[2], const0_rtx));
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  emit_insn (gen_rtx_CLOBBER (VOIDmode,
	operand2));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1420 */
rtx
gen_reload_outcc (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  rtx dst;
  if (GET_CODE(operands[0]) == MEM)
    dst = change_address (operands[0], SImode, NULL_RTX);
  else if (GET_CODE (operands[0]) == REG || GET_CODE (operands[0]) == SUBREG)
    dst = gen_rtx (REG, SImode, true_regnum (operands[0]));
  else
    dst = operands[0];
  emit_insn(gen_mfb_cc_insn (operands[2], operands[1]));
  emit_insn(gen_move_insn (dst, operands[2]));
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  emit_insn (gen_rtx_CLOBBER (VOIDmode,
	operand2));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1441 */
rtx
gen_reload_outcc_br (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  rtx dst;
  if (GET_CODE(operands[0]) == MEM)
    dst = change_address (operands[0], SImode, NULL_RTX);
  else if (GET_CODE (operands[0]) == REG || GET_CODE (operands[0]) == SUBREG)
    dst = gen_rtx (REG, SImode, true_regnum (operands[0]));
  else
    dst = operands[0];
  emit_insn(gen_mfb_cc_br_insn (operands[2], operands[1]));
  emit_insn(gen_move_insn (dst, operands[2]));
  DONE;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	operand1));
  emit_insn (gen_rtx_CLOBBER (VOIDmode,
	operand2));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1491 */
rtx
gen_movsicc (operand0, operand1, operand2, operand3)
     rtx operand0;
     rtx operand1;
     rtx operand2;
     rtx operand3;
{
  rtx operand4;
  rtx operand5;
  rtx operand6;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[7];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;
    operands[3] = operand3;

{
  operands[4] = gen_reg_rtx (CCmode);
  operands[5] = lx_compare_op0;
  operands[6] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
    operand4 = operands[4];
    operand5 = operands[5];
    operand6 = operands[6];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand4,
	gen_rtx (GET_CODE (operand1), CCmode,
		operand5,
		operand6)));
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_IF_THEN_ELSE (SImode,
	gen_rtx_NE (CCmode,
	operand4,
	const0_rtx),
	operand2,
	operand3)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1542 */
rtx
gen_seq (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_EQ (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1554 */
rtx
gen_sne (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_NE (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1566 */
rtx
gen_sgt (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_GT (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1578 */
rtx
gen_sgtu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_GTU (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1590 */
rtx
gen_slt (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_LT (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1602 */
rtx
gen_sltu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_LTU (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1614 */
rtx
gen_sge (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_GE (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1626 */
rtx
gen_sgeu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_GEU (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1638 */
rtx
gen_sle (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_LE (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1650 */
rtx
gen_sleu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;

{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_LEU (SImode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1669 */
rtx
gen_beq (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_EQ (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1687 */
rtx
gen_bne (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_NE (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1706 */
rtx
gen_bgt (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_GT (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1724 */
rtx
gen_bgtu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_GTU (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1742 */
rtx
gen_blt (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_LT (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1760 */
rtx
gen_bltu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_LTU (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1778 */
rtx
gen_bge (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_GE (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1796 */
rtx
gen_bgeu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_GEU (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1814 */
rtx
gen_ble (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_LE (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1832 */
rtx
gen_bleu (operand0)
     rtx operand0;
{
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[4];
    operands[0] = operand0;

{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
    operand3 = operands[3];
  }
  emit_insn (gen_rtx_SET (VOIDmode,
	operand1,
	gen_rtx_LEU (CC_BRmode,
	operand2,
	operand3)));
  emit_jump_insn (gen_rtx_SET (VOIDmode,
	pc_rtx,
	gen_rtx_IF_THEN_ELSE (VOIDmode,
	gen_rtx_NE (CC_BRmode,
	operand1,
	const0_rtx),
	gen_rtx_LABEL_REF (VOIDmode,
	operand0),
	pc_rtx)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1937 */
rtx
gen_call (operand0, operand1)
     rtx operand0;
     rtx operand1;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[2];
    operands[0] = operand0;
    operands[1] = operand1;

{
  if (MEM_P (operands[0]) && REG_P (XEXP (operands[0], 0)))
    {
      rtx ret_rtx = gen_rtx_REG (SImode, RETURN_POINTER_REGNUM);
      emit_move_insn (ret_rtx, XEXP (operands[0], 0));
      operands[0] = gen_rtx (MEM, SImode, ret_rtx);
    }
}
    operand0 = operands[0];
    operand1 = operands[1];
  }
  emit_call_insn (gen_rtx_CALL (VOIDmode,
	operand0,
	operand1));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:1963 */
rtx
gen_call_value (operand0, operand1, operand2)
     rtx operand0;
     rtx operand1;
     rtx operand2;
{
  rtx _val = 0;
  start_sequence ();
  {
    rtx operands[3];
    operands[0] = operand0;
    operands[1] = operand1;
    operands[2] = operand2;

{
  if (MEM_P (operands[1]) && REG_P (XEXP (operands[1], 0)))
    {
      rtx ret_rtx = gen_rtx_REG (SImode, RETURN_POINTER_REGNUM);
      emit_move_insn (ret_rtx, XEXP (operands[1], 0));
      operands[1] = gen_rtx (MEM, SImode, ret_rtx);
    }
}
    operand0 = operands[0];
    operand1 = operands[1];
    operand2 = operands[2];
  }
  emit_call_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_CALL (VOIDmode,
	operand1,
	operand2)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:2001 */
rtx
gen_prologue ()
{
  rtx _val = 0;
  start_sequence ();
  {

{
  lx_expand_prologue ();
  DONE;
}
  }
  emit_insn (const1_rtx);
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:2012 */
rtx
gen_epilogue ()
{
  rtx _val = 0;
  start_sequence ();
  {

{
  lx_expand_epilogue ();
  DONE;
}
  }
  emit_insn (GEN_INT (2));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:2074 */
extern rtx gen_split_109 PARAMS ((rtx *));
rtx
gen_split_109 (operands)
      rtx *operands;
{
  rtx operand0;
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx _val = 0;
  start_sequence ();

  operand0 = operands[0];
  operand1 = operands[1];
  operand2 = operands[2];
  operand3 = operands[3];
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_PLUS (SImode,
	operand1,
	operand2)));
  emit_insn (gen_rtx_SET (VOIDmode,
	copy_rtx (operand0),
	gen_rtx_PLUS (SImode,
	copy_rtx (operand0),
	operand3)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}

/* /home/bruel/Work/gcc_st200/gcc-3.3.2/gcc/config/st200/st200.md:2099 */
extern rtx gen_split_110 PARAMS ((rtx *));
rtx
gen_split_110 (operands)
      rtx *operands;
{
  rtx operand0;
  rtx operand1;
  rtx operand2;
  rtx operand3;
  rtx operand4;
  rtx _val = 0;
  start_sequence ();

  operand0 = operands[0];
  operand1 = operands[1];
  operand2 = operands[2];
  operand3 = operands[3];
  operand4 = operands[4];
  emit_insn (gen_rtx_SET (VOIDmode,
	operand0,
	gen_rtx_PLUS (SImode,
	gen_rtx_MULT (SImode,
	operand1,
	operand2),
	operand3)));
  emit_insn (gen_rtx_SET (VOIDmode,
	copy_rtx (operand0),
	gen_rtx_PLUS (SImode,
	copy_rtx (operand0),
	operand4)));
  _val = get_insns ();
  end_sequence ();
  return _val;
}



void
add_clobbers (pattern, insn_code_number)
     rtx pattern ATTRIBUTE_UNUSED;
     int insn_code_number;
{
  switch (insn_code_number)
    {
    default:
      abort ();
    }
}


int
added_clobbers_hard_reg_p (insn_code_number)
     int insn_code_number;
{
  switch (insn_code_number)
    {
    default:
      abort ();
    }
}
