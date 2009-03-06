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


/* Builtins for class sx */
#ifdef __SX

#ifndef _BUILTIN_SX_H_
#define _BUILTIN_SX_H_


/*  absbp gpr, gpr
 * gpr absbp( gpr);
 */
extern unsigned __builtin_sx_absbp(int);

/*  abshp gpr, gpr
 * gpr abshp( gpr);
 */
extern unsigned __builtin_sx_abshp(int);

/*  absubp gpr, gpr
 * gpr absubp( gpr);
 */
extern unsigned __builtin_sx_absubp(int);

/*  absuhp gpr, gpr
 * gpr absuhp( gpr);
 */
extern unsigned __builtin_sx_absuhp(int);

/*  absu gpr, gpr
 * gpr absu( gpr);
 */
extern unsigned __builtin_sx_absu(int);

/*  abs gpr, gpr
 * gpr abs( gpr);
 */
extern unsigned __builtin_sx_abs(int);

/*  addbp gpr, gpr, gpr
 * gpr addbp( gpr, gpr);
 */
extern int __builtin_sx_addbp_r(int, int);

/*  addbp gpr, gpr, <u8>
 * gpr addbp( gpr, <u8>);
 */
extern int __builtin_sx_addbp_i8(int, unsigned short);

/*  addcu gpr, gpr, gpr
 * gpr addcu( gpr, gpr);
 */
extern unsigned __builtin_sx_addcu(unsigned, unsigned);

/*  addc gpr, gpr, gpr
 * gpr addc( gpr, gpr);
 */
extern int __builtin_sx_addc(int, int);

/*  addhp gpr, gpr, gpr
 * gpr addhp( gpr, gpr);
 */
extern int __builtin_sx_addhp_r(int, int);

/*  addhp gpr, gpr, <u8>
 * gpr addhp( gpr, <u8>);
 */
extern int __builtin_sx_addhp_i8(int, unsigned short);

/*  addubp gpr, gpr, gpr
 * gpr addubp( gpr, gpr);
 */
extern unsigned __builtin_sx_addubp_r(unsigned, unsigned);

/*  addubp gpr, gpr, <u8>
 * gpr addubp( gpr, <u8>);
 */
extern unsigned __builtin_sx_addubp_i8(unsigned, unsigned short);

/*  addugp gpr, <u16>
 * gpr addugp( <u16>);
 */
extern unsigned __builtin_sx_addugp(unsigned short);

/*  adduhp gpr, gpr, gpr
 * gpr adduhp( gpr, gpr);
 */
extern unsigned __builtin_sx_adduhp_r(unsigned, unsigned);

/*  adduhp gpr, gpr, <u8>
 * gpr adduhp( gpr, <u8>);
 */
extern unsigned __builtin_sx_adduhp_i8(unsigned, unsigned short);

/*  addur gpr, gpr
 * gpr addur( gpr);
 */
extern int __builtin_sx_addur_r(int);

/*  addur gpr, <s16>
 * gpr addur( <s16>);
 */
extern int __builtin_sx_addur_i16(short);

/*  addu gpr, gpr, gpr
 * gpr addu( gpr, gpr);
 */
extern unsigned __builtin_sx_addu_r(unsigned, unsigned);

/*  addu gpr, gpr, <u8>
 * gpr addu( gpr, <u8>);
 */
extern unsigned __builtin_sx_addu_i8(unsigned, unsigned short);

/*  add gpr, gpr, gpr
 * gpr add( gpr, gpr);
 */
extern int __builtin_sx_add_r(int, int);

/*  add gpr, gpr, <u8>
 * gpr add( gpr, <u8>);
 */
extern int __builtin_sx_add_i8(int, unsigned short);

/*  andg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andg( gr, gr);
 */
extern int __builtin_sx_andg(int, int);

/*  andng ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andng( gr, gr);
 */
extern int __builtin_sx_andng(int, int);

/*  andn gpr, gpr, gpr
 * gpr andn( gpr, gpr);
 */
extern int __builtin_sx_andn(int, int);

/*  and gpr, gpr, gpr
 * gpr and( gpr, gpr);
 */
extern int __builtin_sx_and_r(int, int);

/*  and gpr, gpr, <u8>
 * gpr and( gpr, <u8>);
 */
extern int __builtin_sx_and_i8(int, unsigned short);

/* barrier
 * barrier( );
 */
extern void __builtin_sx_barrier(void);

/*  bclr gpr, gpr, gpr
 * gpr bclr( gpr, gpr);
 */
extern int __builtin_sx_bclr_r(int, int);

/*  bclr gpr, gpr, <u5>
 * gpr bclr( gpr, <u5>);
 */
extern int __builtin_sx_bclr_i5(int, unsigned short);

/* bkp
 * bkp( );
 */
extern void __builtin_sx_bkp(void);

/*  bnot gpr, gpr, gpr
 * gpr bnot( gpr, gpr);
 */
extern int __builtin_sx_bnot_r(int, int);

/*  bnot gpr, gpr, <u5>
 * gpr bnot( gpr, <u5>);
 */
extern int __builtin_sx_bnot_i5(int, unsigned short);

/*  boolbp gpr, gr
 * gpr boolbp( gr);
 */
extern int __builtin_sx_boolbp(int);

/*  boolhp gpr, gr
 * gpr boolhp( gr);
 */
extern int __builtin_sx_boolhp(int);

/*  bool gpr, gr
 * gpr bool( gr);
 */
extern int __builtin_sx_bool(int);

/*  bset gpr, gpr, gpr
 * gpr bset( gpr, gpr);
 */
extern int __builtin_sx_bset_r(int, int);

/*  bset gpr, gpr, <u5>
 * gpr bset( gpr, <u5>);
 */
extern int __builtin_sx_bset_i5(int, unsigned short);

/*  clamph gpr, gpr
 * gpr clamph( gpr);
 */
extern int __builtin_sx_clamph(int);

/*  extb gpr, gpr
 * gpr extb( gpr);
 */
extern int __builtin_sx_extb(int);

/*  exth gpr, gpr
 * gpr exth( gpr);
 */
extern int __builtin_sx_exth(int);

/*  extub gpr, gpr
 * gpr extub( gpr);
 */
extern unsigned __builtin_sx_extub(int);

/*  extuh gpr, gpr
 * gpr extuh( gpr);
 */
extern unsigned __builtin_sx_extuh(int);

/*  idle <u2>
 * idle( <u2>);
 */
extern void __builtin_sx_idle(unsigned short);

/* loopdis lr
 * lr loopdis( u[0..1]);
 */
extern void __builtin_sx_loopdis(unsigned);

/* loopena lr
 * lr loopena( u[0..1]);
 */
extern void __builtin_sx_loopena(unsigned);

/*  lzc gpr, gpr
 * gpr lzc( gpr);
 */
extern int __builtin_sx_lzc(int);

/*  makehp gpr, <s16>
 * gpr makehp( <s16>);
 */
extern int __builtin_sx_makehp(short);

/*  make gpr, <s16>
 * gpr make( <s16>);
 */
extern int __builtin_sx_make(short);

/*  maxbp gpr, gpr, gpr
 * gpr maxbp( gpr, gpr);
 */
extern int __builtin_sx_maxbp_r(int, int);

/*  maxbp gpr, gpr, <u8>
 * gpr maxbp( gpr, <u8>);
 */
extern int __builtin_sx_maxbp_i8(int, unsigned short);

/*  maxhp gpr, gpr, gpr
 * gpr maxhp( gpr, gpr);
 */
extern int __builtin_sx_maxhp_r(int, int);

/*  maxhp gpr, gpr, <u8>
 * gpr maxhp( gpr, <u8>);
 */
extern int __builtin_sx_maxhp_i8(int, unsigned short);

/*  maxubp gpr, gpr, gpr
 * gpr maxubp( gpr, gpr);
 */
extern unsigned __builtin_sx_maxubp_r(unsigned, unsigned);

/*  maxubp gpr, gpr, <u8>
 * gpr maxubp( gpr, <u8>);
 */
extern unsigned __builtin_sx_maxubp_i8(unsigned, unsigned short);

/*  maxuhp gpr, gpr, gpr
 * gpr maxuhp( gpr, gpr);
 */
extern unsigned __builtin_sx_maxuhp_r(unsigned, unsigned);

/*  maxuhp gpr, gpr, <u8>
 * gpr maxuhp( gpr, <u8>);
 */
extern unsigned __builtin_sx_maxuhp_i8(unsigned, unsigned short);

/*  maxu gpr, gpr, gpr
 * gpr maxu( gpr, gpr);
 */
extern unsigned __builtin_sx_maxu_r(unsigned, unsigned);

/*  maxu gpr, gpr, <u8>
 * gpr maxu( gpr, <u8>);
 */
extern unsigned __builtin_sx_maxu_i8(unsigned, unsigned short);

/*  max gpr, gpr, gpr
 * gpr max( gpr, gpr);
 */
extern int __builtin_sx_max_r(int, int);

/*  max gpr, gpr, <u8>
 * gpr max( gpr, <u8>);
 */
extern int __builtin_sx_max_i8(int, unsigned short);

/*  minbp gpr, gpr, gpr
 * gpr minbp( gpr, gpr);
 */
extern int __builtin_sx_minbp_r(int, int);

/*  minbp gpr, gpr, <u8>
 * gpr minbp( gpr, <u8>);
 */
extern int __builtin_sx_minbp_i8(int, unsigned short);

/*  minhp gpr, gpr, gpr
 * gpr minhp( gpr, gpr);
 */
extern int __builtin_sx_minhp_r(int, int);

/*  minhp gpr, gpr, <u8>
 * gpr minhp( gpr, <u8>);
 */
extern int __builtin_sx_minhp_i8(int, unsigned short);

/*  minubp gpr, gpr, gpr
 * gpr minubp( gpr, gpr);
 */
extern unsigned __builtin_sx_minubp_r(unsigned, unsigned);

/*  minubp gpr, gpr, <u8>
 * gpr minubp( gpr, <u8>);
 */
extern unsigned __builtin_sx_minubp_i8(unsigned, unsigned short);

/*  minuhp gpr, gpr, gpr
 * gpr minuhp( gpr, gpr);
 */
extern unsigned __builtin_sx_minuhp_r(unsigned, unsigned);

/*  minuhp gpr, gpr, <u8>
 * gpr minuhp( gpr, <u8>);
 */
extern unsigned __builtin_sx_minuhp_i8(unsigned, unsigned short);

/*  minu gpr, gpr, gpr
 * gpr minu( gpr, gpr);
 */
extern unsigned __builtin_sx_minu_r(unsigned, unsigned);

/*  minu gpr, gpr, <u8>
 * gpr minu( gpr, <u8>);
 */
extern unsigned __builtin_sx_minu_i8(unsigned, unsigned short);

/*  min gpr, gpr, gpr
 * gpr min( gpr, gpr);
 */
extern int __builtin_sx_min_r(int, int);

/*  min gpr, gpr, <u8>
 * gpr min( gpr, <u8>);
 */
extern int __builtin_sx_min_i8(int, unsigned short);

/*  more gpr, <u16>
 * gpr more( gpr, <u16>);
 */
extern int __builtin_sx_more(int, unsigned short);

/*  moveh2h gpr, gpr
 * gpr moveh2h( gpr, gpr);
 */
extern int __builtin_sx_moveh2h(int, int);

/*  moveh2l gpr, gpr
 * gpr moveh2l( gpr, gpr);
 */
extern int __builtin_sx_moveh2l(int, int);

/*  movel2h gpr, gpr
 * gpr movel2h( gpr, gpr);
 */
extern int __builtin_sx_movel2h(int, int);

/*  movel2l gpr, gpr
 * gpr movel2l( gpr, gpr);
 */
extern int __builtin_sx_movel2l(int, int);

/*  mover2sfr sfr, gpr
 * sfr mover2sfr( u[0..31], gpr);
 */
extern void __builtin_sx_mover2sfr(unsigned, int);

/*  movesfr2r gpr, sfr
 * gpr movesfr2r( u[0..31]);
 */
extern int __builtin_sx_movesfr2r(unsigned);

/*  negbp gpr, gpr
 * gpr negbp( gpr);
 */
extern int __builtin_sx_negbp(int);

/*  neghp gpr, gpr
 * gpr neghp( gpr);
 */
extern int __builtin_sx_neghp(int);

/*  negubp gpr, gpr
 * gpr negubp( gpr);
 */
extern int __builtin_sx_negubp(int);

/*  neguhp gpr, gpr
 * gpr neguhp( gpr);
 */
extern int __builtin_sx_neguhp(int);

/*  negu gpr, gpr
 * gpr negu( gpr);
 */
extern int __builtin_sx_negu(int);

/*  neg gpr, gpr
 * gpr neg( gpr);
 */
extern int __builtin_sx_neg(int);

/* nop
 * nop( );
 */
extern void __builtin_sx_nop(void);

/*  norn gpr, gpr, <u8>
 * gpr norn( gpr, <u8>);
 */
extern int __builtin_sx_norn(int, unsigned short);

/*  not gpr, gpr
 * gpr not( gpr);
 */
extern int __builtin_sx_not(int);

/*  org ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst org( gr, gr);
 */
extern int __builtin_sx_org(int, int);

/*  or gpr, gpr, gpr
 * gpr or( gpr, gpr);
 */
extern int __builtin_sx_or_r(int, int);

/*  or gpr, gpr, <u8>
 * gpr or( gpr, <u8>);
 */
extern int __builtin_sx_or_i8(int, unsigned short);

/*  prior gpr, gpr
 * gpr prior( gpr);
 */
extern int __builtin_sx_prior(int);

/*  rotl gpr, gpr, gpr
 * gpr rotl( gpr, gpr);
 */
extern int __builtin_sx_rotl_r(int, int);

/*  rotl gpr, gpr, <u5>
 * gpr rotl( gpr, <u5>);
 */
extern int __builtin_sx_rotl_i5(int, unsigned short);

/* rte
 * rte( );
 */
extern void __builtin_sx_rte(void);

/*  shlmu gpr, gpr
 * gpr shlmu( gpr);
 */
extern unsigned __builtin_sx_shlmu_r(unsigned);

/*  shlmu gpr, <u5>
 * gpr shlmu( <u5>);
 */
extern unsigned __builtin_sx_shlmu_i5(unsigned short);

/*  shlnu gpr, gpr, gpr
 * gpr shlnu( gpr, gpr);
 */
extern unsigned __builtin_sx_shlnu(unsigned, unsigned);

/*  shlu gpr, gpr, gpr
 * gpr shlu( gpr, gpr);
 */
extern unsigned __builtin_sx_shlu_r(unsigned, unsigned);

/*  shlu gpr, gpr, <u5>
 * gpr shlu( gpr, <u5>);
 */
extern unsigned __builtin_sx_shlu_i5(unsigned, unsigned short);

/*  shl gpr, gpr, gpr
 * gpr shl( gpr, gpr);
 */
extern int __builtin_sx_shl_r(int, int);

/*  shl gpr, gpr, <u5>
 * gpr shl( gpr, <u5>);
 */
extern int __builtin_sx_shl_i5(int, unsigned short);

/*  shrmu gpr, gpr
 * gpr shrmu( gpr);
 */
extern unsigned __builtin_sx_shrmu_r(unsigned);

/*  shrmu gpr, <u5>
 * gpr shrmu( <u5>);
 */
extern unsigned __builtin_sx_shrmu_i5(unsigned short);

/*  shrnu gpr, gpr, gpr
 * gpr shrnu( gpr, gpr);
 */
extern unsigned __builtin_sx_shrnu(unsigned, unsigned);

/*  shru gpr, gpr, gpr
 * gpr shru( gpr, gpr);
 */
extern unsigned __builtin_sx_shru_r(unsigned, unsigned);

/*  shru gpr, gpr, <u5>
 * gpr shru( gpr, <u5>);
 */
extern unsigned __builtin_sx_shru_i5(unsigned, unsigned short);

/*  shr gpr, gpr, gpr
 * gpr shr( gpr, gpr);
 */
extern int __builtin_sx_shr_r(int, int);

/*  shr gpr, gpr, <u5>
 * gpr shr( gpr, <u5>);
 */
extern int __builtin_sx_shr_i5(int, unsigned short);

/*  subbp gpr, gpr, gpr
 * gpr subbp( gpr, gpr);
 */
extern int __builtin_sx_subbp_r(int, int);

/*  subbp gpr, gpr, <u8>
 * gpr subbp( gpr, <u8>);
 */
extern int __builtin_sx_subbp_i8(int, unsigned short);

/*  subcu gpr, gpr, gpr
 * gpr subcu( gpr, gpr);
 */
extern unsigned __builtin_sx_subcu(unsigned, unsigned);

/*  subc gpr, gpr, gpr
 * gpr subc( gpr, gpr);
 */
extern int __builtin_sx_subc(int, int);

/*  subhp gpr, gpr, gpr
 * gpr subhp( gpr, gpr);
 */
extern int __builtin_sx_subhp_r(int, int);

/*  subhp gpr, gpr, <u8>
 * gpr subhp( gpr, <u8>);
 */
extern int __builtin_sx_subhp_i8(int, unsigned short);

/*  sububp gpr, gpr, gpr
 * gpr sububp( gpr, gpr);
 */
extern int __builtin_sx_sububp_r(int, int);

/*  sububp gpr, gpr, <u8>
 * gpr sububp( gpr, <u8>);
 */
extern int __builtin_sx_sububp_i8(int, unsigned short);

/*  subuhp gpr, gpr, gpr
 * gpr subuhp( gpr, gpr);
 */
extern int __builtin_sx_subuhp_r(int, int);

/*  subuhp gpr, gpr, <u8>
 * gpr subuhp( gpr, <u8>);
 */
extern int __builtin_sx_subuhp_i8(int, unsigned short);

/*  subu gpr, gpr, gpr
 * gpr subu( gpr, gpr);
 */
extern unsigned __builtin_sx_subu_r(unsigned, unsigned);

/*  subu gpr, gpr, <u8>
 * gpr subu( gpr, <u8>);
 */
extern unsigned __builtin_sx_subu_i8(unsigned, unsigned short);

/*  sub gpr, gpr, gpr
 * gpr sub( gpr, gpr);
 */
extern int __builtin_sx_sub_r(int, int);

/*  sub gpr, gpr, <u8>
 * gpr sub( gpr, <u8>);
 */
extern int __builtin_sx_sub_i8(int, unsigned short);

/*  tfandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfandn( gpr, <u8>);
 */
extern int __builtin_sx_tfandn(int, unsigned short);

/*  tfand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfand( gpr, <u8>);
 */
extern int __builtin_sx_tfand(int, unsigned short);

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, gpr);
 */
extern int __builtin_sx_tfbit_r(int, int);

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, <u5>);
 */
extern int __builtin_sx_tfbit_i5(int, unsigned short);

/* trap <u2>
 * trap( <u2>);
 */
extern void __builtin_sx_trap(unsigned short);

/*  ttandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttandn( gpr, <u8>);
 */
extern int __builtin_sx_ttandn(int, unsigned short);

/*  ttand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttand( gpr, <u8>);
 */
extern int __builtin_sx_ttand(int, unsigned short);

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, gpr);
 */
extern int __builtin_sx_ttbit_r(int, int);

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, <u5>);
 */
extern int __builtin_sx_ttbit_i5(int, unsigned short);

/*  xorg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst xorg( gr, gr);
 */
extern int __builtin_sx_xorg(int, int);

/*  xor gpr, gpr, gpr
 * gpr xor( gpr, gpr);
 */
extern int __builtin_sx_xor_r(int, int);

/*  xor gpr, gpr, <u8>
 * gpr xor( gpr, <u8>);
 */
extern int __builtin_sx_xor_i8(int, unsigned short);


#endif /* _BUILTIN_SX_H_ */
#endif /* __SX */
