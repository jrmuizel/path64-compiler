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



#ifndef _X3_H_
#define _X3_H_

#if defined(__X3) && !defined(__X3_C_MODEL)

/* Macros mapped on builtins for class x3 */
#include <builtins_x3.h>

/*  cancelg gpr
 * cancelg( gpr);
 */
#define x3_cancelg_r_g(a) { \
  __builtin_x3_cancelg_r_g(a); \
}

/*  cancelg B<u2>, <u8>
 * cancelg( <u2>, <u8>);
 */
#define x3_cancelg_i8_i2_g(a, b) { \
  __builtin_x3_cancelg_i8_i2_g(a, b); \
}

/*  cancell gpr
 * cancell( gpr);
 */
#define x3_cancell_r_g(a) { \
  __builtin_x3_cancell_r_g(a); \
}

/*  cancell B<u2>, <u8>
 * cancell( <u2>, <u8>);
 */
#define x3_cancell_i8_i2_g(a, b) { \
  __builtin_x3_cancell_i8_i2_g(a, b); \
}

/* clrcc1
 * clrcc1( );
 */
#define x3_clrcc1() { \
  __builtin_x3_clrcc1(); \
}

/* clrcca
 * clrcca( );
 */
#define x3_clrcca() { \
  __builtin_x3_clrcca(); \
}

/* clrcc
 * clrcc( );
 */
#define x3_clrcc() { \
  __builtin_x3_clrcc(); \
}

/*  clric itcr, gpr
 * itcr clric( u[0..31], gpr);
 */
#define x3_clric(a, b) { \
  __builtin_x3_clric(a, b); \
}

/*  clrie gpr
 * gpr clrie( );
 */
#define x3_clrie(a) { \
  a = __builtin_x3_clrie(); \
}

/* dflushw
 * dflushw( );
 */
#define x3_dflushw() { \
  __builtin_x3_dflushw(); \
}

/* dflush gpr
 * dflush( gpr);
 */
#define x3_dflush(a) { \
  __builtin_x3_dflush(a); \
}

/* dinval gpr
 * dinval( gpr);
 */
#define x3_dinval(a) { \
  __builtin_x3_dinval(a); \
}

/* dlock gpr
 * dlock( gpr);
 */
#define x3_dlock(a) { \
  __builtin_x3_dlock(a); \
}

/* dstata gpr
 * dstata( gpr);
 */
#define x3_dstata(a) { \
  __builtin_x3_dstata(a); \
}

/* dstatwi gpr
 * dstatwi( gpr);
 */
#define x3_dstatwi(a) { \
  __builtin_x3_dstatwi(a); \
}

/* dtouch gpr
 * dtouch( gpr);
 */
#define x3_dtouch(a) { \
  __builtin_x3_dtouch(a); \
}

/*  fork<u3> gpr, gpr, <u1>
 * fork( <u3>, gpr, gpr, <u1>);
 */
#define x3_fork_i3_g(a, b, c, d) { \
  __builtin_x3_fork_i3_g(a, b, c, d); \
}

/*  fork gpr, gpr, <u1>
 * fork( gpr, gpr, <u1>);
 */
#define x3_fork_g(a, b, c) { \
  __builtin_x3_fork_g(a, b, c); \
}

/*  movee2r gpr, gecr
 * gpr movee2r( u[0..31]);
 */
#define x3_movee2r(a, b) { \
  a = __builtin_x3_movee2r(b); \
}

/*  moveic2ri gpr, gpr
 * gpr moveic2ri( gpr);
 */
#define x3_moveic2ri(a, b) { \
  a = __builtin_x3_moveic2ri(b); \
}

/*  moveic2r gpr, itcr
 * gpr moveic2r( u[0..31]);
 */
#define x3_moveic2r(a, b) { \
  a = __builtin_x3_moveic2r(b); \
}

/*  moveie2r gpr
 * gpr moveie2r( );
 */
#define x3_moveie2r(a) { \
  a = __builtin_x3_moveie2r(); \
}

/*  mover2e gecr, gpr
 * gecr mover2e( u[0..31], gpr);
 */
#define x3_mover2e(a, b) { \
  __builtin_x3_mover2e(a, b); \
}

/*  mover2ici gpr, gpr
 * mover2ici( gpr, gpr);
 */
#define x3_mover2ici(a, b) { \
  __builtin_x3_mover2ici(a, b); \
}

/*  mover2ic itcr, gpr
 * itcr mover2ic( u[0..31], gpr);
 */
#define x3_mover2ic(a, b) { \
  __builtin_x3_mover2ic(a, b); \
}

/*  mover2ie gpr
 * mover2ie( gpr);
 */
#define x3_mover2ie(a) { \
  __builtin_x3_mover2ie(a); \
}

/*  mpnsshh gpr, gpr, gpr
 * gpr mpnsshh( gpr, gpr);
 */
#define x3_mpnsshh(a, b, c) { \
  a = __builtin_x3_mpnsshh(b, c); \
}

/*  mpnsshl gpr, gpr, gpr
 * gpr mpnsshl( gpr, gpr);
 */
#define x3_mpnsshl(a, b, c) { \
  a = __builtin_x3_mpnsshl(b, c); \
}

/*  mpnsslh gpr, gpr, gpr
 * gpr mpnsslh( gpr, gpr);
 */
#define x3_mpnsslh(a, b, c) { \
  a = __builtin_x3_mpnsslh(b, c); \
}

/*  mpnssll gpr, gpr, gpr
 * gpr mpnssll( gpr, gpr);
 */
#define x3_mpnssll(a, b, c) { \
  a = __builtin_x3_mpnssll(b, c); \
}

/*  mpnsuhh gpr, gpr, gpr
 * gpr mpnsuhh( gpr, gpr);
 */
#define x3_mpnsuhh(a, b, c) { \
  a = __builtin_x3_mpnsuhh(b, c); \
}

/*  mpnsuhl gpr, gpr, gpr
 * gpr mpnsuhl( gpr, gpr);
 */
#define x3_mpnsuhl(a, b, c) { \
  a = __builtin_x3_mpnsuhl(b, c); \
}

/*  mpnsulh gpr, gpr, gpr
 * gpr mpnsulh( gpr, gpr);
 */
#define x3_mpnsulh(a, b, c) { \
  a = __builtin_x3_mpnsulh(b, c); \
}

/*  mpnsull gpr, gpr, gpr
 * gpr mpnsull( gpr, gpr);
 */
#define x3_mpnsull(a, b, c) { \
  a = __builtin_x3_mpnsull(b, c); \
}

/*  mpnushh gpr, gpr, gpr
 * gpr mpnushh( gpr, gpr);
 */
#define x3_mpnushh(a, b, c) { \
  a = __builtin_x3_mpnushh(b, c); \
}

/*  mpnushl gpr, gpr, gpr
 * gpr mpnushl( gpr, gpr);
 */
#define x3_mpnushl(a, b, c) { \
  a = __builtin_x3_mpnushl(b, c); \
}

/*  mpnuslh gpr, gpr, gpr
 * gpr mpnuslh( gpr, gpr);
 */
#define x3_mpnuslh(a, b, c) { \
  a = __builtin_x3_mpnuslh(b, c); \
}

/*  mpnusll gpr, gpr, gpr
 * gpr mpnusll( gpr, gpr);
 */
#define x3_mpnusll(a, b, c) { \
  a = __builtin_x3_mpnusll(b, c); \
}

/*  mpnuuhh gpr, gpr, gpr
 * gpr mpnuuhh( gpr, gpr);
 */
#define x3_mpnuuhh(a, b, c) { \
  a = __builtin_x3_mpnuuhh(b, c); \
}

/*  mpnuuhl gpr, gpr, gpr
 * gpr mpnuuhl( gpr, gpr);
 */
#define x3_mpnuuhl(a, b, c) { \
  a = __builtin_x3_mpnuuhl(b, c); \
}

/*  mpnuulh gpr, gpr, gpr
 * gpr mpnuulh( gpr, gpr);
 */
#define x3_mpnuulh(a, b, c) { \
  a = __builtin_x3_mpnuulh(b, c); \
}

/*  mpnuull gpr, gpr, gpr
 * gpr mpnuull( gpr, gpr);
 */
#define x3_mpnuull(a, b, c) { \
  a = __builtin_x3_mpnuull(b, c); \
}

/*  mpn gpr, gpr, gpr
 * gpr mpn( gpr, gpr);
 */
#define x3_mpn(a, b, c) { \
  a = __builtin_x3_mpn(b, c); \
}

/*  mpsshh gpr, gpr, gpr
 * gpr mpsshh( gpr, gpr);
 */
#define x3_mpsshh(a, b, c) { \
  a = __builtin_x3_mpsshh(b, c); \
}

/*  mpsshl gpr, gpr, gpr
 * gpr mpsshl( gpr, gpr);
 */
#define x3_mpsshl(a, b, c) { \
  a = __builtin_x3_mpsshl(b, c); \
}

/*  mpsslh gpr, gpr, gpr
 * gpr mpsslh( gpr, gpr);
 */
#define x3_mpsslh(a, b, c) { \
  a = __builtin_x3_mpsslh(b, c); \
}

/*  mpssll gpr, gpr, gpr
 * gpr mpssll( gpr, gpr);
 */
#define x3_mpssll(a, b, c) { \
  a = __builtin_x3_mpssll(b, c); \
}

/*  mpsuhh gpr, gpr, gpr
 * gpr mpsuhh( gpr, gpr);
 */
#define x3_mpsuhh(a, b, c) { \
  a = __builtin_x3_mpsuhh(b, c); \
}

/*  mpsuhl gpr, gpr, gpr
 * gpr mpsuhl( gpr, gpr);
 */
#define x3_mpsuhl(a, b, c) { \
  a = __builtin_x3_mpsuhl(b, c); \
}

/*  mpsulh gpr, gpr, gpr
 * gpr mpsulh( gpr, gpr);
 */
#define x3_mpsulh(a, b, c) { \
  a = __builtin_x3_mpsulh(b, c); \
}

/*  mpsull gpr, gpr, gpr
 * gpr mpsull( gpr, gpr);
 */
#define x3_mpsull(a, b, c) { \
  a = __builtin_x3_mpsull(b, c); \
}

/*  mpushh gpr, gpr, gpr
 * gpr mpushh( gpr, gpr);
 */
#define x3_mpushh(a, b, c) { \
  a = __builtin_x3_mpushh(b, c); \
}

/*  mpushl gpr, gpr, gpr
 * gpr mpushl( gpr, gpr);
 */
#define x3_mpushl(a, b, c) { \
  a = __builtin_x3_mpushl(b, c); \
}

/*  mpuslh gpr, gpr, gpr
 * gpr mpuslh( gpr, gpr);
 */
#define x3_mpuslh(a, b, c) { \
  a = __builtin_x3_mpuslh(b, c); \
}

/*  mpusll gpr, gpr, gpr
 * gpr mpusll( gpr, gpr);
 */
#define x3_mpusll(a, b, c) { \
  a = __builtin_x3_mpusll(b, c); \
}

/*  mpuuhh gpr, gpr, gpr
 * gpr mpuuhh( gpr, gpr);
 */
#define x3_mpuuhh(a, b, c) { \
  a = __builtin_x3_mpuuhh(b, c); \
}

/*  mpuuhl gpr, gpr, gpr
 * gpr mpuuhl( gpr, gpr);
 */
#define x3_mpuuhl(a, b, c) { \
  a = __builtin_x3_mpuuhl(b, c); \
}

/*  mpuulh gpr, gpr, gpr
 * gpr mpuulh( gpr, gpr);
 */
#define x3_mpuulh(a, b, c) { \
  a = __builtin_x3_mpuulh(b, c); \
}

/*  mpuull gpr, gpr, gpr
 * gpr mpuull( gpr, gpr);
 */
#define x3_mpuull(a, b, c) { \
  a = __builtin_x3_mpuull(b, c); \
}

/*  mp gpr, gpr, gpr
 * gpr mp( gpr, gpr);
 */
#define x3_mp(a, b, c) { \
  a = __builtin_x3_mp(b, c); \
}

/*  notifyg gpr
 * notifyg( gpr);
 */
#define x3_notifyg_r_g(a) { \
  __builtin_x3_notifyg_r_g(a); \
}

/*  notifyg B<u2>, <u8>
 * notifyg( <u2>, <u8>);
 */
#define x3_notifyg_i8_i2_g(a, b) { \
  __builtin_x3_notifyg_i8_i2_g(a, b); \
}

/*  notifyla gpr
 * notifyla( gpr);
 */
#define x3_notifyla_r_g(a) { \
  __builtin_x3_notifyla_r_g(a); \
}

/*  notifyla B<u2>, <u8>
 * notifyla( <u2>, <u8>);
 */
#define x3_notifyla_i8_i2_g(a, b) { \
  __builtin_x3_notifyla_i8_i2_g(a, b); \
}

/*  notifyl<u3> gpr
 * notifyl( <u3>, gpr);
 */
#define x3_notifyl_r_i3_g(a, b) { \
  __builtin_x3_notifyl_r_i3_g(a, b); \
}

/*  notifyl<u3> B<u2>, <u8>
 * notifyl( <u3>, <u2>, <u8>);
 */
#define x3_notifyl_i8_i2_i3_g(a, b, c) { \
  __builtin_x3_notifyl_i8_i2_i3_g(a, b, c); \
}

/* pflushw
 * pflushw( );
 */
#define x3_pflushw() { \
  __builtin_x3_pflushw(); \
}

/* pinvalr gpr
 * pinvalr( gpr);
 */
#define x3_pinvalr(a) { \
  __builtin_x3_pinvalr(a); \
}

/* pinval gpr
 * pinval( gpr);
 */
#define x3_pinval(a) { \
  __builtin_x3_pinval(a); \
}

/* plockr gpr
 * plockr( gpr);
 */
#define x3_plockr(a) { \
  __builtin_x3_plockr(a); \
}

/* plock gpr
 * plock( gpr);
 */
#define x3_plock(a) { \
  __builtin_x3_plock(a); \
}

/* pstata gpr
 * pstata( gpr);
 */
#define x3_pstata(a) { \
  __builtin_x3_pstata(a); \
}

/* pstatwi gpr
 * pstatwi( gpr);
 */
#define x3_pstatwi(a) { \
  __builtin_x3_pstatwi(a); \
}

/* ptouch gpr
 * ptouch( gpr);
 */
#define x3_ptouch(a) { \
  __builtin_x3_ptouch(a); \
}

/*  setic itcr, gpr
 * itcr setic( u[0..31], gpr);
 */
#define x3_setic(a, b) { \
  __builtin_x3_setic(a, b); \
}

/*  setie gpr
 * setie( gpr);
 */
#define x3_setie(a) { \
  __builtin_x3_setie(a); \
}

/* startcc1
 * startcc1( );
 */
#define x3_startcc1() { \
  __builtin_x3_startcc1(); \
}

/* startcca
 * startcca( );
 */
#define x3_startcca() { \
  __builtin_x3_startcca(); \
}

/* startcc
 * startcc( );
 */
#define x3_startcc() { \
  __builtin_x3_startcc(); \
}

/* stopcc1
 * stopcc1( );
 */
#define x3_stopcc1() { \
  __builtin_x3_stopcc1(); \
}

/* stopcca
 * stopcca( );
 */
#define x3_stopcca() { \
  __builtin_x3_stopcca(); \
}

/* stopcc
 * stopcc( );
 */
#define x3_stopcc() { \
  __builtin_x3_stopcc(); \
}

/* terminate<u3>
 * terminate( <u3>);
 */
#define x3_terminate_i3(a) { \
  __builtin_x3_terminate_i3(a); \
}

/* terminate
 * terminate( );
 */
#define x3_terminate() { \
  __builtin_x3_terminate(); \
}

/*  waitg gpr
 * waitg( gpr);
 */
#define x3_waitg_r_g(a) { \
  __builtin_x3_waitg_r_g(a); \
}

/*  waitg B<u2>, <u8>
 * waitg( <u2>, <u8>);
 */
#define x3_waitg_i8_i2_g(a, b) { \
  __builtin_x3_waitg_i8_i2_g(a, b); \
}

/*  waitl gpr
 * waitl( gpr);
 */
#define x3_waitl_r_g(a) { \
  __builtin_x3_waitl_r_g(a); \
}

/*  waitl B<u2>, <u8>
 * waitl( <u2>, <u8>);
 */
#define x3_waitl_i8_i2_g(a, b) { \
  __builtin_x3_waitl_i8_i2_g(a, b); \
}

/*  waitnbg gpr
 * waitnbg( gpr);
 */
#define x3_waitnbg_r_g(a) { \
  __builtin_x3_waitnbg_r_g(a); \
}

/*  waitnbg B<u2>, <u8>
 * waitnbg( <u2>, <u8>);
 */
#define x3_waitnbg_i8_i2_g(a, b) { \
  __builtin_x3_waitnbg_i8_i2_g(a, b); \
}

/*  waitnbl gpr
 * waitnbl( gpr);
 */
#define x3_waitnbl_r_g(a) { \
  __builtin_x3_waitnbl_r_g(a); \
}

/*  waitnbl B<u2>, <u8>
 * waitnbl( <u2>, <u8>);
 */
#define x3_waitnbl_i8_i2_g(a, b) { \
  __builtin_x3_waitnbl_i8_i2_g(a, b); \
}

/*  waitnb gpr, gpr
 * waitnb( gpr, gpr);
 */
#define x3_waitnb(a, b) { \
  __builtin_x3_waitnb(a, b); \
}

/*  wait gpr, gpr
 * wait( gpr, gpr);
 */
#define x3_wait(a, b) { \
  __builtin_x3_wait(a, b); \
}

#else /* !defined(__X3) || defined(__X3_C_MODEL) */

/* Macros mapped on C models for class x3 */
#include <builtins_model_x3.h>

/*  cancelg gpr
 * cancelg( gpr);
 */
#define x3_cancelg_r_g(a) { \
  __cmodel_x3_cancelg_r_g(a); \
}

/*  cancelg B<u2>, <u8>
 * cancelg( <u2>, <u8>);
 */
#define x3_cancelg_i8_i2_g(a, b) { \
  __cmodel_x3_cancelg_i8_i2_g(a, b); \
}

/*  cancell gpr
 * cancell( gpr);
 */
#define x3_cancell_r_g(a) { \
  __cmodel_x3_cancell_r_g(a); \
}

/*  cancell B<u2>, <u8>
 * cancell( <u2>, <u8>);
 */
#define x3_cancell_i8_i2_g(a, b) { \
  __cmodel_x3_cancell_i8_i2_g(a, b); \
}

/* clrcc1
 * clrcc1( );
 */
#define x3_clrcc1() { \
  __cmodel_x3_clrcc1(); \
}

/* clrcca
 * clrcca( );
 */
#define x3_clrcca() { \
  __cmodel_x3_clrcca(); \
}

/* clrcc
 * clrcc( );
 */
#define x3_clrcc() { \
  __cmodel_x3_clrcc(); \
}

/*  clric itcr, gpr
 * itcr clric( u[0..31], gpr);
 */
#define x3_clric(a, b) { \
  __cmodel_x3_clric(a, b); \
}

/*  clrie gpr
 * gpr clrie( );
 */
#define x3_clrie(a) { \
  a = __cmodel_x3_clrie(); \
}

/* dflushw
 * dflushw( );
 */
#define x3_dflushw() { \
  __cmodel_x3_dflushw(); \
}

/* dflush gpr
 * dflush( gpr);
 */
#define x3_dflush(a) { \
  __cmodel_x3_dflush(a); \
}

/* dinval gpr
 * dinval( gpr);
 */
#define x3_dinval(a) { \
  __cmodel_x3_dinval(a); \
}

/* dlock gpr
 * dlock( gpr);
 */
#define x3_dlock(a) { \
  __cmodel_x3_dlock(a); \
}

/* dstata gpr
 * dstata( gpr);
 */
#define x3_dstata(a) { \
  __cmodel_x3_dstata(a); \
}

/* dstatwi gpr
 * dstatwi( gpr);
 */
#define x3_dstatwi(a) { \
  __cmodel_x3_dstatwi(a); \
}

/* dtouch gpr
 * dtouch( gpr);
 */
#define x3_dtouch(a) { \
  __cmodel_x3_dtouch(a); \
}

/*  fork<u3> gpr, gpr, <u1>
 * fork( <u3>, gpr, gpr, <u1>);
 */
#define x3_fork_i3_g(a, b, c, d) { \
  __cmodel_x3_fork_i3_g(a, b, c, d); \
}

/*  fork gpr, gpr, <u1>
 * fork( gpr, gpr, <u1>);
 */
#define x3_fork_g(a, b, c) { \
  __cmodel_x3_fork_g(a, b, c); \
}

/*  movee2r gpr, gecr
 * gpr movee2r( u[0..31]);
 */
#define x3_movee2r(a, b) { \
  a = __cmodel_x3_movee2r(b); \
}

/*  moveic2ri gpr, gpr
 * gpr moveic2ri( gpr);
 */
#define x3_moveic2ri(a, b) { \
  a = __cmodel_x3_moveic2ri(b); \
}

/*  moveic2r gpr, itcr
 * gpr moveic2r( u[0..31]);
 */
#define x3_moveic2r(a, b) { \
  a = __cmodel_x3_moveic2r(b); \
}

/*  moveie2r gpr
 * gpr moveie2r( );
 */
#define x3_moveie2r(a) { \
  a = __cmodel_x3_moveie2r(); \
}

/*  mover2e gecr, gpr
 * gecr mover2e( u[0..31], gpr);
 */
#define x3_mover2e(a, b) { \
  __cmodel_x3_mover2e(a, b); \
}

/*  mover2ici gpr, gpr
 * mover2ici( gpr, gpr);
 */
#define x3_mover2ici(a, b) { \
  __cmodel_x3_mover2ici(a, b); \
}

/*  mover2ic itcr, gpr
 * itcr mover2ic( u[0..31], gpr);
 */
#define x3_mover2ic(a, b) { \
  __cmodel_x3_mover2ic(a, b); \
}

/*  mover2ie gpr
 * mover2ie( gpr);
 */
#define x3_mover2ie(a) { \
  __cmodel_x3_mover2ie(a); \
}

/*  mpnsshh gpr, gpr, gpr
 * gpr mpnsshh( gpr, gpr);
 */
#define x3_mpnsshh(a, b, c) { \
  a = __cmodel_x3_mpnsshh(b, c); \
}

/*  mpnsshl gpr, gpr, gpr
 * gpr mpnsshl( gpr, gpr);
 */
#define x3_mpnsshl(a, b, c) { \
  a = __cmodel_x3_mpnsshl(b, c); \
}

/*  mpnsslh gpr, gpr, gpr
 * gpr mpnsslh( gpr, gpr);
 */
#define x3_mpnsslh(a, b, c) { \
  a = __cmodel_x3_mpnsslh(b, c); \
}

/*  mpnssll gpr, gpr, gpr
 * gpr mpnssll( gpr, gpr);
 */
#define x3_mpnssll(a, b, c) { \
  a = __cmodel_x3_mpnssll(b, c); \
}

/*  mpnsuhh gpr, gpr, gpr
 * gpr mpnsuhh( gpr, gpr);
 */
#define x3_mpnsuhh(a, b, c) { \
  a = __cmodel_x3_mpnsuhh(b, c); \
}

/*  mpnsuhl gpr, gpr, gpr
 * gpr mpnsuhl( gpr, gpr);
 */
#define x3_mpnsuhl(a, b, c) { \
  a = __cmodel_x3_mpnsuhl(b, c); \
}

/*  mpnsulh gpr, gpr, gpr
 * gpr mpnsulh( gpr, gpr);
 */
#define x3_mpnsulh(a, b, c) { \
  a = __cmodel_x3_mpnsulh(b, c); \
}

/*  mpnsull gpr, gpr, gpr
 * gpr mpnsull( gpr, gpr);
 */
#define x3_mpnsull(a, b, c) { \
  a = __cmodel_x3_mpnsull(b, c); \
}

/*  mpnushh gpr, gpr, gpr
 * gpr mpnushh( gpr, gpr);
 */
#define x3_mpnushh(a, b, c) { \
  a = __cmodel_x3_mpnushh(b, c); \
}

/*  mpnushl gpr, gpr, gpr
 * gpr mpnushl( gpr, gpr);
 */
#define x3_mpnushl(a, b, c) { \
  a = __cmodel_x3_mpnushl(b, c); \
}

/*  mpnuslh gpr, gpr, gpr
 * gpr mpnuslh( gpr, gpr);
 */
#define x3_mpnuslh(a, b, c) { \
  a = __cmodel_x3_mpnuslh(b, c); \
}

/*  mpnusll gpr, gpr, gpr
 * gpr mpnusll( gpr, gpr);
 */
#define x3_mpnusll(a, b, c) { \
  a = __cmodel_x3_mpnusll(b, c); \
}

/*  mpnuuhh gpr, gpr, gpr
 * gpr mpnuuhh( gpr, gpr);
 */
#define x3_mpnuuhh(a, b, c) { \
  a = __cmodel_x3_mpnuuhh(b, c); \
}

/*  mpnuuhl gpr, gpr, gpr
 * gpr mpnuuhl( gpr, gpr);
 */
#define x3_mpnuuhl(a, b, c) { \
  a = __cmodel_x3_mpnuuhl(b, c); \
}

/*  mpnuulh gpr, gpr, gpr
 * gpr mpnuulh( gpr, gpr);
 */
#define x3_mpnuulh(a, b, c) { \
  a = __cmodel_x3_mpnuulh(b, c); \
}

/*  mpnuull gpr, gpr, gpr
 * gpr mpnuull( gpr, gpr);
 */
#define x3_mpnuull(a, b, c) { \
  a = __cmodel_x3_mpnuull(b, c); \
}

/*  mpn gpr, gpr, gpr
 * gpr mpn( gpr, gpr);
 */
#define x3_mpn(a, b, c) { \
  a = __cmodel_x3_mpn(b, c); \
}

/*  mpsshh gpr, gpr, gpr
 * gpr mpsshh( gpr, gpr);
 */
#define x3_mpsshh(a, b, c) { \
  a = __cmodel_x3_mpsshh(b, c); \
}

/*  mpsshl gpr, gpr, gpr
 * gpr mpsshl( gpr, gpr);
 */
#define x3_mpsshl(a, b, c) { \
  a = __cmodel_x3_mpsshl(b, c); \
}

/*  mpsslh gpr, gpr, gpr
 * gpr mpsslh( gpr, gpr);
 */
#define x3_mpsslh(a, b, c) { \
  a = __cmodel_x3_mpsslh(b, c); \
}

/*  mpssll gpr, gpr, gpr
 * gpr mpssll( gpr, gpr);
 */
#define x3_mpssll(a, b, c) { \
  a = __cmodel_x3_mpssll(b, c); \
}

/*  mpsuhh gpr, gpr, gpr
 * gpr mpsuhh( gpr, gpr);
 */
#define x3_mpsuhh(a, b, c) { \
  a = __cmodel_x3_mpsuhh(b, c); \
}

/*  mpsuhl gpr, gpr, gpr
 * gpr mpsuhl( gpr, gpr);
 */
#define x3_mpsuhl(a, b, c) { \
  a = __cmodel_x3_mpsuhl(b, c); \
}

/*  mpsulh gpr, gpr, gpr
 * gpr mpsulh( gpr, gpr);
 */
#define x3_mpsulh(a, b, c) { \
  a = __cmodel_x3_mpsulh(b, c); \
}

/*  mpsull gpr, gpr, gpr
 * gpr mpsull( gpr, gpr);
 */
#define x3_mpsull(a, b, c) { \
  a = __cmodel_x3_mpsull(b, c); \
}

/*  mpushh gpr, gpr, gpr
 * gpr mpushh( gpr, gpr);
 */
#define x3_mpushh(a, b, c) { \
  a = __cmodel_x3_mpushh(b, c); \
}

/*  mpushl gpr, gpr, gpr
 * gpr mpushl( gpr, gpr);
 */
#define x3_mpushl(a, b, c) { \
  a = __cmodel_x3_mpushl(b, c); \
}

/*  mpuslh gpr, gpr, gpr
 * gpr mpuslh( gpr, gpr);
 */
#define x3_mpuslh(a, b, c) { \
  a = __cmodel_x3_mpuslh(b, c); \
}

/*  mpusll gpr, gpr, gpr
 * gpr mpusll( gpr, gpr);
 */
#define x3_mpusll(a, b, c) { \
  a = __cmodel_x3_mpusll(b, c); \
}

/*  mpuuhh gpr, gpr, gpr
 * gpr mpuuhh( gpr, gpr);
 */
#define x3_mpuuhh(a, b, c) { \
  a = __cmodel_x3_mpuuhh(b, c); \
}

/*  mpuuhl gpr, gpr, gpr
 * gpr mpuuhl( gpr, gpr);
 */
#define x3_mpuuhl(a, b, c) { \
  a = __cmodel_x3_mpuuhl(b, c); \
}

/*  mpuulh gpr, gpr, gpr
 * gpr mpuulh( gpr, gpr);
 */
#define x3_mpuulh(a, b, c) { \
  a = __cmodel_x3_mpuulh(b, c); \
}

/*  mpuull gpr, gpr, gpr
 * gpr mpuull( gpr, gpr);
 */
#define x3_mpuull(a, b, c) { \
  a = __cmodel_x3_mpuull(b, c); \
}

/*  mp gpr, gpr, gpr
 * gpr mp( gpr, gpr);
 */
#define x3_mp(a, b, c) { \
  a = __cmodel_x3_mp(b, c); \
}

/*  notifyg gpr
 * notifyg( gpr);
 */
#define x3_notifyg_r_g(a) { \
  __cmodel_x3_notifyg_r_g(a); \
}

/*  notifyg B<u2>, <u8>
 * notifyg( <u2>, <u8>);
 */
#define x3_notifyg_i8_i2_g(a, b) { \
  __cmodel_x3_notifyg_i8_i2_g(a, b); \
}

/*  notifyla gpr
 * notifyla( gpr);
 */
#define x3_notifyla_r_g(a) { \
  __cmodel_x3_notifyla_r_g(a); \
}

/*  notifyla B<u2>, <u8>
 * notifyla( <u2>, <u8>);
 */
#define x3_notifyla_i8_i2_g(a, b) { \
  __cmodel_x3_notifyla_i8_i2_g(a, b); \
}

/*  notifyl<u3> gpr
 * notifyl( <u3>, gpr);
 */
#define x3_notifyl_r_i3_g(a, b) { \
  __cmodel_x3_notifyl_r_i3_g(a, b); \
}

/*  notifyl<u3> B<u2>, <u8>
 * notifyl( <u3>, <u2>, <u8>);
 */
#define x3_notifyl_i8_i2_i3_g(a, b, c) { \
  __cmodel_x3_notifyl_i8_i2_i3_g(a, b, c); \
}

/* pflushw
 * pflushw( );
 */
#define x3_pflushw() { \
  __cmodel_x3_pflushw(); \
}

/* pinvalr gpr
 * pinvalr( gpr);
 */
#define x3_pinvalr(a) { \
  __cmodel_x3_pinvalr(a); \
}

/* pinval gpr
 * pinval( gpr);
 */
#define x3_pinval(a) { \
  __cmodel_x3_pinval(a); \
}

/* plockr gpr
 * plockr( gpr);
 */
#define x3_plockr(a) { \
  __cmodel_x3_plockr(a); \
}

/* plock gpr
 * plock( gpr);
 */
#define x3_plock(a) { \
  __cmodel_x3_plock(a); \
}

/* pstata gpr
 * pstata( gpr);
 */
#define x3_pstata(a) { \
  __cmodel_x3_pstata(a); \
}

/* pstatwi gpr
 * pstatwi( gpr);
 */
#define x3_pstatwi(a) { \
  __cmodel_x3_pstatwi(a); \
}

/* ptouch gpr
 * ptouch( gpr);
 */
#define x3_ptouch(a) { \
  __cmodel_x3_ptouch(a); \
}

/*  setic itcr, gpr
 * itcr setic( u[0..31], gpr);
 */
#define x3_setic(a, b) { \
  __cmodel_x3_setic(a, b); \
}

/*  setie gpr
 * setie( gpr);
 */
#define x3_setie(a) { \
  __cmodel_x3_setie(a); \
}

/* startcc1
 * startcc1( );
 */
#define x3_startcc1() { \
  __cmodel_x3_startcc1(); \
}

/* startcca
 * startcca( );
 */
#define x3_startcca() { \
  __cmodel_x3_startcca(); \
}

/* startcc
 * startcc( );
 */
#define x3_startcc() { \
  __cmodel_x3_startcc(); \
}

/* stopcc1
 * stopcc1( );
 */
#define x3_stopcc1() { \
  __cmodel_x3_stopcc1(); \
}

/* stopcca
 * stopcca( );
 */
#define x3_stopcca() { \
  __cmodel_x3_stopcca(); \
}

/* stopcc
 * stopcc( );
 */
#define x3_stopcc() { \
  __cmodel_x3_stopcc(); \
}

/* terminate<u3>
 * terminate( <u3>);
 */
#define x3_terminate_i3(a) { \
  __cmodel_x3_terminate_i3(a); \
}

/* terminate
 * terminate( );
 */
#define x3_terminate() { \
  __cmodel_x3_terminate(); \
}

/*  waitg gpr
 * waitg( gpr);
 */
#define x3_waitg_r_g(a) { \
  __cmodel_x3_waitg_r_g(a); \
}

/*  waitg B<u2>, <u8>
 * waitg( <u2>, <u8>);
 */
#define x3_waitg_i8_i2_g(a, b) { \
  __cmodel_x3_waitg_i8_i2_g(a, b); \
}

/*  waitl gpr
 * waitl( gpr);
 */
#define x3_waitl_r_g(a) { \
  __cmodel_x3_waitl_r_g(a); \
}

/*  waitl B<u2>, <u8>
 * waitl( <u2>, <u8>);
 */
#define x3_waitl_i8_i2_g(a, b) { \
  __cmodel_x3_waitl_i8_i2_g(a, b); \
}

/*  waitnbg gpr
 * waitnbg( gpr);
 */
#define x3_waitnbg_r_g(a) { \
  __cmodel_x3_waitnbg_r_g(a); \
}

/*  waitnbg B<u2>, <u8>
 * waitnbg( <u2>, <u8>);
 */
#define x3_waitnbg_i8_i2_g(a, b) { \
  __cmodel_x3_waitnbg_i8_i2_g(a, b); \
}

/*  waitnbl gpr
 * waitnbl( gpr);
 */
#define x3_waitnbl_r_g(a) { \
  __cmodel_x3_waitnbl_r_g(a); \
}

/*  waitnbl B<u2>, <u8>
 * waitnbl( <u2>, <u8>);
 */
#define x3_waitnbl_i8_i2_g(a, b) { \
  __cmodel_x3_waitnbl_i8_i2_g(a, b); \
}

/*  waitnb gpr, gpr
 * waitnb( gpr, gpr);
 */
#define x3_waitnb(a, b) { \
  __cmodel_x3_waitnb(a, b); \
}

/*  wait gpr, gpr
 * wait( gpr, gpr);
 */
#define x3_wait(a, b) { \
  __cmodel_x3_wait(a, b); \
}


#endif /* defined(__X3) && !defined(__X3_C_MODEL) */
#endif /* _X3_H_ */
