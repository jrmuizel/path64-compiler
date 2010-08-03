#ifdef INTRN_EXPAND

/* Check_Opd_Enum
 *   Generates the proper enum tn.
 */
static inline TN*
Check_Opd_Enum(TN *opd, INT enum_base, SRCPOS srcpos) {
  if (TN_is_rematerializable(opd))  {
    WN *wn = TN_remat(opd);
    opd = Gen_Enum_TN(WN_const_val(wn)+enum_base);
  } else {
    opd = Gen_Enum_TN(TN_value(opd)+enum_base);
  }
  return opd;
}

/* Check_Opd_RClass
 *   Expand a copy if source tn is not of given register class.
 */
static inline TN*
Check_Opd_RClass(TN *opd, ISA_REGISTER_CLASS rc, OPS *ops) {
  if(TN_is_register(opd) && TN_register_class(opd) != rc) {
    TN *tn_rc = Build_RCLASS_TN(rc);
    Expand_Copy(tn_rc,True_TN,opd,ops);
    return tn_rc;
  }
  else {
    return opd;
  }
}

/* Check_Opd_Literal
 *  Check the availability of TN value and if it fit in
 *  corresponding literal class range.
 *  Return literal TN.
 */
static inline TN*
Check_Opd_Literal( TN *tn_opnd, ISA_LIT_CLASS lc, INT32 arg_idx, const char *intrn_name, SRCPOS srcpos  ) {
  char error_msg[256];

  TN *current_value = Gen_Literal_TN(-1,4);

  if (TN_is_rematerializable(tn_opnd)) {
    WN *wn = TN_home(tn_opnd);
    if (WN_operator_is(wn, OPR_INTCONST)) {
      TN *cunknown = Gen_Literal_TN(WN_const_val(wn), 4);
      current_value = cunknown;
    } else {
      DevWarn("%s::Expand_Extension_intrinsic:"
	" TN_is_rematerializable BUT WN_operator_is *not* OPR_INTCONST.", __FILE__);
    }
  }
  else if (TN_is_zero(tn_opnd)) {
    current_value = Gen_Literal_TN(0,4);
  }
  else if (TN_has_value(tn_opnd)) {
   current_value = tn_opnd;
  }
  else {
    sprintf(error_msg, "Immediate value expected for arg %d of intrinsic call '%s'",
                       arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }

  // Check immediate value is in range
  if (!ISA_LC_Value_In_Class(TN_value(current_value), lc)) {
    sprintf(error_msg, "Immediate value '%lld' of arg %d out of bounds"
          " for intrinsic call '%s'.",
          TN_value(current_value), arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }
  return current_value;
}

/* Check_Opd_Literal_For_Dedicated
 *  Check the availability of TN value and if it fit in
 *  given register idx range [0..reg_file_size[.
 *  Return literal TN.
 */
static inline TN*
Check_Opd_Literal_For_Dedicated( TN *tn_opnd, INT32 reg_file_size, INT32 arg_idx, const char *intrn_name, SRCPOS srcpos  ) {
  char error_msg[256];

  TN *current_value = Gen_Literal_TN(-1,4);

  if (TN_is_rematerializable(tn_opnd)) {
    WN *wn = TN_home(tn_opnd);
    if (WN_operator_is(wn, OPR_INTCONST)) {
      INT64 value = WN_const_val(wn);
        current_value = Gen_Literal_TN(WN_const_val(wn), 4);
    } else {
      DevWarn("%s::Expand_Extension_intrinsic:"
	" TN_is_rematerializable BUT WN_operator_is *not* OPR_INTCONST.", __FILE__);
    }
  }
  else if (TN_is_zero(tn_opnd)) {
    current_value = Gen_Literal_TN(0,4);
  }
  else if (TN_has_value(tn_opnd)) {
   current_value = tn_opnd;
  }
  else {
    sprintf(error_msg, "Immediate value expected for arg %d of intrinsic call '%s'",
                       arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }

  // Check immediate value is in range
  if ( ! (0 <= TN_value(current_value) && TN_value(current_value) < reg_file_size) ) {
    sprintf(error_msg, "Immediate value '%lld' of arg %d out of bounds"
          " or wrong alignment for intrinsic call '%s'.",
          TN_value(current_value), arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }
  return current_value;
}

#endif /* INTRN_EXPAND */

#ifdef INTRN_SWITCH

#endif /* INTRN_SWITCH */

#ifdef INTRN_DEFINES

INTRINSIC_TARG_LAST	=	INTRINSIC_GENERIC_LAST+0,
#endif /* INTRN_DEFINES */

#ifdef INTRN_INFO

#endif /* INTRN_INFO */

#ifdef INTRN_WUTIL

#endif /* INTRN_WUTIL */

#ifdef INTRN_GFEC_WFE_EXPR

#endif /* INTRN_GFEC_WFE_EXPR */

#ifdef INTRN_GFECC_BUILTINS_DEF

#endif /* INTRN_GFECC_BUILTINS_DEF */

#ifdef INTRN_GFECC4_BUILTINS_DEF

#endif /* INTRN_GFECC4_BUILTINS_DEF */

#ifdef INTRN_GFEC_BUILTINS_DEF

#endif /* INTRN_GFEC_BUILTINS_DEF */

#ifdef INTRN_GFEC_BUILTINS_H

#endif /* INTRN_GFEC_BUILTINS_H */

#ifdef INTRN_ENUM_GSBI

#endif /* INTRN_ENUM_GSBI */

#ifdef INTRN_GCC_BUILT_IN2GSBI

#endif /* INTRN_GCC_BUILT_IN2GSBI */

#ifdef INTRN_WGEN_EXPR

#endif /* INTRN_WGEN_EXPR */

#ifdef INTRN_GSBI_NAME

#endif /* INTRN_GSBI_NAME */

