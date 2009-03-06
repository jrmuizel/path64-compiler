/***********************/
/* vx_pixel_rodering.c */
/***********************/
/* to be included in model_vx.c */


/**************** Vx-DEI: ADB ********************/
/* Delay Line Update                             */
/* Gx? DLUPDATE Df, Vr, Vs                       */
/*************************************************/

/*
// Delay Line Update
// Input data:
//   Vfo= not used
//   Vfe= 8 previous pixels;
//   Vr = 8 current pixels; Vs = 8 new pixels
// Outputs:
//   Vfo= 2 previous and 2 new pixels; Vfe= 8 current pixels;
 */

INT128 builtvx_dlupdate(long long var_Df_h, long long var_Df_l, long long var_Vr, long long var_Vs) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

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

        /* --------------------------------------------- */
        /* Gx? DLUPDATE Df, Vr, Vs                       */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

        tmpVfe = GPVR_Read((2*f));
        tmpVr = GPVR_Read(r);
        tmpVs = GPVR_Read(s);

        if(x_guard[0]) {

                /* previous pixels */
                tmpVfo.B[6] = tmpVfe.B[6];
                tmpVfo.B[7] = tmpVfe.B[7];

                /* current pixels */
                for (j=0; j<8; j++) {
                        tmpVfe.B[j] = tmpVr.B[j];
                }

                /* new pixels */
                tmpVfo.B[0] = tmpVs.B[0];
                tmpVfo.B[1] = tmpVs.B[1];

                /* not used bytes are cleared */
                tmpVfo.B[2] = 0;
                tmpVfo.B[3] = 0;
                tmpVfo.B[4] = 0;
                tmpVfo.B[5] = 0;

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
/* COPY LSB                                      */
/* Gx? CPLSB Vk, Vr, u3                          */
/*************************************************/

/*
// For u3 = 1..7: Extend the byte Vr.B[u3] to the right, put the result in Vk.
// If u3 = 0, Vk = Vr.
 */

long long builtvx_cplsb(long long var_Vr, unsigned char imm_u3) {
        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint3 u3;

        uint3 tmpU3;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);

        /* Operands to X interface */

        /* Immediate data */

        u3 = imm_u3;  /* LSB position */

        /* --------------------------------------------- */
        /* Gx? CPLSB Vk, Vr, u3                          */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

        tmpVr = GPVR_Read(r);
        tmpU3 = BF_range(u3, 2, 0);  /* LSB position */

        if(x_guard[0]) {

          if (tmpU3 == 0) {
	    tmpVk = tmpVr;
          } else {
            for (j=0; j<tmpU3; j++) {
              tmpVk.B[j] = tmpVr.B[tmpU3]; /* Extend the byte Vr.B[tmpU3] to the right */
            }
            for (j=tmpU3; j<8; j++) {
              tmpVk.B[j] = tmpVr.B[j];        
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


/**************** Vx-DEI: ADB ********************/
/* COPY MSB                                      */
/* Gx? CPMSB Vk, Vr, u3                          */
/*************************************************/

/*
// For u3 = 0..6: Extend the byte Vr.B[u3] to the left, put the result in Vk.
// If u3 = 7, Vk = Vr.
 */

long long builtvx_cpmsb(long long var_Vr, unsigned char imm_u3) {
        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint3 u3;

        uint3 tmpU3;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);

        /* Operands to X interface */

        /* Immediate data */

        u3 = imm_u3;  /* MSB position */

        /* --------------------------------------------- */
        /* Gx? CPMSB Vk, Vr, u3                          */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

        tmpVr = GPVR_Read(r);
        tmpU3 = BF_range(u3, 2, 0);  /* MSB position */

        if(x_guard[0]) {

          if (tmpU3 == 7) {
            tmpVk = tmpVr;
          } else {
            for (j=(tmpU3+1); j<8; j++) {
              tmpVk.B[j] = tmpVr.B[tmpU3]; /* Extend the byte Vr.B[tmpU3] to the left */
            }
            for (j=0; j<(tmpU3+1); j++) {
              tmpVk.B[j] = tmpVr.B[j];        
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


/**************** Vx-DEI: ADB ********************/
/* COPY MSB                                      */
/* Gx? CPMSBR Vk, Vr, Rn                         */
/*************************************************/

/*
// For Rn=u3= 0..6: Extend the byte Vr.B[u3] to the left, put the result in Vk.
// If u3 = 7, Vk = Vr.
 */

long long builtvx_cpmsbr(long long var_Vr, long var_Rn) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint3 tmpU3;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);

        /* Operands to X interface */

	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? CPMSBR Vk, Vr, Rn                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

        tmpVr = GPVR_Read(r);
        tmpU3 = BF_range(x_opa, 2, 0);  /* MSB position */

        if(x_guard[0]) {

          if (tmpU3 == 7) {
            tmpVk = tmpVr;
          } else {
            for (j=(tmpU3+1); j<8; j++) {
              tmpVk.B[j] = tmpVr.B[tmpU3]; /* Extend the byte Vr.B[tmpU3] to the left */
            }
            for (j=0; j<(tmpU3+1); j++) {
              tmpVk.B[j] = tmpVr.B[j];        
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


/**************** Vx-DEI: ADB ********************/
/* COPY pair of interleaved LSB                  */
/* Gx? CPLSBI Vk, Vr, u2                         */
/*************************************************/

/*
// COPY pair of interleaved LSB 
// For u2 = 1..3: Extend the pair of bytes Vr.B[2*u2+1] and
//                Vr.B[2*u2] to the right, put the result in Vk.
// If u3 = 0, Vk = Vr.
 */

long long builtvx_cplsbi(long long var_Vr, unsigned char imm_u2) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint2 u2;

        uint2 tmpU2;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);

        /* Operands to X interface */

        /* Immediate data */

        u2 = imm_u2;  /* pair of LSBs position */

        /* --------------------------------------------- */
        /* Gx? CPLSBI Vk, Vr, u2                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpU2 = BF_range(u2, 1, 0);  /* pair of LSBs position */

	/* Copy pair of LSBs */

        if(x_guard[0]) {

          if (tmpU2 == 0) {
            tmpVk = tmpVr;
          } else {
            for (j=0; j<tmpU2; j++) {
	      /* Extend to the right */
	      tmpVk.B[2*j] = tmpVr.B[2*tmpU2];
	      tmpVk.B[2*j+1] = tmpVr.B[2*tmpU2+1];
            }
            for (j=tmpU2; j<4; j++) {
              tmpVk.B[2*j] = tmpVr.B[2*j];
              tmpVk.B[2*j+1] = tmpVr.B[2*j+1];
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


/**************** Vx-DEI: ADB ********************/
/* COPY pair of interleaved MSB                  */
/* Gx? CPMSBI Vk, Vr, u2                         */
/*************************************************/

/*
// For u2 = 0..2: Extend the pair of bytes Vr.B[2*u2+1] and
//                Vr.B[2*u2] to the left, put the result in Vk.
// If u2 = 3, Vk = Vr;
 */

long long builtvx_cpmsbi(long long var_Vr, unsigned char imm_u2) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint2 u2;

        uint2 tmpU2;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);

        /* Operands to X interface */

        /* Immediate data */

        u2 = imm_u2;  /* MSBs position */

        /* --------------------------------------------- */
        /* Gx? CPMSBI Vk, Vr, u2                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpU2 = BF_range(u2, 1, 0);  /* MSBs position */

	/* Copy pair of MSBs */

        if(x_guard[0]) {

          if (tmpU2 == 3) {
            tmpVk = tmpVr;
          } else {
            for (j=(tmpU2+1); j<4; j++) {
	      /* Extend to the left */
	      tmpVk.B[2*j] = tmpVr.B[2*tmpU2];
	      tmpVk.B[2*j+1] = tmpVr.B[2*tmpU2+1];
            }
            for (j=0; j<(tmpU2+1); j++) {
              tmpVk.B[2*j] = tmpVr.B[2*j];
              tmpVk.B[2*j+1] = tmpVr.B[2*j+1];
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


/**************** Vx-DEI: ADB ********************/
/* COPY pair of interleaved MSB                  */
/* Gx? CPMSBIR Vk, Vr, Rn                        */
/*************************************************/

/*
// For Rn=u2= 0..2: Extend the pair of bytes Vr.B[2*u2+1] and
//                Vr.B[2*u2] to the left, put the result in Vk.
// If u2 = 3, Vk = Vr;
 */

long long builtvx_cpmsbir(long long var_Vr, long var_Rn) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        uint2 tmpU2;

        GPVR tmpVk, tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;

        GPVR_Write(r, tmpVr);

        /* Operands to X interface */

	x_opa = var_Rn;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? CPMSBIR Vk, Vr, Rn                        */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

        tmpVr = GPVR_Read(r);
        tmpU2 = BF_range(x_opa, 1, 0);  /* MSBs position */

	/* Copy pair of MSBs */

        if(x_guard[0]) {

          if (tmpU2 == 3) {
            tmpVk = tmpVr;
          } else {
            for (j=(tmpU2+1); j<4; j++) {
	      /* Extend to the left */
	      tmpVk.B[2*j] = tmpVr.B[2*tmpU2];
	      tmpVk.B[2*j+1] = tmpVr.B[2*tmpU2+1];
            }
            for (j=0; j<(tmpU2+1); j++) {
              tmpVk.B[2*j] = tmpVr.B[2*j];
              tmpVk.B[2*j+1] = tmpVr.B[2*j+1];
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
