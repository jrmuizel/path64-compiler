/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version ST/HPC/STS (2006/Oct/18)
 */



#ifdef __AX

#error "builtins_model_ax must not be used on SX+AX Core"

#else
#ifndef  __BUILTIN_MODEL_AX_H_
#define	 __BUILTIN_MODEL_AX_H_

extern long long __builtin_ax_mpss(int s1, int s2);
extern long long __builtin_ax_mnss(int s1, int s2);
extern long long __builtin_ax_mpf(int s1, int s2);
extern long long __builtin_ax_mnf(int s1, int s2);
extern long long __builtin_ax_mpfr(int s1, int s2);
extern long long __builtin_ax_mnfr(int s1, int s2);
 
extern long long __builtin_ax_shlf(int s1, int s2);
extern long long __builtin_ax_shrf(int s1, int s2);
extern long long __builtin_ax_m2xhl(int s1, int s2);
 
extern long long __builtin_ax_mpssl(long long s1, int s2);
extern long long __builtin_ax_mnssl(long long s1, int s2);
extern long long __builtin_ax_mpfh(long long s1, int s2);
extern long long __builtin_ax_mnfh(long long s1, int s2);
extern long long __builtin_ax_mpfhr(long long s1, int s2);
extern long long __builtin_ax_mnfhr(long long s1, int s2);
 
extern long long __builtin_ax_addf(long long s1, int s2);
extern long long __builtin_ax_subf(long long s1, int s2);
extern long long __builtin_ax_addfr(long long s1, int s2);
extern long long __builtin_ax_subfr(long long s1, int s2);
extern long long __builtin_ax_addi(long long s1, int s2);
extern long long __builtin_ax_subi(long long s1, int s2);
extern long long __builtin_ax_addiu(long long s1, unsigned int s2);
extern long long __builtin_ax_subiu(long long s1, unsigned int s2);
 
extern long long __builtin_ax_shl_r(long long s1, int s2);
extern long long __builtin_ax_shr_r(long long s1, int s2);
extern long long __builtin_ax_shlr_r(long long s1, int s2);
extern long long __builtin_ax_shrr_r(long long s1, int s2);
extern unsigned long long __builtin_ax_shru_r(unsigned long long s1, unsigned int s2);

extern long long __builtin_ax_add(long long s1, long long s2) ;
extern long long __builtin_ax_sub(long long s1, long long s2) ;
extern long long __builtin_ax_addr(long long s1, long long s2);
extern long long __builtin_ax_subr(long long s1, long long s2);

extern int __builtin_ax_shlm2rl_r(long long s1, int s2);
extern int __builtin_ax_shlm2rh_r(long long s1, int s2);
extern int __builtin_ax_shrm2rl_r(long long s1, int s2);
extern int __builtin_ax_shrm2rh_r(long long s1, int s2);

// TNT extern unsigned long long __builtin_ax_llabs(long long s);
extern unsigned long long __builtin_ax_abs(long long s);


extern long long __builtin_ax_rndc(long long s)	;
extern long long __builtin_ax_rnd16(long long s)	;
extern long long __builtin_ax_rndc16(long long s)	;
extern long long __builtin_ax_clp32(long long s)	;
extern long long __builtin_ax_neg(long long s)		;

extern long long __builtin_ax_m2xf(int s)	;
extern long long __builtin_ax_m2xi(int s)	;
extern long long __builtin_ax_m2xiu(unsigned int s);
extern long long __builtin_ax_m2xl(long long s1, int s2);
extern long long __builtin_ax_m2xh(long long s1, int s2);

extern long long __builtin_ax_shlr_i6(long long s1, unsigned short s2);
extern long long __builtin_ax_shrr_i6(long long s1, unsigned short s2);
extern long long __builtin_ax_shl_i6(long long s1, unsigned short s2);
extern long long __builtin_ax_shr_i6(long long s1, unsigned short s2);
extern unsigned long long __builtin_ax_shru_i6(unsigned long long s1, unsigned short s2);

extern int __builtin_ax_shlm2rl_i6(long long s1, unsigned short s2);
extern int __builtin_ax_shlm2rh_i6(long long s1, unsigned short s2);
extern int __builtin_ax_shrm2rl_i6(long long s1, unsigned short s2);
extern int __builtin_ax_shrm2rh_i6(long long s1, unsigned short s2);


extern long long __builtin_ax_maf(long long s1, int s2, int s3);
extern long long __builtin_ax_mafr(long long s1, int s2, int s3);
extern long long __builtin_ax_mafh(long long s1, long long s2, int s3);
extern long long __builtin_ax_mafhr(long long s1, long long s2, int s3);
extern long long __builtin_ax_mass(long long s1, int s2, int s3);
extern long long __builtin_ax_massl(long long s1, long long s2, int s3);

extern long long __builtin_ax_msf(long long s1, int s2, int s3);
extern long long __builtin_ax_msfr(long long s1, int s2, int s3);
extern long long __builtin_ax_msfh(long long s1, long long s2, int s3);
extern long long __builtin_ax_msfhr(long long s1, long long s2, int s3);
extern long long __builtin_ax_msss(long long s1, int s2, int s3);
extern long long __builtin_ax_msssl(long long s1, long long s2, int s3);


extern long long __builtin_ax_shli_r(int s1, unsigned int s2);
extern long long __builtin_ax_shri_r(int s1, unsigned int s2);
extern unsigned long long __builtin_ax_shliu_r(unsigned int s1, unsigned int s2);
extern unsigned long long __builtin_ax_shriu_r(unsigned int s1, unsigned int s2);

extern long long __builtin_ax_shli_i6(int s1, unsigned short s2);
extern long long __builtin_ax_shri_i6(int s1, unsigned short s2);
extern unsigned long long __builtin_ax_shliu_i6(unsigned int s1, unsigned short s2);
extern unsigned long long __builtin_ax_shriu_i6(unsigned int s1, unsigned short s2);


extern int __builtin_ax_m2rl(long long s)	;
extern int __builtin_ax_m2rh(long long s)	;

extern long long __builtin_ax_addhl(long long s1, int s2, int s3);
extern long long __builtin_ax_subhl(long long s1, int s2, int s3);
extern long long __builtin_ax_addhlr(long long s1, int s2, int s3);
extern long long __builtin_ax_subhlr(long long s1, int s2, int s3);

extern long long __builtin_ax_clr(long long);


#endif  /*__BUILTIN_MODEL_AX_H_  */


#endif

