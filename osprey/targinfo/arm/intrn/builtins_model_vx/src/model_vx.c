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

/*******************/
/* model_vx.c      */
/*******************/

#ifndef DA_MA_H
#define DA_MA_H
#include "../inc/Datatype.h"
#include "../inc/Macros.h"
#endif

#ifndef DA_MA_NEW_H
#define DA_MA_NEW_H
#include "../inc/Datatype_new.h"
#include "../inc/Macros_new.h"
#endif

#include "../inc/type_vx.h"

/*********** Bilbo Extension Interface ***********/

/* x_inst[31:0] = current_inst[31:0]; */
/* The 32 bits of the current instruction */
/* x_guard[0] = GR[x]; */
/* x_guard[1] = GR[x+8]; */
/* x_guard[2] = GR[x+16]; */
/* x_guard[3] = GR[x+24]; */

/* x_opa[31:0] = Rn[31:0]; */
/* x_opb[31:0] = Rp[31:0]; */

/* tmp32[31:0] = x_result[31:0]; */ /* Update Rm with x_result[31:0] */
/* GPR_Update(m, x, tmp32[31:24], tmp32[23:16], tmp32[15:8], tmp32[7:0]); */

uint8 x_guard[4];
uint32 x_opa, x_opb;
uint32 x_result;

/**************** GPVR *****************/
/* GPVR (General Purpose Vx registers) */

#define GPVR_NB 32   /* Number of GPVR registers */

GPVR VRF[GPVR_NB];

/* Vx registers: Vk, Vr, Vs */
#define Vk VRF[k]
#define Vr VRF[r]
#define Vs VRF[s]

/* GPVR register indices: k, r, s, f, h, q (for C models). */
#define GPVR_indices_init() const uint8 k=0, r=1, s=2, f=2, h=3, q=4;

/********* Macros *********/

#define M_MIN(x,y) ( ( x < y )? x : y)
#define M_MAX(x,y) ( ( x < y )? y : x)
#define M_MEDIAN(x,y,z) ( (( x <= y) == (y <= z) ) ? y : ( (x > y) ? M_MIN(x,z): M_MAX(x,z) ) )

/********* Initialization *********/
/* only for C models */

void set_x_guard(void) {
  x_guard[0]=0x1;
  x_guard[1]=0x1;
  x_guard[2]=0x1;
  x_guard[3]=0x1;
}

void init_vx(void) {
  set_x_guard();
}

/********* Functions/Procedures ***********/

#include "vx_func.c"

/********* Vx builtins C models *********/

#include "vx_DD_SI_ins.c"
#include "vx_Fader_ins.c"
#include "vx_MD_TI_ins.c"
#include "vx_SAD_ins.c"
#include "vx_fgt_ins.c"
#include "vx_move.c"
#include "vx_mv_format.c"
#include "vx_pixel_ordering.c"
#include "vx_sa_ins.c"
#include "vx_tnr_ins1.c"
#include "vx_tnr_ins2.c"
#include "vx_tnr_ins3.c"
