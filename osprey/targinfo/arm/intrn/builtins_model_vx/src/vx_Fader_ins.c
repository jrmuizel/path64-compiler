/*******************/
/* vx_Fader_ins.c  */
/*******************/
/* to be included in model_vx.c */


/**************** Vx-DEI: ADB *******************/
/* ADD, SUBtract and Clamp, Multiply Fractional */
/* Gx? ASCMFR Df, Dh, Rn                   */
/************************************************/

/*
 * Computing 8 pairs of results:
 * Dh = 8 pairs of uint8 values (Motions, TI Y)
 * Rn = 8 uint4 coefficients (c/8, results to be shifted right by 3),
 * c=0..15.
 * Outputs: Df = 8 pairs of results.
 * 'R' for rounding.
 * For example: coefficients = (kcor+const), kcor=0..7.
 *   Mc=((kcor+const)*M + 0x4)>>3.
 *   Vfe: 8 clamp(ti_y+Mc).
 *   Vfo: 8 clamp(ti_y-Mc).
 *   Clamp to 0 or 255.
 */

INT128 builtvx_ascmfr(long long var_Dh_h, long long var_Dh_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;

	uint4 tmpU4tab[8];  /* coefficients */
	uint9 tmpU9a, tmpU9b;
	uint10 tmpU10;
	sint10 tmp10;

	INT128 tmpDf;
	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVfo, tmpVfe);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */
	round = 1;

	/* --------------------------------------------- */
	/* Gx? ASCMF<attr> Df, Dh, Rn                    */
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

	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));


	if (x_guard[0]) {

		/* Add, subtrsct and clamp, multiply fractional */

		for (j=0; j<8; j++) {

			tmpU9a = ZeroEx(9, 8, tmpVhe.B[j]);
			tmpU9b = mpshr3U9(round, tmpVho.B[j], tmpU4tab[j]);

			tmpU10 = addU10(tmpU9a, tmpU9b);
			tmp10 = subS10(tmpU9a, tmpU9b);

			tmpVfe.B[j] = clampU8(ZeroEx(11, 10, tmpU10));
			tmpVfo.B[j] = clampU8(SignEx(11, 10, tmp10));
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


/**************** Vx-DEI: ADB *******************/
/* ADD, SUBtract and Clamp, Multiply Fractional */
/* Gx? ASCMF Df, Dh, Rn                   */
/************************************************/

/*
 * Computing 8 pairs of results:
 * Dh = 8 pairs of uint8 values (Motions, TI Y)
 * Rn = 8 uint4 coefficients (c/8, results to be shifted right by 3),
 * c=0..15.
 * Outputs: Df = 8 pairs of results.
 * For example: coefficients = (kcor+const), kcor=0..7.
 *   Mc=((kcor+const)*M + 0x4)>>3.
 *   Vfe: 8 clamp(ti_y+Mc).
 *   Vfo: 8 clamp(ti_y-Mc).
 *   Clamp to 0 or 255.
 */

INT128 builtvx_ascmf(long long var_Dh_h, long long var_Dh_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint1 round;

	uint4 tmpU4tab[8];  /* coefficients */
	uint9 tmpU9a, tmpU9b;
	uint10 tmpU10;
	sint10 tmp10;

	INT128 tmpDf;
	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;

	GPVR_Write_Pair(h, tmpVfo, tmpVfe);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */
	round = 0;

	/* --------------------------------------------- */
	/* Gx? ASCMF<attr> Df, Dh, Rn                    */
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

	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));


	if (x_guard[0]) {

		/* Add, subtrsct and clamp, multiply fractional */

		for (j=0; j<8; j++) {

			tmpU9a = ZeroEx(9, 8, tmpVhe.B[j]);
			tmpU9b = mpshr3U9(round, tmpVho.B[j], tmpU4tab[j]);

			tmpU10 = addU10(tmpU9a, tmpU9b);
			tmp10 = subS10(tmpU9a, tmpU9b);

			tmpVfe.B[j] = clampU8(ZeroEx(11, 10, tmpU10));
			tmpVfo.B[j] = clampU8(SignEx(11, 10, tmp10));
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
/* Median of Three values                        */
/* Gx? MEDIAN Vk, Dh, Vs                         */
/*************************************************/

/*
 * Computing 8 medians:
 * Dh  = 8 pairs of uint8 values
 * Vs  = 8 uint8 values
 * Outputs: Vk = 8 uint8 results (medians)
 */

long long builtvx_median(long long var_Dh_h, long long var_Dh_l, long long var_Vs) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

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
	/* Gx? MEDIAN Vk, Dh, Vs                         */
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

		/* Median */
		for (j=0; j<8; j++) {
			tmpVk.B[j] = medianU8(tmpVho.B[j], tmpVhe.B[j], tmpVs.B[j]);
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
/* ABSolute and Difference, MultiPly, SeLecT     */
/* Gx? ABSDMPSLT Df, Dh, Dq, Rn                  */
/*************************************************/

/*
 * Computing 8 results:
 * Dh = 8 pairs of uint8 values (Motions, TI Y).
 * Dq = 8 pairs of uint8 values (dedails, SI Y).
 * Rn = 8 uint4 coefficients (0..15).
 * as inputs: Vfe = 8 uint8 values (Medians).
 * Outputs: Df = 8 pairs of results (Interpolated Y, SI Y).
 * For example: coefficients = (kcor+const), kcor=0..7.
 *   Mc=(kcor+const)*M, kcor=0..7.
 *   if((Mc > (d<<3)) || (Mc > (abs(si_y-ti_y)<<3)))
 *      I_y = si_y;
 *   else
 *      I_y = median;
 */

INT128 builtvx_absdmpslt(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l,
		 long long var_Dq_h, long long var_Dq_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint4 tmpU4tab[8];  /* coefficients */
	uint8 tmpU8;
	uint12 tmpU12a, tmpU12b, tmpU12c;

	INT128 tmpDf;
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

	/* Operands to the extension interface */
	x_opa = var_Rn;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? ABSDMPSLT Df, Dh, Dq, Rn                  */
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

	tmpVfe = GPVR_Read((2*f));
	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	if (x_guard[0]) {

		/* Absolute and Difference, Multiply, Select */

		for (j=0; j<8; j++) {

			tmpU12a = tmpU4tab[j]*tmpVho.B[j];

			tmpU12b = ZeroEx(12, 8, tmpVqo.B[j]) << 3;

			tmpU8 = absdiffU8(tmpVhe.B[j], tmpVqe.B[j]);
			tmpU12c = ZeroEx(12, 8, tmpU8) << 3;

			if((tmpU12a > tmpU12b) || (tmpU12a > tmpU12c)) {
				tmpVfo.B[j] = tmpVqe.B[j];
			} else {
				tmpVfo.B[j] = tmpVfe.B[j];
			}
			tmpVfe.B[j] = tmpVqe.B[j]; /* copy */
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
/* ABSolute and Difference, SeLecT               */
/* Gx? ABSDSLT Df, Dh, Vs                        */
/*************************************************/

/*
 * Computing 8 results:
 * Dh = 8 pairs of uint8 values (Interpolated Y, SI Y).
 * Vs = 8 uint8 values (TI Y).
 * as inputs: Df = 8 pairs of uint8 values (SI UV, TI UV).
 * Outputs: Df = 8 pairs of results (Interpolated Y, Interpolated UV).
 * if (abs(I_y - si_y) <= abs(I_y - ti_y))
 *   I_uv = si_uv;
 * else
 *   I_uv = ti_uv;
 */

INT128 builtvx_absdslt( long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, 
			long long var_Vs) {



	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8a, tmpU8b;

	INT128 tmpDf;
	GPVR tmpVfe, tmpVfo;
	GPVR tmpVhe, tmpVho;
	GPVR tmpVs;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	tmpVhe.LL = var_Dh_l;
	tmpVho.LL = var_Dh_h;
	tmpVs.LL = var_Vs;

	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? ABSDSLT Df, Dh, Vs                        */
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
	tmpVs = GPVR_Read(s);

	if (x_guard[0]) {

		/* Absolute and Difference, Select */

		for (j=0; j<4; j++) {

			tmpU8a = absdiffU8(tmpVho.B[2*j], tmpVhe.B[2*j]);
			tmpU8b = absdiffU8(tmpVho.B[2*j], tmpVs.B[2*j]);

			if(tmpU8a <= tmpU8b) {
				tmpVfe.B[2*j] = tmpVfo.B[2*j];
				tmpVfe.B[2*j+1] = tmpVfo.B[2*j+1];
			}
			tmpVfo.B[2*j] = tmpVho.B[2*j]; /* copy */
			tmpVfo.B[2*j+1] = tmpVho.B[2*j+1]; /* copy */
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
