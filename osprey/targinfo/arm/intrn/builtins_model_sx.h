/*
 *      Copyright 2005-2006, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version ST/HPC/STS (2006/Oct/16)
 */


#ifdef __SX

#error "builtins_model_sx must not be used on SX Core"

#else

#ifndef __builtins_model_sx_h
#define __builtins_model_sx_h

extern void 	__cmodel_sx_barrier(void)		;
extern void 	__cmodel_sx_nop(void)			;
extern void 	__cmodel_sx_bkp(void)			;
extern void 	__cmodel_sx_rte(void)			;


extern int 		__cmodel_sx_bclr_r(int, int)	;
extern int 		__cmodel_sx_bset_r(int, int)	;
extern int 		__cmodel_sx_bnot_r(int, int)	;
extern int 		__cmodel_sx_bclr_i5(int, unsigned short)	;
extern int 		__cmodel_sx_bset_i5(int, unsigned short)	;
extern int 		__cmodel_sx_bnot_i5(int, unsigned short)	;

extern int 		__cmodel_sx_max_r(int, int)	;
extern unsigned __cmodel_sx_maxu_r(unsigned, unsigned)	;
extern int 		__cmodel_sx_min_r(int, int)	;
extern unsigned __cmodel_sx_minu_r(unsigned, unsigned)	;
extern int 		__cmodel_sx_max_i8(int, unsigned short)	;
extern unsigned __cmodel_sx_maxu_i8(unsigned, unsigned short)	;
extern int 		__cmodel_sx_min_i8(int, unsigned short)	;
extern unsigned __cmodel_sx_minu_i8(unsigned, unsigned short)	;


extern int 		__cmodel_sx_add_r(int, int)	;
extern int 		__cmodel_sx_sub_r(int, int)	;
extern unsigned __cmodel_sx_addu_r(unsigned, unsigned)		;
extern unsigned __cmodel_sx_subu_r(unsigned, unsigned)		;
extern int 		__cmodel_sx_add_i8(int, unsigned short)	;
extern int 		__cmodel_sx_sub_i8(int, unsigned short)	;
extern unsigned __cmodel_sx_addu_i8(unsigned, unsigned short)	;
extern unsigned __cmodel_sx_subu_i8(unsigned, unsigned short)	;

extern int 		__cmodel_sx_and_r(int, int)	;
extern int 		__cmodel_sx_and_i8(int, unsigned short)	;
extern int 		__cmodel_sx_andn(int, int)	;
extern int 		__cmodel_sx_xor_r(int, int)	;
extern int 		__cmodel_sx_xor_i8(int, unsigned short)	;
extern int 		__cmodel_sx_or_r(int, int)	;
extern int 		__cmodel_sx_or_i8(int, unsigned short)	;
extern int 		__cmodel_sx_norn(int, unsigned short)	;

extern int 		__cmodel_sx_shl_r(int, int)	;
extern unsigned __cmodel_sx_shlu_r(unsigned, unsigned)	;
extern int 		__cmodel_sx_shr_r(int, int)	;
extern unsigned __cmodel_sx_shru_r(unsigned, unsigned)	;
extern int 		__cmodel_sx_rotl_r(int, int)	;

extern int 		__cmodel_sx_shl_i5(int, unsigned short)	;
extern unsigned __cmodel_sx_shlu_i5(unsigned, unsigned short)	;
extern int 		__cmodel_sx_shr_i5(int, unsigned short)	;
extern unsigned __cmodel_sx_shru_i5(unsigned, unsigned short)	;
extern int 		__cmodel_sx_rotl_i5(int, unsigned short)	;

extern unsigned __cmodel_sx_shrnu(unsigned, unsigned)	;
extern unsigned __cmodel_sx_shlnu(unsigned, unsigned)	;

extern unsigned __cmodel_sx_shlmu_r(unsigned)	;
extern unsigned __cmodel_sx_shrmu_r(unsigned)	;
extern unsigned __cmodel_sx_shlmu_i5(unsigned short)	;
extern unsigned __cmodel_sx_shrmu_i5(unsigned short)	;


extern unsigned __cmodel_sx_abs(int)		;
extern unsigned __cmodel_sx_absu(int)		;
extern int 		__cmodel_sx_prior(int)		;
extern int 		__cmodel_sx_lzc(int)		;
extern int 	   	__cmodel_sx_clamph(int)	;

extern int 		__cmodel_sx_extb(int)	;
extern int 		__cmodel_sx_exth(int)	;
extern unsigned __cmodel_sx_extub(int)	;
extern unsigned __cmodel_sx_extuh(int)	;

extern int 		__cmodel_sx_neg(int)	;
extern int 		__cmodel_sx_negu(int)	;
extern int 		__cmodel_sx_not(int)	;

extern int 		__cmodel_sx_make(short)	;
extern int 		__cmodel_sx_more(int, unsigned short)	;


extern unsigned __cmodel_sx_absbp(int)		;
extern unsigned __cmodel_sx_abshp(int)		;
extern unsigned __cmodel_sx_absubp(int)	;
extern unsigned __cmodel_sx_absuhp(int)	;

extern int 		__cmodel_sx_addbp_i8(int, unsigned short)	;
extern int 		__cmodel_sx_addhp_i8(int, unsigned short)	;
extern unsigned __cmodel_sx_addubp_i8(unsigned, unsigned short)	;
extern unsigned __cmodel_sx_adduhp_i8(unsigned, unsigned short)	;

extern int 		__cmodel_sx_subbp_i8(int, unsigned short)	;
extern int 		__cmodel_sx_subhp_i8(int, unsigned short)	;
extern int 		__cmodel_sx_sububp_i8(int, unsigned short)	;
extern int 		__cmodel_sx_subuhp_i8(int, unsigned short)	;

extern int 		__cmodel_sx_addbp_r(int, int)	;
extern int 		__cmodel_sx_addhp_r(int, int)	;
extern unsigned __cmodel_sx_addubp_r(unsigned, unsigned)	;
extern unsigned __cmodel_sx_adduhp_r(unsigned, unsigned)	;

extern int 		__cmodel_sx_subbp_r(int, int)	;
extern int 		__cmodel_sx_subhp_r(int, int)	;
extern int 		__cmodel_sx_sububp_r(int, int)	;
extern int 		__cmodel_sx_subuhp_r(int, int)	;


extern int 		__cmodel_sx_maxhp_i8(int, unsigned short)	;
extern int 		__cmodel_sx_minhp_i8(int, unsigned short)	;
extern unsigned __cmodel_sx_maxuhp_i8(unsigned, unsigned short)	;
extern unsigned __cmodel_sx_minuhp_i8(unsigned, unsigned short)	;

extern int 		__cmodel_sx_maxhp_r(int, int)	;
extern int 		__cmodel_sx_minhp_r(int, int)	;
extern unsigned __cmodel_sx_maxuhp_r(unsigned, unsigned)	;
extern unsigned __cmodel_sx_minuhp_r(unsigned, unsigned)	;

extern int 		__cmodel_sx_maxbp_i8(int, unsigned short)	;
extern int 		__cmodel_sx_minbp_i8(int, unsigned short)	;
extern unsigned __cmodel_sx_maxubp_i8(unsigned, unsigned short)	;
extern unsigned __cmodel_sx_minubp_i8(unsigned, unsigned short)	;

extern int 		__cmodel_sx_maxbp_r(int, int)	;
extern int 		__cmodel_sx_minbp_r(int, int)	;
extern unsigned __cmodel_sx_maxubp_r(unsigned, unsigned)	;
extern unsigned __cmodel_sx_minubp_r(unsigned, unsigned)	;


extern int 		__cmodel_sx_negbp(int)	;
extern int 		__cmodel_sx_neghp(int)	;
extern int 		__cmodel_sx_negubp(int)	;
extern int 		__cmodel_sx_neguhp(int)	;


extern int 		__cmodel_sx_moveh2h(int s1, int s2)	;
extern int 		__cmodel_sx_moveh2l(int s1, int s2)	;
extern int 		__cmodel_sx_movel2h(int s1, int s2)	;
extern int 		__cmodel_sx_movel2l(int s1, int s2)	;

//-------------------------------------------------

#endif  /* __builtins_model_sx_h */

#endif  /* __SX */
