
#ifndef targ_cgir_lao_INCLUDED
#define targ_cgir_lao_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------- Init/Fini --------------------------------*/
/* Need to be called only once per process execution. */
extern void CGIR_LAI_Init(void);
extern void CGIR_LAI_Fini(void);

/*-------------------- CGIR -> LIR Conversions ------------------*/
extern LAI_Operator CGIR_TOP_to_Operator(TOP top);
extern LAI_Operator CGIR_TOP_to_Operator(TOP top);
extern LAI_Register CGIR_CRP_to_Register(CLASS_REG_PAIR crp);
extern LAI_RegClass CGIR_IRC_to_RegClass(ISA_REGISTER_CLASS irc);
extern LAI_Immediate CGIR_LC_to_Immediate(ISA_LIT_CLASS ilc);
extern LAI_Modifier CGIR_IEC_to_Modifier(ISA_ENUM_CLASS iec);
extern LAI_InstrMode CGIR_IS_to_InstrMode(ISA_SUBSET is);

/*-------------------- LIR -> CGIR Conversions ------------------*/
extern ISA_REGISTER_CLASS TARG_RegClass_to_CGIR_IRC(LAI_RegClass regClass);
extern CLASS_REG_PAIR TARG_Register_to_CGIR_CRP(LAI_Register registre);
extern TOP TARG_Operator_to_CGIR_TOP(LAI_Operator lir_operator);
extern TYPE_ID TARG_MType_to_CGIR_TYPE_ID(LAI_MType lir_mtype);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* !targ_cgir_lao_INCLUDED */
