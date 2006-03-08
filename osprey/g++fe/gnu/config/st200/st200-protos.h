/* Prototypes for st200.c functions used in the md file & elsewhere.
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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

#ifndef GCC_ST200_PROTOS_H
#define GCC_ST200_PROTOS_H

#ifdef RTX_CODE
extern rtx  lx_builtin_saveregs           PARAMS ((void));
extern int  lx_long_imm_operand           PARAMS ((rtx, enum machine_mode));
extern int  lx_long_add_operand           PARAMS ((rtx, enum machine_mode));
extern void lx_print_operand              PARAMS ((FILE *, rtx, int));
extern enum reg_class lx_secondary_reload_class  PARAMS ((enum reg_class, enum machine_mode, rtx));
extern void lx_reorg                      PARAMS ((rtx));

#ifdef TREE_CODE
extern rtx  lx_function_arg               PARAMS ((CUMULATIVE_ARGS *, enum machine_mode, tree, int));
extern void lx_va_start                   PARAMS ((tree, rtx));
extern rtx  lx_va_arg                     PARAMS ((tree, tree));

#endif /* TREE_CODE */
#endif /* RTX_CODE */

#ifdef TREE_CODE
extern int  lx_function_arg_partial_nregs PARAMS ((CUMULATIVE_ARGS, enum machine_mode, tree, int));
extern void lx_function_arg_advance       PARAMS ((CUMULATIVE_ARGS *, enum machine_mode, tree, int named));
extern tree lx_build_va_list              PARAMS ((void));
extern int  lx_must_pass_in_stack     	  PARAMS ((enum machine_mode, tree));
#endif /* TREE_CODE */

extern int  lx_direct_return              PARAMS ((void));
extern void lx_override_options           PARAMS ((void));
extern int  lx_elimination_offset         PARAMS ((int, int));

#endif /* GCC_ST200_PROTOS_H */
