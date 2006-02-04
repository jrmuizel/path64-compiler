/* Definitions of target machine GNU compiler for the ST200.
   Christian Bruel <christian.bruel@st.com>


This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "system.h"
#include "tree.h"
#include "c-tree.h"
#include "rtl.h"
#include "real.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "insn-flags.h"
#include "insn-attr.h"
#include "expr.h"
#include "flags.h"
#include "function.h"
#include "toplev.h"
#include "insn-config.h"
#include "recog.h"
#include "output.h"
#include "target.h"
#include "target-def.h"
#include "langhooks.h"

/* Define the information needed to generate branch and slct insns.
   This is stored from the compare operation. */
struct rtx_def * lx_compare_op0;
struct rtx_def * lx_compare_op1;

int lx_cpu;
int lx_delay_model;
const char *lx_cpu_string;

#undef TARGET_HAVE_TLS
#define TARGET_HAVE_TLS false


struct gcc_target targetm = TARGET_INITIALIZER;

/* dummy function. assembly code done by open64 code generator */
void default_lxopen64_assemble (tree t, int v) { }

int override_options ()
{
  flag_omit_frame_pointer = 1;

  if (lx_cpu_string) {
    if (! strcmp (lx_cpu_string, "st200") ||
	! strcmp (lx_cpu_string, "st210"))
        lx_cpu = (int)CPU_ST210;
    else if (! strcmp (lx_cpu_string, "st220"))
        lx_cpu = (int)CPU_ST220;
    else
      error ("Unknown -mcpu=%s.\nValid cpus are st200, st210, st220\n",
	     lx_cpu_string);
  }
  else {
    /* default */
    lx_cpu = (int)CPU_ST220; /* default */
  }
}

/* Print an operand to a assembler instruction. */
void
lx_print_operand (file, op, letter)
     FILE * file;
     rtx    op;
     int    letter;
{
  /* General cases, more or less independent of CODE(x) */
  switch (letter)
    {
    case 'j':
      switch (GET_CODE (op))
	{
	case EQ:
	  fputs ("eq", file);
	  break;
	case NE:
	  fputs ("ne", file);
	  break;
	case LE:
	  fputs ("le", file);
	  break;
	case GT:
	  fputs ("gt", file);
	  break;
	case LT:
	  fputs ("lt", file);
	  break;
	case GE:
	  fputs ("ge", file);
	  break;
	case LEU:
	  fputs ("leu", file);
	  break;
	case LTU:
	  fputs ("ltu", file);
	  break;
	case GTU:
	  fputs ("gtu", file);
	  break;
	case GEU:
	  fputs ("geu", file);
	  break;
	default:
	  abort();
	}
      return;

    default:
	// [HK] label at end of compound statement is deprecated in gcc-3-4-0
	break;
    }

  switch (GET_CODE (op))
    {
    case REG:
      {
        int reg = REGNO(op);
	if (GR_REGNO_P (reg) || LINK_REGNO_P (reg))
	  fputs ("$r0", file);
	else if (BR_REGNO_P (reg))
	  fputs ("$b0", file);	
	else
	  abort();
        fputs (reg_names[reg], file);
	return;
      }

    case CONST_INT:
      {
	output_addr_const (file, op);
	return;
      }

    case CONST_DOUBLE:
      {
	REAL_VALUE_TYPE rv;
	unsigned long k;

	REAL_VALUE_FROM_CONST_DOUBLE (rv, op);
	REAL_VALUE_TO_TARGET_SINGLE (rv, k);

	fprintf (file, HOST_WIDE_INT_PRINT_HEX, k);
	return;
      }

    case MEM:
      {
	rtx x = XEXP (op, 0);
	if (CONSTANT_P (x))
	  {
	    output_addr_const(file, x);
	    if (!(letter == 'O'))
	      fputs("[$r0.0]", file);
	    return;
	  }

	if (GET_CODE (x) == REG)
	  {
	    fputs ("0[$r0", file);
	    fputs (reg_names[REGNO (x)], file);
	    fputc (']', file);
	    return;
	  }

	if (GET_CODE (x) == PLUS)
	  {
	    rtx x1, x2;
	    rtx offset, reg;

	    x1 = XEXP (x, 0);
	    x2 = XEXP (x, 1);

	    if (REG_P (x1) && CONSTANT_P (x2))
	      {
		offset = x2;
		reg = x1;
	      }

	    else if (REG_P (x2) && CONSTANT_P (x1))
	      {
		offset = x1;
		reg = x2;
	      }
	
	    else abort ();

	    output_addr_const (file, offset);
	    fputs ("[$r0", file);
	    fputs (reg_names[REGNO (reg)], file);
	    fputc (']', file);
	    return ;
	  }
      }

    default:
      output_addr_const (file, op);
      return;
    }

  abort();
}



int
lx_long_imm_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (CONSTANT_P(op)) {
    if ((GET_CODE (op) == LABEL_REF) || (GET_CODE (op) == SYMBOL_REF) ||
        (GET_CODE (op) == CONST))
      /* 32-bit wide Link-time computed address */
      return 1;

    if (GET_CODE (op) == CONST_DOUBLE)
      return 1;

    if (GET_CODE (op) == CONST_INT)
      if ((INTVAL(op) > 255) || (INTVAL(op) < -256))
        return 1;
      else
        return 0;

    fatal_insn ("Unrecognized Long Immediate (1):", op);
    return -1;
  }

  if (REG_P(op))
    return 0;

  /* Why did you called me? */
  fatal_insn ("Unrecognized Long Immediate (2):", op);
  return -1;
}



int
lx_long_add_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  rtx add;

  if (! MEM_P (op))
    return 0;    /* Not a memory operand */

  add = XEXP (op, 0);
#if 0
  debug_rtx (op);
#endif
  if (REG_P(add))
    return 0;    /* the address is simply a register */

  if (CONSTANT_P(add)) {
    if ((GET_CODE (add) == LABEL_REF) || (GET_CODE (add) == SYMBOL_REF) ||
        (GET_CODE (add) == CONST))
      return 1;  /* 32-bit wide Link-time computed address */

    if (GET_CODE (add) == CONST_INT)     /* immediate address */
      return lx_long_imm_operand (add, GET_MODE(add));
  }

  if (GET_CODE (add) == PLUS) {
    rtx x1, x2;
    x1 = XEXP (add, 0);
    x2 = XEXP (add, 1);

    if (REG_P (x1) && CONSTANT_P (x2))
      return lx_long_imm_operand (x2, GET_MODE(x2));

    if (REG_P (x2) && CONSTANT_P (x1))
      return lx_long_imm_operand (x1, GET_MODE(x1));
  }

  /* don't know, so be safe */
  fatal_insn ("Unrecognized Long Address:", op);
  return -1;
}

int
lx_shXadd_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  if (GET_CODE (op) == CONST_INT) {
    if ((INTVAL (op) == 2) ||  /* sh1add */
	(INTVAL (op) == 4) ||  /* sh2add */
	(INTVAL (op) == 8) ||  /* sh3add */
	(INTVAL (op) == 16))   /* sh4add */
      return 1;
  }
  return 0;
}

#if 0
rtx
make_call_from (call)
     rtx call;
{
  if ((MEM_P (call) &&
       GET_CODE (XEXP (call, 0)) != SYMBOL_REF &&
       !REG_P (XEXP (call, 0))) ||
      (REG_P (XEXP (call, 0)) && REGNO (XEXP (call, 0)) !=
       RETURN_POINTER_REGNUM))
    {
      rtx reg = gen_rtx (REG, SImode, RETURN_POINTER_REGNUM);
      emit_move_insn (reg, call);
      call = reg;
    }

  return call;
}
#endif



HARD_REG_SET current_frame_mask;	/* mask of saved registers.  */

unsigned int
lx_compute_frame_size (unsigned int size)
{
  HARD_REG_SET mask;
  int r_size = 0;
  int regno;

  CLEAR_HARD_REG_SET (mask);

  /* Calculate space needed for general registers.  */
  /* We never need to save any of the stacked registers  */
  for (regno = 0; regno <= LAST_GR_REGNUM; regno++)
    if (regs_ever_live[regno] && ! call_used_regs[regno])
      {
	SET_HARD_REG_BIT (mask, regno);
	r_size += 4;
      }

  COPY_HARD_REG_SET (current_frame_mask, mask);

  return r_size + size + current_function_outgoing_args_size;
}

#define VARARG_EXTRA_SLOTS                                   \
         (((MAX_ARGUMENT_SLOTS - current_function_args_info) \
	   * UNITS_PER_WORD)                                 \
	  - STACK_POINTER_OFFSET)

int
lx_elimination_offset (int from, int to)
{
  unsigned int offset;
  
    if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM) {
      offset = lx_compute_frame_size (get_frame_size ());

      if (offset != 0)
	offset += STACK_POINTER_OFFSET;

      offset += STACK_POINTER_OFFSET;
    }
  else if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM) {
      offset = get_frame_size () + current_function_outgoing_args_size;  
      offset -= (current_function_stdarg) ?
	         VARARG_EXTRA_SLOTS : 0;

      if (offset != 0)
	offset += STACK_POINTER_OFFSET;
    }
  else if (to == HARD_FRAME_POINTER_REGNUM) {
    offset = get_frame_size () + current_function_outgoing_args_size;  
      offset -= (current_function_stdarg) ?
	         VARARG_EXTRA_SLOTS : 0;

      if (offset != 0)
	offset += STACK_POINTER_OFFSET;
    }
  else {
    fprintf(stderr, "lx_elimination_offset, failure %d -> %d\n", from, to);
    abort ();
  }

  return offset;

}

void
save_restore_insns (save_p)
     int save_p;
{
  int regno;
  int offset = STACK_POINTER_OFFSET + get_frame_size() +
    current_function_outgoing_args_size;  

  /* start to save after the varargs */
  offset -= (current_function_stdarg) ?
    VARARG_EXTRA_SLOTS : 0;

  for (regno = 0; regno <= LAST_GR_REGNUM; regno++)
    if (TEST_HARD_REG_BIT (current_frame_mask, regno))
      {
	rtx reg, mem;

	reg = gen_rtx_REG (SImode, regno);
	mem = gen_rtx_MEM (SImode,
				gen_rtx (PLUS, Pmode, stack_pointer_rtx,
					 GEN_INT (offset)));

	offset += GET_MODE_SIZE (SImode);

	if (save_p)
	    emit_move_insn (mem, reg);
	else
	    emit_move_insn (reg, mem);
      }
}

/* Return true if REGNO is used by the epilogue.  */
int
lx_epilogue_uses (regno)
     int regno;
{
  return 0;
}

void
lx_function_prologue (file, size)
     FILE *file;
     int size;
{
}

void
lx_function_epilogue (file, size)
     FILE *file;
     int size;
{
}

/* Return 1  if this function is known to have a null epilogue.
   This allows the optimizer to omit jumps to jumps if no stack
   was created.  */
int
lx_direct_return (void)
{
  if (! reload_completed)
    return 0;

  return 1;
}

/* Called after register allocation to add any instructions needed for the
   prologue.  Using a prologue insn is favored compared to putting all of the
   instructions in the FUNCTION_PROLOGUE macro, since it allows the scheduler
   to intermix instructions with the saves of the caller saved registers.  In
   some cases, it might be necessary to emit a barrier instruction as the last
   insn to prevent such scheduling.

   Also any insns generated here should have RTX_FRAME_RELATED_P(insn) = 1
   so that the debug info generation code can handle them properly.  */
void
lx_expand_prologue ()
{
  rtx insn;
  int frame_size = lx_compute_frame_size (get_frame_size());

  if (frame_size != 0)
    {
      frame_size += STACK_POINTER_OFFSET;
      insn = GEN_INT (-frame_size);
      insn = emit_insn (gen_addsi3 (stack_pointer_rtx, stack_pointer_rtx,
				    insn));
      RTX_FRAME_RELATED_P (insn) = 1;
    }

  /* Save registers to frame.  */
  save_restore_insns (1);
}

/* Called after register allocation to add any instructions needed for the
   epilogue.  Using a epilogue insn is favored compared to putting all of the
   instructions in the FUNCTION_PROLOGUE macro, since it allows the scheduler
   to intermix instructions with the saves of the caller saved registers.  In
   some cases, it might be necessary to emit a barrier instruction as the last
   insn to prevent such scheduling.  */
void
lx_expand_epilogue ()
{
  int frame_size = lx_compute_frame_size (get_frame_size());
  rtx insn;

  /* Restore registers from frame.  */
  save_restore_insns (0);

  if (frame_size != 0)
    {
      frame_size += STACK_POINTER_OFFSET;
      insn = GEN_INT (frame_size);
      insn = emit_insn (gen_addsi3 (stack_pointer_rtx, stack_pointer_rtx,
				    insn));
    }

  emit_jump_insn (gen_return ());
}

/* This function returns the register class required for a secondary
   register when copying between one of the registers in CLASS, and X,
   using MODE.  A return value of NO_REGS means that no secondary register
   is required.  */
enum reg_class
lx_secondary_reload_class (class, mode, x)
     enum reg_class class;
     enum machine_mode mode;
     rtx x;
{
  /* if (mode == CCmode) */
  if (GET_MODE_CLASS (mode) == MODE_CC)
    return GR_REGS;

  return NO_REGS;
}

/* Return rtx for register where argument is passed, or zero if it is passed
   on the stack.  */
rtx
lx_function_arg (cum, mode, type, named)
     CUMULATIVE_ARGS *cum;
     enum machine_mode mode;
     tree type;
     int named;
{
  int words = (((mode == BLKmode ? int_size_in_bytes (type)
		 : GET_MODE_SIZE (mode)) + UNITS_PER_WORD - 1)
	       / UNITS_PER_WORD);
  int offset = 0;

  /* Arguments larger than 4 bytes start at the next even boundary.  */
  if (words > 1 && (*cum & 1))
    offset = 1;

  /* If all argument slots are used, then it must go on the stack.  */
  if (*cum + offset >= MAX_ARGUMENT_SLOTS)
    return 0;

  *cum += offset;

  return gen_rtx_REG (mode, GP_ARG_REGNUM + *cum);
}


/* Return the number of words, at the beginning of an argument,
   that must be put in registers */
int
lx_function_arg_partial_nregs (cum, mode, type, named)
     CUMULATIVE_ARGS cum;
     enum machine_mode mode;
     tree type;
     int named;
{
  int words = (((mode == BLKmode ? int_size_in_bytes (type)
		 : GET_MODE_SIZE (mode)) + UNITS_PER_WORD - 1)
	       / UNITS_PER_WORD);
  int offset = 0;

  /* Arguments larger than 4 bytes start at the next even boundary.  */
  if (words > 1 && (cum & 1))
    offset = 1;

  /* If all argument slots are already used, then it must go on the stack.  */
  if ((cum + offset) >= MAX_ARGUMENT_SLOTS)
    return 0;

  /* If some argument slots are still available, but not
     sufficient, part of the argument must go on the stack */
  else if ((cum + offset + words) > MAX_ARGUMENT_SLOTS)
    return MAX_ARGUMENT_SLOTS - (cum + offset);

  /* Otherwise, the argument entirely fits into registers */
  return 0;
}

int
lx_must_pass_in_stack (mode, type)
     enum machine_mode mode;
     tree type;
{
  return 0;
}

/* Update CUM to point after this argument. */
void
lx_function_arg_advance (cum, mode, type, named)
     CUMULATIVE_ARGS *cum;
     enum machine_mode mode;
     tree type;
     int named;
{
  int words = (((mode == BLKmode ? int_size_in_bytes (type)
		 : GET_MODE_SIZE (mode)) + UNITS_PER_WORD - 1)
	       / UNITS_PER_WORD);
  int offset = 0;

  /* If all arg slots are already full, then there is nothing to do.  */
  if (*cum >= MAX_ARGUMENT_SLOTS)
    return;

  /* Arguments larger than 4 bytes start at the next even boundary. */
  if (words > 1 && (*cum & 1))
    offset = 1;

  *cum += words + offset;
}

tree
lx_build_va_list ()
{
  if (TARGET_BIG_ENDIAN)
    {
      tree record, f_next, f_reg_limit;

      record = (*lang_hooks.types.make_type) (RECORD_TYPE);

      f_next = build_decl (FIELD_DECL, get_identifier ("__next"),
			   ptr_type_node);
      f_reg_limit = build_decl (FIELD_DECL, get_identifier ("__reg_limit"),
				ptr_type_node);

      DECL_FIELD_CONTEXT (f_next) = record;
      DECL_FIELD_CONTEXT (f_reg_limit) = record;

      TYPE_FIELDS (record) = f_next;
      TREE_CHAIN (f_next) = f_reg_limit;

      layout_type (record);

      return record;
    }
  else
    return ptr_type_node;
}

/* Do what is necessary for `va_start'.  The argument is ignored. */
rtx
lx_builtin_saveregs ()
{
  int regno;
  int offset, slot;
  
  /* Allocate the va_list constructor */
  assign_stack_local (BLKmode, VARARG_EXTRA_SLOTS, 0);
  offset = (MAX_ARGUMENT_SLOTS - current_function_args_info) * UNITS_PER_WORD;

  slot = 0;

  /* use arg_pointer since saved register slots are not known at that time */
  for (regno = current_function_args_info; regno < MAX_ARGUMENT_SLOTS; regno++) {
    emit_move_insn (gen_rtx_MEM (SImode,
				 gen_rtx_PLUS (Pmode, arg_pointer_rtx,
					       GEN_INT ((slot++ * UNITS_PER_WORD) - offset))),
		    gen_rtx_REG (SImode, GP_ARG_REGNUM + regno));
  }
				 
  return force_reg (Pmode, gen_rtx_PLUS (Pmode, arg_pointer_rtx, GEN_INT (-offset)));
}

void
lx_va_start (valist, nextarg)
     tree valist;
     rtx nextarg;
{
  tree f_next, f_reg_limit;
  tree a_next, a_reg_limit;
  tree t, u;

  if (! TARGET_BIG_ENDIAN)
    {
      expand_builtin_saveregs ();
      std_expand_builtin_va_start (valist, nextarg);
      return;
    }

  f_next = TYPE_FIELDS (va_list_type_node);
  f_reg_limit = TREE_CHAIN (f_next);

  a_next = build (COMPONENT_REF, TREE_TYPE (f_next), valist, f_next);
  a_reg_limit = build (COMPONENT_REF, TREE_TYPE (f_reg_limit),
		       valist, f_reg_limit);

  u = make_tree (ptr_type_node, expand_builtin_saveregs ());
  t = build (MODIFY_EXPR, ptr_type_node, a_next, u);
  TREE_SIDE_EFFECTS (t) = 1;
  expand_expr (t, const0_rtx, VOIDmode, EXPAND_NORMAL);

  u = fold (build (MINUS_EXPR, ptr_type_node, u,
		   build_int_2 (UNITS_PER_WORD * MAX_ARGUMENT_SLOTS, 0)));
  t = build (MODIFY_EXPR, ptr_type_node, a_reg_limit, u);
  TREE_SIDE_EFFECTS (t) = 1;
  expand_expr (t, const0_rtx, VOIDmode, EXPAND_NORMAL);
}

/* Emit RTL insns to initialize the variable parts of a trampoline.
   FNADDR is an RTX for the address of the function's pure code.
   CXT is an RTX for the static chain value for the function.  */

void
lx_initialize_trampoline (tramp, fnaddr, cxt)
     rtx tramp, fnaddr, cxt;
{
  rtx cxt_lo = gen_reg_rtx (SImode), cxt_hi = gen_reg_rtx (SImode);
  rtx fnaddr_lo = gen_reg_rtx (SImode), fnaddr_hi = gen_reg_rtx (SImode);
  
  emit_move_insn (gen_rtx_MEM (SImode, tramp), GEN_INT (0x00009fc0));
  fnaddr = force_reg (SImode, tramp);
  emit_insn (gen_andsi3 (fnaddr_lo, fnaddr, GEN_INT (0x1ff)));
  emit_insn (gen_ashlsi3 (fnaddr_lo, fnaddr_lo, GEN_INT (12)));
  emit_insn (gen_iorsi3 (fnaddr_lo, fnaddr_lo, GEN_INT (0x08000fc0)));
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (tramp, 4)),
	     fnaddr_lo);
  emit_insn (gen_lshrsi3 (fnaddr_hi, fnaddr, GEN_INT (9)));
  emit_insn (gen_andsi3 (fnaddr_hi, fnaddr_hi, GEN_INT (0x7fffff)));
  emit_insn (gen_iorsi3 (fnaddr_hi, fnaddr_hi, GEN_INT (0x95000000)));
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (tramp, 8)),
	     fnaddr_hi);
  cxt = force_reg (SImode, cxt);
  emit_insn (gen_andsi3 (cxt_lo, cxt, GEN_INT (0x1ff)));
  emit_insn (gen_ashlsi3 (cxt_lo, cxt_lo, GEN_INT (12)));
  emit_insn (gen_iorsi3 (cxt_lo, cxt_lo, GEN_INT (0x08000200)));
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (tramp, 12)),
	     cxt_lo);
  emit_insn (gen_lshrsi3 (cxt_hi, cxt, GEN_INT (9)));
  emit_insn (gen_andsi3 (cxt_hi, cxt_hi, GEN_INT (0x7fffff)));
  emit_insn (gen_iorsi3 (cxt_hi, cxt_hi, GEN_INT (0x95000000)));
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (tramp, 16)),
	     cxt_hi);
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (tramp, 20)),
             GEN_INT (0x31800000));
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (tramp, 24)),
             GEN_INT (0x8003f240));
}

int localedir; /* XXX */
