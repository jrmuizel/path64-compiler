/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */


/* Note on dedicated registers: as these registers are not allocated by the compiler,
 * it is necessary to pass the index of such register. When it is passed as builtin
 * operand, an immediate of the form 'u[0..n]' is expected in place of dedicated register.
 * 'n' is the size of the register file minus one.
 * When dedicated register is the result, one additional operand of the same form 'u[0..n]'
 * is placed as first parameter to select the result register index.
 *
 * Note on immediates: some builtins may accept constant value as parameter. Programmer must
 * respect the immediate range given by the bits number and signess.
 * Immediates are given under the form '[su][0-9]+'
 * - [su] for signed and unsigned respectively.
 * - [0-9]+ the bits number associated to the immediate.
 */


/* Builtins for class fpx */
#ifdef __FPX

#ifndef _BUILTIN_FPX_H_
#define _BUILTIN_FPX_H_


/*  divu fpr , gpr , gpr
 * fpr divu( gpr, gpr);
 */
extern unsigned __builtin_fpx_divu(unsigned, unsigned);

/*  div fpr , gpr , gpr
 * fpr div( gpr, gpr);
 */
extern int __builtin_fpx_div(int, int);

/*  fabs fpr , fpr
 * fpr fabs( fpr);
 */
extern float __builtin_fpx_fabs(float);

/*  faddaa fpr , fpr , fpr
 * fpr faddaa( fpr, fpr);
 */
extern float __builtin_fpx_faddaa(float, float);

/*  faddn fpr , fpr , fpr
 * fpr faddn( fpr, fpr);
 */
extern float __builtin_fpx_faddn(float, float);

/*  fadd fpr , fpr , fpr
 * fpr fadd( fpr, fpr);
 */
extern float __builtin_fpx_fadd(float, float);

/*  fasub fpr , fpr , fpr
 * fpr fasub( fpr, fpr);
 */
extern float __builtin_fpx_fasub(float, float);

/*  fclrs <u6>
 * fclrs( <u6>);
 */
extern void __builtin_fpx_fclrs(unsigned short);

/*  fcmpeqn gpr , fpr , fpr
 * gpr fcmpeqn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpeqn(float, float);

/*  fcmpeq gpr , fpr , fpr
 * gpr fcmpeq( fpr, fpr);
 */
extern int __builtin_fpx_fcmpeq(float, float);

/*  fcmpgen gpr , fpr , fpr
 * gpr fcmpgen( fpr, fpr);
 */
extern int __builtin_fpx_fcmpgen(float, float);

/*  fcmpge gpr , fpr , fpr
 * gpr fcmpge( fpr, fpr);
 */
extern int __builtin_fpx_fcmpge(float, float);

/*  fcmpgtn gpr , fpr , fpr
 * gpr fcmpgtn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpgtn(float, float);

/*  fcmpgt gpr , fpr , fpr
 * gpr fcmpgt( fpr, fpr);
 */
extern int __builtin_fpx_fcmpgt(float, float);

/*  fcmplen gpr , fpr , fpr
 * gpr fcmplen( fpr, fpr);
 */
extern int __builtin_fpx_fcmplen(float, float);

/*  fcmple gpr , fpr , fpr
 * gpr fcmple( fpr, fpr);
 */
extern int __builtin_fpx_fcmple(float, float);

/*  fcmpltn gpr , fpr , fpr
 * gpr fcmpltn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpltn(float, float);

/*  fcmplt gpr , fpr , fpr
 * gpr fcmplt( fpr, fpr);
 */
extern int __builtin_fpx_fcmplt(float, float);

/*  fcmpnen gpr , fpr , fpr
 * gpr fcmpnen( fpr, fpr);
 */
extern int __builtin_fpx_fcmpnen(float, float);

/*  fcmpne gpr , fpr , fpr
 * gpr fcmpne( fpr, fpr);
 */
extern int __builtin_fpx_fcmpne(float, float);

/*  fcmpueqn gpr , fpr , fpr
 * gpr fcmpueqn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpueqn(float, float);

/*  fcmpueq gpr , fpr , fpr
 * gpr fcmpueq( fpr, fpr);
 */
extern int __builtin_fpx_fcmpueq(float, float);

/*  fcmpugen gpr , fpr , fpr
 * gpr fcmpugen( fpr, fpr);
 */
extern int __builtin_fpx_fcmpugen(float, float);

/*  fcmpuge gpr , fpr , fpr
 * gpr fcmpuge( fpr, fpr);
 */
extern int __builtin_fpx_fcmpuge(float, float);

/*  fcmpugtn gpr , fpr , fpr
 * gpr fcmpugtn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpugtn(float, float);

/*  fcmpugt gpr , fpr , fpr
 * gpr fcmpugt( fpr, fpr);
 */
extern int __builtin_fpx_fcmpugt(float, float);

/*  fcmpulen gpr , fpr , fpr
 * gpr fcmpulen( fpr, fpr);
 */
extern int __builtin_fpx_fcmpulen(float, float);

/*  fcmpule gpr , fpr , fpr
 * gpr fcmpule( fpr, fpr);
 */
extern int __builtin_fpx_fcmpule(float, float);

/*  fcmpultn gpr , fpr , fpr
 * gpr fcmpultn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpultn(float, float);

/*  fcmpult gpr , fpr , fpr
 * gpr fcmpult( fpr, fpr);
 */
extern int __builtin_fpx_fcmpult(float, float);

/*  fcmpunen gpr , fpr , fpr
 * gpr fcmpunen( fpr, fpr);
 */
extern int __builtin_fpx_fcmpunen(float, float);

/*  fcmpune gpr , fpr , fpr
 * gpr fcmpune( fpr, fpr);
 */
extern int __builtin_fpx_fcmpune(float, float);

/*  fcmpunn gpr , fpr , fpr
 * gpr fcmpunn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpunn(float, float);

/*  fcmpun gpr , fpr , fpr
 * gpr fcmpun( fpr, fpr);
 */
extern int __builtin_fpx_fcmpun(float, float);

/*  fcmpuunn gpr , fpr , fpr
 * gpr fcmpuunn( fpr, fpr);
 */
extern int __builtin_fpx_fcmpuunn(float, float);

/*  fcmpuun gpr , fpr , fpr
 * gpr fcmpuun( fpr, fpr);
 */
extern int __builtin_fpx_fcmpuun(float, float);

/*  fcnst fpr , <u9>
 * fpr fcnst( <u9>);
 */
extern float __builtin_fpx_fcnst(unsigned short);

/*  fdiv fpr , fpr , fpr
 * fpr fdiv( fpr, fpr);
 */
extern float __builtin_fpx_fdiv(float, float);

/*  ff2in gpr , fpr
 * gpr ff2in( fpr);
 */
extern int __builtin_fpx_ff2in_r(float);

/*  ff2in fpr , fpr
 * fpr ff2in( fpr);
 */
extern int __builtin_fpx_ff2in_f(float);

/*  ff2i gpr , fpr
 * gpr ff2i( fpr);
 */
extern int __builtin_fpx_ff2i_r(float);

/*  ff2i fpr , fpr
 * fpr ff2i( fpr);
 */
extern int __builtin_fpx_ff2i_f(float);

/*  ff2un gpr , fpr
 * gpr ff2un( fpr);
 */
extern unsigned __builtin_fpx_ff2un_r(float);

/*  ff2un fpr , fpr
 * fpr ff2un( fpr);
 */
extern unsigned __builtin_fpx_ff2un_f(float);

/*  ff2u gpr , fpr
 * gpr ff2u( fpr);
 */
extern unsigned __builtin_fpx_ff2u_r(float);

/*  ff2u fpr , fpr
 * fpr ff2u( fpr);
 */
extern unsigned __builtin_fpx_ff2u_f(float);

/*  fi2f fpr , gpr
 * fpr fi2f( gpr);
 */
extern float __builtin_fpx_fi2f_r(int);

/*  fi2f fpr , fpr
 * fpr fi2f( fpr);
 */
extern float __builtin_fpx_fi2f_f(int);

/*  fmacn fpr , fpr , fpr
 * fpr fmacn( fpr, fpr, fpr);
 */
extern float __builtin_fpx_fmacn(float, float, float);

/*  fmac fpr , fpr , fpr
 * fpr fmac( fpr, fpr, fpr);
 */
extern float __builtin_fpx_fmac(float, float, float);

/*  fmax fpr , fpr , fpr
 * fpr fmax( fpr, fpr);
 */
extern float __builtin_fpx_fmax(float, float);

/*  fmin fpr , fpr , fpr
 * fpr fmin( fpr, fpr);
 */
extern float __builtin_fpx_fmin(float, float);

/*  fmuln fpr , fpr , fpr
 * fpr fmuln( fpr, fpr);
 */
extern float __builtin_fpx_fmuln(float, float);

/*  fmul fpr , fpr , fpr
 * fpr fmul( fpr, fpr);
 */
extern float __builtin_fpx_fmul(float, float);

/*  fmvf2f fpr , fpr
 * fpr fmvf2f( fpr);
 */
extern float __builtin_fpx_fmvf2f(float);

/*  fmvf2r gpr , fpr
 * gpr fmvf2r( fpr);
 */
extern int __builtin_fpx_fmvf2r(float);

/*  fmvr2f fpr , gpr
 * fpr fmvr2f( gpr);
 */
extern float __builtin_fpx_fmvr2f(int);

/*  fmvr2st gpr
 * fmvr2st( gpr);
 */
extern void __builtin_fpx_fmvr2st(int);

/*  fmvr2s gpr
 * fmvr2s( gpr);
 */
extern void __builtin_fpx_fmvr2s(int);

/*  fmvs2r gpr
 * gpr fmvs2r( );
 */
extern int __builtin_fpx_fmvs2r(void);

/*  fneg fpr , fpr
 * fpr fneg( fpr);
 */
extern float __builtin_fpx_fneg(float);

/*  fscalb fpr , fpr , gpr
 * fpr fscalb( fpr, gpr);
 */
extern float __builtin_fpx_fscalb(float, int);

/*  fsqrt fpr , fpr
 * fpr fsqrt( fpr);
 */
extern float __builtin_fpx_fsqrt(float);

/*  fsub fpr , fpr , fpr
 * fpr fsub( fpr, fpr);
 */
extern float __builtin_fpx_fsub(float, float);

/*  fu2f fpr , gpr
 * fpr fu2f( gpr);
 */
extern float __builtin_fpx_fu2f_r(unsigned);

/*  fu2f fpr , fpr
 * fpr fu2f( fpr);
 */
extern float __builtin_fpx_fu2f_f(unsigned);

/*  macnsshh fpr , gpr , gpr
 * fpr macnsshh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsshh(int, int, int);

/*  macnsshl fpr , gpr , gpr
 * fpr macnsshl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsshl(int, int, int);

/*  macnsslh fpr , gpr , gpr
 * fpr macnsslh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsslh(int, int, int);

/*  macnssll fpr , gpr , gpr
 * fpr macnssll( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnssll(int, int, int);

/*  macnsuhh fpr , gpr , gpr
 * fpr macnsuhh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsuhh(int, int, unsigned);

/*  macnsuhl fpr , gpr , gpr
 * fpr macnsuhl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsuhl(int, int, unsigned);

/*  macnsulh fpr , gpr , gpr
 * fpr macnsulh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsulh(int, int, unsigned);

/*  macnsull fpr , gpr , gpr
 * fpr macnsull( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnsull(int, int, unsigned);

/*  macnushh fpr , gpr , gpr
 * fpr macnushh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnushh(int, unsigned, int);

/*  macnushl fpr , gpr , gpr
 * fpr macnushl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnushl(int, unsigned, int);

/*  macnuslh fpr , gpr , gpr
 * fpr macnuslh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnuslh(int, unsigned, int);

/*  macnusll fpr , gpr , gpr
 * fpr macnusll( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnusll(int, unsigned, int);

/*  macnuuhh fpr , gpr , gpr
 * fpr macnuuhh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnuuhh(int, unsigned, unsigned);

/*  macnuuhl fpr , gpr , gpr
 * fpr macnuuhl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnuuhl(int, unsigned, unsigned);

/*  macnuulh fpr , gpr , gpr
 * fpr macnuulh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnuulh(int, unsigned, unsigned);

/*  macnuull fpr , gpr , gpr
 * fpr macnuull( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macnuull(int, unsigned, unsigned);

/*  macn fpr , gpr , gpr
 * fpr macn( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macn(int, int, int);

/*  macsshh fpr , gpr , gpr
 * fpr macsshh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsshh(int, int, int);

/*  macsshl fpr , gpr , gpr
 * fpr macsshl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsshl(int, int, int);

/*  macsslh fpr , gpr , gpr
 * fpr macsslh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsslh(int, int, int);

/*  macssll fpr , gpr , gpr
 * fpr macssll( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macssll(int, int, int);

/*  macsuhh fpr , gpr , gpr
 * fpr macsuhh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsuhh(int, int, unsigned);

/*  macsuhl fpr , gpr , gpr
 * fpr macsuhl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsuhl(int, int, unsigned);

/*  macsulh fpr , gpr , gpr
 * fpr macsulh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsulh(int, int, unsigned);

/*  macsull fpr , gpr , gpr
 * fpr macsull( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macsull(int, int, unsigned);

/*  macushh fpr , gpr , gpr
 * fpr macushh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macushh(int, unsigned, int);

/*  macushl fpr , gpr , gpr
 * fpr macushl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macushl(int, unsigned, int);

/*  macuslh fpr , gpr , gpr
 * fpr macuslh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macuslh(int, unsigned, int);

/*  macusll fpr , gpr , gpr
 * fpr macusll( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macusll(int, unsigned, int);

/*  macuuhh fpr , gpr , gpr
 * fpr macuuhh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macuuhh(int, unsigned, unsigned);

/*  macuuhl fpr , gpr , gpr
 * fpr macuuhl( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macuuhl(int, unsigned, unsigned);

/*  macuulh fpr , gpr , gpr
 * fpr macuulh( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macuulh(int, unsigned, unsigned);

/*  macuull fpr , gpr , gpr
 * fpr macuull( fpr, gpr, gpr);
 */
extern int __builtin_fpx_macuull(int, unsigned, unsigned);

/*  mac fpr , gpr , gpr
 * fpr mac( fpr, gpr, gpr);
 */
extern int __builtin_fpx_mac(int, int, int);

/*  modu fpr , gpr , gpr
 * fpr modu( gpr, gpr);
 */
extern unsigned __builtin_fpx_modu(unsigned, unsigned);

/*  mod fpr , gpr , gpr
 * fpr mod( gpr, gpr);
 */
extern int __builtin_fpx_mod(int, int);

/*  mpnsshh gpr , gpr , gpr
 * gpr mpnsshh( gpr, gpr);
 */
extern int __builtin_fpx_mpnsshh(int, int);

/*  mpnsshl gpr , gpr , gpr
 * gpr mpnsshl( gpr, gpr);
 */
extern int __builtin_fpx_mpnsshl(int, int);

/*  mpnsslh gpr , gpr , gpr
 * gpr mpnsslh( gpr, gpr);
 */
extern int __builtin_fpx_mpnsslh(int, int);

/*  mpnssll gpr , gpr , gpr
 * gpr mpnssll( gpr, gpr);
 */
extern int __builtin_fpx_mpnssll(int, int);

/*  mpnsuhh gpr , gpr , gpr
 * gpr mpnsuhh( gpr, gpr);
 */
extern int __builtin_fpx_mpnsuhh(int, unsigned);

/*  mpnsuhl gpr , gpr , gpr
 * gpr mpnsuhl( gpr, gpr);
 */
extern int __builtin_fpx_mpnsuhl(int, unsigned);

/*  mpnsulh gpr , gpr , gpr
 * gpr mpnsulh( gpr, gpr);
 */
extern int __builtin_fpx_mpnsulh(int, unsigned);

/*  mpnsull gpr , gpr , gpr
 * gpr mpnsull( gpr, gpr);
 */
extern int __builtin_fpx_mpnsull(int, unsigned);

/*  mpnushh gpr , gpr , gpr
 * gpr mpnushh( gpr, gpr);
 */
extern int __builtin_fpx_mpnushh(unsigned, int);

/*  mpnushl gpr , gpr , gpr
 * gpr mpnushl( gpr, gpr);
 */
extern int __builtin_fpx_mpnushl(unsigned, int);

/*  mpnuslh gpr , gpr , gpr
 * gpr mpnuslh( gpr, gpr);
 */
extern int __builtin_fpx_mpnuslh(unsigned, int);

/*  mpnusll gpr , gpr , gpr
 * gpr mpnusll( gpr, gpr);
 */
extern int __builtin_fpx_mpnusll(unsigned, int);

/*  mpnuuhh gpr , gpr , gpr
 * gpr mpnuuhh( gpr, gpr);
 */
extern int __builtin_fpx_mpnuuhh(unsigned, unsigned);

/*  mpnuuhl gpr , gpr , gpr
 * gpr mpnuuhl( gpr, gpr);
 */
extern int __builtin_fpx_mpnuuhl(unsigned, unsigned);

/*  mpnuulh gpr , gpr , gpr
 * gpr mpnuulh( gpr, gpr);
 */
extern int __builtin_fpx_mpnuulh(unsigned, unsigned);

/*  mpnuull gpr , gpr , gpr
 * gpr mpnuull( gpr, gpr);
 */
extern int __builtin_fpx_mpnuull(unsigned, unsigned);

/*  mpn gpr , gpr , gpr
 * gpr mpn( gpr, gpr);
 */
extern int __builtin_fpx_mpn(int, int);

/*  mpsshh gpr , gpr , gpr
 * gpr mpsshh( gpr, gpr);
 */
extern int __builtin_fpx_mpsshh(int, int);

/*  mpsshl gpr , gpr , gpr
 * gpr mpsshl( gpr, gpr);
 */
extern int __builtin_fpx_mpsshl(int, int);

/*  mpsslh gpr , gpr , gpr
 * gpr mpsslh( gpr, gpr);
 */
extern int __builtin_fpx_mpsslh(int, int);

/*  mpssll gpr , gpr , gpr
 * gpr mpssll( gpr, gpr);
 */
extern int __builtin_fpx_mpssll(int, int);

/*  mpsuhh gpr , gpr , gpr
 * gpr mpsuhh( gpr, gpr);
 */
extern int __builtin_fpx_mpsuhh(int, unsigned);

/*  mpsuhl gpr , gpr , gpr
 * gpr mpsuhl( gpr, gpr);
 */
extern int __builtin_fpx_mpsuhl(int, unsigned);

/*  mpsulh gpr , gpr , gpr
 * gpr mpsulh( gpr, gpr);
 */
extern int __builtin_fpx_mpsulh(int, unsigned);

/*  mpsull gpr , gpr , gpr
 * gpr mpsull( gpr, gpr);
 */
extern int __builtin_fpx_mpsull(int, unsigned);

/*  mpushh gpr , gpr , gpr
 * gpr mpushh( gpr, gpr);
 */
extern int __builtin_fpx_mpushh(unsigned, int);

/*  mpushl gpr , gpr , gpr
 * gpr mpushl( gpr, gpr);
 */
extern int __builtin_fpx_mpushl(unsigned, int);

/*  mpuslh gpr , gpr , gpr
 * gpr mpuslh( gpr, gpr);
 */
extern int __builtin_fpx_mpuslh(unsigned, int);

/*  mpusll gpr , gpr , gpr
 * gpr mpusll( gpr, gpr);
 */
extern int __builtin_fpx_mpusll(unsigned, int);

/*  mpuuhh gpr , gpr , gpr
 * gpr mpuuhh( gpr, gpr);
 */
extern int __builtin_fpx_mpuuhh(unsigned, unsigned);

/*  mpuuhl gpr , gpr , gpr
 * gpr mpuuhl( gpr, gpr);
 */
extern int __builtin_fpx_mpuuhl(unsigned, unsigned);

/*  mpuulh gpr , gpr , gpr
 * gpr mpuulh( gpr, gpr);
 */
extern int __builtin_fpx_mpuulh(unsigned, unsigned);

/*  mpuull gpr , gpr , gpr
 * gpr mpuull( gpr, gpr);
 */
extern int __builtin_fpx_mpuull(unsigned, unsigned);

/*  mp gpr , gpr , gpr
 * gpr mp( gpr, gpr);
 */
extern int __builtin_fpx_mp(int, int);


#endif /* _BUILTIN_FPX_H_ */
#endif /* __FPX */
