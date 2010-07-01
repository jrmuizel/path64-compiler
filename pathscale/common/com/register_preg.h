
#ifdef __cplusplus
extern "C" {
#endif
//Export functions needed by both the de and the be
#include "targ_isa_registers.h"
BE_EXPORTED extern PREG_NUM CGTARG_Regclass_Preg_Min(  ISA_REGISTER_CLASS rclass);
BE_EXPORTED extern PREG_NUM CGTARG_Regclass_Preg_Max(  ISA_REGISTER_CLASS rclass);
extern void Reset_RegisterClass_To_Preg(void);
#ifdef __cplusplus
}
#endif
