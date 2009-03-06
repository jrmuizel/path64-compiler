/*
 *      Copyright 2006, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version  ST/HPC/STS (2006/Oct/17)
 */

#ifdef __X3

#error "builtins_model_X3 must not be used on X3 Core"

#else
#ifndef __builtins_model_x3_h
#define __builtins_model_x3_h


extern void __cmodel_x3_clrcc(void)	 	;
extern void __cmodel_x3_startcc(void)		;
extern void __cmodel_x3_stopcc(void)		;
extern void __cmodel_x3_clrcc1(void)	   	;
extern void __cmodel_x3_startcc1(void)   	;
extern void __cmodel_x3_stopcc1(void)		;
extern void __cmodel_x3_clrcca(void)	  	;
extern void __cmodel_x3_startcca(void)   	;
extern void __cmodel_x3_stopcca(void)		;

extern void __cmodel_x3_terminate(void)  	;
extern void __cmodel_x3_ptouch(int)   	;
extern void __cmodel_x3_pinval(int)   	;
extern void __cmodel_x3_pinvalr(int)	;
extern void __cmodel_x3_plock(int)	   	;
extern void __cmodel_x3_plockr(int)	;
extern void __cmodel_x3_pflushw(void) 	;
extern void __cmodel_x3_dflush(int)   	;
extern void __cmodel_x3_dflushw(void)	;
extern void __cmodel_x3_dtouch(int)   	;
extern void __cmodel_x3_dlock(int)	 	;
extern void __cmodel_x3_dinval(int)   	;
extern void __cmodel_x3_pstata(int)   	;
extern void __cmodel_x3_pstatwi(int)	;
extern void __cmodel_x3_dstata(int)	;
extern void __cmodel_x3_dstatwi(int)	;

extern int __cmodel_x3_mpssll(int, int);
extern int __cmodel_x3_mpsslh(int, int);
extern int __cmodel_x3_mpsshl(int, int);
extern int __cmodel_x3_mpsshh(int, int);

extern int __cmodel_x3_mpuull(unsigned, unsigned);
extern int __cmodel_x3_mpuulh(unsigned, unsigned);
extern int __cmodel_x3_mpuuhl(unsigned, unsigned);
extern int __cmodel_x3_mpuuhh(unsigned, unsigned);

extern int __cmodel_x3_mpusll(unsigned, int);
extern int __cmodel_x3_mpuslh(unsigned, int);
extern int __cmodel_x3_mpushl(unsigned, int);
extern int __cmodel_x3_mpushh(unsigned, int);

extern int __cmodel_x3_mpsull(int, unsigned);
extern int __cmodel_x3_mpsulh(int, unsigned);
extern int __cmodel_x3_mpsuhl(int, unsigned);
extern int __cmodel_x3_mpsuhh(int, unsigned);

#endif  /* __builtins_model_x3_h */

#endif	/* __X3	*/



