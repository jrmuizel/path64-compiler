/*******************/
/* vx_MD_TI_ins.c  */
/*******************/
/* to be included in model_vx.c */


/**************** Vx-DEI: ADB ********************/
/* Absolute Difference                           */
/* Gx? ABSD Vk, Vr, Vs                           */
/*************************************************/

/*
// Absolute and Difference
// Vr = 8 1st input uint8 values.
// Vs = 8 2nd input uint8 values.
// Vk = 8 output uint8 values.
// 
// Vk = ABS(Vr - Vs), 8 values at time.
 */

long long builtvx_absd(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

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
	/* Gx? ABSD Vk, Vr, Vs                           */
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

	if(x_guard[0]) {

		/* ABSDIFF */

		for (j=0; j<8; j++) {
			tmpVk.B[j] = absdiffU8(tmpVr.B[j], tmpVs.B[j]);
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


/**************** Vx-DEI: ADB ********************/
/* Multiply Fractional Rounded              */
/* Gx? MFR Vk, Vr, Rn                       */
/*************************************************/

/*
 * Computing 8 4x8-bit fractional mutiplications
 * Vr = 8 uint8 values.
 * Rn = 8 uint4 coefficients (n/16, results to be shifted right by 4).
 * Outputs: Vk = 8 uint8 results.
 * 'R' for rounding.
 */

long long builtvx_mfr(long long var_Vr, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;
	uint4 tmpU4tab[8];  /* coefficients */

	GPVR tmpVk, tmpVr;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;

	GPVR_Write(r, tmpVr);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */
	round = 1;
	/* --------------------------------------------- */
	/* Gx? MF<attr> Vk, Vr, Rn                       */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	for (j=0; j<8; j++) {
		tmpU4tab[j] = BF_range(x_opa, (4*(j+1)-1), 4*j);
	}

	tmpVr = GPVR_Read(r);

	if(x_guard[0]) {

		/* Fractional Mutiplication */
		for (j=0; j<8; j++) {
			tmpVk.B[j] = mfU8(round, tmpVr.B[j], tmpU4tab[j]);
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


/**************** Vx-DEI: ADB ********************/
/* Multiply Fractional 		              */
/* Gx? MFR Vk, Vr, Rn                       */
/*************************************************/

/*
 * Computing 8 4x8-bit fractional mutiplications
 * Vr = 8 uint8 values.
 * Rn = 8 uint4 coefficients (n/16, results to be shifted right by 4).
 * Outputs: Vk = 8 uint8 results.
 */

long long builtvx_mf(long long var_Vr, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;
	uint4 tmpU4tab[8];  /* coefficients */

	GPVR tmpVk, tmpVr;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVr.LL = var_Vr;

	GPVR_Write(r, tmpVr);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */
	round = 0;
	/* --------------------------------------------- */
	/* Gx? MF<attr> Vk, Vr, Rn                       */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	for (j=0; j<8; j++) {
		tmpU4tab[j] = BF_range(x_opa, (4*(j+1)-1), 4*j);
	}

	tmpVr = GPVR_Read(r);

	if(x_guard[0]) {

		/* Fractional Mutiplication */
		for (j=0; j<8; j++) {
			tmpVk.B[j] = mfU8(round, tmpVr.B[j], tmpU4tab[j]);
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


/**************** Vx-DEI: ADB ********************/
/* MAXimum                                       */
/* Gx? MAX Vk, Vr, Vs                            */
/*************************************************/

/*
// Maximum
// Vr = 8 1st input uint8 values.
// Vs = 8 2nd input uint8 values.
// Vk = 8 output uint8 values.
//
// Vk = MAX(Vr,Vs), 8 values at time.
 */

long long builtvx_max(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

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
	/* Gx? MAX<wpack> Vk, Vr                         */
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

	if(x_guard[0]) {

		/* Maximum */
		for (j=0; j<8; j++) {
			tmpVk.B[j] = M_MAX(tmpVr.B[j], tmpVs.B[j]);
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


/**************** Vx-DEI: ADB ********************/
/* MINimum                                       */
/* Gx? MIN Vk, Vr, Vs                            */
/*************************************************/

/*
// Minimum
// Vr = 8 1st input uint8 values.
// Vs = 8 2nd input uint8 values.
// Vk = 8 output uint8 values.
//
// Vk = MIN(Vr,Vs), 8 values at time.
 */

long long builtvx_min(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

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
	/* Gx? MIN<wpack> Vk, Vr                         */
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

	if(x_guard[0]) {

		/* Minimum */
		for (j=0; j<8; j++) {
			tmpVk.B[j] = M_MIN(tmpVr.B[j], tmpVs.B[j]);
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


/**************** Vx-DEI: ADB ********************/
/* MEAN of 2 values rounded                      */
/* Gx? MEANR Vk, Vr, Vs                     */
/*************************************************/

/*
 * Computing 8 means (with fixed coefficients)
 * Vr = 8 uint8 values.
 * Vs = 8 uint8 values.
 * Outputs: Vk = 8 uint8 results.
 * 'R' for rounding.
 */

long long builtvx_meanr(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;

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
	round = 1;

	/* --------------------------------------------- */
	/* Gx? MEAN<attr><wpack> Vk, Vr                  */
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

	if(x_guard[0]) {

		/* Mean */

		for (j=0; j<8; j++) {
			tmpVk.B[j] = mean2U8(round, tmpVr.B[j], tmpVs.B[j]);
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

/**************** Vx-DEI: ADB ********************/
/* MEAN of 2 values                      */
/* Gx? MEAN Vk, Vr, Vs                     */
/*************************************************/

/*
 * Computing 8 means (with fixed coefficients)
 * Vr = 8 uint8 values.
 * Vs = 8 uint8 values.
 * Outputs: Vk = 8 uint8 results.
 */

long long builtvx_mean(long long var_Vr, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;

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
	round = 0;

	/* --------------------------------------------- */
	/* Gx? MEAN<attr><wpack> Vk, Vr                  */
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

	if(x_guard[0]) {

		/* Mean */

		for (j=0; j<8; j++) {
			tmpVk.B[j] = mean2U8(round, tmpVr.B[j], tmpVs.B[j]);
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
