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



#ifndef _FPX_H_
#define _FPX_H_

#if defined(__FPX) && !defined(__FPX_C_MODEL)

/* Macros mapped on builtins for class fpx */
#include <builtins_fpx.h>

/*  divu fpr , gpr , gpr
 * fpr divu( gpr, gpr);
 */
#define fpx_divu(a, b, c) { \
  a = __builtin_fpx_divu(b, c); \
}

/*  div fpr , gpr , gpr
 * fpr div( gpr, gpr);
 */
#define fpx_div(a, b, c) { \
  a = __builtin_fpx_div(b, c); \
}

/*  fabs fpr , fpr
 * fpr fabs( fpr);
 */
#define fpx_fabs(a, b) { \
  a = __builtin_fpx_fabs(b); \
}

/*  faddaa fpr , fpr , fpr
 * fpr faddaa( fpr, fpr);
 */
#define fpx_faddaa(a, b, c) { \
  a = __builtin_fpx_faddaa(b, c); \
}

/*  faddn fpr , fpr , fpr
 * fpr faddn( fpr, fpr);
 */
#define fpx_faddn(a, b, c) { \
  a = __builtin_fpx_faddn(b, c); \
}

/*  fadd fpr , fpr , fpr
 * fpr fadd( fpr, fpr);
 */
#define fpx_fadd(a, b, c) { \
  a = __builtin_fpx_fadd(b, c); \
}

/*  fasub fpr , fpr , fpr
 * fpr fasub( fpr, fpr);
 */
#define fpx_fasub(a, b, c) { \
  a = __builtin_fpx_fasub(b, c); \
}

/*  fclrs <u6>
 * fclrs( <u6>);
 */
#define fpx_fclrs(a) { \
  __builtin_fpx_fclrs(a); \
}

/*  fcmpeqn gpr , fpr , fpr
 * gpr fcmpeqn( fpr, fpr);
 */
#define fpx_fcmpeqn(a, b, c) { \
  a = __builtin_fpx_fcmpeqn(b, c); \
}

/*  fcmpeq gpr , fpr , fpr
 * gpr fcmpeq( fpr, fpr);
 */
#define fpx_fcmpeq(a, b, c) { \
  a = __builtin_fpx_fcmpeq(b, c); \
}

/*  fcmpgen gpr , fpr , fpr
 * gpr fcmpgen( fpr, fpr);
 */
#define fpx_fcmpgen(a, b, c) { \
  a = __builtin_fpx_fcmpgen(b, c); \
}

/*  fcmpge gpr , fpr , fpr
 * gpr fcmpge( fpr, fpr);
 */
#define fpx_fcmpge(a, b, c) { \
  a = __builtin_fpx_fcmpge(b, c); \
}

/*  fcmpgtn gpr , fpr , fpr
 * gpr fcmpgtn( fpr, fpr);
 */
#define fpx_fcmpgtn(a, b, c) { \
  a = __builtin_fpx_fcmpgtn(b, c); \
}

/*  fcmpgt gpr , fpr , fpr
 * gpr fcmpgt( fpr, fpr);
 */
#define fpx_fcmpgt(a, b, c) { \
  a = __builtin_fpx_fcmpgt(b, c); \
}

/*  fcmplen gpr , fpr , fpr
 * gpr fcmplen( fpr, fpr);
 */
#define fpx_fcmplen(a, b, c) { \
  a = __builtin_fpx_fcmplen(b, c); \
}

/*  fcmple gpr , fpr , fpr
 * gpr fcmple( fpr, fpr);
 */
#define fpx_fcmple(a, b, c) { \
  a = __builtin_fpx_fcmple(b, c); \
}

/*  fcmpltn gpr , fpr , fpr
 * gpr fcmpltn( fpr, fpr);
 */
#define fpx_fcmpltn(a, b, c) { \
  a = __builtin_fpx_fcmpltn(b, c); \
}

/*  fcmplt gpr , fpr , fpr
 * gpr fcmplt( fpr, fpr);
 */
#define fpx_fcmplt(a, b, c) { \
  a = __builtin_fpx_fcmplt(b, c); \
}

/*  fcmpnen gpr , fpr , fpr
 * gpr fcmpnen( fpr, fpr);
 */
#define fpx_fcmpnen(a, b, c) { \
  a = __builtin_fpx_fcmpnen(b, c); \
}

/*  fcmpne gpr , fpr , fpr
 * gpr fcmpne( fpr, fpr);
 */
#define fpx_fcmpne(a, b, c) { \
  a = __builtin_fpx_fcmpne(b, c); \
}

/*  fcmpueqn gpr , fpr , fpr
 * gpr fcmpueqn( fpr, fpr);
 */
#define fpx_fcmpueqn(a, b, c) { \
  a = __builtin_fpx_fcmpueqn(b, c); \
}

/*  fcmpueq gpr , fpr , fpr
 * gpr fcmpueq( fpr, fpr);
 */
#define fpx_fcmpueq(a, b, c) { \
  a = __builtin_fpx_fcmpueq(b, c); \
}

/*  fcmpugen gpr , fpr , fpr
 * gpr fcmpugen( fpr, fpr);
 */
#define fpx_fcmpugen(a, b, c) { \
  a = __builtin_fpx_fcmpugen(b, c); \
}

/*  fcmpuge gpr , fpr , fpr
 * gpr fcmpuge( fpr, fpr);
 */
#define fpx_fcmpuge(a, b, c) { \
  a = __builtin_fpx_fcmpuge(b, c); \
}

/*  fcmpugtn gpr , fpr , fpr
 * gpr fcmpugtn( fpr, fpr);
 */
#define fpx_fcmpugtn(a, b, c) { \
  a = __builtin_fpx_fcmpugtn(b, c); \
}

/*  fcmpugt gpr , fpr , fpr
 * gpr fcmpugt( fpr, fpr);
 */
#define fpx_fcmpugt(a, b, c) { \
  a = __builtin_fpx_fcmpugt(b, c); \
}

/*  fcmpulen gpr , fpr , fpr
 * gpr fcmpulen( fpr, fpr);
 */
#define fpx_fcmpulen(a, b, c) { \
  a = __builtin_fpx_fcmpulen(b, c); \
}

/*  fcmpule gpr , fpr , fpr
 * gpr fcmpule( fpr, fpr);
 */
#define fpx_fcmpule(a, b, c) { \
  a = __builtin_fpx_fcmpule(b, c); \
}

/*  fcmpultn gpr , fpr , fpr
 * gpr fcmpultn( fpr, fpr);
 */
#define fpx_fcmpultn(a, b, c) { \
  a = __builtin_fpx_fcmpultn(b, c); \
}

/*  fcmpult gpr , fpr , fpr
 * gpr fcmpult( fpr, fpr);
 */
#define fpx_fcmpult(a, b, c) { \
  a = __builtin_fpx_fcmpult(b, c); \
}

/*  fcmpunen gpr , fpr , fpr
 * gpr fcmpunen( fpr, fpr);
 */
#define fpx_fcmpunen(a, b, c) { \
  a = __builtin_fpx_fcmpunen(b, c); \
}

/*  fcmpune gpr , fpr , fpr
 * gpr fcmpune( fpr, fpr);
 */
#define fpx_fcmpune(a, b, c) { \
  a = __builtin_fpx_fcmpune(b, c); \
}

/*  fcmpunn gpr , fpr , fpr
 * gpr fcmpunn( fpr, fpr);
 */
#define fpx_fcmpunn(a, b, c) { \
  a = __builtin_fpx_fcmpunn(b, c); \
}

/*  fcmpun gpr , fpr , fpr
 * gpr fcmpun( fpr, fpr);
 */
#define fpx_fcmpun(a, b, c) { \
  a = __builtin_fpx_fcmpun(b, c); \
}

/*  fcmpuunn gpr , fpr , fpr
 * gpr fcmpuunn( fpr, fpr);
 */
#define fpx_fcmpuunn(a, b, c) { \
  a = __builtin_fpx_fcmpuunn(b, c); \
}

/*  fcmpuun gpr , fpr , fpr
 * gpr fcmpuun( fpr, fpr);
 */
#define fpx_fcmpuun(a, b, c) { \
  a = __builtin_fpx_fcmpuun(b, c); \
}

/*  fcnst fpr , <u9>
 * fpr fcnst( <u9>);
 */
#define fpx_fcnst(a, b) { \
  a = __builtin_fpx_fcnst(b); \
}

/*  fdiv fpr , fpr , fpr
 * fpr fdiv( fpr, fpr);
 */
#define fpx_fdiv(a, b, c) { \
  a = __builtin_fpx_fdiv(b, c); \
}

/*  ff2in gpr , fpr
 * gpr ff2in( fpr);
 */
#define fpx_ff2in_r(a, b) { \
  a = __builtin_fpx_ff2in_r(b); \
}

/*  ff2in fpr , fpr
 * fpr ff2in( fpr);
 */
#define fpx_ff2in_f(a, b) { \
  a = __builtin_fpx_ff2in_f(b); \
}

/*  ff2i gpr , fpr
 * gpr ff2i( fpr);
 */
#define fpx_ff2i_r(a, b) { \
  a = __builtin_fpx_ff2i_r(b); \
}

/*  ff2i fpr , fpr
 * fpr ff2i( fpr);
 */
#define fpx_ff2i_f(a, b) { \
  a = __builtin_fpx_ff2i_f(b); \
}

/*  ff2un gpr , fpr
 * gpr ff2un( fpr);
 */
#define fpx_ff2un_r(a, b) { \
  a = __builtin_fpx_ff2un_r(b); \
}

/*  ff2un fpr , fpr
 * fpr ff2un( fpr);
 */
#define fpx_ff2un_f(a, b) { \
  a = __builtin_fpx_ff2un_f(b); \
}

/*  ff2u gpr , fpr
 * gpr ff2u( fpr);
 */
#define fpx_ff2u_r(a, b) { \
  a = __builtin_fpx_ff2u_r(b); \
}

/*  ff2u fpr , fpr
 * fpr ff2u( fpr);
 */
#define fpx_ff2u_f(a, b) { \
  a = __builtin_fpx_ff2u_f(b); \
}

/*  fi2f fpr , gpr
 * fpr fi2f( gpr);
 */
#define fpx_fi2f_r(a, b) { \
  a = __builtin_fpx_fi2f_r(b); \
}

/*  fi2f fpr , fpr
 * fpr fi2f( fpr);
 */
#define fpx_fi2f_f(a, b) { \
  a = __builtin_fpx_fi2f_f(b); \
}

/*  fmacn fpr , fpr , fpr
 * fpr fmacn( fpr, fpr, fpr);
 */
#define fpx_fmacn(a, b, c) { \
  a = __builtin_fpx_fmacn(a, b, c); \
}

/*  fmac fpr , fpr , fpr
 * fpr fmac( fpr, fpr, fpr);
 */
#define fpx_fmac(a, b, c) { \
  a = __builtin_fpx_fmac(a, b, c); \
}

/*  fmax fpr , fpr , fpr
 * fpr fmax( fpr, fpr);
 */
#define fpx_fmax(a, b, c) { \
  a = __builtin_fpx_fmax(b, c); \
}

/*  fmin fpr , fpr , fpr
 * fpr fmin( fpr, fpr);
 */
#define fpx_fmin(a, b, c) { \
  a = __builtin_fpx_fmin(b, c); \
}

/*  fmuln fpr , fpr , fpr
 * fpr fmuln( fpr, fpr);
 */
#define fpx_fmuln(a, b, c) { \
  a = __builtin_fpx_fmuln(b, c); \
}

/*  fmul fpr , fpr , fpr
 * fpr fmul( fpr, fpr);
 */
#define fpx_fmul(a, b, c) { \
  a = __builtin_fpx_fmul(b, c); \
}

/*  fmvf2f fpr , fpr
 * fpr fmvf2f( fpr);
 */
#define fpx_fmvf2f(a, b) { \
  a = __builtin_fpx_fmvf2f(b); \
}

/*  fmvf2r gpr , fpr
 * gpr fmvf2r( fpr);
 */
#define fpx_fmvf2r(a, b) { \
  a = __builtin_fpx_fmvf2r(b); \
}

/*  fmvr2f fpr , gpr
 * fpr fmvr2f( gpr);
 */
#define fpx_fmvr2f(a, b) { \
  a = __builtin_fpx_fmvr2f(b); \
}

/*  fmvr2st gpr
 * fmvr2st( gpr);
 */
#define fpx_fmvr2st(a) { \
  __builtin_fpx_fmvr2st(a); \
}

/*  fmvr2s gpr
 * fmvr2s( gpr);
 */
#define fpx_fmvr2s(a) { \
  __builtin_fpx_fmvr2s(a); \
}

/*  fmvs2r gpr
 * gpr fmvs2r( );
 */
#define fpx_fmvs2r(a) { \
  a = __builtin_fpx_fmvs2r(); \
}

/*  fneg fpr , fpr
 * fpr fneg( fpr);
 */
#define fpx_fneg(a, b) { \
  a = __builtin_fpx_fneg(b); \
}

/*  fscalb fpr , fpr , gpr
 * fpr fscalb( fpr, gpr);
 */
#define fpx_fscalb(a, b, c) { \
  a = __builtin_fpx_fscalb(b, c); \
}

/*  fsqrt fpr , fpr
 * fpr fsqrt( fpr);
 */
#define fpx_fsqrt(a, b) { \
  a = __builtin_fpx_fsqrt(b); \
}

/*  fsub fpr , fpr , fpr
 * fpr fsub( fpr, fpr);
 */
#define fpx_fsub(a, b, c) { \
  a = __builtin_fpx_fsub(b, c); \
}

/*  fu2f fpr , gpr
 * fpr fu2f( gpr);
 */
#define fpx_fu2f_r(a, b) { \
  a = __builtin_fpx_fu2f_r(b); \
}

/*  fu2f fpr , fpr
 * fpr fu2f( fpr);
 */
#define fpx_fu2f_f(a, b) { \
  a = __builtin_fpx_fu2f_f(b); \
}

/*  macnsshh fpr , gpr , gpr
 * fpr macnsshh( fpr, gpr, gpr);
 */
#define fpx_macnsshh(a, b, c) { \
  a = __builtin_fpx_macnsshh(a, b, c); \
}

/*  macnsshl fpr , gpr , gpr
 * fpr macnsshl( fpr, gpr, gpr);
 */
#define fpx_macnsshl(a, b, c) { \
  a = __builtin_fpx_macnsshl(a, b, c); \
}

/*  macnsslh fpr , gpr , gpr
 * fpr macnsslh( fpr, gpr, gpr);
 */
#define fpx_macnsslh(a, b, c) { \
  a = __builtin_fpx_macnsslh(a, b, c); \
}

/*  macnssll fpr , gpr , gpr
 * fpr macnssll( fpr, gpr, gpr);
 */
#define fpx_macnssll(a, b, c) { \
  a = __builtin_fpx_macnssll(a, b, c); \
}

/*  macnsuhh fpr , gpr , gpr
 * fpr macnsuhh( fpr, gpr, gpr);
 */
#define fpx_macnsuhh(a, b, c) { \
  a = __builtin_fpx_macnsuhh(a, b, c); \
}

/*  macnsuhl fpr , gpr , gpr
 * fpr macnsuhl( fpr, gpr, gpr);
 */
#define fpx_macnsuhl(a, b, c) { \
  a = __builtin_fpx_macnsuhl(a, b, c); \
}

/*  macnsulh fpr , gpr , gpr
 * fpr macnsulh( fpr, gpr, gpr);
 */
#define fpx_macnsulh(a, b, c) { \
  a = __builtin_fpx_macnsulh(a, b, c); \
}

/*  macnsull fpr , gpr , gpr
 * fpr macnsull( fpr, gpr, gpr);
 */
#define fpx_macnsull(a, b, c) { \
  a = __builtin_fpx_macnsull(a, b, c); \
}

/*  macnushh fpr , gpr , gpr
 * fpr macnushh( fpr, gpr, gpr);
 */
#define fpx_macnushh(a, b, c) { \
  a = __builtin_fpx_macnushh(a, b, c); \
}

/*  macnushl fpr , gpr , gpr
 * fpr macnushl( fpr, gpr, gpr);
 */
#define fpx_macnushl(a, b, c) { \
  a = __builtin_fpx_macnushl(a, b, c); \
}

/*  macnuslh fpr , gpr , gpr
 * fpr macnuslh( fpr, gpr, gpr);
 */
#define fpx_macnuslh(a, b, c) { \
  a = __builtin_fpx_macnuslh(a, b, c); \
}

/*  macnusll fpr , gpr , gpr
 * fpr macnusll( fpr, gpr, gpr);
 */
#define fpx_macnusll(a, b, c) { \
  a = __builtin_fpx_macnusll(a, b, c); \
}

/*  macnuuhh fpr , gpr , gpr
 * fpr macnuuhh( fpr, gpr, gpr);
 */
#define fpx_macnuuhh(a, b, c) { \
  a = __builtin_fpx_macnuuhh(a, b, c); \
}

/*  macnuuhl fpr , gpr , gpr
 * fpr macnuuhl( fpr, gpr, gpr);
 */
#define fpx_macnuuhl(a, b, c) { \
  a = __builtin_fpx_macnuuhl(a, b, c); \
}

/*  macnuulh fpr , gpr , gpr
 * fpr macnuulh( fpr, gpr, gpr);
 */
#define fpx_macnuulh(a, b, c) { \
  a = __builtin_fpx_macnuulh(a, b, c); \
}

/*  macnuull fpr , gpr , gpr
 * fpr macnuull( fpr, gpr, gpr);
 */
#define fpx_macnuull(a, b, c) { \
  a = __builtin_fpx_macnuull(a, b, c); \
}

/*  macn fpr , gpr , gpr
 * fpr macn( fpr, gpr, gpr);
 */
#define fpx_macn(a, b, c) { \
  a = __builtin_fpx_macn(a, b, c); \
}

/*  macsshh fpr , gpr , gpr
 * fpr macsshh( fpr, gpr, gpr);
 */
#define fpx_macsshh(a, b, c) { \
  a = __builtin_fpx_macsshh(a, b, c); \
}

/*  macsshl fpr , gpr , gpr
 * fpr macsshl( fpr, gpr, gpr);
 */
#define fpx_macsshl(a, b, c) { \
  a = __builtin_fpx_macsshl(a, b, c); \
}

/*  macsslh fpr , gpr , gpr
 * fpr macsslh( fpr, gpr, gpr);
 */
#define fpx_macsslh(a, b, c) { \
  a = __builtin_fpx_macsslh(a, b, c); \
}

/*  macssll fpr , gpr , gpr
 * fpr macssll( fpr, gpr, gpr);
 */
#define fpx_macssll(a, b, c) { \
  a = __builtin_fpx_macssll(a, b, c); \
}

/*  macsuhh fpr , gpr , gpr
 * fpr macsuhh( fpr, gpr, gpr);
 */
#define fpx_macsuhh(a, b, c) { \
  a = __builtin_fpx_macsuhh(a, b, c); \
}

/*  macsuhl fpr , gpr , gpr
 * fpr macsuhl( fpr, gpr, gpr);
 */
#define fpx_macsuhl(a, b, c) { \
  a = __builtin_fpx_macsuhl(a, b, c); \
}

/*  macsulh fpr , gpr , gpr
 * fpr macsulh( fpr, gpr, gpr);
 */
#define fpx_macsulh(a, b, c) { \
  a = __builtin_fpx_macsulh(a, b, c); \
}

/*  macsull fpr , gpr , gpr
 * fpr macsull( fpr, gpr, gpr);
 */
#define fpx_macsull(a, b, c) { \
  a = __builtin_fpx_macsull(a, b, c); \
}

/*  macushh fpr , gpr , gpr
 * fpr macushh( fpr, gpr, gpr);
 */
#define fpx_macushh(a, b, c) { \
  a = __builtin_fpx_macushh(a, b, c); \
}

/*  macushl fpr , gpr , gpr
 * fpr macushl( fpr, gpr, gpr);
 */
#define fpx_macushl(a, b, c) { \
  a = __builtin_fpx_macushl(a, b, c); \
}

/*  macuslh fpr , gpr , gpr
 * fpr macuslh( fpr, gpr, gpr);
 */
#define fpx_macuslh(a, b, c) { \
  a = __builtin_fpx_macuslh(a, b, c); \
}

/*  macusll fpr , gpr , gpr
 * fpr macusll( fpr, gpr, gpr);
 */
#define fpx_macusll(a, b, c) { \
  a = __builtin_fpx_macusll(a, b, c); \
}

/*  macuuhh fpr , gpr , gpr
 * fpr macuuhh( fpr, gpr, gpr);
 */
#define fpx_macuuhh(a, b, c) { \
  a = __builtin_fpx_macuuhh(a, b, c); \
}

/*  macuuhl fpr , gpr , gpr
 * fpr macuuhl( fpr, gpr, gpr);
 */
#define fpx_macuuhl(a, b, c) { \
  a = __builtin_fpx_macuuhl(a, b, c); \
}

/*  macuulh fpr , gpr , gpr
 * fpr macuulh( fpr, gpr, gpr);
 */
#define fpx_macuulh(a, b, c) { \
  a = __builtin_fpx_macuulh(a, b, c); \
}

/*  macuull fpr , gpr , gpr
 * fpr macuull( fpr, gpr, gpr);
 */
#define fpx_macuull(a, b, c) { \
  a = __builtin_fpx_macuull(a, b, c); \
}

/*  mac fpr , gpr , gpr
 * fpr mac( fpr, gpr, gpr);
 */
#define fpx_mac(a, b, c) { \
  a = __builtin_fpx_mac(a, b, c); \
}

/*  modu fpr , gpr , gpr
 * fpr modu( gpr, gpr);
 */
#define fpx_modu(a, b, c) { \
  a = __builtin_fpx_modu(b, c); \
}

/*  mod fpr , gpr , gpr
 * fpr mod( gpr, gpr);
 */
#define fpx_mod(a, b, c) { \
  a = __builtin_fpx_mod(b, c); \
}

/*  mpnsshh gpr , gpr , gpr
 * gpr mpnsshh( gpr, gpr);
 */
#define fpx_mpnsshh(a, b, c) { \
  a = __builtin_fpx_mpnsshh(b, c); \
}

/*  mpnsshl gpr , gpr , gpr
 * gpr mpnsshl( gpr, gpr);
 */
#define fpx_mpnsshl(a, b, c) { \
  a = __builtin_fpx_mpnsshl(b, c); \
}

/*  mpnsslh gpr , gpr , gpr
 * gpr mpnsslh( gpr, gpr);
 */
#define fpx_mpnsslh(a, b, c) { \
  a = __builtin_fpx_mpnsslh(b, c); \
}

/*  mpnssll gpr , gpr , gpr
 * gpr mpnssll( gpr, gpr);
 */
#define fpx_mpnssll(a, b, c) { \
  a = __builtin_fpx_mpnssll(b, c); \
}

/*  mpnsuhh gpr , gpr , gpr
 * gpr mpnsuhh( gpr, gpr);
 */
#define fpx_mpnsuhh(a, b, c) { \
  a = __builtin_fpx_mpnsuhh(b, c); \
}

/*  mpnsuhl gpr , gpr , gpr
 * gpr mpnsuhl( gpr, gpr);
 */
#define fpx_mpnsuhl(a, b, c) { \
  a = __builtin_fpx_mpnsuhl(b, c); \
}

/*  mpnsulh gpr , gpr , gpr
 * gpr mpnsulh( gpr, gpr);
 */
#define fpx_mpnsulh(a, b, c) { \
  a = __builtin_fpx_mpnsulh(b, c); \
}

/*  mpnsull gpr , gpr , gpr
 * gpr mpnsull( gpr, gpr);
 */
#define fpx_mpnsull(a, b, c) { \
  a = __builtin_fpx_mpnsull(b, c); \
}

/*  mpnushh gpr , gpr , gpr
 * gpr mpnushh( gpr, gpr);
 */
#define fpx_mpnushh(a, b, c) { \
  a = __builtin_fpx_mpnushh(b, c); \
}

/*  mpnushl gpr , gpr , gpr
 * gpr mpnushl( gpr, gpr);
 */
#define fpx_mpnushl(a, b, c) { \
  a = __builtin_fpx_mpnushl(b, c); \
}

/*  mpnuslh gpr , gpr , gpr
 * gpr mpnuslh( gpr, gpr);
 */
#define fpx_mpnuslh(a, b, c) { \
  a = __builtin_fpx_mpnuslh(b, c); \
}

/*  mpnusll gpr , gpr , gpr
 * gpr mpnusll( gpr, gpr);
 */
#define fpx_mpnusll(a, b, c) { \
  a = __builtin_fpx_mpnusll(b, c); \
}

/*  mpnuuhh gpr , gpr , gpr
 * gpr mpnuuhh( gpr, gpr);
 */
#define fpx_mpnuuhh(a, b, c) { \
  a = __builtin_fpx_mpnuuhh(b, c); \
}

/*  mpnuuhl gpr , gpr , gpr
 * gpr mpnuuhl( gpr, gpr);
 */
#define fpx_mpnuuhl(a, b, c) { \
  a = __builtin_fpx_mpnuuhl(b, c); \
}

/*  mpnuulh gpr , gpr , gpr
 * gpr mpnuulh( gpr, gpr);
 */
#define fpx_mpnuulh(a, b, c) { \
  a = __builtin_fpx_mpnuulh(b, c); \
}

/*  mpnuull gpr , gpr , gpr
 * gpr mpnuull( gpr, gpr);
 */
#define fpx_mpnuull(a, b, c) { \
  a = __builtin_fpx_mpnuull(b, c); \
}

/*  mpn gpr , gpr , gpr
 * gpr mpn( gpr, gpr);
 */
#define fpx_mpn(a, b, c) { \
  a = __builtin_fpx_mpn(b, c); \
}

/*  mpsshh gpr , gpr , gpr
 * gpr mpsshh( gpr, gpr);
 */
#define fpx_mpsshh(a, b, c) { \
  a = __builtin_fpx_mpsshh(b, c); \
}

/*  mpsshl gpr , gpr , gpr
 * gpr mpsshl( gpr, gpr);
 */
#define fpx_mpsshl(a, b, c) { \
  a = __builtin_fpx_mpsshl(b, c); \
}

/*  mpsslh gpr , gpr , gpr
 * gpr mpsslh( gpr, gpr);
 */
#define fpx_mpsslh(a, b, c) { \
  a = __builtin_fpx_mpsslh(b, c); \
}

/*  mpssll gpr , gpr , gpr
 * gpr mpssll( gpr, gpr);
 */
#define fpx_mpssll(a, b, c) { \
  a = __builtin_fpx_mpssll(b, c); \
}

/*  mpsuhh gpr , gpr , gpr
 * gpr mpsuhh( gpr, gpr);
 */
#define fpx_mpsuhh(a, b, c) { \
  a = __builtin_fpx_mpsuhh(b, c); \
}

/*  mpsuhl gpr , gpr , gpr
 * gpr mpsuhl( gpr, gpr);
 */
#define fpx_mpsuhl(a, b, c) { \
  a = __builtin_fpx_mpsuhl(b, c); \
}

/*  mpsulh gpr , gpr , gpr
 * gpr mpsulh( gpr, gpr);
 */
#define fpx_mpsulh(a, b, c) { \
  a = __builtin_fpx_mpsulh(b, c); \
}

/*  mpsull gpr , gpr , gpr
 * gpr mpsull( gpr, gpr);
 */
#define fpx_mpsull(a, b, c) { \
  a = __builtin_fpx_mpsull(b, c); \
}

/*  mpushh gpr , gpr , gpr
 * gpr mpushh( gpr, gpr);
 */
#define fpx_mpushh(a, b, c) { \
  a = __builtin_fpx_mpushh(b, c); \
}

/*  mpushl gpr , gpr , gpr
 * gpr mpushl( gpr, gpr);
 */
#define fpx_mpushl(a, b, c) { \
  a = __builtin_fpx_mpushl(b, c); \
}

/*  mpuslh gpr , gpr , gpr
 * gpr mpuslh( gpr, gpr);
 */
#define fpx_mpuslh(a, b, c) { \
  a = __builtin_fpx_mpuslh(b, c); \
}

/*  mpusll gpr , gpr , gpr
 * gpr mpusll( gpr, gpr);
 */
#define fpx_mpusll(a, b, c) { \
  a = __builtin_fpx_mpusll(b, c); \
}

/*  mpuuhh gpr , gpr , gpr
 * gpr mpuuhh( gpr, gpr);
 */
#define fpx_mpuuhh(a, b, c) { \
  a = __builtin_fpx_mpuuhh(b, c); \
}

/*  mpuuhl gpr , gpr , gpr
 * gpr mpuuhl( gpr, gpr);
 */
#define fpx_mpuuhl(a, b, c) { \
  a = __builtin_fpx_mpuuhl(b, c); \
}

/*  mpuulh gpr , gpr , gpr
 * gpr mpuulh( gpr, gpr);
 */
#define fpx_mpuulh(a, b, c) { \
  a = __builtin_fpx_mpuulh(b, c); \
}

/*  mpuull gpr , gpr , gpr
 * gpr mpuull( gpr, gpr);
 */
#define fpx_mpuull(a, b, c) { \
  a = __builtin_fpx_mpuull(b, c); \
}

/*  mp gpr , gpr , gpr
 * gpr mp( gpr, gpr);
 */
#define fpx_mp(a, b, c) { \
  a = __builtin_fpx_mp(b, c); \
}

#else /* !defined(__FPX) || defined(__FPX_C_MODEL) */

/* Macros mapped on C models for class fpx */
#error "FPX models are not yet available"

#if 0

#include <builtins_model_fpx.h>

/*  divu fpr , gpr , gpr
 * fpr divu( gpr, gpr);
 */
#define fpx_divu(a, b, c) { \
  a = __cmodel_fpx_divu(b, c); \
}

/*  div fpr , gpr , gpr
 * fpr div( gpr, gpr);
 */
#define fpx_div(a, b, c) { \
  a = __cmodel_fpx_div(b, c); \
}

/*  fabs fpr , fpr
 * fpr fabs( fpr);
 */
#define fpx_fabs(a, b) { \
  a = __cmodel_fpx_fabs(b); \
}

/*  faddaa fpr , fpr , fpr
 * fpr faddaa( fpr, fpr);
 */
#define fpx_faddaa(a, b, c) { \
  a = __cmodel_fpx_faddaa(b, c); \
}

/*  faddn fpr , fpr , fpr
 * fpr faddn( fpr, fpr);
 */
#define fpx_faddn(a, b, c) { \
  a = __cmodel_fpx_faddn(b, c); \
}

/*  fadd fpr , fpr , fpr
 * fpr fadd( fpr, fpr);
 */
#define fpx_fadd(a, b, c) { \
  a = __cmodel_fpx_fadd(b, c); \
}

/*  fasub fpr , fpr , fpr
 * fpr fasub( fpr, fpr);
 */
#define fpx_fasub(a, b, c) { \
  a = __cmodel_fpx_fasub(b, c); \
}

/*  fclrs <u6>
 * fclrs( <u6>);
 */
#define fpx_fclrs(a) { \
  __cmodel_fpx_fclrs(a); \
}

/*  fcmpeqn gpr , fpr , fpr
 * gpr fcmpeqn( fpr, fpr);
 */
#define fpx_fcmpeqn(a, b, c) { \
  a = __cmodel_fpx_fcmpeqn(b, c); \
}

/*  fcmpeq gpr , fpr , fpr
 * gpr fcmpeq( fpr, fpr);
 */
#define fpx_fcmpeq(a, b, c) { \
  a = __cmodel_fpx_fcmpeq(b, c); \
}

/*  fcmpgen gpr , fpr , fpr
 * gpr fcmpgen( fpr, fpr);
 */
#define fpx_fcmpgen(a, b, c) { \
  a = __cmodel_fpx_fcmpgen(b, c); \
}

/*  fcmpge gpr , fpr , fpr
 * gpr fcmpge( fpr, fpr);
 */
#define fpx_fcmpge(a, b, c) { \
  a = __cmodel_fpx_fcmpge(b, c); \
}

/*  fcmpgtn gpr , fpr , fpr
 * gpr fcmpgtn( fpr, fpr);
 */
#define fpx_fcmpgtn(a, b, c) { \
  a = __cmodel_fpx_fcmpgtn(b, c); \
}

/*  fcmpgt gpr , fpr , fpr
 * gpr fcmpgt( fpr, fpr);
 */
#define fpx_fcmpgt(a, b, c) { \
  a = __cmodel_fpx_fcmpgt(b, c); \
}

/*  fcmplen gpr , fpr , fpr
 * gpr fcmplen( fpr, fpr);
 */
#define fpx_fcmplen(a, b, c) { \
  a = __cmodel_fpx_fcmplen(b, c); \
}

/*  fcmple gpr , fpr , fpr
 * gpr fcmple( fpr, fpr);
 */
#define fpx_fcmple(a, b, c) { \
  a = __cmodel_fpx_fcmple(b, c); \
}

/*  fcmpltn gpr , fpr , fpr
 * gpr fcmpltn( fpr, fpr);
 */
#define fpx_fcmpltn(a, b, c) { \
  a = __cmodel_fpx_fcmpltn(b, c); \
}

/*  fcmplt gpr , fpr , fpr
 * gpr fcmplt( fpr, fpr);
 */
#define fpx_fcmplt(a, b, c) { \
  a = __cmodel_fpx_fcmplt(b, c); \
}

/*  fcmpnen gpr , fpr , fpr
 * gpr fcmpnen( fpr, fpr);
 */
#define fpx_fcmpnen(a, b, c) { \
  a = __cmodel_fpx_fcmpnen(b, c); \
}

/*  fcmpne gpr , fpr , fpr
 * gpr fcmpne( fpr, fpr);
 */
#define fpx_fcmpne(a, b, c) { \
  a = __cmodel_fpx_fcmpne(b, c); \
}

/*  fcmpueqn gpr , fpr , fpr
 * gpr fcmpueqn( fpr, fpr);
 */
#define fpx_fcmpueqn(a, b, c) { \
  a = __cmodel_fpx_fcmpueqn(b, c); \
}

/*  fcmpueq gpr , fpr , fpr
 * gpr fcmpueq( fpr, fpr);
 */
#define fpx_fcmpueq(a, b, c) { \
  a = __cmodel_fpx_fcmpueq(b, c); \
}

/*  fcmpugen gpr , fpr , fpr
 * gpr fcmpugen( fpr, fpr);
 */
#define fpx_fcmpugen(a, b, c) { \
  a = __cmodel_fpx_fcmpugen(b, c); \
}

/*  fcmpuge gpr , fpr , fpr
 * gpr fcmpuge( fpr, fpr);
 */
#define fpx_fcmpuge(a, b, c) { \
  a = __cmodel_fpx_fcmpuge(b, c); \
}

/*  fcmpugtn gpr , fpr , fpr
 * gpr fcmpugtn( fpr, fpr);
 */
#define fpx_fcmpugtn(a, b, c) { \
  a = __cmodel_fpx_fcmpugtn(b, c); \
}

/*  fcmpugt gpr , fpr , fpr
 * gpr fcmpugt( fpr, fpr);
 */
#define fpx_fcmpugt(a, b, c) { \
  a = __cmodel_fpx_fcmpugt(b, c); \
}

/*  fcmpulen gpr , fpr , fpr
 * gpr fcmpulen( fpr, fpr);
 */
#define fpx_fcmpulen(a, b, c) { \
  a = __cmodel_fpx_fcmpulen(b, c); \
}

/*  fcmpule gpr , fpr , fpr
 * gpr fcmpule( fpr, fpr);
 */
#define fpx_fcmpule(a, b, c) { \
  a = __cmodel_fpx_fcmpule(b, c); \
}

/*  fcmpultn gpr , fpr , fpr
 * gpr fcmpultn( fpr, fpr);
 */
#define fpx_fcmpultn(a, b, c) { \
  a = __cmodel_fpx_fcmpultn(b, c); \
}

/*  fcmpult gpr , fpr , fpr
 * gpr fcmpult( fpr, fpr);
 */
#define fpx_fcmpult(a, b, c) { \
  a = __cmodel_fpx_fcmpult(b, c); \
}

/*  fcmpunen gpr , fpr , fpr
 * gpr fcmpunen( fpr, fpr);
 */
#define fpx_fcmpunen(a, b, c) { \
  a = __cmodel_fpx_fcmpunen(b, c); \
}

/*  fcmpune gpr , fpr , fpr
 * gpr fcmpune( fpr, fpr);
 */
#define fpx_fcmpune(a, b, c) { \
  a = __cmodel_fpx_fcmpune(b, c); \
}

/*  fcmpunn gpr , fpr , fpr
 * gpr fcmpunn( fpr, fpr);
 */
#define fpx_fcmpunn(a, b, c) { \
  a = __cmodel_fpx_fcmpunn(b, c); \
}

/*  fcmpun gpr , fpr , fpr
 * gpr fcmpun( fpr, fpr);
 */
#define fpx_fcmpun(a, b, c) { \
  a = __cmodel_fpx_fcmpun(b, c); \
}

/*  fcmpuunn gpr , fpr , fpr
 * gpr fcmpuunn( fpr, fpr);
 */
#define fpx_fcmpuunn(a, b, c) { \
  a = __cmodel_fpx_fcmpuunn(b, c); \
}

/*  fcmpuun gpr , fpr , fpr
 * gpr fcmpuun( fpr, fpr);
 */
#define fpx_fcmpuun(a, b, c) { \
  a = __cmodel_fpx_fcmpuun(b, c); \
}

/*  fcnst fpr , <u9>
 * fpr fcnst( <u9>);
 */
#define fpx_fcnst(a, b) { \
  a = __cmodel_fpx_fcnst(b); \
}

/*  fdiv fpr , fpr , fpr
 * fpr fdiv( fpr, fpr);
 */
#define fpx_fdiv(a, b, c) { \
  a = __cmodel_fpx_fdiv(b, c); \
}

/*  ff2in gpr , fpr
 * gpr ff2in( fpr);
 */
#define fpx_ff2in_r(a, b) { \
  a = __cmodel_fpx_ff2in_r(b); \
}

/*  ff2in fpr , fpr
 * fpr ff2in( fpr);
 */
#define fpx_ff2in_f(a, b) { \
  a = __cmodel_fpx_ff2in_f(b); \
}

/*  ff2i gpr , fpr
 * gpr ff2i( fpr);
 */
#define fpx_ff2i_r(a, b) { \
  a = __cmodel_fpx_ff2i_r(b); \
}

/*  ff2i fpr , fpr
 * fpr ff2i( fpr);
 */
#define fpx_ff2i_f(a, b) { \
  a = __cmodel_fpx_ff2i_f(b); \
}

/*  ff2un gpr , fpr
 * gpr ff2un( fpr);
 */
#define fpx_ff2un_r(a, b) { \
  a = __cmodel_fpx_ff2un_r(b); \
}

/*  ff2un fpr , fpr
 * fpr ff2un( fpr);
 */
#define fpx_ff2un_f(a, b) { \
  a = __cmodel_fpx_ff2un_f(b); \
}

/*  ff2u gpr , fpr
 * gpr ff2u( fpr);
 */
#define fpx_ff2u_r(a, b) { \
  a = __cmodel_fpx_ff2u_r(b); \
}

/*  ff2u fpr , fpr
 * fpr ff2u( fpr);
 */
#define fpx_ff2u_f(a, b) { \
  a = __cmodel_fpx_ff2u_f(b); \
}

/*  fi2f fpr , gpr
 * fpr fi2f( gpr);
 */
#define fpx_fi2f_r(a, b) { \
  a = __cmodel_fpx_fi2f_r(b); \
}

/*  fi2f fpr , fpr
 * fpr fi2f( fpr);
 */
#define fpx_fi2f_f(a, b) { \
  a = __cmodel_fpx_fi2f_f(b); \
}

/*  fmacn fpr , fpr , fpr
 * fpr fmacn( fpr, fpr, fpr);
 */
#define fpx_fmacn(a, b, c) { \
  a = __cmodel_fpx_fmacn(a, b, c); \
}

/*  fmac fpr , fpr , fpr
 * fpr fmac( fpr, fpr, fpr);
 */
#define fpx_fmac(a, b, c) { \
  a = __cmodel_fpx_fmac(a, b, c); \
}

/*  fmax fpr , fpr , fpr
 * fpr fmax( fpr, fpr);
 */
#define fpx_fmax(a, b, c) { \
  a = __cmodel_fpx_fmax(b, c); \
}

/*  fmin fpr , fpr , fpr
 * fpr fmin( fpr, fpr);
 */
#define fpx_fmin(a, b, c) { \
  a = __cmodel_fpx_fmin(b, c); \
}

/*  fmuln fpr , fpr , fpr
 * fpr fmuln( fpr, fpr);
 */
#define fpx_fmuln(a, b, c) { \
  a = __cmodel_fpx_fmuln(b, c); \
}

/*  fmul fpr , fpr , fpr
 * fpr fmul( fpr, fpr);
 */
#define fpx_fmul(a, b, c) { \
  a = __cmodel_fpx_fmul(b, c); \
}

/*  fmvf2f fpr , fpr
 * fpr fmvf2f( fpr);
 */
#define fpx_fmvf2f(a, b) { \
  a = __cmodel_fpx_fmvf2f(b); \
}

/*  fmvf2r gpr , fpr
 * gpr fmvf2r( fpr);
 */
#define fpx_fmvf2r(a, b) { \
  a = __cmodel_fpx_fmvf2r(b); \
}

/*  fmvr2f fpr , gpr
 * fpr fmvr2f( gpr);
 */
#define fpx_fmvr2f(a, b) { \
  a = __cmodel_fpx_fmvr2f(b); \
}

/*  fmvr2st gpr
 * fmvr2st( gpr);
 */
#define fpx_fmvr2st(a) { \
  __cmodel_fpx_fmvr2st(a); \
}

/*  fmvr2s gpr
 * fmvr2s( gpr);
 */
#define fpx_fmvr2s(a) { \
  __cmodel_fpx_fmvr2s(a); \
}

/*  fmvs2r gpr
 * gpr fmvs2r( );
 */
#define fpx_fmvs2r(a) { \
  a = __cmodel_fpx_fmvs2r(); \
}

/*  fneg fpr , fpr
 * fpr fneg( fpr);
 */
#define fpx_fneg(a, b) { \
  a = __cmodel_fpx_fneg(b); \
}

/*  fscalb fpr , fpr , gpr
 * fpr fscalb( fpr, gpr);
 */
#define fpx_fscalb(a, b, c) { \
  a = __cmodel_fpx_fscalb(b, c); \
}

/*  fsqrt fpr , fpr
 * fpr fsqrt( fpr);
 */
#define fpx_fsqrt(a, b) { \
  a = __cmodel_fpx_fsqrt(b); \
}

/*  fsub fpr , fpr , fpr
 * fpr fsub( fpr, fpr);
 */
#define fpx_fsub(a, b, c) { \
  a = __cmodel_fpx_fsub(b, c); \
}

/*  fu2f fpr , gpr
 * fpr fu2f( gpr);
 */
#define fpx_fu2f_r(a, b) { \
  a = __cmodel_fpx_fu2f_r(b); \
}

/*  fu2f fpr , fpr
 * fpr fu2f( fpr);
 */
#define fpx_fu2f_f(a, b) { \
  a = __cmodel_fpx_fu2f_f(b); \
}

/*  macnsshh fpr , gpr , gpr
 * fpr macnsshh( fpr, gpr, gpr);
 */
#define fpx_macnsshh(a, b, c) { \
  a = __cmodel_fpx_macnsshh(a, b, c); \
}

/*  macnsshl fpr , gpr , gpr
 * fpr macnsshl( fpr, gpr, gpr);
 */
#define fpx_macnsshl(a, b, c) { \
  a = __cmodel_fpx_macnsshl(a, b, c); \
}

/*  macnsslh fpr , gpr , gpr
 * fpr macnsslh( fpr, gpr, gpr);
 */
#define fpx_macnsslh(a, b, c) { \
  a = __cmodel_fpx_macnsslh(a, b, c); \
}

/*  macnssll fpr , gpr , gpr
 * fpr macnssll( fpr, gpr, gpr);
 */
#define fpx_macnssll(a, b, c) { \
  a = __cmodel_fpx_macnssll(a, b, c); \
}

/*  macnsuhh fpr , gpr , gpr
 * fpr macnsuhh( fpr, gpr, gpr);
 */
#define fpx_macnsuhh(a, b, c) { \
  a = __cmodel_fpx_macnsuhh(a, b, c); \
}

/*  macnsuhl fpr , gpr , gpr
 * fpr macnsuhl( fpr, gpr, gpr);
 */
#define fpx_macnsuhl(a, b, c) { \
  a = __cmodel_fpx_macnsuhl(a, b, c); \
}

/*  macnsulh fpr , gpr , gpr
 * fpr macnsulh( fpr, gpr, gpr);
 */
#define fpx_macnsulh(a, b, c) { \
  a = __cmodel_fpx_macnsulh(a, b, c); \
}

/*  macnsull fpr , gpr , gpr
 * fpr macnsull( fpr, gpr, gpr);
 */
#define fpx_macnsull(a, b, c) { \
  a = __cmodel_fpx_macnsull(a, b, c); \
}

/*  macnushh fpr , gpr , gpr
 * fpr macnushh( fpr, gpr, gpr);
 */
#define fpx_macnushh(a, b, c) { \
  a = __cmodel_fpx_macnushh(a, b, c); \
}

/*  macnushl fpr , gpr , gpr
 * fpr macnushl( fpr, gpr, gpr);
 */
#define fpx_macnushl(a, b, c) { \
  a = __cmodel_fpx_macnushl(a, b, c); \
}

/*  macnuslh fpr , gpr , gpr
 * fpr macnuslh( fpr, gpr, gpr);
 */
#define fpx_macnuslh(a, b, c) { \
  a = __cmodel_fpx_macnuslh(a, b, c); \
}

/*  macnusll fpr , gpr , gpr
 * fpr macnusll( fpr, gpr, gpr);
 */
#define fpx_macnusll(a, b, c) { \
  a = __cmodel_fpx_macnusll(a, b, c); \
}

/*  macnuuhh fpr , gpr , gpr
 * fpr macnuuhh( fpr, gpr, gpr);
 */
#define fpx_macnuuhh(a, b, c) { \
  a = __cmodel_fpx_macnuuhh(a, b, c); \
}

/*  macnuuhl fpr , gpr , gpr
 * fpr macnuuhl( fpr, gpr, gpr);
 */
#define fpx_macnuuhl(a, b, c) { \
  a = __cmodel_fpx_macnuuhl(a, b, c); \
}

/*  macnuulh fpr , gpr , gpr
 * fpr macnuulh( fpr, gpr, gpr);
 */
#define fpx_macnuulh(a, b, c) { \
  a = __cmodel_fpx_macnuulh(a, b, c); \
}

/*  macnuull fpr , gpr , gpr
 * fpr macnuull( fpr, gpr, gpr);
 */
#define fpx_macnuull(a, b, c) { \
  a = __cmodel_fpx_macnuull(a, b, c); \
}

/*  macn fpr , gpr , gpr
 * fpr macn( fpr, gpr, gpr);
 */
#define fpx_macn(a, b, c) { \
  a = __cmodel_fpx_macn(a, b, c); \
}

/*  macsshh fpr , gpr , gpr
 * fpr macsshh( fpr, gpr, gpr);
 */
#define fpx_macsshh(a, b, c) { \
  a = __cmodel_fpx_macsshh(a, b, c); \
}

/*  macsshl fpr , gpr , gpr
 * fpr macsshl( fpr, gpr, gpr);
 */
#define fpx_macsshl(a, b, c) { \
  a = __cmodel_fpx_macsshl(a, b, c); \
}

/*  macsslh fpr , gpr , gpr
 * fpr macsslh( fpr, gpr, gpr);
 */
#define fpx_macsslh(a, b, c) { \
  a = __cmodel_fpx_macsslh(a, b, c); \
}

/*  macssll fpr , gpr , gpr
 * fpr macssll( fpr, gpr, gpr);
 */
#define fpx_macssll(a, b, c) { \
  a = __cmodel_fpx_macssll(a, b, c); \
}

/*  macsuhh fpr , gpr , gpr
 * fpr macsuhh( fpr, gpr, gpr);
 */
#define fpx_macsuhh(a, b, c) { \
  a = __cmodel_fpx_macsuhh(a, b, c); \
}

/*  macsuhl fpr , gpr , gpr
 * fpr macsuhl( fpr, gpr, gpr);
 */
#define fpx_macsuhl(a, b, c) { \
  a = __cmodel_fpx_macsuhl(a, b, c); \
}

/*  macsulh fpr , gpr , gpr
 * fpr macsulh( fpr, gpr, gpr);
 */
#define fpx_macsulh(a, b, c) { \
  a = __cmodel_fpx_macsulh(a, b, c); \
}

/*  macsull fpr , gpr , gpr
 * fpr macsull( fpr, gpr, gpr);
 */
#define fpx_macsull(a, b, c) { \
  a = __cmodel_fpx_macsull(a, b, c); \
}

/*  macushh fpr , gpr , gpr
 * fpr macushh( fpr, gpr, gpr);
 */
#define fpx_macushh(a, b, c) { \
  a = __cmodel_fpx_macushh(a, b, c); \
}

/*  macushl fpr , gpr , gpr
 * fpr macushl( fpr, gpr, gpr);
 */
#define fpx_macushl(a, b, c) { \
  a = __cmodel_fpx_macushl(a, b, c); \
}

/*  macuslh fpr , gpr , gpr
 * fpr macuslh( fpr, gpr, gpr);
 */
#define fpx_macuslh(a, b, c) { \
  a = __cmodel_fpx_macuslh(a, b, c); \
}

/*  macusll fpr , gpr , gpr
 * fpr macusll( fpr, gpr, gpr);
 */
#define fpx_macusll(a, b, c) { \
  a = __cmodel_fpx_macusll(a, b, c); \
}

/*  macuuhh fpr , gpr , gpr
 * fpr macuuhh( fpr, gpr, gpr);
 */
#define fpx_macuuhh(a, b, c) { \
  a = __cmodel_fpx_macuuhh(a, b, c); \
}

/*  macuuhl fpr , gpr , gpr
 * fpr macuuhl( fpr, gpr, gpr);
 */
#define fpx_macuuhl(a, b, c) { \
  a = __cmodel_fpx_macuuhl(a, b, c); \
}

/*  macuulh fpr , gpr , gpr
 * fpr macuulh( fpr, gpr, gpr);
 */
#define fpx_macuulh(a, b, c) { \
  a = __cmodel_fpx_macuulh(a, b, c); \
}

/*  macuull fpr , gpr , gpr
 * fpr macuull( fpr, gpr, gpr);
 */
#define fpx_macuull(a, b, c) { \
  a = __cmodel_fpx_macuull(a, b, c); \
}

/*  mac fpr , gpr , gpr
 * fpr mac( fpr, gpr, gpr);
 */
#define fpx_mac(a, b, c) { \
  a = __cmodel_fpx_mac(a, b, c); \
}

/*  modu fpr , gpr , gpr
 * fpr modu( gpr, gpr);
 */
#define fpx_modu(a, b, c) { \
  a = __cmodel_fpx_modu(b, c); \
}

/*  mod fpr , gpr , gpr
 * fpr mod( gpr, gpr);
 */
#define fpx_mod(a, b, c) { \
  a = __cmodel_fpx_mod(b, c); \
}

/*  mpnsshh gpr , gpr , gpr
 * gpr mpnsshh( gpr, gpr);
 */
#define fpx_mpnsshh(a, b, c) { \
  a = __cmodel_fpx_mpnsshh(b, c); \
}

/*  mpnsshl gpr , gpr , gpr
 * gpr mpnsshl( gpr, gpr);
 */
#define fpx_mpnsshl(a, b, c) { \
  a = __cmodel_fpx_mpnsshl(b, c); \
}

/*  mpnsslh gpr , gpr , gpr
 * gpr mpnsslh( gpr, gpr);
 */
#define fpx_mpnsslh(a, b, c) { \
  a = __cmodel_fpx_mpnsslh(b, c); \
}

/*  mpnssll gpr , gpr , gpr
 * gpr mpnssll( gpr, gpr);
 */
#define fpx_mpnssll(a, b, c) { \
  a = __cmodel_fpx_mpnssll(b, c); \
}

/*  mpnsuhh gpr , gpr , gpr
 * gpr mpnsuhh( gpr, gpr);
 */
#define fpx_mpnsuhh(a, b, c) { \
  a = __cmodel_fpx_mpnsuhh(b, c); \
}

/*  mpnsuhl gpr , gpr , gpr
 * gpr mpnsuhl( gpr, gpr);
 */
#define fpx_mpnsuhl(a, b, c) { \
  a = __cmodel_fpx_mpnsuhl(b, c); \
}

/*  mpnsulh gpr , gpr , gpr
 * gpr mpnsulh( gpr, gpr);
 */
#define fpx_mpnsulh(a, b, c) { \
  a = __cmodel_fpx_mpnsulh(b, c); \
}

/*  mpnsull gpr , gpr , gpr
 * gpr mpnsull( gpr, gpr);
 */
#define fpx_mpnsull(a, b, c) { \
  a = __cmodel_fpx_mpnsull(b, c); \
}

/*  mpnushh gpr , gpr , gpr
 * gpr mpnushh( gpr, gpr);
 */
#define fpx_mpnushh(a, b, c) { \
  a = __cmodel_fpx_mpnushh(b, c); \
}

/*  mpnushl gpr , gpr , gpr
 * gpr mpnushl( gpr, gpr);
 */
#define fpx_mpnushl(a, b, c) { \
  a = __cmodel_fpx_mpnushl(b, c); \
}

/*  mpnuslh gpr , gpr , gpr
 * gpr mpnuslh( gpr, gpr);
 */
#define fpx_mpnuslh(a, b, c) { \
  a = __cmodel_fpx_mpnuslh(b, c); \
}

/*  mpnusll gpr , gpr , gpr
 * gpr mpnusll( gpr, gpr);
 */
#define fpx_mpnusll(a, b, c) { \
  a = __cmodel_fpx_mpnusll(b, c); \
}

/*  mpnuuhh gpr , gpr , gpr
 * gpr mpnuuhh( gpr, gpr);
 */
#define fpx_mpnuuhh(a, b, c) { \
  a = __cmodel_fpx_mpnuuhh(b, c); \
}

/*  mpnuuhl gpr , gpr , gpr
 * gpr mpnuuhl( gpr, gpr);
 */
#define fpx_mpnuuhl(a, b, c) { \
  a = __cmodel_fpx_mpnuuhl(b, c); \
}

/*  mpnuulh gpr , gpr , gpr
 * gpr mpnuulh( gpr, gpr);
 */
#define fpx_mpnuulh(a, b, c) { \
  a = __cmodel_fpx_mpnuulh(b, c); \
}

/*  mpnuull gpr , gpr , gpr
 * gpr mpnuull( gpr, gpr);
 */
#define fpx_mpnuull(a, b, c) { \
  a = __cmodel_fpx_mpnuull(b, c); \
}

/*  mpn gpr , gpr , gpr
 * gpr mpn( gpr, gpr);
 */
#define fpx_mpn(a, b, c) { \
  a = __cmodel_fpx_mpn(b, c); \
}

/*  mpsshh gpr , gpr , gpr
 * gpr mpsshh( gpr, gpr);
 */
#define fpx_mpsshh(a, b, c) { \
  a = __cmodel_fpx_mpsshh(b, c); \
}

/*  mpsshl gpr , gpr , gpr
 * gpr mpsshl( gpr, gpr);
 */
#define fpx_mpsshl(a, b, c) { \
  a = __cmodel_fpx_mpsshl(b, c); \
}

/*  mpsslh gpr , gpr , gpr
 * gpr mpsslh( gpr, gpr);
 */
#define fpx_mpsslh(a, b, c) { \
  a = __cmodel_fpx_mpsslh(b, c); \
}

/*  mpssll gpr , gpr , gpr
 * gpr mpssll( gpr, gpr);
 */
#define fpx_mpssll(a, b, c) { \
  a = __cmodel_fpx_mpssll(b, c); \
}

/*  mpsuhh gpr , gpr , gpr
 * gpr mpsuhh( gpr, gpr);
 */
#define fpx_mpsuhh(a, b, c) { \
  a = __cmodel_fpx_mpsuhh(b, c); \
}

/*  mpsuhl gpr , gpr , gpr
 * gpr mpsuhl( gpr, gpr);
 */
#define fpx_mpsuhl(a, b, c) { \
  a = __cmodel_fpx_mpsuhl(b, c); \
}

/*  mpsulh gpr , gpr , gpr
 * gpr mpsulh( gpr, gpr);
 */
#define fpx_mpsulh(a, b, c) { \
  a = __cmodel_fpx_mpsulh(b, c); \
}

/*  mpsull gpr , gpr , gpr
 * gpr mpsull( gpr, gpr);
 */
#define fpx_mpsull(a, b, c) { \
  a = __cmodel_fpx_mpsull(b, c); \
}

/*  mpushh gpr , gpr , gpr
 * gpr mpushh( gpr, gpr);
 */
#define fpx_mpushh(a, b, c) { \
  a = __cmodel_fpx_mpushh(b, c); \
}

/*  mpushl gpr , gpr , gpr
 * gpr mpushl( gpr, gpr);
 */
#define fpx_mpushl(a, b, c) { \
  a = __cmodel_fpx_mpushl(b, c); \
}

/*  mpuslh gpr , gpr , gpr
 * gpr mpuslh( gpr, gpr);
 */
#define fpx_mpuslh(a, b, c) { \
  a = __cmodel_fpx_mpuslh(b, c); \
}

/*  mpusll gpr , gpr , gpr
 * gpr mpusll( gpr, gpr);
 */
#define fpx_mpusll(a, b, c) { \
  a = __cmodel_fpx_mpusll(b, c); \
}

/*  mpuuhh gpr , gpr , gpr
 * gpr mpuuhh( gpr, gpr);
 */
#define fpx_mpuuhh(a, b, c) { \
  a = __cmodel_fpx_mpuuhh(b, c); \
}

/*  mpuuhl gpr , gpr , gpr
 * gpr mpuuhl( gpr, gpr);
 */
#define fpx_mpuuhl(a, b, c) { \
  a = __cmodel_fpx_mpuuhl(b, c); \
}

/*  mpuulh gpr , gpr , gpr
 * gpr mpuulh( gpr, gpr);
 */
#define fpx_mpuulh(a, b, c) { \
  a = __cmodel_fpx_mpuulh(b, c); \
}

/*  mpuull gpr , gpr , gpr
 * gpr mpuull( gpr, gpr);
 */
#define fpx_mpuull(a, b, c) { \
  a = __cmodel_fpx_mpuull(b, c); \
}

/*  mp gpr , gpr , gpr
 * gpr mp( gpr, gpr);
 */
#define fpx_mp(a, b, c) { \
  a = __cmodel_fpx_mp(b, c); \
}

#endif /* 0 */


#endif /* defined(__FPX) && !defined(__FPX_C_MODEL) */
#endif /* _FPX_H_ */
