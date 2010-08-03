
#ifndef _GLOBALS_H_
#define _GLOBALS_H_


extern char ** stringTable_Glb;
extern uint16 stringTableSize_Glb ;
extern nodeSEM ** semTable_Glb;
extern uint16 semTableSize_Glb;

extern nodeSEM * NTR_expr_Glb;
extern nodeSEM * NTR_MACRO_EXP_Glb;
extern nodeSEM * NTR_CONTROL_DIR_Glb;
extern nodeSEM * NTR_LOOP_LINE_Glb;
extern nodeSEM * NTR_PREPROC_LINE_Glb;

#endif

