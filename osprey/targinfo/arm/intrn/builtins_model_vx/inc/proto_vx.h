/*
 *      Copyright 2006, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/*****************/
/* proto_vx.h    */
/*****************/

#ifndef _PROTO_VX_H
#define _PROTO_VX_H

/* Initialization */

extern void init_vx(void); /* only for C models */

/* DD SI: Detail Detection and Spatial Interpolation */

extern INT128 builtvx_meanyr(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
extern INT128 builtvx_meany(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
	
extern INT128 builtvx_meanuvr(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
extern INT128 builtvx_meanuv(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
	
extern INT128 builtvx_insmeanr(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);
extern INT128 builtvx_insmean(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);

extern INT128 builtvx_getsad0(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);

/* Fader */

extern INT128 builtvx_ascmfr(long long var_Dh_h, long long var_Dh_l, long var_Rn);
extern INT128 builtvx_ascmf(long long var_Dh_h, long long var_Dh_l, long var_Rn);

extern long long builtvx_median(long long var_Dh_h, long long var_Dh_l, long long var_Vs);

extern INT128 builtvx_absdmpslt(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, long var_Rn);
		
extern INT128 builtvx_absdslt( long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Vs);

/* MD TI */

extern long long builtvx_absd(long long var_Vr, long long var_Vs);

extern long long builtvx_mfr(long long var_Vr, long var_Rn);
extern long long builtvx_mf(long long var_Vr, long var_Rn);

extern long long builtvx_max(long long var_Vr, long long var_Vs);
extern long long builtvx_min(long long var_Vr, long long var_Vs);

extern long long builtvx_meanr(long long var_Vr, long long var_Vs);
extern long long builtvx_mean(long long var_Vr, long long var_Vs);

/* SAD */

extern INT128 builtvx_sad( long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, long var_Rn);
extern INT128 builtvx_sadmin(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, long var_Rn);
extern INT128 builtvx_minsad(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);

/* move, formating, etc. */

extern long long builtvx_m2xb(long long var_Vk, long var_Rn, unsigned char imm_u3);
extern long long builtvx_m2xhw(long long var_Vk, long var_Rn, unsigned char imm_u2);
extern long long builtvx_m2x(long var_Rn, long var_Rp);

extern long builtvx_m2rub(long long var_Vr, unsigned char imm_u3);
extern long builtvx_m2ruhw(long long var_Vr, unsigned char imm_u2);

extern long long builtvx_clr(void);

extern long long builtvx_m2xw0(long long var_Vk, long var_Rn);
extern long long builtvx_m2xw1(long long var_Vk, long var_Rn);

extern long builtvx_m2rw0(long long var_Vr);
extern long builtvx_m2rw1(long long var_Vr);

extern long long builtvx_m2xshldb(long long, long, long);
extern INT128 builtvx_intlvb(long long, long long);
extern INT128 builtvx_dintlvb(long long, long long);

extern INT128 builtvx_dlupdate(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);

extern long long builtvx_cplsb(long long var_Vr, unsigned char imm_u3);
extern long long builtvx_cpmsb(long long var_Vr, unsigned char imm_u3);
extern long long builtvx_cplsbi(long long var_Vr, unsigned char imm_u2);
extern long long builtvx_cpmsbi(long long var_Vr, unsigned char imm_u2);
extern long long builtvx_cpmsbr(long long var_Vr, long var_Rn);
extern long long builtvx_cpmsbir(long long var_Vr, long var_Rn);

/* SA */

extern long long builtvx_acc(long long var_Vk, long long var_Vr);
extern long long builtvx_absdmin(long long var_Dh_h, long long var_Dh_l, long long var_Vs);
extern long long builtvx_incgt(long long var_Vk, long long var_Vr, long var_Rn);

/* FGT */

extern long long builtvx_scales8s9(long long, long, long);
extern long long builtvx_mpu8u8shr(long long, long, long);
extern INT128 builtvx_fir3edge(long long, long long);
extern long long builtvx_addu16shr6(long long, long long);
extern long long builtvx_random(long long);
extern long long builtvx_modand(long long);
extern long long builtvx_offset(long long, long);

/* TNR */

extern long long builtvx_absdmax(long long var_Dh_h, long long var_Dh_l, long long var_Vs);
extern long long builtvx_maxpair(long long var_Vr);
extern long long builtvx_clp63(long long var_Vr);
extern long long builtvx_clpu8(long long var_Vr, long var_Rn);
extern long long builtvx_clpsym(long long var_Dh_h, long long var_Dh_l, long var_Rn);
extern INT128 builtvx_firu8s8p(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u3);
extern INT128 builtvx_firu8s8p1clr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);

extern long long builtvx_bshr(long long var_Vr, long long var_Vs, unsigned char imm_u4);
extern long long builtvx_bshrr(long long var_Vr, long long var_Vs, long var_Rn);

extern INT128 builtvx_shr1s16(long long var_Dh_h, long long var_Dh_l);
extern INT128 builtvx_shl1s16(long long var_Dh_h, long long var_Dh_l);

extern INT128 builtvx_adds16(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
extern INT128 builtvx_subs16(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);

extern INT128 builtvx_cntdeltafw(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u3);
extern INT128 builtvx_cntdeltabw(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u3);
extern INT128 builtvx_cntdelta0clr(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);

extern INT128 builtvx_mpru8u8(long long var_Vr, long var_Rn);
extern INT128 builtvx_maru8u8(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn);
extern INT128 builtvx_mpru8s8(long long var_Vr, long var_Rn);
extern INT128 builtvx_maru8s8(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn);

extern INT128 builtvx_mpvu8u8(long long var_Vr, long long var_Vs);
extern INT128 builtvx_mavu8u8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs);

extern INT128 builtvx_maviu8u8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs);
extern INT128 builtvx_mpviu8u8(long long var_Vr, long long var_Vs);

extern INT128 builtvx_mpvu8s8(long long var_Vr, long long var_Vs);
extern INT128 builtvx_mavu8s8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs);

extern INT128 builtvx_maviu8s8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs);
extern INT128 builtvx_mpviu8s8(long long var_Vr, long long var_Vs);

extern long long builtvx_incgth3(long long var_Vk, long long var_Dh_h, long long var_Dh_l, long long var_Vs, long var_Rn);
extern long long builtvx_incgth3clr(long long var_Dh_h, long long var_Dh_l, long long var_Vs, long var_Rn);
extern long long builtvx_maxh3(long long var_Dh_h, long long var_Dh_l, long long var_Vs);
extern long long builtvx_fir3(long long var_Dh_h, long long var_Dh_l, long long var_Vs);
extern long long builtvx_select(long long var_Vk, long long var_Vr, long long var_Vs);
extern long long builtvx_absdhm1(long long var_Vr, long long var_Vs);
extern long long builtvx_absdhp1(long long var_Vr, long long var_Vs);
extern long long builtvx_addu8clp63(long long var_Vr, long var_Rn);
extern long long builtvx_sub128(long long var_Vr);
extern INT128 builtvx_addvu8u8(long long var_Vr,  long long var_Vs);
extern INT128 builtvx_addu16m1(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
extern INT128 builtvx_addu16m2(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l);
extern long long builtvx_incinsu16(long long var_Vk, long long var_Dh_h, long long var_Dh_l, long var_Rn, long var_Rp);

extern long long builtvx_cmpeqru8(long long var_Vr, long var_Rn);
extern long long builtvx_cmpneru8(long long var_Vr, long var_Rn);
extern long long builtvx_cmpltru8(long long var_Vr, long var_Rn);
extern long long builtvx_cmpleru8(long long var_Vr, long var_Rn);
extern long long builtvx_cmpgtru8(long long var_Vr, long var_Rn);
extern long long builtvx_cmpgeru8(long long var_Vr, long var_Rn);

extern long long builtvx_shr7s16s8rc(long long var_Dh_h, long long var_Dh_l);
extern long long builtvx_shr7s16s8rs(long long var_Dh_h, long long var_Dh_l);

extern long long builtvx_shrrs16s8(long long var_Dh_h, long long var_Dh_l, long var_Rn);
extern long long builtvx_shrrs16s8r(long long var_Dh_h, long long var_Dh_l, long var_Rn);

extern long long builtvx_shrs16s8(long long var_Dh_h, long long var_Dh_l, unsigned char imm_u4);
extern long long builtvx_shrs16s8r(long long var_Dh_h, long long var_Dh_l, unsigned char imm_u4);

extern long long builtvx_adds16shr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4);
extern long long builtvx_adds16shrr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4);

extern long long builtvx_subs16shr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4);
extern long long builtvx_subs16shrr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4);

extern INT128 builtvx_maru8u8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn);
extern INT128 builtvx_maru8u8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn);

extern INT128 builtvx_maru8s8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn);
extern INT128 builtvx_maru8s8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn);

extern INT128 builtvx_mavu8u8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);
extern INT128 builtvx_mavu8u8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);

extern INT128 builtvx_mavu8s8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);
extern INT128 builtvx_mavu8s8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);

extern INT128 builtvx_maviu8u8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);
extern INT128 builtvx_maviu8u8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);

extern INT128 builtvx_maviu8s8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);
extern INT128 builtvx_maviu8s8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs);

#endif
