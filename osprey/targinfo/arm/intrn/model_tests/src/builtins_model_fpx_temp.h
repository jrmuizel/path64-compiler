/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version ST/HPC/STS (2006/Oct/26)
 */


/* Builtins model for class __FPX */
#ifdef __FPX

#error "builtins_model_fpx must not be used on SX+FPX Core"

#else
#ifndef _BUILTIN_MODEL_FPX_H_
#define _BUILTIN_MODEL_FPX_H_

extern unsigned __builtin_fpx_divu(unsigned, unsigned);
extern int 		__builtin_fpx_div(int, int);
extern float __builtin_fpx_fabs(float);
extern float __builtin_fpx_faddaa(float, float);
extern float __builtin_fpx_faddn(float, float);
extern float __builtin_fpx_fadd(float, float);
extern float __builtin_fpx_fasub(float, float);
extern int __builtin_fpx_fcmpeqn(float, float);
extern int __builtin_fpx_fcmpeq(float, float);
extern int __builtin_fpx_fcmpgen(float, float);
extern int __builtin_fpx_fcmpge(float, float);
extern int __builtin_fpx_fcmpgtn(float, float);
extern int __builtin_fpx_fcmpgt(float, float);
extern int __builtin_fpx_fcmplen(float, float);
extern int __builtin_fpx_fcmple(float, float);
extern int __builtin_fpx_fcmpltn(float, float);
extern int __builtin_fpx_fcmplt(float, float);
extern int __builtin_fpx_fcmpnen(float, float);
extern int __builtin_fpx_fcmpne(float, float);
extern int __builtin_fpx_fcmpueqn(float, float);
extern int __builtin_fpx_fcmpueq(float, float);
extern int __builtin_fpx_fcmpugen(float, float);
extern int __builtin_fpx_fcmpuge(float, float);
extern int __builtin_fpx_fcmpugtn(float, float);
extern int __builtin_fpx_fcmpugt(float, float);
extern int __builtin_fpx_fcmpulen(float, float);
extern int __builtin_fpx_fcmpule(float, float);
extern int __builtin_fpx_fcmpultn(float, float);
extern int __builtin_fpx_fcmpult(float, float);
extern int __builtin_fpx_fcmpunen(float, float);
extern int __builtin_fpx_fcmpune(float, float);
extern int __builtin_fpx_fcmpunn(float, float);
extern int __builtin_fpx_fcmpun(float, float);
extern int __builtin_fpx_fcmpuunn(float, float);
extern int __builtin_fpx_fcmpuun(float, float);
extern float __builtin_fpx_fdiv(float, float);
extern int __builtin_fpx_ff2in_r(float);
extern float __builtin_fpx_ff2in_f(float);
extern int __builtin_fpx_ff2i_r(float);
extern float __builtin_fpx_ff2i_f(float);
extern int __builtin_fpx_ff2un_r(float);
extern float __builtin_fpx_ff2un_f(float);
extern int __builtin_fpx_ff2u_r(float);
extern float __builtin_fpx_ff2u_f(float);
extern float __builtin_fpx_fi2f_r(int);
extern float __builtin_fpx_fi2f_f(float);
extern float __builtin_fpx_fmacn(float, float, float);
extern float __builtin_fpx_fmac(float, float, float);
extern float __builtin_fpx_fmax(float, float);
extern float __builtin_fpx_fmin(float, float);
extern float __builtin_fpx_fmuln(float, float);
extern float __builtin_fpx_fmul(float, float);
extern float __builtin_fpx_fmvf2f(float);
extern int __builtin_fpx_fmvf2r(float);
extern float __builtin_fpx_fmvr2f(int);
extern void __builtin_fpx_fmvr2st(int);
extern void __builtin_fpx_fmvr2s(int);
extern int __builtin_fpx_fmvs2r(void);
extern float __builtin_fpx_fneg(float);
extern float __builtin_fpx_fscalb(float, int);
extern float __builtin_fpx_fsqrt(float);
extern float __builtin_fpx_fsub(float, float);
extern float __builtin_fpx_fu2f_r(int);
extern float __builtin_fpx_fu2f_f(float);
extern int __builtin_fpx_macnsshh(int, int, int);
extern int __builtin_fpx_macnsshl(int, int, int);
extern int __builtin_fpx_macnsslh(int, int, int);
extern int __builtin_fpx_macnssll(int, int, int);
extern int __builtin_fpx_macnsuhh(int, int, unsigned);
extern int __builtin_fpx_macnsuhl(int, int, unsigned);
extern int __builtin_fpx_macnsulh(int, int, unsigned);
extern int __builtin_fpx_macnsull(int, int, unsigned);
extern int __builtin_fpx_macnushh(int, unsigned, int);
extern int __builtin_fpx_macnushl(int, unsigned, int);
extern int __builtin_fpx_macnuslh(int, unsigned, int);
extern int __builtin_fpx_macnusll(int, unsigned, int);
extern int __builtin_fpx_macnuuhh(int, unsigned, unsigned);
extern int __builtin_fpx_macnuuhl(int, unsigned, unsigned);
extern int __builtin_fpx_macnuulh(int, unsigned, unsigned);
extern int __builtin_fpx_macnuull(int, unsigned, unsigned);
extern int __builtin_fpx_macn(int, int, int);
extern int __builtin_fpx_macsshh(int, int, int);
extern int __builtin_fpx_macsshl(int, int, int);
extern int __builtin_fpx_macsslh(int, int, int);
extern int __builtin_fpx_macssll(int, int, int);
extern int __builtin_fpx_macsuhh(int, int, unsigned);
extern int __builtin_fpx_macsuhl(int, int, unsigned);
extern int __builtin_fpx_macsulh(int, int, unsigned);
extern int __builtin_fpx_macsull(int, int, unsigned);
extern int __builtin_fpx_macushh(int, unsigned, int);
extern int __builtin_fpx_macushl(int, unsigned, int);
extern int __builtin_fpx_macuslh(int, unsigned, int);
extern int __builtin_fpx_macusll(int, unsigned, int);
extern int __builtin_fpx_macuuhh(int, unsigned, unsigned);
extern int __builtin_fpx_macuuhl(int, unsigned, unsigned);
extern int __builtin_fpx_macuulh(int, unsigned, unsigned);
extern int __builtin_fpx_macuull(int, unsigned, unsigned);
extern int __builtin_fpx_mac(int, int, int);
extern unsigned __builtin_fpx_modu(unsigned, unsigned);
extern int __builtin_fpx_mod(int, int);
extern int __builtin_fpx_mpnsshh(int, int);
extern int __builtin_fpx_mpnsshl(int, int);
extern int __builtin_fpx_mpnsslh(int, int);
extern int __builtin_fpx_mpnssll(int, int);
extern int __builtin_fpx_mpnsuhh(int, unsigned);
extern int __builtin_fpx_mpnsuhl(int, unsigned);
extern int __builtin_fpx_mpnsulh(int, unsigned);
extern int __builtin_fpx_mpnsull(int, unsigned);
extern int __builtin_fpx_mpnushh(unsigned, int);
extern int __builtin_fpx_mpnushl(unsigned, int);
extern int __builtin_fpx_mpnuslh(unsigned, int);
extern int __builtin_fpx_mpnusll(unsigned, int);
extern int __builtin_fpx_mpnuuhh(unsigned, unsigned);
extern int __builtin_fpx_mpnuuhl(unsigned, unsigned);
extern int __builtin_fpx_mpnuulh(unsigned, unsigned);
extern int __builtin_fpx_mpnuull(unsigned, unsigned);
extern int __builtin_fpx_mpn(int, int);
extern int __builtin_fpx_mpsshh(int, int);
extern int __builtin_fpx_mpsshl(int, int);
extern int __builtin_fpx_mpsslh(int, int);
extern int __builtin_fpx_mpssll(int, int);
extern int __builtin_fpx_mpsuhh(int, unsigned);
extern int __builtin_fpx_mpsuhl(int, unsigned);
extern int __builtin_fpx_mpsulh(int, unsigned);
extern int __builtin_fpx_mpsull(int, unsigned);
extern int __builtin_fpx_mpushh(unsigned, int);
extern int __builtin_fpx_mpushl(unsigned, int);
extern int __builtin_fpx_mpuslh(unsigned, int);
extern int __builtin_fpx_mpusll(unsigned, int);
extern int __builtin_fpx_mpuuhh(unsigned, unsigned);
extern int __builtin_fpx_mpuuhl(unsigned, unsigned);
extern int __builtin_fpx_mpuulh(unsigned, unsigned);
extern int __builtin_fpx_mpuull(unsigned, unsigned);
extern int __builtin_fpx_mp(int, int);


#endif /* _BUILTIN_MODEL_FPX_H_	*/

#endif

