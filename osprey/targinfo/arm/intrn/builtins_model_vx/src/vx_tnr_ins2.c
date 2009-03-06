/*******************/
/* vx_tnr_ins2.c   */
/*******************/
/* to be included in model_vx.c */


/**************** Vx: ADB ***********************/
/* Multiply with one coefficient                */
/* Gx? MPRU8U8 Df, Vr, Rn                       */
/************************************************/

/*
// Multiply with one coefficient
//   Vr = 8 uint8 values;
//   Rn.B[0] = uint8 coefficient;
// Outputs:
//   Df = 8 uint16 results.
Gx? MPRU8U8 Df, Vr, Rn // Df.H[j] = Vr.B[j]*Rn.B[0];
*/

INT128 builtvx_mpru8u8(long long var_Vr, long var_Rn) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint8 tmpU8;
	uint16 tmpU16;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

        /* Operands to X interface */
	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MPRU8U8 Df, Vr, Rn                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	tmpU8 = BF_range(x_opa, 7, 0);    /* coefficient */

	/* Multiply */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16 = tmpVr.B[j]*tmpU8;
	    tmpVfe.HW[j] = tmpU16;

	    tmpU16 = tmpVr.B[j+4]*tmpU8;
	    tmpVfo.HW[j] = tmpU16;
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


/**************** Vx: ADB ***********************/
/* MAC with one coefficient                     */
/* Gx? MARU8U8 Df, Vr, Rn                       */
/************************************************/

/*
// MAC with one coefficient
//   Vr = 8 uint8 values;
//   Rn.B[0] = uint8 coefficient;
//   as inputs: Df = 8 uint16 values
// Outputs:
//   Df = 8 uint16 results.
Gx? MARU8U8 Df, Vr, Rn // Df.H[j] += Vr.B[j]*Rn.B[0];
*/

INT128 builtvx_maru8u8(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint8 tmpU8;
	uint16 tmpU16a, tmpU16b;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

        /* Operands to X interface */
	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MARU8U8 Df, Vr, Rn                        */
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
        tmpVr = GPVR_Read(r);

	tmpU8 = BF_range(x_opa, 7, 0);    /* coefficient */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16a = tmpVr.B[j]*tmpU8;
	    tmpU16b = tmpVfe.HW[j];
	    tmpU16b += tmpU16a;
	    tmpVfe.HW[j] = tmpU16b;

	    tmpU16a = tmpVr.B[j+4]*tmpU8;
	    tmpU16b = tmpVfo.HW[j];
	    tmpU16b += tmpU16a;
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


/**************** Vx: ADB ***********************/
/* MAC with one coefficient                     */
/* Gx? MARU8U8SHR7<attr> Df, Vr, Rn             */
/************************************************/

/*
// MAC with one coefficient
//   Vr = 8 uint8 values;
//   Rn.B[0] = uint8 coefficient;
//   as inputs: Df = 8 uint16 values
// Outputs:
//   Df = 8 uint16 results.
Gx? MARU8U8 Df, Vr, Rn // Df.HW[j] += Vr.B[j]*Rn.B[0];
Gx? MARU8U8SHR7<attr> Vk, Vr, Rn  // Dfe 8 uint8 results
// results right shifted by 7 with or without rounding (coefficients as n/128)
// if overflow, wrap-arounded. n could be > 128.
// <attr>=,R (2's complement, "(...+64)>>7")
*/

INT128 vx_maru8u8shr7_attr(uint8 attr, long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;

	uint8 tmpU8, tmpU8out;
	uint16 tmpU16a, tmpU16b;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVfe_out;
        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

        /* Operands to X interface */
	x_opa = var_Rn;

        /* Immediate data */

	round = attr;

        /* --------------------------------------------- */
        /* Gx? MARU8U8SHR7<attr> Df, Vr, R               */
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
        tmpVr = GPVR_Read(r);

	tmpU8 = BF_range(x_opa, 7, 0);    /* coefficient */

	/* MAC and shift */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16a = tmpVr.B[j]*tmpU8;
	    tmpU16b = tmpVfe.HW[j];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[j] = tmpU8out;

	    tmpU16a = tmpVr.B[j+4]*tmpU8;
	    tmpU16b = tmpVfo.HW[j];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[j+4] = tmpU8out;

	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe_out);
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


INT128 builtvx_maru8u8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

  return (vx_maru8u8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Rn));

}

INT128 builtvx_maru8u8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

  return (vx_maru8u8shr7_attr(1, var_Df_h, var_Df_l, var_Vr, var_Rn));

}


/**************** Vx: ADB ***********************/
/* Multiply with one coefficient                */
/* Gx? MPRU8S8 Df, Vr, Rn                       */
/************************************************/

/*
// Multiply with one coefficient
//   Vr = 8 uint8 values;
//   Rn.B[0] = sint8 coefficient;
// Outputs:
//   Df = 8 sint16 results.
Gx? MPRU8S8 Df, Vr, Rn // Df.H[j] = Vr.B[j]*Rn.B[0];
*/

INT128 builtvx_mpru8s8(long long var_Vr, long var_Rn) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint8 tmp8;
	sint16 tmp16;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

        /* Operands to X interface */
	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MPRU8S8 Df, Vr, Rn                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);

	tmp8 = BF_range(x_opa, 7, 0);    /* coefficient */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVr.B[j]*tmp8;
	    tmpVfe.HW[j] = tmp16;

	    tmp16 = tmpVr.B[j+4]*tmp8;
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



/**************** Vx: ADB ***********************/
/* MAC with one coefficient                     */
/* Gx? MARU8S8 Df, Vr, Rn                       */
/************************************************/

/*
// MAC with one coefficient
//   Vr = 8 uint8 values;
//   Rn.B[0] = sint8 coefficient;
//   as inputs: Df = 8 sint16 values
// Outputs:
//   Df = 8 sint16 results.
Gx? MARU8S8 Df, Vr, Rn // Df.H[j] += Vr.B[j]*Rn.B[0];
*/

INT128 builtvx_maru8s8(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint8 tmp8;
	sint16 tmp16a, tmp16b;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

        /* Operands to X interface */
	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MARU8S8 Df, Vr, Rn                        */
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
        tmpVr = GPVR_Read(r);

	tmp8 = BF_range(x_opa, 7, 0);    /* coefficient */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVr.B[j]*tmp8;
	    tmp16b = tmpVfe.HW[j];
	    tmp16b += tmp16a;
	    tmpVfe.HW[j] = tmp16b;

	    tmp16a = tmpVr.B[j+4]*tmp8;
	    tmp16b = tmpVfo.HW[j];
	    tmp16b += tmp16a;
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


/**************** Vx: ADB ***********************/
/* MAC with one coefficient                     */
/* Gx? MARU8S8SHR7<attr> Df, Vr, Rn             */
/************************************************/

/*
// MAC with one coefficient
//   Vr = 8 uint8 values;
//   Rn.B[0] = sint8 coefficient;
//   as inputs: Df = 8 sint16 values
// Outputs:
//   Dfe = 8 sint8 results.
Gx? MARU8S8 Df, Vr, Rn // Df.HW[j] += Vr.B[j]*Rn.B[0];
Gx? MARU8S8SHR7<attr> Vk, Vr, Rn  // Dfe 8 sint8 results
// results right shifted by 7 with or without rounding (coefficients as n/128)
// if overflow, wrap-arounded. n could be > 128.
// <attr>=,R (2's complement, "(...+64)>>7")
*/

INT128 vx_maru8s8shr7_attr(uint8 attr, long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;

	sint8 tmp8, tmp8out;
	sint16 tmp16a, tmp16b;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVfe_out;
        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

        /* Operands to X interface */
	x_opa = var_Rn;

        /* Immediate data */

	round = attr;

        /* --------------------------------------------- */
        /* Gx? MARU8S8SHR7<attr> Df, Vr, Rn              */
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
        tmpVr = GPVR_Read(r);

	tmp8 = BF_range(x_opa, 7, 0);    /* coefficient */

	/* MAC and shift */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVr.B[j]*tmp8;
	    tmp16b = tmpVfe.HW[j];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[j] = tmp8out;

	    tmp16a = tmpVr.B[j+4]*tmp8;
	    tmp16b = tmpVfo.HW[j];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[j+4] = tmp8out;

	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe_out);
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

INT128 builtvx_maru8s8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

  return(vx_maru8s8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Rn));

}

INT128 builtvx_maru8s8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long var_Rn) {

  return(vx_maru8s8shr7_attr(1, var_Df_h, var_Df_l, var_Vr, var_Rn));

}

/**************** Vx: ADB ***********************/
/* Multiply multiple unsigned coefficients      */
/* Gx? MPVU8U8 Df, Vr, Vs                       */
/************************************************/

/*
// Multiply with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 uint8 coefficients;
// Outputs:
//   Df = 8 uint16 results.
Gx? MPVU8U8 Df, Vr, Vs  // Df.H[j] = Vr.B[j]*Vs.B[j];
*/

INT128 builtvx_mpvu8u8(long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint16 tmpU16;

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
        /* Gx? MPVU8U8 Df, Vr, Vs                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* Multiply */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16 = tmpVr.B[j]*tmpVs.B[j];
	    tmpVfe.HW[j] = tmpU16;

	    tmpU16 = tmpVr.B[j+4]*tmpVs.B[j+4];
	    tmpVfo.HW[j] = tmpU16;
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


/**************** Vx: ADB ***********************/
/* MAC with multiple unsigned coefficients      */
/* Gx? MAVU8U8 Df, Vr, Vs                       */
/************************************************/

/*
// MAC with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 uint8 coefficients;
//   as inputs: Df = 8 uint16 values;
// Outputs:
//   Df = 8 uint16 results.
Gx? MAVU8U8 Df, Vr, Vs  // Df.H[j] += Vr.B[j]*Vs.B[j];
*/

INT128 builtvx_mavu8u8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint16 tmpU16a, tmpU16b;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

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

	tmpVfo = GPVR_Read((2*f+1));
	tmpVfe = GPVR_Read((2*f));
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16a = tmpVr.B[j]*tmpVs.B[j];
	    tmpU16b = tmpVfe.HW[j];
	    tmpU16b += tmpU16a;
	    tmpVfe.HW[j] = tmpU16b;

	    tmpU16a = tmpVr.B[j+4]*tmpVs.B[j+4];
	    tmpU16b = tmpVfo.HW[j];
	    tmpU16b += tmpU16a;
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


/**************** Vx: ADB ***********************/
/* MAC with multiple unsigned coefficients      */
/* Gx? MAVU8U8SHR7<attr> Df, Vr, Vs             */
/************************************************/

/*
// MAC with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 uint8 coefficients;
//   as inputs: Df = 8 uint16 values;
// Outputs:
//   Dfe = 8 uint8 results.
Gx? MAVU8U8 Df, Vr, Vs  // Df.H[j] += Vr.B[j]*Vs.B[j];
Gx? MAVU8U8SHR7<attr> Df, Vr, Vs // Dfe 8 uint8 results
// results right shifted by 7 with or without rounding (coefficients as n/128)
// if overflow, wrap-arounded. n could be > 128.
// <attr>=,R (2's complement, "(...+64)>>7")
*/

INT128 vx_mavu8u8shr7_attr(uint8 attr, long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;

	uint8 tmpU8out;
	uint16 tmpU16a, tmpU16b;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVfe_out;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

	round = attr;

        /* --------------------------------------------- */
        /* Gx? MAVU8U8SHR7<attr> Df, Vr, Vs              */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC and shift */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmpU16a = tmpVr.B[j]*tmpVs.B[j];
	    tmpU16b = tmpVfe.HW[j];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[j] = tmpU8out;

	    tmpU16a = tmpVr.B[j+4]*tmpVs.B[j+4];
	    tmpU16b = tmpVfo.HW[j];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[j+4] = tmpU8out;

	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe_out);
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

INT128 builtvx_mavu8u8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return(vx_mavu8u8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

INT128 builtvx_mavu8u8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return(vx_mavu8u8shr7_attr(1, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

/**************** Vx: ADB ***********************/
/* MAC interleave with multiple coefficients    */
/* Gx? MAVIU8U8 Df, Vr, Vs                      */
/************************************************/

/*
// MAC interleaved (UV) with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 uint8 coefficients; only 4 coefficients at even position are used.
//   as inputs: Df = 8 uint16 values;
// Outputs:
//   Df = 8 uint16 results.
Gx? MAVIU8U8 Df, Vr, Vs  // Df.H[2*j] += Vr.B[2*j]*Vs.B[2*j];
                         // Df.H[2*j+1] += Vr.B[2*j+1]*Vs.B[2*j];
*/

INT128 builtvx_maviu8u8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint16 tmpU16a, tmpU16b;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MAVIU8U8 Df, Vr, Vs                       */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<2; j++) {

	    tmpU16a = tmpVr.B[2*j]*tmpVs.B[2*j];
	    tmpU16b = tmpVfe.HW[2*j];
	    tmpU16b += tmpU16a;
	    tmpVfe.HW[2*j] = tmpU16b;

	    tmpU16a = tmpVr.B[2*j+1]*tmpVs.B[2*j]; /* same coefficient */
	    tmpU16b = tmpVfe.HW[2*j+1];
	    tmpU16b += tmpU16a;
	    tmpVfe.HW[2*j+1] = tmpU16b;

	    tmpU16a = tmpVr.B[2*j+4]*tmpVs.B[2*j+4];
	    tmpU16b = tmpVfo.HW[2*j];
	    tmpU16b += tmpU16a;
	    tmpVfo.HW[2*j] = tmpU16b;

	    tmpU16a = tmpVr.B[2*j+1+4]*tmpVs.B[2*j+4]; /* same coefficient */ 
	    tmpU16b = tmpVfo.HW[2*j+1];
	    tmpU16b += tmpU16a;
	    tmpVfo.HW[2*j+1] = tmpU16b;

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


/**************** Vx: ADB ***********************/
/* Multiply interleave multiple coefficients    */
/* Gx? MPVIU8U8 Df, Vr, Vs                      */
/************************************************/

/*
// MAC interleaved (UV) with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 uint8 coefficients; only 4 coefficients at even position are used.
// Outputs:
//   Df = 8 uint16 results.
Gx? MPVIU8U8 Df, Vr, Vs  // Df.H[2*j] = Vr.B[2*j]*Vs.B[2*j];
                         // Df.H[2*j+1] = Vr.B[2*j+1]*Vs.B[2*j];
*/

INT128 builtvx_mpviu8u8(long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint16 tmpU16;

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
        /* Gx? MPVU8U8 Df, Vr, Vs                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* Multiply */

        if(x_guard[0]) {

	  for (j=0; j<2; j++) {

	    tmpU16 = tmpVr.B[2*j]*tmpVs.B[2*j];
	    tmpVfe.HW[2*j] = tmpU16;

	    tmpU16 = tmpVr.B[2*j+1]*tmpVs.B[2*j]; /* same coefficient */
	    tmpVfe.HW[2*j+1] = tmpU16;



	    tmpU16 = tmpVr.B[2*j+4]*tmpVs.B[2*j+4];
	    tmpVfo.HW[2*j] = tmpU16;

	    tmpU16 = tmpVr.B[2*j+1+4]*tmpVs.B[2*j+4]; /* same coefficient */
	    tmpVfo.HW[2*j+1] = tmpU16;

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


/**************** Vx: ADB ***********************/
/* MAC interleave with multiple coefficients    */
/* Gx? MAVIU8U8SHR7<attr> Df, Vr, Vs            */
/************************************************/

/*
// MAC interleaved (UV) with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 uint8 coefficients; only 4 coefficients at even position are used.
//   as inputs: Df = 8 uint16 values;
// Outputs:
//   Df = 8 uint8 results.
Gx? MAVIU8U8 Df, Vr, Vs  // Df.H[2*j] += Vr.B[2*j]*Vs.B[2*j];
                         // Df.H[2*j+1] += Vr.B[2*j+1]*Vs.B[2*j];
Gx? MAVIU8U8SHR7<attr> Df, Vr, Vs  // Dfe 8 uint8 results
// results right shifted by 7 with or without rounding (coefficients as n/128)
// if overflow, wrap-arounded. n could be > 128.
// <attr>=,R (2's complement, "(...+64)>>7")
*/

INT128 vx_maviu8u8shr7_attr(uint8 attr, long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;

	uint8 tmpU8out;
	uint16 tmpU16a, tmpU16b;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVfe_out;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

	round = attr;

        /* --------------------------------------------- */
        /* Gx? MAVIU8U8SHR7<attr> Df, Vr, Vs             */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC and shift */

        if(x_guard[0]) {

	  for (j=0; j<2; j++) {

	    tmpU16a = tmpVr.B[2*j]*tmpVs.B[2*j];
	    tmpU16b = tmpVfe.HW[2*j];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j] = tmpU8out;

	    tmpU16a = tmpVr.B[2*j+1]*tmpVs.B[2*j]; /* same coefficient */
	    tmpU16b = tmpVfe.HW[2*j+1];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j+1] = tmpU8out;


	    tmpU16a = tmpVr.B[2*j+4]*tmpVs.B[2*j+4];
	    tmpU16b = tmpVfo.HW[2*j];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j+4] = tmpU8out;

	    tmpU16a = tmpVr.B[2*j+1+4]*tmpVs.B[2*j+4]; /* same coefficient */ 
	    tmpU16b = tmpVfo.HW[2*j+1];
	    tmpU16b += tmpU16a;
	    tmpU8out = shru16u8r(round, tmpU16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j+1+4] = tmpU8out;

	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe_out);
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

INT128 builtvx_maviu8u8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return (vx_maviu8u8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

INT128 builtvx_maviu8u8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return (vx_maviu8u8shr7_attr(1, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

/**************** Vx: ADB ***********************/
/* Multiply multiple signed coefficients        */
/* Gx? MPVU8S8 Df, Vr, Vs                       */
/************************************************/

/*
// Multiply with multiple signed coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 sint8 coefficients;
// Outputs:
//   Df = 8 sint16 results.
Gx? MPVU8S8 Df, Vr, Vs  // Df.H[j] = Vr.B[j]*Vs.B[j];
*/

INT128 builtvx_mpvu8s8(long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16;

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
        /* Gx? MPVU8S8 Df, Vr, Vs                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* Multiply */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16 = tmpVr.B[j]*((sint8) tmpVs.B[j]);
	    tmpVfe.HW[j] = tmp16;

	    tmp16 = tmpVr.B[j+4]*((sint8) tmpVs.B[j+4]);
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


/**************** Vx: ADB ***********************/
/* MAC with multiple signed coefficients      */
/* Gx? MAVU8S8 Df, Vr, Vs                       */
/************************************************/

/*
// MAC with multiple signed coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 sint8 coefficients;
//   as inputs: Df = 8 sint16 values;
// Outputs:
//   Df = 8 sint16 results.
Gx? MAVU8S8 Df, Vr, Vs  // Df.H[j] += Vr.B[j]*Vs.B[j];
*/

INT128 builtvx_mavu8s8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16a, tmp16b;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MAVU8S8 Df, Vr, Vs                        */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVr.B[j]*((sint8) tmpVs.B[j]);
	    tmp16b = tmpVfe.HW[j];
	    tmp16b += tmp16a;
	    tmpVfe.HW[j] = tmp16b;

	    tmp16a = tmpVr.B[j+4]*((sint8) tmpVs.B[j+4]);
	    tmp16b = tmpVfo.HW[j];
	    tmp16b += tmp16a;
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


/**************** Vx: ADB ***********************/
/* MAC with multiple signed coefficients        */
/* Gx? MAVU8S8SHR7<attr> Df, Vr, Vs             */
/************************************************/

/*
// MAC with multiple unsigned coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 sint8 coefficients;
//   as inputs: Df = 8 sint16 values;
// Outputs:
//   Dfe = 8 sint8 results.
Gx? MAVU8S8 Df, Vr, Vs  // Df.H[j] += Vr.B[j]*Vs.B[j];
Gx? MAVU8S8SHR7<attr> Df, Vr, Vs // Dfe 8 uint8 results
// results right shifted by 7 with or without rounding (coefficients as n/128)
// if overflow, wrap-arounded. n could be > 128.
// <attr>=,R (2's complement, "(...+64)>>7")
*/

INT128 vx_mavu8s8shr7_attr(uint8 attr, long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;

	sint8 tmp8out;
	sint16 tmp16a, tmp16b;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVfe_out;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

	round = attr;

        /* --------------------------------------------- */
        /* Gx? MAVU8S8SHR7<attr> Df, Vr, Vs              */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC and shift */

        if(x_guard[0]) {

	  for (j=0; j<4; j++) {

	    tmp16a = tmpVr.B[j]*((sint8) tmpVs.B[j]);
	    tmp16b = tmpVfe.HW[j];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[j] = tmp8out;

	    tmp16a = tmpVr.B[j+4]*((sint8) tmpVs.B[j+4]);
	    tmp16b = tmpVfo.HW[j];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[j+4] = tmp8out;

	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe_out);
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

INT128 builtvx_mavu8s8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return (vx_mavu8s8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

INT128 builtvx_mavu8s8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return (vx_mavu8s8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

/**************** Vx: ADB ***********************/
/* MAC interleave with multiple coefficients    */
/* Gx? MAVIU8S8 Df, Vr, Vs                      */
/************************************************/

/*
// MAC interleaved (UV) with multiple signed coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 sint8 coefficients; only 4 coefficients at even position are used.
//   as inputs: Df = 8 sint16 values;
// Outputs:
//   Df = 8 sint16 results.
Gx? MAVIU8S8 Df, Vr, Vs  // Df.H[2*j] += Vr.B[2*j]*Vs.B[2*j];
                         // Df.H[2*j+1] += Vr.B[2*j+1]*Vs.B[2*j];
*/

INT128 builtvx_maviu8s8(long long var_Df_h, long long var_Df_l, long long var_Vr,  long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16a, tmp16b;

	GPVR tmpVfe, tmpVfo;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? MAVIU8S8 Df, Vr, Vs                       */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC */

        if(x_guard[0]) {

	  for (j=0; j<2; j++) {

	    tmp16a = tmpVr.B[2*j]*((sint8) tmpVs.B[2*j]);
	    tmp16b = tmpVfe.HW[2*j];
	    tmp16b += tmp16a;
	    tmpVfe.HW[2*j] = tmp16b;

	    tmp16a = tmpVr.B[2*j+1]*((sint8) tmpVs.B[2*j]); /* same coefficient */
	    tmp16b = tmpVfe.HW[2*j+1];
	    tmp16b += tmp16a;
	    tmpVfe.HW[2*j+1] = tmp16b;

	    tmp16a = tmpVr.B[2*j+4]*((sint8) tmpVs.B[2*j+4]);
	    tmp16b = tmpVfo.HW[2*j];
	    tmp16b += tmp16a;
	    tmpVfo.HW[2*j] = tmp16b;

	    tmp16a = tmpVr.B[2*j+1+4]*((sint8) tmpVs.B[2*j+4]); /* same coefficient */ 
	    tmp16b = tmpVfo.HW[2*j+1];
	    tmp16b += tmp16a;
	    tmpVfo.HW[2*j+1] = tmp16b;

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


/**************** Vx: ADB ***********************/
/* Multiply interleave multiple coefficients    */
/* Gx? MPVIU8S8 Df, Vr, Vs                      */
/************************************************/

/*
// Multiply interleaved (UV) with multiple signed coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 sint8 coefficients; only 4 coefficients at even position are used.
// Outputs:
//   Df = 8 sint16 results.
Gx? MPVIU8S8 Df, Vr, Vs  // Df.H[2*j] = Vr.B[2*j]*Vs.B[2*j];
                         // Df.H[2*j+1] = Vr.B[2*j+1]*Vs.B[2*j];
*/

INT128 builtvx_mpviu8s8(long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	sint16 tmp16;

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
        /* Gx? MPVU8S8 Df, Vr, Vs                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* Multiply */

        if(x_guard[0]) {

	  for (j=0; j<2; j++) {

	    tmp16 = tmpVr.B[2*j]*((sint8) tmpVs.B[2*j]);
	    tmpVfe.HW[2*j] = tmp16;

	    tmp16 = tmpVr.B[2*j+1]*((sint8) tmpVs.B[2*j]); /* same coefficient */
	    tmpVfe.HW[2*j+1] = tmp16;



	    tmp16 = tmpVr.B[2*j+4]*((sint8) tmpVs.B[2*j+4]);
	    tmpVfo.HW[2*j] = tmp16;

	    tmp16 = tmpVr.B[2*j+1+4]*((sint8) tmpVs.B[2*j+4]); /* same coefficient */
	    tmpVfo.HW[2*j+1] = tmp16;

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


/**************** Vx: ADB ***********************/
/* MAC interleave with multiple coefficients    */
/* Gx? MAVIU8S8SHR7<attr> Df, Vr, Vs            */
/************************************************/

/*
// MAC interleaved (UV) with multiple signed coefficients
//   Vr = 8 uint8 values;
//   Vs = 8 sint8 coefficients; only 4 coefficients at even position are used.
//   as inputs: Df = 8 sint16 values;
// Outputs:
//   Df = 8 sint8 results.
Gx? MAVIU8S8 Df, Vr, Vs  // Df.H[2*j] += Vr.B[2*j]*Vs.B[2*j];
                         // Df.H[2*j+1] += Vr.B[2*j+1]*Vs.B[2*j];
Gx? MAVIU8S8SHR7<attr> Df, Vr, Vs  // Dfe 8 sint8 results
// results right shifted by 7 with or without rounding (coefficients as n/128)
// if overflow, wrap-arounded. n could be > 128.
// <attr>=,R (2's complement, "(...+64)>>7")
*/

INT128 vx_maviu8s8shr7_attr(uint8 attr, long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

	INT128 tmpDf;

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

	uint1 round;

	sint8 tmp8out;
	sint16 tmp16a, tmp16b;

	GPVR tmpVfe, tmpVfo;
	GPVR tmpVfe_out;
        GPVR tmpVr, tmpVs;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	tmpVfe.LL = var_Df_l;
	tmpVfo.LL = var_Df_h;
	GPVR_Write_Pair(f, tmpVfo, tmpVfe);

        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);
        tmpVs.LL = var_Vs;
        GPVR_Write(s, tmpVs);

        /* Operands to X interface */

        /* Immediate data */

	round = attr;

        /* --------------------------------------------- */
        /* Gx? MAVIU8S8SHR7<attr> Df, Vr, Vs             */
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
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s); /* coefficients */

	/* MAC and shift */

        if(x_guard[0]) {

	  for (j=0; j<2; j++) {

	    tmp16a = tmpVr.B[2*j]*((sint8) tmpVs.B[2*j]);
	    tmp16b = tmpVfe.HW[2*j];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j] = tmp8out;

	    tmp16a = tmpVr.B[2*j+1]*((sint8) tmpVs.B[2*j]); /* same coefficient */
	    tmp16b = tmpVfe.HW[2*j+1];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j+1] = tmp8out;


	    tmp16a = tmpVr.B[2*j+4]*((sint8) tmpVs.B[2*j+4]);
	    tmp16b = tmpVfo.HW[2*j];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j+4] = tmp8out;

	    tmp16a = tmpVr.B[2*j+1+4]*((sint8) tmpVs.B[2*j+4]); /* same coefficient */ 
	    tmp16b = tmpVfo.HW[2*j+1];
	    tmp16b += tmp16a;
	    tmp8out = shrs16s8r(round, tmp16b, 7); /* shift and round */
	    tmpVfe_out.B[2*j+1+4] = tmp8out;

	  }

          /* Write Back Results to Vfe */
	  GPVR_Write((2*f), tmpVfe_out);
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

INT128 builtvx_maviu8s8shr7(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return (vx_maviu8s8shr7_attr(0, var_Df_h, var_Df_l, var_Vr, var_Vs));

}

INT128 builtvx_maviu8s8shr7r(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

  return (vx_maviu8s8shr7_attr(1, var_Df_h, var_Df_l, var_Vr, var_Vs));

}
