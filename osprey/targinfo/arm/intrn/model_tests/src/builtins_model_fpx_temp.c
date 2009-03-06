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


/* Builtins models for class __FPX */
#ifdef __FPX

#error "builtins_model_fpx must not be used on SX+FPX Core"

#else
#include "builtins_model_common.h"

unsigned __builtin_fpx_divu(unsigned u1, unsigned u2)
{
	 return (unsigned)((unsigned)u1/(unsigned)u2) ;		 
}

int 		__builtin_fpx_div(int s1, int s2)
{
	 return (int) ((int)s1/(int)s2) ;					 
}

float __builtin_fpx_fabs(float f1)
{
  return (float) fabsf((float)f1) ;		
}

float __builtin_fpx_faddaa(float f1, float f2)
{
   float t1 = fabsf((float)f1) ;
   float t2 = fabsf((float)f2) ;
   return (float)t1 + (float)t2 ;
}

float __builtin_fpx_faddn(float f1, float f2)
{
	float t1 = (float)f1 + (float)f2 ;
	return (float) (-t1)  ;
}


float __builtin_fpx_fadd(float f1 , float f2 )
{
	return (float)f1 + (float)f2 ;
}

float __builtin_fpx_fasub(float f1, float f2)
{
	float t1 = (float)f1 - (float)f2 ;
    return (float) fabsf((float)t1) ;		
}


int __builtin_fpx_fcmpeqn(float f1, float f2){}
int __builtin_fpx_fcmpeq(float f1, float f2){}
int __builtin_fpx_fcmpgen(float f1, float f2){}
int __builtin_fpx_fcmpge(float f1, float f2){}
int __builtin_fpx_fcmpgtn(float f1, float f2){}
int __builtin_fpx_fcmpgt(float f1, float f2){}
int __builtin_fpx_fcmplen(float f1, float f2){}
int __builtin_fpx_fcmple(float f1, float f2){}
int __builtin_fpx_fcmpltn(float f1, float f2){}
int __builtin_fpx_fcmplt(float f1, float f2){}
int __builtin_fpx_fcmpnen(float f1, float f2){}
int __builtin_fpx_fcmpne(float f1, float f2){}
int __builtin_fpx_fcmpueqn(float f1, float f2){}
int __builtin_fpx_fcmpueq(float f1, float f2){}
int __builtin_fpx_fcmpugen(float f1, float f2){}
int __builtin_fpx_fcmpuge(float f1, float f2){}
int __builtin_fpx_fcmpugtn(float f1, float f2){}
int __builtin_fpx_fcmpugt(float f1, float f2){}
int __builtin_fpx_fcmpulen(float f1, float f2){}
int __builtin_fpx_fcmpule(float f1, float f2){}
int __builtin_fpx_fcmpultn(float f1, float f2){}
int __builtin_fpx_fcmpult(float f1, float f2){}
int __builtin_fpx_fcmpunen(float f1, float f2){}
int __builtin_fpx_fcmpune(float f1, float f2){}
int __builtin_fpx_fcmpunn(float f1, float f2){}
int __builtin_fpx_fcmpun(float f1, float f2){}
int __builtin_fpx_fcmpuunn(float f1, float f2){}
int __builtin_fpx_fcmpuun(float f1, float f2){}


float __builtin_fpx_fdiv(float f1, float f2)
{ //be careful to the order
	return (float)f2 / (float)f1 ;

}





int __builtin_fpx_ff2in_r(float f1){}
float __builtin_fpx_ff2in_f(float f1){}
int __builtin_fpx_ff2i_r(float f1){}
float __builtin_fpx_ff2i_f(float f1){}
int __builtin_fpx_ff2un_r(float f1){}
float __builtin_fpx_ff2un_f(float f1){}
int __builtin_fpx_ff2u_r(float f1){}
float __builtin_fpx_ff2u_f(float f1){}
float __builtin_fpx_fi2f_r(int s1){}
float __builtin_fpx_fi2f_f(float f1){}
float __builtin_fpx_fmacn(float f1, float f2, float f3){}
float __builtin_fpx_fmac(float f1, float f2, float f3){}
float __builtin_fpx_fmax(float f1, float f2){}
float __builtin_fpx_fmin(float f1, float f2){}
float __builtin_fpx_fmuln(float f1, float f2){}
float __builtin_fpx_fmul(float f1, float f2){}
float __builtin_fpx_fmvf2f(float f1){}
int __builtin_fpx_fmvf2r(float f1){}
float __builtin_fpx_fmvr2f(int s1){}
void __builtin_fpx_fmvr2st(int s1){}
void __builtin_fpx_fmvr2s(int s1){}
int __builtin_fpx_fmvs2r(void){}
float __builtin_fpx_fneg(float f1){}
float __builtin_fpx_fscalb(float f1, int s2){}
float __builtin_fpx_fsqrt(float f1){}
float __builtin_fpx_fsub(float f1, float f2){}
float __builtin_fpx_fu2f_r(int s1){}
float __builtin_fpx_fu2f_f(float f1){}
int __builtin_fpx_macnsshh(int s1, int s2, int s3){}
int __builtin_fpx_macnsshl(int s1, int s2, int s3){}
int __builtin_fpx_macnsslh(int s1, int s2, int s3){}
int __builtin_fpx_macnssll(int s1, int s2, int s3){}
int __builtin_fpx_macnsuhh(int s1, int s2, unsigned u3){}
int __builtin_fpx_macnsuhl(int s1, int s2, unsigned u3){}
int __builtin_fpx_macnsulh(int s1, int s2, unsigned u3){}
int __builtin_fpx_macnsull(int s1, int s2, unsigned u3){}
int __builtin_fpx_macnushh(int s1, unsigned u2, int s3){}
int __builtin_fpx_macnushl(int s1, unsigned u2, int s3){}
int __builtin_fpx_macnuslh(int s1, unsigned u2, int s3){}
int __builtin_fpx_macnusll(int s1, unsigned u2, int s3){}
int __builtin_fpx_macnuuhh(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macnuuhl(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macnuulh(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macnuull(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macn(int s1, int s2, int s3){}
int __builtin_fpx_macsshh(int s1, int s2, int s3){}
int __builtin_fpx_macsshl(int s1, int s2, int s3){}
int __builtin_fpx_macsslh(int s1, int s2, int s3){}
int __builtin_fpx_macssll(int s1, int s2, int s3){}
int __builtin_fpx_macsuhh(int s1, int s2, unsigned u3){}
int __builtin_fpx_macsuhl(int s1, int s2, unsigned u3){}
int __builtin_fpx_macsulh(int s1, int s2, unsigned u3){}
int __builtin_fpx_macsull(int s1, int s2, unsigned u3){}
int __builtin_fpx_macushh(int s1, unsigned u2, int s3){}
int __builtin_fpx_macushl(int s1, unsigned u2, int s3){}
int __builtin_fpx_macuslh(int s1, unsigned u2, int s3){}
int __builtin_fpx_macusll(int s1, unsigned u2, int s3){}
int __builtin_fpx_macuuhh(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macuuhl(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macuulh(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_macuull(int s1, unsigned u2, unsigned u3){}
int __builtin_fpx_mac(int s1, int s2, int s3){}



TNT MP voir dans X3 ???
unsigned __builtin_fpx_modu(unsigned u1, unsigned u2){}
int __builtin_fpx_mod(int s1, int s2){}
int __builtin_fpx_mpnsshh(int s1, int s2){}
int __builtin_fpx_mpnsshl(int s1, int s2){}
int __builtin_fpx_mpnsslh(int s1, int s2){}
int __builtin_fpx_mpnssll(int s1, int s2){}
int __builtin_fpx_mpnsuhh(int s1, unsigned u2){}
int __builtin_fpx_mpnsuhl(int s1, unsigned u2){}
int __builtin_fpx_mpnsulh(int s1, unsigned u2){}
int __builtin_fpx_mpnsull(int s1, unsigned u2){}
int __builtin_fpx_mpnushh(unsigned u1, int s2){}
int __builtin_fpx_mpnushl(unsigned u1, int s2){}
int __builtin_fpx_mpnuslh(unsigned u1, int s2){}
int __builtin_fpx_mpnusll(unsigned u1, int s2){}
int __builtin_fpx_mpnuuhh(unsigned u1, unsigned u2){}
int __builtin_fpx_mpnuuhl(unsigned u1, unsigned u2){}
int __builtin_fpx_mpnuulh(unsigned u1, unsigned u2){}
int __builtin_fpx_mpnuull(unsigned u1, unsigned u2){}
int __builtin_fpx_mpn(int s1, int s2){}
int __builtin_fpx_mpsshh(int s1, int s2){}
int __builtin_fpx_mpsshl(int s1, int s2){}
int __builtin_fpx_mpsslh(int s1, int s2){}
int __builtin_fpx_mpssll(int s1, int s2){}
int __builtin_fpx_mpsuhh(int s1, unsigned u2){}
int __builtin_fpx_mpsuhl(int s1, unsigned u2){}
int __builtin_fpx_mpsulh(int s1, unsigned u2){}
int __builtin_fpx_mpsull(int s1, unsigned u2){}
int __builtin_fpx_mpushh(unsigned u1, int s2){}
int __builtin_fpx_mpushl(unsigned u1, int s2){}
int __builtin_fpx_mpuslh(unsigned u1, int s2){}
int __builtin_fpx_mpusll(unsigned u1, int s2){}
int __builtin_fpx_mpuuhh(unsigned u1, unsigned u2){}
int __builtin_fpx_mpuuhl(unsigned u1, unsigned u2){}
int __builtin_fpx_mpuulh(unsigned u1, unsigned u2){}
int __builtin_fpx_mpuull(unsigned u1, unsigned u2){}
int __builtin_fpx_mp(int s1, int s2){}


#endif

