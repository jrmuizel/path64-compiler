/*******************/
/* vx_mv_format.c  */
/*******************/
/* to be included in model_vx.c */


/**************** Vx: ADB ***********************/
/* Move double bytes to GPVR                    */
/* Gx? M2XSHLDB Vk, Rn, Rp                      */
/************************************************/

/*
// Move to GPVR, double bytes, shift left 2x8 bits.
// Vk = (Vk<<16) | (Rn.B[0]<<8) | Rp.B[0].
M2XSHLDB Vk, Rn, Rp
*/

long long builtvx_m2xshldb(long long var_Vk, long var_Rn, long var_Rp) {

        /* local variables used in the instruction description */

	uint8 tmpU8a, tmpU8b;

        GPVR tmpVk;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVk.LL = var_Vk;
        GPVR_Write(k, tmpVk);

	/* Operands to the extension interface */
	x_opa = var_Rn;
	x_opb = var_Rp;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? M2XSHLDB Vk, Rn, Rp                       */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVk = GPVR_Read(k);

	tmpU8a = BF_range(x_opa, 7, 0);
	tmpU8b = BF_range(x_opb, 7, 0);

	/* Shift and Move */

	if(x_guard[0]) {

	  tmpVk.LL = tmpVk.LL << 16;
	  tmpVk.B[1] = tmpU8a;
	  tmpVk.B[0] = tmpU8b;

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
/* Interleave Bytes                              */
/* Gx? INTLVB Df, Vr, Vs                         */
/*************************************************/

/*
// Interleave Bytes
// Df = Byte_Interleave(Vr, Vs)
// Vr = odd bytes, Vs = even bytes.
INTLVB Df, Vr, Vs
 */

INT128 builtvx_intlvb(long long var_Vr, long long var_Vs) {

        INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

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
        /* Gx? INTLVB Df, Vr, Vs                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);  /* Odd bytes in Df */
        tmpVs = GPVR_Read(s);  /* Even bytes in Df */

	/* Interleave Bytes */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {
	    tmpVfe.B[2*j+1] = tmpVr.B[j];
	    tmpVfe.B[2*j] = tmpVs.B[j];
	  }

	  for (j=0; j<4; j++) {
	    tmpVfo.B[2*j+1] = tmpVr.B[j+4];
	    tmpVfo.B[2*j] = tmpVs.B[j+4];
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
/* Deinterleave Bytes                            */
/* Gx? DINTLVB Df, Vr, Vs                        */
/*************************************************/

/*
// Deinterleave Bytes
// Df = Byte_DeInterleave(Vr, Vs)
// Vr = 8 MSB, Vs = 8 LSB.
// Vfe = even bytes; Vfo = odd bytes;
DINTLVB Df, Vr, Vs
 */

INT128 builtvx_dintlvb(long long var_Vr, long long var_Vs) {

        INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

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
        /* Gx? DINTLVB Df, Vr, Vs                        */
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

	/* De-Interleave Bytes */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {
	    tmpVfe.B[j] = tmpVs.B[2*j];
	    tmpVfo.B[j] = tmpVs.B[2*j+1];
	  }

	  for (j=0; j<4; j++) {
	    tmpVfe.B[j+4] = tmpVr.B[2*j];
	    tmpVfo.B[j+4] = tmpVr.B[2*j+1];
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

