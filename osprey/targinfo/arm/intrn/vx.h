
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
//  File Name        : vx.h
//  Purpose          : extension user model
//
//----------------------------------------------------------------------------
//
//  Created on 2006/11/13 at 11:25:11
//
//============================================================================

#ifndef vx_h
#define vx_h

// Note: No vx id provided yet for vx extension
// #include <vx_id.h>

#if defined( __vx ) && ! defined( __vx_C_MODEL )

/*==================================================================*
 *
 * Use vx compiler's internal typedefs and builtins
 *
 *==================================================================*/

#include <builtins_vx.h>

#define vx_128_get_high(dv) __builtin_vx_EXTRACT_DP_FROM_DX1 ( (dv) )
#define vx_128_get_low(dv)  __builtin_vx_EXTRACT_DP_FROM_DX0 ( (dv) )
#define vx_128_set_high(dv, h) dv = __builtin_vx_INSERT_DP_INTO_DX1  ( (dv), (h) )
#define vx_128_set_low(dv, l)  dv = __builtin_vx_INSERT_DP_INTO_DX0  ( (dv), (l) )
#define vx_128_compose(h, l) __builtin_vx_COMPOSE_DX_FROM_2xDP( (h), (l) )
#define vx_64_from_llong(/*vx_64*/res, /*long long*/ll) *((long long*)&(res)) = ll;

/*==================================================================*
 *
 * make macros
 *
 *==================================================================*/

#define vx_make_DX( _DX_, _word_3_, _word_2_, _word_1_, _word_0_ ) {	\
	_DX_ = __builtin_vx_m2d(__builtin_vx_m2x(_word_3_, _word_2_),	\
				__builtin_vx_m2x(_word_1_, _word_0_));	\
}

#define vx_make_DP( _DP_, _word_1_, _word_0_ ) {     \
	_DP_ = __builtin_vx_m2x(_word_1_, _word_0_); \
}

/*==================================================================*
 *
 * compose macros
 *
 *==================================================================*/

#define vx_compose_2xDP( _DX_, _DP_2_, _DP_1_ ) { \
	_DX_ = __builtin_vx_COMPOSE_DX_FROM_2xDP(_DP_2_, _DP_1_); \
}

/*==================================================================*
 *
 * split macros
 *
 *==================================================================*/

#define vx_split_2xDP( _DX_, _DP_2_, _DP_1_ ) { \
	_DP_1_= *(((vx_64*)&( _DX_ ))+0); \
	_DP_2_= *(((vx_64*)&( _DX_ ))+1); \
}

/*==================================================================*
 *
 * insert macros
 *
 *==================================================================*/

#ifdef __OPTIMIZE__

#define vx_insert_DP_into_DX( _DP_, _DX_, _part_)                                 \
	if (__builtin_constant_p(_part_)) {                                       \
	  _DX_ = ((_part_) == 0 ? __builtin_vx_INSERT_DP_INTO_DX0( _DX_, _DP_ )   \
	                        : __builtin_vx_INSERT_DP_INTO_DX1( _DX_, _DP_ )); \
	} else {                                                                  \
	  *(((vx_64*)&( _DX_ )) + _part_ ) = _DP_;                                \
	}

#else

#define vx_insert_DP_into_DX( _DP_, _DX_, _part_) \
	*(((vx_64*)&( _DX_ )) + _part_ ) = _DP_;

#endif

/*---- Insert DP# into DX ------------------------------------------*/
#define vx_insert_DP_into_DX0( _DP_, _DX_ ) \
	_DX_ = __builtin_vx_INSERT_DP_INTO_DX0( _DX_, _DP_ );

#define vx_insert_DP_into_DX1( _DP_, _DX_ ) \
	_DX_ = __builtin_vx_INSERT_DP_INTO_DX1( _DX_, _DP_ );

/*==================================================================*
 *
 * extract macros
 *
 *==================================================================*/

#ifdef __OPTIMIZE__

#define vx_extract_DP_from_DX( _DP_, _DX_, _part_)                           \
	if (__builtin_constant_p(_part_)) {                                  \
	  _DP_ = ((_part_) == 0 ? __builtin_vx_EXTRACT_DP_FROM_DX0( _DX_ )   \
	                        : __builtin_vx_EXTRACT_DP_FROM_DX1( _DX_ )); \
	} else {                                                             \
	  _DP_ = *(((vx_64*)&( _DX_ )) + _part_ );                           \
	}

#else

#define vx_extract_DP_from_DX( _DP_, _DX_, _part_ ) \
	_DP_ = *(((vx_64*)&( _DX_ )) + _part_ );

#endif

/*---- Extract DP# from DX -----------------------------------------*/
#define vx_extract_DP_from_DX0( _DP_, _DX_ ) \
	_DP_ = __builtin_vx_EXTRACT_DP_FROM_DX0( _DX_ );

#define vx_extract_DP_from_DX1( _DP_, _DX_ ) \
	_DP_ = __builtin_vx_EXTRACT_DP_FROM_DX1( _DX_ );

/*==================================================================*
 *
 * Print macros
 *
 *==================================================================*/

#define vx_fprintf_DX( _file_, _DX_ ) \
	fprintf( _file_, "0x%08X%08X%08X%08X", \
		*(((unsigned int*)&( _DX_ ))+3), \
		*(((unsigned int*)&( _DX_ ))+2), \
		*(((unsigned int*)&( _DX_ ))+1), \
		*(((unsigned int*)&( _DX_ ))+0) \
	)

#define vx_fprintf_DP( _file_, _DP_ ) \
	fprintf( _file_, "0x%08X%08X", \
		*(((unsigned int*)&( _DP_ ))+1), \
		*(((unsigned int*)&( _DP_ ))+0) \
	)

/*------------------------------------------------------------------*
 *
 * User to compiler's builtins mapping of vx instructions
 *
 *------------------------------------------------------------------*/

/* absd Vr, Vr, Vr
 * Vr absd (Vr, Vr);
 */
#define vx_absd(a, b, c) ( a = __builtin_vx_absd(b, c) )

/* absdhm1 Vr, Vr, Vr
 * Vr absdhm1 (Vr, Vr);
 */
#define vx_absdhm1(a, b, c) ( a = __builtin_vx_absdhm1(b, c) )

/* absdhp1 Vr, Vr, Vr
 * Vr absdhp1 (Vr, Vr);
 */
#define vx_absdhp1(a, b, c) ( a = __builtin_vx_absdhp1(b, c) )

/* absdmax Vr, Dr, Vr
 * Vr absdmax (Dr, Vr);
 */
#define vx_absdmax(a, b, c) ( a = __builtin_vx_absdmax(b, c) )

/* absdmin Vr, Dr, Vr
 * Vr absdmin (Dr, Vr);
 */
#define vx_absdmin(a, b, c) ( a = __builtin_vx_absdmin(b, c) )

/* absdmpslt Dr, Dr, Dr, gpr
 * Dr absdmpslt (Dr, Dr, Dr, gpr);
 */
#define vx_absdmpslt(a, b, c, d) ( a = __builtin_vx_absdmpslt(a, b, c, d) )

/* absdslt Dr, Dr, Vr
 * Dr absdslt (Dr, Dr, Vr);
 */
#define vx_absdslt(a, b, c) ( a = __builtin_vx_absdslt(a, b, c) )

/* acc Vr, Vr
 * Vr acc (Vr, Vr);
 */
#define vx_acc(a, b) ( a = __builtin_vx_acc(a, b) )

/* adds16 Dr, Dr, Dr
 * Dr adds16 (Dr, Dr);
 */
#define vx_adds16(a, b, c) ( a = __builtin_vx_adds16(b, c) )

/* adds16shr Vr, Dr, Dr, u4
 * Vr adds16shr (Dr, Dr, u4);
 */
#define vx_adds16shr(a, b, c, d) ( a = __builtin_vx_adds16shr(b, c, d) )

/* adds16shrr Vr, Dr, Dr, u4
 * Vr adds16shrr (Dr, Dr, u4);
 */
#define vx_adds16shrr(a, b, c, d) ( a = __builtin_vx_adds16shrr(b, c, d) )

/* addu16m1 Dr, Dr, Dr
 * Dr addu16m1 (Dr, Dr);
 */
#define vx_addu16m1(a, b, c) ( a = __builtin_vx_addu16m1(b, c) )

/* addu16m2 Dr, Dr, Dr
 * Dr addu16m2 (Dr, Dr, Dr);
 */
#define vx_addu16m2(a, b, c) ( a = __builtin_vx_addu16m2(a, b, c) )

/* addu16shr6 Vr, Vr, Vr
 * Vr addu16shr6 (Vr, Vr);
 */
#define vx_addu16shr6(a, b, c) ( a = __builtin_vx_addu16shr6(b, c) )

/* addu8clp63 Vr, Vr, gpr
 * Vr addu8clp63 (Vr, gpr);
 */
#define vx_addu8clp63(a, b, c) ( a = __builtin_vx_addu8clp63(b, c) )

/* addvu8u8 Dr, Vr, Vr
 * Dr addvu8u8 (Vr, Vr);
 */
#define vx_addvu8u8(a, b, c) ( a = __builtin_vx_addvu8u8(b, c) )

/* ascmf Dr, Dr, gpr
 * Dr ascmf (Dr, gpr);
 */
#define vx_ascmf(a, b, c) ( a = __builtin_vx_ascmf(b, c) )

/* ascmfr Dr, Dr, gpr
 * Dr ascmfr (Dr, gpr);
 */
#define vx_ascmfr(a, b, c) ( a = __builtin_vx_ascmfr(b, c) )

/* bshr Vr, Vr, Vr, u4
 * Vr bshr (Vr, Vr, u4);
 */
#define vx_bshr(a, b, c, d) ( a = __builtin_vx_bshr(b, c, d) )

/* bshrr Vr, Vr, Vr, gpr
 * Vr bshrr (Vr, Vr, gpr);
 */
#define vx_bshrr(a, b, c, d) ( a = __builtin_vx_bshrr(b, c, d) )

/* clp63 Vr, Vr
 * Vr clp63 (Vr);
 */
#define vx_clp63(a, b) ( a = __builtin_vx_clp63(b) )

/* clpsym Vr, Dr, gpr
 * Vr clpsym (Dr, gpr);
 */
#define vx_clpsym(a, b, c) ( a = __builtin_vx_clpsym(b, c) )

/* clpu8 Vr, Vr, gpr
 * Vr clpu8 (Vr, gpr);
 */
#define vx_clpu8(a, b, c) ( a = __builtin_vx_clpu8(b, c) )

/* clr Vr
 * Vr clr ();
 */
#define vx_clr(a) ( a = __builtin_vx_clr() )

/* cmpeqru8 Vr, Vr, gpr
 * Vr cmpeqru8 (Vr, gpr);
 */
#define vx_cmpeqru8(a, b, c) ( a = __builtin_vx_cmpeqru8(b, c) )

/* cmpgeru8 Vr, Vr, gpr
 * Vr cmpgeru8 (Vr, gpr);
 */
#define vx_cmpgeru8(a, b, c) ( a = __builtin_vx_cmpgeru8(b, c) )

/* cmpgtru8 Vr, Vr, gpr
 * Vr cmpgtru8 (Vr, gpr);
 */
#define vx_cmpgtru8(a, b, c) ( a = __builtin_vx_cmpgtru8(b, c) )

/* cmpleru8 Vr, Vr, gpr
 * Vr cmpleru8 (Vr, gpr);
 */
#define vx_cmpleru8(a, b, c) ( a = __builtin_vx_cmpleru8(b, c) )

/* cmpltru8 Vr, Vr, gpr
 * Vr cmpltru8 (Vr, gpr);
 */
#define vx_cmpltru8(a, b, c) ( a = __builtin_vx_cmpltru8(b, c) )

/* cmpneru8 Vr, Vr, gpr
 * Vr cmpneru8 (Vr, gpr);
 */
#define vx_cmpneru8(a, b, c) ( a = __builtin_vx_cmpneru8(b, c) )

/* cntdelta0clr Dr, Dr, Dr
 * Dr cntdelta0clr (Dr, Dr, Dr);
 */
#define vx_cntdelta0clr(a, b, c) ( a = __builtin_vx_cntdelta0clr(a, b, c) )

/* cntdeltabw Dr, Dr, Dr, u2
 * Dr cntdeltabw (Dr, Dr, Dr, u2);
 */
#define vx_cntdeltabw(a, b, c, d) ( a = __builtin_vx_cntdeltabw(a, b, c, d) )

/* cntdeltafw Dr, Dr, Dr, u2
 * Dr cntdeltafw (Dr, Dr, Dr, u2);
 */
#define vx_cntdeltafw(a, b, c, d) ( a = __builtin_vx_cntdeltafw(a, b, c, d) )

/* cplsb Vr, Vr, u3
 * Vr cplsb (Vr, u3);
 */
#define vx_cplsb(a, b, c) ( a = __builtin_vx_cplsb(b, c) )

/* cplsbi Vr, Vr, u2
 * Vr cplsbi (Vr, u2);
 */
#define vx_cplsbi(a, b, c) ( a = __builtin_vx_cplsbi(b, c) )

/* cpmsb Vr, Vr, u3
 * Vr cpmsb (Vr, u3);
 */
#define vx_cpmsb(a, b, c) ( a = __builtin_vx_cpmsb(b, c) )

/* cpmsbi Vr, Vr, u2
 * Vr cpmsbi (Vr, u2);
 */
#define vx_cpmsbi(a, b, c) ( a = __builtin_vx_cpmsbi(b, c) )

/* cpmsbir Vr, Vr, gpr
 * Vr cpmsbir (Vr, gpr);
 */
#define vx_cpmsbir(a, b, c) ( a = __builtin_vx_cpmsbir(b, c) )

/* cpmsbr Vr, Vr, gpr
 * Vr cpmsbr (Vr, gpr);
 */
#define vx_cpmsbr(a, b, c) ( a = __builtin_vx_cpmsbr(b, c) )

/* dintlvb Dr, Vr, Vr
 * Dr dintlvb (Vr, Vr);
 */
#define vx_dintlvb(a, b, c) ( a = __builtin_vx_dintlvb(b, c) )

/* dlupdate Dr, Vr, Vr
 * Dr dlupdate (Dr, Vr, Vr);
 */
#define vx_dlupdate(a, b, c) ( a = __builtin_vx_dlupdate(a, b, c) )

/* fir3 Vr, Dr, Vr
 * Vr fir3 (Dr, Vr);
 */
#define vx_fir3(a, b, c) ( a = __builtin_vx_fir3(b, c) )

/* fir3edge Dr, Vr, Vr
 * Dr fir3edge (Vr, Vr);
 */
#define vx_fir3edge(a, b, c) ( a = __builtin_vx_fir3edge(b, c) )

/* firu8s8p Dr, Dr, Dr, u3
 * Dr firu8s8p (Dr, Dr, Dr, u3);
 */
#define vx_firu8s8p(a, b, c, d) ( a = __builtin_vx_firu8s8p(a, b, c, d) )

/* firu8s8p1clr Dr, Dr, Dr
 * Dr firu8s8p1clr (Dr, Dr);
 */
#define vx_firu8s8p1clr(a, b, c) ( a = __builtin_vx_firu8s8p1clr(b, c) )

/* getsad0 Dr, Dr, Dr
 * Dr getsad0 (Dr, Dr);
 */
#define vx_getsad0(a, b, c) ( a = __builtin_vx_getsad0(b, c) )

/* incgt Vr, Vr, gpr
 * Vr incgt (Vr, Vr, gpr);
 */
#define vx_incgt(a, b, c) ( a = __builtin_vx_incgt(a, b, c) )

/* incgth3 Vr, Dr, Vr, gpr
 * Vr incgth3 (Vr, Dr, Vr, gpr);
 */
#define vx_incgth3(a, b, c, d) ( a = __builtin_vx_incgth3(a, b, c, d) )

/* incgth3clr Vr, Dr, Vr, gpr
 * Vr incgth3clr (Dr, Vr, gpr);
 */
#define vx_incgth3clr(a, b, c, d) ( a = __builtin_vx_incgth3clr(b, c, d) )

/* incinsu16 Vr, Dr, gpr, gpr
 * Vr incinsu16 (Vr, Dr, gpr, gpr);
 */
#define vx_incinsu16(a, b, c, d) ( a = __builtin_vx_incinsu16(a, b, c, d) )

/* insmean Dr, Vr, Vr
 * Dr insmean (Dr, Vr, Vr);
 */
#define vx_insmean(a, b, c) ( a = __builtin_vx_insmean(a, b, c) )

/* insmeanr Dr, Vr, Vr
 * Dr insmeanr (Dr, Vr, Vr);
 */
#define vx_insmeanr(a, b, c) ( a = __builtin_vx_insmeanr(a, b, c) )

/* intlvb Dr, Vr, Vr
 * Dr intlvb (Vr, Vr);
 */
#define vx_intlvb(a, b, c) ( a = __builtin_vx_intlvb(b, c) )

/* m2rub gpr, Vr, u3
 * gpr m2rub (Vr, u3);
 */
#define vx_m2rub(a, b, c) ( a = __builtin_vx_m2rub(b, c) )

/* m2ruhw gpr, Vr, u2
 * gpr m2ruhw (Vr, u2);
 */
#define vx_m2ruhw(a, b, c) ( a = __builtin_vx_m2ruhw(b, c) )

/* m2rw0 gpr, Vr
 * gpr m2rw0 (Vr);
 */
#define vx_m2rw0(a, b) ( a = __builtin_vx_m2rw0(b) )

/* m2rw1 gpr, Vr
 * gpr m2rw1 (Vr);
 */
#define vx_m2rw1(a, b) ( a = __builtin_vx_m2rw1(b) )

/* m2v Vr, Vr
 * Vr m2v (Vr);
 */
#define vx_m2v(a, b) ( a = __builtin_vx_m2v(b) )

/* m2x Vr, gpr, gpr
 * Vr m2x (gpr, gpr);
 */
#define vx_m2x(a, b, c) ( a = __builtin_vx_m2x(b, c) )

/* m2xb Vr, gpr, u3
 * Vr m2xb (Vr, gpr, u3);
 */
#define vx_m2xb(a, b, c) ( a = __builtin_vx_m2xb(a, b, c) )

/* m2xhw Vr, gpr, u2
 * Vr m2xhw (Vr, gpr, u2);
 */
#define vx_m2xhw(a, b, c) ( a = __builtin_vx_m2xhw(a, b, c) )

/* m2xshldb Vr, gpr, gpr
 * Vr m2xshldb (Vr, gpr, gpr);
 */
#define vx_m2xshldb(a, b, c) ( a = __builtin_vx_m2xshldb(a, b, c) )

/* m2xw0 Vr, gpr
 * Vr m2xw0 (Vr, gpr);
 */
#define vx_m2xw0(a, b) ( a = __builtin_vx_m2xw0(a, b) )

/* m2xw1 Vr, gpr
 * Vr m2xw1 (Vr, gpr);
 */
#define vx_m2xw1(a, b) ( a = __builtin_vx_m2xw1(a, b) )

/* maru8s8 Dr, Vr, gpr
 * Dr maru8s8 (Dr, Vr, gpr);
 */
#define vx_maru8s8(a, b, c) ( a = __builtin_vx_maru8s8(a, b, c) )

/* maru8s8shr7 Dr, Vr, gpr
 * Dr maru8s8shr7 (Dr, Vr, gpr);
 */
#define vx_maru8s8shr7(a, b, c) ( a = __builtin_vx_maru8s8shr7(a, b, c) )

/* maru8s8shr7r Dr, Vr, gpr
 * Dr maru8s8shr7r (Dr, Vr, gpr);
 */
#define vx_maru8s8shr7r(a, b, c) ( a = __builtin_vx_maru8s8shr7r(a, b, c) )

/* maru8u8 Dr, Vr, gpr
 * Dr maru8u8 (Dr, Vr, gpr);
 */
#define vx_maru8u8(a, b, c) ( a = __builtin_vx_maru8u8(a, b, c) )

/* maru8u8shr7 Dr, Vr, gpr
 * Dr maru8u8shr7 (Dr, Vr, gpr);
 */
#define vx_maru8u8shr7(a, b, c) ( a = __builtin_vx_maru8u8shr7(a, b, c) )

/* maru8u8shr7r Dr, Vr, gpr
 * Dr maru8u8shr7r (Dr, Vr, gpr);
 */
#define vx_maru8u8shr7r(a, b, c) ( a = __builtin_vx_maru8u8shr7r(a, b, c) )

/* maviu8s8 Dr, Vr, Vr
 * Dr maviu8s8 (Dr, Vr, Vr);
 */
#define vx_maviu8s8(a, b, c) ( a = __builtin_vx_maviu8s8(a, b, c) )

/* maviu8s8shr7 Dr, Vr, Vr
 * Dr maviu8s8shr7 (Dr, Vr, Vr);
 */
#define vx_maviu8s8shr7(a, b, c) ( a = __builtin_vx_maviu8s8shr7(a, b, c) )

/* maviu8s8shr7r Dr, Vr, Vr
 * Dr maviu8s8shr7r (Dr, Vr, Vr);
 */
#define vx_maviu8s8shr7r(a, b, c) ( a = __builtin_vx_maviu8s8shr7r(a, b, c) )

/* maviu8u8 Dr, Vr, Vr
 * Dr maviu8u8 (Dr, Vr, Vr);
 */
#define vx_maviu8u8(a, b, c) ( a = __builtin_vx_maviu8u8(a, b, c) )

/* maviu8u8shr7 Dr, Vr, Vr
 * Dr maviu8u8shr7 (Dr, Vr, Vr);
 */
#define vx_maviu8u8shr7(a, b, c) ( a = __builtin_vx_maviu8u8shr7(a, b, c) )

/* maviu8u8shr7r Dr, Vr, Vr
 * Dr maviu8u8shr7r (Dr, Vr, Vr);
 */
#define vx_maviu8u8shr7r(a, b, c) ( a = __builtin_vx_maviu8u8shr7r(a, b, c) )

/* mavu8s8 Dr, Vr, Vr
 * Dr mavu8s8 (Dr, Vr, Vr);
 */
#define vx_mavu8s8(a, b, c) ( a = __builtin_vx_mavu8s8(a, b, c) )

/* mavu8s8shr7 Dr, Vr, Vr
 * Dr mavu8s8shr7 (Dr, Vr, Vr);
 */
#define vx_mavu8s8shr7(a, b, c) ( a = __builtin_vx_mavu8s8shr7(a, b, c) )

/* mavu8s8shr7r Dr, Vr, Vr
 * Dr mavu8s8shr7r (Dr, Vr, Vr);
 */
#define vx_mavu8s8shr7r(a, b, c) ( a = __builtin_vx_mavu8s8shr7r(a, b, c) )

/* mavu8u8 Dr, Vr, Vr
 * Dr mavu8u8 (Dr, Vr, Vr);
 */
#define vx_mavu8u8(a, b, c) ( a = __builtin_vx_mavu8u8(a, b, c) )

/* mavu8u8shr7 Dr, Vr, Vr
 * Dr mavu8u8shr7 (Dr, Vr, Vr);
 */
#define vx_mavu8u8shr7(a, b, c) ( a = __builtin_vx_mavu8u8shr7(a, b, c) )

/* mavu8u8shr7r Dr, Vr, Vr
 * Dr mavu8u8shr7r (Dr, Vr, Vr);
 */
#define vx_mavu8u8shr7r(a, b, c) ( a = __builtin_vx_mavu8u8shr7r(a, b, c) )

/* max Vr, Vr, Vr
 * Vr max (Vr, Vr);
 */
#define vx_max(a, b, c) ( a = __builtin_vx_max(b, c) )

/* maxh3 Vr, Dr, Vr
 * Vr maxh3 (Dr, Vr);
 */
#define vx_maxh3(a, b, c) ( a = __builtin_vx_maxh3(b, c) )

/* maxpair Vr, Vr
 * Vr maxpair (Vr);
 */
#define vx_maxpair(a, b) ( a = __builtin_vx_maxpair(b) )

/* mean Vr, Vr, Vr
 * Vr mean (Vr, Vr);
 */
#define vx_mean(a, b, c) ( a = __builtin_vx_mean(b, c) )

/* meanr Vr, Vr, Vr
 * Vr meanr (Vr, Vr);
 */
#define vx_meanr(a, b, c) ( a = __builtin_vx_meanr(b, c) )

/* meanuv Dr, Dr, Dr
 * Dr meanuv (Dr, Dr, Dr);
 */
#define vx_meanuv(a, b, c) ( a = __builtin_vx_meanuv(a, b, c) )

/* meanuvr Dr, Dr, Dr
 * Dr meanuvr (Dr, Dr, Dr);
 */
#define vx_meanuvr(a, b, c) ( a = __builtin_vx_meanuvr(a, b, c) )

/* meany Dr, Dr, Dr
 * Dr meany (Dr, Dr, Dr);
 */
#define vx_meany(a, b, c) ( a = __builtin_vx_meany(a, b, c) )

/* meanyr Dr, Dr, Dr
 * Dr meanyr (Dr, Dr, Dr);
 */
#define vx_meanyr(a, b, c) ( a = __builtin_vx_meanyr(a, b, c) )

/* median Vr, Dr, Vr
 * Vr median (Dr, Vr);
 */
#define vx_median(a, b, c) ( a = __builtin_vx_median(b, c) )

/* mf Vr, Vr, gpr
 * Vr mf (Vr, gpr);
 */
#define vx_mf(a, b, c) ( a = __builtin_vx_mf(b, c) )

/* mfr Vr, Vr, gpr
 * Vr mfr (Vr, gpr);
 */
#define vx_mfr(a, b, c) ( a = __builtin_vx_mfr(b, c) )

/* min Vr, Vr, Vr
 * Vr min (Vr, Vr);
 */
#define vx_min(a, b, c) ( a = __builtin_vx_min(b, c) )

/* minsad Dr, Dr, Dr
 * Dr minsad (Dr, Dr);
 */
#define vx_minsad(a, b, c) ( a = __builtin_vx_minsad(b, c) )

/* modand Vr, Vr
 * Vr modand (Vr);
 */
#define vx_modand(a, b) ( a = __builtin_vx_modand(b) )

/* mpru8s8 Dr, Vr, gpr
 * Dr mpru8s8 (Vr, gpr);
 */
#define vx_mpru8s8(a, b, c) ( a = __builtin_vx_mpru8s8(b, c) )

/* mpru8u8 Dr, Vr, gpr
 * Dr mpru8u8 (Vr, gpr);
 */
#define vx_mpru8u8(a, b, c) ( a = __builtin_vx_mpru8u8(b, c) )

/* mpu8u8shr Vr, Vr, gpr, gpr
 * Vr mpu8u8shr (Vr, gpr, gpr);
 */
#define vx_mpu8u8shr(a, b, c, d) ( a = __builtin_vx_mpu8u8shr(b, c, d) )

/* mpviu8s8 Dr, Vr, Vr
 * Dr mpviu8s8 (Vr, Vr);
 */
#define vx_mpviu8s8(a, b, c) ( a = __builtin_vx_mpviu8s8(b, c) )

/* mpviu8u8 Dr, Vr, Vr
 * Dr mpviu8u8 (Vr, Vr);
 */
#define vx_mpviu8u8(a, b, c) ( a = __builtin_vx_mpviu8u8(b, c) )

/* mpvu8s8 Dr, Vr, Vr
 * Dr mpvu8s8 (Vr, Vr);
 */
#define vx_mpvu8s8(a, b, c) ( a = __builtin_vx_mpvu8s8(b, c) )

/* mpvu8u8 Dr, Vr, Vr
 * Dr mpvu8u8 (Vr, Vr);
 */
#define vx_mpvu8u8(a, b, c) ( a = __builtin_vx_mpvu8u8(b, c) )

/* offset Vr, Vr, gpr
 * Vr offset (Vr, gpr);
 */
#define vx_offset(a, b, c) ( a = __builtin_vx_offset(b, c) )

/* random Vr, Vr
 * Vr random (Vr);
 */
#define vx_random(a, b) ( a = __builtin_vx_random(b) )

/* sad Dr, Dr, Dr, gpr
 * Dr sad (Dr, Dr, gpr);
 */
#define vx_sad(a, b, c, d) ( a = __builtin_vx_sad(b, c, d) )

/* sadmin Dr, Dr, Dr, gpr
 * Dr sadmin (Dr, Dr, Dr, gpr);
 */
#define vx_sadmin(a, b, c, d) ( a = __builtin_vx_sadmin(a, b, c, d) )

/* scales8s9 Vr, Vr, gpr, gpr
 * Vr scales8s9 (Vr, gpr, gpr);
 */
#define vx_scales8s9(a, b, c, d) ( a = __builtin_vx_scales8s9(b, c, d) )

/* select Vr, Vr, Vr
 * Vr select (Vr, Vr, Vr);
 */
#define vx_select(a, b, c) ( a = __builtin_vx_select(a, b, c) )

/* shl1s16 Dr, Dr
 * Dr shl1s16 (Dr);
 */
#define vx_shl1s16(a, b) ( a = __builtin_vx_shl1s16(b) )

/* shr1s16 Dr, Dr
 * Dr shr1s16 (Dr);
 */
#define vx_shr1s16(a, b) ( a = __builtin_vx_shr1s16(b) )

/* shr7s16s8rc Vr, Dr
 * Vr shr7s16s8rc (Dr);
 */
#define vx_shr7s16s8rc(a, b) ( a = __builtin_vx_shr7s16s8rc(b) )

/* shr7s16s8rs Vr, Dr
 * Vr shr7s16s8rs (Dr);
 */
#define vx_shr7s16s8rs(a, b) ( a = __builtin_vx_shr7s16s8rs(b) )

/* shrrs16s8 Vr, Dr, gpr
 * Vr shrrs16s8 (Dr, gpr);
 */
#define vx_shrrs16s8(a, b, c) ( a = __builtin_vx_shrrs16s8(b, c) )

/* shrrs16s8r Vr, Dr, gpr
 * Vr shrrs16s8r (Dr, gpr);
 */
#define vx_shrrs16s8r(a, b, c) ( a = __builtin_vx_shrrs16s8r(b, c) )

/* shrs16s8 Vr, Dr, u4
 * Vr shrs16s8 (Dr, u4);
 */
#define vx_shrs16s8(a, b, c) ( a = __builtin_vx_shrs16s8(b, c) )

/* shrs16s8r Vr, Dr, u4
 * Vr shrs16s8r (Dr, u4);
 */
#define vx_shrs16s8r(a, b, c) ( a = __builtin_vx_shrs16s8r(b, c) )

/* sub128 Vr, Vr
 * Vr sub128 (Vr);
 */
#define vx_sub128(a, b) ( a = __builtin_vx_sub128(b) )

/* subs16 Dr, Dr, Dr
 * Dr subs16 (Dr, Dr);
 */
#define vx_subs16(a, b, c) ( a = __builtin_vx_subs16(b, c) )

/* subs16shr Vr, Dr, Dr, u4
 * Vr subs16shr (Dr, Dr, u4);
 */
#define vx_subs16shr(a, b, c, d) ( a = __builtin_vx_subs16shr(b, c, d) )

/* subs16shrr Vr, Dr, Dr, u4
 * Vr subs16shrr (Dr, Dr, u4);
 */
#define vx_subs16shrr(a, b, c, d) ( a = __builtin_vx_subs16shrr(b, c, d) )

/* Instructions supported only
 * through assembly statement
 */
#define vx_lwh( Op_1, address ) \
	__asm( "LWH %0, @( %1 + 0 )" : _vx_INOUT "vx_DP" ( Op_1 ) : _vx_IN _vx_PTR ( address ) : _vx_MEMORY )

#define vx_lwl( Op_1, address ) \
	__asm( "LWL %0, @( %1 + 0 )" : _vx_INOUT "vx_DP" ( Op_1 ) : _vx_IN _vx_PTR ( address ) : _vx_MEMORY )

#define vx_swh( address, Op_1 ) \
	__asm( "SWH @( %0 + 0 ), %1" : : _vx_IN _vx_PTR ( address ), _vx_IN "vx_DP" ( Op_1 ) : _vx_MEMORY )

#define vx_swl( address, Op_1 ) \
	__asm( "SWL @( %0 + 0 ), %1" : : _vx_IN _vx_PTR ( address ), _vx_IN "vx_DP" ( Op_1 ) : _vx_MEMORY )


#ifdef __vx_DEPRECATED
/*############################################################################*
 *
 * Define the deprecated builtin syntax: a = builtvx_<mnemonic>(b, c)
 * as well as deprecated types: INT128, INT64
 *
 *############################################################################*/

/*==================================================================*
 *
 * Define depecrated type names
 *
 *==================================================================*/

typedef vx_64 INT64;

typedef vx_128 INT128;

/*------------------------------------------------------------------*
 *
 * Deprecated user to compiler's builtins mapping of vx instructions
 *
 *------------------------------------------------------------------*/

/* absd Vr, Vr, Vr  =  absd a[w], b[r], c[r]
 * Vr absd (Vr, Vr);
 */
#define builtvx_absd(b, c) __builtin_vx_absd(b, c)

/* absdhm1 Vr, Vr, Vr  =  absdhm1 a[w], b[r], c[r]
 * Vr absdhm1 (Vr, Vr);
 */
#define builtvx_absdhm1(b, c) __builtin_vx_absdhm1(b, c)

/* absdhp1 Vr, Vr, Vr  =  absdhp1 a[w], b[r], c[r]
 * Vr absdhp1 (Vr, Vr);
 */
#define builtvx_absdhp1(b, c) __builtin_vx_absdhp1(b, c)

/* absdmax Vr, Dr, Vr  =  absdmax a[w], b[r], c[r]
 * Vr absdmax (Dr, Vr);
 */
#define builtvx_absdmax(b, c) __builtin_vx_absdmax(b, c)

/* absdmin Vr, Dr, Vr  =  absdmin a[w], b[r], c[r]
 * Vr absdmin (Dr, Vr);
 */
#define builtvx_absdmin(b, c) __builtin_vx_absdmin(b, c)

/* absdmpslt Dr, Dr, Dr, gpr  =  absdmpslt a[rw], b[r], c[r], d[r]
 * Dr absdmpslt (Dr, Dr, Dr, gpr);
 */
#define builtvx_absdmpslt(a, b, c, d) __builtin_vx_absdmpslt(a, b, c, d)

/* absdslt Dr, Dr, Vr  =  absdslt a[rw], b[r], c[r]
 * Dr absdslt (Dr, Dr, Vr);
 */
#define builtvx_absdslt(a, b, c) __builtin_vx_absdslt(a, b, c)

/* acc Vr, Vr  =  acc a[rw], b[r]
 * Vr acc (Vr, Vr);
 */
#define builtvx_acc(a, b) __builtin_vx_acc(a, b)

/* adds16 Dr, Dr, Dr  =  adds16 a[w], b[r], c[r]
 * Dr adds16 (Dr, Dr);
 */
#define builtvx_adds16(b, c) __builtin_vx_adds16(b, c)

/* adds16shr Vr, Dr, Dr, u4  =  adds16shr a[w], b[r], c[r], d[r]
 * Vr adds16shr (Dr, Dr, u4);
 */
#define builtvx_adds16shr(b, c, d) __builtin_vx_adds16shr(b, c, d)

/* adds16shrr Vr, Dr, Dr, u4  =  adds16shrr a[w], b[r], c[r], d[r]
 * Vr adds16shrr (Dr, Dr, u4);
 */
#define builtvx_adds16shrr(b, c, d) __builtin_vx_adds16shrr(b, c, d)

/* addu16m1 Dr, Dr, Dr  =  addu16m1 a[w], b[r], c[r]
 * Dr addu16m1 (Dr, Dr);
 */
#define builtvx_addu16m1(b, c) __builtin_vx_addu16m1(b, c)

/* addu16m2 Dr, Dr, Dr  =  addu16m2 a[rw], b[r], c[r]
 * Dr addu16m2 (Dr, Dr, Dr);
 */
#define builtvx_addu16m2(a, b, c) __builtin_vx_addu16m2(a, b, c)

/* addu16shr6 Vr, Vr, Vr  =  addu16shr6 a[w], b[r], c[r]
 * Vr addu16shr6 (Vr, Vr);
 */
#define builtvx_addu16shr6(b, c) __builtin_vx_addu16shr6(b, c)

/* addu8clp63 Vr, Vr, gpr  =  addu8clp63 a[w], b[r], c[r]
 * Vr addu8clp63 (Vr, gpr);
 */
#define builtvx_addu8clp63(b, c) __builtin_vx_addu8clp63(b, c)

/* addvu8u8 Dr, Vr, Vr  =  addvu8u8 a[w], b[r], c[r]
 * Dr addvu8u8 (Vr, Vr);
 */
#define builtvx_addvu8u8(b, c) __builtin_vx_addvu8u8(b, c)

/* ascmf Dr, Dr, gpr  =  ascmf a[w], b[r], c[r]
 * Dr ascmf (Dr, gpr);
 */
#define builtvx_ascmf(b, c) __builtin_vx_ascmf(b, c)

/* ascmfr Dr, Dr, gpr  =  ascmfr a[w], b[r], c[r]
 * Dr ascmfr (Dr, gpr);
 */
#define builtvx_ascmfr(b, c) __builtin_vx_ascmfr(b, c)

/* bshr Vr, Vr, Vr, u4  =  bshr a[w], b[r], c[r], d[r]
 * Vr bshr (Vr, Vr, u4);
 */
#define builtvx_bshr(b, c, d) __builtin_vx_bshr(b, c, d)

/* bshrr Vr, Vr, Vr, gpr  =  bshrr a[w], b[r], c[r], d[r]
 * Vr bshrr (Vr, Vr, gpr);
 */
#define builtvx_bshrr(b, c, d) __builtin_vx_bshrr(b, c, d)

/* clp63 Vr, Vr  =  clp63 a[w], b[r]
 * Vr clp63 (Vr);
 */
#define builtvx_clp63(b) __builtin_vx_clp63(b)

/* clpsym Vr, Dr, gpr  =  clpsym a[w], b[r], c[r]
 * Vr clpsym (Dr, gpr);
 */
#define builtvx_clpsym(b, c) __builtin_vx_clpsym(b, c)

/* clpu8 Vr, Vr, gpr  =  clpu8 a[w], b[r], c[r]
 * Vr clpu8 (Vr, gpr);
 */
#define builtvx_clpu8(b, c) __builtin_vx_clpu8(b, c)

/* clr Vr  =  clr a[w]
 * Vr clr ();
 */
#define builtvx_clr() __builtin_vx_clr()

/* cmpeqru8 Vr, Vr, gpr  =  cmpeqru8 a[w], b[r], c[r]
 * Vr cmpeqru8 (Vr, gpr);
 */
#define builtvx_cmpeqru8(b, c) __builtin_vx_cmpeqru8(b, c)

/* cmpgeru8 Vr, Vr, gpr  =  cmpgeru8 a[w], b[r], c[r]
 * Vr cmpgeru8 (Vr, gpr);
 */
#define builtvx_cmpgeru8(b, c) __builtin_vx_cmpgeru8(b, c)

/* cmpgtru8 Vr, Vr, gpr  =  cmpgtru8 a[w], b[r], c[r]
 * Vr cmpgtru8 (Vr, gpr);
 */
#define builtvx_cmpgtru8(b, c) __builtin_vx_cmpgtru8(b, c)

/* cmpleru8 Vr, Vr, gpr  =  cmpleru8 a[w], b[r], c[r]
 * Vr cmpleru8 (Vr, gpr);
 */
#define builtvx_cmpleru8(b, c) __builtin_vx_cmpleru8(b, c)

/* cmpltru8 Vr, Vr, gpr  =  cmpltru8 a[w], b[r], c[r]
 * Vr cmpltru8 (Vr, gpr);
 */
#define builtvx_cmpltru8(b, c) __builtin_vx_cmpltru8(b, c)

/* cmpneru8 Vr, Vr, gpr  =  cmpneru8 a[w], b[r], c[r]
 * Vr cmpneru8 (Vr, gpr);
 */
#define builtvx_cmpneru8(b, c) __builtin_vx_cmpneru8(b, c)

/* cntdelta0clr Dr, Dr, Dr  =  cntdelta0clr a[rw], b[r], c[r]
 * Dr cntdelta0clr (Dr, Dr, Dr);
 */
#define builtvx_cntdelta0clr(a, b, c) __builtin_vx_cntdelta0clr(a, b, c)

/* cntdeltabw Dr, Dr, Dr, u2  =  cntdeltabw a[rw], b[r], c[r], d[r]
 * Dr cntdeltabw (Dr, Dr, Dr, u2);
 */
#define builtvx_cntdeltabw(a, b, c, d) __builtin_vx_cntdeltabw(a, b, c, d)

/* cntdeltafw Dr, Dr, Dr, u2  =  cntdeltafw a[rw], b[r], c[r], d[r]
 * Dr cntdeltafw (Dr, Dr, Dr, u2);
 */
#define builtvx_cntdeltafw(a, b, c, d) __builtin_vx_cntdeltafw(a, b, c, d)

/* cplsb Vr, Vr, u3  =  cplsb a[w], b[r], c[r]
 * Vr cplsb (Vr, u3);
 */
#define builtvx_cplsb(b, c) __builtin_vx_cplsb(b, c)

/* cplsbi Vr, Vr, u2  =  cplsbi a[w], b[r], c[r]
 * Vr cplsbi (Vr, u2);
 */
#define builtvx_cplsbi(b, c) __builtin_vx_cplsbi(b, c)

/* cpmsb Vr, Vr, u3  =  cpmsb a[w], b[r], c[r]
 * Vr cpmsb (Vr, u3);
 */
#define builtvx_cpmsb(b, c) __builtin_vx_cpmsb(b, c)

/* cpmsbi Vr, Vr, u2  =  cpmsbi a[w], b[r], c[r]
 * Vr cpmsbi (Vr, u2);
 */
#define builtvx_cpmsbi(b, c) __builtin_vx_cpmsbi(b, c)

/* cpmsbir Vr, Vr, gpr  =  cpmsbir a[w], b[r], c[r]
 * Vr cpmsbir (Vr, gpr);
 */
#define builtvx_cpmsbir(b, c) __builtin_vx_cpmsbir(b, c)

/* cpmsbr Vr, Vr, gpr  =  cpmsbr a[w], b[r], c[r]
 * Vr cpmsbr (Vr, gpr);
 */
#define builtvx_cpmsbr(b, c) __builtin_vx_cpmsbr(b, c)

/* dintlvb Dr, Vr, Vr  =  dintlvb a[w], b[r], c[r]
 * Dr dintlvb (Vr, Vr);
 */
#define builtvx_dintlvb(b, c) __builtin_vx_dintlvb(b, c)

/* dlupdate Dr, Vr, Vr  =  dlupdate a[rw], b[r], c[r]
 * Dr dlupdate (Dr, Vr, Vr);
 */
#define builtvx_dlupdate(a, b, c) __builtin_vx_dlupdate(a, b, c)

/* fir3 Vr, Dr, Vr  =  fir3 a[w], b[r], c[r]
 * Vr fir3 (Dr, Vr);
 */
#define builtvx_fir3(b, c) __builtin_vx_fir3(b, c)

/* fir3edge Dr, Vr, Vr  =  fir3edge a[w], b[r], c[r]
 * Dr fir3edge (Vr, Vr);
 */
#define builtvx_fir3edge(b, c) __builtin_vx_fir3edge(b, c)

/* firu8s8p Dr, Dr, Dr, u3  =  firu8s8p a[rw], b[r], c[r], d[r]
 * Dr firu8s8p (Dr, Dr, Dr, u3);
 */
#define builtvx_firu8s8p(a, b, c, d) __builtin_vx_firu8s8p(a, b, c, d)

/* firu8s8p1clr Dr, Dr, Dr  =  firu8s8p1clr a[w], b[r], c[r]
 * Dr firu8s8p1clr (Dr, Dr);
 */
#define builtvx_firu8s8p1clr(b, c) __builtin_vx_firu8s8p1clr(b, c)

/* getsad0 Dr, Dr, Dr  =  getsad0 a[w], b[r], c[r]
 * Dr getsad0 (Dr, Dr);
 */
#define builtvx_getsad0(b, c) __builtin_vx_getsad0(b, c)

/* incgt Vr, Vr, gpr  =  incgt a[rw], b[r], c[r]
 * Vr incgt (Vr, Vr, gpr);
 */
#define builtvx_incgt(a, b, c) __builtin_vx_incgt(a, b, c)

/* incgth3 Vr, Dr, Vr, gpr  =  incgth3 a[rw], b[r], c[r], d[r]
 * Vr incgth3 (Vr, Dr, Vr, gpr);
 */
#define builtvx_incgth3(a, b, c, d) __builtin_vx_incgth3(a, b, c, d)

/* incgth3clr Vr, Dr, Vr, gpr  =  incgth3clr a[w], b[r], c[r], d[r]
 * Vr incgth3clr (Dr, Vr, gpr);
 */
#define builtvx_incgth3clr(b, c, d) __builtin_vx_incgth3clr(b, c, d)

/* incinsu16 Vr, Dr, gpr, gpr  =  incinsu16 a[rw], b[r], c[r], d[r]
 * Vr incinsu16 (Vr, Dr, gpr, gpr);
 */
#define builtvx_incinsu16(a, b, c, d) __builtin_vx_incinsu16(a, b, c, d)

/* insmean Dr, Vr, Vr  =  insmean a[rw], b[r], c[r]
 * Dr insmean (Dr, Vr, Vr);
 */
#define builtvx_insmean(a, b, c) __builtin_vx_insmean(a, b, c)

/* insmeanr Dr, Vr, Vr  =  insmeanr a[rw], b[r], c[r]
 * Dr insmeanr (Dr, Vr, Vr);
 */
#define builtvx_insmeanr(a, b, c) __builtin_vx_insmeanr(a, b, c)

/* intlvb Dr, Vr, Vr  =  intlvb a[w], b[r], c[r]
 * Dr intlvb (Vr, Vr);
 */
#define builtvx_intlvb(b, c) __builtin_vx_intlvb(b, c)

/* m2rub gpr, Vr, u3  =  m2rub a[w], b[r], c[r]
 * gpr m2rub (Vr, u3);
 */
#define builtvx_m2rub(b, c) __builtin_vx_m2rub(b, c)

/* m2ruhw gpr, Vr, u2  =  m2ruhw a[w], b[r], c[r]
 * gpr m2ruhw (Vr, u2);
 */
#define builtvx_m2ruhw(b, c) __builtin_vx_m2ruhw(b, c)

/* m2rw0 gpr, Vr  =  m2rw0 a[w], b[r]
 * gpr m2rw0 (Vr);
 */
#define builtvx_m2rw0(b) __builtin_vx_m2rw0(b)

/* m2rw1 gpr, Vr  =  m2rw1 a[w], b[r]
 * gpr m2rw1 (Vr);
 */
#define builtvx_m2rw1(b) __builtin_vx_m2rw1(b)

/* m2v Vr, Vr  =  m2v a[w], b[r]
 * Vr m2v (Vr);
 */
#define builtvx_m2v(b) __builtin_vx_m2v(b)

/* m2x Vr, gpr, gpr  =  m2x a[w], b[r], c[r]
 * Vr m2x (gpr, gpr);
 */
#define builtvx_m2x(b, c) __builtin_vx_m2x(b, c)

/* m2xb Vr, gpr, u3  =  m2xb a[rw], b[r], c[r]
 * Vr m2xb (Vr, gpr, u3);
 */
#define builtvx_m2xb(a, b, c) __builtin_vx_m2xb(a, b, c)

/* m2xhw Vr, gpr, u2  =  m2xhw a[rw], b[r], c[r]
 * Vr m2xhw (Vr, gpr, u2);
 */
#define builtvx_m2xhw(a, b, c) __builtin_vx_m2xhw(a, b, c)

/* m2xshldb Vr, gpr, gpr  =  m2xshldb a[rw], b[r], c[r]
 * Vr m2xshldb (Vr, gpr, gpr);
 */
#define builtvx_m2xshldb(a, b, c) __builtin_vx_m2xshldb(a, b, c)

/* m2xw0 Vr, gpr  =  m2xw0 a[rw], b[r]
 * Vr m2xw0 (Vr, gpr);
 */
#define builtvx_m2xw0(a, b) __builtin_vx_m2xw0(a, b)

/* m2xw1 Vr, gpr  =  m2xw1 a[rw], b[r]
 * Vr m2xw1 (Vr, gpr);
 */
#define builtvx_m2xw1(a, b) __builtin_vx_m2xw1(a, b)

/* maru8s8 Dr, Vr, gpr  =  maru8s8 a[rw], b[r], c[r]
 * Dr maru8s8 (Dr, Vr, gpr);
 */
#define builtvx_maru8s8(a, b, c) __builtin_vx_maru8s8(a, b, c)

/* maru8s8shr7 Dr, Vr, gpr  =  maru8s8shr7 a[rw], b[r], c[r]
 * Dr maru8s8shr7 (Dr, Vr, gpr);
 */
#define builtvx_maru8s8shr7(a, b, c) __builtin_vx_maru8s8shr7(a, b, c)

/* maru8s8shr7r Dr, Vr, gpr  =  maru8s8shr7r a[rw], b[r], c[r]
 * Dr maru8s8shr7r (Dr, Vr, gpr);
 */
#define builtvx_maru8s8shr7r(a, b, c) __builtin_vx_maru8s8shr7r(a, b, c)

/* maru8u8 Dr, Vr, gpr  =  maru8u8 a[rw], b[r], c[r]
 * Dr maru8u8 (Dr, Vr, gpr);
 */
#define builtvx_maru8u8(a, b, c) __builtin_vx_maru8u8(a, b, c)

/* maru8u8shr7 Dr, Vr, gpr  =  maru8u8shr7 a[rw], b[r], c[r]
 * Dr maru8u8shr7 (Dr, Vr, gpr);
 */
#define builtvx_maru8u8shr7(a, b, c) __builtin_vx_maru8u8shr7(a, b, c)

/* maru8u8shr7r Dr, Vr, gpr  =  maru8u8shr7r a[rw], b[r], c[r]
 * Dr maru8u8shr7r (Dr, Vr, gpr);
 */
#define builtvx_maru8u8shr7r(a, b, c) __builtin_vx_maru8u8shr7r(a, b, c)

/* maviu8s8 Dr, Vr, Vr  =  maviu8s8 a[rw], b[r], c[r]
 * Dr maviu8s8 (Dr, Vr, Vr);
 */
#define builtvx_maviu8s8(a, b, c) __builtin_vx_maviu8s8(a, b, c)

/* maviu8s8shr7 Dr, Vr, Vr  =  maviu8s8shr7 a[rw], b[r], c[r]
 * Dr maviu8s8shr7 (Dr, Vr, Vr);
 */
#define builtvx_maviu8s8shr7(a, b, c) __builtin_vx_maviu8s8shr7(a, b, c)

/* maviu8s8shr7r Dr, Vr, Vr  =  maviu8s8shr7r a[rw], b[r], c[r]
 * Dr maviu8s8shr7r (Dr, Vr, Vr);
 */
#define builtvx_maviu8s8shr7r(a, b, c) __builtin_vx_maviu8s8shr7r(a, b, c)

/* maviu8u8 Dr, Vr, Vr  =  maviu8u8 a[rw], b[r], c[r]
 * Dr maviu8u8 (Dr, Vr, Vr);
 */
#define builtvx_maviu8u8(a, b, c) __builtin_vx_maviu8u8(a, b, c)

/* maviu8u8shr7 Dr, Vr, Vr  =  maviu8u8shr7 a[rw], b[r], c[r]
 * Dr maviu8u8shr7 (Dr, Vr, Vr);
 */
#define builtvx_maviu8u8shr7(a, b, c) __builtin_vx_maviu8u8shr7(a, b, c)

/* maviu8u8shr7r Dr, Vr, Vr  =  maviu8u8shr7r a[rw], b[r], c[r]
 * Dr maviu8u8shr7r (Dr, Vr, Vr);
 */
#define builtvx_maviu8u8shr7r(a, b, c) __builtin_vx_maviu8u8shr7r(a, b, c)

/* mavu8s8 Dr, Vr, Vr  =  mavu8s8 a[rw], b[r], c[r]
 * Dr mavu8s8 (Dr, Vr, Vr);
 */
#define builtvx_mavu8s8(a, b, c) __builtin_vx_mavu8s8(a, b, c)

/* mavu8s8shr7 Dr, Vr, Vr  =  mavu8s8shr7 a[rw], b[r], c[r]
 * Dr mavu8s8shr7 (Dr, Vr, Vr);
 */
#define builtvx_mavu8s8shr7(a, b, c) __builtin_vx_mavu8s8shr7(a, b, c)

/* mavu8s8shr7r Dr, Vr, Vr  =  mavu8s8shr7r a[rw], b[r], c[r]
 * Dr mavu8s8shr7r (Dr, Vr, Vr);
 */
#define builtvx_mavu8s8shr7r(a, b, c) __builtin_vx_mavu8s8shr7r(a, b, c)

/* mavu8u8 Dr, Vr, Vr  =  mavu8u8 a[rw], b[r], c[r]
 * Dr mavu8u8 (Dr, Vr, Vr);
 */
#define builtvx_mavu8u8(a, b, c) __builtin_vx_mavu8u8(a, b, c)

/* mavu8u8shr7 Dr, Vr, Vr  =  mavu8u8shr7 a[rw], b[r], c[r]
 * Dr mavu8u8shr7 (Dr, Vr, Vr);
 */
#define builtvx_mavu8u8shr7(a, b, c) __builtin_vx_mavu8u8shr7(a, b, c)

/* mavu8u8shr7r Dr, Vr, Vr  =  mavu8u8shr7r a[rw], b[r], c[r]
 * Dr mavu8u8shr7r (Dr, Vr, Vr);
 */
#define builtvx_mavu8u8shr7r(a, b, c) __builtin_vx_mavu8u8shr7r(a, b, c)

/* max Vr, Vr, Vr  =  max a[w], b[r], c[r]
 * Vr max (Vr, Vr);
 */
#define builtvx_max(b, c) __builtin_vx_max(b, c)

/* maxh3 Vr, Dr, Vr  =  maxh3 a[w], b[r], c[r]
 * Vr maxh3 (Dr, Vr);
 */
#define builtvx_maxh3(b, c) __builtin_vx_maxh3(b, c)

/* maxpair Vr, Vr  =  maxpair a[w], b[r]
 * Vr maxpair (Vr);
 */
#define builtvx_maxpair(b) __builtin_vx_maxpair(b)

/* mean Vr, Vr, Vr  =  mean a[w], b[r], c[r]
 * Vr mean (Vr, Vr);
 */
#define builtvx_mean(b, c) __builtin_vx_mean(b, c)

/* meanr Vr, Vr, Vr  =  meanr a[w], b[r], c[r]
 * Vr meanr (Vr, Vr);
 */
#define builtvx_meanr(b, c) __builtin_vx_meanr(b, c)

/* meanuv Dr, Dr, Dr  =  meanuv a[rw], b[r], c[r]
 * Dr meanuv (Dr, Dr, Dr);
 */
#define builtvx_meanuv(a, b, c) __builtin_vx_meanuv(a, b, c)

/* meanuvr Dr, Dr, Dr  =  meanuvr a[rw], b[r], c[r]
 * Dr meanuvr (Dr, Dr, Dr);
 */
#define builtvx_meanuvr(a, b, c) __builtin_vx_meanuvr(a, b, c)

/* meany Dr, Dr, Dr  =  meany a[rw], b[r], c[r]
 * Dr meany (Dr, Dr, Dr);
 */
#define builtvx_meany(a, b, c) __builtin_vx_meany(a, b, c)

/* meanyr Dr, Dr, Dr  =  meanyr a[rw], b[r], c[r]
 * Dr meanyr (Dr, Dr, Dr);
 */
#define builtvx_meanyr(a, b, c) __builtin_vx_meanyr(a, b, c)

/* median Vr, Dr, Vr  =  median a[w], b[r], c[r]
 * Vr median (Dr, Vr);
 */
#define builtvx_median(b, c) __builtin_vx_median(b, c)

/* mf Vr, Vr, gpr  =  mf a[w], b[r], c[r]
 * Vr mf (Vr, gpr);
 */
#define builtvx_mf(b, c) __builtin_vx_mf(b, c)

/* mfr Vr, Vr, gpr  =  mfr a[w], b[r], c[r]
 * Vr mfr (Vr, gpr);
 */
#define builtvx_mfr(b, c) __builtin_vx_mfr(b, c)

/* min Vr, Vr, Vr  =  min a[w], b[r], c[r]
 * Vr min (Vr, Vr);
 */
#define builtvx_min(b, c) __builtin_vx_min(b, c)

/* minsad Dr, Dr, Dr  =  minsad a[w], b[r], c[r]
 * Dr minsad (Dr, Dr);
 */
#define builtvx_minsad(b, c) __builtin_vx_minsad(b, c)

/* modand Vr, Vr  =  modand a[w], b[r]
 * Vr modand (Vr);
 */
#define builtvx_modand(b) __builtin_vx_modand(b)

/* mpru8s8 Dr, Vr, gpr  =  mpru8s8 a[w], b[r], c[r]
 * Dr mpru8s8 (Vr, gpr);
 */
#define builtvx_mpru8s8(b, c) __builtin_vx_mpru8s8(b, c)

/* mpru8u8 Dr, Vr, gpr  =  mpru8u8 a[w], b[r], c[r]
 * Dr mpru8u8 (Vr, gpr);
 */
#define builtvx_mpru8u8(b, c) __builtin_vx_mpru8u8(b, c)

/* mpu8u8shr Vr, Vr, gpr, gpr  =  mpu8u8shr a[w], b[r], c[r], d[r]
 * Vr mpu8u8shr (Vr, gpr, gpr);
 */
#define builtvx_mpu8u8shr(b, c, d) __builtin_vx_mpu8u8shr(b, c, d)

/* mpviu8s8 Dr, Vr, Vr  =  mpviu8s8 a[w], b[r], c[r]
 * Dr mpviu8s8 (Vr, Vr);
 */
#define builtvx_mpviu8s8(b, c) __builtin_vx_mpviu8s8(b, c)

/* mpviu8u8 Dr, Vr, Vr  =  mpviu8u8 a[w], b[r], c[r]
 * Dr mpviu8u8 (Vr, Vr);
 */
#define builtvx_mpviu8u8(b, c) __builtin_vx_mpviu8u8(b, c)

/* mpvu8s8 Dr, Vr, Vr  =  mpvu8s8 a[w], b[r], c[r]
 * Dr mpvu8s8 (Vr, Vr);
 */
#define builtvx_mpvu8s8(b, c) __builtin_vx_mpvu8s8(b, c)

/* mpvu8u8 Dr, Vr, Vr  =  mpvu8u8 a[w], b[r], c[r]
 * Dr mpvu8u8 (Vr, Vr);
 */
#define builtvx_mpvu8u8(b, c) __builtin_vx_mpvu8u8(b, c)

/* offset Vr, Vr, gpr  =  offset a[w], b[r], c[r]
 * Vr offset (Vr, gpr);
 */
#define builtvx_offset(b, c) __builtin_vx_offset(b, c)

/* random Vr, Vr  =  random a[w], b[r]
 * Vr random (Vr);
 */
#define builtvx_random(b) __builtin_vx_random(b)

/* sad Dr, Dr, Dr, gpr  =  sad a[w], b[r], c[r], d[r]
 * Dr sad (Dr, Dr, gpr);
 */
#define builtvx_sad(b, c, d) __builtin_vx_sad(b, c, d)

/* sadmin Dr, Dr, Dr, gpr  =  sadmin a[rw], b[r], c[r], d[r]
 * Dr sadmin (Dr, Dr, Dr, gpr);
 */
#define builtvx_sadmin(a, b, c, d) __builtin_vx_sadmin(a, b, c, d)

/* scales8s9 Vr, Vr, gpr, gpr  =  scales8s9 a[w], b[r], c[r], d[r]
 * Vr scales8s9 (Vr, gpr, gpr);
 */
#define builtvx_scales8s9(b, c, d) __builtin_vx_scales8s9(b, c, d)

/* select Vr, Vr, Vr  =  select a[rw], b[r], c[r]
 * Vr select (Vr, Vr, Vr);
 */
#define builtvx_select(a, b, c) __builtin_vx_select(a, b, c)

/* shl1s16 Dr, Dr  =  shl1s16 a[w], b[r]
 * Dr shl1s16 (Dr);
 */
#define builtvx_shl1s16(b) __builtin_vx_shl1s16(b)

/* shr1s16 Dr, Dr  =  shr1s16 a[w], b[r]
 * Dr shr1s16 (Dr);
 */
#define builtvx_shr1s16(b) __builtin_vx_shr1s16(b)

/* shr7s16s8rc Vr, Dr  =  shr7s16s8rc a[w], b[r]
 * Vr shr7s16s8rc (Dr);
 */
#define builtvx_shr7s16s8rc(b) __builtin_vx_shr7s16s8rc(b)

/* shr7s16s8rs Vr, Dr  =  shr7s16s8rs a[w], b[r]
 * Vr shr7s16s8rs (Dr);
 */
#define builtvx_shr7s16s8rs(b) __builtin_vx_shr7s16s8rs(b)

/* shrrs16s8 Vr, Dr, gpr  =  shrrs16s8 a[w], b[r], c[r]
 * Vr shrrs16s8 (Dr, gpr);
 */
#define builtvx_shrrs16s8(b, c) __builtin_vx_shrrs16s8(b, c)

/* shrrs16s8r Vr, Dr, gpr  =  shrrs16s8r a[w], b[r], c[r]
 * Vr shrrs16s8r (Dr, gpr);
 */
#define builtvx_shrrs16s8r(b, c) __builtin_vx_shrrs16s8r(b, c)

/* shrs16s8 Vr, Dr, u4  =  shrs16s8 a[w], b[r], c[r]
 * Vr shrs16s8 (Dr, u4);
 */
#define builtvx_shrs16s8(b, c) __builtin_vx_shrs16s8(b, c)

/* shrs16s8r Vr, Dr, u4  =  shrs16s8r a[w], b[r], c[r]
 * Vr shrs16s8r (Dr, u4);
 */
#define builtvx_shrs16s8r(b, c) __builtin_vx_shrs16s8r(b, c)

/* sub128 Vr, Vr  =  sub128 a[w], b[r]
 * Vr sub128 (Vr);
 */
#define builtvx_sub128(b) __builtin_vx_sub128(b)

/* subs16 Dr, Dr, Dr  =  subs16 a[w], b[r], c[r]
 * Dr subs16 (Dr, Dr);
 */
#define builtvx_subs16(b, c) __builtin_vx_subs16(b, c)

/* subs16shr Vr, Dr, Dr, u4  =  subs16shr a[w], b[r], c[r], d[r]
 * Vr subs16shr (Dr, Dr, u4);
 */
#define builtvx_subs16shr(b, c, d) __builtin_vx_subs16shr(b, c, d)

/* subs16shrr Vr, Dr, Dr, u4  =  subs16shrr a[w], b[r], c[r], d[r]
 * Vr subs16shrr (Dr, Dr, u4);
 */
#define builtvx_subs16shrr(b, c, d) __builtin_vx_subs16shrr(b, c, d)

/* Instructions supported only
 * through assembly statement
 */
#define builtvx_lwh( Op_1, address ) \
	__asm( "LWH %0, @( %1 + 0 )" : _vx_INOUT "vx_DP" ( Op_1 ) : _vx_IN _vx_PTR ( address ) : _vx_MEMORY )

#define builtvx_lwl( Op_1, address ) \
	__asm( "LWL %0, @( %1 + 0 )" : _vx_INOUT "vx_DP" ( Op_1 ) : _vx_IN _vx_PTR ( address ) : _vx_MEMORY )

#define builtvx_swh( address, Op_1 ) \
	__asm( "SWH @( %0 + 0 ), %1" : : _vx_IN _vx_PTR ( address ), _vx_IN "vx_DP" ( Op_1 ) : _vx_MEMORY )

#define builtvx_swl( address, Op_1 ) \
	__asm( "SWL @( %0 + 0 ), %1" : : _vx_IN _vx_PTR ( address ), _vx_IN "vx_DP" ( Op_1 ) : _vx_MEMORY )


#endif  /* __vx_DEPRECATED */


#else  /* C models */

/*==================================================================*
 *
 * Use vx C model typedefs and builtins
 *
 *==================================================================*/

#include "builtins_model_vx.h"

#define vx_128_get_high(dv) dv.high
#define vx_128_get_low(dv)  dv.low
#define vx_128_set_high(dv, h) (dv).high = (h)
#define vx_128_set_low(dv, l)  (dv).low = (l)
#define vx_128_compose(h, l)  ( { INT128 dv; dv.high = h; dv.low  = l; dv; } )
#define vx_64_from_llong(/*vx_64*/res, /*long long*/ll) *((long long*)&(res)) = ll;

/*==================================================================*
 *
 * make macros
 *
 *==================================================================*/

#define vx_make_DX( _DX_, _word_3_, _word_2_, _word_1_, _word_0_ ) {	\
        *(((unsigned int*)&( _DX_ ))+0) = ((unsigned int)( _word_0_ )); \
        *(((unsigned int*)&( _DX_ ))+1) = ((unsigned int)( _word_1_ )); \
        *(((unsigned int*)&( _DX_ ))+2) = ((unsigned int)( _word_2_ )); \
        *(((unsigned int*)&( _DX_ ))+3) = ((unsigned int)( _word_3_ )); \
}

#define vx_make_DP( _DP_, _word_1_, _word_0_ ) {     \
        *(((unsigned int*)&( _DP_ ))+0) = ((unsigned int)( _word_0_ )); \
        *(((unsigned int*)&( _DP_ ))+1) = ((unsigned int)( _word_1_ )); \
}

/*==================================================================*
 *
 * compose macros
 *
 *==================================================================*/

#define vx_compose_2xDP( _DX_, _DP_2_, _DP_1_ ) { \
        _DX_.high = _DP_2_; \
        _DX_.low  = _DP_1_; \
}

/*==================================================================*
 *
 * split macros
 *
 *==================================================================*/

#define vx_split_2xDP( _DX_, _DP_2_, _DP_1_ ) { \
	_DP_1_= *(((vx_64*)&( _DX_ ))+0); \
	_DP_2_= *(((vx_64*)&( _DX_ ))+1); \
}

/*==================================================================*
 *
 * insert macros
 *
 *==================================================================*/

#define vx_insert_DP_into_DX( _DP_, _DX_, _part_ ) \
	*(((vx_64*)&( _DX_ )) + _part_ )= _DP_;

/*---- Insert DP# into DX ------------------------------------------*/
#define vx_insert_DP_into_DX0( _DP_, _DX_ ) \
	_DX_.low = _DP_;

#define vx_insert_DP_into_DX1( _DP_, _DX_ ) \
	_DX_.high = _DP_;

/*==================================================================*
 *
 * extract macros
 *
 *==================================================================*/

#define vx_extract_DP_from_DX( _DP_, _DX_, _part_ ) \
	_DP_ = *(((vx_64*)&( _DX_ )) + _part_ );

/*---- Extract DP# from DX -----------------------------------------*/
#define vx_extract_DP_from_DX0( _DP_, _DX_ ) \
	_DP_ = _DX_.low;

#define vx_extract_DP_from_DX1( _DP_, _DX_ ) \
	_DP_ = _DX_.high;

/*==================================================================*
 *
 * Print macros
 *
 *==================================================================*/

#define vx_fprintf_DX( _file_, _DX_ ) \
	fprintf( _file_, "0x%08X%08X%08X%08X", \
		*(((unsigned int*)&( _DX_ ))+3), \
		*(((unsigned int*)&( _DX_ ))+2), \
		*(((unsigned int*)&( _DX_ ))+1), \
		*(((unsigned int*)&( _DX_ ))+0) \
	)

#define vx_fprintf_DP( _file_, _DP_ ) \
	fprintf( _file_, "0x%08X%08X", \
		*(((unsigned int*)&( _DP_ ))+1), \
		*(((unsigned int*)&( _DP_ ))+0) \
	)

/*------------------------------------------------------------------*
 *
 * User to C model mapping of vx instructions
 *
 *------------------------------------------------------------------*/

/* absd Vr, Vr, Vr
 * Vr absd (Vr, Vr);
 */
#define vx_absd(a, b, c) ( a = builtvx_absd(b, c) )

/* absdhm1 Vr, Vr, Vr
 * Vr absdhm1 (Vr, Vr);
 */
#define vx_absdhm1(a, b, c) ( a = builtvx_absdhm1(b, c) )

/* absdhp1 Vr, Vr, Vr
 * Vr absdhp1 (Vr, Vr);
 */
#define vx_absdhp1(a, b, c) ( a = builtvx_absdhp1(b, c) )

/* absdmax Vr, Dr, Vr
 * Vr absdmax (Dr, Vr);
 */
#define vx_absdmax(a, b, c) ( a = builtvx_absdmax(b.high, b.low, c) )

/* absdmin Vr, Dr, Vr
 * Vr absdmin (Dr, Vr);
 */
#define vx_absdmin(a, b, c) ( a = builtvx_absdmin(b.high, b.low, c) )

/* absdmpslt Dr, Dr, Dr, gpr
 * Dr absdmpslt (Dr, Dr, Dr, gpr);
 */
#define vx_absdmpslt(a, b, c, d) ( a = builtvx_absdmpslt(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* absdslt Dr, Dr, Vr
 * Dr absdslt (Dr, Dr, Vr);
 */
#define vx_absdslt(a, b, c) ( a = builtvx_absdslt(a.high, a.low, b.high, b.low, c) )

/* acc Vr, Vr
 * Vr acc (Vr, Vr);
 */
#define vx_acc(a, b) ( a = builtvx_acc(a, b) )

/* adds16 Dr, Dr, Dr
 * Dr adds16 (Dr, Dr);
 */
#define vx_adds16(a, b, c) ( a = builtvx_adds16(b.high, b.low, c.high, c.low) )

/* adds16shr Vr, Dr, Dr, u4
 * Vr adds16shr (Dr, Dr, u4);
 */
#define vx_adds16shr(a, b, c, d) ( a = builtvx_adds16shr(b.high, b.low, c.high, c.low, d) )

/* adds16shrr Vr, Dr, Dr, u4
 * Vr adds16shrr (Dr, Dr, u4);
 */
#define vx_adds16shrr(a, b, c, d) ( a = builtvx_adds16shrr(b.high, b.low, c.high, c.low, d) )

/* addu16m1 Dr, Dr, Dr
 * Dr addu16m1 (Dr, Dr);
 */
#define vx_addu16m1(a, b, c) ( a = builtvx_addu16m1(b.high, b.low, c.high, c.low) )

/* addu16m2 Dr, Dr, Dr
 * Dr addu16m2 (Dr, Dr, Dr);
 */
#define vx_addu16m2(a, b, c) ( a = builtvx_addu16m2(a.high, a.low, b.high, b.low, c.high, c.low) )

/* addu16shr6 Vr, Vr, Vr
 * Vr addu16shr6 (Vr, Vr);
 */
#define vx_addu16shr6(a, b, c) ( a = builtvx_addu16shr6(b, c) )

/* addu8clp63 Vr, Vr, gpr
 * Vr addu8clp63 (Vr, gpr);
 */
#define vx_addu8clp63(a, b, c) ( a = builtvx_addu8clp63(b, c) )

/* addvu8u8 Dr, Vr, Vr
 * Dr addvu8u8 (Vr, Vr);
 */
#define vx_addvu8u8(a, b, c) ( a = builtvx_addvu8u8(b, c) )

/* ascmf Dr, Dr, gpr
 * Dr ascmf (Dr, gpr);
 */
#define vx_ascmf(a, b, c) ( a = builtvx_ascmf(b.high, b.low, c) )

/* ascmfr Dr, Dr, gpr
 * Dr ascmfr (Dr, gpr);
 */
#define vx_ascmfr(a, b, c) ( a = builtvx_ascmfr(b.high, b.low, c) )

/* bshr Vr, Vr, Vr, u4
 * Vr bshr (Vr, Vr, u4);
 */
#define vx_bshr(a, b, c, d) ( a = builtvx_bshr(b, c, d) )

/* bshrr Vr, Vr, Vr, gpr
 * Vr bshrr (Vr, Vr, gpr);
 */
#define vx_bshrr(a, b, c, d) ( a = builtvx_bshrr(b, c, d) )

/* clp63 Vr, Vr
 * Vr clp63 (Vr);
 */
#define vx_clp63(a, b) ( a = builtvx_clp63(b) )

/* clpsym Vr, Dr, gpr
 * Vr clpsym (Dr, gpr);
 */
#define vx_clpsym(a, b, c) ( a = builtvx_clpsym(b.high, b.low, c) )

/* clpu8 Vr, Vr, gpr
 * Vr clpu8 (Vr, gpr);
 */
#define vx_clpu8(a, b, c) ( a = builtvx_clpu8(b, c) )

/* clr Vr
 * Vr clr ();
 */
#define vx_clr(a) ( a = builtvx_clr() )

/* cmpeqru8 Vr, Vr, gpr
 * Vr cmpeqru8 (Vr, gpr);
 */
#define vx_cmpeqru8(a, b, c) ( a = builtvx_cmpeqru8(b, c) )

/* cmpgeru8 Vr, Vr, gpr
 * Vr cmpgeru8 (Vr, gpr);
 */
#define vx_cmpgeru8(a, b, c) ( a = builtvx_cmpgeru8(b, c) )

/* cmpgtru8 Vr, Vr, gpr
 * Vr cmpgtru8 (Vr, gpr);
 */
#define vx_cmpgtru8(a, b, c) ( a = builtvx_cmpgtru8(b, c) )

/* cmpleru8 Vr, Vr, gpr
 * Vr cmpleru8 (Vr, gpr);
 */
#define vx_cmpleru8(a, b, c) ( a = builtvx_cmpleru8(b, c) )

/* cmpltru8 Vr, Vr, gpr
 * Vr cmpltru8 (Vr, gpr);
 */
#define vx_cmpltru8(a, b, c) ( a = builtvx_cmpltru8(b, c) )

/* cmpneru8 Vr, Vr, gpr
 * Vr cmpneru8 (Vr, gpr);
 */
#define vx_cmpneru8(a, b, c) ( a = builtvx_cmpneru8(b, c) )

/* cntdelta0clr Dr, Dr, Dr
 * Dr cntdelta0clr (Dr, Dr, Dr);
 */
#define vx_cntdelta0clr(a, b, c) ( a = builtvx_cntdelta0clr(a.high, a.low, b.high, b.low, c.high, c.low) )

/* cntdeltabw Dr, Dr, Dr, u2
 * Dr cntdeltabw (Dr, Dr, Dr, u2);
 */
#define vx_cntdeltabw(a, b, c, d) ( a = builtvx_cntdeltabw(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* cntdeltafw Dr, Dr, Dr, u2
 * Dr cntdeltafw (Dr, Dr, Dr, u2);
 */
#define vx_cntdeltafw(a, b, c, d) ( a = builtvx_cntdeltafw(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* cplsb Vr, Vr, u3
 * Vr cplsb (Vr, u3);
 */
#define vx_cplsb(a, b, c) ( a = builtvx_cplsb(b, c) )

/* cplsbi Vr, Vr, u2
 * Vr cplsbi (Vr, u2);
 */
#define vx_cplsbi(a, b, c) ( a = builtvx_cplsbi(b, c) )

/* cpmsb Vr, Vr, u3
 * Vr cpmsb (Vr, u3);
 */
#define vx_cpmsb(a, b, c) ( a = builtvx_cpmsb(b, c) )

/* cpmsbi Vr, Vr, u2
 * Vr cpmsbi (Vr, u2);
 */
#define vx_cpmsbi(a, b, c) ( a = builtvx_cpmsbi(b, c) )

/* cpmsbir Vr, Vr, gpr
 * Vr cpmsbir (Vr, gpr);
 */
#define vx_cpmsbir(a, b, c) ( a = builtvx_cpmsbir(b, c) )

/* cpmsbr Vr, Vr, gpr
 * Vr cpmsbr (Vr, gpr);
 */
#define vx_cpmsbr(a, b, c) ( a = builtvx_cpmsbr(b, c) )

/* dintlvb Dr, Vr, Vr
 * Dr dintlvb (Vr, Vr);
 */
#define vx_dintlvb(a, b, c) ( a = builtvx_dintlvb(b, c) )

/* dlupdate Dr, Vr, Vr
 * Dr dlupdate (Dr, Vr, Vr);
 */
#define vx_dlupdate(a, b, c) ( a = builtvx_dlupdate(a.high, a.low, b, c) )

/* fir3 Vr, Dr, Vr
 * Vr fir3 (Dr, Vr);
 */
#define vx_fir3(a, b, c) ( a = builtvx_fir3(b.high, b.low, c) )

/* fir3edge Dr, Vr, Vr
 * Dr fir3edge (Vr, Vr);
 */
#define vx_fir3edge(a, b, c) ( a = builtvx_fir3edge(b, c) )

/* firu8s8p Dr, Dr, Dr, u3
 * Dr firu8s8p (Dr, Dr, Dr, u3);
 */
#define vx_firu8s8p(a, b, c, d) ( a = builtvx_firu8s8p(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* firu8s8p1clr Dr, Dr, Dr
 * Dr firu8s8p1clr (Dr, Dr);
 */
#define vx_firu8s8p1clr(a, b, c) ( a = builtvx_firu8s8p1clr(b.high, b.low, c.high, c.low) )

/* getsad0 Dr, Dr, Dr
 * Dr getsad0 (Dr, Dr);
 */
#define vx_getsad0(a, b, c) ( a = builtvx_getsad0(b.high, b.low, c.high, c.low) )

/* incgt Vr, Vr, gpr
 * Vr incgt (Vr, Vr, gpr);
 */
#define vx_incgt(a, b, c) ( a = builtvx_incgt(a, b, c) )

/* incgth3 Vr, Dr, Vr, gpr
 * Vr incgth3 (Vr, Dr, Vr, gpr);
 */
#define vx_incgth3(a, b, c, d) ( a = builtvx_incgth3(a, b.high, b.low, c, d) )

/* incgth3clr Vr, Dr, Vr, gpr
 * Vr incgth3clr (Dr, Vr, gpr);
 */
#define vx_incgth3clr(a, b, c, d) ( a = builtvx_incgth3clr(b.high, b.low, c, d) )

/* incinsu16 Vr, Dr, gpr, gpr
 * Vr incinsu16 (Vr, Dr, gpr, gpr);
 */
#define vx_incinsu16(a, b, c, d) ( a = builtvx_incinsu16(a, b.high, b.low, c, d) )

/* insmean Dr, Vr, Vr
 * Dr insmean (Dr, Vr, Vr);
 */
#define vx_insmean(a, b, c) ( a = builtvx_insmean(a.high, a.low, b, c) )

/* insmeanr Dr, Vr, Vr
 * Dr insmeanr (Dr, Vr, Vr);
 */
#define vx_insmeanr(a, b, c) ( a = builtvx_insmeanr(a.high, a.low, b, c) )

/* intlvb Dr, Vr, Vr
 * Dr intlvb (Vr, Vr);
 */
#define vx_intlvb(a, b, c) ( a = builtvx_intlvb(b, c) )

/* m2rub gpr, Vr, u3
 * gpr m2rub (Vr, u3);
 */
#define vx_m2rub(a, b, c) ( a = builtvx_m2rub(b, c) )

/* m2ruhw gpr, Vr, u2
 * gpr m2ruhw (Vr, u2);
 */
#define vx_m2ruhw(a, b, c) ( a = builtvx_m2ruhw(b, c) )

/* m2rw0 gpr, Vr
 * gpr m2rw0 (Vr);
 */
#define vx_m2rw0(a, b) ( a = builtvx_m2rw0(b) )

/* m2rw1 gpr, Vr
 * gpr m2rw1 (Vr);
 */
#define vx_m2rw1(a, b) ( a = builtvx_m2rw1(b) )

/* m2v Vr, Vr
 * Vr m2v (Vr);
 */
#define vx_m2v(a, b) ( a = b )

/* m2x Vr, gpr, gpr
 * Vr m2x (gpr, gpr);
 */
#define vx_m2x(a, b, c) ( a = builtvx_m2x(b, c) )

/* m2xb Vr, gpr, u3
 * Vr m2xb (Vr, gpr, u3);
 */
#define vx_m2xb(a, b, c) ( a = builtvx_m2xb(a, b, c) )

/* m2xhw Vr, gpr, u2
 * Vr m2xhw (Vr, gpr, u2);
 */
#define vx_m2xhw(a, b, c) ( a = builtvx_m2xhw(a, b, c) )

/* m2xshldb Vr, gpr, gpr
 * Vr m2xshldb (Vr, gpr, gpr);
 */
#define vx_m2xshldb(a, b, c) ( a = builtvx_m2xshldb(a, b, c) )

/* m2xw0 Vr, gpr
 * Vr m2xw0 (Vr, gpr);
 */
#define vx_m2xw0(a, b) ( a = builtvx_m2xw0(a, b) )

/* m2xw1 Vr, gpr
 * Vr m2xw1 (Vr, gpr);
 */
#define vx_m2xw1(a, b) ( a = builtvx_m2xw1(a, b) )

/* maru8s8 Dr, Vr, gpr
 * Dr maru8s8 (Dr, Vr, gpr);
 */
#define vx_maru8s8(a, b, c) ( a = builtvx_maru8s8(a.high, a.low, b, c) )

/* maru8s8shr7 Dr, Vr, gpr
 * Dr maru8s8shr7 (Dr, Vr, gpr);
 */
#define vx_maru8s8shr7(a, b, c) ( a = builtvx_maru8s8shr7(a.high, a.low, b, c) )

/* maru8s8shr7r Dr, Vr, gpr
 * Dr maru8s8shr7r (Dr, Vr, gpr);
 */
#define vx_maru8s8shr7r(a, b, c) ( a = builtvx_maru8s8shr7r(a.high, a.low, b, c) )

/* maru8u8 Dr, Vr, gpr
 * Dr maru8u8 (Dr, Vr, gpr);
 */
#define vx_maru8u8(a, b, c) ( a = builtvx_maru8u8(a.high, a.low, b, c) )

/* maru8u8shr7 Dr, Vr, gpr
 * Dr maru8u8shr7 (Dr, Vr, gpr);
 */
#define vx_maru8u8shr7(a, b, c) ( a = builtvx_maru8u8shr7(a.high, a.low, b, c) )

/* maru8u8shr7r Dr, Vr, gpr
 * Dr maru8u8shr7r (Dr, Vr, gpr);
 */
#define vx_maru8u8shr7r(a, b, c) ( a = builtvx_maru8u8shr7r(a.high, a.low, b, c) )

/* maviu8s8 Dr, Vr, Vr
 * Dr maviu8s8 (Dr, Vr, Vr);
 */
#define vx_maviu8s8(a, b, c) ( a = builtvx_maviu8s8(a.high, a.low, b, c) )

/* maviu8s8shr7 Dr, Vr, Vr
 * Dr maviu8s8shr7 (Dr, Vr, Vr);
 */
#define vx_maviu8s8shr7(a, b, c) ( a = builtvx_maviu8s8shr7(a.high, a.low, b, c) )

/* maviu8s8shr7r Dr, Vr, Vr
 * Dr maviu8s8shr7r (Dr, Vr, Vr);
 */
#define vx_maviu8s8shr7r(a, b, c) ( a = builtvx_maviu8s8shr7r(a.high, a.low, b, c) )

/* maviu8u8 Dr, Vr, Vr
 * Dr maviu8u8 (Dr, Vr, Vr);
 */
#define vx_maviu8u8(a, b, c) ( a = builtvx_maviu8u8(a.high, a.low, b, c) )

/* maviu8u8shr7 Dr, Vr, Vr
 * Dr maviu8u8shr7 (Dr, Vr, Vr);
 */
#define vx_maviu8u8shr7(a, b, c) ( a = builtvx_maviu8u8shr7(a.high, a.low, b, c) )

/* maviu8u8shr7r Dr, Vr, Vr
 * Dr maviu8u8shr7r (Dr, Vr, Vr);
 */
#define vx_maviu8u8shr7r(a, b, c) ( a = builtvx_maviu8u8shr7r(a.high, a.low, b, c) )

/* mavu8s8 Dr, Vr, Vr
 * Dr mavu8s8 (Dr, Vr, Vr);
 */
#define vx_mavu8s8(a, b, c) ( a = builtvx_mavu8s8(a.high, a.low, b, c) )

/* mavu8s8shr7 Dr, Vr, Vr
 * Dr mavu8s8shr7 (Dr, Vr, Vr);
 */
#define vx_mavu8s8shr7(a, b, c) ( a = builtvx_mavu8s8shr7(a.high, a.low, b, c) )

/* mavu8s8shr7r Dr, Vr, Vr
 * Dr mavu8s8shr7r (Dr, Vr, Vr);
 */
#define vx_mavu8s8shr7r(a, b, c) ( a = builtvx_mavu8s8shr7r(a.high, a.low, b, c) )

/* mavu8u8 Dr, Vr, Vr
 * Dr mavu8u8 (Dr, Vr, Vr);
 */
#define vx_mavu8u8(a, b, c) ( a = builtvx_mavu8u8(a.high, a.low, b, c) )

/* mavu8u8shr7 Dr, Vr, Vr
 * Dr mavu8u8shr7 (Dr, Vr, Vr);
 */
#define vx_mavu8u8shr7(a, b, c) ( a = builtvx_mavu8u8shr7(a.high, a.low, b, c) )

/* mavu8u8shr7r Dr, Vr, Vr
 * Dr mavu8u8shr7r (Dr, Vr, Vr);
 */
#define vx_mavu8u8shr7r(a, b, c) ( a = builtvx_mavu8u8shr7r(a.high, a.low, b, c) )

/* max Vr, Vr, Vr
 * Vr max (Vr, Vr);
 */
#define vx_max(a, b, c) ( a = builtvx_max(b, c) )

/* maxh3 Vr, Dr, Vr
 * Vr maxh3 (Dr, Vr);
 */
#define vx_maxh3(a, b, c) ( a = builtvx_maxh3(b.high, b.low, c) )

/* maxpair Vr, Vr
 * Vr maxpair (Vr);
 */
#define vx_maxpair(a, b) ( a = builtvx_maxpair(b) )

/* mean Vr, Vr, Vr
 * Vr mean (Vr, Vr);
 */
#define vx_mean(a, b, c) ( a = builtvx_mean(b, c) )

/* meanr Vr, Vr, Vr
 * Vr meanr (Vr, Vr);
 */
#define vx_meanr(a, b, c) ( a = builtvx_meanr(b, c) )

/* meanuv Dr, Dr, Dr
 * Dr meanuv (Dr, Dr, Dr);
 */
#define vx_meanuv(a, b, c) ( a = builtvx_meanuv(a.high, a.low, b.high, b.low, c.high, c.low) )

/* meanuvr Dr, Dr, Dr
 * Dr meanuvr (Dr, Dr, Dr);
 */
#define vx_meanuvr(a, b, c) ( a = builtvx_meanuvr(a.high, a.low, b.high, b.low, c.high, c.low) )

/* meany Dr, Dr, Dr
 * Dr meany (Dr, Dr, Dr);
 */
#define vx_meany(a, b, c) ( a = builtvx_meany(a.high, a.low, b.high, b.low, c.high, c.low) )

/* meanyr Dr, Dr, Dr
 * Dr meanyr (Dr, Dr, Dr);
 */
#define vx_meanyr(a, b, c) ( a = builtvx_meanyr(a.high, a.low, b.high, b.low, c.high, c.low) )

/* median Vr, Dr, Vr
 * Vr median (Dr, Vr);
 */
#define vx_median(a, b, c) ( a = builtvx_median(b.high, b.low, c) )

/* mf Vr, Vr, gpr
 * Vr mf (Vr, gpr);
 */
#define vx_mf(a, b, c) ( a = builtvx_mf(b, c) )

/* mfr Vr, Vr, gpr
 * Vr mfr (Vr, gpr);
 */
#define vx_mfr(a, b, c) ( a = builtvx_mfr(b, c) )

/* min Vr, Vr, Vr
 * Vr min (Vr, Vr);
 */
#define vx_min(a, b, c) ( a = builtvx_min(b, c) )

/* minsad Dr, Dr, Dr
 * Dr minsad (Dr, Dr);
 */
#define vx_minsad(a, b, c) ( a = builtvx_minsad(b.high, b.low, c.high, c.low) )

/* modand Vr, Vr
 * Vr modand (Vr);
 */
#define vx_modand(a, b) ( a = builtvx_modand(b) )

/* mpru8s8 Dr, Vr, gpr
 * Dr mpru8s8 (Vr, gpr);
 */
#define vx_mpru8s8(a, b, c) ( a = builtvx_mpru8s8(b, c) )

/* mpru8u8 Dr, Vr, gpr
 * Dr mpru8u8 (Vr, gpr);
 */
#define vx_mpru8u8(a, b, c) ( a = builtvx_mpru8u8(b, c) )

/* mpu8u8shr Vr, Vr, gpr, gpr
 * Vr mpu8u8shr (Vr, gpr, gpr);
 */
#define vx_mpu8u8shr(a, b, c, d) ( a = builtvx_mpu8u8shr(b, c, d) )

/* mpviu8s8 Dr, Vr, Vr
 * Dr mpviu8s8 (Vr, Vr);
 */
#define vx_mpviu8s8(a, b, c) ( a = builtvx_mpviu8s8(b, c) )

/* mpviu8u8 Dr, Vr, Vr
 * Dr mpviu8u8 (Vr, Vr);
 */
#define vx_mpviu8u8(a, b, c) ( a = builtvx_mpviu8u8(b, c) )

/* mpvu8s8 Dr, Vr, Vr
 * Dr mpvu8s8 (Vr, Vr);
 */
#define vx_mpvu8s8(a, b, c) ( a = builtvx_mpvu8s8(b, c) )

/* mpvu8u8 Dr, Vr, Vr
 * Dr mpvu8u8 (Vr, Vr);
 */
#define vx_mpvu8u8(a, b, c) ( a = builtvx_mpvu8u8(b, c) )

/* offset Vr, Vr, gpr
 * Vr offset (Vr, gpr);
 */
#define vx_offset(a, b, c) ( a = builtvx_offset(b, c) )

/* random Vr, Vr
 * Vr random (Vr);
 */
#define vx_random(a, b) ( a = builtvx_random(b) )

/* sad Dr, Dr, Dr, gpr
 * Dr sad (Dr, Dr, gpr);
 */
#define vx_sad(a, b, c, d) ( a = builtvx_sad(b.high, b.low, c.high, c.low, d) )

/* sadmin Dr, Dr, Dr, gpr
 * Dr sadmin (Dr, Dr, Dr, gpr);
 */
#define vx_sadmin(a, b, c, d) ( a = builtvx_sadmin(a.high, a.low, b.high, b.low, c.high, c.low, d) )

/* scales8s9 Vr, Vr, gpr, gpr
 * Vr scales8s9 (Vr, gpr, gpr);
 */
#define vx_scales8s9(a, b, c, d) ( a = builtvx_scales8s9(b, c, d) )

/* select Vr, Vr, Vr
 * Vr select (Vr, Vr, Vr);
 */
#define vx_select(a, b, c) ( a = builtvx_select(a, b, c) )

/* shl1s16 Dr, Dr
 * Dr shl1s16 (Dr);
 */
#define vx_shl1s16(a, b) ( a = builtvx_shl1s16(b.high, b.low) )

/* shr1s16 Dr, Dr
 * Dr shr1s16 (Dr);
 */
#define vx_shr1s16(a, b) ( a = builtvx_shr1s16(b.high, b.low) )

/* shr7s16s8rc Vr, Dr
 * Vr shr7s16s8rc (Dr);
 */
#define vx_shr7s16s8rc(a, b) ( a = builtvx_shr7s16s8rc(b.high, b.low) )

/* shr7s16s8rs Vr, Dr
 * Vr shr7s16s8rs (Dr);
 */
#define vx_shr7s16s8rs(a, b) ( a = builtvx_shr7s16s8rs(b.high, b.low) )

/* shrrs16s8 Vr, Dr, gpr
 * Vr shrrs16s8 (Dr, gpr);
 */
#define vx_shrrs16s8(a, b, c) ( a = builtvx_shrrs16s8(b.high, b.low, c) )

/* shrrs16s8r Vr, Dr, gpr
 * Vr shrrs16s8r (Dr, gpr);
 */
#define vx_shrrs16s8r(a, b, c) ( a = builtvx_shrrs16s8r(b.high, b.low, c) )

/* shrs16s8 Vr, Dr, u4
 * Vr shrs16s8 (Dr, u4);
 */
#define vx_shrs16s8(a, b, c) ( a = builtvx_shrs16s8(b.high, b.low, c) )

/* shrs16s8r Vr, Dr, u4
 * Vr shrs16s8r (Dr, u4);
 */
#define vx_shrs16s8r(a, b, c) ( a = builtvx_shrs16s8r(b.high, b.low, c) )

/* sub128 Vr, Vr
 * Vr sub128 (Vr);
 */
#define vx_sub128(a, b) ( a = builtvx_sub128(b) )

/* subs16 Dr, Dr, Dr
 * Dr subs16 (Dr, Dr);
 */
#define vx_subs16(a, b, c) ( a = builtvx_subs16(b.high, b.low, c.high, c.low) )

/* subs16shr Vr, Dr, Dr, u4
 * Vr subs16shr (Dr, Dr, u4);
 */
#define vx_subs16shr(a, b, c, d) ( a = builtvx_subs16shr(b.high, b.low, c.high, c.low, d) )

/* subs16shrr Vr, Dr, Dr, u4
 * Vr subs16shrr (Dr, Dr, u4);
 */
#define vx_subs16shrr(a, b, c, d) ( a = builtvx_subs16shrr(b.high, b.low, c.high, c.low, d) )

/* Instructions supported only
 * through assembly statement
 */
#define vx_lwh( Op_1, address ) \
  Op_1 = ((unsigned long long)((unsigned int)(Op_1))) | (((unsigned long long)(*((unsigned int*)(address)))) << 32);

#define vx_lwl( Op_1, address ) \
  Op_1 = ((((unsigned long long)(Op_1))>>32)<<32) | (unsigned long long)(*((unsigned int*)(address)));

#define vx_swh( address, Op_1 ) \
  *((unsigned int*)(address)) = (((unsigned long long)(Op_1))>>32);

#define vx_swl( address, Op_1 ) \
  *((unsigned int*)(address)) = (unsigned int)(Op_1);


#ifdef __vx_DEPRECATED
/*############################################################################*
 *
 * Define the deprecated builtin syntax: a = builtvx_<mnemonic>(b, c)
 * as well as deprecated types: INT128, INT64
 *
 *############################################################################*/

/*==================================================================*
 *
 * Define deprecated type names
 * Note that INT128 is already defined (as a structure)
 *
 *==================================================================*/

typedef vx_64 INT64;

/*------------------------------------------------------------------*
 *
 * Deprecated user to C model mapping of vx instructions
 *
 *------------------------------------------------------------------*/

/* absd Vr, Vr, Vr  =  absd a[w], b[r], c[r]
 * Vr absd (Vr, Vr);
 */
#define builtvx_absd(b, c) builtvx_absd(b, c)

/* absdhm1 Vr, Vr, Vr  =  absdhm1 a[w], b[r], c[r]
 * Vr absdhm1 (Vr, Vr);
 */
#define builtvx_absdhm1(b, c) builtvx_absdhm1(b, c)

/* absdhp1 Vr, Vr, Vr  =  absdhp1 a[w], b[r], c[r]
 * Vr absdhp1 (Vr, Vr);
 */
#define builtvx_absdhp1(b, c) builtvx_absdhp1(b, c)

/* absdmax Vr, Dr, Vr  =  absdmax a[w], b[r], c[r]
 * Vr absdmax (Dr, Vr);
 */
#define builtvx_absdmax(b, c) builtvx_absdmax(b.high, b.low, c)

/* absdmin Vr, Dr, Vr  =  absdmin a[w], b[r], c[r]
 * Vr absdmin (Dr, Vr);
 */
#define builtvx_absdmin(b, c) builtvx_absdmin(b.high, b.low, c)

/* absdmpslt Dr, Dr, Dr, gpr  =  absdmpslt a[rw], b[r], c[r], d[r]
 * Dr absdmpslt (Dr, Dr, Dr, gpr);
 */
#define builtvx_absdmpslt(a, b, c, d) builtvx_absdmpslt(a.high, a.low, b.high, b.low, c.high, c.low, d)

/* absdslt Dr, Dr, Vr  =  absdslt a[rw], b[r], c[r]
 * Dr absdslt (Dr, Dr, Vr);
 */
#define builtvx_absdslt(a, b, c) builtvx_absdslt(a.high, a.low, b.high, b.low, c)

/* acc Vr, Vr  =  acc a[rw], b[r]
 * Vr acc (Vr, Vr);
 */
#define builtvx_acc(a, b) builtvx_acc(a, b)

/* adds16 Dr, Dr, Dr  =  adds16 a[w], b[r], c[r]
 * Dr adds16 (Dr, Dr);
 */
#define builtvx_adds16(b, c) builtvx_adds16(b.high, b.low, c.high, c.low)

/* adds16shr Vr, Dr, Dr, u4  =  adds16shr a[w], b[r], c[r], d[r]
 * Vr adds16shr (Dr, Dr, u4);
 */
#define builtvx_adds16shr(b, c, d) builtvx_adds16shr(b.high, b.low, c.high, c.low, d)

/* adds16shrr Vr, Dr, Dr, u4  =  adds16shrr a[w], b[r], c[r], d[r]
 * Vr adds16shrr (Dr, Dr, u4);
 */
#define builtvx_adds16shrr(b, c, d) builtvx_adds16shrr(b.high, b.low, c.high, c.low, d)

/* addu16m1 Dr, Dr, Dr  =  addu16m1 a[w], b[r], c[r]
 * Dr addu16m1 (Dr, Dr);
 */
#define builtvx_addu16m1(b, c) builtvx_addu16m1(b.high, b.low, c.high, c.low)

/* addu16m2 Dr, Dr, Dr  =  addu16m2 a[rw], b[r], c[r]
 * Dr addu16m2 (Dr, Dr, Dr);
 */
#define builtvx_addu16m2(a, b, c) builtvx_addu16m2(a.high, a.low, b.high, b.low, c.high, c.low)

/* addu16shr6 Vr, Vr, Vr  =  addu16shr6 a[w], b[r], c[r]
 * Vr addu16shr6 (Vr, Vr);
 */
#define builtvx_addu16shr6(b, c) builtvx_addu16shr6(b, c)

/* addu8clp63 Vr, Vr, gpr  =  addu8clp63 a[w], b[r], c[r]
 * Vr addu8clp63 (Vr, gpr);
 */
#define builtvx_addu8clp63(b, c) builtvx_addu8clp63(b, c)

/* addvu8u8 Dr, Vr, Vr  =  addvu8u8 a[w], b[r], c[r]
 * Dr addvu8u8 (Vr, Vr);
 */
#define builtvx_addvu8u8(b, c) builtvx_addvu8u8(b, c)

/* ascmf Dr, Dr, gpr  =  ascmf a[w], b[r], c[r]
 * Dr ascmf (Dr, gpr);
 */
#define builtvx_ascmf(b, c) builtvx_ascmf(b.high, b.low, c)

/* ascmfr Dr, Dr, gpr  =  ascmfr a[w], b[r], c[r]
 * Dr ascmfr (Dr, gpr);
 */
#define builtvx_ascmfr(b, c) builtvx_ascmfr(b.high, b.low, c)

/* bshr Vr, Vr, Vr, u4  =  bshr a[w], b[r], c[r], d[r]
 * Vr bshr (Vr, Vr, u4);
 */
#define builtvx_bshr(b, c, d) builtvx_bshr(b, c, d)

/* bshrr Vr, Vr, Vr, gpr  =  bshrr a[w], b[r], c[r], d[r]
 * Vr bshrr (Vr, Vr, gpr);
 */
#define builtvx_bshrr(b, c, d) builtvx_bshrr(b, c, d)

/* clp63 Vr, Vr  =  clp63 a[w], b[r]
 * Vr clp63 (Vr);
 */
#define builtvx_clp63(b) builtvx_clp63(b)

/* clpsym Vr, Dr, gpr  =  clpsym a[w], b[r], c[r]
 * Vr clpsym (Dr, gpr);
 */
#define builtvx_clpsym(b, c) builtvx_clpsym(b.high, b.low, c)

/* clpu8 Vr, Vr, gpr  =  clpu8 a[w], b[r], c[r]
 * Vr clpu8 (Vr, gpr);
 */
#define builtvx_clpu8(b, c) builtvx_clpu8(b, c)

/* clr Vr  =  clr a[w]
 * Vr clr ();
 */
#define builtvx_clr() builtvx_clr()

/* cmpeqru8 Vr, Vr, gpr  =  cmpeqru8 a[w], b[r], c[r]
 * Vr cmpeqru8 (Vr, gpr);
 */
#define builtvx_cmpeqru8(b, c) builtvx_cmpeqru8(b, c)

/* cmpgeru8 Vr, Vr, gpr  =  cmpgeru8 a[w], b[r], c[r]
 * Vr cmpgeru8 (Vr, gpr);
 */
#define builtvx_cmpgeru8(b, c) builtvx_cmpgeru8(b, c)

/* cmpgtru8 Vr, Vr, gpr  =  cmpgtru8 a[w], b[r], c[r]
 * Vr cmpgtru8 (Vr, gpr);
 */
#define builtvx_cmpgtru8(b, c) builtvx_cmpgtru8(b, c)

/* cmpleru8 Vr, Vr, gpr  =  cmpleru8 a[w], b[r], c[r]
 * Vr cmpleru8 (Vr, gpr);
 */
#define builtvx_cmpleru8(b, c) builtvx_cmpleru8(b, c)

/* cmpltru8 Vr, Vr, gpr  =  cmpltru8 a[w], b[r], c[r]
 * Vr cmpltru8 (Vr, gpr);
 */
#define builtvx_cmpltru8(b, c) builtvx_cmpltru8(b, c)

/* cmpneru8 Vr, Vr, gpr  =  cmpneru8 a[w], b[r], c[r]
 * Vr cmpneru8 (Vr, gpr);
 */
#define builtvx_cmpneru8(b, c) builtvx_cmpneru8(b, c)

/* cntdelta0clr Dr, Dr, Dr  =  cntdelta0clr a[rw], b[r], c[r]
 * Dr cntdelta0clr (Dr, Dr, Dr);
 */
#define builtvx_cntdelta0clr(a, b, c) builtvx_cntdelta0clr(a.high, a.low, b.high, b.low, c.high, c.low)

/* cntdeltabw Dr, Dr, Dr, u2  =  cntdeltabw a[rw], b[r], c[r], d[r]
 * Dr cntdeltabw (Dr, Dr, Dr, u2);
 */
#define builtvx_cntdeltabw(a, b, c, d) builtvx_cntdeltabw(a.high, a.low, b.high, b.low, c.high, c.low, d)

/* cntdeltafw Dr, Dr, Dr, u2  =  cntdeltafw a[rw], b[r], c[r], d[r]
 * Dr cntdeltafw (Dr, Dr, Dr, u2);
 */
#define builtvx_cntdeltafw(a, b, c, d) builtvx_cntdeltafw(a.high, a.low, b.high, b.low, c.high, c.low, d)

/* cplsb Vr, Vr, u3  =  cplsb a[w], b[r], c[r]
 * Vr cplsb (Vr, u3);
 */
#define builtvx_cplsb(b, c) builtvx_cplsb(b, c)

/* cplsbi Vr, Vr, u2  =  cplsbi a[w], b[r], c[r]
 * Vr cplsbi (Vr, u2);
 */
#define builtvx_cplsbi(b, c) builtvx_cplsbi(b, c)

/* cpmsb Vr, Vr, u3  =  cpmsb a[w], b[r], c[r]
 * Vr cpmsb (Vr, u3);
 */
#define builtvx_cpmsb(b, c) builtvx_cpmsb(b, c)

/* cpmsbi Vr, Vr, u2  =  cpmsbi a[w], b[r], c[r]
 * Vr cpmsbi (Vr, u2);
 */
#define builtvx_cpmsbi(b, c) builtvx_cpmsbi(b, c)

/* cpmsbir Vr, Vr, gpr  =  cpmsbir a[w], b[r], c[r]
 * Vr cpmsbir (Vr, gpr);
 */
#define builtvx_cpmsbir(b, c) builtvx_cpmsbir(b, c)

/* cpmsbr Vr, Vr, gpr  =  cpmsbr a[w], b[r], c[r]
 * Vr cpmsbr (Vr, gpr);
 */
#define builtvx_cpmsbr(b, c) builtvx_cpmsbr(b, c)

/* dintlvb Dr, Vr, Vr  =  dintlvb a[w], b[r], c[r]
 * Dr dintlvb (Vr, Vr);
 */
#define builtvx_dintlvb(b, c) builtvx_dintlvb(b, c)

/* dlupdate Dr, Vr, Vr  =  dlupdate a[rw], b[r], c[r]
 * Dr dlupdate (Dr, Vr, Vr);
 */
#define builtvx_dlupdate(a, b, c) builtvx_dlupdate(a.high, a.low, b, c)

/* fir3 Vr, Dr, Vr  =  fir3 a[w], b[r], c[r]
 * Vr fir3 (Dr, Vr);
 */
#define builtvx_fir3(b, c) builtvx_fir3(b.high, b.low, c)

/* fir3edge Dr, Vr, Vr  =  fir3edge a[w], b[r], c[r]
 * Dr fir3edge (Vr, Vr);
 */
#define builtvx_fir3edge(b, c) builtvx_fir3edge(b, c)

/* firu8s8p Dr, Dr, Dr, u3  =  firu8s8p a[rw], b[r], c[r], d[r]
 * Dr firu8s8p (Dr, Dr, Dr, u3);
 */
#define builtvx_firu8s8p(a, b, c, d) builtvx_firu8s8p(a.high, a.low, b.high, b.low, c.high, c.low, d)

/* firu8s8p1clr Dr, Dr, Dr  =  firu8s8p1clr a[w], b[r], c[r]
 * Dr firu8s8p1clr (Dr, Dr);
 */
#define builtvx_firu8s8p1clr(b, c) builtvx_firu8s8p1clr(b.high, b.low, c.high, c.low)

/* getsad0 Dr, Dr, Dr  =  getsad0 a[w], b[r], c[r]
 * Dr getsad0 (Dr, Dr);
 */
#define builtvx_getsad0(b, c) builtvx_getsad0(b.high, b.low, c.high, c.low)

/* incgt Vr, Vr, gpr  =  incgt a[rw], b[r], c[r]
 * Vr incgt (Vr, Vr, gpr);
 */
#define builtvx_incgt(a, b, c) builtvx_incgt(a, b, c)

/* incgth3 Vr, Dr, Vr, gpr  =  incgth3 a[rw], b[r], c[r], d[r]
 * Vr incgth3 (Vr, Dr, Vr, gpr);
 */
#define builtvx_incgth3(a, b, c, d) builtvx_incgth3(a, b.high, b.low, c, d)

/* incgth3clr Vr, Dr, Vr, gpr  =  incgth3clr a[w], b[r], c[r], d[r]
 * Vr incgth3clr (Dr, Vr, gpr);
 */
#define builtvx_incgth3clr(b, c, d) builtvx_incgth3clr(b.high, b.low, c, d)

/* incinsu16 Vr, Dr, gpr, gpr  =  incinsu16 a[rw], b[r], c[r], d[r]
 * Vr incinsu16 (Vr, Dr, gpr, gpr);
 */
#define builtvx_incinsu16(a, b, c, d) builtvx_incinsu16(a, b.high, b.low, c, d)

/* insmean Dr, Vr, Vr  =  insmean a[rw], b[r], c[r]
 * Dr insmean (Dr, Vr, Vr);
 */
#define builtvx_insmean(a, b, c) builtvx_insmean(a.high, a.low, b, c)

/* insmeanr Dr, Vr, Vr  =  insmeanr a[rw], b[r], c[r]
 * Dr insmeanr (Dr, Vr, Vr);
 */
#define builtvx_insmeanr(a, b, c) builtvx_insmeanr(a.high, a.low, b, c)

/* intlvb Dr, Vr, Vr  =  intlvb a[w], b[r], c[r]
 * Dr intlvb (Vr, Vr);
 */
#define builtvx_intlvb(b, c) builtvx_intlvb(b, c)

/* m2rub gpr, Vr, u3  =  m2rub a[w], b[r], c[r]
 * gpr m2rub (Vr, u3);
 */
#define builtvx_m2rub(b, c) builtvx_m2rub(b, c)

/* m2ruhw gpr, Vr, u2  =  m2ruhw a[w], b[r], c[r]
 * gpr m2ruhw (Vr, u2);
 */
#define builtvx_m2ruhw(b, c) builtvx_m2ruhw(b, c)

/* m2rw0 gpr, Vr  =  m2rw0 a[w], b[r]
 * gpr m2rw0 (Vr);
 */
#define builtvx_m2rw0(b) builtvx_m2rw0(b)

/* m2rw1 gpr, Vr  =  m2rw1 a[w], b[r]
 * gpr m2rw1 (Vr);
 */
#define builtvx_m2rw1(b) builtvx_m2rw1(b)

/* m2v Vr, Vr  =  m2v a[w], b[r]
 * Vr m2v (Vr);
 */
#define builtvx_m2v(b) builtvx_m2v(b)

/* m2x Vr, gpr, gpr  =  m2x a[w], b[r], c[r]
 * Vr m2x (gpr, gpr);
 */
#define builtvx_m2x(b, c) builtvx_m2x(b, c)

/* m2xb Vr, gpr, u3  =  m2xb a[rw], b[r], c[r]
 * Vr m2xb (Vr, gpr, u3);
 */
#define builtvx_m2xb(a, b, c) builtvx_m2xb(a, b, c)

/* m2xhw Vr, gpr, u2  =  m2xhw a[rw], b[r], c[r]
 * Vr m2xhw (Vr, gpr, u2);
 */
#define builtvx_m2xhw(a, b, c) builtvx_m2xhw(a, b, c)

/* m2xshldb Vr, gpr, gpr  =  m2xshldb a[rw], b[r], c[r]
 * Vr m2xshldb (Vr, gpr, gpr);
 */
#define builtvx_m2xshldb(a, b, c) builtvx_m2xshldb(a, b, c)

/* m2xw0 Vr, gpr  =  m2xw0 a[rw], b[r]
 * Vr m2xw0 (Vr, gpr);
 */
#define builtvx_m2xw0(a, b) builtvx_m2xw0(a, b)

/* m2xw1 Vr, gpr  =  m2xw1 a[rw], b[r]
 * Vr m2xw1 (Vr, gpr);
 */
#define builtvx_m2xw1(a, b) builtvx_m2xw1(a, b)

/* maru8s8 Dr, Vr, gpr  =  maru8s8 a[rw], b[r], c[r]
 * Dr maru8s8 (Dr, Vr, gpr);
 */
#define builtvx_maru8s8(a, b, c) builtvx_maru8s8(a.high, a.low, b, c)

/* maru8s8shr7 Dr, Vr, gpr  =  maru8s8shr7 a[rw], b[r], c[r]
 * Dr maru8s8shr7 (Dr, Vr, gpr);
 */
#define builtvx_maru8s8shr7(a, b, c) builtvx_maru8s8shr7(a.high, a.low, b, c)

/* maru8s8shr7r Dr, Vr, gpr  =  maru8s8shr7r a[rw], b[r], c[r]
 * Dr maru8s8shr7r (Dr, Vr, gpr);
 */
#define builtvx_maru8s8shr7r(a, b, c) builtvx_maru8s8shr7r(a.high, a.low, b, c)

/* maru8u8 Dr, Vr, gpr  =  maru8u8 a[rw], b[r], c[r]
 * Dr maru8u8 (Dr, Vr, gpr);
 */
#define builtvx_maru8u8(a, b, c) builtvx_maru8u8(a.high, a.low, b, c)

/* maru8u8shr7 Dr, Vr, gpr  =  maru8u8shr7 a[rw], b[r], c[r]
 * Dr maru8u8shr7 (Dr, Vr, gpr);
 */
#define builtvx_maru8u8shr7(a, b, c) builtvx_maru8u8shr7(a.high, a.low, b, c)

/* maru8u8shr7r Dr, Vr, gpr  =  maru8u8shr7r a[rw], b[r], c[r]
 * Dr maru8u8shr7r (Dr, Vr, gpr);
 */
#define builtvx_maru8u8shr7r(a, b, c) builtvx_maru8u8shr7r(a.high, a.low, b, c)

/* maviu8s8 Dr, Vr, Vr  =  maviu8s8 a[rw], b[r], c[r]
 * Dr maviu8s8 (Dr, Vr, Vr);
 */
#define builtvx_maviu8s8(a, b, c) builtvx_maviu8s8(a.high, a.low, b, c)

/* maviu8s8shr7 Dr, Vr, Vr  =  maviu8s8shr7 a[rw], b[r], c[r]
 * Dr maviu8s8shr7 (Dr, Vr, Vr);
 */
#define builtvx_maviu8s8shr7(a, b, c) builtvx_maviu8s8shr7(a.high, a.low, b, c)

/* maviu8s8shr7r Dr, Vr, Vr  =  maviu8s8shr7r a[rw], b[r], c[r]
 * Dr maviu8s8shr7r (Dr, Vr, Vr);
 */
#define builtvx_maviu8s8shr7r(a, b, c) builtvx_maviu8s8shr7r(a.high, a.low, b, c)

/* maviu8u8 Dr, Vr, Vr  =  maviu8u8 a[rw], b[r], c[r]
 * Dr maviu8u8 (Dr, Vr, Vr);
 */
#define builtvx_maviu8u8(a, b, c) builtvx_maviu8u8(a.high, a.low, b, c)

/* maviu8u8shr7 Dr, Vr, Vr  =  maviu8u8shr7 a[rw], b[r], c[r]
 * Dr maviu8u8shr7 (Dr, Vr, Vr);
 */
#define builtvx_maviu8u8shr7(a, b, c) builtvx_maviu8u8shr7(a.high, a.low, b, c)

/* maviu8u8shr7r Dr, Vr, Vr  =  maviu8u8shr7r a[rw], b[r], c[r]
 * Dr maviu8u8shr7r (Dr, Vr, Vr);
 */
#define builtvx_maviu8u8shr7r(a, b, c) builtvx_maviu8u8shr7r(a.high, a.low, b, c)

/* mavu8s8 Dr, Vr, Vr  =  mavu8s8 a[rw], b[r], c[r]
 * Dr mavu8s8 (Dr, Vr, Vr);
 */
#define builtvx_mavu8s8(a, b, c) builtvx_mavu8s8(a.high, a.low, b, c)

/* mavu8s8shr7 Dr, Vr, Vr  =  mavu8s8shr7 a[rw], b[r], c[r]
 * Dr mavu8s8shr7 (Dr, Vr, Vr);
 */
#define builtvx_mavu8s8shr7(a, b, c) builtvx_mavu8s8shr7(a.high, a.low, b, c)

/* mavu8s8shr7r Dr, Vr, Vr  =  mavu8s8shr7r a[rw], b[r], c[r]
 * Dr mavu8s8shr7r (Dr, Vr, Vr);
 */
#define builtvx_mavu8s8shr7r(a, b, c) builtvx_mavu8s8shr7r(a.high, a.low, b, c)

/* mavu8u8 Dr, Vr, Vr  =  mavu8u8 a[rw], b[r], c[r]
 * Dr mavu8u8 (Dr, Vr, Vr);
 */
#define builtvx_mavu8u8(a, b, c) builtvx_mavu8u8(a.high, a.low, b, c)

/* mavu8u8shr7 Dr, Vr, Vr  =  mavu8u8shr7 a[rw], b[r], c[r]
 * Dr mavu8u8shr7 (Dr, Vr, Vr);
 */
#define builtvx_mavu8u8shr7(a, b, c) builtvx_mavu8u8shr7(a.high, a.low, b, c)

/* mavu8u8shr7r Dr, Vr, Vr  =  mavu8u8shr7r a[rw], b[r], c[r]
 * Dr mavu8u8shr7r (Dr, Vr, Vr);
 */
#define builtvx_mavu8u8shr7r(a, b, c) builtvx_mavu8u8shr7r(a.high, a.low, b, c)

/* max Vr, Vr, Vr  =  max a[w], b[r], c[r]
 * Vr max (Vr, Vr);
 */
#define builtvx_max(b, c) builtvx_max(b, c)

/* maxh3 Vr, Dr, Vr  =  maxh3 a[w], b[r], c[r]
 * Vr maxh3 (Dr, Vr);
 */
#define builtvx_maxh3(b, c) builtvx_maxh3(b.high, b.low, c)

/* maxpair Vr, Vr  =  maxpair a[w], b[r]
 * Vr maxpair (Vr);
 */
#define builtvx_maxpair(b) builtvx_maxpair(b)

/* mean Vr, Vr, Vr  =  mean a[w], b[r], c[r]
 * Vr mean (Vr, Vr);
 */
#define builtvx_mean(b, c) builtvx_mean(b, c)

/* meanr Vr, Vr, Vr  =  meanr a[w], b[r], c[r]
 * Vr meanr (Vr, Vr);
 */
#define builtvx_meanr(b, c) builtvx_meanr(b, c)

/* meanuv Dr, Dr, Dr  =  meanuv a[rw], b[r], c[r]
 * Dr meanuv (Dr, Dr, Dr);
 */
#define builtvx_meanuv(a, b, c) builtvx_meanuv(a.high, a.low, b.high, b.low, c.high, c.low)

/* meanuvr Dr, Dr, Dr  =  meanuvr a[rw], b[r], c[r]
 * Dr meanuvr (Dr, Dr, Dr);
 */
#define builtvx_meanuvr(a, b, c) builtvx_meanuvr(a.high, a.low, b.high, b.low, c.high, c.low)

/* meany Dr, Dr, Dr  =  meany a[rw], b[r], c[r]
 * Dr meany (Dr, Dr, Dr);
 */
#define builtvx_meany(a, b, c) builtvx_meany(a.high, a.low, b.high, b.low, c.high, c.low)

/* meanyr Dr, Dr, Dr  =  meanyr a[rw], b[r], c[r]
 * Dr meanyr (Dr, Dr, Dr);
 */
#define builtvx_meanyr(a, b, c) builtvx_meanyr(a.high, a.low, b.high, b.low, c.high, c.low)

/* median Vr, Dr, Vr  =  median a[w], b[r], c[r]
 * Vr median (Dr, Vr);
 */
#define builtvx_median(b, c) builtvx_median(b.high, b.low, c)

/* mf Vr, Vr, gpr  =  mf a[w], b[r], c[r]
 * Vr mf (Vr, gpr);
 */
#define builtvx_mf(b, c) builtvx_mf(b, c)

/* mfr Vr, Vr, gpr  =  mfr a[w], b[r], c[r]
 * Vr mfr (Vr, gpr);
 */
#define builtvx_mfr(b, c) builtvx_mfr(b, c)

/* min Vr, Vr, Vr  =  min a[w], b[r], c[r]
 * Vr min (Vr, Vr);
 */
#define builtvx_min(b, c) builtvx_min(b, c)

/* minsad Dr, Dr, Dr  =  minsad a[w], b[r], c[r]
 * Dr minsad (Dr, Dr);
 */
#define builtvx_minsad(b, c) builtvx_minsad(b.high, b.low, c.high, c.low)

/* modand Vr, Vr  =  modand a[w], b[r]
 * Vr modand (Vr);
 */
#define builtvx_modand(b) builtvx_modand(b)

/* mpru8s8 Dr, Vr, gpr  =  mpru8s8 a[w], b[r], c[r]
 * Dr mpru8s8 (Vr, gpr);
 */
#define builtvx_mpru8s8(b, c) builtvx_mpru8s8(b, c)

/* mpru8u8 Dr, Vr, gpr  =  mpru8u8 a[w], b[r], c[r]
 * Dr mpru8u8 (Vr, gpr);
 */
#define builtvx_mpru8u8(b, c) builtvx_mpru8u8(b, c)

/* mpu8u8shr Vr, Vr, gpr, gpr  =  mpu8u8shr a[w], b[r], c[r], d[r]
 * Vr mpu8u8shr (Vr, gpr, gpr);
 */
#define builtvx_mpu8u8shr(b, c, d) builtvx_mpu8u8shr(b, c, d)

/* mpviu8s8 Dr, Vr, Vr  =  mpviu8s8 a[w], b[r], c[r]
 * Dr mpviu8s8 (Vr, Vr);
 */
#define builtvx_mpviu8s8(b, c) builtvx_mpviu8s8(b, c)

/* mpviu8u8 Dr, Vr, Vr  =  mpviu8u8 a[w], b[r], c[r]
 * Dr mpviu8u8 (Vr, Vr);
 */
#define builtvx_mpviu8u8(b, c) builtvx_mpviu8u8(b, c)

/* mpvu8s8 Dr, Vr, Vr  =  mpvu8s8 a[w], b[r], c[r]
 * Dr mpvu8s8 (Vr, Vr);
 */
#define builtvx_mpvu8s8(b, c) builtvx_mpvu8s8(b, c)

/* mpvu8u8 Dr, Vr, Vr  =  mpvu8u8 a[w], b[r], c[r]
 * Dr mpvu8u8 (Vr, Vr);
 */
#define builtvx_mpvu8u8(b, c) builtvx_mpvu8u8(b, c)

/* offset Vr, Vr, gpr  =  offset a[w], b[r], c[r]
 * Vr offset (Vr, gpr);
 */
#define builtvx_offset(b, c) builtvx_offset(b, c)

/* random Vr, Vr  =  random a[w], b[r]
 * Vr random (Vr);
 */
#define builtvx_random(b) builtvx_random(b)

/* sad Dr, Dr, Dr, gpr  =  sad a[w], b[r], c[r], d[r]
 * Dr sad (Dr, Dr, gpr);
 */
#define builtvx_sad(b, c, d) builtvx_sad(b.high, b.low, c.high, c.low, d)

/* sadmin Dr, Dr, Dr, gpr  =  sadmin a[rw], b[r], c[r], d[r]
 * Dr sadmin (Dr, Dr, Dr, gpr);
 */
#define builtvx_sadmin(a, b, c, d) builtvx_sadmin(a.high, a.low, b.high, b.low, c.high, c.low, d)

/* scales8s9 Vr, Vr, gpr, gpr  =  scales8s9 a[w], b[r], c[r], d[r]
 * Vr scales8s9 (Vr, gpr, gpr);
 */
#define builtvx_scales8s9(b, c, d) builtvx_scales8s9(b, c, d)

/* select Vr, Vr, Vr  =  select a[rw], b[r], c[r]
 * Vr select (Vr, Vr, Vr);
 */
#define builtvx_select(a, b, c) builtvx_select(a, b, c)

/* shl1s16 Dr, Dr  =  shl1s16 a[w], b[r]
 * Dr shl1s16 (Dr);
 */
#define builtvx_shl1s16(b) builtvx_shl1s16(b.high, b.low)

/* shr1s16 Dr, Dr  =  shr1s16 a[w], b[r]
 * Dr shr1s16 (Dr);
 */
#define builtvx_shr1s16(b) builtvx_shr1s16(b.high, b.low)

/* shr7s16s8rc Vr, Dr  =  shr7s16s8rc a[w], b[r]
 * Vr shr7s16s8rc (Dr);
 */
#define builtvx_shr7s16s8rc(b) builtvx_shr7s16s8rc(b.high, b.low)

/* shr7s16s8rs Vr, Dr  =  shr7s16s8rs a[w], b[r]
 * Vr shr7s16s8rs (Dr);
 */
#define builtvx_shr7s16s8rs(b) builtvx_shr7s16s8rs(b.high, b.low)

/* shrrs16s8 Vr, Dr, gpr  =  shrrs16s8 a[w], b[r], c[r]
 * Vr shrrs16s8 (Dr, gpr);
 */
#define builtvx_shrrs16s8(b, c) builtvx_shrrs16s8(b.high, b.low, c)

/* shrrs16s8r Vr, Dr, gpr  =  shrrs16s8r a[w], b[r], c[r]
 * Vr shrrs16s8r (Dr, gpr);
 */
#define builtvx_shrrs16s8r(b, c) builtvx_shrrs16s8r(b.high, b.low, c)

/* shrs16s8 Vr, Dr, u4  =  shrs16s8 a[w], b[r], c[r]
 * Vr shrs16s8 (Dr, u4);
 */
#define builtvx_shrs16s8(b, c) builtvx_shrs16s8(b.high, b.low, c)

/* shrs16s8r Vr, Dr, u4  =  shrs16s8r a[w], b[r], c[r]
 * Vr shrs16s8r (Dr, u4);
 */
#define builtvx_shrs16s8r(b, c) builtvx_shrs16s8r(b.high, b.low, c)

/* sub128 Vr, Vr  =  sub128 a[w], b[r]
 * Vr sub128 (Vr);
 */
#define builtvx_sub128(b) builtvx_sub128(b)

/* subs16 Dr, Dr, Dr  =  subs16 a[w], b[r], c[r]
 * Dr subs16 (Dr, Dr);
 */
#define builtvx_subs16(b, c) builtvx_subs16(b.high, b.low, c.high, c.low)

/* subs16shr Vr, Dr, Dr, u4  =  subs16shr a[w], b[r], c[r], d[r]
 * Vr subs16shr (Dr, Dr, u4);
 */
#define builtvx_subs16shr(b, c, d) builtvx_subs16shr(b.high, b.low, c.high, c.low, d)

/* subs16shrr Vr, Dr, Dr, u4  =  subs16shrr a[w], b[r], c[r], d[r]
 * Vr subs16shrr (Dr, Dr, u4);
 */
#define builtvx_subs16shrr(b, c, d) builtvx_subs16shrr(b.high, b.low, c.high, c.low, d)

/* Instructions supported only
 * through assembly statement
 */
#define builtvx_lwh( Op_1, address ) \
  Op_1 = ((unsigned long long)((unsigned int)(Op_1))) | (((unsigned long long)(*((unsigned int*)(address)))) << 32);

#define builtvx_lwl( Op_1, address ) \
  Op_1 = ((((unsigned long long)(Op_1))>>32)<<32) | (unsigned long long)(*((unsigned int*)(address)));

#define builtvx_swh( address, Op_1 ) \
  *((unsigned int*)(address)) = (((unsigned long long)(Op_1))>>32);

#define builtvx_swl( address, Op_1 ) \
  *((unsigned int*)(address)) = (unsigned int)(Op_1);


#endif  /* __vx_DEPRECATED */


#endif  /* defined( __vx ) && ! defined( __vx_C_MODEL ) */


#endif  /* vx_h */

//============================================================================
//
// END
//
//============================================================================
