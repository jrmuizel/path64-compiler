/*******************/
/* vx_DD_SI_ins1.c */
/*******************/
/* to be included in model_vx.c */


/**************** Vx-DEI: ADB ********************/
/* Sum of Absolute Difference                    */
/* Gx? SAD Df, Dh, Dq, Rn                        */
/*************************************************/

/*
// Sum of Absolute Difference
// Input data: (see also DLUPDATE)
//   (line above Y) Vho= 2 previous and 2 new pixels; Vhe= 8 current pixels;
//   (line below Y) Vqo= 2 previous and 2 new pixels; Vqe= 8 current pixels;
// Rn = one pair of uint4 weights used by SAD(dir),
//      weights symetry exploited: weightb[dir], weighta[dir]
//      one pair of weights for each possible direction.
// Rn[27:24] = dir (equal-bit, direction identifier)
// dir[2:0] is dir index: 0 1 2 3 4  5  6  7 (sign-bit = dir[2])
//             direction: 0 1 2 3 0 -1 -2 -3
// equal-bit = dir[3].
// Rn[28] = round (1 for 2's complement Rounding, 0 without rounding).
// Outputs: Vfo = 8 uint8 calculated SAD, Vfe = 8 dir.
Gx? SAD Df, Dh, Dq, Rn
// throughput >= 1/2, 1/4?
*/

INT128 builtvx_sad( long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 tmpU3dir; /* dir */
	uint1 round;

	uint4 tmpU4a, tmpU4b;  /* weights */
	uint9 tmpU9a, tmpU9b;

	uint8 tmpU8LA[8+4]; /* line above */
	uint8 tmpU8LB[8+4]; /* line below */
	uint8 tmpU8tab[8+4];

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

	/* Operands to the extension interface */
	x_opa = var_Rn;

	/* --------------------------------------------- */
	/* Gx? SAD Df, Dh, Dq, Rn                        */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* x_opa[31:0] = Rn[31:0]; */

	/* Operands Fetch */

	tmpU3dir = BF_range(x_opa, 26, 24); /* dir index */
	if(tmpU3dir == 4) {
		tmpU3dir = 0;
	}
	round = BF_range(x_opa, 28, 28);

	tmpU4a = BF_range(x_opa, 3, 0);  /* 1st weight */
	tmpU4b = BF_range(x_opa, 11, 8); /* 2nd weight */

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

	if(x_guard[0]) {

		/* SAD */

		if(tmpU3dir == 0) {  /* SAD(0) */
			for (j=1; j<11; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j]);
				tmpU9b = addU9(tmpU8tab[j-1], tmpU8tab[j+1]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 1) {  /* SAD(1) */
			for (j=0; j<11; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j+1], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = addU9(tmpU8tab[j-1], tmpU8tab[j]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j+1]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 2) {  /* SAD(2) */
			for (j=0; j<10; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j+2], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-1]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 3) {  /* SAD(3) */
			for (j=0; j<8; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j+4], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-2]);
				tmpU9b = tmpU9a;

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 5) {  /* SAD(-1) */
			for (j=0; j<11; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j+1]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = addU9(tmpU8tab[j-1], tmpU8tab[j]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j+1]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 6) {  /* SAD(-2) */
			for (j=0; j<10; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j+2]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-1]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 7) {  /* SAD(-3) */
			for (j=0; j<8; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j+4]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-2]);
				tmpU9b = tmpU9a;

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
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
/* Sum of Absolute Difference and MINimum        */
/* Gx? SADMIN Df, Dh, Dq, Rn                     */
/*************************************************/

/*
// Sum of Absolute Difference and MINimum
// Input data: (see also DLUPDATE)
//   (line above Y) Vho= 2 previous and 2 new pixels; Vhe= 8 current pixels;
//   (line below Y) Vqo= 2 previous and 2 new pixels; Vqe= 8 current pixels;
// Rn = one pair of uint4 weights used by SAD(dir),
//      weights symetry exploited: weightb[dir], weighta[dir].
//      one pair of weights for each possible direction.
// Rn[27:24] = dir (equal-bit, direction identifier)
// dir[2:0] or dir index: 0 1 2 3 4  5  6  7 (sign-bit = dir[2])
//             direction: 0 1 2 3 0 -1 -2 -3
// equal-bit = dir[3].
// as inputs, Df contains previously calculated SAD and dir.
// Outputs: Vfo = 8 uint8 calculated SAD, Vfe = 8 dir (+equal_bit if any).
// Computing 8 SAD and selecting 8 minimum SAD and dir.
// setting equal bit (dir[3]=1) if SAD equal.
Gx? SADMIN Df, Dh, Dq, Rn
// throughput >= 1/2, 1/4?
 * SAD=previous SAD; dir = previous dir;
 * if (new SAD == previous SAD) {dir = previous dir | 0x8; (set equal bit) }
 * if (new SAD < previous SAD) {SAD = new SAD; dir = new dir;}
 *
 * if dir[2:0]==0 (i.e. new SAD=SAD(0), new dir=0): specific processing
 *   if ((new SAD =< previous SAD) || ((previous dir)>>3&0x1))
 *     {SAD=new SAD; dir=new dir;}
 *   else {SAD=previous SAD; dir= previous dir;}
 */

INT128 builtvx_sadmin(long long var_Df_h, long long var_Df_l, long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 tmpU3dir; /* dir */
	uint1 round;

	uint4 tmpU4a, tmpU4b;  /* weights */
	uint9 tmpU9a, tmpU9b;

	uint8 tmpU8LA[8+4]; /* line above */
	uint8 tmpU8LB[8+4]; /* line below */
	uint8 tmpU8tab[8+4];

	uint8 tmpU8SAD[GPVR_SIZE], tmpU8dir[GPVR_SIZE];

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

	/* --------------------------------------------- */
	/* Gx? SADMIN Df, Dh, Dq, Rn                     */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* x_opa[31:0] = Rn[31:0]; */

	/* Operands Fetch */

	tmpU3dir = BF_range(x_opa, 26, 24); /* dir index */
	if(tmpU3dir == 4) {
		tmpU3dir = 0;
	}
	round = BF_range(x_opa, 28, 28);

	tmpU4a = BF_range(x_opa, 3, 0);  /* 1st weight */
	tmpU4b = BF_range(x_opa, 11, 8); /* 2nd weight */

	tmpVfo = GPVR_Read((2*f+1));
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

	/* previously calculated SAD and dir in respectively Vfo and Vfe */
	for (j=0; j<8; j++) {
		tmpU8SAD[j] = tmpVfo.B[j];
		tmpU8dir[j] = tmpVfe.B[j];
	}

	if(x_guard[0]) {

		/* SAD */

		if(tmpU3dir == 0) {  /* SAD(0) */
			for (j=1; j<11; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j]);
				tmpU9b = addU9(tmpU8tab[j-1], tmpU8tab[j+1]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 1) {  /* SAD(1) */
			for (j=0; j<11; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j+1], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = addU9(tmpU8tab[j-1], tmpU8tab[j]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j+1]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 2) {  /* SAD(2) */
			for (j=0; j<10; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j+2], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-1]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 3) {  /* SAD(3) */
			for (j=0; j<8; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j+4], tmpU8LB[j]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-2]);
				tmpU9b = tmpU9a;

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 5) {  /* SAD(-1) */
			for (j=0; j<11; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j+1]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = addU9(tmpU8tab[j-1], tmpU8tab[j]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j+1]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 6) {  /* SAD(-2) */
			for (j=0; j<10; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j+2]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-1]);
				tmpU9b = addU9(tmpU8tab[j-2], tmpU8tab[j]);

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		if(tmpU3dir == 7) {  /* SAD(-3) */
			for (j=0; j<8; j++) {
				tmpU8tab[j] = absdiffU8(tmpU8LA[j], tmpU8LB[j+4]);
			}
			for (j=2; j<10; j++) {
				tmpU9a = ZeroEx(9, 8, tmpU8tab[j-2]);
				tmpU9b = tmpU9a;

				tmpVfo.B[j-2] = mfaddU8(round, tmpU9a, tmpU4a, tmpU9b, tmpU4b); /* SAD(dir) */
				tmpVfe.B[j-2] = ZeroEx(8, 3, tmpU3dir); /* dir */
			}
		}

		/* Minimun and Equal-Bit Setting */

		if (tmpU3dir == 0) {  /* direction = 0 */
			for (j=0; j<8; j++) {
				if ((tmpVfo.B[j] > tmpU8SAD[j]) && (((tmpU8dir[j]>>3)&0x1) == 0)) {
					tmpVfo.B[j] = tmpU8SAD[j];
					tmpVfe.B[j] = tmpU8dir[j];
				}
			}
		} else {
			for (j=0; j<8; j++) {
				if (tmpVfo.B[j] == tmpU8SAD[j]) {
					tmpVfe.B[j] = tmpU8dir[j] | 0x8; /* set equal-bit */
				}
				if (tmpVfo.B[j] > tmpU8SAD[j]) {
					tmpVfo.B[j] = tmpU8SAD[j];
					tmpVfe.B[j] = tmpU8dir[j];
				}
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
/* MINimum SAD selection                         */
/* Gx? MINSAD Df, Dh, Dq                         */
/*************************************************/

/*
// MINimum Sum of Absolute Difference
// Dh = 8 SAD and dir.
// Dq = 8 SAD and dir.
// Outputs: Df = 8 selected minimum SAD with associated dir
//          (+equal_bit if any)
 */

INT128 builtvx_minsad(long long var_Dh_h, long long var_Dh_l, long long var_Dq_h, long long var_Dq_l) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint3 tmpU3dir; /* dir */

	uint8 tmpU8SAD_a[GPVR_SIZE], tmpU8dir_a[GPVR_SIZE];
	uint8 tmpU8SAD_b[GPVR_SIZE], tmpU8dir_b[GPVR_SIZE];

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

	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
	GPVR_Write_Pair(h, tmpVho, tmpVhe);
	GPVR_Write_Pair(q, tmpVqo, tmpVqe);

	/* Operands to X interface */

	/* Immediate data */

	/* --------------------------------------------- */
	/* Gx? MINSAD Df, Dh, Dq                         */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	tmpVho = GPVR_Read((2*h+1));
	tmpVhe = GPVR_Read((2*h));
	tmpVqo = GPVR_Read((2*q+1));
	tmpVqe = GPVR_Read((2*q));

	/* SAD and dir to be compared */
	for (j=0; j<8; j++) {
		tmpU8SAD_a[j] = tmpVho.B[j];
		tmpU8dir_a[j] = tmpVhe.B[j];
		tmpU8SAD_b[j] = tmpVqo.B[j];
		tmpU8dir_b[j] = tmpVqe.B[j];
	}

	if(x_guard[0]) {

		/* Minimum SAD selection */
		for (j=0; j<8; j++) {

			tmpU3dir = BF_range(tmpU8dir_a[j], 2, 0);

			if (tmpU3dir == 0) {  /* direction = 0 */
				if ((tmpU8SAD_a[j] <= tmpU8SAD_b[j]) || ((tmpU8dir_b[j]>>3)&0x1)) {
					tmpVfo.B[j] = tmpU8SAD_a[j];
					tmpVfe.B[j] = tmpU8dir_a[j];
				} else {
					tmpVfo.B[j] = tmpU8SAD_b[j];
					tmpVfe.B[j] = tmpU8dir_b[j];
				}
			} else {
				if (tmpU8SAD_a[j] <= tmpU8SAD_b[j]) {
					tmpVfo.B[j] = tmpU8SAD_a[j];
					tmpVfe.B[j] = tmpU8dir_a[j];
				} else {
					tmpVfo.B[j] = tmpU8SAD_b[j];
					tmpVfe.B[j] = tmpU8dir_b[j];
				}
			}
		}

		/* Write Back Results to Df */
		GPVR_Write_Pair(f, tmpVfo, tmpVfe);

	}

	/* -------------------------- cut here BOTTOM --- */
	/**************************************************/

	/* Results from GPVR */

	/* Results from GPVR */
	tmpVfo = GPVR_Read((2*f+1));
	tmpVfe = GPVR_Read((2*f));
	
	tmpDf.high = tmpVfo.LL;
	tmpDf.low = tmpVfe.LL;

	return (tmpDf);

}


