#ifndef wn_coverage_INCLUDED
#define wn_coverage_INCLUDED

#include "wn.h"
#include "pu_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialisation of gcov module: need to be called once */
BE_EXPORTED extern void wn_coverage_init ();
/* Finish gcno generation + instrument code: to be called once */
BE_EXPORTED extern void wn_coverage_finish (void);

/* Generate gcov info for the current pu. Need to be called once for each PU*/
BE_EXPORTED extern void wn_coverage_pu(WN */* pu */);

BE_EXPORTED extern void  wn_coverage_Generate_Func_Start_Profiler_PU(PU_Info** /* _pu_tree_p */);

#ifdef __cplusplus
}
#endif

#endif 
