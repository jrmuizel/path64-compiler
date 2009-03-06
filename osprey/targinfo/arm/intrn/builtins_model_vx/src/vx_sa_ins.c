/*******************/
/* vx_sa_ins.c  */
/*******************/
/* to be included in model_vx.c */


/**************** Vx-SA: ADB ********************/
/* Accumulate                                   */
/* Gx? ACC Vk, Vr                               */
/************************************************/

/*
// Accumulate
// Accumulation of 8 unsigned bytes with a 16-bit unsigned integer
// Inputs:
//   Vr: 8 uint8 values.
//   as input: Vk.W[0] = previous value of the accumulator
// Outputs:
//    Vk.W[0]: one uint32 result.
// Vk.W[0] += sum (all 8 uint8 values in Vr); (zero extended to 64-bit)
// if overflow, wrap-around.
Gx? ACC Vk, Vr
*/

long long builtvx_acc(long long var_Vk, long long var_Vr) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint32 tmpU32;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVk.LL = var_Vk;
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);
        GPVR_Write(k, tmpVk);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? ACC Vk, Vr                              */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpVk = GPVR_Read(k);

	tmpU32 = tmpVk.W[0]; /* accumulator */

	/* Accumulate */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    tmpU32 += tmpVr.B[j];
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


/******************* Vx-SA: ADB ******************/
/* ABSolute Difference an MINimum                */
/* Gx? ABSDMIN Vk, Dh, Vs                        */
/*************************************************/

/*
// ABSolute Difference and MINimum
// Computing 8 times "2 absdiff and 1 min"
// Inputs:
//   Dh: 2x 8 uint8 values.
//   Vs: 8 uint8 values.
// Outputs:
//   Vk: 8 uint8 values.
//   Vk = MIN(|Vho-Vs|, |Vhe-Vs|);
Gx? ABSDMIN Vk, Dh, Vs
 */

long long builtvx_absdmin(long long var_Dh_h, long long var_Dh_l, long long var_Vs) {

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
	/* Gx? ABSDMIN Vk, Dh, Vs                        */
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

	  /* ABSDMIN */

	  for (j=0; j<8; j++) {

	    tmpU8a = absdiffU8(tmpVho.B[j], tmpVs.B[j]);
	    tmpU8b = absdiffU8(tmpVhe.B[j], tmpVs.B[j]);

	    if (tmpU8b < tmpU8a) {
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

/******************* Vx-SA: ADB ******************/
/* INCrement if GreaTer                          */
/* Gx? INCGT Vk, Vr, Rn                        */
/*************************************************/

/*
// INCrement if GreaTer
// Inputs:
//   Vr: 8 uint8 values.
//   Rn: 1 uint8 value.
//   as input Vk.W[0]: one uint32 counter.
// Output:
//   Vk.W[0]: 1 uint32 counter incremented.
//
// For each of the 8 uint8 values in Vr, if(Vr>Rn) Vk.W[0]++;
// (zero extended to 64-bit)
Gx? INCGT Vk, Vr, Rn

// INCrement if GreaTer
// Inputs:
//   Vr: 8 uint8 values.
//   Rn: 1 uint8 value.
//   as input Vk.HW0: one uint16 counter.
// Output:
//   Vk.HW0: 1 uint16 counter incremented.
//
// For each of the 8 uint8 values in Vr, if(Vr>Rn) Vk.HW0++; (zero extended to 64-bit)
Gx? INCGT Vk, Vr, Rn
*/

long long builtvx_incgt(long long var_Vk, long long var_Vr, long var_Rn) {

	/* local variables used in the instruction description */

	uint8 j;  /* byte indice */

	uint8 tmpU8;
	uint32 tmpU32;

	GPVR tmpVk, tmpVr;

	/* GPVR Register indices: k, r, s, f, h, q */
	GPVR_indices_init();

	/* Operands to GPVR */
        tmpVk.LL = var_Vk;
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);
        GPVR_Write(k, tmpVk);

	/* Operands to X interface */
	x_opa = var_Rn;

	/* Immediate data and others */

	/* --------------------------------------------- */
	/* Gx? INCGT Vk, Vr, Rn                          */
	/* Execution Field in adb:                       */
	/* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVr = GPVR_Read(r);
	tmpVk = GPVR_Read(k);

	tmpU8 = BF_range(x_opa, 7, 0);  /* threshold */
	tmpU32 = tmpVk.W[0];  /* counter */

	/* INCrement if GreaTer  */

	if(x_guard[0]) {

	  for (j=0; j<8; j++) {
	    if(tmpVr.B[j] > tmpU8) {
	      tmpU32++;
	    }
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
