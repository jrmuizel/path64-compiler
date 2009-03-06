
//============================================================================
//
//         Copyright 2000-2006, STMicroelectronics, Incorporated.
//                          All rights reserved.
//         STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
//
//  This software is supplied under the terms of a license agreement or
//  nondis closure agreement with STMicroelectronics and may not be copied or
//  disclosed except in accordance with the terms of that agreement.
//
//----------------------------------------------------------------------------
//
//  System           : STxP70 Toolset
//  Project Component: STxP70 Reconfiguration Toolkit
//  File Name        : builtins_model_vx.h
//  Purpose          : C Models
//
//----------------------------------------------------------------------------
//
//  Manually generated on Tue Nov 28 14:19:32 CET 2006
//
//============================================================================

#ifndef builtins_model_vx_h
#define builtins_model_vx_h

/*==================================================================*
 *
 * Real C models and types
 *
 *==================================================================*/

#include "builtins_model_vx/inc/vx.h"

/*==================================================================*
 *
 * typedefs
 *
 *==================================================================*/

typedef INT128 vx_DX;

typedef INT128 vx_128;

typedef long long vx_DP;

typedef long long vx_64;

/*------------------------------------------------------------------*
 *
 * User to C model mapping of vx instructions
 *
 *------------------------------------------------------------------*/

/* absd Vr, Vr, Vr
 * Vr absd (Vr, Vr);
 */
#define __cmodel_vx_absd(a, b, c) ( a = builtvx_absd(b, c) )

/* absdhm1 Vr, Vr, Vr
 * Vr absdhm1 (Vr, Vr);
 */
#define __cmodel_vx_absdhm1(a, b, c) ( a = builtvx_absdhm1(b, c) )

/* absdhp1 Vr, Vr, Vr
 * Vr absdhp1 (Vr, Vr);
 */
#define __cmodel_vx_absdhp1(a, b, c) ( a = builtvx_absdhp1(b, c) )

/* absdmax Vr, Dr, Vr
 * Vr absdmax (Dr, Vr);
 */
#define __cmodel_vx_absdmax(a, b, c) ( a = builtvx_absdmax(b.high, b.low, c) )

/* absdmin Vr, Dr, Vr
 * Vr absdmin (Dr, Vr);
 */
#define __cmodel_vx_absdmin(a, b, c) ( a = builtvx_absdmin(b.high, b.low, c) )

/* absdmpslt Dr, Dr, Dr, gpr
 * Dr absdmpslt (Dr, Dr, Dr, gpr);
 */
#define __cmodel_vx_absdmpslt(a, b, c, d) ( a = builtvx_absdmpslt(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* absdslt Dr, Dr, Vr
 * Dr absdslt (Dr, Dr, Vr);
 */
#define __cmodel_vx_absdslt(a, b, c) ( a = builtvx_absdslt(a.high, a.low, b.high, b.low, c) )

/* acc Vr, Vr
 * Vr acc (Vr, Vr);
 */
#define __cmodel_vx_acc(a, b) ( a = builtvx_acc(a, b) )

/* adds16 Dr, Dr, Dr
 * Dr adds16 (Dr, Dr);
 */
#define __cmodel_vx_adds16(a, b, c) ( a = builtvx_adds16(b.high, b.low, c.high, c.low) )

/* adds16shr Vr, Dr, Dr, u4
 * Vr adds16shr (Dr, Dr, u4);
 */
#define __cmodel_vx_adds16shr(a, b, c, d) ( a = builtvx_adds16shr(b.high, b.low, c.high, c.low, d) )

/* adds16shrr Vr, Dr, Dr, u4
 * Vr adds16shrr (Dr, Dr, u4);
 */
#define __cmodel_vx_adds16shrr(a, b, c, d) ( a = builtvx_adds16shrr(b.high, b.low, c.high, c.low, d) )

/* addu16m1 Dr, Dr, Dr
 * Dr addu16m1 (Dr, Dr);
 */
#define __cmodel_vx_addu16m1(a, b, c) ( a = builtvx_addu16m1(b.high, b.low, c.high, c.low) )

/* addu16m2 Dr, Dr, Dr
 * Dr addu16m2 (Dr, Dr, Dr);
 */
#define __cmodel_vx_addu16m2(a, b, c) ( a = builtvx_addu16m2(a.high, a.low, b.high, b.low, c.high, c.low) )

/* addu16shr6 Vr, Vr, Vr
 * Vr addu16shr6 (Vr, Vr);
 */
#define __cmodel_vx_addu16shr6(a, b, c) ( a = builtvx_addu16shr6(b, c) )

/* addu8clp63 Vr, Vr, gpr
 * Vr addu8clp63 (Vr, gpr);
 */
#define __cmodel_vx_addu8clp63(a, b, c) ( a = builtvx_addu8clp63(b, c) )

/* addvu8u8 Dr, Vr, Vr
 * Dr addvu8u8 (Vr, Vr);
 */
#define __cmodel_vx_addvu8u8(a, b, c) ( a = builtvx_addvu8u8(b, c) )

/* ascmf Dr, Dr, gpr
 * Dr ascmf (Dr, gpr);
 */
#define __cmodel_vx_ascmf(a, b, c) ( a = builtvx_ascmf(b.high, b.low, c) )

/* ascmfr Dr, Dr, gpr
 * Dr ascmfr (Dr, gpr);
 */
#define __cmodel_vx_ascmfr(a, b, c) ( a = builtvx_ascmfr(b.high, b.low, c) )

/* bshr Vr, Vr, Vr, u4
 * Vr bshr (Vr, Vr, u4);
 */
#define __cmodel_vx_bshr(a, b, c, d) ( a = builtvx_bshr(b, c, d) )

/* bshrr Vr, Vr, Vr, gpr
 * Vr bshrr (Vr, Vr, gpr);
 */
#define __cmodel_vx_bshrr(a, b, c, d) ( a = builtvx_bshrr(b, c, d) )

/* clp63 Vr, Vr
 * Vr clp63 (Vr);
 */
#define __cmodel_vx_clp63(a, b) ( a = builtvx_clp63(b) )

/* clpsym Vr, Dr, gpr
 * Vr clpsym (Dr, gpr);
 */
#define __cmodel_vx_clpsym(a, b, c) ( a = builtvx_clpsym(b.high, b.low, c) )

/* clpu8 Vr, Vr, gpr
 * Vr clpu8 (Vr, gpr);
 */
#define __cmodel_vx_clpu8(a, b, c) ( a = builtvx_clpu8(b, c) )

/* clr Vr
 * Vr clr ();
 */
#define __cmodel_vx_clr(a) ( a = builtvx_clr() )

/* cmpeqru8 Vr, Vr, gpr
 * Vr cmpeqru8 (Vr, gpr);
 */
#define __cmodel_vx_cmpeqru8(a, b, c) ( a = builtvx_cmpeqru8(b, c) )

/* cmpgeru8 Vr, Vr, gpr
 * Vr cmpgeru8 (Vr, gpr);
 */
#define __cmodel_vx_cmpgeru8(a, b, c) ( a = builtvx_cmpgeru8(b, c) )

/* cmpgtru8 Vr, Vr, gpr
 * Vr cmpgtru8 (Vr, gpr);
 */
#define __cmodel_vx_cmpgtru8(a, b, c) ( a = builtvx_cmpgtru8(b, c) )

/* cmpleru8 Vr, Vr, gpr
 * Vr cmpleru8 (Vr, gpr);
 */
#define __cmodel_vx_cmpleru8(a, b, c) ( a = builtvx_cmpleru8(b, c) )

/* cmpltru8 Vr, Vr, gpr
 * Vr cmpltru8 (Vr, gpr);
 */
#define __cmodel_vx_cmpltru8(a, b, c) ( a = builtvx_cmpltru8(b, c) )

/* cmpneru8 Vr, Vr, gpr
 * Vr cmpneru8 (Vr, gpr);
 */
#define __cmodel_vx_cmpneru8(a, b, c) ( a = builtvx_cmpneru8(b, c) )

/* cntdelta0clr Dr, Dr, Dr
 * Dr cntdelta0clr (Dr, Dr, Dr);
 */
#define __cmodel_vx_cntdelta0clr(a, b, c) ( a = builtvx_cntdelta0clr(a.high, a.low, b.high, b.low, c.high, c.low) )

/* cntdeltabw Dr, Dr, Dr, u2
 * Dr cntdeltabw (Dr, Dr, Dr, u2);
 */
#define __cmodel_vx_cntdeltabw(a, b, c, d) ( a = builtvx_cntdeltabw(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* cntdeltafw Dr, Dr, Dr, u2
 * Dr cntdeltafw (Dr, Dr, Dr, u2);
 */
#define __cmodel_vx_cntdeltafw(a, b, c, d) ( a = builtvx_cntdeltafw(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* cplsb Vr, Vr, u3
 * Vr cplsb (Vr, u3);
 */
#define __cmodel_vx_cplsb(a, b, c) ( a = builtvx_cplsb(b, c) )

/* cplsbi Vr, Vr, u2
 * Vr cplsbi (Vr, u2);
 */
#define __cmodel_vx_cplsbi(a, b, c) ( a = builtvx_cplsbi(b, c) )

/* cpmsb Vr, Vr, u3
 * Vr cpmsb (Vr, u3);
 */
#define __cmodel_vx_cpmsb(a, b, c) ( a = builtvx_cpmsb(b, c) )

/* cpmsbi Vr, Vr, u2
 * Vr cpmsbi (Vr, u2);
 */
#define __cmodel_vx_cpmsbi(a, b, c) ( a = builtvx_cpmsbi(b, c) )

/* cpmsbir Vr, Vr, gpr
 * Vr cpmsbir (Vr, gpr);
 */
#define __cmodel_vx_cpmsbir(a, b, c) ( a = builtvx_cpmsbir(b, c) )

/* cpmsbr Vr, Vr, gpr
 * Vr cpmsbr (Vr, gpr);
 */
#define __cmodel_vx_cpmsbr(a, b, c) ( a = builtvx_cpmsbr(b, c) )

/* dintlvb Dr, Vr, Vr
 * Dr dintlvb (Vr, Vr);
 */
#define __cmodel_vx_dintlvb(a, b, c) ( a = builtvx_dintlvb(b, c) )

/* dlupdate Dr, Vr, Vr
 * Dr dlupdate (Dr, Vr, Vr);
 */
#define __cmodel_vx_dlupdate(a, b, c) ( a = builtvx_dlupdate(a.high, a.low, b, c) )

/* fir3 Vr, Dr, Vr
 * Vr fir3 (Dr, Vr);
 */
#define __cmodel_vx_fir3(a, b, c) ( a = builtvx_fir3(b.high, b.low, c) )

/* fir3edge Dr, Vr, Vr
 * Dr fir3edge (Vr, Vr);
 */
#define __cmodel_vx_fir3edge(a, b, c) ( a = builtvx_fir3edge(b, c) )

/* firu8s8p Dr, Dr, Dr, u3
 * Dr firu8s8p (Dr, Dr, Dr, u3);
 */
#define __cmodel_vx_firu8s8p(a, b, c, d) ( a = builtvx_firu8s8p(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* firu8s8p1clr Dr, Dr, Dr
 * Dr firu8s8p1clr (Dr, Dr);
 */
#define __cmodel_vx_firu8s8p1clr(a, b, c) ( a = builtvx_firu8s8p1clr(b.high, b.low, c.high, c.low) )

/* getsad0 Dr, Dr, Dr
 * Dr getsad0 (Dr, Dr);
 */
#define __cmodel_vx_getsad0(a, b, c) ( a = builtvx_getsad0(b.high, b.low, c.high, c.low) )

/* incgt Vr, Vr, gpr
 * Vr incgt (Vr, Vr, gpr);
 */
#define __cmodel_vx_incgt(a, b, c) ( a = builtvx_incgt(a, b, c) )

/* incgth3 Vr, Dr, Vr, gpr
 * Vr incgth3 (Vr, Dr, Vr, gpr);
 */
#define __cmodel_vx_incgth3(a, b, c, d) ( a = builtvx_incgth3(a, b.high, b.low, c, d) )

/* incgth3clr Vr, Dr, Vr, gpr
 * Vr incgth3clr (Dr, Vr, gpr);
 */
#define __cmodel_vx_incgth3clr(a, b, c, d) ( a = builtvx_incgth3clr(b.high, b.low, c, d) )

/* incinsu16 Vr, Dr, gpr, gpr
 * Vr incinsu16 (Vr, Dr, gpr, gpr);
 */
#define __cmodel_vx_incinsu16(a, b, c, d) ( a = builtvx_incinsu16(a, b.high, b.low, c, d) )

/* insmean Dr, Vr, Vr
 * Dr insmean (Dr, Vr, Vr);
 */
#define __cmodel_vx_insmean(a, b, c) ( a = builtvx_insmean(a.high, a.low, b, c) )

/* insmeanr Dr, Vr, Vr
 * Dr insmeanr (Dr, Vr, Vr);
 */
#define __cmodel_vx_insmeanr(a, b, c) ( a = builtvx_insmeanr(a.high, a.low, b, c) )

/* intlvb Dr, Vr, Vr
 * Dr intlvb (Vr, Vr);
 */
#define __cmodel_vx_intlvb(a, b, c) ( a = builtvx_intlvb(b, c) )

/* m2rub gpr, Vr, u3
 * gpr m2rub (Vr, u3);
 */
#define __cmodel_vx_m2rub(a, b, c) ( a = builtvx_m2rub(b, c) )

/* m2ruhw gpr, Vr, u2
 * gpr m2ruhw (Vr, u2);
 */
#define __cmodel_vx_m2ruhw(a, b, c) ( a = builtvx_m2ruhw(b, c) )

/* m2rw0 gpr, Vr
 * gpr m2rw0 (Vr);
 */
#define __cmodel_vx_m2rw0(a, b) ( a = builtvx_m2rw0(b) )

/* m2rw1 gpr, Vr
 * gpr m2rw1 (Vr);
 */
#define __cmodel_vx_m2rw1(a, b) ( a = builtvx_m2rw1(b) )

/* m2v Vr, Vr
 * Vr m2v (Vr);
 */
#define __cmodel_vx_m2v(a, b) ( a = b )

/* m2x Vr, gpr, gpr
 * Vr m2x (gpr, gpr);
 */
#define __cmodel_vx_m2x(a, b, c) ( a = builtvx_m2x(b, c) )

/* m2xb Vr, gpr, u3
 * Vr m2xb (Vr, gpr, u3);
 */
#define __cmodel_vx_m2xb(a, b, c) ( a = builtvx_m2xb(a, b, c) )

/* m2xhw Vr, gpr, u2
 * Vr m2xhw (Vr, gpr, u2);
 */
#define __cmodel_vx_m2xhw(a, b, c) ( a = builtvx_m2xhw(a, b, c) )

/* m2xshldb Vr, gpr, gpr
 * Vr m2xshldb (Vr, gpr, gpr);
 */
#define __cmodel_vx_m2xshldb(a, b, c) ( a = builtvx_m2xshldb(a, b, c) )

/* m2xw0 Vr, gpr
 * Vr m2xw0 (Vr, gpr);
 */
#define __cmodel_vx_m2xw0(a, b) ( a = builtvx_m2xw0(a, b) )

/* m2xw1 Vr, gpr
 * Vr m2xw1 (Vr, gpr);
 */
#define __cmodel_vx_m2xw1(a, b) ( a = builtvx_m2xw1(a, b) )

/* maru8s8 Dr, Vr, gpr
 * Dr maru8s8 (Dr, Vr, gpr);
 */
#define __cmodel_vx_maru8s8(a, b, c) ( a = builtvx_maru8s8(a.high, a.low, b, c) )

/* maru8s8shr7 Dr, Vr, gpr
 * Dr maru8s8shr7 (Dr, Vr, gpr);
 */
#define __cmodel_vx_maru8s8shr7(a, b, c) ( a = builtvx_maru8s8shr7(a.high, a.low, b, c) )

/* maru8s8shr7r Dr, Vr, gpr
 * Dr maru8s8shr7r (Dr, Vr, gpr);
 */
#define __cmodel_vx_maru8s8shr7r(a, b, c) ( a = builtvx_maru8s8shr7r(a.high, a.low, b, c) )

/* maru8u8 Dr, Vr, gpr
 * Dr maru8u8 (Dr, Vr, gpr);
 */
#define __cmodel_vx_maru8u8(a, b, c) ( a = builtvx_maru8u8(a.high, a.low, b, c) )

/* maru8u8shr7 Dr, Vr, gpr
 * Dr maru8u8shr7 (Dr, Vr, gpr);
 */
#define __cmodel_vx_maru8u8shr7(a, b, c) ( a = builtvx_maru8u8shr7(a.high, a.low, b, c) )

/* maru8u8shr7r Dr, Vr, gpr
 * Dr maru8u8shr7r (Dr, Vr, gpr);
 */
#define __cmodel_vx_maru8u8shr7r(a, b, c) ( a = builtvx_maru8u8shr7r(a.high, a.low, b, c) )

/* maviu8s8 Dr, Vr, Vr
 * Dr maviu8s8 (Dr, Vr, Vr);
 */
#define __cmodel_vx_maviu8s8(a, b, c) ( a = builtvx_maviu8s8(a.high, a.low, b, c) )

/* maviu8s8shr7 Dr, Vr, Vr
 * Dr maviu8s8shr7 (Dr, Vr, Vr);
 */
#define __cmodel_vx_maviu8s8shr7(a, b, c) ( a = builtvx_maviu8s8shr7(a.high, a.low, b, c) )

/* maviu8s8shr7r Dr, Vr, Vr
 * Dr maviu8s8shr7r (Dr, Vr, Vr);
 */
#define __cmodel_vx_maviu8s8shr7r(a, b, c) ( a = builtvx_maviu8s8shr7r(a.high, a.low, b, c) )

/* maviu8u8 Dr, Vr, Vr
 * Dr maviu8u8 (Dr, Vr, Vr);
 */
#define __cmodel_vx_maviu8u8(a, b, c) ( a = builtvx_maviu8u8(a.high, a.low, b, c) )

/* maviu8u8shr7 Dr, Vr, Vr
 * Dr maviu8u8shr7 (Dr, Vr, Vr);
 */
#define __cmodel_vx_maviu8u8shr7(a, b, c) ( a = builtvx_maviu8u8shr7(a.high, a.low, b, c) )

/* maviu8u8shr7r Dr, Vr, Vr
 * Dr maviu8u8shr7r (Dr, Vr, Vr);
 */
#define __cmodel_vx_maviu8u8shr7r(a, b, c) ( a = builtvx_maviu8u8shr7r(a.high, a.low, b, c) )

/* mavu8s8 Dr, Vr, Vr
 * Dr mavu8s8 (Dr, Vr, Vr);
 */
#define __cmodel_vx_mavu8s8(a, b, c) ( a = builtvx_mavu8s8(a.high, a.low, b, c) )

/* mavu8s8shr7 Dr, Vr, Vr
 * Dr mavu8s8shr7 (Dr, Vr, Vr);
 */
#define __cmodel_vx_mavu8s8shr7(a, b, c) ( a = builtvx_mavu8s8shr7(a.high, a.low, b, c) )

/* mavu8s8shr7r Dr, Vr, Vr
 * Dr mavu8s8shr7r (Dr, Vr, Vr);
 */
#define __cmodel_vx_mavu8s8shr7r(a, b, c) ( a = builtvx_mavu8s8shr7r(a.high, a.low, b, c) )

/* mavu8u8 Dr, Vr, Vr
 * Dr mavu8u8 (Dr, Vr, Vr);
 */
#define __cmodel_vx_mavu8u8(a, b, c) ( a = builtvx_mavu8u8(a.high, a.low, b, c) )

/* mavu8u8shr7 Dr, Vr, Vr
 * Dr mavu8u8shr7 (Dr, Vr, Vr);
 */
#define __cmodel_vx_mavu8u8shr7(a, b, c) ( a = builtvx_mavu8u8shr7(a.high, a.low, b, c) )

/* mavu8u8shr7r Dr, Vr, Vr
 * Dr mavu8u8shr7r (Dr, Vr, Vr);
 */
#define __cmodel_vx_mavu8u8shr7r(a, b, c) ( a = builtvx_mavu8u8shr7r(a.high, a.low, b, c) )

/* max Vr, Vr, Vr
 * Vr max (Vr, Vr);
 */
#define __cmodel_vx_max(a, b, c) ( a = builtvx_max(b, c) )

/* maxh3 Vr, Dr, Vr
 * Vr maxh3 (Dr, Vr);
 */
#define __cmodel_vx_maxh3(a, b, c) ( a = builtvx_maxh3(b.high, b.low, c) )

/* maxpair Vr, Vr
 * Vr maxpair (Vr);
 */
#define __cmodel_vx_maxpair(a, b) ( a = builtvx_maxpair(b) )

/* mean Vr, Vr, Vr
 * Vr mean (Vr, Vr);
 */
#define __cmodel_vx_mean(a, b, c) ( a = builtvx_mean(b, c) )

/* meanr Vr, Vr, Vr
 * Vr meanr (Vr, Vr);
 */
#define __cmodel_vx_meanr(a, b, c) ( a = builtvx_meanr(b, c) )

/* meanuv Dr, Dr, Dr
 * Dr meanuv (Dr, Dr, Dr);
 */
#define __cmodel_vx_meanuv(a, b, c) ( a = builtvx_meanuv(a.high, a.low, b.high, b.low, c.high, c.low) )

/* meanuvr Dr, Dr, Dr
 * Dr meanuvr (Dr, Dr, Dr);
 */
#define __cmodel_vx_meanuvr(a, b, c) ( a = builtvx_meanuvr(a.high, a.low, b.high, b.low, c.high, c.low) )

/* meany Dr, Dr, Dr
 * Dr meany (Dr, Dr, Dr);
 */
#define __cmodel_vx_meany(a, b, c) ( a = builtvx_meany(a.high, a.low, b.high, b.low, c.high, c.low) )

/* meanyr Dr, Dr, Dr
 * Dr meanyr (Dr, Dr, Dr);
 */
#define __cmodel_vx_meanyr(a, b, c) ( a = builtvx_meanyr(a.high, a.low, b.high, b.low, c.high, c.low) )

/* median Vr, Dr, Vr
 * Vr median (Dr, Vr);
 */
#define __cmodel_vx_median(a, b, c) ( a = builtvx_median(b.high, b.low, c) )

/* mf Vr, Vr, gpr
 * Vr mf (Vr, gpr);
 */
#define __cmodel_vx_mf(a, b, c) ( a = builtvx_mf(b, c) )

/* mfr Vr, Vr, gpr
 * Vr mfr (Vr, gpr);
 */
#define __cmodel_vx_mfr(a, b, c) ( a = builtvx_mfr(b, c) )

/* min Vr, Vr, Vr
 * Vr min (Vr, Vr);
 */
#define __cmodel_vx_min(a, b, c) ( a = builtvx_min(b, c) )

/* minsad Dr, Dr, Dr
 * Dr minsad (Dr, Dr);
 */
#define __cmodel_vx_minsad(a, b, c) ( a = builtvx_minsad(b.high, b.low, c.high, c.low) )

/* modand Vr, Vr
 * Vr modand (Vr);
 */
#define __cmodel_vx_modand(a, b) ( a = builtvx_modand(b) )

/* mpru8s8 Dr, Vr, gpr
 * Dr mpru8s8 (Vr, gpr);
 */
#define __cmodel_vx_mpru8s8(a, b, c) ( a = builtvx_mpru8s8(b, c) )

/* mpru8u8 Dr, Vr, gpr
 * Dr mpru8u8 (Vr, gpr);
 */
#define __cmodel_vx_mpru8u8(a, b, c) ( a = builtvx_mpru8u8(b, c) )

/* mpu8u8shr Vr, Vr, gpr, gpr
 * Vr mpu8u8shr (Vr, gpr, gpr);
 */
#define __cmodel_vx_mpu8u8shr(a, b, c, d) ( a = builtvx_mpu8u8shr(b, c, d) )

/* mpviu8s8 Dr, Vr, Vr
 * Dr mpviu8s8 (Vr, Vr);
 */
#define __cmodel_vx_mpviu8s8(a, b, c) ( a = builtvx_mpviu8s8(b, c) )

/* mpviu8u8 Dr, Vr, Vr
 * Dr mpviu8u8 (Vr, Vr);
 */
#define __cmodel_vx_mpviu8u8(a, b, c) ( a = builtvx_mpviu8u8(b, c) )

/* mpvu8s8 Dr, Vr, Vr
 * Dr mpvu8s8 (Vr, Vr);
 */
#define __cmodel_vx_mpvu8s8(a, b, c) ( a = builtvx_mpvu8s8(b, c) )

/* mpvu8u8 Dr, Vr, Vr
 * Dr mpvu8u8 (Vr, Vr);
 */
#define __cmodel_vx_mpvu8u8(a, b, c) ( a = builtvx_mpvu8u8(b, c) )

/* offset Vr, Vr, gpr
 * Vr offset (Vr, gpr);
 */
#define __cmodel_vx_offset(a, b, c) ( a = builtvx_offset(b, c) )

/* random Vr, Vr
 * Vr random (Vr);
 */
#define __cmodel_vx_random(a, b) ( a = builtvx_random(b) )

/* sad Dr, Dr, Dr, gpr
 * Dr sad (Dr, Dr, gpr);
 */
#define __cmodel_vx_sad(a, b, c, d) ( a = builtvx_sad(b.high, b.low, c.high, c.low, d) )

/* sadmin Dr, Dr, Dr, gpr
 * Dr sadmin (Dr, Dr, Dr, gpr);
 */
#define __cmodel_vx_sadmin(a, b, c, d) ( a = builtvx_sadmin(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* scales8s9 Vr, Vr, gpr, gpr
 * Vr scales8s9 (Vr, gpr, gpr);
 */
#define __cmodel_vx_scales8s9(a, b, c, d) ( a = builtvx_scales8s9(b, c, d) )

/* select Vr, Vr, Vr
 * Vr select (Vr, Vr, Vr);
 */
#define __cmodel_vx_select(a, b, c) ( a = builtvx_select(a, b, c) )

/* shl1s16 Dr, Dr
 * Dr shl1s16 (Dr);
 */
#define __cmodel_vx_shl1s16(a, b) ( a = builtvx_shl1s16(b.high, b.low) )

/* shr1s16 Dr, Dr
 * Dr shr1s16 (Dr);
 */
#define __cmodel_vx_shr1s16(a, b) ( a = builtvx_shr1s16(b.high, b.low) )

/* shr7s16s8rc Vr, Dr
 * Vr shr7s16s8rc (Dr);
 */
#define __cmodel_vx_shr7s16s8rc(a, b) ( a = builtvx_shr7s16s8rc(b.high, b.low) )

/* shr7s16s8rs Vr, Dr
 * Vr shr7s16s8rs (Dr);
 */
#define __cmodel_vx_shr7s16s8rs(a, b) ( a = builtvx_shr7s16s8rs(b.high, b.low) )

/* shrrs16s8 Vr, Dr, gpr
 * Vr shrrs16s8 (Dr, gpr);
 */
#define __cmodel_vx_shrrs16s8(a, b, c) ( a = builtvx_shrrs16s8(b.high, b.low, c) )

/* shrrs16s8r Vr, Dr, gpr
 * Vr shrrs16s8r (Dr, gpr);
 */
#define __cmodel_vx_shrrs16s8r(a, b, c) ( a = builtvx_shrrs16s8r(b.high, b.low, c) )

/* shrs16s8 Vr, Dr, u4
 * Vr shrs16s8 (Dr, u4);
 */
#define __cmodel_vx_shrs16s8(a, b, c) ( a = builtvx_shrs16s8(b.high, b.low, c) )

/* shrs16s8r Vr, Dr, u4
 * Vr shrs16s8r (Dr, u4);
 */
#define __cmodel_vx_shrs16s8r(a, b, c) ( a = builtvx_shrs16s8r(b.high, b.low, c) )

/* sub128 Vr, Vr
 * Vr sub128 (Vr);
 */
#define __cmodel_vx_sub128(a, b) ( a = builtvx_sub128(b) )

/* subs16 Dr, Dr, Dr
 * Dr subs16 (Dr, Dr);
 */
#define __cmodel_vx_subs16(a, b, c) ( a = builtvx_subs16(b.high, b.low, c.high, c.low) )

/* subs16shr Vr, Dr, Dr, u4
 * Vr subs16shr (Dr, Dr, u4);
 */
#define __cmodel_vx_subs16shr(a, b, c, d) ( a = builtvx_subs16shr(b.high, b.low, c.high, c.low, d) )

/* subs16shrr Vr, Dr, Dr, u4
 * Vr subs16shrr (Dr, Dr, u4);
 */
#define __cmodel_vx_subs16shrr(a, b, c, d) ( a = builtvx_subs16shrr(b.high, b.low, c.high, c.low, d) )

#endif

//============================================================================
//
// END
//
//============================================================================
