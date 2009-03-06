/*******************/
/* vx_DD_SI_ins.c  */
/*******************/
/* to be included in model_vx.c */


/**************** Vx-DEI: ADB ********************/
/* MEAN for Y SI with fixed coefficients         */
/* Gx? MEANYR Df, Dh, Dq                    */
/*************************************************/

/*
// Mean for Y SI with fixed coefficients
//  'R' (Rounding)
// Input data: (see also DLUPDATE)
//   (line above Y) Vho= 2 previous and 2 new pixels; Vhe= 8 current pixels;
//   (line below Y) Vqo= 2 previous and 2 new pixels; Vqe= 8 current pixels;
// as inputs, Dfo not used, Dfe = 4 dir.
// Outputs: Dfo = 8 mean results for Y SI, Dfe unchanged.
 */

INT128 builtvx_meanyr(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l,
long long var_Dq_h, long long var_Dq_l) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 tmpU3dir; /* dir */
	uint1 round;

	uint8 tmpU8LA[8+4]; /* line above */
	uint8 tmpU8LB[8+4]; /* line below */

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

	/* Operands to X interface */

	/* Immediate data and others */
	round = 1;

	/* --------------------------------------------- */
	/* Gx? MEANY<attr> Df, Dh, Dq                    */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfe = GPVR_Read((2*f));
	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	/* previous pixels */
	tmpU8LA[0] = tmpVho.B[6];
	tmpU8LA[1] = tmpVho.B[7];
	tmpU8LB[0] = tmpVqo.B[6];
	tmpU8LB[1] = tmpVqo.B[7];

	/* current pixels */
	for (j=0; j<8; j++) {
		tmpU8LA[j+2] = tmpVhe.B[j];
		tmpU8LB[j+2] = tmpVqe.B[j];
	}

	/* new pixels */
	tmpU8LA[10] = tmpVho.B[0];
	tmpU8LA[11] = tmpVho.B[1];
	tmpU8LB[10] = tmpVqo.B[0];
	tmpU8LB[11] = tmpVqo.B[1];

	if (x_guard[0]) {

		/* Mean */

		for (j=0; j<8; j++) {

			tmpU3dir = BF_range(tmpVfe.B[j], 2, 0); /* dir index */

			if((tmpU3dir == 0) || (tmpU3dir == 4)) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2], tmpU8LB[j+2]);
			}

			if(tmpU3dir == 1) {
				tmpVfo.B[j] = mean4U8(round, tmpU8LA[j+2], tmpU8LB[j+2-1], tmpU8LA[j+2+1], tmpU8LB[j+2]);
			}

			if(tmpU3dir == 2) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2+1], tmpU8LB[j+2-1]);
			}

			if(tmpU3dir == 3) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2+2], tmpU8LB[j+2-2]);
			}

			if(tmpU3dir == 5) {
				tmpVfo.B[j] = mean4U8(round, tmpU8LA[j+2-1], tmpU8LB[j+2], tmpU8LA[j+2], tmpU8LB[j+2+1]);
			}

			if(tmpU3dir == 6) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2-1], tmpU8LB[j+2+1]);
			}

			if(tmpU3dir == 7) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2-2], tmpU8LB[j+2+2]);
			}
		}

		/* Write Back Results to Df */
		GPVR_Write((2*f+1), tmpVfo);

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
/* MEAN for Y SI with fixed coefficients         */
/* Gx? MEANY Df, Dh, Dq                    */
/*************************************************/

/*
// Mean for Y SI with fixed coefficients
// Input data: (see also DLUPDATE)
//   (line above Y) Vho= 2 previous and 2 new pixels; Vhe= 8 current pixels;
//   (line below Y) Vqo= 2 previous and 2 new pixels; Vqe= 8 current pixels;
// as inputs, Dfo not used, Dfe = 4 dir.
// Outputs: Dfo = 8 mean results for Y SI, Dfe unchanged.
 */

INT128 builtvx_meany(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l,
long long var_Dq_h, long long var_Dq_l) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 tmpU3dir; /* dir */
	uint1 round;

	uint8 tmpU8LA[8+4]; /* line above */
	uint8 tmpU8LB[8+4]; /* line below */

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

	/* Operands to X interface */

	/* Immediate data and others */
	round = 0;

	/* --------------------------------------------- */
	/* Gx? MEANY<attr> Df, Dh, Dq                    */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfe = GPVR_Read((2*f));
	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	/* previous pixels */
	tmpU8LA[0] = tmpVho.B[6];
	tmpU8LA[1] = tmpVho.B[7];
	tmpU8LB[0] = tmpVqo.B[6];
	tmpU8LB[1] = tmpVqo.B[7];

	/* current pixels */
	for (j=0; j<8; j++) {
		tmpU8LA[j+2] = tmpVhe.B[j];
		tmpU8LB[j+2] = tmpVqe.B[j];
	}

	/* new pixels */
	tmpU8LA[10] = tmpVho.B[0];
	tmpU8LA[11] = tmpVho.B[1];
	tmpU8LB[10] = tmpVqo.B[0];
	tmpU8LB[11] = tmpVqo.B[1];

	if (x_guard[0]) {

		/* Mean */

		for (j=0; j<8; j++) {

			tmpU3dir = BF_range(tmpVfe.B[j], 2, 0); /* dir index */

			if((tmpU3dir == 0) || (tmpU3dir == 4)) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2], tmpU8LB[j+2]);
			}

			if(tmpU3dir == 1) {
				tmpVfo.B[j] = mean4U8(round, tmpU8LA[j+2], tmpU8LB[j+2-1], tmpU8LA[j+2+1], tmpU8LB[j+2]);
			}

			if(tmpU3dir == 2) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2+1], tmpU8LB[j+2-1]);
			}

			if(tmpU3dir == 3) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2+2], tmpU8LB[j+2-2]);
			}

			if(tmpU3dir == 5) {
				tmpVfo.B[j] = mean4U8(round, tmpU8LA[j+2-1], tmpU8LB[j+2], tmpU8LA[j+2], tmpU8LB[j+2+1]);
			}

			if(tmpU3dir == 6) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2-1], tmpU8LB[j+2+1]);
			}

			if(tmpU3dir == 7) {
				tmpVfo.B[j] = mean2U8(round, tmpU8LA[j+2-2], tmpU8LB[j+2+2]);
			}
		}

		/* Write Back Results to Df */
		GPVR_Write((2*f+1), tmpVfo);

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
/* MEAN for UV SI with fixed coefficients        */
/* Gx? MEANUVR   Df, Dh, Dq                 */
/*************************************************/

/*
// Mean for UV SI with fixed coefficients
// 'R' (Rounding)
// Input data: (see also DLUPDATE)
//   (line above UV) Vho= 2 previous and 2 new pixels; Vhe= 8 current pixels;
//   (line below UV) Vqo= 2 previous and 2 new pixels; Vqe= 8 current pixels;
// as inputs, Dfo not used, Dfe = 4 dir.
// Outputs: Dfo = 4 mean results for U SI, 4 for V SI, Dfe unchanged.
 */

INT128 builtvx_meanuvr(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l,
		long long var_Dq_h, long long var_Dq_l) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint4 tmpU4a, tmpU4b;  /* coefficients */
	uint9 tmpU9a, tmpU9b;

	uint3 tmpU3dir; /* dir */
	uint1 round;

	uint8 tmpU8LA[8+4]; /* line above */
	uint8 tmpU8LB[8+4]; /* line below */

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

	/* Operands to X interface */

	/* Immediate data and others */
	round = 1;

	/* --------------------------------------------- */
	/* Gx? MEANUV<attr>  Df, Dh, Dq                  */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfe = GPVR_Read((2*f));
	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	/* previous pixels */
	tmpU8LA[0] = tmpVho.B[6];
	tmpU8LA[1] = tmpVho.B[7];
	tmpU8LB[0] = tmpVqo.B[6];
	tmpU8LB[1] = tmpVqo.B[7];

	/* current pixels */
	for (j=0; j<8; j++) {
		tmpU8LA[j+2] = tmpVhe.B[j];
		tmpU8LB[j+2] = tmpVqe.B[j];
	}

	/* new pixels */
	tmpU8LA[10] = tmpVho.B[0];
	tmpU8LA[11] = tmpVho.B[1];
	tmpU8LB[10] = tmpVqo.B[0];
	tmpU8LB[11] = tmpVqo.B[1];

	if (x_guard[0]) {

		/* Mean */

		tmpU4a = 6; /* 6/16 = 3/8 */
		tmpU4b = 2; /* 2/16 = 1/8 */

		for (j=0; j<4; j++) {

			tmpU3dir = BF_range(tmpVfe.B[2*j], 2, 0);

			if((tmpU3dir == 0) || (tmpU3dir == 4)) {
				tmpVfo.B[2*j] = mean2U8(round, tmpU8LA[2*j+2], tmpU8LB[2*j+2]);
				tmpVfo.B[2*j+1] = mean2U8(round, tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1]);
			}

			if(tmpU3dir == 1) {

				tmpU9a = addU9(tmpU8LA[2*j+2], tmpU8LB[2*j+2]);
				tmpU9b = addU9(tmpU8LA[2*j+2+2], tmpU8LB[2*j+2-2]);
				tmpVfo.B[2*j] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);

				tmpU9a = addU9(tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1]);
				tmpU9b = addU9(tmpU8LA[2*j+2+2+1], tmpU8LB[2*j+2-2+1]);
				tmpVfo.B[2*j+1] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);
			}

			if(tmpU3dir == 2) {
				tmpVfo.B[2*j] = mean4U8(round, tmpU8LA[2*j+2], tmpU8LB[2*j+2], tmpU8LA[2*j+2+2], tmpU8LB[2*j+2-2]);
				tmpVfo.B[2*j+1] = mean4U8(round, tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1], tmpU8LA[2*j+2+2+1],
				    tmpU8LB[2*j+2-2+1]);
			}

			if(tmpU3dir == 3) {
				tmpVfo.B[2*j] = mean2U8(round, tmpU8LA[2*j+2+2], tmpU8LB[2*j+2-2]);
				tmpVfo.B[2*j+1] = mean2U8(round, tmpU8LA[2*j+2+2+1], tmpU8LB[2*j+2-2+1]);
			}

			if(tmpU3dir == 5) {

				tmpU9a = addU9(tmpU8LA[2*j+2], tmpU8LB[2*j+2]);
				tmpU9b = addU9(tmpU8LA[2*j+2-2], tmpU8LB[2*j+2+2]);
				tmpVfo.B[2*j] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);

				tmpU9a = addU9(tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1]);
				tmpU9b = addU9(tmpU8LA[2*j+2-2+1], tmpU8LB[2*j+2+2+1]);
				tmpVfo.B[2*j+1] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);
			}

			if(tmpU3dir == 6) {
				tmpVfo.B[2*j] = mean4U8(round, tmpU8LA[2*j+2], tmpU8LB[2*j+2], tmpU8LA[2*j+2-2], tmpU8LB[2*j+2+2]);
				tmpVfo.B[2*j+1] = mean4U8(round, tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1], tmpU8LA[2*j+2-2+1],
				    tmpU8LB[2*j+2+2+1]);
			}

			if(tmpU3dir == 7) {
				tmpVfo.B[2*j] = mean2U8(round, tmpU8LA[2*j+2-2], tmpU8LB[2*j+2+2]);
				tmpVfo.B[2*j+1] = mean2U8(round, tmpU8LA[2*j+2-2+1], tmpU8LB[2*j+2+2+1]);
			}
		}

		/* Write Back Results to Df */
		GPVR_Write((2*f+1), tmpVfo);

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
/* MEAN for UV SI with fixed coefficients        */
/* Gx? MEANUV   Df, Dh, Dq                 */
/*************************************************/

/*
// Mean for UV SI with fixed coefficients
// Input data: (see also DLUPDATE)
//   (line above UV) Vho= 2 previous and 2 new pixels; Vhe= 8 current pixels;
//   (line below UV) Vqo= 2 previous and 2 new pixels; Vqe= 8 current pixels;
// as inputs, Dfo not used, Dfe = 4 dir.
// Outputs: Dfo = 4 mean results for U SI, 4 for V SI, Dfe unchanged.
 */

INT128 builtvx_meanuv(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l,
		long long var_Dq_h, long long var_Dq_l) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint4 tmpU4a, tmpU4b;  /* coefficients */
	uint9 tmpU9a, tmpU9b;

	uint3 tmpU3dir; /* dir */
	uint1 round;

	uint8 tmpU8LA[8+4]; /* line above */
	uint8 tmpU8LB[8+4]; /* line below */

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

	/* Operands to X interface */

	/* Immediate data and others */
	round = 0;

	/* --------------------------------------------- */
	/* Gx? MEANUV<attr>  Df, Dh, Dq                  */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfe = GPVR_Read((2*f));
	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	/* previous pixels */
	tmpU8LA[0] = tmpVho.B[6];
	tmpU8LA[1] = tmpVho.B[7];
	tmpU8LB[0] = tmpVqo.B[6];
	tmpU8LB[1] = tmpVqo.B[7];

	/* current pixels */
	for (j=0; j<8; j++) {
		tmpU8LA[j+2] = tmpVhe.B[j];
		tmpU8LB[j+2] = tmpVqe.B[j];
	}

	/* new pixels */
	tmpU8LA[10] = tmpVho.B[0];
	tmpU8LA[11] = tmpVho.B[1];
	tmpU8LB[10] = tmpVqo.B[0];
	tmpU8LB[11] = tmpVqo.B[1];

	if (x_guard[0]) {

		/* Mean */

		tmpU4a = 6; /* 6/16 = 3/8 */
		tmpU4b = 2; /* 2/16 = 1/8 */

		for (j=0; j<4; j++) {

			tmpU3dir = BF_range(tmpVfe.B[2*j], 2, 0);

			if((tmpU3dir == 0) || (tmpU3dir == 4)) {
				tmpVfo.B[2*j] = mean2U8(round, tmpU8LA[2*j+2], tmpU8LB[2*j+2]);
				tmpVfo.B[2*j+1] = mean2U8(round, tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1]);
			}

			if(tmpU3dir == 1) {

				tmpU9a = addU9(tmpU8LA[2*j+2], tmpU8LB[2*j+2]);
				tmpU9b = addU9(tmpU8LA[2*j+2+2], tmpU8LB[2*j+2-2]);
				tmpVfo.B[2*j] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);

				tmpU9a = addU9(tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1]);
				tmpU9b = addU9(tmpU8LA[2*j+2+2+1], tmpU8LB[2*j+2-2+1]);
				tmpVfo.B[2*j+1] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);
			}

			if(tmpU3dir == 2) {
				tmpVfo.B[2*j] = mean4U8(round, tmpU8LA[2*j+2], tmpU8LB[2*j+2], tmpU8LA[2*j+2+2], tmpU8LB[2*j+2-2]);
				tmpVfo.B[2*j+1] = mean4U8(round, tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1], tmpU8LA[2*j+2+2+1],
				    tmpU8LB[2*j+2-2+1]);
			}

			if(tmpU3dir == 3) {
				tmpVfo.B[2*j] = mean2U8(round, tmpU8LA[2*j+2+2], tmpU8LB[2*j+2-2]);
				tmpVfo.B[2*j+1] = mean2U8(round, tmpU8LA[2*j+2+2+1], tmpU8LB[2*j+2-2+1]);
			}

			if(tmpU3dir == 5) {

				tmpU9a = addU9(tmpU8LA[2*j+2], tmpU8LB[2*j+2]);
				tmpU9b = addU9(tmpU8LA[2*j+2-2], tmpU8LB[2*j+2+2]);
				tmpVfo.B[2*j] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);

				tmpU9a = addU9(tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1]);
				tmpU9b = addU9(tmpU8LA[2*j+2-2+1], tmpU8LB[2*j+2+2+1]);
				tmpVfo.B[2*j+1] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b);
			}

			if(tmpU3dir == 6) {
				tmpVfo.B[2*j] = mean4U8(round, tmpU8LA[2*j+2], tmpU8LB[2*j+2], tmpU8LA[2*j+2-2], tmpU8LB[2*j+2+2]);
				tmpVfo.B[2*j+1] = mean4U8(round, tmpU8LA[2*j+2+1], tmpU8LB[2*j+2+1], tmpU8LA[2*j+2-2+1],
				    tmpU8LB[2*j+2+2+1]);
			}

			if(tmpU3dir == 7) {
				tmpVfo.B[2*j] = mean2U8(round, tmpU8LA[2*j+2-2], tmpU8LB[2*j+2+2]);
				tmpVfo.B[2*j+1] = mean2U8(round, tmpU8LA[2*j+2-2+1], tmpU8LB[2*j+2+2+1]);
			}
		}

		/* Write Back Results to Df */
		GPVR_Write((2*f+1), tmpVfo);

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
/* INSide test and MEAN                          */
/* Gx? INSMEANR Df, Vr, Vs                  */
/*************************************************/

/*
// INSide test and MEAN
//  'R' (Rounding)
// Input data: (see also DLUPDATE)
//   (line above Y/UV) Vr= 8 current pixels;
//   (line below Y/UV) Vs= 8 current pixels;
// as inputs, Dfo = 8 pixel values, Dfe = 8 dir
// Outputs: Dfo = 8 calculated pixel values, Dfe = 8 updated dir
// if pixel value not between above and below
//   { new pixel value = (above + below)/2; new dir = 0}
// else pixel value and dir unchanged.
 */

INT128 builtvx_insmeanr(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

	uint8 j;  /* byte indice */

	uint1 round;

	INT128 tmpDf;
	GPVR tmpVfe, tmpVfo;
	GPVR tmpVr, tmpVs;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	tmpVr.LL = var_Vr;
	tmpVs.LL = var_Vs;

	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
	GPVR_Write(r, tmpVr);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */
	round = 1;

	/* --------------------------------------------- */
	/* Gx? INSMEAN<attr> Df, Vr, Vs                  */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfe = GPVR_Read((2*f));
	tmpVfo = GPVR_Read((2*f+1));
	tmpVr = GPVR_Read(r);
	tmpVs = GPVR_Read(s);

	if (x_guard[0]) {

		/* Inside test and mean */

		for (j=0; j<8; j++) {
			if (((tmpVfo.B[j] < tmpVr.B[j]) &&
			    (tmpVfo.B[j] < tmpVs.B[j])) ||
			    ((tmpVfo.B[j] > tmpVr.B[j]) &&
			    (tmpVfo.B[j] > tmpVs.B[j]))) {
				tmpVfo.B[j] = mean2U8(round, tmpVr.B[j], tmpVs.B[j]);
				tmpVfe.B[j] = 0; /* new direction = 0 */
			}
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
/* INSide test and MEAN                          */
/* Gx? INSMEAN Df, Vr, Vs                  */
/*************************************************/

/*
// INSide test and MEAN
// Input data: (see also DLUPDATE)
//   (line above Y/UV) Vr= 8 current pixels;
//   (line below Y/UV) Vs= 8 current pixels;
// as inputs, Dfo = 8 pixel values, Dfe = 8 dir
// Outputs: Dfo = 8 calculated pixel values, Dfe = 8 updated dir
// if pixel value not between above and below
//   { new pixel value = (above + below)/2; new dir = 0}
// else pixel value and dir unchanged.
 */

INT128 builtvx_insmean(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

	uint8 j;  /* byte indice */

	uint1 round;

	INT128 tmpDf;
	GPVR tmpVfe, tmpVfo;
	GPVR tmpVr, tmpVs;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	tmpVr.LL = var_Vr;
	tmpVs.LL = var_Vs;

	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
	GPVR_Write(r, tmpVr);
	GPVR_Write(s, tmpVs);

	/* Operands to X interface */

	/* Immediate data and others */
	round = 0;

	/* --------------------------------------------- */
	/* Gx? INSMEAN<attr> Df, Vr, Vs                  */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVfe = GPVR_Read((2*f));
	tmpVfo = GPVR_Read((2*f+1));
	tmpVr = GPVR_Read(r);
	tmpVs = GPVR_Read(s);

	if (x_guard[0]) {

		/* Inside test and mean */

		for (j=0; j<8; j++) {
			if (((tmpVfo.B[j] < tmpVr.B[j]) &&
			    (tmpVfo.B[j] < tmpVs.B[j])) ||
			    ((tmpVfo.B[j] > tmpVr.B[j]) &&
			    (tmpVfo.B[j] > tmpVs.B[j]))) {
				tmpVfo.B[j] = mean2U8(round, tmpVr.B[j], tmpVs.B[j]);
				tmpVfe.B[j] = 0; /* new direction = 0 */
			}
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
/* GET SAD(0) if new direction = 0               */
/* Gx? GETSAD0 Df, Dh, Dq                        */
/*************************************************/

/*
// GET SAD(0) if new direction = 0
// Vho = 8 SAD; Vhe = 8 SAD(0).
// Vqo = 8 pixels (SI Y); Vqe = 4 dir
// Outputs: Vfo = updated SAD (if dir = 0; new SAD = SAD(0));
//          Vfe = copy of Vqo
 */

INT128 builtvx_getsad0(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {
	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 tmpU3dir; /* dir */

	INT128 tmpDf;
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

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? GETSAD0 Df, Dh, Dq                        */
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

	if (x_guard[0]) {

		/* GET SAD(0) if new direction = 0 */

		for (j=0; j<8; j++) {

			tmpU3dir = BF_range(tmpVqe.B[j], 2, 0);

			if(tmpU3dir == 0) {
				tmpVfo.B[j] = tmpVhe.B[j]; /* SAD(0) */
			} else {
				tmpVfo.B[j] = tmpVho.B[j];
			}

			tmpVfe.B[j] = tmpVqo.B[j];
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
