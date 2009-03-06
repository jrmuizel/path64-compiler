/*******************/
/* vx_tnr_ins3.c   */
/*******************/
/* to be included in model_vx.c */


/******************* Vx: ADB *********************/
/* Increment if great with 3 pixels around       */
/* Gx? INCGTH3 Vk, Dh, Vs, Rn                    */
/*************************************************/

/*
// Increment if greater than (by testing 3 pixels placed horizontally around)
//   Vho= 8 current pixels; Vhe= 8 previous pixels;
//   Vs= 8 new pixels; all in uint8.
//   Rn.B[0] = uint8 (kTh or threshold).
//   as inputs: Vk = 8 uint8 counters (previous values).
// Outputs:
//   Vk = 8 uint8 counters. (mandatory >= uint6).
// Vk.B[j]++ if a pixel > Rn.B[0];
// This is for all three pixels: Vho.B[j] and  two close horizontal neighbors.
//
Gx? INCGTH3 Vk, Dh, Vs, Rn  // t=1
*/

long long builtvx_incgth3(long long var_Vk, long long var_Dh_h, long long var_Dh_l, long long var_Vs, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8a, tmpU8b,tmpU8c;

	uint8 tmpU8tab[3*8];

	GPVR tmpVk, tmpVs;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVk.LL = var_Vk;
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVs.LL = var_Vs;

	GPVR_Write(k, tmpVk);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? INCGTH3 Vk, Dh, Vs, Rn                    */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVk = GPVR_Read(k);
	tmpVhe = GPVR_Read((2*h));
	tmpVho = GPVR_Read((2*h+1));
	tmpVs = GPVR_Read(s);

	tmpU8a = BF_range(x_opa, 7, 0);    /* threshold */


	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVs.B[j];
	}

	/* Increment if greater than */

	if (x_guard[0]) {

	  for (j=0; j<8; j++) {

	    tmpU8c = tmpVk.B[j]; /* counter */

	    tmpU8b = tmpU8tab[j+8]; /* position 0 */
	    if (tmpU8b > tmpU8a)
	      tmpU8c++;
	    tmpU8b = tmpU8tab[j+8+1]; /* position +1 */
	    if (tmpU8b > tmpU8a)
	      tmpU8c++;
	    tmpU8b = tmpU8tab[j+8-1]; /* position -1 */
	    if (tmpU8b > tmpU8a)
	      tmpU8c++;

	    tmpVk.B[j] = tmpU8c;
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/******************* Vx: ADB *********************/
/* Increment if great with 3 pixels around       */
/* Gx? INCGTH3CLR Vk, Dh, Vs, Rn                 */
/*************************************************/

/*
// Increment if greater than (by testing 3 pixels placed horizontally around)
//   Vho= 8 current pixels; Vhe= 8 previous pixels;
//   Vs= 8 new pixels; all in uint8.
//   Rn.B[0] = uint8 (kTh or threshold).
// Outputs:
//   Vk = 8 uint8 counters. (mandatory >= uint6).
// Vk.B[j]++ if a pixel > Rn.B[0];
// This is for all three pixels: Vho.B[j] and  two close horizontal neighbors.
//
Gx? INCGTH3CLR Vk, Dh, Vs, Rn  // t=1, Vk initilaised as 0.
*/

long long builtvx_incgth3clr(long long var_Dh_h, long long var_Dh_l, long long var_Vs, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8a, tmpU8b,tmpU8c;

	uint8 tmpU8tab[3*8];

	GPVR tmpVk, tmpVs;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVs.LL = var_Vs;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? INCGTH3CLR Vk, Dh, Vs, Rn                 */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVhe = GPVR_Read((2*h));
	tmpVho = GPVR_Read((2*h+1));
	tmpVs = GPVR_Read(s);

	tmpU8a = BF_range(x_opa, 7, 0);    /* threshold */


	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVs.B[j];
	}

	/* Increment if greater than */

	if (x_guard[0]) {

	  for (j=0; j<8; j++) {

	    tmpU8c = 0; /* counter */

	    tmpU8b = tmpU8tab[j+8]; /* position 0 */
	    if (tmpU8b > tmpU8a)
	      tmpU8c++;
	    tmpU8b = tmpU8tab[j+8+1]; /* position +1 */
	    if (tmpU8b > tmpU8a)
	      tmpU8c++;
	    tmpU8b = tmpU8tab[j+8-1]; /* position -1 */
	    if (tmpU8b > tmpU8a)
	      tmpU8c++;

	    tmpVk.B[j] = tmpU8c;
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/******************* Vx: ADB *********************/
/* MAX of 3 pixels placed horizontally           */
/* Gx? MAXH3 Vk, Dh, Vs                          */
/*************************************************/

/*
// MAX of 3 pixels placed horizontally
// Input data:
//   Vho= 8 current pixels; Vhe= 8 previous pixels;
//   Vs= 8 new pixels; all in uint8.
// Outputs:
//   Vk= 8 max;
Gx? MAXH3 Vk, Dh, Vs  // t>=1/2
// Note: MAX5 = MAX3H, MAX, MAX, three instructions
// M = max(max(max(A1,C1),max(B0,B2)),B1);
*/

long long builtvx_maxh3(long long var_Dh_h, long long var_Dh_l, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8;

	uint8 tmpU8tab[3*8];

	GPVR tmpVk, tmpVs;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVs.LL = var_Vs;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? MAXH3 Vk, Dh, Vs                          */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVhe = GPVR_Read((2*h));
	tmpVho = GPVR_Read((2*h+1));
	tmpVs = GPVR_Read(s);

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVs.B[j];
	}

	/* MAXH3 */

	if (x_guard[0]) {

	  for (j=0; j<8; j++) {

	    tmpU8 = M_MAX(tmpU8tab[j+8+1], M_MAX(tmpU8tab[j+8], tmpU8tab[j+8-1]));

	    tmpVk.B[j] = tmpU8;
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/******************* Vx: ADB *********************/
/* 3-tap FIR (1/4, 1/2, 1/4)                     */
/* Gx? FIR3 Vk, Dh, Vs                           */
/*************************************************/

/*
// 3-tap FIR
// FIR3 (1/4, 1/2, 1/4) applied on 8 current pixels.
// (No rounding)
//   Vho= 8 current pixels; Vhe= 8 previous pixels;
//   Vs= 8 new pixels; all in uint8.
// Outputs:
//   Vk = 8 filtered pixels. all in uint8.
// M = (B0+2*B1+B2)/4;
Gx? FIR3 Vk, Dh, Vs  // t=1
*/

long long builtvx_fir3(long long var_Dh_h, long long var_Dh_l, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8;
	uint9 tmpU9a, tmpU9b;
	uint10 tmpU10;

	uint8 tmpU8tab[3*8];

	GPVR tmpVk, tmpVs;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVs.LL = var_Vs;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? FIR3 Vk, Dh, Vs                           */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVhe = GPVR_Read((2*h));
	tmpVho = GPVR_Read((2*h+1));
	tmpVs = GPVR_Read(s);

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVs.B[j];
	}

	/* FIR3 */

	if (x_guard[0]) {

	  for (j=0; j<8; j++) {

	    tmpU9a = tmpU8tab[j+8-1] + tmpU8tab[j+8+1];
	    tmpU9b = tmpU8tab[j+8]<<1;
	    tmpU10 = tmpU9a + tmpU9b;
	    tmpU8 = tmpU10 >> 2;

	    tmpVk.B[j] = tmpU8;
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/**************** Vx: ADB ************************/
/* Selection                                     */
/* Gx? SELECT Vk, Vr, Vs                         */
/*************************************************/

/*
// Selection
// Vr: 8 uint8 values.
// Vs: 8 uint8 values.
// as inputs: bit 0 of Vk.B[j] = condition.
// Outputs:
//   Vk = 8 uint8 values.
// Vk.B[j] = (Vk.B[j]&0x1) ? Vr.B[j] : Vs.B[j];
Gx? SELECT Vk, Vr, Vs
 */

long long builtvx_select(long long var_Vk, long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	GPVR tmpVk, tmpVr, tmpVs;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVk.LL = var_Vk;
	tmpVr.LL = var_Vr;
	tmpVs.LL = var_Vs;

	GPVR_Write(k, tmpVk);
	GPVR_Write(r, tmpVr);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? SELECT Vk, Vr, Vs                         */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVk = GPVR_Read(k);
	tmpVr = GPVR_Read(r);
	tmpVs = GPVR_Read(s);

	if(x_guard[0]) {

	  /* SELECT */

	  for (j=0; j<8; j++) {
	    tmpVk.B[j] = (tmpVk.B[j]&0x1) ? tmpVr.B[j] : tmpVs.B[j];
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/**************** Vx: ADB ************************/
/* ABSDiff Horizontal, window=(-1,0)             */
/* Gx? ABSDHM1 Vk, Vr, Vs                        */
/*************************************************/

/*
// ABSDiff Horizontal, window=2=(-1,0)
//   Vr= 8 current pixels; Vs= 8 previous pixels;
// Outputs:
//   Vk = 8 uint8 ABSDIFF results.
// Absdiff(i,j) = abs(Yn(i,j) -Yn(i,j-1));
Gx? ABSDHM1 Vk, Vr, Vs  // t=1
 */

long long builtvx_absdhm1(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8tab[2*8];

	GPVR tmpVk, tmpVr, tmpVs;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;
	tmpVs.LL = var_Vs;

	GPVR_Write(r, tmpVr);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? ABSDHM1 Vk, Vr, Vs                        */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVr = GPVR_Read(r);
	tmpVs = GPVR_Read(s);

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVs.B[j];
	  tmpU8tab[j+8] = tmpVr.B[j];
	}


	if(x_guard[0]) {

	  /* ABSDIFF */

	  for (j=0; j<8; j++) {
	    tmpVk.B[j] = absdiffU8(tmpU8tab[j+8], tmpU8tab[j+8-1]);
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/**************** Vx: ADB ************************/
/* ABSDiff Horizontal, window=(0, +1)            */
/* Gx? ABSDHP1 Vk, Vr, Vs                        */
/*************************************************/

/*
// ABSDiff Horizontal, window=2=(0, +1)
//   Vr= 8 new pixels; Vs= 8 current pixels;
// Outputs:
//   Vk = 8 uint8 ABSDIFF results.
// Absdiff(i,j) = abs(Yn(i,j) -Yn(i,j+1));
Gx? ABSDHP1 Vk, Vr, Vs  // t=1
 */

long long builtvx_absdhp1(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8tab[2*8];

	GPVR tmpVk, tmpVr, tmpVs;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;
	tmpVs.LL = var_Vs;

	GPVR_Write(r, tmpVr);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? ABSDHP1 Vk, Vr, Vs                        */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVr = GPVR_Read(r);
	tmpVs = GPVR_Read(s);

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVs.B[j];
	  tmpU8tab[j+8] = tmpVr.B[j];
	}


	if(x_guard[0]) {

	  /* ABSDIFF */

	  for (j=0; j<8; j++) {
	    tmpVk.B[j] = absdiffU8(tmpU8tab[j], tmpU8tab[j+1]);
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/**************** Vx: ADB ************************/
/* Compare                                       */
/* Gx? CMP<cu>RU8 Vk, Vr, Rn                     */
/*************************************************/

/*
// Compare
// <cu> = EQ, NE, GE, LT, LE, GT.
// Vr: 8 uint8 values.
// Rn.B[0] = uint8 value (for comparison).
// Outputs:
//   Vk.B[j] = (Vr.B[j] <cs> Rn.B[0] ? 0xFF : 0x00 );
Gx? CMP<cu>RU8 Vk, Vr, Rn
// Note: GT is mandatory
 */

long long vx_cmp_cu_RU8(uint3 cu, long long var_Vr, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 CPU;
	uint8 tmpU8;

	GPVR tmpVk, tmpVr;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;

	GPVR_Write(r, tmpVr);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */
	CPU = cu;

	/* --------------------------------------------- */
	/* Gx? CMP<cu>RU8 Vk, Vr, Rn                     */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU8 = BF_range(x_opa, 7, 0);

	tmpVr = GPVR_Read(r);

	/* Compare */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmpVk.B[j] = cmpU8(CPU, tmpVr.B[j], tmpU8);
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);
	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);

}


long long builtvx_cmpeqru8(long long var_Vr, long var_Rn) {

  return(vx_cmp_cu_RU8(0, var_Vr, var_Rn));

}

long long builtvx_cmpneru8(long long var_Vr, long var_Rn) {

  return(vx_cmp_cu_RU8(1, var_Vr, var_Rn));

}

long long builtvx_cmpgeru8(long long var_Vr, long var_Rn) {

  return(vx_cmp_cu_RU8(2, var_Vr, var_Rn));

}

long long builtvx_cmpltru8(long long var_Vr, long var_Rn) {

  return(vx_cmp_cu_RU8(3, var_Vr, var_Rn));

}

long long builtvx_cmpleru8(long long var_Vr, long var_Rn) {

  return(vx_cmp_cu_RU8(4, var_Vr, var_Rn));

}

long long builtvx_cmpgtru8(long long var_Vr, long var_Rn) {

  return(vx_cmp_cu_RU8(5, var_Vr, var_Rn));

}


/**************** Vx: ADB ************************/
/* ADD and Clamping 0..63                        */
/* Gx? ADDU8CLP63 Vk, Vr, Rn                     */
/*************************************************/

/*
// ADD and Clamping 0..63
//   Vr = 8 uint8 values (M);
//   Rn.B[0] = sint8 (e.g.: 24 - kth, kth could be 32.)
// Outputs:
//   Vk.B[j] = CLP63(Vr.B[j] + Rn.B[0]);
Gx? ADDU8CLP63 Vk, Vr, Rn // t>=1/2
 */

long long builtvx_addu8clp63(long long var_Vr, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	sint8 tmp8a, tmp8b;

	GPVR tmpVk, tmpVr;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;

	GPVR_Write(r, tmpVr);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? ADDU8CLP63 Vk, Vr, Rn                     */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmp8a = BF_range(x_opa, 7, 0);

	tmpVr = GPVR_Read(r);

	/* Add and clamp */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmp8b = tmpVr.B[j] + tmp8a; /* if overflow wrap-around */
	    if(tmp8b > 63)  {
	      tmpVk.B[j] = 63;
	    } else {
	      if(tmp8b < 0) {
		tmpVk.B[j] = 0;
	      } else {
		tmpVk.B[j] = tmp8b;
              }
	    }
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);
	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);

}


/**************** Vx: ADB ************************/
/* SUB128                                        */
/* Gx? SUB128 Vk, Vr                             */
/*************************************************/

/*
// Vk.B[j], Vr.B[j] in uint8
// Vk.B[j] = 128 - Vr.B[j]
// T = 128 - S
// if over/underflow wrap-around
Gx? SUB128 Vk, Vr
 */

long long builtvx_sub128(long long var_Vr) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8;

	GPVR tmpVk, tmpVr;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;

	GPVR_Write(r, tmpVr);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? SUB128 Vk, Vr                             */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVr = GPVR_Read(r);

	/* SUB128 */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmpU8 = 128 - tmpVr.B[j];
	    tmpVk.B[j] = tmpU8;
	  }

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);
	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);

}


/**************** Vx: ADB ***********************/
/* Addition                                     */
/* Gx? ADDVU8U8 Df, Vr, Vs                      */
/************************************************/

/*
// Addition
//   Vr= 8 uint8 values
//   Vs= 8 uint8 values
// Outputs:
//   Df=8 uint16 results.
// Df.H[j] = Vr.B[j] + Vs.B[j];
Gx? ADDVU8U8 Df, Vr, Vs  // t=1
// for Absdiff(i,j) = abs(Yn(i,j) -Yn(i,j-1))+abs(Yn(i,j)-Yn(i,j+1))
*/

INT128 builtvx_addvu8u8(long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint9 tmpU9;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MAVU8U8 Df, Vr, Vs                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s);

	/* Add */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU9 = tmpVr.B[j] + tmpVs.B[j];
	    tmpVfe.HW[j] = tmpU9;

	    tmpU9 = tmpVr.B[j+4] + tmpVs.B[j+4];
	    tmpVfo.HW[j] = tmpU9;

	  }

          /* Write Back Results to Df */
	  GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */
	tmpVfo = GPVR_Read((2*f+1));
	tmpVfe = GPVR_Read((2*f));
	
	tmpDf.high = tmpVfo.LL;
	tmpDf.low = tmpVfe.LL;

	return (tmpDf);
}


/**************** Vx: ADB ************************/
/* Addition horizontal                           */
/* Gx? ADDU16M1 Df, Dh, Dq                       */
/*************************************************/

/*
// Addition horizontal, window=(-1,0) or (-2, 0)
//   Dh= 8 uint16 values (current Absdiff(i,j))
//   Dq= 8 uint16 values (previous Absdiff(i,j))
// Outputs:
//   Df=8 uint16 results.
//
// Df.H[j] = Dh.H[j] + Dh.H[j-1], if j-1<0, take Dq.H[8+j-1].
Gx? ADDU16M1 Df, Dh, Dq // Sad(i,j) = Absdiff(i,j)+Absdiff(i,j-1); t=1
 */

INT128 builtvx_addu16m1(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint16 tmpU16a, tmpU16b;

	uint16 tmpU16tab[2*8];

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;
	GPVR tmpVqe, tmpVqo;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVqe.LL = var_Dq_l;
	tmpVqo.LL = var_Dq_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write_Pair(q, tmpVqo, tmpVqe);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? ADDU16M1 Df, Dh, Dq                       */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	for(j=0; j<4; j++) {  /* delay line */
	  tmpU16tab[j] = tmpVqe.HW[j];
	  tmpU16tab[j+4] = tmpVqo.HW[j];
	  tmpU16tab[j+8] = tmpVhe.HW[j];
	  tmpU16tab[j+12] = tmpVho.HW[j];
	}

	/* Add */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16a = tmpU16tab[j+8];
	    tmpU16b = tmpU16tab[j+8-1];
	    tmpU16b = tmpU16a + tmpU16b;
	    tmpVfe.HW[j] = tmpU16b;

	    tmpU16a = tmpU16tab[j+4+8];
	    tmpU16b = tmpU16tab[j+4+8-1];
	    tmpU16b = tmpU16a + tmpU16b;
	    tmpVfo.HW[j] = tmpU16b;

	  }

          /* Write Back Results to Df */
	  GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */
	tmpVfo = GPVR_Read((2*f+1));
	tmpVfe = GPVR_Read((2*f));
	
	tmpDf.high = tmpVfo.LL;
	tmpDf.low = tmpVfe.LL;

	return (tmpDf);
}


/**************** Vx: ADB ************************/
/* Addition horizontal                           */
/* Gx? ADDU16M2 Df, Dh, Dq                       */
/*************************************************/

/*
// Addition horizontal, window=(-1,0) or (-2, 0)
//   Dh= 8 uint16 values (current Absdiff(i,j))
//   Dq= 8 uint16 values (previous Absdiff(i,j))
// Outputs:
//   Df=8 uint16 results.
//
// Df.H[j] += Dh.H[j-2], if j-2<0, take Dq.B[8+j-2].
Gx? ADDU16M2 Df, Dh, Dq // Sad(i,j) += Absdiff(i,j-2); t=1
 */

INT128 builtvx_addu16m2(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint16 tmpU16;

	uint16 tmpU16tab[2*8];

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;
	GPVR tmpVqe, tmpVqo;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVqe.LL = var_Dq_l;
	tmpVqo.LL = var_Dq_h;

	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write_Pair(q, tmpVqo, tmpVqe);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? ADDU16M2 Df, Dh, Dq                       */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfo = GPVR_Read((2*f+1));
	tmpVfe = GPVR_Read((2*f));
	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	for(j=0; j<4; j++) {  /* delay line */
	  tmpU16tab[j] = tmpVqe.HW[j];
	  tmpU16tab[j+4] = tmpVqo.HW[j];
	  tmpU16tab[j+8] = tmpVhe.HW[j];
	  tmpU16tab[j+12] = tmpVho.HW[j];
	}

	/* Add */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16 = tmpU16tab[j+8-2];
	    tmpVfe.HW[j] += tmpU16;

	    tmpU16 = tmpU16tab[j+4+8-2];
	    tmpVfo.HW[j] += tmpU16;

	  }

          /* Write Back Results to Df */
	  GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */
	tmpVfo = GPVR_Read((2*f+1));
	tmpVfe = GPVR_Read((2*f));
	
	tmpDf.high = tmpVfo.LL;
	tmpDf.low = tmpVfe.LL;

	return (tmpDf);
}


/******************* Vx: ADB *********************/
/* Increment if inside of [min, max[             */
/* Gx? INCINSU16 Vk, Dh, Rn, Rp                  */
/*************************************************/

/*
// Increment if inside of [min, max[
//   Dh = 8 uint16 SAD results (sadblock[][]).
//   Rn.H[0] = min, in uint16.
//   Rp.H[0] = max, in uint16.
//   as inputs: Vk.W[0] = previous value of counter
// Outputs:
//   Vk.W[0] = one 32-bit counter (uint32). (zero extended to 64-bit)
// if ((Dh.HW[j] >= min) && (Dh.HW[j] < max))
//    Vk.W[0] ++;
Gx? INCINSU16 Vk, Dh, Rn, Rp  // t>=1/2
 */

long long builtvx_incinsu16(long long var_Vk, long long var_Dh_h, long long var_Dh_l, long var_Rn, long var_Rp) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint16 tmpU16;
	uint16 tmpU16min, tmpU16max;

	uint32 tmpU32;

	GPVR tmpVk;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

        tmpVk.LL = var_Vk;

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

        GPVR_Write(k, tmpVk);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */
	x_opa = var_Rn;
	x_opb = var_Rp;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? INCINSU16 Vk, Dh, Rn, Rp                  */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVk = GPVR_Read(k);

	tmpVhe = GPVR_Read((2*h));
	tmpVho = GPVR_Read((2*h+1));

	tmpU16min = BF_range(x_opa, 15, 0);    /* min */
	tmpU16max = BF_range(x_opb, 15, 0);    /* max */

	tmpU32 = tmpVk.W[0]; /* counter */

	if (x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16 = tmpVhe.HW[j];
	    if ((tmpU16 >= tmpU16min) && (tmpU16 < tmpU16max))
	      tmpU32++;

	    tmpU16 = tmpVho.HW[j];
	    if ((tmpU16 >= tmpU16min) && (tmpU16 < tmpU16max))
	      tmpU32++;

	  }

	  tmpVk.W[0] = tmpU32;
	  tmpVk.W[1] = 0;

	  /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}
