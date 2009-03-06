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



#ifndef _SX_H_
#define _SX_H_

#if defined(__SX) && !defined(__SX_C_MODEL)

/* Macros mapped on builtins for class sx */
#include <builtins_sx.h>

/*  absbp gpr, gpr
 * gpr absbp( gpr);
 */
#define sx_absbp(a, b) { \
  a = __builtin_sx_absbp(b); \
}

/*  abshp gpr, gpr
 * gpr abshp( gpr);
 */
#define sx_abshp(a, b) { \
  a = __builtin_sx_abshp(b); \
}

/*  absubp gpr, gpr
 * gpr absubp( gpr);
 */
#define sx_absubp(a, b) { \
  a = __builtin_sx_absubp(b); \
}

/*  absuhp gpr, gpr
 * gpr absuhp( gpr);
 */
#define sx_absuhp(a, b) { \
  a = __builtin_sx_absuhp(b); \
}

/*  absu gpr, gpr
 * gpr absu( gpr);
 */
#define sx_absu(a, b) { \
  a = __builtin_sx_absu(b); \
}

/*  abs gpr, gpr
 * gpr abs( gpr);
 */
#define sx_abs(a, b) { \
  a = __builtin_sx_abs(b); \
}

/*  addbp gpr, gpr, gpr
 * gpr addbp( gpr, gpr);
 */
#define sx_addbp_r(a, b, c) { \
  a = __builtin_sx_addbp_r(b, c); \
}

/*  addbp gpr, gpr, <u8>
 * gpr addbp( gpr, <u8>);
 */
#define sx_addbp_i8(a, b, c) { \
  a = __builtin_sx_addbp_i8(b, c); \
}

/*  addcu gpr, gpr, gpr
 * gpr addcu( gpr, gpr);
 */
#define sx_addcu(a, b, c) { \
  a = __builtin_sx_addcu(b, c); \
}

/*  addc gpr, gpr, gpr
 * gpr addc( gpr, gpr);
 */
#define sx_addc(a, b, c) { \
  a = __builtin_sx_addc(b, c); \
}

/*  addhp gpr, gpr, gpr
 * gpr addhp( gpr, gpr);
 */
#define sx_addhp_r(a, b, c) { \
  a = __builtin_sx_addhp_r(b, c); \
}

/*  addhp gpr, gpr, <u8>
 * gpr addhp( gpr, <u8>);
 */
#define sx_addhp_i8(a, b, c) { \
  a = __builtin_sx_addhp_i8(b, c); \
}

/*  addubp gpr, gpr, gpr
 * gpr addubp( gpr, gpr);
 */
#define sx_addubp_r(a, b, c) { \
  a = __builtin_sx_addubp_r(b, c); \
}

/*  addubp gpr, gpr, <u8>
 * gpr addubp( gpr, <u8>);
 */
#define sx_addubp_i8(a, b, c) { \
  a = __builtin_sx_addubp_i8(b, c); \
}

/*  addugp gpr, <u16>
 * gpr addugp( <u16>);
 */
#define sx_addugp(a, b) { \
  a = __builtin_sx_addugp(b); \
}

/*  adduhp gpr, gpr, gpr
 * gpr adduhp( gpr, gpr);
 */
#define sx_adduhp_r(a, b, c) { \
  a = __builtin_sx_adduhp_r(b, c); \
}

/*  adduhp gpr, gpr, <u8>
 * gpr adduhp( gpr, <u8>);
 */
#define sx_adduhp_i8(a, b, c) { \
  a = __builtin_sx_adduhp_i8(b, c); \
}

/*  addur gpr, gpr
 * gpr addur( gpr);
 */
#define sx_addur_r(a, b) { \
  a = __builtin_sx_addur_r(b); \
}

/*  addur gpr, <s16>
 * gpr addur( <s16>);
 */
#define sx_addur_i16(a, b) { \
  a = __builtin_sx_addur_i16(b); \
}

/*  addu gpr, gpr, gpr
 * gpr addu( gpr, gpr);
 */
#define sx_addu_r(a, b, c) { \
  a = __builtin_sx_addu_r(b, c); \
}

/*  addu gpr, gpr, <u8>
 * gpr addu( gpr, <u8>);
 */
#define sx_addu_i8(a, b, c) { \
  a = __builtin_sx_addu_i8(b, c); \
}

/*  add gpr, gpr, gpr
 * gpr add( gpr, gpr);
 */
#define sx_add_r(a, b, c) { \
  a = __builtin_sx_add_r(b, c); \
}

/*  add gpr, gpr, <u8>
 * gpr add( gpr, <u8>);
 */
#define sx_add_i8(a, b, c) { \
  a = __builtin_sx_add_i8(b, c); \
}

/*  andg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andg( gr, gr);
 */
#define sx_andg(a, b, c) { \
  a = __builtin_sx_andg(b, c); \
}

/*  andng ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andng( gr, gr);
 */
#define sx_andng(a, b, c) { \
  a = __builtin_sx_andng(b, c); \
}

/*  andn gpr, gpr, gpr
 * gpr andn( gpr, gpr);
 */
#define sx_andn(a, b, c) { \
  a = __builtin_sx_andn(b, c); \
}

/*  and gpr, gpr, gpr
 * gpr and( gpr, gpr);
 */
#define sx_and_r(a, b, c) { \
  a = __builtin_sx_and_r(b, c); \
}

/*  and gpr, gpr, <u8>
 * gpr and( gpr, <u8>);
 */
#define sx_and_i8(a, b, c) { \
  a = __builtin_sx_and_i8(b, c); \
}

/* barrier
 * barrier( );
 */
#define sx_barrier() { \
  __builtin_sx_barrier(); \
}

/*  bclr gpr, gpr, gpr
 * gpr bclr( gpr, gpr);
 */
#define sx_bclr_r(a, b, c) { \
  a = __builtin_sx_bclr_r(b, c); \
}

/*  bclr gpr, gpr, <u5>
 * gpr bclr( gpr, <u5>);
 */
#define sx_bclr_i5(a, b, c) { \
  a = __builtin_sx_bclr_i5(b, c); \
}

/* bkp
 * bkp( );
 */
#define sx_bkp() { \
  __builtin_sx_bkp(); \
}

/*  bnot gpr, gpr, gpr
 * gpr bnot( gpr, gpr);
 */
#define sx_bnot_r(a, b, c) { \
  a = __builtin_sx_bnot_r(b, c); \
}

/*  bnot gpr, gpr, <u5>
 * gpr bnot( gpr, <u5>);
 */
#define sx_bnot_i5(a, b, c) { \
  a = __builtin_sx_bnot_i5(b, c); \
}

/*  boolbp gpr, gr
 * gpr boolbp( gr);
 */
#define sx_boolbp(a, b) { \
  a = __builtin_sx_boolbp(b); \
}

/*  boolhp gpr, gr
 * gpr boolhp( gr);
 */
#define sx_boolhp(a, b) { \
  a = __builtin_sx_boolhp(b); \
}

/*  bool gpr, gr
 * gpr bool( gr);
 */
#define sx_bool(a, b) { \
  a = __builtin_sx_bool(b); \
}

/*  bset gpr, gpr, gpr
 * gpr bset( gpr, gpr);
 */
#define sx_bset_r(a, b, c) { \
  a = __builtin_sx_bset_r(b, c); \
}

/*  bset gpr, gpr, <u5>
 * gpr bset( gpr, <u5>);
 */
#define sx_bset_i5(a, b, c) { \
  a = __builtin_sx_bset_i5(b, c); \
}

/*  clamph gpr, gpr
 * gpr clamph( gpr);
 */
#define sx_clamph(a, b) { \
  a = __builtin_sx_clamph(b); \
}

/*  extb gpr, gpr
 * gpr extb( gpr);
 */
#define sx_extb(a, b) { \
  a = __builtin_sx_extb(b); \
}

/*  exth gpr, gpr
 * gpr exth( gpr);
 */
#define sx_exth(a, b) { \
  a = __builtin_sx_exth(b); \
}

/*  extub gpr, gpr
 * gpr extub( gpr);
 */
#define sx_extub(a, b) { \
  a = __builtin_sx_extub(b); \
}

/*  extuh gpr, gpr
 * gpr extuh( gpr);
 */
#define sx_extuh(a, b) { \
  a = __builtin_sx_extuh(b); \
}

/*  idle <u2>
 * idle( <u2>);
 */
#define sx_idle(a) { \
  __builtin_sx_idle(a); \
}

/* loopdis lr
 * lr loopdis( u[0..1]);
 */
#define sx_loopdis(a) { \
  __builtin_sx_loopdis(a); \
}

/* loopena lr
 * lr loopena( u[0..1]);
 */
#define sx_loopena(a) { \
  __builtin_sx_loopena(a); \
}

/*  lzc gpr, gpr
 * gpr lzc( gpr);
 */
#define sx_lzc(a, b) { \
  a = __builtin_sx_lzc(b); \
}

/*  makehp gpr, <s16>
 * gpr makehp( <s16>);
 */
#define sx_makehp(a, b) { \
  a = __builtin_sx_makehp(b); \
}

/*  make gpr, <s16>
 * gpr make( <s16>);
 */
#define sx_make(a, b) { \
  a = __builtin_sx_make(b); \
}

/*  maxbp gpr, gpr, gpr
 * gpr maxbp( gpr, gpr);
 */
#define sx_maxbp_r(a, b, c) { \
  a = __builtin_sx_maxbp_r(b, c); \
}

/*  maxbp gpr, gpr, <u8>
 * gpr maxbp( gpr, <u8>);
 */
#define sx_maxbp_i8(a, b, c) { \
  a = __builtin_sx_maxbp_i8(b, c); \
}

/*  maxhp gpr, gpr, gpr
 * gpr maxhp( gpr, gpr);
 */
#define sx_maxhp_r(a, b, c) { \
  a = __builtin_sx_maxhp_r(b, c); \
}

/*  maxhp gpr, gpr, <u8>
 * gpr maxhp( gpr, <u8>);
 */
#define sx_maxhp_i8(a, b, c) { \
  a = __builtin_sx_maxhp_i8(b, c); \
}

/*  maxubp gpr, gpr, gpr
 * gpr maxubp( gpr, gpr);
 */
#define sx_maxubp_r(a, b, c) { \
  a = __builtin_sx_maxubp_r(b, c); \
}

/*  maxubp gpr, gpr, <u8>
 * gpr maxubp( gpr, <u8>);
 */
#define sx_maxubp_i8(a, b, c) { \
  a = __builtin_sx_maxubp_i8(b, c); \
}

/*  maxuhp gpr, gpr, gpr
 * gpr maxuhp( gpr, gpr);
 */
#define sx_maxuhp_r(a, b, c) { \
  a = __builtin_sx_maxuhp_r(b, c); \
}

/*  maxuhp gpr, gpr, <u8>
 * gpr maxuhp( gpr, <u8>);
 */
#define sx_maxuhp_i8(a, b, c) { \
  a = __builtin_sx_maxuhp_i8(b, c); \
}

/*  maxu gpr, gpr, gpr
 * gpr maxu( gpr, gpr);
 */
#define sx_maxu_r(a, b, c) { \
  a = __builtin_sx_maxu_r(b, c); \
}

/*  maxu gpr, gpr, <u8>
 * gpr maxu( gpr, <u8>);
 */
#define sx_maxu_i8(a, b, c) { \
  a = __builtin_sx_maxu_i8(b, c); \
}

/*  max gpr, gpr, gpr
 * gpr max( gpr, gpr);
 */
#define sx_max_r(a, b, c) { \
  a = __builtin_sx_max_r(b, c); \
}

/*  max gpr, gpr, <u8>
 * gpr max( gpr, <u8>);
 */
#define sx_max_i8(a, b, c) { \
  a = __builtin_sx_max_i8(b, c); \
}

/*  minbp gpr, gpr, gpr
 * gpr minbp( gpr, gpr);
 */
#define sx_minbp_r(a, b, c) { \
  a = __builtin_sx_minbp_r(b, c); \
}

/*  minbp gpr, gpr, <u8>
 * gpr minbp( gpr, <u8>);
 */
#define sx_minbp_i8(a, b, c) { \
  a = __builtin_sx_minbp_i8(b, c); \
}

/*  minhp gpr, gpr, gpr
 * gpr minhp( gpr, gpr);
 */
#define sx_minhp_r(a, b, c) { \
  a = __builtin_sx_minhp_r(b, c); \
}

/*  minhp gpr, gpr, <u8>
 * gpr minhp( gpr, <u8>);
 */
#define sx_minhp_i8(a, b, c) { \
  a = __builtin_sx_minhp_i8(b, c); \
}

/*  minubp gpr, gpr, gpr
 * gpr minubp( gpr, gpr);
 */
#define sx_minubp_r(a, b, c) { \
  a = __builtin_sx_minubp_r(b, c); \
}

/*  minubp gpr, gpr, <u8>
 * gpr minubp( gpr, <u8>);
 */
#define sx_minubp_i8(a, b, c) { \
  a = __builtin_sx_minubp_i8(b, c); \
}

/*  minuhp gpr, gpr, gpr
 * gpr minuhp( gpr, gpr);
 */
#define sx_minuhp_r(a, b, c) { \
  a = __builtin_sx_minuhp_r(b, c); \
}

/*  minuhp gpr, gpr, <u8>
 * gpr minuhp( gpr, <u8>);
 */
#define sx_minuhp_i8(a, b, c) { \
  a = __builtin_sx_minuhp_i8(b, c); \
}

/*  minu gpr, gpr, gpr
 * gpr minu( gpr, gpr);
 */
#define sx_minu_r(a, b, c) { \
  a = __builtin_sx_minu_r(b, c); \
}

/*  minu gpr, gpr, <u8>
 * gpr minu( gpr, <u8>);
 */
#define sx_minu_i8(a, b, c) { \
  a = __builtin_sx_minu_i8(b, c); \
}

/*  min gpr, gpr, gpr
 * gpr min( gpr, gpr);
 */
#define sx_min_r(a, b, c) { \
  a = __builtin_sx_min_r(b, c); \
}

/*  min gpr, gpr, <u8>
 * gpr min( gpr, <u8>);
 */
#define sx_min_i8(a, b, c) { \
  a = __builtin_sx_min_i8(b, c); \
}

/*  more gpr, <u16>
 * gpr more( gpr, <u16>);
 */
#define sx_more(a, b) { \
  a = __builtin_sx_more(a, b); \
}

/*  moveh2h gpr, gpr
 * gpr moveh2h( gpr, gpr);
 */
#define sx_moveh2h(a, b) { \
  a = __builtin_sx_moveh2h(a, b); \
}

/*  moveh2l gpr, gpr
 * gpr moveh2l( gpr, gpr);
 */
#define sx_moveh2l(a, b) { \
  a = __builtin_sx_moveh2l(a, b); \
}

/*  movel2h gpr, gpr
 * gpr movel2h( gpr, gpr);
 */
#define sx_movel2h(a, b) { \
  a = __builtin_sx_movel2h(a, b); \
}

/*  movel2l gpr, gpr
 * gpr movel2l( gpr, gpr);
 */
#define sx_movel2l(a, b) { \
  a = __builtin_sx_movel2l(a, b); \
}

/*  mover2sfr sfr, gpr
 * sfr mover2sfr( u[0..31], gpr);
 */
#define sx_mover2sfr(a, b) { \
  __builtin_sx_mover2sfr(a, b); \
}

/*  movesfr2r gpr, sfr
 * gpr movesfr2r( u[0..31]);
 */
#define sx_movesfr2r(a, b) { \
  a = __builtin_sx_movesfr2r(b); \
}

/*  negbp gpr, gpr
 * gpr negbp( gpr);
 */
#define sx_negbp(a, b) { \
  a = __builtin_sx_negbp(b); \
}

/*  neghp gpr, gpr
 * gpr neghp( gpr);
 */
#define sx_neghp(a, b) { \
  a = __builtin_sx_neghp(b); \
}

/*  negubp gpr, gpr
 * gpr negubp( gpr);
 */
#define sx_negubp(a, b) { \
  a = __builtin_sx_negubp(b); \
}

/*  neguhp gpr, gpr
 * gpr neguhp( gpr);
 */
#define sx_neguhp(a, b) { \
  a = __builtin_sx_neguhp(b); \
}

/*  negu gpr, gpr
 * gpr negu( gpr);
 */
#define sx_negu(a, b) { \
  a = __builtin_sx_negu(b); \
}

/*  neg gpr, gpr
 * gpr neg( gpr);
 */
#define sx_neg(a, b) { \
  a = __builtin_sx_neg(b); \
}

/* nop
 * nop( );
 */
#define sx_nop() { \
  __builtin_sx_nop(); \
}

/*  norn gpr, gpr, <u8>
 * gpr norn( gpr, <u8>);
 */
#define sx_norn(a, b, c) { \
  a = __builtin_sx_norn(b, c); \
}

/*  not gpr, gpr
 * gpr not( gpr);
 */
#define sx_not(a, b) { \
  a = __builtin_sx_not(b); \
}

/*  org ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst org( gr, gr);
 */
#define sx_org(a, b, c) { \
  a = __builtin_sx_org(b, c); \
}

/*  or gpr, gpr, gpr
 * gpr or( gpr, gpr);
 */
#define sx_or_r(a, b, c) { \
  a = __builtin_sx_or_r(b, c); \
}

/*  or gpr, gpr, <u8>
 * gpr or( gpr, <u8>);
 */
#define sx_or_i8(a, b, c) { \
  a = __builtin_sx_or_i8(b, c); \
}

/*  prior gpr, gpr
 * gpr prior( gpr);
 */
#define sx_prior(a, b) { \
  a = __builtin_sx_prior(b); \
}

/*  rotl gpr, gpr, gpr
 * gpr rotl( gpr, gpr);
 */
#define sx_rotl_r(a, b, c) { \
  a = __builtin_sx_rotl_r(b, c); \
}

/*  rotl gpr, gpr, <u5>
 * gpr rotl( gpr, <u5>);
 */
#define sx_rotl_i5(a, b, c) { \
  a = __builtin_sx_rotl_i5(b, c); \
}

/* rte
 * rte( );
 */
#define sx_rte() { \
  __builtin_sx_rte(); \
}

/*  shlmu gpr, gpr
 * gpr shlmu( gpr);
 */
#define sx_shlmu_r(a, b) { \
  a = __builtin_sx_shlmu_r(b); \
}

/*  shlmu gpr, <u5>
 * gpr shlmu( <u5>);
 */
#define sx_shlmu_i5(a, b) { \
  a = __builtin_sx_shlmu_i5(b); \
}

/*  shlnu gpr, gpr, gpr
 * gpr shlnu( gpr, gpr);
 */
#define sx_shlnu(a, b, c) { \
  a = __builtin_sx_shlnu(b, c); \
}

/*  shlu gpr, gpr, gpr
 * gpr shlu( gpr, gpr);
 */
#define sx_shlu_r(a, b, c) { \
  a = __builtin_sx_shlu_r(b, c); \
}

/*  shlu gpr, gpr, <u5>
 * gpr shlu( gpr, <u5>);
 */
#define sx_shlu_i5(a, b, c) { \
  a = __builtin_sx_shlu_i5(b, c); \
}

/*  shl gpr, gpr, gpr
 * gpr shl( gpr, gpr);
 */
#define sx_shl_r(a, b, c) { \
  a = __builtin_sx_shl_r(b, c); \
}

/*  shl gpr, gpr, <u5>
 * gpr shl( gpr, <u5>);
 */
#define sx_shl_i5(a, b, c) { \
  a = __builtin_sx_shl_i5(b, c); \
}

/*  shrmu gpr, gpr
 * gpr shrmu( gpr);
 */
#define sx_shrmu_r(a, b) { \
  a = __builtin_sx_shrmu_r(b); \
}

/*  shrmu gpr, <u5>
 * gpr shrmu( <u5>);
 */
#define sx_shrmu_i5(a, b) { \
  a = __builtin_sx_shrmu_i5(b); \
}

/*  shrnu gpr, gpr, gpr
 * gpr shrnu( gpr, gpr);
 */
#define sx_shrnu(a, b, c) { \
  a = __builtin_sx_shrnu(b, c); \
}

/*  shru gpr, gpr, gpr
 * gpr shru( gpr, gpr);
 */
#define sx_shru_r(a, b, c) { \
  a = __builtin_sx_shru_r(b, c); \
}

/*  shru gpr, gpr, <u5>
 * gpr shru( gpr, <u5>);
 */
#define sx_shru_i5(a, b, c) { \
  a = __builtin_sx_shru_i5(b, c); \
}

/*  shr gpr, gpr, gpr
 * gpr shr( gpr, gpr);
 */
#define sx_shr_r(a, b, c) { \
  a = __builtin_sx_shr_r(b, c); \
}

/*  shr gpr, gpr, <u5>
 * gpr shr( gpr, <u5>);
 */
#define sx_shr_i5(a, b, c) { \
  a = __builtin_sx_shr_i5(b, c); \
}

/*  subbp gpr, gpr, gpr
 * gpr subbp( gpr, gpr);
 */
#define sx_subbp_r(a, b, c) { \
  a = __builtin_sx_subbp_r(b, c); \
}

/*  subbp gpr, gpr, <u8>
 * gpr subbp( gpr, <u8>);
 */
#define sx_subbp_i8(a, b, c) { \
  a = __builtin_sx_subbp_i8(b, c); \
}

/*  subcu gpr, gpr, gpr
 * gpr subcu( gpr, gpr);
 */
#define sx_subcu(a, b, c) { \
  a = __builtin_sx_subcu(b, c); \
}

/*  subc gpr, gpr, gpr
 * gpr subc( gpr, gpr);
 */
#define sx_subc(a, b, c) { \
  a = __builtin_sx_subc(b, c); \
}

/*  subhp gpr, gpr, gpr
 * gpr subhp( gpr, gpr);
 */
#define sx_subhp_r(a, b, c) { \
  a = __builtin_sx_subhp_r(b, c); \
}

/*  subhp gpr, gpr, <u8>
 * gpr subhp( gpr, <u8>);
 */
#define sx_subhp_i8(a, b, c) { \
  a = __builtin_sx_subhp_i8(b, c); \
}

/*  sububp gpr, gpr, gpr
 * gpr sububp( gpr, gpr);
 */
#define sx_sububp_r(a, b, c) { \
  a = __builtin_sx_sububp_r(b, c); \
}

/*  sububp gpr, gpr, <u8>
 * gpr sububp( gpr, <u8>);
 */
#define sx_sububp_i8(a, b, c) { \
  a = __builtin_sx_sububp_i8(b, c); \
}

/*  subuhp gpr, gpr, gpr
 * gpr subuhp( gpr, gpr);
 */
#define sx_subuhp_r(a, b, c) { \
  a = __builtin_sx_subuhp_r(b, c); \
}

/*  subuhp gpr, gpr, <u8>
 * gpr subuhp( gpr, <u8>);
 */
#define sx_subuhp_i8(a, b, c) { \
  a = __builtin_sx_subuhp_i8(b, c); \
}

/*  subu gpr, gpr, gpr
 * gpr subu( gpr, gpr);
 */
#define sx_subu_r(a, b, c) { \
  a = __builtin_sx_subu_r(b, c); \
}

/*  subu gpr, gpr, <u8>
 * gpr subu( gpr, <u8>);
 */
#define sx_subu_i8(a, b, c) { \
  a = __builtin_sx_subu_i8(b, c); \
}

/*  sub gpr, gpr, gpr
 * gpr sub( gpr, gpr);
 */
#define sx_sub_r(a, b, c) { \
  a = __builtin_sx_sub_r(b, c); \
}

/*  sub gpr, gpr, <u8>
 * gpr sub( gpr, <u8>);
 */
#define sx_sub_i8(a, b, c) { \
  a = __builtin_sx_sub_i8(b, c); \
}

/*  tfandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfandn( gpr, <u8>);
 */
#define sx_tfandn(a, b, c) { \
  a = __builtin_sx_tfandn(b, c); \
}

/*  tfand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfand( gpr, <u8>);
 */
#define sx_tfand(a, b, c) { \
  a = __builtin_sx_tfand(b, c); \
}

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, gpr);
 */
#define sx_tfbit_r(a, b, c) { \
  a = __builtin_sx_tfbit_r(b, c); \
}

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, <u5>);
 */
#define sx_tfbit_i5(a, b, c) { \
  a = __builtin_sx_tfbit_i5(b, c); \
}

/* trap <u2>
 * trap( <u2>);
 */
#define sx_trap(a) { \
  __builtin_sx_trap(a); \
}

/*  ttandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttandn( gpr, <u8>);
 */
#define sx_ttandn(a, b, c) { \
  a = __builtin_sx_ttandn(b, c); \
}

/*  ttand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttand( gpr, <u8>);
 */
#define sx_ttand(a, b, c) { \
  a = __builtin_sx_ttand(b, c); \
}

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, gpr);
 */
#define sx_ttbit_r(a, b, c) { \
  a = __builtin_sx_ttbit_r(b, c); \
}

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, <u5>);
 */
#define sx_ttbit_i5(a, b, c) { \
  a = __builtin_sx_ttbit_i5(b, c); \
}

/*  xorg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst xorg( gr, gr);
 */
#define sx_xorg(a, b, c) { \
  a = __builtin_sx_xorg(b, c); \
}

/*  xor gpr, gpr, gpr
 * gpr xor( gpr, gpr);
 */
#define sx_xor_r(a, b, c) { \
  a = __builtin_sx_xor_r(b, c); \
}

/*  xor gpr, gpr, <u8>
 * gpr xor( gpr, <u8>);
 */
#define sx_xor_i8(a, b, c) { \
  a = __builtin_sx_xor_i8(b, c); \
}

#else /* !defined(__SX) || defined(__SX_C_MODEL) */

/* Macros mapped on C models for class sx */
#include <builtins_model_sx.h>

/*  absbp gpr, gpr
 * gpr absbp( gpr);
 */
#define sx_absbp(a, b) { \
  a = __cmodel_sx_absbp(b); \
}

/*  abshp gpr, gpr
 * gpr abshp( gpr);
 */
#define sx_abshp(a, b) { \
  a = __cmodel_sx_abshp(b); \
}

/*  absubp gpr, gpr
 * gpr absubp( gpr);
 */
#define sx_absubp(a, b) { \
  a = __cmodel_sx_absubp(b); \
}

/*  absuhp gpr, gpr
 * gpr absuhp( gpr);
 */
#define sx_absuhp(a, b) { \
  a = __cmodel_sx_absuhp(b); \
}

/*  absu gpr, gpr
 * gpr absu( gpr);
 */
#define sx_absu(a, b) { \
  a = __cmodel_sx_absu(b); \
}

/*  abs gpr, gpr
 * gpr abs( gpr);
 */
#define sx_abs(a, b) { \
  a = __cmodel_sx_abs(b); \
}

/*  addbp gpr, gpr, gpr
 * gpr addbp( gpr, gpr);
 */
#define sx_addbp_r(a, b, c) { \
  a = __cmodel_sx_addbp_r(b, c); \
}

/*  addbp gpr, gpr, <u8>
 * gpr addbp( gpr, <u8>);
 */
#define sx_addbp_i8(a, b, c) { \
  a = __cmodel_sx_addbp_i8(b, c); \
}

/*  addcu gpr, gpr, gpr
 * gpr addcu( gpr, gpr);
 */
#define sx_addcu(a, b, c) { \
  a = __cmodel_sx_addcu(b, c); \
}

/*  addc gpr, gpr, gpr
 * gpr addc( gpr, gpr);
 */
#define sx_addc(a, b, c) { \
  a = __cmodel_sx_addc(b, c); \
}

/*  addhp gpr, gpr, gpr
 * gpr addhp( gpr, gpr);
 */
#define sx_addhp_r(a, b, c) { \
  a = __cmodel_sx_addhp_r(b, c); \
}

/*  addhp gpr, gpr, <u8>
 * gpr addhp( gpr, <u8>);
 */
#define sx_addhp_i8(a, b, c) { \
  a = __cmodel_sx_addhp_i8(b, c); \
}

/*  addubp gpr, gpr, gpr
 * gpr addubp( gpr, gpr);
 */
#define sx_addubp_r(a, b, c) { \
  a = __cmodel_sx_addubp_r(b, c); \
}

/*  addubp gpr, gpr, <u8>
 * gpr addubp( gpr, <u8>);
 */
#define sx_addubp_i8(a, b, c) { \
  a = __cmodel_sx_addubp_i8(b, c); \
}

/*  addugp gpr, <u16>
 * gpr addugp( <u16>);
 */
#define sx_addugp(a, b) { \
  a = __cmodel_sx_addugp(b); \
}

/*  adduhp gpr, gpr, gpr
 * gpr adduhp( gpr, gpr);
 */
#define sx_adduhp_r(a, b, c) { \
  a = __cmodel_sx_adduhp_r(b, c); \
}

/*  adduhp gpr, gpr, <u8>
 * gpr adduhp( gpr, <u8>);
 */
#define sx_adduhp_i8(a, b, c) { \
  a = __cmodel_sx_adduhp_i8(b, c); \
}

/*  addur gpr, gpr
 * gpr addur( gpr);
 */
#define sx_addur_r(a, b) { \
  a = __cmodel_sx_addur_r(b); \
}

/*  addur gpr, <s16>
 * gpr addur( <s16>);
 */
#define sx_addur_i16(a, b) { \
  a = __cmodel_sx_addur_i16(b); \
}

/*  addu gpr, gpr, gpr
 * gpr addu( gpr, gpr);
 */
#define sx_addu_r(a, b, c) { \
  a = __cmodel_sx_addu_r(b, c); \
}

/*  addu gpr, gpr, <u8>
 * gpr addu( gpr, <u8>);
 */
#define sx_addu_i8(a, b, c) { \
  a = __cmodel_sx_addu_i8(b, c); \
}

/*  add gpr, gpr, gpr
 * gpr add( gpr, gpr);
 */
#define sx_add_r(a, b, c) { \
  a = __cmodel_sx_add_r(b, c); \
}

/*  add gpr, gpr, <u8>
 * gpr add( gpr, <u8>);
 */
#define sx_add_i8(a, b, c) { \
  a = __cmodel_sx_add_i8(b, c); \
}

/*  andg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andg( gr, gr);
 */
#define sx_andg(a, b, c) { \
  a = __cmodel_sx_andg(b, c); \
}

/*  andng ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andng( gr, gr);
 */
#define sx_andng(a, b, c) { \
  a = __cmodel_sx_andng(b, c); \
}

/*  andn gpr, gpr, gpr
 * gpr andn( gpr, gpr);
 */
#define sx_andn(a, b, c) { \
  a = __cmodel_sx_andn(b, c); \
}

/*  and gpr, gpr, gpr
 * gpr and( gpr, gpr);
 */
#define sx_and_r(a, b, c) { \
  a = __cmodel_sx_and_r(b, c); \
}

/*  and gpr, gpr, <u8>
 * gpr and( gpr, <u8>);
 */
#define sx_and_i8(a, b, c) { \
  a = __cmodel_sx_and_i8(b, c); \
}

/* barrier
 * barrier( );
 */
#define sx_barrier() { \
  __cmodel_sx_barrier(); \
}

/*  bclr gpr, gpr, gpr
 * gpr bclr( gpr, gpr);
 */
#define sx_bclr_r(a, b, c) { \
  a = __cmodel_sx_bclr_r(b, c); \
}

/*  bclr gpr, gpr, <u5>
 * gpr bclr( gpr, <u5>);
 */
#define sx_bclr_i5(a, b, c) { \
  a = __cmodel_sx_bclr_i5(b, c); \
}

/* bkp
 * bkp( );
 */
#define sx_bkp() { \
  __cmodel_sx_bkp(); \
}

/*  bnot gpr, gpr, gpr
 * gpr bnot( gpr, gpr);
 */
#define sx_bnot_r(a, b, c) { \
  a = __cmodel_sx_bnot_r(b, c); \
}

/*  bnot gpr, gpr, <u5>
 * gpr bnot( gpr, <u5>);
 */
#define sx_bnot_i5(a, b, c) { \
  a = __cmodel_sx_bnot_i5(b, c); \
}

/*  boolbp gpr, gr
 * gpr boolbp( gr);
 */
#define sx_boolbp(a, b) { \
  a = __cmodel_sx_boolbp(b); \
}

/*  boolhp gpr, gr
 * gpr boolhp( gr);
 */
#define sx_boolhp(a, b) { \
  a = __cmodel_sx_boolhp(b); \
}

/*  bool gpr, gr
 * gpr bool( gr);
 */
#define sx_bool(a, b) { \
  a = __cmodel_sx_bool(b); \
}

/*  bset gpr, gpr, gpr
 * gpr bset( gpr, gpr);
 */
#define sx_bset_r(a, b, c) { \
  a = __cmodel_sx_bset_r(b, c); \
}

/*  bset gpr, gpr, <u5>
 * gpr bset( gpr, <u5>);
 */
#define sx_bset_i5(a, b, c) { \
  a = __cmodel_sx_bset_i5(b, c); \
}

/*  clamph gpr, gpr
 * gpr clamph( gpr);
 */
#define sx_clamph(a, b) { \
  a = __cmodel_sx_clamph(b); \
}

/*  extb gpr, gpr
 * gpr extb( gpr);
 */
#define sx_extb(a, b) { \
  a = __cmodel_sx_extb(b); \
}

/*  exth gpr, gpr
 * gpr exth( gpr);
 */
#define sx_exth(a, b) { \
  a = __cmodel_sx_exth(b); \
}

/*  extub gpr, gpr
 * gpr extub( gpr);
 */
#define sx_extub(a, b) { \
  a = __cmodel_sx_extub(b); \
}

/*  extuh gpr, gpr
 * gpr extuh( gpr);
 */
#define sx_extuh(a, b) { \
  a = __cmodel_sx_extuh(b); \
}

/*  idle <u2>
 * idle( <u2>);
 */
#define sx_idle(a) { \
  __cmodel_sx_idle(a); \
}

/* loopdis lr
 * lr loopdis( u[0..1]);
 */
#define sx_loopdis(a) { \
  __cmodel_sx_loopdis(a); \
}

/* loopena lr
 * lr loopena( u[0..1]);
 */
#define sx_loopena(a) { \
  __cmodel_sx_loopena(a); \
}

/*  lzc gpr, gpr
 * gpr lzc( gpr);
 */
#define sx_lzc(a, b) { \
  a = __cmodel_sx_lzc(b); \
}

/*  makehp gpr, <s16>
 * gpr makehp( <s16>);
 */
#define sx_makehp(a, b) { \
  a = __cmodel_sx_makehp(b); \
}

/*  make gpr, <s16>
 * gpr make( <s16>);
 */
#define sx_make(a, b) { \
  a = __cmodel_sx_make(b); \
}

/*  maxbp gpr, gpr, gpr
 * gpr maxbp( gpr, gpr);
 */
#define sx_maxbp_r(a, b, c) { \
  a = __cmodel_sx_maxbp_r(b, c); \
}

/*  maxbp gpr, gpr, <u8>
 * gpr maxbp( gpr, <u8>);
 */
#define sx_maxbp_i8(a, b, c) { \
  a = __cmodel_sx_maxbp_i8(b, c); \
}

/*  maxhp gpr, gpr, gpr
 * gpr maxhp( gpr, gpr);
 */
#define sx_maxhp_r(a, b, c) { \
  a = __cmodel_sx_maxhp_r(b, c); \
}

/*  maxhp gpr, gpr, <u8>
 * gpr maxhp( gpr, <u8>);
 */
#define sx_maxhp_i8(a, b, c) { \
  a = __cmodel_sx_maxhp_i8(b, c); \
}

/*  maxubp gpr, gpr, gpr
 * gpr maxubp( gpr, gpr);
 */
#define sx_maxubp_r(a, b, c) { \
  a = __cmodel_sx_maxubp_r(b, c); \
}

/*  maxubp gpr, gpr, <u8>
 * gpr maxubp( gpr, <u8>);
 */
#define sx_maxubp_i8(a, b, c) { \
  a = __cmodel_sx_maxubp_i8(b, c); \
}

/*  maxuhp gpr, gpr, gpr
 * gpr maxuhp( gpr, gpr);
 */
#define sx_maxuhp_r(a, b, c) { \
  a = __cmodel_sx_maxuhp_r(b, c); \
}

/*  maxuhp gpr, gpr, <u8>
 * gpr maxuhp( gpr, <u8>);
 */
#define sx_maxuhp_i8(a, b, c) { \
  a = __cmodel_sx_maxuhp_i8(b, c); \
}

/*  maxu gpr, gpr, gpr
 * gpr maxu( gpr, gpr);
 */
#define sx_maxu_r(a, b, c) { \
  a = __cmodel_sx_maxu_r(b, c); \
}

/*  maxu gpr, gpr, <u8>
 * gpr maxu( gpr, <u8>);
 */
#define sx_maxu_i8(a, b, c) { \
  a = __cmodel_sx_maxu_i8(b, c); \
}

/*  max gpr, gpr, gpr
 * gpr max( gpr, gpr);
 */
#define sx_max_r(a, b, c) { \
  a = __cmodel_sx_max_r(b, c); \
}

/*  max gpr, gpr, <u8>
 * gpr max( gpr, <u8>);
 */
#define sx_max_i8(a, b, c) { \
  a = __cmodel_sx_max_i8(b, c); \
}

/*  minbp gpr, gpr, gpr
 * gpr minbp( gpr, gpr);
 */
#define sx_minbp_r(a, b, c) { \
  a = __cmodel_sx_minbp_r(b, c); \
}

/*  minbp gpr, gpr, <u8>
 * gpr minbp( gpr, <u8>);
 */
#define sx_minbp_i8(a, b, c) { \
  a = __cmodel_sx_minbp_i8(b, c); \
}

/*  minhp gpr, gpr, gpr
 * gpr minhp( gpr, gpr);
 */
#define sx_minhp_r(a, b, c) { \
  a = __cmodel_sx_minhp_r(b, c); \
}

/*  minhp gpr, gpr, <u8>
 * gpr minhp( gpr, <u8>);
 */
#define sx_minhp_i8(a, b, c) { \
  a = __cmodel_sx_minhp_i8(b, c); \
}

/*  minubp gpr, gpr, gpr
 * gpr minubp( gpr, gpr);
 */
#define sx_minubp_r(a, b, c) { \
  a = __cmodel_sx_minubp_r(b, c); \
}

/*  minubp gpr, gpr, <u8>
 * gpr minubp( gpr, <u8>);
 */
#define sx_minubp_i8(a, b, c) { \
  a = __cmodel_sx_minubp_i8(b, c); \
}

/*  minuhp gpr, gpr, gpr
 * gpr minuhp( gpr, gpr);
 */
#define sx_minuhp_r(a, b, c) { \
  a = __cmodel_sx_minuhp_r(b, c); \
}

/*  minuhp gpr, gpr, <u8>
 * gpr minuhp( gpr, <u8>);
 */
#define sx_minuhp_i8(a, b, c) { \
  a = __cmodel_sx_minuhp_i8(b, c); \
}

/*  minu gpr, gpr, gpr
 * gpr minu( gpr, gpr);
 */
#define sx_minu_r(a, b, c) { \
  a = __cmodel_sx_minu_r(b, c); \
}

/*  minu gpr, gpr, <u8>
 * gpr minu( gpr, <u8>);
 */
#define sx_minu_i8(a, b, c) { \
  a = __cmodel_sx_minu_i8(b, c); \
}

/*  min gpr, gpr, gpr
 * gpr min( gpr, gpr);
 */
#define sx_min_r(a, b, c) { \
  a = __cmodel_sx_min_r(b, c); \
}

/*  min gpr, gpr, <u8>
 * gpr min( gpr, <u8>);
 */
#define sx_min_i8(a, b, c) { \
  a = __cmodel_sx_min_i8(b, c); \
}

/*  more gpr, <u16>
 * gpr more( gpr, <u16>);
 */
#define sx_more(a, b) { \
  a = __cmodel_sx_more(a, b); \
}

/*  moveh2h gpr, gpr
 * gpr moveh2h( gpr, gpr);
 */
#define sx_moveh2h(a, b) { \
  a = __cmodel_sx_moveh2h(a, b); \
}

/*  moveh2l gpr, gpr
 * gpr moveh2l( gpr, gpr);
 */
#define sx_moveh2l(a, b) { \
  a = __cmodel_sx_moveh2l(a, b); \
}

/*  movel2h gpr, gpr
 * gpr movel2h( gpr, gpr);
 */
#define sx_movel2h(a, b) { \
  a = __cmodel_sx_movel2h(a, b); \
}

/*  movel2l gpr, gpr
 * gpr movel2l( gpr, gpr);
 */
#define sx_movel2l(a, b) { \
  a = __cmodel_sx_movel2l(a, b); \
}

/*  mover2sfr sfr, gpr
 * sfr mover2sfr( u[0..31], gpr);
 */
#define sx_mover2sfr(a, b) { \
  __cmodel_sx_mover2sfr(a, b); \
}

/*  movesfr2r gpr, sfr
 * gpr movesfr2r( u[0..31]);
 */
#define sx_movesfr2r(a, b) { \
  a = __cmodel_sx_movesfr2r(b); \
}

/*  negbp gpr, gpr
 * gpr negbp( gpr);
 */
#define sx_negbp(a, b) { \
  a = __cmodel_sx_negbp(b); \
}

/*  neghp gpr, gpr
 * gpr neghp( gpr);
 */
#define sx_neghp(a, b) { \
  a = __cmodel_sx_neghp(b); \
}

/*  negubp gpr, gpr
 * gpr negubp( gpr);
 */
#define sx_negubp(a, b) { \
  a = __cmodel_sx_negubp(b); \
}

/*  neguhp gpr, gpr
 * gpr neguhp( gpr);
 */
#define sx_neguhp(a, b) { \
  a = __cmodel_sx_neguhp(b); \
}

/*  negu gpr, gpr
 * gpr negu( gpr);
 */
#define sx_negu(a, b) { \
  a = __cmodel_sx_negu(b); \
}

/*  neg gpr, gpr
 * gpr neg( gpr);
 */
#define sx_neg(a, b) { \
  a = __cmodel_sx_neg(b); \
}

/* nop
 * nop( );
 */
#define sx_nop() { \
  __cmodel_sx_nop(); \
}

/*  norn gpr, gpr, <u8>
 * gpr norn( gpr, <u8>);
 */
#define sx_norn(a, b, c) { \
  a = __cmodel_sx_norn(b, c); \
}

/*  not gpr, gpr
 * gpr not( gpr);
 */
#define sx_not(a, b) { \
  a = __cmodel_sx_not(b); \
}

/*  org ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst org( gr, gr);
 */
#define sx_org(a, b, c) { \
  a = __cmodel_sx_org(b, c); \
}

/*  or gpr, gpr, gpr
 * gpr or( gpr, gpr);
 */
#define sx_or_r(a, b, c) { \
  a = __cmodel_sx_or_r(b, c); \
}

/*  or gpr, gpr, <u8>
 * gpr or( gpr, <u8>);
 */
#define sx_or_i8(a, b, c) { \
  a = __cmodel_sx_or_i8(b, c); \
}

/*  prior gpr, gpr
 * gpr prior( gpr);
 */
#define sx_prior(a, b) { \
  a = __cmodel_sx_prior(b); \
}

/*  rotl gpr, gpr, gpr
 * gpr rotl( gpr, gpr);
 */
#define sx_rotl_r(a, b, c) { \
  a = __cmodel_sx_rotl_r(b, c); \
}

/*  rotl gpr, gpr, <u5>
 * gpr rotl( gpr, <u5>);
 */
#define sx_rotl_i5(a, b, c) { \
  a = __cmodel_sx_rotl_i5(b, c); \
}

/* rte
 * rte( );
 */
#define sx_rte() { \
  __cmodel_sx_rte(); \
}

/*  shlmu gpr, gpr
 * gpr shlmu( gpr);
 */
#define sx_shlmu_r(a, b) { \
  a = __cmodel_sx_shlmu_r(b); \
}

/*  shlmu gpr, <u5>
 * gpr shlmu( <u5>);
 */
#define sx_shlmu_i5(a, b) { \
  a = __cmodel_sx_shlmu_i5(b); \
}

/*  shlnu gpr, gpr, gpr
 * gpr shlnu( gpr, gpr);
 */
#define sx_shlnu(a, b, c) { \
  a = __cmodel_sx_shlnu(b, c); \
}

/*  shlu gpr, gpr, gpr
 * gpr shlu( gpr, gpr);
 */
#define sx_shlu_r(a, b, c) { \
  a = __cmodel_sx_shlu_r(b, c); \
}

/*  shlu gpr, gpr, <u5>
 * gpr shlu( gpr, <u5>);
 */
#define sx_shlu_i5(a, b, c) { \
  a = __cmodel_sx_shlu_i5(b, c); \
}

/*  shl gpr, gpr, gpr
 * gpr shl( gpr, gpr);
 */
#define sx_shl_r(a, b, c) { \
  a = __cmodel_sx_shl_r(b, c); \
}

/*  shl gpr, gpr, <u5>
 * gpr shl( gpr, <u5>);
 */
#define sx_shl_i5(a, b, c) { \
  a = __cmodel_sx_shl_i5(b, c); \
}

/*  shrmu gpr, gpr
 * gpr shrmu( gpr);
 */
#define sx_shrmu_r(a, b) { \
  a = __cmodel_sx_shrmu_r(b); \
}

/*  shrmu gpr, <u5>
 * gpr shrmu( <u5>);
 */
#define sx_shrmu_i5(a, b) { \
  a = __cmodel_sx_shrmu_i5(b); \
}

/*  shrnu gpr, gpr, gpr
 * gpr shrnu( gpr, gpr);
 */
#define sx_shrnu(a, b, c) { \
  a = __cmodel_sx_shrnu(b, c); \
}

/*  shru gpr, gpr, gpr
 * gpr shru( gpr, gpr);
 */
#define sx_shru_r(a, b, c) { \
  a = __cmodel_sx_shru_r(b, c); \
}

/*  shru gpr, gpr, <u5>
 * gpr shru( gpr, <u5>);
 */
#define sx_shru_i5(a, b, c) { \
  a = __cmodel_sx_shru_i5(b, c); \
}

/*  shr gpr, gpr, gpr
 * gpr shr( gpr, gpr);
 */
#define sx_shr_r(a, b, c) { \
  a = __cmodel_sx_shr_r(b, c); \
}

/*  shr gpr, gpr, <u5>
 * gpr shr( gpr, <u5>);
 */
#define sx_shr_i5(a, b, c) { \
  a = __cmodel_sx_shr_i5(b, c); \
}

/*  subbp gpr, gpr, gpr
 * gpr subbp( gpr, gpr);
 */
#define sx_subbp_r(a, b, c) { \
  a = __cmodel_sx_subbp_r(b, c); \
}

/*  subbp gpr, gpr, <u8>
 * gpr subbp( gpr, <u8>);
 */
#define sx_subbp_i8(a, b, c) { \
  a = __cmodel_sx_subbp_i8(b, c); \
}

/*  subcu gpr, gpr, gpr
 * gpr subcu( gpr, gpr);
 */
#define sx_subcu(a, b, c) { \
  a = __cmodel_sx_subcu(b, c); \
}

/*  subc gpr, gpr, gpr
 * gpr subc( gpr, gpr);
 */
#define sx_subc(a, b, c) { \
  a = __cmodel_sx_subc(b, c); \
}

/*  subhp gpr, gpr, gpr
 * gpr subhp( gpr, gpr);
 */
#define sx_subhp_r(a, b, c) { \
  a = __cmodel_sx_subhp_r(b, c); \
}

/*  subhp gpr, gpr, <u8>
 * gpr subhp( gpr, <u8>);
 */
#define sx_subhp_i8(a, b, c) { \
  a = __cmodel_sx_subhp_i8(b, c); \
}

/*  sububp gpr, gpr, gpr
 * gpr sububp( gpr, gpr);
 */
#define sx_sububp_r(a, b, c) { \
  a = __cmodel_sx_sububp_r(b, c); \
}

/*  sububp gpr, gpr, <u8>
 * gpr sububp( gpr, <u8>);
 */
#define sx_sububp_i8(a, b, c) { \
  a = __cmodel_sx_sububp_i8(b, c); \
}

/*  subuhp gpr, gpr, gpr
 * gpr subuhp( gpr, gpr);
 */
#define sx_subuhp_r(a, b, c) { \
  a = __cmodel_sx_subuhp_r(b, c); \
}

/*  subuhp gpr, gpr, <u8>
 * gpr subuhp( gpr, <u8>);
 */
#define sx_subuhp_i8(a, b, c) { \
  a = __cmodel_sx_subuhp_i8(b, c); \
}

/*  subu gpr, gpr, gpr
 * gpr subu( gpr, gpr);
 */
#define sx_subu_r(a, b, c) { \
  a = __cmodel_sx_subu_r(b, c); \
}

/*  subu gpr, gpr, <u8>
 * gpr subu( gpr, <u8>);
 */
#define sx_subu_i8(a, b, c) { \
  a = __cmodel_sx_subu_i8(b, c); \
}

/*  sub gpr, gpr, gpr
 * gpr sub( gpr, gpr);
 */
#define sx_sub_r(a, b, c) { \
  a = __cmodel_sx_sub_r(b, c); \
}

/*  sub gpr, gpr, <u8>
 * gpr sub( gpr, <u8>);
 */
#define sx_sub_i8(a, b, c) { \
  a = __cmodel_sx_sub_i8(b, c); \
}

/*  tfandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfandn( gpr, <u8>);
 */
#define sx_tfandn(a, b, c) { \
  a = __cmodel_sx_tfandn(b, c); \
}

/*  tfand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfand( gpr, <u8>);
 */
#define sx_tfand(a, b, c) { \
  a = __cmodel_sx_tfand(b, c); \
}

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, gpr);
 */
#define sx_tfbit_r(a, b, c) { \
  a = __cmodel_sx_tfbit_r(b, c); \
}

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, <u5>);
 */
#define sx_tfbit_i5(a, b, c) { \
  a = __cmodel_sx_tfbit_i5(b, c); \
}

/* trap <u2>
 * trap( <u2>);
 */
#define sx_trap(a) { \
  __cmodel_sx_trap(a); \
}

/*  ttandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttandn( gpr, <u8>);
 */
#define sx_ttandn(a, b, c) { \
  a = __cmodel_sx_ttandn(b, c); \
}

/*  ttand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttand( gpr, <u8>);
 */
#define sx_ttand(a, b, c) { \
  a = __cmodel_sx_ttand(b, c); \
}

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, gpr);
 */
#define sx_ttbit_r(a, b, c) { \
  a = __cmodel_sx_ttbit_r(b, c); \
}

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, <u5>);
 */
#define sx_ttbit_i5(a, b, c) { \
  a = __cmodel_sx_ttbit_i5(b, c); \
}

/*  xorg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst xorg( gr, gr);
 */
#define sx_xorg(a, b, c) { \
  a = __cmodel_sx_xorg(b, c); \
}

/*  xor gpr, gpr, gpr
 * gpr xor( gpr, gpr);
 */
#define sx_xor_r(a, b, c) { \
  a = __cmodel_sx_xor_r(b, c); \
}

/*  xor gpr, gpr, <u8>
 * gpr xor( gpr, <u8>);
 */
#define sx_xor_i8(a, b, c) { \
  a = __cmodel_sx_xor_i8(b, c); \
}


#endif /* defined(__SX) && !defined(__SX_C_MODEL) */
#endif /* _SX_H_ */
