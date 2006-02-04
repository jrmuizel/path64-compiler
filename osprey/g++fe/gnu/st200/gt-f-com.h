/* Type information for f/com.c.
   Copyright (C) 2002 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

/* This file is machine generated.  Do not edit.  */

void
gt_ggc_mx_tree_ggc_tracker (x_p)
      void *x_p;
{
  struct tree_ggc_tracker * const x = (struct tree_ggc_tracker *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_16tree_ggc_tracker ((*x).next);
      {
        size_t i1_0;
        const size_t ilimit1_0 = (NUM_TRACKED_CHUNK);
        for (i1_0 = 0; i1_0 < ilimit1_0; i1_0++) {
          gt_ggc_m_9tree_node ((*x).trees[i1_0]);
        }
      }
  }
}

void
gt_ggc_mx_f_binding_level (x_p)
      void *x_p;
{
  struct f_binding_level * const x = (struct f_binding_level *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_9tree_node ((*x).names);
      gt_ggc_m_9tree_node ((*x).blocks);
      gt_ggc_m_9tree_node ((*x).this_block);
      gt_ggc_m_15f_binding_level ((*x).level_chain);
  }
}

void
gt_ggc_mx_lang_tree_node (x_p)
      void *x_p;
{
  union lang_tree_node * x = (union lang_tree_node *)x_p;
  union lang_tree_node * xlimit = x;
  while (ggc_test_and_set_mark (xlimit))
   xlimit = ((union lang_tree_node *)TREE_CHAIN (&(*xlimit).generic));
  while (x != xlimit)
    {
      switch (TREE_CODE (&((*x)).generic) == IDENTIFIER_NODE)
        {
        case 0:
          switch (tree_node_structure (&((*x).generic)))
            {
            case TS_COMMON:
              gt_ggc_m_9tree_node ((*x).generic.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.common.type);
              break;
            case TS_INT_CST:
              gt_ggc_m_9tree_node ((*x).generic.int_cst.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.int_cst.common.type);
              gt_ggc_m_7rtx_def ((*x).generic.int_cst.rtl);
              break;
            case TS_REAL_CST:
              gt_ggc_m_9tree_node ((*x).generic.real_cst.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.real_cst.common.type);
              gt_ggc_m_7rtx_def ((*x).generic.real_cst.rtl);
              gt_ggc_m_10real_value ((*x).generic.real_cst.real_cst_ptr);
              break;
            case TS_VECTOR:
              gt_ggc_m_9tree_node ((*x).generic.vector.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.vector.common.type);
              gt_ggc_m_7rtx_def ((*x).generic.vector.rtl);
              gt_ggc_m_9tree_node ((*x).generic.vector.elements);
              break;
            case TS_STRING:
              gt_ggc_m_9tree_node ((*x).generic.string.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.string.common.type);
              gt_ggc_m_7rtx_def ((*x).generic.string.rtl);
              break;
            case TS_COMPLEX:
              gt_ggc_m_9tree_node ((*x).generic.complex.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.complex.common.type);
              gt_ggc_m_7rtx_def ((*x).generic.complex.rtl);
              gt_ggc_m_9tree_node ((*x).generic.complex.real);
              gt_ggc_m_9tree_node ((*x).generic.complex.imag);
              break;
            case TS_IDENTIFIER:
              gt_ggc_m_9tree_node ((*x).generic.identifier.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.identifier.common.type);
              break;
            case TS_DECL:
              gt_ggc_m_9tree_node ((*x).generic.decl.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.decl.common.type);
              gt_ggc_m_9tree_node ((*x).generic.decl.size);
              gt_ggc_m_9tree_node ((*x).generic.decl.size_unit);
              gt_ggc_m_9tree_node ((*x).generic.decl.name);
              gt_ggc_m_9tree_node ((*x).generic.decl.context);
              gt_ggc_m_9tree_node ((*x).generic.decl.arguments);
              gt_ggc_m_9tree_node ((*x).generic.decl.result);
              gt_ggc_m_9tree_node ((*x).generic.decl.initial);
              gt_ggc_m_9tree_node ((*x).generic.decl.abstract_origin);
              gt_ggc_m_9tree_node ((*x).generic.decl.assembler_name);
              gt_ggc_m_9tree_node ((*x).generic.decl.section_name);
              gt_ggc_m_9tree_node ((*x).generic.decl.attributes);
              gt_ggc_m_7rtx_def ((*x).generic.decl.rtl);
              gt_ggc_m_7rtx_def ((*x).generic.decl.live_range_rtl);
              switch (TREE_CODE((tree) &((*x))))
                {
                case FUNCTION_DECL:
                  gt_ggc_m_8function ((*x).generic.decl.u2.f);
                  break;
                case PARM_DECL:
                  gt_ggc_m_7rtx_def ((*x).generic.decl.u2.r);
                  break;
                case FIELD_DECL:
                  gt_ggc_m_9tree_node ((*x).generic.decl.u2.t);
                  break;
                default:
                  break;
                }
              gt_ggc_m_9tree_node ((*x).generic.decl.saved_tree);
              gt_ggc_m_9tree_node ((*x).generic.decl.inlined_fns);
              gt_ggc_m_9tree_node ((*x).generic.decl.vindex);
              gt_ggc_m_9lang_decl ((*x).generic.decl.lang_specific);
              break;
            case TS_TYPE:
              gt_ggc_m_9tree_node ((*x).generic.type.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.type.common.type);
              gt_ggc_m_9tree_node ((*x).generic.type.values);
              gt_ggc_m_9tree_node ((*x).generic.type.size);
              gt_ggc_m_9tree_node ((*x).generic.type.size_unit);
              gt_ggc_m_9tree_node ((*x).generic.type.attributes);
              gt_ggc_m_9tree_node ((*x).generic.type.pointer_to);
              gt_ggc_m_9tree_node ((*x).generic.type.reference_to);
              switch (debug_hooks == &sdb_debug_hooks ? 1 : debug_hooks == &dwarf2_debug_hooks ? 2 : 0)
                {
                case 1:
                  break;
                default:
                  break;
                }
              gt_ggc_m_9tree_node ((*x).generic.type.name);
              gt_ggc_m_9tree_node ((*x).generic.type.minval);
              gt_ggc_m_9tree_node ((*x).generic.type.maxval);
              gt_ggc_m_9tree_node ((*x).generic.type.next_variant);
              gt_ggc_m_9tree_node ((*x).generic.type.main_variant);
              gt_ggc_m_9tree_node ((*x).generic.type.binfo);
              gt_ggc_m_9tree_node ((*x).generic.type.context);
              gt_ggc_m_9lang_type ((*x).generic.type.lang_specific);
              break;
            case TS_LIST:
              gt_ggc_m_9tree_node ((*x).generic.list.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.list.common.type);
              gt_ggc_m_9tree_node ((*x).generic.list.purpose);
              gt_ggc_m_9tree_node ((*x).generic.list.value);
              break;
            case TS_VEC:
              gt_ggc_m_9tree_node ((*x).generic.vec.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.vec.common.type);
              {
                size_t i1_0;
                const size_t ilimit1_0 = (TREE_VEC_LENGTH ((tree)&((*x).generic.vec)));
                for (i1_0 = 0; i1_0 < ilimit1_0; i1_0++) {
                  gt_ggc_m_9tree_node ((*x).generic.vec.a[i1_0]);
                }
              }
              break;
            case TS_EXP:
              gt_ggc_m_9tree_node ((*x).generic.exp.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.exp.common.type);
              switch (TREE_CODE ((tree) &(*x)))
                {
                case METHOD_CALL_EXPR:
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[3]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[2]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[1]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[0]);
                  break;
                case WITH_CLEANUP_EXPR:
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[2]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[1]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[0]);
                  break;
                case RTL_EXPR:
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[1]);
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[0]);
                  break;
                case GOTO_SUBROUTINE_EXPR:
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[1]);
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[0]);
                  break;
                case SAVE_EXPR:
                  gt_ggc_m_7rtx_def ((*x).generic.exp.operands[2]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[1]);
                  gt_ggc_m_9tree_node ((*x).generic.exp.operands[0]);
                  break;
                default:
                  {
                    size_t i2_0;
                    const size_t ilimit2_0 = (TREE_CODE_LENGTH (TREE_CODE ((tree) &(*x))));
                    for (i2_0 = 0; i2_0 < ilimit2_0; i2_0++) {
                      gt_ggc_m_9tree_node ((*x).generic.exp.operands[i2_0]);
                    }
                  }
                  break;
                }
              break;
            case TS_BLOCK:
              gt_ggc_m_9tree_node ((*x).generic.block.common.chain);
              gt_ggc_m_9tree_node ((*x).generic.block.common.type);
              gt_ggc_m_9tree_node ((*x).generic.block.vars);
              gt_ggc_m_9tree_node ((*x).generic.block.subblocks);
              gt_ggc_m_9tree_node ((*x).generic.block.supercontext);
              gt_ggc_m_9tree_node ((*x).generic.block.abstract_origin);
              gt_ggc_m_9tree_node ((*x).generic.block.fragment_origin);
              gt_ggc_m_9tree_node ((*x).generic.block.fragment_chain);
              break;
            default:
              break;
            }
          break;
        case 1:
          gt_ggc_m_9tree_node ((*x).identifier.common.common.chain);
          gt_ggc_m_9tree_node ((*x).identifier.common.common.type);
          gt_ggc_m_9tree_node ((*x).identifier.global_value);
          gt_ggc_m_9tree_node ((*x).identifier.local_value);
          gt_ggc_m_9tree_node ((*x).identifier.label_value);
          break;
        default:
          break;
        }
      x = ((union lang_tree_node *)TREE_CHAIN (&(*x).generic));
  }
}

void
gt_ggc_mx_lang_decl (x_p)
      void *x_p;
{
  struct lang_decl * const x = (struct lang_decl *)x_p;
  if (ggc_test_and_set_mark (x))
    {
  }
}

void
gt_ggc_mx_lang_type (x_p)
      void *x_p;
{
  struct lang_type * const x = (struct lang_type *)x_p;
  if (ggc_test_and_set_mark (x))
    {
  }
}

void
gt_ggc_mx_language_function (x_p)
      void *x_p;
{
  struct language_function * const x = (struct language_function *)x_p;
  if (ggc_test_and_set_mark (x))
    {
  }
}

/* GC roots.  */

const struct ggc_root_tab gt_ggc_r_gt_f_com_h[] = {
  {
    &ffecom_type_vardesc_var,
    1,
    sizeof (ffecom_type_vardesc_var),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_type_namelist_var,
    1,
    sizeof (ffecom_type_namelist_var),
    &gt_ggc_mx_tree_node

  },
  {
    &tracker_head,
    1,
    sizeof (tracker_head),
    &gt_ggc_mx_tree_ggc_tracker

  },
  {
    &shadowed_labels,
    1,
    sizeof (shadowed_labels),
    &gt_ggc_mx_tree_node

  },
  {
    &named_labels,
    1,
    sizeof (named_labels),
    &gt_ggc_mx_tree_node

  },
  {
    &current_binding_level,
    1,
    sizeof (current_binding_level),
    &gt_ggc_mx_f_binding_level

  },
  {
    &ffecom_gfrt_[0],
    1 * (FFECOM_gfrt),
    sizeof (ffecom_gfrt_[0]),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_multi_fields_[0][0],
    1 * (FFEINFO_basictype) * (FFEINFO_kindtype),
    sizeof (ffecom_multi_fields_[0][0]),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_multi_retval_,
    1,
    sizeof (ffecom_multi_retval_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_multi_type_node_,
    1,
    sizeof (ffecom_multi_type_node_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_func_length_,
    1,
    sizeof (ffecom_func_length_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_func_result_,
    1,
    sizeof (ffecom_func_result_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_double_half_,
    1,
    sizeof (ffecom_double_half_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_double_zero_,
    1,
    sizeof (ffecom_double_zero_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_float_half_,
    1,
    sizeof (ffecom_float_half_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_float_zero_,
    1,
    sizeof (ffecom_float_zero_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_which_entrypoint_decl_,
    1,
    sizeof (ffecom_which_entrypoint_decl_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_previous_function_decl_,
    1,
    sizeof (ffecom_previous_function_decl_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_outer_function_decl_,
    1,
    sizeof (ffecom_outer_function_decl_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_f2c_ptr_to_real_type_node,
    1,
    sizeof (ffecom_f2c_ptr_to_real_type_node),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_f2c_ptr_to_integer_type_node,
    1,
    sizeof (ffecom_f2c_ptr_to_integer_type_node),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_xargc_,
    1,
    sizeof (ffecom_tree_xargc_),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_blockdata_type,
    1,
    sizeof (ffecom_tree_blockdata_type),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_ptr_to_subr_type,
    1,
    sizeof (ffecom_tree_ptr_to_subr_type),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_subr_type,
    1,
    sizeof (ffecom_tree_subr_type),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_ptr_to_fun_type[0][0],
    1 * (FFEINFO_basictype) * (FFEINFO_kindtype),
    sizeof (ffecom_tree_ptr_to_fun_type[0][0]),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_fun_type[0][0],
    1 * (FFEINFO_basictype) * (FFEINFO_kindtype),
    sizeof (ffecom_tree_fun_type[0][0]),
    &gt_ggc_mx_tree_node

  },
  {
    &ffecom_tree_fun_type_void,
    1,
    sizeof (ffecom_tree_fun_type_void),
    &gt_ggc_mx_tree_node

  },
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_rd_gt_f_com_h[] = {
  { &free_binding_level, 1, sizeof (free_binding_level), NULL },
  LAST_GGC_ROOT_TAB
};

