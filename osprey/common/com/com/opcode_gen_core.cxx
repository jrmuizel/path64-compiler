/* opcode_gen_core.cxx: This file automatically generated. */

/*

  Copyright (C) 2001 ST Microelectronics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  ST Microelectronics, Inc., 
  address, or:

  http://www.st.com

  For further information regarding this notice, see:

  http://mcdt home page

*/

/* Initialization of operator information */

#include "errors.h"
#include "opcode.h"
#include "config.h"

struct OPERATOR_info_struct OPERATOR_info[OPERATOR_LAST+1] = {

  {"UNKNOWN_OPERATOR"},

  {"OPR_ABS", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_ADD", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_AGOTO", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_ALTENTRY", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym},

  {"OPR_ARRAY", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_ARRAY /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_ndim	|
   OPERATOR_PROPERTY_esize},

  {"OPR_ARRAYEXP", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_ARRSECTION", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_ARRAY /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_ndim	|
   OPERATOR_PROPERTY_esize},

  {"OPR_ASHR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_ASSERT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset},

  {"OPR_BACKWARD_BARRIER", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_barrier},

  {"OPR_BAND", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_BIOR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_BLOCK", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_BNOR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_BNOT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_BXOR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_CALL", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_CALL /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_call	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_flags},

  {"OPR_CAND", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_CASEGOTO", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label	|
   OPERATOR_PROPERTY_value},

  {"OPR_CEIL", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_CIOR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_COMMA", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_COMMENT", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_not_executable	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym},

  {"OPR_COMPGOTO", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_num_entries	|
   OPERATOR_PROPERTY_last_label},

  {"OPR_PAIR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_CONST", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_sym},

  {"OPR_CSELECT", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_CVT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_CVTL", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_bits},

  {"OPR_DIV", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_DIVREM", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_DO_LOOP", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_DO_WHILE", 
   2 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_EQ", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_compare	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_EVAL", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_EXC_SCOPE_BEGIN", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_ereg_supp},

  {"OPR_EXC_SCOPE_END", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset},

  {"OPR_FALSEBR", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label},

  {"OPR_FLOOR", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_FORWARD_BARRIER", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_barrier},

  {"OPR_FUNC_ENTRY", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_HDR /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym},

  {"OPR_GE", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_compare	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_GOTO", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label},

  {"OPR_GT", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_compare	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_HIGHMPY", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_HIGHPART", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_ICALL", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_CALL /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_call	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_flags	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_IDNAME", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset},

  {"OPR_IF", 
   3 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_ILDA", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_ILDBITS", 
   1 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_load	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_2ty},

  {"OPR_ILOAD", 
   1 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_load	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_2ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_ILOADX", 
   2 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_load	|
   OPERATOR_PROPERTY_2ty},

  {"OPR_SECONDPART", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_INTCONST", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_value},

  {"OPR_INTRINSIC_CALL", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_CALL /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_call	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_flags	|
   OPERATOR_PROPERTY_inumber},

  {"OPR_INTRINSIC_OP", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_flags	|
   OPERATOR_PROPERTY_inumber},

  {"OPR_IO", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_flags	|
   OPERATOR_PROPERTY_inumber},

  {"OPR_IO_ITEM", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_inumber	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_ISTBITS", 
   2 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_store	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_ISTORE", 
   2 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_store	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_ISTOREX", 
   3 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_store	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_LABEL", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_not_executable	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label	|
   OPERATOR_PROPERTY_flags},

  {"OPR_LAND", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_LDA", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_LDBITS", 
   0 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_load	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_LDID", 
   0 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_load	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_LE", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_compare	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_LIOR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_LNOT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_LOOP_INFO", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_2offsets	|
   OPERATOR_PROPERTY_flags},

  {"OPR_LOWPART", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_LSHR", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_LT", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_compare	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_MADD", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MAX", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MAXPART", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MIN", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MINMAX", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MINPART", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MLOAD", 
   2 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_load	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_MOD", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MPY", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_MSTORE", 
   3 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_store	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_MSUB", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_NE", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_compare	|
   OPERATOR_PROPERTY_boolean},

  {"OPR_NEG", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_NMADD", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_NMSUB", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_OPTPARM", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_not_executable},

  {"OPR_OPT_CHI", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_OPT_RESERVE2", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_not_executable},

  {"OPR_PAREN", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_PARM", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_not_executable	|
   OPERATOR_PROPERTY_flags	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_PICCALL", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_CALL /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_call	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_flags},

  {"OPR_PRAGMA", 
   0 /* nkids */,
   OPERATOR_MAPCAT_PRAGMA /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_not_executable	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_value},

  {"OPR_PREFETCH", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_prefetch	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_flags},

  {"OPR_PREFETCHX", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_prefetch	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_flags},

  {"OPR_RCOMMA", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_FIRSTPART", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_RECIP", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_REGION", 
   3 /* nkids */,
   OPERATOR_MAPCAT_HDR /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_ereg_supp},

  {"OPR_REGION_EXIT", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label},

  {"OPR_REM", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_RETURN", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_RETURN_VAL", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_RND", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_RSQRT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_SELECT", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_SHL", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_SQRT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_STBITS", 
   1 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_store	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_STID", 
   1 /* nkids */,
   OPERATOR_MAPCAT_LDST /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_store	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_SUB", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_SWITCH", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_num_entries	|
   OPERATOR_PROPERTY_last_label},

  {"OPR_TAS", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_TRAP", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_offset},

  {"OPR_TRIPLET", 
   3 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_TRUEBR", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label},

  {"OPR_TRUNC", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_VFCALL", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_CALL /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_call	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_flags	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_WHERE", 
   3 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_WHILE_DO", 
   2 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_scf	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_XGOTO", 
   2 /* nkids */,
   OPERATOR_MAPCAT_SCF /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_num_entries},

  {"OPR_XMPY", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_XPRAGMA", 
   1 /* nkids */,
   OPERATOR_MAPCAT_PRAGMA /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_not_executable	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset},

  {"OPR_AFFIRM", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_ALLOCA", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_DEALLOCA", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev},

  {"OPR_LDMA", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_offset	|
   OPERATOR_PROPERTY_1ty	|
   OPERATOR_PROPERTY_field_id},

  {"OPR_ASM_STMT", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_flags},

  {"OPR_ASM_EXPR", 
   -1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_sym	|
   OPERATOR_PROPERTY_flags},

  {"OPR_ASM_INPUT", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_sym},

  {"OPR_RROTATE", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_LDA_LABEL", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_label	|
   OPERATOR_PROPERTY_1ty},

  {"OPR_GOTO_OUTER_BLOCK", 
   0 /* nkids */,
   OPERATOR_MAPCAT_OSTMT /* mapcat */,
   OPERATOR_PROPERTY_stmt	|
   OPERATOR_PROPERTY_leaf	|
   OPERATOR_PROPERTY_non_scf	|
   OPERATOR_PROPERTY_endsbb	|
   OPERATOR_PROPERTY_next_prev	|
   OPERATOR_PROPERTY_label},

  {"OPR_EXTRACT_BITS", 
   1 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},

  {"OPR_COMPOSE_BITS", 
   2 /* nkids */,
   OPERATOR_MAPCAT_OEXP /* mapcat */,
   OPERATOR_PROPERTY_expression},
};

static BOOL
Is_MTYPE_p [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_V [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_b_f_i_p_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_b_bs_f_i_p_s_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  1, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_f_i_p_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_I4_I8_U4_U8_s [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_V_f_i_p_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_b_f_i_p_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_B_V [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_i [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f_i [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f_i_p_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_I4_I5_I8_f [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_V_b_f_i_p_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  1, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_V_f_i_p_s_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_F4_F8_M_V_i_p [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  1, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_U4_U8 [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_U1_U2_U4_U8 [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  0, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_b_i_p [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_f_i_p_s_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f_i_p [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_I4_I8_U4_U8 [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_F4_F8_V_i_p [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_f_i_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_I4_I8_U4_U8_b [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f_i_p_s_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_V_f_i_p_s_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  1, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_f_i_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  0, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_b [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  0, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  0, /* MTYPE_U4 */
  0, /* MTYPE_U8 */
  0, /* MTYPE_I5 */
  0, /* MTYPE_U5 */
  0, /* MTYPE_A4 */
  0, /* MTYPE_A8 */
  0, /* MTYPE_F4 */
  0, /* MTYPE_F8 */
  0, /* MTYPE_F10 */
  0, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  0, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  0, /* MTYPE_I16 */
  0  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_M_b_f_i_p_s_z [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  1, /* MTYPE_I1 */
  1, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  1, /* MTYPE_U1 */
  1, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  1, /* MTYPE_M */
  1, /* MTYPE_C4 */
  1, /* MTYPE_C8 */
  1, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  1, /* MTYPE_C10 */
  1, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};

static BOOL
Is_MTYPE_b_f_i_p [MTYPE_LAST+1] = {
  0, /* MTYPE_UNKNOWN */
  1, /* MTYPE_B */
  0, /* MTYPE_I1 */
  0, /* MTYPE_I2 */
  1, /* MTYPE_I4 */
  1, /* MTYPE_I8 */
  0, /* MTYPE_U1 */
  0, /* MTYPE_U2 */
  1, /* MTYPE_U4 */
  1, /* MTYPE_U8 */
  1, /* MTYPE_I5 */
  1, /* MTYPE_U5 */
  1, /* MTYPE_A4 */
  1, /* MTYPE_A8 */
  1, /* MTYPE_F4 */
  1, /* MTYPE_F8 */
  1, /* MTYPE_F10 */
  1, /* MTYPE_F16 */
  0, /* MTYPE_STR */
  1, /* MTYPE_FQ */
  0, /* MTYPE_M */
  0, /* MTYPE_C4 */
  0, /* MTYPE_C8 */
  0, /* MTYPE_CQ */
  0, /* MTYPE_V */
  0, /* MTYPE_BS */
  0, /* MTYPE_C10 */
  0, /* MTYPE_C16 */
  1, /* MTYPE_I16 */
  1  /* MTYPE_U16 */
};


void
breakpoint ()
{
}

BOOL
Is_Valid_Opcode_Parts (OPERATOR opr, TYPE_ID rtype, TYPE_ID desc)
{
  BOOL valid;

       if (opr   < OPERATOR_FIRST || opr   > OPERATOR_LAST) valid = FALSE;
  else if (rtype < MTYPE_FIRST    || rtype > MTYPE_LAST)    valid = FALSE;
  else if (desc  < MTYPE_FIRST    || desc  > MTYPE_LAST)    valid = FALSE;

  else  {
    switch (opr) {
      case OPR_LDA_LABEL:
      case OPR_LDMA:
      case OPR_ALLOCA:
      case OPR_LDA:
      case OPR_ILDA:
      case OPR_ARRSECTION:
      case OPR_ARRAY:
        // [RTYPE]: p [DESC]: V 
        valid = Is_MTYPE_p[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_LDID:
        if (   !(rtype == MTYPE_B && desc == MTYPE_B)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I1)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I2)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I5)
          && !(rtype == MTYPE_I4 && desc == MTYPE_BS)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I1)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I2)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I5)
          && !(rtype == MTYPE_I8 && desc == MTYPE_BS)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U1)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U2)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U5)
          && !(rtype == MTYPE_U4 && desc == MTYPE_BS)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U1)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U2)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U5)
          && !(rtype == MTYPE_U8 && desc == MTYPE_BS)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I1)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I2)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I5)
          && !(rtype == MTYPE_I5 && desc == MTYPE_BS)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U1)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U2)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U5)
          && !(rtype == MTYPE_U5 && desc == MTYPE_BS)
          && !(rtype == MTYPE_A4 && desc == MTYPE_A4)
          && !(rtype == MTYPE_A8 && desc == MTYPE_A8)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F10 && desc == MTYPE_F10)
          && !(rtype == MTYPE_F16 && desc == MTYPE_F16)
          && !(rtype == MTYPE_FQ && desc == MTYPE_FQ)
          && !(rtype == MTYPE_M && desc == MTYPE_M)
          && !(rtype == MTYPE_C4 && desc == MTYPE_C4)
          && !(rtype == MTYPE_C8 && desc == MTYPE_C8)
          && !(rtype == MTYPE_CQ && desc == MTYPE_CQ)
          && !(rtype == MTYPE_C10 && desc == MTYPE_C10)
          && !(rtype == MTYPE_C16 && desc == MTYPE_C16)) {

          valid = FALSE;
        }
        else {
          valid = TRUE;
        }
        break;

      case OPR_ILOAD:
        if (   !(rtype == MTYPE_B && desc == MTYPE_B)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I1)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I2)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I5)
          && !(rtype == MTYPE_I4 && desc == MTYPE_BS)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I1)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I2)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I5)
          && !(rtype == MTYPE_I8 && desc == MTYPE_BS)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U1)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U2)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U5)
          && !(rtype == MTYPE_U4 && desc == MTYPE_BS)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U1)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U2)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U5)
          && !(rtype == MTYPE_U8 && desc == MTYPE_BS)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I1)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I2)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I5 && desc == MTYPE_I5)
          && !(rtype == MTYPE_I5 && desc == MTYPE_BS)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U1)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U2)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U5 && desc == MTYPE_U5)
          && !(rtype == MTYPE_U5 && desc == MTYPE_BS)
          && !(rtype == MTYPE_A4 && desc == MTYPE_A4)
          && !(rtype == MTYPE_A8 && desc == MTYPE_A8)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F10 && desc == MTYPE_F10)
          && !(rtype == MTYPE_F16 && desc == MTYPE_F16)
          && !(rtype == MTYPE_FQ && desc == MTYPE_FQ)
          && !(rtype == MTYPE_M && desc == MTYPE_M)
          && !(rtype == MTYPE_C4 && desc == MTYPE_C4)
          && !(rtype == MTYPE_C8 && desc == MTYPE_C8)
          && !(rtype == MTYPE_CQ && desc == MTYPE_CQ)
          && !(rtype == MTYPE_C10 && desc == MTYPE_C10)
          && !(rtype == MTYPE_C16 && desc == MTYPE_C16)) {

          valid = FALSE;
        }
        else {
          valid = TRUE;
        }
        break;

      case OPR_RCOMMA:
      case OPR_COMMA:
        // [RTYPE]: M,f,i,p,z [DESC]: V 
        valid = Is_MTYPE_M_f_i_p_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_STBITS:
      case OPR_ISTBITS:
        // [RTYPE]: V [DESC]: I4,I8,U4,U8,s 
        valid = Is_MTYPE_V[rtype] && Is_MTYPE_I4_I8_U4_U8_s[desc];
        break;

      case OPR_PARM:
        // [RTYPE]: M,V,f,i,p,z [DESC]: V 
        valid = Is_MTYPE_M_V_f_i_p_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_SELECT:
        // [RTYPE]: b,f,i,p,z [DESC]: B,V 
        valid = Is_MTYPE_b_f_i_p_z[rtype] && Is_MTYPE_B_V[desc];
        break;

      case OPR_TRUNC:
      case OPR_RND:
      case OPR_FLOOR:
      case OPR_CEIL:
        // [RTYPE]: i [DESC]: f 
        valid = Is_MTYPE_i[rtype] && Is_MTYPE_f[desc];
        break;

      case OPR_PAIR:
        // [RTYPE]: z [DESC]: V 
        valid = Is_MTYPE_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_MINPART:
      case OPR_MAXPART:
      case OPR_MADD:
        // [RTYPE]: f,i [DESC]: V 
        valid = Is_MTYPE_f_i[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_ASM_EXPR:
      case OPR_SUB:
      case OPR_NEG:
      case OPR_MPY:
      case OPR_CONST:
      case OPR_ADD:
        // [RTYPE]: f,i,p,z [DESC]: V 
        valid = Is_MTYPE_f_i_p_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_STID:
      case OPR_ISTORE:
        // [RTYPE]: V [DESC]: M,b,bs,f,i,p,s,z 
        valid = Is_MTYPE_V[rtype] && Is_MTYPE_M_b_bs_f_i_p_s_z[desc];
        break;

      case OPR_ABS:
        // [RTYPE]: I4,I5,I8,f [DESC]: V 
        valid = Is_MTYPE_I4_I5_I8_f[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_CSELECT:
        // [RTYPE]: M,V,b,f,i,p,z [DESC]: V 
        valid = Is_MTYPE_M_V_b_f_i_p_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_LDBITS:
      case OPR_ILDBITS:
        // [RTYPE]: i [DESC]: I4,I8,U4,U8,s 
        valid = Is_MTYPE_i[rtype] && Is_MTYPE_I4_I8_U4_U8_s[desc];
        break;

      case OPR_SQRT:
      case OPR_RSQRT:
      case OPR_RECIP:
      case OPR_PAREN:
        // [RTYPE]: f,z [DESC]: V 
        valid = Is_MTYPE_f_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_MLOAD:
        // [RTYPE]: M [DESC]: V 
        valid = Is_MTYPE_M[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_INTRINSIC_CALL:
        if (   !(rtype == MTYPE_I1 && desc == MTYPE_V)
          && !(rtype == MTYPE_I2 && desc == MTYPE_V)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I4 && desc == MTYPE_V)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I8 && desc == MTYPE_V)
          && !(rtype == MTYPE_U1 && desc == MTYPE_V)
          && !(rtype == MTYPE_U2 && desc == MTYPE_V)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U4 && desc == MTYPE_V)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U8 && desc == MTYPE_V)
          && !(rtype == MTYPE_I5 && desc == MTYPE_V)
          && !(rtype == MTYPE_U5 && desc == MTYPE_V)
          && !(rtype == MTYPE_A4 && desc == MTYPE_V)
          && !(rtype == MTYPE_A8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F4 && desc == MTYPE_V)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F10 && desc == MTYPE_V)
          && !(rtype == MTYPE_F16 && desc == MTYPE_V)
          && !(rtype == MTYPE_FQ && desc == MTYPE_V)
          && !(rtype == MTYPE_M && desc == MTYPE_V)
          && !(rtype == MTYPE_C4 && desc == MTYPE_V)
          && !(rtype == MTYPE_C8 && desc == MTYPE_V)
          && !(rtype == MTYPE_CQ && desc == MTYPE_V)
          && !(rtype == MTYPE_V && desc == MTYPE_V)
          && !(rtype == MTYPE_C10 && desc == MTYPE_V)
          && !(rtype == MTYPE_C16 && desc == MTYPE_V)) {

          valid = FALSE;
        }
        else {
          valid = TRUE;
        }
        break;

      case OPR_CALL:
        if (   !(rtype == MTYPE_I1 && desc == MTYPE_V)
          && !(rtype == MTYPE_I2 && desc == MTYPE_V)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I4 && desc == MTYPE_V)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I8 && desc == MTYPE_V)
          && !(rtype == MTYPE_U1 && desc == MTYPE_V)
          && !(rtype == MTYPE_U2 && desc == MTYPE_V)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U4 && desc == MTYPE_V)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U8 && desc == MTYPE_V)
          && !(rtype == MTYPE_I5 && desc == MTYPE_V)
          && !(rtype == MTYPE_U5 && desc == MTYPE_V)
          && !(rtype == MTYPE_A4 && desc == MTYPE_V)
          && !(rtype == MTYPE_A8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F4 && desc == MTYPE_V)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F10 && desc == MTYPE_V)
          && !(rtype == MTYPE_F16 && desc == MTYPE_V)
          && !(rtype == MTYPE_FQ && desc == MTYPE_V)
          && !(rtype == MTYPE_M && desc == MTYPE_V)
          && !(rtype == MTYPE_C4 && desc == MTYPE_V)
          && !(rtype == MTYPE_C8 && desc == MTYPE_V)
          && !(rtype == MTYPE_CQ && desc == MTYPE_V)
          && !(rtype == MTYPE_V && desc == MTYPE_V)
          && !(rtype == MTYPE_C10 && desc == MTYPE_V)
          && !(rtype == MTYPE_C16 && desc == MTYPE_V)) {

          valid = FALSE;
        }
        else {
          valid = TRUE;
        }
        break;

      case OPR_RROTATE:
        if (   (rtype == MTYPE_U4 && desc == MTYPE_U8)
          || (rtype == MTYPE_U8 && desc == MTYPE_U1)
          || (rtype == MTYPE_U8 && desc == MTYPE_U2)
          || (rtype == MTYPE_U8 && desc == MTYPE_U4)) {

          valid = FALSE;
        }
        else {
          // [RTYPE]: U4,U8 [DESC]: U1,U2,U4,U8 
          valid = Is_MTYPE_U4_U8[rtype] && Is_MTYPE_U1_U2_U4_U8[desc];
        }
        break;

      case OPR_INTCONST:
        // [RTYPE]: b,i,p [DESC]: V 
        valid = Is_MTYPE_b_i_p[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_RETURN_VAL:
        // [RTYPE]: M,f,i,p,s,z [DESC]: V 
        valid = Is_MTYPE_M_f_i_p_s_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_GOTO_OUTER_BLOCK:
      case OPR_ASM_INPUT:
      case OPR_ASM_STMT:
      case OPR_DEALLOCA:
      case OPR_AFFIRM:
      case OPR_XPRAGMA:
      case OPR_XGOTO:
      case OPR_WHILE_DO:
      case OPR_WHERE:
      case OPR_VFCALL:
      case OPR_TRUEBR:
      case OPR_TRAP:
      case OPR_SWITCH:
      case OPR_RETURN:
      case OPR_REGION_EXIT:
      case OPR_REGION:
      case OPR_PREFETCHX:
      case OPR_PREFETCH:
      case OPR_PRAGMA:
      case OPR_OPT_RESERVE2:
      case OPR_OPT_CHI:
      case OPR_MSTORE:
      case OPR_LOOP_INFO:
      case OPR_LABEL:
      case OPR_IO_ITEM:
      case OPR_IO:
      case OPR_IF:
      case OPR_IDNAME:
      case OPR_GOTO:
      case OPR_FUNC_ENTRY:
      case OPR_FORWARD_BARRIER:
      case OPR_FALSEBR:
      case OPR_EXC_SCOPE_END:
      case OPR_EXC_SCOPE_BEGIN:
      case OPR_EVAL:
      case OPR_DO_WHILE:
      case OPR_DO_LOOP:
      case OPR_COMPGOTO:
      case OPR_COMMENT:
      case OPR_CASEGOTO:
      case OPR_BLOCK:
      case OPR_BACKWARD_BARRIER:
      case OPR_ASSERT:
      case OPR_ALTENTRY:
      case OPR_AGOTO:
        // [RTYPE]: V [DESC]: V 
        valid = Is_MTYPE_V[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_MINMAX:
      case OPR_MIN:
      case OPR_MAX:
        // [RTYPE]: f,i,p [DESC]: V 
        valid = Is_MTYPE_f_i_p[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_COMPOSE_BITS:
      case OPR_EXTRACT_BITS:
        // [RTYPE]: I4,I8,U4,U8 [DESC]: V 
        valid = Is_MTYPE_I4_I8_U4_U8[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_ICALL:
        if (   !(rtype == MTYPE_I1 && desc == MTYPE_V)
          && !(rtype == MTYPE_I2 && desc == MTYPE_V)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I4 && desc == MTYPE_V)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I8 && desc == MTYPE_V)
          && !(rtype == MTYPE_U1 && desc == MTYPE_V)
          && !(rtype == MTYPE_U2 && desc == MTYPE_V)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U4 && desc == MTYPE_V)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U8 && desc == MTYPE_V)
          && !(rtype == MTYPE_I5 && desc == MTYPE_V)
          && !(rtype == MTYPE_U5 && desc == MTYPE_V)
          && !(rtype == MTYPE_A4 && desc == MTYPE_V)
          && !(rtype == MTYPE_A8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F4 && desc == MTYPE_V)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F10 && desc == MTYPE_V)
          && !(rtype == MTYPE_F16 && desc == MTYPE_V)
          && !(rtype == MTYPE_FQ && desc == MTYPE_V)
          && !(rtype == MTYPE_M && desc == MTYPE_V)
          && !(rtype == MTYPE_C4 && desc == MTYPE_V)
          && !(rtype == MTYPE_C8 && desc == MTYPE_V)
          && !(rtype == MTYPE_CQ && desc == MTYPE_V)
          && !(rtype == MTYPE_V && desc == MTYPE_V)
          && !(rtype == MTYPE_C10 && desc == MTYPE_V)
          && !(rtype == MTYPE_C16 && desc == MTYPE_V)) {

          valid = FALSE;
        }
        else {
          valid = TRUE;
        }
        break;

      case OPR_ARRAYEXP:
        // [RTYPE]: M,f,i,z [DESC]: V 
        valid = Is_MTYPE_M_f_i_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_NE:
        if (   (rtype == MTYPE_I4 && desc == MTYPE_B)
          || (rtype == MTYPE_I8 && desc == MTYPE_B)
          || (rtype == MTYPE_U4 && desc == MTYPE_B)
          || (rtype == MTYPE_U8 && desc == MTYPE_B)) {

          valid = FALSE;
        }
        else {
          // [RTYPE]: I4,I8,U4,U8,b [DESC]: b,f,i,p,z 
          valid = Is_MTYPE_I4_I8_U4_U8_b[rtype] && Is_MTYPE_b_f_i_p_z[desc];
        }
        break;

      case OPR_EQ:
        if (   (rtype == MTYPE_I4 && desc == MTYPE_B)
          || (rtype == MTYPE_I8 && desc == MTYPE_B)
          || (rtype == MTYPE_U4 && desc == MTYPE_B)
          || (rtype == MTYPE_U8 && desc == MTYPE_B)) {

          valid = FALSE;
        }
        else {
          // [RTYPE]: I4,I8,U4,U8,b [DESC]: b,f,i,p,z 
          valid = Is_MTYPE_I4_I8_U4_U8_b[rtype] && Is_MTYPE_b_f_i_p_z[desc];
        }
        break;

      case OPR_TAS:
        // [RTYPE]: f,i,p,s,z [DESC]: V 
        valid = Is_MTYPE_f_i_p_s_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_PICCALL:
        if (   !(rtype == MTYPE_I1 && desc == MTYPE_V)
          && !(rtype == MTYPE_I2 && desc == MTYPE_V)
          && !(rtype == MTYPE_I4 && desc == MTYPE_I4)
          && !(rtype == MTYPE_I4 && desc == MTYPE_V)
          && !(rtype == MTYPE_I8 && desc == MTYPE_I8)
          && !(rtype == MTYPE_I8 && desc == MTYPE_V)
          && !(rtype == MTYPE_U1 && desc == MTYPE_V)
          && !(rtype == MTYPE_U2 && desc == MTYPE_V)
          && !(rtype == MTYPE_U4 && desc == MTYPE_U4)
          && !(rtype == MTYPE_U4 && desc == MTYPE_V)
          && !(rtype == MTYPE_U8 && desc == MTYPE_U8)
          && !(rtype == MTYPE_U8 && desc == MTYPE_V)
          && !(rtype == MTYPE_I5 && desc == MTYPE_V)
          && !(rtype == MTYPE_U5 && desc == MTYPE_V)
          && !(rtype == MTYPE_A4 && desc == MTYPE_V)
          && !(rtype == MTYPE_A8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F4 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F4 && desc == MTYPE_V)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F4)
          && !(rtype == MTYPE_F8 && desc == MTYPE_F8)
          && !(rtype == MTYPE_F8 && desc == MTYPE_V)
          && !(rtype == MTYPE_F10 && desc == MTYPE_V)
          && !(rtype == MTYPE_F16 && desc == MTYPE_V)
          && !(rtype == MTYPE_FQ && desc == MTYPE_V)
          && !(rtype == MTYPE_C4 && desc == MTYPE_V)
          && !(rtype == MTYPE_C8 && desc == MTYPE_V)
          && !(rtype == MTYPE_V && desc == MTYPE_V)) {

          valid = FALSE;
        }
        else {
          valid = TRUE;
        }
        break;

      case OPR_OPTPARM:
      case OPR_DIV:
        // [RTYPE]: f,i,z [DESC]: V 
        valid = Is_MTYPE_f_i_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_LNOT:
      case OPR_LIOR:
      case OPR_LAND:
      case OPR_CIOR:
      case OPR_CAND:
        // [RTYPE]: b [DESC]: V 
        valid = Is_MTYPE_b[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_INTRINSIC_OP:
        // [RTYPE]: M,b,f,i,p,s,z [DESC]: V 
        valid = Is_MTYPE_M_b_f_i_p_s_z[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_CVT:
        if (   (rtype == MTYPE_I4 && desc == MTYPE_I4)
          || (rtype == MTYPE_I8 && desc == MTYPE_I8)
          || (rtype == MTYPE_U4 && desc == MTYPE_U4)
          || (rtype == MTYPE_U8 && desc == MTYPE_U8)
          || (rtype == MTYPE_I5 && desc == MTYPE_I5)
          || (rtype == MTYPE_U5 && desc == MTYPE_U5)
          || (rtype == MTYPE_A4 && desc == MTYPE_B)
          || (rtype == MTYPE_A4 && desc == MTYPE_A4)
          || (rtype == MTYPE_A8 && desc == MTYPE_B)
          || (rtype == MTYPE_A8 && desc == MTYPE_A8)
          || (rtype == MTYPE_F4 && desc == MTYPE_B)
          || (rtype == MTYPE_F4 && desc == MTYPE_F4)
          || (rtype == MTYPE_F8 && desc == MTYPE_B)
          || (rtype == MTYPE_F8 && desc == MTYPE_F8)
          || (rtype == MTYPE_F10 && desc == MTYPE_B)
          || (rtype == MTYPE_F10 && desc == MTYPE_F10)
          || (rtype == MTYPE_F16 && desc == MTYPE_B)
          || (rtype == MTYPE_F16 && desc == MTYPE_F16)
          || (rtype == MTYPE_FQ && desc == MTYPE_B)
          || (rtype == MTYPE_FQ && desc == MTYPE_FQ)
          || (rtype == MTYPE_I16 && desc == MTYPE_B)
          || (rtype == MTYPE_U16 && desc == MTYPE_B)) {

          valid = FALSE;
        }
        else {
          // [RTYPE]: f,i,p [DESC]: b,f,i,p 
          valid = Is_MTYPE_f_i_p[rtype] && Is_MTYPE_b_f_i_p[desc];
        }
        break;

      case OPR_FIRSTPART:
      case OPR_NMSUB:
      case OPR_NMADD:
      case OPR_MSUB:
      case OPR_SECONDPART:
      case OPR_ILOADX:
        // [RTYPE]: f [DESC]: V 
        valid = Is_MTYPE_f[rtype] && Is_MTYPE_V[desc];
        break;

      case OPR_ISTOREX:
        // [RTYPE]: V [DESC]: f 
        valid = Is_MTYPE_V[rtype] && Is_MTYPE_f[desc];
        break;

      case OPR_LT:
      case OPR_LE:
      case OPR_GT:
      case OPR_GE:
        // [RTYPE]: I4,I8,U4,U8,b [DESC]: f,i,p 
        valid = Is_MTYPE_I4_I8_U4_U8_b[rtype] && Is_MTYPE_f_i_p[desc];
        break;

      case OPR_XMPY:
      case OPR_TRIPLET:
      case OPR_SHL:
      case OPR_REM:
      case OPR_MOD:
      case OPR_LSHR:
      case OPR_LOWPART:
      case OPR_HIGHPART:
      case OPR_HIGHMPY:
      case OPR_DIVREM:
      case OPR_CVTL:
      case OPR_BXOR:
      case OPR_BNOT:
      case OPR_BNOR:
      case OPR_BIOR:
      case OPR_BAND:
      case OPR_ASHR:
        // [RTYPE]: i [DESC]: V 
        valid = Is_MTYPE_i[rtype] && Is_MTYPE_V[desc];
        break;

      default:
        valid = FALSE;
        break;
    }
  }

  if (valid == FALSE)
    breakpoint ();

  return valid;
}

BOOL
Is_Valid_Opcode_FUNC (OPCODE opc)
{
  OPERATOR opr   = (OPERATOR) (((UINT32) opc) & 0xFF);
  TYPE_ID  rtype = (TYPE_ID)  ((((UINT32) opc) >> 8) & 0x1F);
  TYPE_ID  desc  = (TYPE_ID)  ((((UINT32) opc) >> 13) & 0x1F);

  return Is_Valid_Opcode_Parts (opr, rtype, desc);
}

char*
OPCODE_name (OPERATOR opr, TYPE_ID rtype, TYPE_ID desc)
{
  static char buffer [64];

  Is_True(Is_Valid_Opcode_Parts (opr, rtype, desc),
			 ("Bad opcode %d %d %d", opr, rtype, desc));

  switch (opr) {

      case OPR_LDA_LABEL:
      case OPR_LDMA:
      case OPR_ALLOCA:
      case OPR_LDA:
      case OPR_ILDA:
      case OPR_ARRSECTION:
      case OPR_ARRAY:
        // [RTYPE]: p [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_LDID:
      case OPR_ILOAD:
        // [RTYPE]: M,b,f,i,p,z [DESC]: M,b,bs,f,i,p,s,z 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_RCOMMA:
      case OPR_COMMA:
        // [RTYPE]: M,f,i,p,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_STBITS:
      case OPR_ISTBITS:
        // [RTYPE]: V [DESC]: I4,I8,U4,U8,s 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_PARM:
        // [RTYPE]: M,V,f,i,p,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_SELECT:
        // [RTYPE]: b,f,i,p,z [DESC]: B,V 
        if (desc == MTYPE_V)
          sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        else
          sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_TRUNC:
      case OPR_RND:
      case OPR_FLOOR:
      case OPR_CEIL:
        // [RTYPE]: i [DESC]: f 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_PAIR:
        // [RTYPE]: z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_MINPART:
      case OPR_MAXPART:
      case OPR_MADD:
        // [RTYPE]: f,i [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_ASM_EXPR:
      case OPR_SUB:
      case OPR_NEG:
      case OPR_MPY:
      case OPR_CONST:
      case OPR_ADD:
        // [RTYPE]: f,i,p,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_STID:
      case OPR_ISTORE:
        // [RTYPE]: V [DESC]: M,b,bs,f,i,p,s,z 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_ABS:
        // [RTYPE]: I4,I5,I8,f [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_CSELECT:
        // [RTYPE]: M,V,b,f,i,p,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_LDBITS:
      case OPR_ILDBITS:
        // [RTYPE]: i [DESC]: I4,I8,U4,U8,s 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_SQRT:
      case OPR_RSQRT:
      case OPR_RECIP:
      case OPR_PAREN:
        // [RTYPE]: f,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

        // [RTYPE]: M [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_INTRINSIC_CALL:
      case OPR_CALL:
        // [RTYPE]: M,V,f,i,p,s,z [DESC]: F4,F8,M,V,i,p 
        if (desc == MTYPE_V)
          sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        else
          sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_RROTATE:
        // [RTYPE]: U4,U8 [DESC]: U1,U2,U4,U8 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_INTCONST:
        // [RTYPE]: b,i,p [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_RETURN_VAL:
        // [RTYPE]: M,f,i,p,s,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_GOTO_OUTER_BLOCK:
      case OPR_ASM_INPUT:
      case OPR_ASM_STMT:
      case OPR_DEALLOCA:
      case OPR_AFFIRM:
      case OPR_XPRAGMA:
      case OPR_XGOTO:
      case OPR_WHILE_DO:
      case OPR_WHERE:
      case OPR_VFCALL:
      case OPR_TRUEBR:
      case OPR_TRAP:
      case OPR_SWITCH:
      case OPR_RETURN:
      case OPR_REGION_EXIT:
      case OPR_REGION:
      case OPR_PREFETCHX:
      case OPR_PREFETCH:
      case OPR_PRAGMA:
      case OPR_OPT_RESERVE2:
      case OPR_OPT_CHI:
      case OPR_MSTORE:
      case OPR_LOOP_INFO:
      case OPR_LABEL:
      case OPR_IO_ITEM:
      case OPR_IO:
      case OPR_IF:
      case OPR_IDNAME:
      case OPR_GOTO:
      case OPR_FUNC_ENTRY:
      case OPR_FORWARD_BARRIER:
      case OPR_FALSEBR:
      case OPR_EXC_SCOPE_END:
      case OPR_EXC_SCOPE_BEGIN:
      case OPR_EVAL:
      case OPR_DO_WHILE:
      case OPR_DO_LOOP:
      case OPR_COMPGOTO:
      case OPR_COMMENT:
      case OPR_CASEGOTO:
      case OPR_BLOCK:
      case OPR_BACKWARD_BARRIER:
      case OPR_ASSERT:
      case OPR_ALTENTRY:
      case OPR_AGOTO:
        // [RTYPE]: V [DESC]: V 
        sprintf (buffer, "OPC_%s", &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_MINMAX:
      case OPR_MIN:
      case OPR_MAX:
        // [RTYPE]: f,i,p [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_COMPOSE_BITS:
      case OPR_EXTRACT_BITS:
        // [RTYPE]: I4,I8,U4,U8 [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_ICALL:
        // [RTYPE]: M,V,f,i,p,s,z [DESC]: F4,F8,V,i,p 
        if (desc == MTYPE_V)
          sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        else
          sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_ARRAYEXP:
        // [RTYPE]: M,f,i,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_NE:
      case OPR_EQ:
        // [RTYPE]: I4,I8,U4,U8,b [DESC]: b,f,i,p,z 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_TAS:
        // [RTYPE]: f,i,p,s,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_PICCALL:
        // [RTYPE]: V,f,i,p,s,z [DESC]: F4,F8,V,i,p 
        if (desc == MTYPE_V)
          sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        else
          sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_OPTPARM:
      case OPR_DIV:
        // [RTYPE]: f,i,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_LNOT:
      case OPR_LIOR:
      case OPR_LAND:
      case OPR_CIOR:
      case OPR_CAND:
        // [RTYPE]: b [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_INTRINSIC_OP:
        // [RTYPE]: M,b,f,i,p,s,z [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_CVT:
        // [RTYPE]: f,i,p [DESC]: b,f,i,p 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_FIRSTPART:
      case OPR_NMSUB:
      case OPR_NMADD:
      case OPR_MSUB:
      case OPR_SECONDPART:
      case OPR_ILOADX:
        // [RTYPE]: f [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_ISTOREX:
        // [RTYPE]: V [DESC]: f 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_LT:
      case OPR_LE:
      case OPR_GT:
      case OPR_GE:
        // [RTYPE]: I4,I8,U4,U8,b [DESC]: f,i,p 
        sprintf (buffer, "OPC_%s%s%s", MTYPE_name(rtype), MTYPE_name(desc), &OPERATOR_info [opr]._name [4]);
        break;

      case OPR_XMPY:
      case OPR_TRIPLET:
      case OPR_SHL:
      case OPR_REM:
      case OPR_MOD:
      case OPR_LSHR:
      case OPR_LOWPART:
      case OPR_HIGHPART:
      case OPR_HIGHMPY:
      case OPR_DIVREM:
      case OPR_CVTL:
      case OPR_BXOR:
      case OPR_BNOT:
      case OPR_BNOR:
      case OPR_BIOR:
      case OPR_BAND:
      case OPR_ASHR:
        // [RTYPE]: i [DESC]: V 
        sprintf (buffer, "OPC_%s%s", MTYPE_name(rtype), &OPERATOR_info [opr]._name [4]);
        break;

    case OPR_MLOAD: 
      // [RTYPE] : M [DESC] : V 
      sprintf (buffer, "OPC_%s", &OPERATOR_info [opr]._name [4]);
      break;

    default:
      buffer [0] = 0;
      break;
  }

  return buffer;
}

char*
OPCODE_name (OPCODE opc)
{
  OPERATOR opr   = (OPERATOR) (((UINT32) opc) & 0xFF);
  TYPE_ID  rtype = (TYPE_ID)  ((((UINT32) opc) >> 8) & 0x1F);
  TYPE_ID  desc  = (TYPE_ID)  ((((UINT32) opc) >> 13) & 0x1F);

  return OPCODE_name (opr, rtype, desc);
}

