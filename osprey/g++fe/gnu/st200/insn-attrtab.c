/* Generated automatically by the program `genattrtab'
from the machine description file `md'.  */

#include "config.h"
#include "system.h"
#include "rtl.h"
#include "tm_p.h"
#include "insn-config.h"
#include "recog.h"
#include "regs.h"
#include "real.h"
#include "output.h"
#include "insn-attr.h"
#include "toplev.h"
#include "flags.h"
#include "function.h"

#define operands recog_data.operand

extern int insn_current_length PARAMS ((rtx));
int
insn_current_length (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 0;

    }
}

extern int insn_variable_length_p PARAMS ((rtx));
int
insn_variable_length_p (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 0;

    }
}

extern int insn_default_length PARAMS ((rtx));
int
insn_default_length (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 61:
      return 0;

    case 41:
      extract_insn_cached (insn);
      if (lx_long_add_operand (operands[1], VOIDmode))
        {
	  return 8;
        }
      else
        {
	  return 4;
        }

    case 49:
    case 48:
    case 45:
    case 44:
    case 43:
    case 20:
      extract_insn_cached (insn);
      if (lx_long_imm_operand (operands[3], VOIDmode))
        {
	  return 8;
        }
      else
        {
	  return 4;
        }

    case 47:
    case 46:
    case 19:
    case 18:
      extract_insn_cached (insn);
      if (lx_long_imm_operand (operands[1], VOIDmode))
        {
	  return 8;
        }
      else
        {
	  return 4;
        }

    case 42:
    case 40:
    case 39:
    case 3:
    case 2:
    case 1:
    case 0:
      if (get_attr_LongImm (insn) == LONGIMM_YES)
        {
	  return 8;
        }
      else
        {
	  return 4;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    case 63:
    case 62:
    case 60:
    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
    case 23:
    case 4:
      return 4;

    default:
      extract_insn_cached (insn);
      if (lx_long_imm_operand (operands[2], VOIDmode))
        {
	  return 8;
        }
      else
        {
	  return 4;
        }

    }
}

extern int bypass_p PARAMS ((rtx));
int
bypass_p (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 0;

    }
}

extern int insn_default_latency PARAMS ((rtx));
int
insn_default_latency (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 0;

    }
}

extern int insn_alts PARAMS ((rtx));
int
insn_alts (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 0;

    }
}

extern int internal_dfa_insn_code PARAMS ((rtx));
int
internal_dfa_insn_code (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 1;

    }
}

extern int result_ready_cost PARAMS ((rtx));
int
result_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 41:
      if (((lx_delay_model) == (DELAY_MODEL_BUNDLE)))
        {
	  return 12 /* 0xc */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) || (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 3;
        }
      else
        {
	  return 1;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 12 /* 0xc */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (which_alternative == 0))
        {
	  return 4;
        }
      else if ((which_alternative == 1) && ((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) || (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE)))))
        {
	  return 3;
        }
      else
        {
	  return 1;
        }

    case 44:
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (! (((lx_cpu) == (CPU_ST210)))))
        {
	  return 12 /* 0xc */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (((lx_cpu) == (CPU_ST210))))
        {
	  return 8;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (! (((lx_cpu) == (CPU_ST210))))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (! (((lx_cpu) == (CPU_ST210))))))
        {
	  return 3;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (((lx_cpu) == (CPU_ST210)))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (((lx_cpu) == (CPU_ST210)))))
        {
	  return 2;
        }
      else
        {
	  return 1;
        }

    case 3:
      extract_constrain_insn_cached (insn);
      if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && ((which_alternative == 1) && (! (((lx_cpu) == (CPU_ST210))))))
        {
	  return 12 /* 0xc */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && ((which_alternative == 1) && (((lx_cpu) == (CPU_ST210)))))
        {
	  return 8;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (which_alternative != 1))
        {
	  return 4;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && ((which_alternative == 1) && (! (((lx_cpu) == (CPU_ST210)))))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && ((which_alternative == 1) && (! (((lx_cpu) == (CPU_ST210)))))))
        {
	  return 3;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && ((which_alternative == 1) && (((lx_cpu) == (CPU_ST210))))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && ((which_alternative == 1) && (((lx_cpu) == (CPU_ST210))))))
        {
	  return 2;
        }
      else
        {
	  return 1;
        }

    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (which_alternative == 1))
        {
	  return 12 /* 0xc */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (which_alternative != 1))
        {
	  return 4;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (which_alternative == 1)) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (which_alternative == 1)))
        {
	  return 3;
        }
      else
        {
	  return 1;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (which_alternative == 1))
        {
	  return 16 /* 0x10 */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (((which_alternative == 3) && (! (((lx_cpu) == (CPU_ST210))))) || (which_alternative == 0)))
        {
	  return 12 /* 0xc */;
        }
      else if ((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && ((which_alternative == 3) && (((lx_cpu) == (CPU_ST210)))))
        {
	  return 8;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (which_alternative == 1)) || (((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && ((which_alternative != 3) && ((which_alternative != 1) && (which_alternative != 0)))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (which_alternative == 1))))
        {
	  return 4;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (((which_alternative == 3) && (! (((lx_cpu) == (CPU_ST210))))) || (which_alternative == 0))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (((which_alternative == 3) && (! (((lx_cpu) == (CPU_ST210))))) || (which_alternative == 0))))
        {
	  return 3;
        }
      else if (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && ((which_alternative == 3) && (((lx_cpu) == (CPU_ST210))))) || ((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && ((which_alternative == 3) && (((lx_cpu) == (CPU_ST210))))))
        {
	  return 2;
        }
      else
        {
	  return 1;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      if (((lx_delay_model) == (DELAY_MODEL_BUNDLE)))
        {
	  return 4;
        }
      else
        {
	  return 1;
        }

    }
}

extern int jmp3_unit_ready_cost PARAMS ((rtx));
int
jmp3_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 1;

    }
}

extern int mem3_unit_ready_cost PARAMS ((rtx));
int
mem3_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 41:
      if (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE)))
        {
	  return 3;
        }
      else
        {
	  return 4;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 3;
        }
      else
        {
	  return 4;
        }

    case 3:
      extract_constrain_insn_cached (insn);
      if (((which_alternative == 1) && (! (((lx_cpu) == (CPU_ST210))))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 3;
        }
      else if (((which_alternative == 1) && (((lx_cpu) == (CPU_ST210)))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 2;
        }
      else if ((which_alternative == 2) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 4;
        }

    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 3;
        }
      else if ((which_alternative == 2) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 4;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (((which_alternative == 3) && (! (((lx_cpu) == (CPU_ST210))))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 3;
        }
      else if (((which_alternative == 3) && (((lx_cpu) == (CPU_ST210)))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 2;
        }
      else if ((which_alternative == 4) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 4;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 4;

    }
}

extern int mul3_unit_ready_cost PARAMS ((rtx));
int
mul3_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      if ((((lx_cpu) == (CPU_ST210))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 2;
        }
      else
        {
	  return 3;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 3;

    }
}

extern int int3_unit_ready_cost PARAMS ((rtx));
int
int3_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 44:
      if ((((lx_cpu) == (CPU_ST210))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 2;
        }
      else
        {
	  return 3;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 0) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
    case 41:
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      return 3;

    case 3:
    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if (((which_alternative != 2) && (which_alternative != 1)) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (((which_alternative != 4) && ((which_alternative != 3) && ((which_alternative != 1) && (which_alternative != 0)))) && (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      if (((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE)))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    }
}

extern int jmp2_unit_ready_cost PARAMS ((rtx));
int
jmp2_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 4;

    }
}

extern int mem2_unit_ready_cost PARAMS ((rtx));
int
mem2_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 41:
      if (((lx_delay_model) == (DELAY_MODEL_BUNDLE)))
        {
	  return 12 /* 0xc */;
        }
      else
        {
	  return 16 /* 0x10 */;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 12 /* 0xc */;
        }
      else
        {
	  return 16 /* 0x10 */;
        }

    case 3:
      extract_constrain_insn_cached (insn);
      if (((which_alternative == 1) && (! (((lx_cpu) == (CPU_ST210))))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 12 /* 0xc */;
        }
      else if (((which_alternative == 1) && (((lx_cpu) == (CPU_ST210)))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 8;
        }
      else if ((which_alternative == 2) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 4;
        }
      else
        {
	  return 16 /* 0x10 */;
        }

    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 12 /* 0xc */;
        }
      else if ((which_alternative == 2) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 4;
        }
      else
        {
	  return 16 /* 0x10 */;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (((which_alternative == 3) && (! (((lx_cpu) == (CPU_ST210))))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 12 /* 0xc */;
        }
      else if (((which_alternative == 3) && (((lx_cpu) == (CPU_ST210)))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 8;
        }
      else if ((which_alternative == 4) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 4;
        }
      else
        {
	  return 16 /* 0x10 */;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 16 /* 0x10 */;

    }
}

extern int mul2_unit_ready_cost PARAMS ((rtx));
int
mul2_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      if ((((lx_cpu) == (CPU_ST210))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 8;
        }
      else
        {
	  return 12 /* 0xc */;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 12 /* 0xc */;

    }
}

extern int int2_unit_ready_cost PARAMS ((rtx));
int
int2_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 44:
      if ((((lx_cpu) == (CPU_ST210))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 8;
        }
      else
        {
	  return 12 /* 0xc */;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 0) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 4;
        }
      else
        {
	  return 12 /* 0xc */;
        }

    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
    case 41:
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      return 12 /* 0xc */;

    case 3:
    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if (((which_alternative != 2) && (which_alternative != 1)) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 4;
        }
      else
        {
	  return 12 /* 0xc */;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (((which_alternative != 4) && ((which_alternative != 3) && ((which_alternative != 1) && (which_alternative != 0)))) && (((lx_delay_model) == (DELAY_MODEL_BUNDLE))))
        {
	  return 4;
        }
      else
        {
	  return 12 /* 0xc */;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      if (((lx_delay_model) == (DELAY_MODEL_BUNDLE)))
        {
	  return 4;
        }
      else
        {
	  return 12 /* 0xc */;
        }

    }
}

extern int jmp1_unit_ready_cost PARAMS ((rtx));
int
jmp1_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 1;

    }
}

extern int mem1_unit_ready_cost PARAMS ((rtx));
int
mem1_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 41:
      if (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE)))
        {
	  return 3;
        }
      else
        {
	  return 4;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 3;
        }
      else
        {
	  return 4;
        }

    case 3:
      extract_constrain_insn_cached (insn);
      if (((which_alternative == 1) && (! (((lx_cpu) == (CPU_ST210))))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 3;
        }
      else if (((which_alternative == 1) && (((lx_cpu) == (CPU_ST210)))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 2;
        }
      else if ((which_alternative == 2) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 4;
        }

    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 3;
        }
      else if ((which_alternative == 2) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 4;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (((which_alternative == 3) && (! (((lx_cpu) == (CPU_ST210))))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 3;
        }
      else if (((which_alternative == 3) && (((lx_cpu) == (CPU_ST210)))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 2;
        }
      else if ((which_alternative == 4) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 4;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 4;

    }
}

extern int mul1_unit_ready_cost PARAMS ((rtx));
int
mul1_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      if ((((lx_cpu) == (CPU_ST210))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 2;
        }
      else
        {
	  return 3;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return 3;

    }
}

extern int int1_unit_ready_cost PARAMS ((rtx));
int
int1_unit_ready_cost (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 44:
      if ((((lx_cpu) == (CPU_ST210))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 2;
        }
      else
        {
	  return 3;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 0) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
    case 41:
    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      return 3;

    case 3:
    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if (((which_alternative != 2) && (which_alternative != 1)) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (((which_alternative != 4) && ((which_alternative != 3) && ((which_alternative != 1) && (which_alternative != 0)))) && (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      if (((lx_delay_model) == (DELAY_MODEL_NOBUNDLE)))
        {
	  return 1;
        }
      else
        {
	  return 3;
        }

    }
}

extern int function_units_used PARAMS ((rtx));
int
function_units_used (insn)
     rtx insn;
{
  enum attr_Unit_type attr_Unit_type = get_attr_Unit_type (insn);
  unsigned long accum = 0;

  accum |= (((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (((attr_Unit_type == UNIT_TYPE_ALU_1) || (attr_Unit_type == UNIT_TYPE_ALU_2)) || (attr_Unit_type == UNIT_TYPE_ALU_3))) ? (1) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && ((attr_Unit_type == UNIT_TYPE_MUL_2) || (attr_Unit_type == UNIT_TYPE_MUL_3))) ? (2) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && ((((attr_Unit_type == UNIT_TYPE_MEM_1) || (attr_Unit_type == UNIT_TYPE_MEM_2)) || (attr_Unit_type == UNIT_TYPE_MEM_3)) || (attr_Unit_type == UNIT_TYPE_MEM_4))) ? (4) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_NOBUNDLE))) && (attr_Unit_type == UNIT_TYPE_BRANCH)) ? (8) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (((attr_Unit_type == UNIT_TYPE_ALU_1) || (attr_Unit_type == UNIT_TYPE_ALU_2)) || (attr_Unit_type == UNIT_TYPE_ALU_3))) ? (16) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && ((attr_Unit_type == UNIT_TYPE_MUL_2) || (attr_Unit_type == UNIT_TYPE_MUL_3))) ? (32) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && ((((attr_Unit_type == UNIT_TYPE_MEM_1) || (attr_Unit_type == UNIT_TYPE_MEM_2)) || (attr_Unit_type == UNIT_TYPE_MEM_3)) || (attr_Unit_type == UNIT_TYPE_MEM_4))) ? (64) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_BUNDLE))) && (attr_Unit_type == UNIT_TYPE_BRANCH)) ? (128) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (((attr_Unit_type == UNIT_TYPE_ALU_1) || (attr_Unit_type == UNIT_TYPE_ALU_2)) || (attr_Unit_type == UNIT_TYPE_ALU_3))) ? (256) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && ((attr_Unit_type == UNIT_TYPE_MUL_2) || (attr_Unit_type == UNIT_TYPE_MUL_3))) ? (512) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && (attr_Unit_type == UNIT_TYPE_BRANCH)) ? (2048) : (0));
  accum |= (((((lx_delay_model) == (DELAY_MODEL_REORG_BUNDLE))) && ((((attr_Unit_type == UNIT_TYPE_MEM_1) || (attr_Unit_type == UNIT_TYPE_MEM_2)) || (attr_Unit_type == UNIT_TYPE_MEM_3)) || (attr_Unit_type == UNIT_TYPE_MEM_4))) ? (1024) : (0));

  if (accum && accum == (accum & -accum))
    {
      int i;
      for (i = 0; accum >>= 1; ++i) continue;
      accum = i;
    }
  else
    accum = ~accum;
  return accum;
}

extern enum attr_LongImm get_attr_LongImm PARAMS ((rtx));
enum attr_LongImm
get_attr_LongImm (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 41:
      extract_insn_cached (insn);
      if (lx_long_add_operand (operands[1], VOIDmode))
        {
	  return LONGIMM_YES;
        }
      else
        {
	  return LONGIMM_NO;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if (which_alternative != 0)
        {
	  if (lx_long_add_operand (operands[1], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }
      else
        {
	  if (lx_long_imm_operand (operands[1], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }

    case 49:
    case 48:
    case 45:
    case 44:
    case 43:
    case 20:
      extract_insn_cached (insn);
      if (lx_long_imm_operand (operands[3], VOIDmode))
        {
	  return LONGIMM_YES;
        }
      else
        {
	  return LONGIMM_NO;
        }

    case 47:
    case 46:
    case 19:
    case 18:
      extract_insn_cached (insn);
      if (lx_long_imm_operand (operands[1], VOIDmode))
        {
	  return LONGIMM_YES;
        }
      else
        {
	  return LONGIMM_NO;
        }

    case 3:
    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 1)
        {
	  if (lx_long_add_operand (operands[1], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }
      else if (which_alternative == 2)
        {
	  if (lx_long_add_operand (operands[0], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }
      else
        {
	  if (lx_long_imm_operand (operands[1], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 3) || (which_alternative == 1))
        {
	  if (lx_long_add_operand (operands[1], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }
      else if (which_alternative == 4)
        {
	  if (lx_long_add_operand (operands[0], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }
      else if ((which_alternative == 2) || (which_alternative == 5))
        {
	  if (lx_long_imm_operand (operands[1], VOIDmode))
	    {
	      return LONGIMM_YES;
	    }
	  else
	    {
	      return LONGIMM_NO;
	    }
        }
      else
        {
	  return LONGIMM_NO;
        }

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    case 63:
    case 62:
    case 61:
    case 60:
    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
    case 23:
    case 4:
      return LONGIMM_NO;

    default:
      extract_insn_cached (insn);
      if (lx_long_imm_operand (operands[2], VOIDmode))
        {
	  return LONGIMM_YES;
        }
      else
        {
	  return LONGIMM_NO;
        }

    }
}

extern enum attr_Unit_type get_attr_Unit_type PARAMS ((rtx));
enum attr_Unit_type
get_attr_Unit_type (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 44:
      if (((lx_cpu) == (CPU_ST210)))
        {
	  return UNIT_TYPE_ALU_2;
        }
      else
        {
	  return UNIT_TYPE_ALU_3;
        }

    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if (which_alternative != 0)
        {
	  return UNIT_TYPE_MEM_3;
        }
      else
        {
	  return UNIT_TYPE_ALU_1;
        }

    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      if (((lx_cpu) == (CPU_ST210)))
        {
	  return UNIT_TYPE_MUL_2;
        }
      else
        {
	  return UNIT_TYPE_MUL_3;
        }

    case 3:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 2)
        {
	  return UNIT_TYPE_MEM_1;
        }
      else if (which_alternative == 1)
        {
	  if (((lx_cpu) == (CPU_ST210)))
	    {
	      return UNIT_TYPE_MEM_2;
	    }
	  else
	    {
	      return UNIT_TYPE_MEM_3;
	    }
        }
      else
        {
	  return UNIT_TYPE_ALU_1;
        }

    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 2)
        {
	  return UNIT_TYPE_MEM_1;
        }
      else if (which_alternative == 1)
        {
	  return UNIT_TYPE_MEM_3;
        }
      else
        {
	  return UNIT_TYPE_ALU_1;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 4)
        {
	  return UNIT_TYPE_MEM_1;
        }
      else if (which_alternative == 3)
        {
	  if (((lx_cpu) == (CPU_ST210)))
	    {
	      return UNIT_TYPE_MEM_2;
	    }
	  else
	    {
	      return UNIT_TYPE_MEM_3;
	    }
        }
      else if (which_alternative == 1)
        {
	  return UNIT_TYPE_MEM_4;
        }
      else if (which_alternative == 0)
        {
	  return UNIT_TYPE_ALU_3;
        }
      else
        {
	  return UNIT_TYPE_ALU_1;
        }

    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
      return UNIT_TYPE_BRANCH;

    case 41:
      return UNIT_TYPE_MEM_3;

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return UNIT_TYPE_ALU_1;

    }
}

extern enum attr_issue_type get_attr_issue_type PARAMS ((rtx));
enum attr_issue_type
get_attr_issue_type (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 42:
    case 40:
    case 39:
      extract_constrain_insn_cached (insn);
      if (which_alternative != 0)
        {
	  return ISSUE_TYPE_M;
        }
      else
        {
	  return ISSUE_TYPE_A;
        }

    case 3:
    case 2:
    case 1:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 1) || (which_alternative == 2))
        {
	  return ISSUE_TYPE_M;
        }
      else
        {
	  return ISSUE_TYPE_A;
        }

    case 0:
      extract_constrain_insn_cached (insn);
      if ((which_alternative == 3) || ((which_alternative == 1) || (which_alternative == 4)))
        {
	  return ISSUE_TYPE_M;
        }
      else
        {
	  return ISSUE_TYPE_A;
        }

    case 61:
      return ISSUE_TYPE_S;

    case 38:
    case 37:
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 30:
    case 29:
    case 28:
    case 27:
    case 26:
    case 25:
    case 24:
      return ISSUE_TYPE_U;

    case 41:
      return ISSUE_TYPE_M;

    case 59:
    case 58:
    case 57:
    case 56:
    case 55:
    case 54:
    case 53:
    case 52:
    case 51:
    case 50:
      return ISSUE_TYPE_B;

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    default:
      return ISSUE_TYPE_A;

    }
}

extern enum attr_insn_type get_attr_insn_type PARAMS ((rtx));
enum attr_insn_type
get_attr_insn_type (insn)
     rtx insn;
{
  switch (recog_memoized (insn))
    {
    case 0:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 0)
        {
	  return INSN_TYPE_MOV2LR;
        }
      else if (which_alternative == 1)
        {
	  return INSN_TYPE_LOADLR;
        }
      else if (which_alternative == 2)
        {
	  return INSN_TYPE_ALU1;
        }
      else if (which_alternative == 3)
        {
	  return INSN_TYPE_LOADW;
        }
      else if (which_alternative == 4)
        {
	  return INSN_TYPE_STORE;
        }
      else
        {
	  return INSN_TYPE_ALU1;
        }

    case 1:
    case 2:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 0)
        {
	  return INSN_TYPE_ALU1;
        }
      else if (which_alternative == 1)
        {
	  return INSN_TYPE_LOADNW;
        }
      else if (which_alternative == 2)
        {
	  return INSN_TYPE_STORE;
        }
      else
        {
	  return INSN_TYPE_ALU1;
        }

    case 3:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 0)
        {
	  return INSN_TYPE_ALU1;
        }
      else if (which_alternative == 1)
        {
	  return INSN_TYPE_LOADW;
        }
      else if (which_alternative == 2)
        {
	  return INSN_TYPE_STORE;
        }
      else
        {
	  return INSN_TYPE_ALU1;
        }

    case 39:
    case 40:
    case 42:
      extract_constrain_insn_cached (insn);
      if (which_alternative == 0)
        {
	  return INSN_TYPE_ALU1;
        }
      else
        {
	  return INSN_TYPE_LOADNW;
        }

    case 61:
      return INSN_TYPE_BUNDLESTOP;

    case -1:
      if (GET_CODE (PATTERN (insn)) != ASM_INPUT
          && asm_noperands (PATTERN (insn)) < 0)
        fatal_insn_not_found (insn);
    case 4:
    case 23:
    case 60:
    case 62:
    case 63:
      return INSN_TYPE_MISC;

    case 59:
      return INSN_TYPE_RETURN;

    case 57:
    case 58:
      return INSN_TYPE_CALL;

    case 54:
    case 55:
    case 56:
      return INSN_TYPE_JUMP;

    case 50:
    case 51:
    case 52:
    case 53:
      return INSN_TYPE_BRANCH;

    case 44:
      return INSN_TYPE_CMP2BR;

    case 43:
      return INSN_TYPE_CMP2SLCT;

    case 20:
    case 45:
    case 48:
    case 49:
      return INSN_TYPE_ALU3;

    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 21:
    case 22:
      return INSN_TYPE_ALU2;

    case 18:
    case 19:
    case 46:
    case 47:
      return INSN_TYPE_ALU1;

    case 41:
      return INSN_TYPE_LOADNW;

    default:
      return INSN_TYPE_MULT;

    }
}

const struct function_unit_desc function_units[] = {
  {"int1", 1, 1, 0, 1, 1, int1_unit_ready_cost, 0, 1, 0, 0}, 
  {"mul1", 2, 1, 0, 1, 1, mul1_unit_ready_cost, 0, 1, 0, 0}, 
  {"mem1", 4, 1, 0, 1, 1, mem1_unit_ready_cost, 0, 1, 0, 0}, 
  {"jmp1", 8, 1, 0, 1, 1, jmp1_unit_ready_cost, 0, 1, 0, 0}, 
  {"int2", 16, 4, 0, 4, 4, int2_unit_ready_cost, 0, 4, 0, 0}, 
  {"mul2", 32, 2, 0, 4, 4, mul2_unit_ready_cost, 0, 4, 0, 0}, 
  {"mem2", 64, 1, 0, 4, 4, mem2_unit_ready_cost, 0, 4, 0, 0}, 
  {"jmp2", 128, 1, 0, 4, 4, jmp2_unit_ready_cost, 0, 4, 0, 0}, 
  {"int3", 256, 4, 0, 1, 1, int3_unit_ready_cost, 0, 1, 0, 0}, 
  {"mul3", 512, 2, 0, 1, 1, mul3_unit_ready_cost, 0, 1, 0, 0}, 
  {"mem3", 1024, 1, 0, 1, 1, mem3_unit_ready_cost, 0, 1, 0, 0}, 
  {"jmp3", 2048, 1, 0, 1, 1, jmp3_unit_ready_cost, 0, 1, 0, 0}, 
};


int max_dfa_issue_rate = 0;
/* Vector translating external insn codes to internal ones.*/
static const unsigned char translate_0[] ATTRIBUTE_UNUSED = {
    0};

/* Vector for state transitions.  */
static const unsigned char transitions_0[] ATTRIBUTE_UNUSED = {
    0};


#if AUTOMATON_STATE_ALTS
/* Vector for state insn alternatives.  */
static const unsigned char state_alts_0[] ATTRIBUTE_UNUSED = {
    1};


#endif /* #if AUTOMATON_STATE_ALTS */

/* Vector of min issue delay of insns.*/
static const unsigned char min_issue_delay_0[] ATTRIBUTE_UNUSED = {
    0};

/* Vector for locked state flags.  */
static const unsigned char dead_lock_0[] = {
    1};


#define DFA__ADVANCE_CYCLE 0

struct DFA_chip
{
  unsigned char automaton_state_0;
};


int max_insn_queue_index = 1;

static int internal_min_issue_delay PARAMS ((int, struct DFA_chip *));
static int
internal_min_issue_delay (insn_code, chip)
	int insn_code;
	struct DFA_chip *chip  ATTRIBUTE_UNUSED;
{
  int temp ATTRIBUTE_UNUSED;
  int res = -1;

  switch (insn_code)
    {
    case 0: /* $advance_cycle */
      break;


    default:
      res = -1;
      break;
    }
  return res;
}

static int internal_state_transition PARAMS ((int, struct DFA_chip *));
static int
internal_state_transition (insn_code, chip)
	int insn_code;
	struct DFA_chip *chip  ATTRIBUTE_UNUSED;
{
  int temp ATTRIBUTE_UNUSED;

  switch (insn_code)
    {
    case 0: /* $advance_cycle */
      {
        return -1;
      }

    default:
      return -1;
    }
}


static int *dfa_insn_codes;

static int dfa_insn_codes_length;

#ifdef __GNUC__
__inline__
#endif
static int dfa_insn_code PARAMS ((rtx));
static int
dfa_insn_code (insn)
	rtx insn;
{
  int insn_code;
  int temp;

  if (INSN_UID (insn) >= dfa_insn_codes_length)
    {
      temp = dfa_insn_codes_length;
      dfa_insn_codes_length = 2 * INSN_UID (insn);
      dfa_insn_codes = xrealloc (dfa_insn_codes, dfa_insn_codes_length * sizeof (int));
      for (; temp < dfa_insn_codes_length; temp++)
        dfa_insn_codes [temp] = -1;
    }
  if ((insn_code = dfa_insn_codes [INSN_UID (insn)]) < 0)
    {
      insn_code = internal_dfa_insn_code (insn);
      dfa_insn_codes [INSN_UID (insn)] = insn_code;
    }
  return insn_code;
}

int
state_transition (state, insn)
	state_t state;
	rtx insn;
{
  int insn_code;

  if (insn != 0)
    {
      insn_code = dfa_insn_code (insn);
      if (insn_code > DFA__ADVANCE_CYCLE)
        return -1;
    }
  else
    insn_code = DFA__ADVANCE_CYCLE;

  return internal_state_transition (insn_code, state);
}


#if AUTOMATON_STATE_ALTS

static int internal_state_alts PARAMS ((int, struct DFA_chip *));
static int
internal_state_alts (insn_code, chip)
	int insn_code;
	struct DFA_chip *chip;
{
  int res;

  switch (insn_code)
    {
    case 0: /* $advance_cycle */
      {
        break;
      }


    default:
      res = 0;
      break;
    }
  return res;
}

int
state_alts (state, insn)
	state_t state;
	rtx insn;
{
  int insn_code;

  if (insn != 0)
    {
      insn_code = dfa_insn_code (insn);
      if (insn_code > DFA__ADVANCE_CYCLE)
        return 0;
    }
  else
    insn_code = DFA__ADVANCE_CYCLE;

  return internal_state_alts (insn_code, state);
}


#endif /* #if AUTOMATON_STATE_ALTS */

int
min_issue_delay (state, insn)
	state_t state;
	rtx insn;
{
  int insn_code;

  if (insn != 0)
    {
      insn_code = dfa_insn_code (insn);
      if (insn_code > DFA__ADVANCE_CYCLE)
        return 0;
    }
  else
    insn_code = DFA__ADVANCE_CYCLE;

  return internal_min_issue_delay (insn_code, state);
}

static int internal_state_dead_lock_p PARAMS ((struct DFA_chip *));
static int
internal_state_dead_lock_p (chip)
	struct DFA_chip *chip;
{
  if (dead_lock_0 [chip->automaton_state_0])
    return 1/* TRUE */;
  return 0/* FALSE */;
}

int
state_dead_lock_p (state)
	state_t state;
{
  return internal_state_dead_lock_p (state);
}

int
state_size ()
{
  return sizeof (struct DFA_chip);
}

static void internal_reset PARAMS ((struct DFA_chip *));
static void
internal_reset (chip)
	struct DFA_chip *chip;
{
  memset (chip, 0, sizeof (struct DFA_chip));
}

void
state_reset (state)
	 state_t state;
{
  internal_reset (state);
}

int
min_insn_conflict_delay (state, insn, insn2)
	state_t state;
	rtx insn;
	rtx insn2;
{
  struct DFA_chip DFA_chip;
  int insn_code, insn2_code;

  if (insn != 0)
    {
      insn_code = dfa_insn_code (insn);
      if (insn_code > DFA__ADVANCE_CYCLE)
        return 0;
    }
  else
    insn_code = DFA__ADVANCE_CYCLE;


  if (insn2 != 0)
    {
      insn2_code = dfa_insn_code (insn2);
      if (insn2_code > DFA__ADVANCE_CYCLE)
        return 0;
    }
  else
    insn2_code = DFA__ADVANCE_CYCLE;

  memcpy (&DFA_chip, state, sizeof (DFA_chip));
  internal_reset (&DFA_chip);
  if (internal_state_transition (insn_code, &DFA_chip) > 0)
    abort ();
  return internal_min_issue_delay (insn2_code, &DFA_chip);
}

static int internal_insn_latency PARAMS ((int, int, rtx, rtx));
static int
internal_insn_latency (insn_code, insn2_code, insn, insn2)
	int insn_code;
	int insn2_code;
	rtx insn ATTRIBUTE_UNUSED;
	rtx insn2 ATTRIBUTE_UNUSED;
{
  switch (insn_code)
    {
    case 0:
      return (insn2_code != DFA__ADVANCE_CYCLE ? 0 : 0);
    default:
      return 0;
    }
}

int
insn_latency (insn, insn2)
	rtx insn;
	rtx insn2;
{
  int insn_code, insn2_code;

  if (insn != 0)
    {
      insn_code = dfa_insn_code (insn);
      if (insn_code > DFA__ADVANCE_CYCLE)
        return 0;
    }
  else
    insn_code = DFA__ADVANCE_CYCLE;


  if (insn2 != 0)
    {
      insn2_code = dfa_insn_code (insn2);
      if (insn2_code > DFA__ADVANCE_CYCLE)
        return 0;
    }
  else
    insn2_code = DFA__ADVANCE_CYCLE;

  return internal_insn_latency (insn_code, insn2_code, insn, insn2);
}

void
print_reservation (f, insn)
	FILE *f;
	rtx insn;
{
  int insn_code;

  if (insn != 0)
    {
      insn_code = dfa_insn_code (insn);
      if (insn_code > DFA__ADVANCE_CYCLE)
        {
          fprintf (f, "nothing");
          return;
        }
    }
  else
    {
      fprintf (f, "nothing");
      return;
    }
  switch (insn_code)
    {
    default:
      fprintf (f, "nothing");
    }
}

void
dfa_start ()
{
  int i;

  dfa_insn_codes_length = get_max_uid ();
  dfa_insn_codes = (int *) xmalloc (dfa_insn_codes_length * sizeof (int));
  for (i = 0; i < dfa_insn_codes_length; i++)
    dfa_insn_codes [i] = -1;
}

void
dfa_finish ()
{
  free (dfa_insn_codes);
}

int length_unit_log = 2;
