/*******************/
/* vx_fgt_ins.c  */
/*******************/
/* to be included in model_vx.c */


/**************** Vx: ADB ***********************/
/* Scaling                                      */
/* Gx? SCALES8S9 Vk, Vr, Rn, Rp                 */
/************************************************/

/*
// Scaling
// Vr = 8 film-grain pixels (sint8)
// Rn = scale_factor (sint9, -256 excluded) (or sign-extended to 32-bit).
//      (No 16-bit overflow).
// Rp.B[0] = log2_scale_factor+6 (uint4) (or zero-extended to 32-bit).
// Outputs
// Vk = (Vr * Rn) >> Rp, 8 pixels at time, result in sint8.
SCALES8S9 Vk, Vr, Rn, Rp  // throughput=1.
// Note: using XP70 Core instructions for
//        "scale_factor = (BIT0(x(r, ec)) == 0) ?
//       scale_factor : -scale_factor;" computing.
*/

long long builtvx_scales8s9(long long var_Vr, long var_Rn, long var_Rp) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint4 tmpU4;
	sint8 tmp8;
	sint9 tmp9;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */
	x_opa = var_Rn;
	x_opb = var_Rp;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? SCALES8S9 Vk, Vr, Rn, Rp                  */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	tmp9 = SignEx(9, 9, BF_range(x_opa, 8, 0)); /* sint9 scale factor */

	tmpU4 = BF_range(x_opb, 3, 0);  /* shift length */

	/* SCALE */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmp8 = mps8s9_m256(tmpVr.B[j], tmp9) >> tmpU4;
	    tmpVk.B[j] = tmp8;
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
/* Scaling unsigned                             */
/* Gx? MPU8U8SHR Vk, Vr, Rn, Rp                 */
/************************************************/

/*
// Scaling unsigned
// Vr = 8 uint8 pixels
// Rn.B[0] = scale_factor (uint8) (or zero-extended to 32-bit).
// Rp.B[0] = log2_scale_factor+6 (uint4) (or zero-extended to 32-bit).
// Outputs
// Vk = (Vr * Rn) >> Rp, 8 pixels at time, result in uint8.
*/

long long builtvx_mpu8u8shr(long long var_Vr, long var_Rn, long var_Rp) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint4 tmpU4;
	uint8 tmpU8a, tmpU8b;
	uint16 tmpU16;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */
	x_opa = var_Rn;
	x_opb = var_Rp;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MPU8U8SHR Vk, Vr, Rn, Rp                  */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	tmpU8a = BF_range(x_opa, 7, 0);  /* usin8 scale factor */
	tmpU4 = BF_range(x_opb, 3, 0);  /* shift length */

	/* SCALE */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {

	    tmpU16 = tmpVr.B[j]*tmpU8a;
	    tmpU8b = tmpU16 >> tmpU4;

	    tmpVk.B[j] = tmpU8b;
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
/* 3-tap FIR on EDGE                             */
/* Gx? FIR3EDGE Df, Vr, Vs                       */
/*************************************************/

/*
// 3-tap FIR on EDGE
// FIR3 (1/4, 1/2, 1/4) applied on applied on the edge of two block.
// (No rounding)
// Vr = 8 sint8 pixels of current  fg_block.
// Vs = 8 sint8 pixels of previous fg_block.
// Outputs:
// Vfo = 8 sint8 pixels of current fg_block filtered on the left-side (LSB filtered)
// Vfe = 8 sint8 pixels of previous fg_block filtered on the right-side (MSB filtered)
//
// l1 = previous_fg_block[ 6 ]
// l0 = previous_fg_block[ 7 ]
// r0 = current_fg_block[ 0 ]
// r1 = current_fg_block[ 1 ]
// current_fg_block[ 0 ] = (l0 + (r0 << 1) + r1) >> 2
// previous_fg_block[ 7 ] = (l1 + (l0 << 1) + r0) >> 2
FIR3EDGE Df, Vr, Vs
 */

INT128 builtvx_fir3edge(long long var_Vr, long long var_Vs) {

        INT128 tmpDf;

        /* local variables used in the instruction description */

	sint8 tmp8;
	sint9 tmp9a, tmp9b;
	sint10 tmp10;

        GPVR tmpVfe, tmpVfo;
        GPVR tmpVr, tmpVs;

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
        /* Gx? FIR3EDGE Df, Vr, Vs                       */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s);

        if(x_guard[0]) {

	  tmpVfo = tmpVr;
	  tmpVfe = tmpVs;
          
	  tmp9a = ((sint8) tmpVs.B[7]) + ((sint8) tmpVr.B[1]);
	  tmp9b = ((sint8) tmpVr.B[0]) << 1;
	  tmp10 = tmp9a + tmp9b;
	  tmp8 = tmp10 >> 2;
	  tmpVfo.B[0] = tmp8;

	  tmp9a = ((sint8) tmpVs.B[6]) + ((sint8) tmpVr.B[0]);
	  tmp9b = ((sint8) tmpVs.B[7]) << 1;
	  tmp10 = tmp9a + tmp9b;
	  tmp8 = tmp10 >> 2;
	  tmpVfe.B[7] = tmp8;

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
/* Add and Shift Right by 6 bits                 */
/* Gx? ADDU16SHR6 Vk, Vr, Vs                     */
/*************************************************/

/*
// Add and Shift Right by 6 bits for 16-bit usigned intergers
// (No rounding)
// For 8x8 block average calculation from two half-block-sum
// Vr = 4 uint16 values (half-block-sum[0]);
// Vs = 4 uint16 values (half-block-sum[1]);
// Outputs:
// Vk.W0 = (Vr+Vs)>>6, 4 results at time => 4 uint8 results.
ADDU16SHR6 Vk, Vs, Vr
 */

long long builtvx_addu16shr6(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* half-word indice */

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
	/* Gx? ADDU16SHR6 Vk, Vr, Vs                     */
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

        /* ADDU16SHR6 */

	if(x_guard[0]) {

	  tmpVk.W[1] = 0;

	  for (j=0; j<4; j++) {
	    tmpVk.B[j] = (tmpVr.HW[j] + tmpVs.HW[j]) >> 6;
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


/**************** Vx: ADB ***********************/
/* RANDOM                                       */
/* Gx? RANDOM Vk, Vr                            */
/************************************************/

/*
// pseudo-random number generation
// Vr.W0 = Previous pseudo-random number = x(r-1, ec)
// Vr.W1 = Previous number of iterations (not used,
/  only for debugging)
// outputs:
// Vk.W0 = random(Vr.W0) = x(r, ec)
// Vk.W1 = Vr.W1 + 1;
RANDOM Vk, Vr // t=1
*/

long long builtvx_random(long long var_Vr) {

        /* local variables used in the instruction description */

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? RANDOM Vk, Vr                             */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	/* RANDOM */

	if(x_guard[0]) {

	  tmpVk.W[0] = random(tmpVr.W[0]);

	  tmpVk.W[1] = tmpVr.W[1] + 1;

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
/* MODAND                                       */
/* Gx? MODAND Vk, Vr                            */
/************************************************/

/*
// Vr.W0 = pseudo-random number = x(r, ec);
// outputs:
//   Vk.B[0] = (Vr.HW[1] % d0) & c0;
//   Vk.B[1] = (Vr.HW[0] % d1) & c1;
// Where: constant divisors (d0=52, d1=56) and
//        masks (c0=0xFC, c1=0xF8).
*/

long long builtvx_modand(long long var_Vr) {

        /* local variables used in the instruction description */

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MODAND Vk, Vr                             */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	/* MODAND */

	if(x_guard[0]) {

	  tmpVk.LL = 0;

	  tmpVk.B[0] = (tmpVr.HW[1] % 52) & 0xFC;
	  tmpVk.B[1] = (tmpVr.HW[0] % 56) & 0xF8;

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
/* Offset                                       */
/* Gx? OFFSET Vk, Vr, Rn                        */
/************************************************/

/*
// offset calculation based on pseudo-random number and the block position (m,n)
// Vr.B[0] = k_offset;
// Vr.B[1] = l_offset;
// Rn.HW[0] = m;
// Rn.Hw[1] = n;
// outputs:
// Vk = (l_offset + n&0x8)*64 + (k_offset + m&0x8); offset in uint16,
//         zero extended to 64-bit.
// 
OFFSET Vk, Vr, Rn
// Note: (m,n) updated with XP70 Core instructions
*/

long long builtvx_offset(long long var_Vr, long var_Rn) {

        /* local variables used in the instruction description */

	uint4 tmpU4m, tmpU4n;

	uint16 tmpU16;

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
        /* Gx? OFFSET Vk, Vr, Rn                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	tmpU4m = BF_range(x_opa, 3, 0);    /* m */
	tmpU4n = BF_range(x_opa, 19, 16);  /* n */

	/* OFFSET */

	if(x_guard[0]) {

	  tmpU16 = (tmpVr.B[1] + (tmpU4n&0x8))*64 + (tmpVr.B[0] + (tmpU4m&0x8));
	  tmpVk.LL = ZeroEx(64, 16, tmpU16);

          /* Write Back Results to Vk */
	  GPVR_Write(k, tmpVk);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */

        tmpVk = GPVR_Read(k);
        return(tmpVk.LL);

}
