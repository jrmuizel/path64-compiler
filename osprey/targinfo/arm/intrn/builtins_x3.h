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


/* Builtins for class x3 */
#ifdef __X3

#ifndef _BUILTIN_X3_H_
#define _BUILTIN_X3_H_


/*  cancelg gpr
 * cancelg( gpr);
 */
extern void __builtin_x3_cancelg_r_g(int);

/*  cancelg B<u2>, <u8>
 * cancelg( <u2>, <u8>);
 */
extern void __builtin_x3_cancelg_i8_i2_g(unsigned short, unsigned short);

/*  cancell gpr
 * cancell( gpr);
 */
extern void __builtin_x3_cancell_r_g(int);

/*  cancell B<u2>, <u8>
 * cancell( <u2>, <u8>);
 */
extern void __builtin_x3_cancell_i8_i2_g(unsigned short, unsigned short);

/* clrcc1
 * clrcc1( );
 */
extern void __builtin_x3_clrcc1(void);

/* clrcca
 * clrcca( );
 */
extern void __builtin_x3_clrcca(void);

/* clrcc
 * clrcc( );
 */
extern void __builtin_x3_clrcc(void);

/*  clric itcr, gpr
 * itcr clric( u[0..31], gpr);
 */
extern void __builtin_x3_clric(unsigned, int);

/*  clrie gpr
 * gpr clrie( );
 */
extern int __builtin_x3_clrie(void);

/* dflushw
 * dflushw( );
 */
extern void __builtin_x3_dflushw(void);

/* dflush gpr
 * dflush( gpr);
 */
extern void __builtin_x3_dflush(int);

/* dinval gpr
 * dinval( gpr);
 */
extern void __builtin_x3_dinval(int);

/* dlock gpr
 * dlock( gpr);
 */
extern void __builtin_x3_dlock(int);

/* dstata gpr
 * dstata( gpr);
 */
extern void __builtin_x3_dstata(int);

/* dstatwi gpr
 * dstatwi( gpr);
 */
extern void __builtin_x3_dstatwi(int);

/* dtouch gpr
 * dtouch( gpr);
 */
extern void __builtin_x3_dtouch(int);

/*  fork<u3> gpr, gpr, <u1>
 * fork( <u3>, gpr, gpr, <u1>);
 */
extern void __builtin_x3_fork_i3_g(unsigned short, int, int, unsigned short);

/*  fork gpr, gpr, <u1>
 * fork( gpr, gpr, <u1>);
 */
extern void __builtin_x3_fork_g(int, int, unsigned short);

/*  movee2r gpr, gecr
 * gpr movee2r( u[0..31]);
 */
extern int __builtin_x3_movee2r(unsigned);

/*  moveic2ri gpr, gpr
 * gpr moveic2ri( gpr);
 */
extern int __builtin_x3_moveic2ri(int);

/*  moveic2r gpr, itcr
 * gpr moveic2r( u[0..31]);
 */
extern int __builtin_x3_moveic2r(unsigned);

/*  moveie2r gpr
 * gpr moveie2r( );
 */
extern int __builtin_x3_moveie2r(void);

/*  mover2e gecr, gpr
 * gecr mover2e( u[0..31], gpr);
 */
extern void __builtin_x3_mover2e(unsigned, int);

/*  mover2ici gpr, gpr
 * mover2ici( gpr, gpr);
 */
extern void __builtin_x3_mover2ici(int, int);

/*  mover2ic itcr, gpr
 * itcr mover2ic( u[0..31], gpr);
 */
extern void __builtin_x3_mover2ic(unsigned, int);

/*  mover2ie gpr
 * mover2ie( gpr);
 */
extern void __builtin_x3_mover2ie(int);

/*  mpnsshh gpr, gpr, gpr
 * gpr mpnsshh( gpr, gpr);
 */
extern int __builtin_x3_mpnsshh(int, int);

/*  mpnsshl gpr, gpr, gpr
 * gpr mpnsshl( gpr, gpr);
 */
extern int __builtin_x3_mpnsshl(int, int);

/*  mpnsslh gpr, gpr, gpr
 * gpr mpnsslh( gpr, gpr);
 */
extern int __builtin_x3_mpnsslh(int, int);

/*  mpnssll gpr, gpr, gpr
 * gpr mpnssll( gpr, gpr);
 */
extern int __builtin_x3_mpnssll(int, int);

/*  mpnsuhh gpr, gpr, gpr
 * gpr mpnsuhh( gpr, gpr);
 */
extern int __builtin_x3_mpnsuhh(int, unsigned);

/*  mpnsuhl gpr, gpr, gpr
 * gpr mpnsuhl( gpr, gpr);
 */
extern int __builtin_x3_mpnsuhl(int, unsigned);

/*  mpnsulh gpr, gpr, gpr
 * gpr mpnsulh( gpr, gpr);
 */
extern int __builtin_x3_mpnsulh(int, unsigned);

/*  mpnsull gpr, gpr, gpr
 * gpr mpnsull( gpr, gpr);
 */
extern int __builtin_x3_mpnsull(int, unsigned);

/*  mpnushh gpr, gpr, gpr
 * gpr mpnushh( gpr, gpr);
 */
extern int __builtin_x3_mpnushh(unsigned, int);

/*  mpnushl gpr, gpr, gpr
 * gpr mpnushl( gpr, gpr);
 */
extern int __builtin_x3_mpnushl(unsigned, int);

/*  mpnuslh gpr, gpr, gpr
 * gpr mpnuslh( gpr, gpr);
 */
extern int __builtin_x3_mpnuslh(unsigned, int);

/*  mpnusll gpr, gpr, gpr
 * gpr mpnusll( gpr, gpr);
 */
extern int __builtin_x3_mpnusll(unsigned, int);

/*  mpnuuhh gpr, gpr, gpr
 * gpr mpnuuhh( gpr, gpr);
 */
extern int __builtin_x3_mpnuuhh(unsigned, unsigned);

/*  mpnuuhl gpr, gpr, gpr
 * gpr mpnuuhl( gpr, gpr);
 */
extern int __builtin_x3_mpnuuhl(unsigned, unsigned);

/*  mpnuulh gpr, gpr, gpr
 * gpr mpnuulh( gpr, gpr);
 */
extern int __builtin_x3_mpnuulh(unsigned, unsigned);

/*  mpnuull gpr, gpr, gpr
 * gpr mpnuull( gpr, gpr);
 */
extern int __builtin_x3_mpnuull(unsigned, unsigned);

/*  mpn gpr, gpr, gpr
 * gpr mpn( gpr, gpr);
 */
extern int __builtin_x3_mpn(int, int);

/*  mpsshh gpr, gpr, gpr
 * gpr mpsshh( gpr, gpr);
 */
extern int __builtin_x3_mpsshh(int, int);

/*  mpsshl gpr, gpr, gpr
 * gpr mpsshl( gpr, gpr);
 */
extern int __builtin_x3_mpsshl(int, int);

/*  mpsslh gpr, gpr, gpr
 * gpr mpsslh( gpr, gpr);
 */
extern int __builtin_x3_mpsslh(int, int);

/*  mpssll gpr, gpr, gpr
 * gpr mpssll( gpr, gpr);
 */
extern int __builtin_x3_mpssll(int, int);

/*  mpsuhh gpr, gpr, gpr
 * gpr mpsuhh( gpr, gpr);
 */
extern int __builtin_x3_mpsuhh(int, unsigned);

/*  mpsuhl gpr, gpr, gpr
 * gpr mpsuhl( gpr, gpr);
 */
extern int __builtin_x3_mpsuhl(int, unsigned);

/*  mpsulh gpr, gpr, gpr
 * gpr mpsulh( gpr, gpr);
 */
extern int __builtin_x3_mpsulh(int, unsigned);

/*  mpsull gpr, gpr, gpr
 * gpr mpsull( gpr, gpr);
 */
extern int __builtin_x3_mpsull(int, unsigned);

/*  mpushh gpr, gpr, gpr
 * gpr mpushh( gpr, gpr);
 */
extern int __builtin_x3_mpushh(unsigned, int);

/*  mpushl gpr, gpr, gpr
 * gpr mpushl( gpr, gpr);
 */
extern int __builtin_x3_mpushl(unsigned, int);

/*  mpuslh gpr, gpr, gpr
 * gpr mpuslh( gpr, gpr);
 */
extern int __builtin_x3_mpuslh(unsigned, int);

/*  mpusll gpr, gpr, gpr
 * gpr mpusll( gpr, gpr);
 */
extern int __builtin_x3_mpusll(unsigned, int);

/*  mpuuhh gpr, gpr, gpr
 * gpr mpuuhh( gpr, gpr);
 */
extern int __builtin_x3_mpuuhh(unsigned, unsigned);

/*  mpuuhl gpr, gpr, gpr
 * gpr mpuuhl( gpr, gpr);
 */
extern int __builtin_x3_mpuuhl(unsigned, unsigned);

/*  mpuulh gpr, gpr, gpr
 * gpr mpuulh( gpr, gpr);
 */
extern int __builtin_x3_mpuulh(unsigned, unsigned);

/*  mpuull gpr, gpr, gpr
 * gpr mpuull( gpr, gpr);
 */
extern int __builtin_x3_mpuull(unsigned, unsigned);

/*  mp gpr, gpr, gpr
 * gpr mp( gpr, gpr);
 */
extern int __builtin_x3_mp(int, int);

/*  notifyg gpr
 * notifyg( gpr);
 */
extern void __builtin_x3_notifyg_r_g(int);

/*  notifyg B<u2>, <u8>
 * notifyg( <u2>, <u8>);
 */
extern void __builtin_x3_notifyg_i8_i2_g(unsigned short, unsigned short);

/*  notifyla gpr
 * notifyla( gpr);
 */
extern void __builtin_x3_notifyla_r_g(int);

/*  notifyla B<u2>, <u8>
 * notifyla( <u2>, <u8>);
 */
extern void __builtin_x3_notifyla_i8_i2_g(unsigned short, unsigned short);

/*  notifyl<u3> gpr
 * notifyl( <u3>, gpr);
 */
extern void __builtin_x3_notifyl_r_i3_g(unsigned short, int);

/*  notifyl<u3> B<u2>, <u8>
 * notifyl( <u3>, <u2>, <u8>);
 */
extern void __builtin_x3_notifyl_i8_i2_i3_g(unsigned short, unsigned short, unsigned short);

/* pflushw
 * pflushw( );
 */
extern void __builtin_x3_pflushw(void);

/* pinvalr gpr
 * pinvalr( gpr);
 */
extern void __builtin_x3_pinvalr(int);

/* pinval gpr
 * pinval( gpr);
 */
extern void __builtin_x3_pinval(int);

/* plockr gpr
 * plockr( gpr);
 */
extern void __builtin_x3_plockr(int);

/* plock gpr
 * plock( gpr);
 */
extern void __builtin_x3_plock(int);

/* pstata gpr
 * pstata( gpr);
 */
extern void __builtin_x3_pstata(int);

/* pstatwi gpr
 * pstatwi( gpr);
 */
extern void __builtin_x3_pstatwi(int);

/* ptouch gpr
 * ptouch( gpr);
 */
extern void __builtin_x3_ptouch(int);

/*  setic itcr, gpr
 * itcr setic( u[0..31], gpr);
 */
extern void __builtin_x3_setic(unsigned, int);

/*  setie gpr
 * setie( gpr);
 */
extern void __builtin_x3_setie(int);

/* startcc1
 * startcc1( );
 */
extern void __builtin_x3_startcc1(void);

/* startcca
 * startcca( );
 */
extern void __builtin_x3_startcca(void);

/* startcc
 * startcc( );
 */
extern void __builtin_x3_startcc(void);

/* stopcc1
 * stopcc1( );
 */
extern void __builtin_x3_stopcc1(void);

/* stopcca
 * stopcca( );
 */
extern void __builtin_x3_stopcca(void);

/* stopcc
 * stopcc( );
 */
extern void __builtin_x3_stopcc(void);

/* terminate<u3>
 * terminate( <u3>);
 */
extern void __builtin_x3_terminate_i3(unsigned short);

/* terminate
 * terminate( );
 */
extern void __builtin_x3_terminate(void);

/*  waitg gpr
 * waitg( gpr);
 */
extern void __builtin_x3_waitg_r_g(int);

/*  waitg B<u2>, <u8>
 * waitg( <u2>, <u8>);
 */
extern void __builtin_x3_waitg_i8_i2_g(unsigned short, unsigned short);

/*  waitl gpr
 * waitl( gpr);
 */
extern void __builtin_x3_waitl_r_g(int);

/*  waitl B<u2>, <u8>
 * waitl( <u2>, <u8>);
 */
extern void __builtin_x3_waitl_i8_i2_g(unsigned short, unsigned short);

/*  waitnbg gpr
 * waitnbg( gpr);
 */
extern void __builtin_x3_waitnbg_r_g(int);

/*  waitnbg B<u2>, <u8>
 * waitnbg( <u2>, <u8>);
 */
extern void __builtin_x3_waitnbg_i8_i2_g(unsigned short, unsigned short);

/*  waitnbl gpr
 * waitnbl( gpr);
 */
extern void __builtin_x3_waitnbl_r_g(int);

/*  waitnbl B<u2>, <u8>
 * waitnbl( <u2>, <u8>);
 */
extern void __builtin_x3_waitnbl_i8_i2_g(unsigned short, unsigned short);

/*  waitnb gpr, gpr
 * waitnb( gpr, gpr);
 */
extern void __builtin_x3_waitnb(int, int);

/*  wait gpr, gpr
 * wait( gpr, gpr);
 */
extern void __builtin_x3_wait(int, int);


#endif /* _BUILTIN_X3_H_ */
#endif /* __X3 */
