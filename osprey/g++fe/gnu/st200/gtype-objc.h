/* Type information for objc.
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
gt_ggc_mx_imp_entry (x_p)
      void *x_p;
{
  struct imp_entry * const x = (struct imp_entry *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_9imp_entry ((*x).next);
      gt_ggc_m_9tree_node ((*x).imp_context);
      gt_ggc_m_9tree_node ((*x).imp_template);
      gt_ggc_m_9tree_node ((*x).class_decl);
      gt_ggc_m_9tree_node ((*x).meta_decl);
  }
}

void
gt_ggc_mx_hashed_attribute (x_p)
      void *x_p;
{
  struct hashed_attribute * const x = (struct hashed_attribute *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_16hashed_attribute ((*x).next);
      gt_ggc_m_9tree_node ((*x).value);
  }
}

void
gt_ggc_mx_hashed_entry (x_p)
      void *x_p;
{
  struct hashed_entry * const x = (struct hashed_entry *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_16hashed_attribute ((*x).list);
      gt_ggc_m_12hashed_entry ((*x).next);
      gt_ggc_m_9tree_node ((*x).key);
  }
}

/* GC roots.  */

static void gt_ggc_ma_cls_method_hash_list PARAMS ((void *));
static void
gt_ggc_ma_cls_method_hash_list (x_p)
      void *x_p;
{
  size_t i;
  struct hashed_entry ** const x = (struct hashed_entry **)x_p;
  if (ggc_test_and_set_mark (x))
    for (i = 0; i < (SIZEHASHTABLE); i++)
      gt_ggc_m_12hashed_entry (x[i]);
}

static void gt_ggc_ma_nst_method_hash_list PARAMS ((void *));
static void
gt_ggc_ma_nst_method_hash_list (x_p)
      void *x_p;
{
  size_t i;
  struct hashed_entry ** const x = (struct hashed_entry **)x_p;
  if (ggc_test_and_set_mark (x))
    for (i = 0; i < (SIZEHASHTABLE); i++)
      gt_ggc_m_12hashed_entry (x[i]);
}

const struct ggc_root_tab gt_ggc_r_gtype_objc_h[] = {
  {
    &objc_global_trees[0],
    1 * (OCTI_MAX),
    sizeof (objc_global_trees[0]),
    &gt_ggc_mx_tree_node

  },
  {
    &imp_list,
    1,
    sizeof (imp_list),
    &gt_ggc_mx_imp_entry

  },
  {
    &cls_method_hash_list,
    1,
    sizeof (cls_method_hash_list),
    &gt_ggc_ma_cls_method_hash_list
  },
  {
    &nst_method_hash_list,
    1,
    sizeof (nst_method_hash_list),
    &gt_ggc_ma_nst_method_hash_list
  },
  LAST_GGC_ROOT_TAB
};

extern const struct ggc_root_tab gt_ggc_r_gtype_desc_c[];
extern const struct ggc_root_tab gt_ggc_r_gt_alias_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_cselib_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_dwarf2out_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_emit_rtl_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_except_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_explow_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_expr_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_fold_const_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_function_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_gcse_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_integrate_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_optabs_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_profile_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_ra_build_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_regclass_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_reg_stack_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_sdbout_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_stor_layout_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_varasm_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_c_common_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_c_pragma_h[];
extern const struct ggc_root_tab gt_ggc_r_gtype_objc_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_c_parse_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_c_decl_h[];
extern const struct ggc_root_tab gt_ggc_r_gt_c_objc_common_h[];
const struct ggc_root_tab * const gt_ggc_rtab[] = {
  gt_ggc_r_gtype_desc_c,
  gt_ggc_r_gt_alias_h,
  gt_ggc_r_gt_cselib_h,
  gt_ggc_r_gt_dwarf2out_h,
  gt_ggc_r_gt_emit_rtl_h,
  gt_ggc_r_gt_except_h,
  gt_ggc_r_gt_explow_h,
  gt_ggc_r_gt_expr_h,
  gt_ggc_r_gt_fold_const_h,
  gt_ggc_r_gt_function_h,
  gt_ggc_r_gt_gcse_h,
  gt_ggc_r_gt_integrate_h,
  gt_ggc_r_gt_optabs_h,
  gt_ggc_r_gt_profile_h,
  gt_ggc_r_gt_ra_build_h,
  gt_ggc_r_gt_regclass_h,
  gt_ggc_r_gt_reg_stack_h,
  gt_ggc_r_gt_sdbout_h,
  gt_ggc_r_gt_stor_layout_h,
  gt_ggc_r_gt_varasm_h,
  gt_ggc_r_gt_c_common_h,
  gt_ggc_r_gt_c_pragma_h,
  gt_ggc_r_gtype_objc_h,
  gt_ggc_r_gt_c_parse_h,
  gt_ggc_r_gt_c_decl_h,
  gt_ggc_r_gt_c_objc_common_h,
  NULL
};
extern const struct ggc_root_tab gt_ggc_rd_gt_bitmap_h[];
extern const struct ggc_root_tab gt_ggc_rd_gt_cselib_h[];
extern const struct ggc_root_tab gt_ggc_rd_gt_emit_rtl_h[];
extern const struct ggc_root_tab gt_ggc_rd_gt_lists_h[];
extern const struct ggc_root_tab gt_ggc_rd_gt_c_decl_h[];
const struct ggc_root_tab * const gt_ggc_deletable_rtab[] = {
  gt_ggc_rd_gt_bitmap_h,
  gt_ggc_rd_gt_cselib_h,
  gt_ggc_rd_gt_emit_rtl_h,
  gt_ggc_rd_gt_lists_h,
  gt_ggc_rd_gt_c_decl_h,
  NULL
};
extern const struct ggc_cache_tab gt_ggc_rc_gt_emit_rtl_h[];
extern const struct ggc_cache_tab gt_ggc_rc_gt_fold_const_h[];
extern const struct ggc_cache_tab gt_ggc_rc_gt_tree_h[];
const struct ggc_cache_tab * const gt_ggc_cache_rtab[] = {
  gt_ggc_rc_gt_emit_rtl_h,
  gt_ggc_rc_gt_fold_const_h,
  gt_ggc_rc_gt_tree_h,
  NULL
};
