/*******************/
/* vx_tnr_ins1.c   */
/*******************/
/* to be included in model_vx.c */


/******************* Vx: ADB *********************/
/* ABSolute Difference an Maximum                */
/* Gx? ABSDMAX Vk, Dh, Vs                        */
/*************************************************/

/*
// ABSolute Difference and MAXimun
// Computing 8 times "2 absdiff and 1 max"
// Inputs:
//   Dh: 2x8 uint8 values.
//   Vs: 8 uint8 values to substrate.
// Outputs:
//   Vk: 8 uint8 values.
//   Vk = MAX(|Vho-Vs|, |Vhe-Vs|);
Gx? ABSDMAX Vk, Dh, Vs
// Note: similar as ABSDMIN
 */

long long builtvx_absdmax(long long var_Dh_h, long long var_Dh_l, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8a, tmpU8b;

	GPVR tmpVk, tmpVs;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVs.LL = var_Vs;

	GPVR_Write(k, tmpVk);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? ABSDMAX Vk, Dh, Vs                        */
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

	if (x_guard[0]) {

	  /* ABSDMAX */

	  for (j=0; j<8; j++) {

	    tmpU8a = absdiffU8(tmpVho.B[j], tmpVs.B[j]);
	    tmpU8b = absdiffU8(tmpVhe.B[j], tmpVs.B[j]);

	    if (tmpU8b > tmpU8a) {
	      tmpU8a = tmpU8b;
	    }

	    tmpVk.B[j] = tmpU8a;
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
/* MAXimun of a pair of values                   */
/* Gx? MAXPAIR Vk, Vr                            */
/*************************************************/

/*
// MAXimun of a pair of values (uint8)
// Vk.B[2*j+1]=Vk.B[2*j]=max(Vr.B[2*j+1], Vr.B[2*j]).
Gx? MAXPAIR Vk, Vr
// Note: similar as MAX, however horizontally.
 */

long long builtvx_maxpair(long long var_Vr) {

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
	/* Gx? MAXPAIR Vk, Vr                              */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVr = GPVR_Read(r);

	/* Maximum */

	if(x_guard[0]) {

	  for (j=0; j<4; j++) {
	    tmpU8 = M_MAX(tmpVr.B[2*j+1], tmpVr.B[2*j]);
	    tmpVk.B[2*j+1]=tmpU8;
	    tmpVk.B[2*j]=tmpU8;
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
/* Clamping (0..63)                              */
/* Gx? CLP63 Vk, Vr                              */
/*************************************************/

/*
// Clamping (0..63)
// Vr = 8 input uint8 values.
// Outputs:
// Vk = 8 output uint8 values (clamped).
// Vk.B[j] = CLP63(Vr.B[j]);
Gx? CLP63 Vk, Vr
//Note: could also use CLPU8
 */

long long builtvx_clp63(long long var_Vr) {

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
	/* Gx? CLP63 Vk, Vr                              */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVr = GPVR_Read(r);

	/* Clamping */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {

	    if(tmpVr.B[j] > 63)
	      tmpU8 = 63;
	    else
	      tmpU8 = tmpVr.B[j];

	    tmpVk.B[j]=tmpU8;

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


/*/**************** Vx: ADB **********************/
/* Clamping (0..255)                              */
/* Gx? CLPU8 Vk, Vr                              */
/*************************************************/

/*
// Clamping [0, limit], max limit = 255 (uint8)
// Vr = 8 uint8 values
// Rn.B[0] = limit;
// Outputs:
//   Vk = 8 uint8 results.
Gx? CLPU8 Vk, Vr, Rn
*/

long long builtvx_clpu8(long long var_Vr, long var_Rn) {

        /* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8a, tmpU8b;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */
	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? CLPU8 Vk, Vr, Rn                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	tmpU8a = BF_range(x_opa, 7, 0);    /* limit */

	/* Clamping */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {

	    if(tmpVr.B[j] > tmpU8a)
	      tmpU8b = tmpU8a;
	    else
	      tmpU8b = tmpVr.B[j];

	    tmpVk.B[j]=tmpU8b;

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
/* Clamping Symetric                             */
/* Gx? CLPSYM  Vk, Dh, Rn                        */
/*************************************************/

/*
// Clamping Symetric [-limit, +limit], max limit = 127 (uint7, converted to sint8)
// Dh = 8 sint16 values
// Rn.B[0] = limit (uint7, zero-extended to sint8);
// Outputs:
//   Vk = 8 sint8 results.
Gx? CLPSYM Vk, Dh, Rn
// if(avg_diff<-sigman) {
//   pn = -sigman;
// } else if (avg_diff>sigman) {
//   pn = sigman;
// } else {
//   pn = avg_diff;
// }
 */

long long builtvx_clpsym(long long var_Dh_h, long long var_Dh_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint7 tmpU7;
	sint8 tmp8a, tmp8b;
	sint16 tmp16;

	GPVR tmpVk;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? CLPSYM Vk, Dh, Rn                         */
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

	tmpU7 = BF_range(x_opa, 6, 0);    /* limit */

	tmp8a = tmpU7; /* limit */

	if (x_guard[0]) {

	  /* Clamping */

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVhe.HW[j];

	    if (tmp16 < -tmp8a) {
	      tmp8b = -tmp8a;
	    } else {
	      if (tmp16 > tmp8a) {
		tmp8b = tmp8a;
	      } else {
		tmp8b = tmp16;
	      }
	    }

	    tmpVk.B[j] = tmp8b;
	  }

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVho.HW[j];

	    if (tmp16 < -tmp8a) {
	      tmp8b = -tmp8a;
	    } else {
	      if (tmp16 > tmp8a) {
		tmp8b = tmp8a;
	      } else {
		tmp8b = tmp16;
	      }
	    }

	    tmpVk.B[j+4] = tmp8b;
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
/* FIR pixel pair                                */
/* Gx? FIRU8S8P Df, Dh, Dq, u3                   */
/*************************************************/

/*
// FIR pixel pair (one step)
// FIRU8S8P, results in 16-bit sint16
// FIR3 to FIR17.
// ppi=u3+1=1..8, index of pixel pair.
// e.g. for pixel j, the paire are pixel[j+u3+1] and pixel[j-(u3+1)].
// computing the (pixel[j+u3+1]+pixel[j-(u3+1)])*coeff[u3], and accumulate.
// Input data:
//   Vho= 8 current pixels in uint8; Vhe= 8 previous pixels in uint8;
//   Vqo= up to 8 coefficients (exploiting the symmetry), sint8.
//   Vqe= 8 new pixels in uint8;
//   as inputs: Df= 8 accumulations in sint16.
// Output data:
//   Df= 8 accumulations in sint16.
FIRU8S8P Df, Dh, Dq, u3  // t=1.
FIRU8S8P1CLR Df, Dh, Dq  // t=1, for u3=0, Df=0
// for position 0, using MARU8S8SHR7<attr>
 */

INT128 builtvx_firu8s8p(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u3) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint3 u3;

        uint3 tmpU3;

	sint8 tmp8;
	uint9 tmpU9;
	sint16 tmp16a, tmp16b;

	uint8 tmpU8tab[3*8];

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

        u3 = imm_u3;  /* u3+1:index of pixel pair */

        /* --------------------------------------------- */
        /* Gx? FIRU8S8P Df, Dh, Dq, u3                   */
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

        tmpU3 = BF_range(u3, 2, 0);  /* u3+1: index of pixel pair */

	tmp8 = tmpVqo.B[tmpU3]; /* coefficient */

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVqe.B[j];
	}

	/* one step of FIR */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {
	    tmpU9 = tmpU8tab[8+j-(tmpU3+1)] + tmpU8tab[8+j+(tmpU3+1)];
	    tmp16a = tmpU9*tmp8;
	    tmp16b = tmpVfe.HW[j];
	    tmp16b += tmp16a;
	    tmpVfe.HW[j] = tmp16b;
	  }

	  for (j=4; j<8; j++) {
	    tmpU9 = tmpU8tab[8+j-(tmpU3+1)] + tmpU8tab[8+j+(tmpU3+1)];
	    tmp16a = tmpU9*tmp8;
	    tmp16b = tmpVfo.HW[j-4];
	    tmp16b += tmp16a;
	    tmpVfo.HW[j-4] = tmp16b;
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
/* FIR pixel pair 1 with Df=0                    */
/* Gx? FIRU8S8P1CLR Df, Dh, Dq                   */
/*************************************************/

/*
FIRU8S8P1CLR Df, Dh, Dq  // u3=0, Df=0
// for position 0, using MARU8S8SHR7<attr>
 */

INT128 builtvx_firu8s8p1clr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint3 tmpU3;

	sint8 tmp8;
	uint9 tmpU9;
	sint16 tmp16;

	uint8 tmpU8tab[3*8];

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
        /* Gx? FIRU8S8P1CLR Df, Dh, Dq                   */
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

        tmpU3 = 0;  /* u3+1: index of pixel pair */

	tmp8 = tmpVqo.B[tmpU3]; /* coefficient */

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVqe.B[j];
	}

	/* one step of FIR */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {
	    tmpU9 = tmpU8tab[8+j-(tmpU3+1)] + tmpU8tab[8+j+(tmpU3+1)];
	    tmp16 = tmpU9*tmp8;
	    tmpVfe.HW[j] = tmp16;
	  }

	  for (j=4; j<8; j++) {
	    tmpU9 = tmpU8tab[8+j-(tmpU3+1)] + tmpU8tab[8+j+(tmpU3+1)];
	    tmp16 = tmpU9*tmp8;
	    tmpVfo.HW[j-4] = tmp16;
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
/* Byte shift right (16 bytes)                   */
/* Gx? BSHR Vk, Vr, Vs, u4                       */
/*************************************************/

/*
// Byte shift right (16 bytes)
Gx? BSHR Vk, Vr, Vs, u4 // ((Vr<<8*8)|Vs) >> (u4*8), keep 8 LSB of result, t=1.
*/

long long builtvx_bshr(long long var_Vr, long long var_Vs, unsigned char imm_u4) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint4 u4;

        uint4 tmpU4;

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

        /* Immediate data */

        u4 = imm_u4;  /* shift u4 bytes */

        /* --------------------------------------------- */
        /* Gx? BSHR Vk, Vr, Vs, u4                       */
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

        tmpU4 = BF_range(u4, 3, 0);  /* shift u4 bytes */

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVs.B[j];
	  tmpU8tab[j+8] = tmpVr.B[j];
	}

	/* Bytes shift right */

        if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    if ((j+tmpU4) < 16) {
	      tmpVk.B[j] = tmpU8tab[j+tmpU4];
	    } else {
	      tmpVk.B[j] = 0;
	    }
	  }

          /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/**************** Vx: ADB ************************/
/* Byte shift right (16 bytes)                   */
/* Gx? BSHRR Vk, Vr, Vs, Rn                      */
/*************************************************/

/*
// Byte shift right (16 bytes)
Gx? BSHRR Vk, Vr, Vs, Rn // Rn=u4, ((Vr<<8*8)|Vs) >> (u4*8), keep 8 LSB of result, t=1.
*/

long long builtvx_bshrr(long long var_Vr, long long var_Vs, long var_Rn) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint4 tmpU4;

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
	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? BSHRR Vk, Vr, Vs, Rn                      */
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

        tmpU4 = BF_range(x_opa, 3, 0);  /* shift u4 bytes */

	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVs.B[j];
	  tmpU8tab[j+8] = tmpVr.B[j];
	}

	/* Bytes shift right */

        if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    if ((j+tmpU4) < 16) {
	      tmpVk.B[j] = tmpU8tab[j+tmpU4];
	    } else {
	      tmpVk.B[j] = 0;
	    }
	  }

          /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);
}


/**************** Vx: ADB ************************/
/* shift right of 8 sint16 by 1                  */
/* Gx? SHR1S16 Df, Dh                            */
/*************************************************/

/*
// shift right of 8 sint16 by 1
Gx? SHR1S16 Df, Dh //  t=1.
 */

INT128 builtvx_shr1s16(long long var_Dh_h, long long var_Dh_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? SHR1S16 Df, Dh                            */
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

	/* shift right */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVhe.HW[j];
	    tmp16 >>= 1;
	    tmpVfe.HW[j] = tmp16;

	    tmp16 = tmpVho.HW[j];
	    tmp16 >>= 1;
	    tmpVfo.HW[j] = tmp16;

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
/* shift left of 8 sint16 by 1                   */
/* Gx? SHL1S16 Df, Dh                            */
/*************************************************/

/*
// shift left of 8 sint16 by 1
Gx? SHL1S16 Df, Dh //  t=1.
 */

INT128 builtvx_shl1s16(long long var_Dh_h, long long var_Dh_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? SHL1S16 Df, Dh                            */
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

	/* shift left */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVhe.HW[j];
	    tmp16 <<= 1;
	    tmpVfe.HW[j] = tmp16;

	    tmp16 = tmpVho.HW[j];
	    tmp16 <<= 1;
	    tmpVfo.HW[j] = tmp16;

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


/**************** Vx-DEI: ADB ********************/
/* shift right of 8 sint16                       */
/* Gx? SHRRS16S8<attr> Vk, Dh, Rn                 */
/*************************************************/

/*
// shift right of 8 sint16 by Rn=u4
Gx? SHRRS16S8<attr> Vk, Dh, Rn  //  t=1.
// <attr>=,R (2's complement)
 */

long long vx_shrrs16s8_attr(uint8 attr, long long var_Dh_h, long long var_Dh_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;

	uint4 tmpU4;
	sint8 tmp8;
	sint16 tmp16;

	GPVR tmpVk;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */

	x_opa = var_Rn;

	/* Immediate data and others */

	round = attr;

	/* --------------------------------------------- */
	/* Gx? SHRRS16S8<attr> Vk, Dh, Rn                */
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

	tmpU4 = BF_range(x_opa, 3, 0); /* shift by tmpU4 bits */

	/* shift right */

	if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVhe.HW[j];
	    tmp8 = shrs16s8r(round, tmp16, tmpU4); /* shift and round */
	    tmpVk.B[j] = tmp8;

	    tmp16 = tmpVho.HW[j];
	    tmp8 = shrs16s8r(round, tmp16, tmpU4); /* shift and round */
	    tmpVk.B[j+4] = tmp8;

	  }

	  /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);

}


long long builtvx_shrrs16s8(long long var_Dh_h, long long var_Dh_l, long var_Rn) {

  return(vx_shrrs16s8_attr(0, var_Dh_h, var_Dh_l, var_Rn));

}

long long builtvx_shrrs16s8r(long long var_Dh_h, long long var_Dh_l, long var_Rn) {

  return(vx_shrrs16s8_attr(1, var_Dh_h, var_Dh_l, var_Rn));

}

/**************** Vx-DEI: ADB ********************/
/* shift right of 8 sint16                       */
/* Gx? SHRS16S8<attr> Vk, Dh, u4                 */
/*************************************************/

/*
// shift right of 8 sint16 by u4
Gx? SHRS16S8<attr> Vk, Dh, u4  //  t=1.
// <attr>=,R (2's complement)
 */

long long vx_shrs16s8_attr(uint8 attr, long long var_Dh_h, long long var_Dh_l, unsigned char imm_u4) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;
	uint4 u4;

	uint4 tmpU4;
	sint8 tmp8;
	sint16 tmp16;

	GPVR tmpVk;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */

	/* Immediate data and others */

	round = attr;
	u4 = imm_u4;  /* shift by u4 bits */

	/* --------------------------------------------- */
	/* Gx? SHRS16S8<attr> Vk, Dh, u4                 */
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

	tmpU4 = BF_range(u4, 3, 0); /* shift by u4 bits */

	/* shift right */

	if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVhe.HW[j];
	    tmp8 = shrs16s8r(round, tmp16, tmpU4); /* shift and round */
	    tmpVk.B[j] = tmp8;

	    tmp16 = tmpVho.HW[j];
	    tmp8 = shrs16s8r(round, tmp16, tmpU4); /* shift and round */
	    tmpVk.B[j+4] = tmp8;

	  }

	  /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);

}

long long builtvx_shrs16s8(long long var_Dh_h, long long var_Dh_l, unsigned char imm_u4) {

  return(vx_shrs16s8_attr(0, var_Dh_h, var_Dh_l, imm_u4));

}

long long builtvx_shrs16s8r(long long var_Dh_h, long long var_Dh_l, unsigned char imm_u4) {

  return(vx_shrs16s8_attr(1, var_Dh_h, var_Dh_l, imm_u4));

}


/**************** Vx-DEI: ADB ********************/
/* shift right with convergent or symetric round */
/* Gx? SHR7S16S8<attr> Vk, Dh                    */
/*************************************************/

/*
// shift right of 8 sint16 by 7
Gx? SHR7S16S8<attr> Vk, Dh //  t=1.
// <attr>= RC (Convergent rounding), RS (symetric rounding),
 */

long long vx_shr7s16s8(uint8 attr, long long var_Dh_h, long long var_Dh_l) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;

	sint8 tmp8;
	sint16 tmp16;

	GPVR tmpVk;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */

	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */

	/* Immediate data and others */

	round = attr;

	/* --------------------------------------------- */
	/* Gx? SHR7S16S8<attr> Vk, Dh                    */
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

	/* shift right */

	if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVhe.HW[j];
	    tmp8 = shr7s16s8rcrs(round, tmp16); /* shift and round */
	    tmpVk.B[j] = tmp8;

	    tmp16 = tmpVho.HW[j];
	    tmp8 = shr7s16s8rcrs(round, tmp16); /* shift and round */
	    tmpVk.B[j+4] = tmp8;

	  }

	  /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */
	tmpVk = GPVR_Read(k);

	return(tmpVk.LL);

}

long long builtvx_shr7s16s8rc(long long var_Dh_h, long long var_Dh_l) {

  return(vx_shr7s16s8(0, var_Dh_h, var_Dh_l));

}

long long builtvx_shr7s16s8rs(long long var_Dh_h, long long var_Dh_l) {

  return(vx_shr7s16s8(1, var_Dh_h, var_Dh_l));

}

/**************** Vx: ADB ************************/
/* ADD 16-bit signed                             */
/* Gx? ADDS16   Df, Dh, Dq                       */
/*************************************************/

/*
// Input data:
//   Dh= 8 sint16;
//   Dq= 8 sint16;
// Outputs
//   Df = 8 sint16 results;
// Df.H[j] = Dh.H[j] + Dq.H[j].
Gx? ADDS16   Df, Dh, Dq // t>=1/2
// If overflow => wrap-around.
 */

INT128 builtvx_adds16(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16a, tmp16b;

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
        /* Gx? ADDS16 Df, Dh, Dq                         */
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

	/* Add */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVhe.HW[j];
	    tmp16b = tmpVqe.HW[j];
	    tmp16b = tmp16a + tmp16b;
	    tmpVfe.HW[j] = tmp16b;

	    tmp16a = tmpVho.HW[j];
	    tmp16b = tmpVqo.HW[j];
	    tmp16b = tmp16a + tmp16b;
	    tmpVfo.HW[j] = tmp16b;

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
/* SUB 16-bit signed                             */
/* Gx? SUBS16   Df, Dh, Dq                       */
/*************************************************/

/*
// Input data:
//   Dh= 8 sint16;
//   Dq= 8 sint16;
// Outputs
//   Df = 8 sint16 results;
// Df.H[j] = Dh.H[j] - Dq.H[j].
Gx? SUBS16   Df, Dh, Dq // t>=1/2
// If overflow => wrap-around.
 */

INT128 builtvx_subs16(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16a, tmp16b;

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
        /* Gx? SUBS16 Df, Dh, Dq                         */
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

	/* Sub */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVhe.HW[j];
	    tmp16b = tmpVqe.HW[j];
	    tmp16b = tmp16a - tmp16b;
	    tmpVfe.HW[j] = tmp16b;

	    tmp16a = tmpVho.HW[j];
	    tmp16b = tmpVqo.HW[j];
	    tmp16b = tmp16a - tmp16b;
	    tmpVfo.HW[j] = tmp16b;
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
/* ADD 16-bit signed and shift right             */
/* Gx? ADDS16SHR<attr> Vk, Dh, Dq, u4            */
/*************************************************/

/*
// sint16 addtion, converted to sint8.
// Input data:
//   Dh= 8 sint16;
//   Dq= 8 sint16;
// Outputs
//   Vk = 8 sint8 results;
// Vk.B[j] = (Dh.H[j] + Dq.H[j])>>u4, and rounding if needed.
Gx? ADDS16SHR<attr> Vk, Dh, Dq, u4 // t=1
// <attr>=,R (2's complement)
// If overflow => wrap-around.
 */

long long vx_adds16shr_attr(uint8 attr, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;
	uint4 u4;

	uint4 tmpU4;
	sint8 tmp8;
	sint16 tmp16a, tmp16b;

	GPVR tmpVk;
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

	round = attr;
	u4 = imm_u4;  /* shift by u4 bits */

        /* --------------------------------------------- */
        /* Gx? ADDS16SHR<attr> Vk, Dh, Dq, u4            */
        /* Execution Field in adb: */
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

	tmpU4 = BF_range(u4, 3, 0);  /* shift by u4 bits */

	/* Add and shift */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVhe.HW[j];
	    tmp16b = tmpVqe.HW[j];
	    tmp16b = tmp16a + tmp16b;
	    tmp8 = shrs16s8r(round, tmp16b, tmpU4); /* shift and round */
	    tmpVk.B[j] = tmp8;

	    tmp16a = tmpVho.HW[j];
	    tmp16b = tmpVqo.HW[j];
	    tmp16b = tmp16a + tmp16b;
	    tmp8 = shrs16s8r(round, tmp16b, tmpU4); /* shift and round */
	    tmpVk.B[j+4] = tmp8;
	  }

          /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */

        tmpVk = GPVR_Read(k);
        return(tmpVk.LL);
}

long long builtvx_adds16shr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4) {

  return(vx_adds16shr_attr(0, var_Dh_h, var_Dh_l, var_Dq_h, var_Dq_l, imm_u4));

}

long long builtvx_adds16shrr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4) {

  return(vx_adds16shr_attr(1, var_Dh_h, var_Dh_l, var_Dq_h, var_Dq_l, imm_u4));

}

/**************** Vx: ADB ************************/
/* SUB 16-bit signed and shift right             */
/* Gx? SUBS16SHR<attr> Vk, Dh, Dq, u4            */
/*************************************************/

/*
// sint16 addtion, converted to sint8.
// Input data:
//   Dh= 8 sint16;
//   Dq= 8 sint16;
// Outputs
//   Vk = 8 sint8 results;
// Vk.B[j] = (Dh.H[j] - Dq.H[j])>>7, and rounding if needed.
Gx? SUBS16SHR<attr> Vk, Dh, Dq, u4 // t=1
// <attr>=,R (2's complement)
// If overflow => wrap-around.
 */

long long vx_subs16shr_attr(uint8 attr, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;
	uint4 u4;

	uint4 tmpU4;
	sint8 tmp8;
	sint16 tmp16a, tmp16b;

	GPVR tmpVk;
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

	round = attr;
	u4 = imm_u4;  /* shift by u4 bits */

        /* --------------------------------------------- */
        /* Gx? SUBS16SHR7<attr> Vk, Dh, Dq               */
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

	tmpU4 = BF_range(u4, 3, 0);  /* shift by u4 bits */

	/* Sub and shift */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVhe.HW[j];
	    tmp16b = tmpVqe.HW[j];
	    tmp16b = tmp16a - tmp16b;
	    tmp8 = shrs16s8r(round, tmp16b, tmpU4); /* shift and round */
	    tmpVk.B[j] = tmp8;

	    tmp16a = tmpVho.HW[j];
	    tmp16b = tmpVqo.HW[j];
	    tmp16b = tmp16a - tmp16b;
	    tmp8 = shrs16s8r(round, tmp16b, tmpU4); /* shift and round */
	    tmpVk.B[j+4] = tmp8;
	  }

          /* Write Back Results to GPVR */
	  GPVR_Write(k, tmpVk);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */

        tmpVk = GPVR_Read(k);
        return(tmpVk.LL);
}

long long builtvx_subs16shr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4) {

  return(vx_subs16shr_attr(0, var_Dh_h, var_Dh_l, var_Dq_h, var_Dq_l, imm_u4));

}

long long builtvx_subs16shrr(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u4) {

  return(vx_subs16shr_attr(1, var_Dh_h, var_Dh_l, var_Dq_h, var_Dq_l, imm_u4));

}


/**************** Vx: ADB ************************/
/* Count delta Forward                           */
/* Gx? CNTDELTAFW Df, Dh, Dq, u2                 */
/*************************************************/

/*
// Count delta
//   position = +/-u2.
//   Vho= 8 current pixels (uint8); Vhe= 8 previous pixels (uint8);
//   Vqo= 8 mean (uint8);  Vqe= 8 new pixels (uint8);
//   as inputs: Vfo.B[0] = ulThImpulseNoise; Vfe= 8 counters.
// Output data:
//   Vfe= 8 updated counters (mandatory >= uint6).
// For each of the 8 central pixels of delayline:
// tmpU8delta = abs(delay_line.B[j+position]-mean); // delta
// if (tmpU8delta < ulThImpulseNoise.B[j]) counter.B[j]++;
Gx? CNTDELTAFW Df, Dh, Dq, u2 // t=1, forward, position = u2 = 0..3.
 */

INT128 builtvx_cntdeltafw(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u2) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint2 u2;

        uint2 tmpU2;

	uint8 tmpU8a, tmpU8b, tmpU8c;

	uint8 tmpU8tab[3*8];

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

        u2 = imm_u2;  /* position */

        /* --------------------------------------------- */
        /* Gx? CNTDELTAFW Df, Dh, Dq, u2                   */
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
	tmpVqo = GPVR_Read((2*q+1)); /* means */
	tmpVqe = GPVR_Read((2*q));

        tmpU2 = BF_range(u2, 1, 0);  /* forward position = +u2 */

	tmpU8a = tmpVfo.B[0];  /* ulThImpulseNoise */
 
	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVqe.B[j];
	}

	/* Count delta */

        if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmpU8b = absdiffU8(tmpU8tab[8+j+tmpU2], tmpVqo.B[j]); /* delta */
	    tmpU8c = tmpVfe.B[j]; /* counter */
	    if(tmpU8b < tmpU8a)
	      tmpU8c++;
	    tmpVfe.B[j] = tmpU8c;
	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe);
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
/* Count delta Backword                          */
/* Gx? CNTDELTABW Df, Dh, Dq, u2                 */
/*************************************************/

/*
// Count delta
//   position = +/-u2.
//   Vho= 8 current pixels (uint8); Vhe= 8 previous pixels (uint8);
//   Vqo= 8 mean (uint8);  Vqe= 8 new pixels (uint8);
//   as inputs: Vfo.B[0] = ulThImpulseNoise; Vfe= 8 counters.
// Output data:
//   Vfe= 8 updated counters (mandatory >= uint6).
// For each of the 8 central pixels of delayline:
// tmpU8delta = abs(delay_line.B[j+position]-mean); // delta
// if (tmpU8delta < ulThImpulseNoise.B[j]) counter.B[j]++;
Gx? CNTDELTABW Df, Dh, Dq, u2 // t=1, backward, position = -u2 = 0..-3.
 */

INT128 builtvx_cntdeltabw(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, unsigned char imm_u2) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint2 u2;

        uint2 tmpU2;

	uint8 tmpU8a, tmpU8b, tmpU8c;

	uint8 tmpU8tab[3*8];

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

        u2 = imm_u2;  /* position */

        /* --------------------------------------------- */
        /* Gx? CNTDELTABW Df, Dh, Dq, u2                 */
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
	tmpVqo = GPVR_Read((2*q+1)); /* means */
	tmpVqe = GPVR_Read((2*q));

        tmpU2 = BF_range(u2, 1, 0);  /* backward position = -u2 */

	tmpU8a = tmpVfo.B[0];  /* ulThImpulseNoise */
 
	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVqe.B[j];
	}

	/* Count delta */

        if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmpU8b = absdiffU8(tmpU8tab[8+j-tmpU2], tmpVqo.B[j]); /* delta */
	    tmpU8c = tmpVfe.B[j]; /* counter */
	    if(tmpU8b < tmpU8a)
	      tmpU8c++;
	    tmpVfe.B[j] = tmpU8c;
	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe);
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
/* Count delta (counter initialized as 0)        */
/* Gx? CNTDELTA0CLR Df, Dh, Dq                   */
/*************************************************/

/*
// Count delta
//   position = +/-u3.
//   Vho= 8 current pixels (uint8); Vhe= 8 previous pixels (uint8);
//   Vqo= 8 mean (uint8);  Vqe= 8 new pixels (uint8);
//   as inputs: Vfo.B[0] = ulThImpulseNoise; Vfe= 8 counters.
// Output data:
//   Vfe= 8 updated counters (mandatory >= uint6).
// For each of the 8 central pixels of delayline:
// tmpU8delta = abs(delay_line.B[j+position]-mean); // delta
// if (tmpU8delta < ulThImpulseNoise.B[j]) counter.B[j]++;
Gx? CNTDELTA0CLR Df, Dh, Dq // t=1, position=0, Vfe.B[j] initialized as 0;
 */

INT128 builtvx_cntdelta0clr(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint2 tmpU2;

	uint8 tmpU8a, tmpU8b, tmpU8c;

	uint8 tmpU8tab[3*8];

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
        /* Gx? CNTDELTA0CLR Df, Dh, Dq                   */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfo = GPVR_Read((2*f+1));

	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1)); /* means */
	tmpVqe = GPVR_Read((2*q));

        tmpU2 = 0;  /* position = 0 */

	tmpU8a = tmpVfo.B[0];  /* ulThImpulseNoise */
 
	for(j=0; j<8; j++) {  /* delay line */
	  tmpU8tab[j] = tmpVhe.B[j];
	  tmpU8tab[j+8] = tmpVho.B[j];
	  tmpU8tab[j+16] = tmpVqe.B[j];
	}

	/* Count delta */

        if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmpU8b = absdiffU8(tmpU8tab[8+j+tmpU2], tmpVqo.B[j]); /* delta */
	    tmpU8c = 0; /* counter initialized as 0 */
	    if(tmpU8b < tmpU8a)
	      tmpU8c++;
	    tmpVfe.B[j] = tmpU8c;
	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe);
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
