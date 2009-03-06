/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

#include "defs.h"
#include "erglob.h"
#include "erbe.h"
#include "glob.h"
#include "tracing.h"
#include "config_target.h"
#include "config.h"
#include "config_TARG.h"

#include "symtab.h"
#include "strtab.h"
#include "be_symtab.h"
#include "targ_isa_lits.h"
#include "topcode.h"
#include "cgir.h"
#include "tn_set.h"
#include "tn_map.h"

#include "ttype.h"
#include "const.h"
#include "data_layout.h"
#include "targ_sim.h"
#include "whirl2ops.h"
#include "entry_exit_targ.h"
#include "targ_abi_properties.h"

#include "cg_flags.h"
#include "cg.h"
#include "calls.h"
#include "register.h"
#include "cgexp.h"
#include "cg_spill.h"
#include "cgtarget.h"
#include "mexpand.h"
#include "reg_live.h"
#include "bb.h"
#include "config_TARG.h"


static std::vector<CLASS_REG_PAIR> callee_saved_vector;
static std::vector<CLASS_REG_PAIR> not_pushed_callee_saved_vector;
static std::vector<CLASS_REG_PAIR> caller_saved_vector;
static std::vector<CLASS_REG_PAIR> not_pushed_caller_saved_vector;
static INT push_pop_mask;
ST *first_st_save, *last_st_save;
INT64 push_area_size, upper_size, lower_size, frame_size;
BOOL has_fp_adjust;

// [JV] Flag to mark if GP is restored
BOOL GP_is_restored;

/* ====================================================================
 *   EETARG_Build_Jump_Instead_Of_Call (call_op)
 * ====================================================================
 */
OP *
EETARG_Build_Jump_Instead_Of_Call (
  OP *call_op
)
{
  OP *jump_op = NULL;
  TOP jump_top;
  TOP call_top = OP_code(call_op);

  switch (call_top) {
  case TOP_blx_npc_cond:
    jump_op = Mk_OP(TOP_bx_cond, OP_result(call_op, 0), OP_result(call_op, 2), OP_opnd(call_op, 0), OP_opnd(call_op, 1), OP_opnd(call_op, 2));
    break;
  case TOP_blx_npc:
    jump_op = Mk_OP(TOP_bx, OP_result(call_op, 0), OP_result(call_op, 2), OP_opnd(call_op, 0));
    break;
  case TOP_bl_cond:
    jump_op = Mk_OP(TOP_b_cond, OP_result(call_op, 1), OP_result(call_op, 2), OP_opnd(call_op, 0), OP_opnd(call_op, 1), OP_opnd(call_op, 2), OP_opnd(call_op, 3));
    break;
  case TOP_bl:
    jump_op = Mk_OP(TOP_b, OP_result(call_op, 1), OP_opnd(call_op, 0), OP_opnd(call_op, 1));
    break;
  default:
    FmtAssert(FALSE, ("don't know how to generate tail call for %s",
		     TOP_Name(call_top)));
    /*NOTREACHED*/
  }

  return jump_op;
}


/* ====================================================================
 * EETARG_restore_gp_from_symbol
 * Restore GP TN from global symbol __GP_BASE
 * ====================================================================
 */
static void EETARG_restore_gp_from_symbol( BB *bb ) {

  FmtAssert(FALSE,("Not implemented"));
#if 0
  FmtAssert(BB_entry(bb),("BB not entry block"));

  // [JV] Restore only once GP.
  if(GP_is_restored) return;

  OPS ops_gp = OPS_EMPTY;
  OP *point = NULL;
  TN *tmp_tn, *tmp_tn1, *tmp_tn2;
  ST *st = New_ST (GLOBAL_SYMTAB);
  TY_IDX ty = MTYPE_To_TY(Pointer_Mtype);
  ST_Init (st, Save_Str("__GP_BASE"), CLASS_VAR
	   , SCLASS_EXTERN, EXPORT_PREEMPTIBLE, ty);
  tmp_tn = Gen_Symbol_TN(st, 0, 0);
  tmp_tn1 = Dup_TN(tmp_tn);
  tmp_tn2 = Dup_TN(tmp_tn);
  Set_TN_is_reloc_abs16to31(tmp_tn1);
  Set_TN_is_reloc_abs0to15(tmp_tn2);      
  Build_OP(TOP_make, GP_TN, True_TN, tmp_tn1, &ops_gp);
  Build_OP(TOP_more, GP_TN, True_TN, GP_TN, tmp_tn2, &ops_gp);
  point = BB_first_op(bb);
  if (point == NULL) {
    BB_Append_Ops (bb, &ops_gp);
  }
  else {
    if(BB_entry_sp_adj_op(bb)) {
      // [JV] GP must not be restored before spadjust.
      point = OP_next(BB_entry_sp_adj_op(bb));
    }
    BB_Insert_Ops_Before(bb, point, &ops_gp);
  }

  // [JV] Mark GP restored.
  GP_is_restored = TRUE;
#endif
}

static int
init_callee_saved(void)
{
  callee_saved_vector.clear();
  not_pushed_callee_saved_vector.clear();
  INT size = 0;
  INT callee_num;

  for (callee_num = 0; callee_num < Callee_Saved_Regs_Count; callee_num++) {
    TN *tn = CALLEE_tn(callee_num);
    ISA_REGISTER_CLASS cl = TN_save_rclass(tn);
    REGISTER reg = TN_save_reg(tn);
    if (EETARG_Save_With_Regmask (cl, reg) &&
	REGISTER_SET_MemberP(Callee_Saved_Regs_Mask[cl], reg)) {
      callee_saved_vector.push_back(TN_save_creg(tn));
      size += 4;
    }
  }
  if (RA_TN != NULL &&
      EETARG_Save_With_Regmask (REGISTER_CLASS_ra, REGISTER_ra) &&
      REGISTER_SET_MemberP(Callee_Saved_Regs_Mask[REGISTER_CLASS_ra], 
			   REGISTER_ra)) {
    callee_saved_vector.push_back(TN_class_reg(RA_TN));
    size += 4;

  }

  return size;
}

static int
init_caller_saved(void)
{
  INT size = 0;
  caller_saved_vector.clear();

#if 0

  if (PU_is_interrupt(Get_Current_PU()) ||
      PU_is_interrupt_nostkaln(Get_Current_PU())) {
    ISA_REGISTER_CLASS cl;
    REGISTER_SET scratchs_used[ISA_REGISTER_CLASS_MAX+1];
    REGISTER_SET avails_scratchs[ISA_REGISTER_CLASS_MAX+1];
    
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      if ((cl == ISA_REGISTER_CLASS_gpr) ||
	  (cl == ISA_REGISTER_CLASS_gr) ||
	  (cl == ISA_REGISTER_CLASS_lr) ||
	  (cl == ISA_REGISTER_CLASS_fpr)) {
	scratchs_used[cl] = REGISTER_CLASS_caller_saves(cl);
	REGISTER member;
	// [CR] if calls, all scratchs have to be saved
	// [CR] else, just used scratchs have to be saved
	if (!PU_Has_Calls) {
	  REG_LIVE_Analyze_Region();
    
	  BB * bb;
	  scratchs_used[cl] = REGISTER_SET_EMPTY_SET;
	  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
	    REG_LIVE_Prolog_Temps(bb, BB_first_op(bb), BB_last_op(bb), avails_scratchs);
	    scratchs_used[cl] =
	      REGISTER_SET_Union(scratchs_used[cl],
				 REGISTER_SET_Difference(REGISTER_CLASS_caller_saves(cl),
							 avails_scratchs[cl]));
	  }
	  REG_LIVE_Finish();
	}
      
	for (member = REGISTER_SET_Choose(scratchs_used[cl]);
	     member != REGISTER_UNDEFINED;
	     member = REGISTER_SET_Choose_Next(scratchs_used[cl],member)) {
	  if (cl == ISA_REGISTER_CLASS_lr) {
          if(Core_Has_HWLoop) {
              // [CR} loop register L1
              int i ;
              for (i = 0 ; i < 4 ; i++) {
                  CLASS_REG_PAIR pair;
                  pair.class_reg.reg = i+REGISTER_MIN+HWloopRegisterLR0SFRRank+4;
                  pair.class_reg.rclass = ISA_REGISTER_CLASS_sfr;
                  caller_saved_vector.push_back(pair);
                  size += 4;
              }
          }
	  }
	  else {
	    CLASS_REG_PAIR pair;
	    pair.class_reg.reg = member;
	    pair.class_reg.rclass = cl;
	    caller_saved_vector.push_back(pair);
	    size += 4;
	  }
	}
      }
    }  
  }

#endif
  return size;
}

void
EETARG_Fixup_Stack_Frame (void)
{
  INT i;
  INT size = 0;

  GP_is_restored = FALSE;

  if((!CG_gen_callee_saved_regs_mask) &&
     !(PU_is_interrupt(Get_Current_PU()) ||
       PU_is_interrupt_nostkaln(Get_Current_PU())))
    return;

  if (CG_gen_callee_saved_regs_mask)
    size = init_callee_saved();
  size += init_caller_saved();

  // If push_area_size isn't  empty, allocate REGSPACE
  if (size > 0) {
    Allocate_Reg_Save_Area(size);
  }

  push_pop_mask = 0;

#if 0
  fprintf(TFile,"Callee save registers for pu %s:\n", Cur_PU_Name);
#endif

  for (i = 0; i < callee_saved_vector.size(); i++) {
    CLASS_REG_PAIR reg_pair = callee_saved_vector[i];
    ISA_REGISTER_CLASS rclass = CLASS_REG_PAIR_rclass(reg_pair);
    REGISTER           reg =    CLASS_REG_PAIR_reg(reg_pair);

#if 0
    REGISTER_Print(rclass,reg,TFile);
    fprintf(TFile,"\n");
#endif

    switch(rclass) {
    case ISA_REGISTER_CLASS_integer:
      FmtAssert(reg < 16,("register integer greater than 16"));
      push_pop_mask |= 1 << (reg-1);
      break;
    default:
      not_pushed_callee_saved_vector.push_back(reg_pair);
#if 1
      FmtAssert(FALSE,("register class %s not managed",REGISTER_CLASS_name(rclass)));
#endif
    }
  }

  if (PU_is_interrupt(Get_Current_PU()) ||
      PU_is_interrupt_nostkaln(Get_Current_PU())) {
    for (i = 0; i < caller_saved_vector.size(); i++) {
      CLASS_REG_PAIR reg_pair = caller_saved_vector[i];
      ISA_REGISTER_CLASS rclass = CLASS_REG_PAIR_rclass(reg_pair);
      REGISTER           reg =    CLASS_REG_PAIR_reg(reg_pair);
      
#if 0
      REGISTER_Print(rclass,reg,TFile);
      fprintf(TFile,"\n");
#endif
      
      switch(rclass) {
      case ISA_REGISTER_CLASS_integer:
	FmtAssert(reg < 16,("register integer greater than 16"));
	push_pop_mask |= 1 << (reg-1);
	break;
      default:
	not_pushed_caller_saved_vector.push_back(reg_pair);
      }
    }
  }
  
#if 0
  fprintf(TFile,"rl_mask = %#x\n",push_pop_mask);
#endif

}

/* ====================================================================
 *   EETARG_Set_Frame_Len
 *
 *   Setup the computed frame len for the specific code defined here.
 * ====================================================================
 */
void
EETARG_Set_Frame_Len(INT64 frame_len)
{
  // if push mask is not requested, don't change.
  if (!CG_gen_callee_saved_regs_mask) return;

  // If push_area_size is empty, don't change.
  if ((callee_saved_vector.size() == 0) &&
      (caller_saved_vector.size() == 0))
      return;

  ST *regsave_stb;
  regsave_stb = Get_Reg_Save_Area();

  INT64 ofst =  ST_ofst(regsave_stb);
  ST *base = ST_base(regsave_stb);
  INT64 size = STB_size(regsave_stb);

  if (base == SP_Sym) {
    /* If REGSAVE is SP based (SMALL and LARGE model). */
    lower_size = ofst;
    push_area_size = size;
    upper_size = frame_len - push_area_size - lower_size;
  } else /* FP_Sym. */ {
    /* If REGSAVE is FP based (DYNAMIC model), 
       in this case ofst is negative. */
    upper_size = -ofst - size;
    push_area_size = size;
    lower_size = frame_len - push_area_size - upper_size;
  }
  frame_size = frame_len;
#if 0
  fprintf(TFile, "<lay> frame_len: %lld, regsave: %lld, lower: %lld, upper: %lld\n", frame_len, push_area_size, lower_size, upper_size);
#endif
}

/* ====================================================================
 *   EETARG_Fixup_Entry_Code
 *
 *   Generate push/pop sequence.
 * ====================================================================
 */
void
EETARG_Fixup_Entry_Code (
  BB *bb
)
{
  INT i;

  if (PU_is_interrupt(Get_Current_PU()) ||
      PU_is_interrupt_nostkaln(Get_Current_PU())) {

  FmtAssert(FALSE,("Not implemented"));
#if 0

    // [CR] Interrupt handlers have to enable HWLOOP bit if necessary
    if (PU_Has_Calls || PU_Has_Hwloops) {

      OPS ops_hwloop = OPS_EMPTY;
      OP *point = NULL;

      /* Find available scratch register. */
      TN *tmp_tn, *tmp_sn;
      ISA_REGISTER_CLASS rc = ISA_REGISTER_CLASS_gpr;
      REGISTER_SET avail_temps[ISA_REGISTER_CLASS_MAX+1];
      /* Get available registers at the start of the block. */
      REG_LIVE_Prolog_Temps(bb, NULL, NULL, avail_temps);
      REGISTER reg = REGISTER_SET_Choose(avail_temps[rc]);
      /* This should not happen if handled correctly before code gen. */
      FmtAssert(reg != REGISTER_UNDEFINED, ("No available register for prolog seq"));

      tmp_tn = Build_Dedicated_TN(rc, reg, 4);
      tmp_sn = Build_Dedicated_TN(ISA_REGISTER_CLASS_sfr, 2, 0);
      TN *flag1_tn = Gen_Literal_TN(6, 4);
      TN *flag2_tn = Gen_Literal_TN(7, 4);
      Build_OP(TOP_movesfr2r, tmp_tn, True_TN, tmp_sn, &ops_hwloop);
      Build_OP(TOP_bset_i5, tmp_tn, True_TN, tmp_tn, flag1_tn, &ops_hwloop);
      Build_OP(TOP_bset_i5, tmp_tn, True_TN, tmp_tn, flag2_tn, &ops_hwloop);
      Build_OP(TOP_mover2sfr, tmp_sn, True_TN, tmp_tn, &ops_hwloop);
      point = OPS_first(&ops_hwloop);
      point = BB_first_op(bb);
      if (point == NULL) {
	BB_Append_Ops (bb, &ops_hwloop);
      }
      else {
	BB_Insert_Ops_Before(bb, point, &ops_hwloop);
      }
    }

    // Set GP
    if (PU_Has_Calls || PU_References_GP) {
      EETARG_restore_gp_from_symbol(bb);
    }
#endif
  }

  // if push mask is not required -- nada
  if ((!CG_gen_callee_saved_regs_mask) &&
      (!(PU_is_interrupt(Get_Current_PU()) ||
	 PU_is_interrupt_nostkaln(Get_Current_PU())))) 
    return;

  if ((callee_saved_vector.size() == 0) &&
      (caller_saved_vector.size() == 0))
    return;

  ANNOTATION *ant = ANNOT_Get(BB_annotations(bb), ANNOT_ENTRYINFO);
  ENTRYINFO *entry_info = ANNOT_entryinfo(ant);
  OP *ent_adj = ENTRYINFO_sp_adj(entry_info);
  OP *sp_adj;
  OPS ops_adjust = OPS_EMPTY;
  OPS ops_adjust_lower = OPS_EMPTY;
  OPS ops_push = OPS_EMPTY;
  

  sp_adj = ent_adj;
  
  if (has_fp_adjust) {
    do {
      sp_adj = OP_prev(sp_adj);
    } while (sp_adj != NULL &&
	     !(OP_results(sp_adj) > 0 && TN_is_sp_reg(OP_result(sp_adj,0))));
  }

#if 0
  fprintf(TFile, "\nFrame allocation before target dependent fixup:\n");
  if (sp_adj != NULL) Print_OP_No_SrcLine(sp_adj);
  if (ent_adj != NULL && sp_adj != ent_adj) Print_OP_No_SrcLine(ent_adj);
#endif

  OP *point = NULL;

  /* Find available register for adjusts. */
  TN *tmp_tn;
  ISA_REGISTER_CLASS rc = ISA_REGISTER_CLASS_integer;
  REGISTER_SET avail_temps[ISA_REGISTER_CLASS_MAX+1];
  /* Get available registers at the start of the block. */
  REG_LIVE_Prolog_Temps(bb, NULL, NULL, avail_temps);
  REGISTER reg = REGISTER_SET_Choose(avail_temps[rc]);
  /* This should not happen is handled correctly before code gen.
     TODO: find correct condition (16 register && all arguments used ?)
     Then disallow use_push_pop at function level before code gen. */
#if 0
  FmtAssert(reg != REGISTER_UNDEFINED, ("No available register for prologue sequence"));
  tmp_tn = Build_Dedicated_TN(rc, reg, 4);
#else
  // [JV] Try to use GP
  if(reg == REGISTER_UNDEFINED) {
    tmp_tn = EETARG_get_temp_for_spadjust(bb);
    FmtAssert(tmp_tn != NULL, ("No available register for prologue sequence"));
  }
  else {
    tmp_tn = Build_Dedicated_TN(rc, reg, 4);
  }
  
  if (upper_size != 0) {
    TN *size_tn = Gen_Literal_TN(upper_size, 4);
    Exp_SUB (Pointer_Mtype, SP_TN, SP_TN, size_tn, &ops_adjust);
    if (OPS_last(&ops_adjust) != OPS_first(&ops_adjust)) {
      /* More than one instruction, we will do explicitly a 
	 move rx / sub sp, sp, rx sequence with an assigned register. */
      ops_adjust = (OPS)OPS_EMPTY;
      Exp_Immediate(tmp_tn, size_tn, FALSE, &ops_adjust);
      Exp_SUB(Pointer_Mtype, SP_TN, SP_TN, tmp_tn, &ops_adjust);
    }
  }
  
  if(push_pop_mask != 0) {
    Build_OP(TOP_stm_reglist_pre_npc_amode4s,SP_TN, Gen_Enum_TN(ISA_ECV_amode4s_fd), SP_TN, Gen_Literal_TN ( push_pop_mask, 4 ), &ops_push );
  }

  for(i = 0; i < not_pushed_callee_saved_vector.size(); i++) {
    CLASS_REG_PAIR reg_pair =   not_pushed_callee_saved_vector[i];
    ISA_REGISTER_CLASS rclass = CLASS_REG_PAIR_rclass(reg_pair);
    REGISTER           reg =    CLASS_REG_PAIR_reg(reg_pair);
    TN                *src =    Build_Dedicated_TN(rclass,reg,0);
    
    Expand_SinglePush( SP_TN, 4, src, TRUE, &ops_push);
  }
    
  for(i = 0; i < not_pushed_caller_saved_vector.size(); i++) {
    CLASS_REG_PAIR reg_pair =   not_pushed_caller_saved_vector[i];
    ISA_REGISTER_CLASS rclass = CLASS_REG_PAIR_rclass(reg_pair);
    REGISTER           reg =    CLASS_REG_PAIR_reg(reg_pair);
    TN                *src =    Build_Dedicated_TN(rclass,reg,0);
    
    Expand_SinglePush( SP_TN, 4, src, TRUE, &ops_push);
  }
    
  if (lower_size != 0) {
    TN *size_tn = Gen_Literal_TN(lower_size, 4);
    Exp_SUB (Pointer_Mtype, SP_TN, SP_TN, size_tn, &ops_adjust_lower);
    if (OPS_last(&ops_adjust_lower) != OPS_first(&ops_adjust_lower)) {
      /* More than one instruction, we will do explicitly a 
	 move rx / sub sp, sp, rx sequence with an assigned register. */
      ops_adjust_lower = (OPS)OPS_EMPTY;
      Exp_Immediate(tmp_tn, size_tn, FALSE, &ops_adjust_lower);
      Exp_SUB(Pointer_Mtype, SP_TN, SP_TN, tmp_tn, &ops_adjust_lower);
    }
  }
  

  /* Adjust insertion point. */
  if (sp_adj != NULL) {
    OP *new_adj = OPS_last(&ops_adjust_lower);
    /* Point to the possibly new SP adjust 
       (it may be NULL if lower_size == 0). */
    ENTRYINFO_sp_adj(entry_info) = new_adj;
    BB_Remove_Op(bb, sp_adj);
  }   
  point = BB_first_op(bb);

#if 0
  fprintf(TFile, "\nNew target dependent stack frame allocation:\n");
  Print_OPS(&ops_adjust);
  Print_OPS(&ops_push);
  Print_OPS(&ops_adjust_lower);
#endif

  if (point == NULL) {
    /* Effective operations order in block after this is:
       ops_adjust
       ops_push
       ops_adjust_lower
    */
    BB_Append_Ops (bb, &ops_adjust);
    BB_Append_Ops (bb, &ops_push);
    BB_Append_Ops (bb, &ops_adjust_lower);
  } else {
   /* Effective operations order in block after this is:
       ops_adjust
       ops_push
       ops_adjust_lower
    */
    BB_Insert_Ops_Before(bb, point, &ops_adjust);
    BB_Insert_Ops_Before(bb, point, &ops_push);
    BB_Insert_Ops_Before(bb, point, &ops_adjust_lower);
  }

#endif
  return;
}

/* ====================================================================
 *   EETARG_Fixup_Exit_Code
 *
 *   Generate push/pop sequence.
 * ====================================================================
 */
void
EETARG_Fixup_Exit_Code (
  BB *bb
)
{
  INT i;
  /* check for need to generate restore code for callee-saved regs */
  if ((!CG_gen_callee_saved_regs_mask) &&
      (!(PU_is_interrupt(Get_Current_PU()) ||
	 PU_is_interrupt_nostkaln(Get_Current_PU())))) return;
      
  if ((callee_saved_vector.size() == 0) &&
      (caller_saved_vector.size() == 0))
    return;

  ANNOTATION *ant = ANNOT_Get(BB_annotations(bb), ANNOT_EXITINFO);
  EXITINFO *exit_info = ANNOT_exitinfo(ant);
  OP *sp_adj = EXITINFO_sp_adj(exit_info);
  OP *xfer_op = BB_last_op(bb);
  BOOL has_dynamic_frame = Current_PU_Stack_Model == SMODEL_DYNAMIC;
  FmtAssert(xfer_op != NULL && OP_xfer(xfer_op), ("Unexpected empty or non xfer op in bb exit"));
  
  OPS ops_adjust = OPS_EMPTY;
  OPS ops_adjust_lower = OPS_EMPTY;
  OPS ops_pop = OPS_EMPTY;
  OPS ops_sp_fp_adjust = OPS_EMPTY;
  
  /* Find available register for adjusts. */
  TN *tmp_tn;
  ISA_REGISTER_CLASS rc = ISA_REGISTER_CLASS_integer;
  REGISTER_SET avail_temps[ISA_REGISTER_CLASS_MAX+1];
  /* Get available registers at the sp adjust or xfer operation. */
  REG_LIVE_Epilog_Temps(Get_Current_PU_ST(), bb, sp_adj != NULL ? OP_next(sp_adj) : xfer_op, avail_temps);
  REGISTER reg = REGISTER_SET_Choose(avail_temps[rc]);
  /* This should not happen as some scratch registers are available. */
  FmtAssert(reg != REGISTER_UNDEFINED, ("No available register for epilogue sequence"));
  tmp_tn = Build_Dedicated_TN(rc, reg, 4);
  
  if (upper_size != 0) {
    TN *size_tn = Gen_Literal_TN(upper_size, 4);
    Exp_ADD (Pointer_Mtype, SP_TN, SP_TN, size_tn, &ops_adjust);
    if (OPS_last(&ops_adjust) != OPS_first(&ops_adjust)) {
      /* More than one instruction, we will do explicitly a 
	 move rx / add sp, sp, rx sequence with an assigned register. */
      ops_adjust = (OPS)OPS_EMPTY;
      Exp_Immediate(tmp_tn, size_tn, FALSE, &ops_adjust);
      Exp_ADD(Pointer_Mtype, SP_TN, SP_TN, tmp_tn, &ops_adjust);
    }
  }

  for(i = not_pushed_caller_saved_vector.size()-1; i >=0; i--) {
    CLASS_REG_PAIR reg_pair   = not_pushed_caller_saved_vector[i];
    ISA_REGISTER_CLASS rclass = CLASS_REG_PAIR_rclass(reg_pair);
    REGISTER           reg    = CLASS_REG_PAIR_reg(reg_pair);
    TN                *res    = Build_Dedicated_TN(rclass,reg,0);
    
    Expand_SinglePop( res, SP_TN, 4, TRUE, &ops_pop);
  }

  for(i = not_pushed_callee_saved_vector.size()-1; i >=0; i--) {
    CLASS_REG_PAIR reg_pair   = not_pushed_callee_saved_vector[i];
    ISA_REGISTER_CLASS rclass = CLASS_REG_PAIR_rclass(reg_pair);
    REGISTER           reg    = CLASS_REG_PAIR_reg(reg_pair);
    TN                *res    = Build_Dedicated_TN(rclass,reg,0);
    
    Expand_SinglePop( res, SP_TN, 4, TRUE, &ops_pop);
  }

  if(push_pop_mask != 0) {
    Build_OP(TOP_ldm_reglist_pre_npc_amode4l, SP_TN, Gen_Enum_TN(ISA_ECV_amode4l_fd), SP_TN, Gen_Literal_TN ( push_pop_mask, 4 ), &ops_pop );
  }
  
  if (has_dynamic_frame) {
    /* In the case of a dynamic frame we must compute sp before 
     * generating the pop and upper adjust with:
     * sp = fp - (upper_size + push_area_size)
     */
    TN *size_tn;
    FmtAssert(sp_adj != NULL, ("Expected sp adjust for dynamic model"));
    size_tn =  Gen_Literal_TN(upper_size+push_area_size, 4);
    Exp_SUB (Pointer_Mtype, SP_TN, FP_TN, size_tn, &ops_adjust_lower);
    if (OPS_last(&ops_adjust_lower) != OPS_first(&ops_adjust_lower)) {
      /* More than one instruction, we will do explicitly a 
	 move rx / add sp, sp, rx sequence with an assigned register. */
      ops_adjust_lower = (OPS)OPS_EMPTY;
      Exp_Immediate(tmp_tn, size_tn, FALSE, &ops_adjust_lower);
      Exp_SUB(Pointer_Mtype, SP_TN, FP_TN, tmp_tn, &ops_adjust_lower);
    }
  } else if (lower_size != 0) {
    TN *size_tn = Gen_Literal_TN(lower_size, 4);
    Exp_ADD (Pointer_Mtype, SP_TN, SP_TN, size_tn, &ops_adjust_lower);
    if (OPS_last(&ops_adjust_lower) != OPS_first(&ops_adjust_lower)) {
      /* More than one instruction, we will do explicitly a 
	 move rx / add sp, sp, rx sequence with an assigned register. */
      ops_adjust_lower = (OPS)OPS_EMPTY;
      Exp_Immediate(tmp_tn, size_tn, FALSE, &ops_adjust_lower);
      Exp_ADD(Pointer_Mtype, SP_TN, SP_TN, tmp_tn, &ops_adjust_lower);
    }
  }
  
  if (sp_adj == NULL) {
    /* Effective operations order in block after this is:
       ops_adjust_lower
       ops_pop
       ops_adjust
    */
#if 0
    fprintf(TFile, "\nExit sequence:\n");
    Print_OPS(&ops_adjust_lower);
    Print_OPS(&ops_pop);
    Print_OPS(&ops_adjust);
#endif
    BB_Insert_Ops_Before(bb, xfer_op, &ops_adjust_lower);
    BB_Insert_Ops_Before(bb, xfer_op, &ops_pop);
    BB_Insert_Ops_Before(bb, xfer_op, &ops_adjust); 
  } else {
    OP *new_adj = OPS_last(&ops_adjust_lower);
    /* Point to the possibly new SP adjust 
       (it may be NULL if lower_size == 0). */
    EXITINFO_sp_adj(exit_info) = new_adj;

    /* Effective operations order in block after this is:
       [ops_sp_fp_adjust]
       ops_adjust_lower
       [other initial ops]
       ops_pop
       ops_adjust

       Note that pop operation must be generated after the [other initial ops]
       between the initial sp adjust and the end of the block as these
       operations may refer to a callee saved register that the pop could
       destroy. The ops_pop and ops_adjust operations, though, are generated
       just before the xfer operation.
    */
#if 0
    fprintf(TFile, "\nSP adjust before exit sequence:\n");
    Print_OP_No_SrcLine(sp_adj);
    fprintf(TFile, "\nExit sequence:\n");
    Print_OPS(&ops_adjust_lower);
    Print_OPS(&ops_pop);
    Print_OPS(&ops_adjust);
#endif
    BB_Insert_Ops_After(bb, sp_adj, &ops_adjust_lower);
    BB_Remove_Op(bb, sp_adj);
    BB_Insert_Ops_Before(bb, xfer_op, &ops_pop);
    BB_Insert_Ops_Before(bb, xfer_op, &ops_adjust); 
  }

  return;
}

/* =================================================================
 *   EETARG_Callee_Saved_Regs_Mask_Size
 *
 *   Calculate the size of the save area needed for the callee
 *   saved register mask.
 * =================================================================
 */
INT
EETARG_Callee_Saved_Regs_Mask_Size ()
{
  INT size = 0;

  /* check for need to generate restore code for callee-saved regs */
  if (!CG_gen_callee_saved_regs_mask) return 0;
  
  return size;
}

/* ====================================================================
 *   EETARG_Init_Entry_Exit_Code ()
 * ====================================================================
 */
void
EETARG_Init_Entry_Exit_Code (
  WN *pu_wn, 
  BOOL need_frame_pointer
)
{
  has_fp_adjust = need_frame_pointer;
  return;
}

/* ====================================================================
 *   EETARG_Save_Pfs
 *
 *   nothing to do, no PFS
 * ====================================================================
 */
void
EETARG_Save_Pfs (TN *saved_pfs, OPS *ops)
{
  return;
}

/* ====================================================================
 *   EETARG_Restore_Pfs
 *
 *   nothing to do, no PFS
 * ====================================================================
 */
void
EETARG_Restore_Pfs (TN *saved_pfs, OPS *ops)
{
  return;
}

/* ====================================================================
 *   EETARG_Save_Extra_Callee_Tns
 *
 *   save predicates
 *   This is handled differently cause we need to
 *   save and restore the whole bank of predicates in one instruction.
 *   Note that we assume GRA will remove the predicate save/restore
 *   if no callee-save predicates are used.
 * ====================================================================
 */
void EETARG_Save_Extra_Callee_Tns (
  OPS *ops
)
{
  // 1. Save the guard registers:
#if 0
  TN *callee_tn = CALLEE_tn(Callee_Saved_Regs_Count);
  // save callee_tn in callee-saved-regs array;
  // this works cause originally allocated space for all regs,
  // yet only use space for callee-save (so available space).
  // could get broken if ever allocated minimal space originally.
  if (callee_tn == NULL) {
    callee_tn = Build_RCLASS_TN(ISA_REGISTER_CLASS_du);
    Set_TN_is_gra_cannot_split(callee_tn);
    CALLEE_tn(Callee_Saved_Regs_Count) = callee_tn;
  }
  Build_OP (TOP_cmove, callee_tn, True_TN, Pr_TN, ops);
  Set_OP_no_move_before_gra(OPS_last(ops));
#endif
}

/* ====================================================================
 *   EETARG_Restore_Extra_Callee_Tns
 * ====================================================================
 */
void EETARG_Restore_Extra_Callee_Tns (
  OPS *ops
)
{
#if 0
  // restore all predicates
  TN *callee_tn = CALLEE_tn(Callee_Saved_Regs_Count);
  Build_OP (TOP_cmove, Pr_TN, True_TN, callee_tn, ops);
  Set_OP_no_move_before_gra(OPS_last(ops));
#endif
}


/* ====================================================================
 * EETARG_Do_Not_Save_Callee_Reg_Class
 * ====================================================================
 */

BOOL EETARG_Do_Not_Save_Callee_Reg_Class ( ISA_REGISTER_CLASS cl ) {
  return FALSE;
}

/* ======================================================================
 * EETARG_get_temp_for_spadjust
 * Should returns a super scratch (not allocatable) or defined register.
 * This register will be used to initialize stack pointer in entry block.
 * ======================================================================
 */

TN *EETARG_get_temp_for_spadjust( BB *bb) {

  return Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,13,0); /* Get super scratch R12 (ip) */
}

/* ======================================================================
 * EETARG_Save_With_Regmask
 * Return TRUE if (cl,reg) should be saved using the regmask mechanism
 * rather than the save_reg mechanism.
 * 
 * ======================================================================
 */

BOOL EETARG_Save_With_Regmask (ISA_REGISTER_CLASS cl, REGISTER reg)
{
  return CG_gen_callee_saved_regs_mask;
}
