/*******************/
/* vx_move.c       */
/*******************/
/* to be included in model_vx.c */


/**************** Vx: ADB ***********************/
/* Move byte to GPVR                            */
/* Gx? M2XB Vk, Rn, u3                          */
/************************************************/

/*
M2XB Vk, Rn, u3  // move byte Rn.B[0] to Vk.B[u3]
*/

long long builtvx_m2xb(long long var_Vk, long var_Rn, unsigned char imm_u3) {

        /* local variables used in the instruction description */

        uint3 u3;

	uint8 tmpU8;

        GPVR tmpVk;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVk.LL = var_Vk;
        GPVR_Write(k, tmpVk);

	/* Operands to the extension interface */
	x_opa = var_Rn;

        /* Immediate data */
	u3 = imm_u3;

        /* --------------------------------------------- */
        /* Gx? M2XB Vk, Rn, u3                           */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU8 = BF_range(x_opa, 7, 0);

	/* Shift and Move */

	if(x_guard[0]) {

          /* Write byte to Vk */
	  GPVR_Write_Byte(k, u3, tmpU8);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */

        tmpVk = GPVR_Read(k);
        return(tmpVk.LL);

}


/**************** Vx: ADB ***********************/
/* Move half-word to GPVR                       */
/* Gx? M2XHW Vk, Rn, u2                         */
/************************************************/

/*
M2XHW Vk, Rn, u2 // move half-word Rn.HW[0] to Vk.HW[u2]
*/

long long builtvx_m2xhw(long long var_Vk, long var_Rn, unsigned char imm_u2) {

        /* local variables used in the instruction description */

        uint2 u2;

	uint16 tmpU16;

        GPVR tmpVk;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVk.LL = var_Vk;
        GPVR_Write(k, tmpVk);

	/* Operands to the extension interface */
	x_opa = var_Rn;

        /* Immediate data */
	u2 = imm_u2;

        /* --------------------------------------------- */
        /* Gx? M2XHW Vk, Rn, u2                          */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU16 = BF_range(x_opa, 15, 0);

	/* Move */

	if(x_guard[0]) {

          /* Write half-word to Vk */

	  GPVR_Write_HalfWord(k, u2, tmpU16);
        }

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */

        tmpVk = GPVR_Read(k);
        return(tmpVk.LL);

}


/**************** Vx: ADB ***********************/
/* Move Word to GPVR                            */
/* Gx? M2XW<wp> Vk, Rn                          */
/************************************************/

long long vx_m2xw(uint8 attr_wk, long long var_Vk, long var_Rn) {

        /* local variables used in the instruction description */

        uint1 wk;

	uint32 tmpU32;

        GPVR tmpVk;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVk.LL = var_Vk;
        GPVR_Write(k, tmpVk);

	/* Operands to the extension interface */
	x_opa = var_Rn;

        /* Immediate data */
	wk = attr_wk;

        /* --------------------------------------------- */
        /* Gx? M2XW<wp> Vk, Rn                           */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU32 = x_opa;

	if(x_guard[0]) {

		/* Write Back Results to Vk */
		GPVR_Write_Word(k, wk, tmpU32);

	}

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from GPVR */

        tmpVk = GPVR_Read(k);
        return(tmpVk.LL);

}

long long builtvx_m2xw0(long long var_Vk, long var_Rn) {

  return(vx_m2xw(0, var_Vk, var_Rn));

}

long long builtvx_m2xw1(long long var_Vk, long var_Rn) {

  return(vx_m2xw(1, var_Vk, var_Rn));

}


/**************** Vx: ADB ***********************/
/* Move Two Words to GPVR                       */
/* Gx? M2X Vk, Rn, Rp                           */
/************************************************/

long long builtvx_m2x(long var_Rn, long var_Rp) {

        /* local variables used in the instruction description */

        GPVR tmpVk;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	/* Operands to the extension interface */
	x_opa = var_Rn;
	x_opb = var_Rp;

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? M2X Vk, Rn, Rp                            */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpVk.W[1] = x_opa;
	tmpVk.W[0] = x_opb;

	if(x_guard[0]) {

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
/* Move unsigned byte to GPR                    */
/* Gx? M2RUB Rm, Vr, u3                         */
/************************************************/

/*
M2RUB Rm, Vr, u3  // move Vr.B[u3] (zero-extended to 32-bit) to Rm
*/

long builtvx_m2rub(long long var_Vr, unsigned char imm_u3) {

        /* local variables used in the instruction description */

        uint3 u3;

	uint8 tmpU8;
	uint32 tmpU32;

        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */

        /* Immediate data */
	u3 = imm_u3;

        /* --------------------------------------------- */
        /* Gx? M2RUB Rm, Vr, u3                          */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU8 = GPVR_Read_Byte(r, u3);

	tmpU32 = ZeroEx(32, 8, tmpU8);

	/* Move to GPR */

	if (x_guard[0] | x_guard[1] | x_guard[2] | x_guard[3]) {

	  x_result = tmpU32;

	}

/* tmp32[31:0] = x_result[31:0]; */
/* GPR_Update(m, x, tmp32[31:24], tmp32[23:16], tmp32[15:8], tmp32[7:0]); */

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from x_result */

        return(x_result);

}


/**************** Vx: ADB ***********************/
/* Move unsigned half-word to GPR               */
/* Gx? M2RUHW Rm, Vr, u2                        */
/************************************************/

/*
M2RUHW Rm, Vr, u2  // move half-word Vk.HW[u2] (zero-extended to 32-bit) to Rm
*/

long builtvx_m2ruhw(long long var_Vr, unsigned char imm_u2) {

        /* local variables used in the instruction description */

        uint2 u2;

	uint16 tmpU16;
	uint32 tmpU32;

        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */

        /* Immediate data */
	u2 = imm_u2;

        /* --------------------------------------------- */
        /* Gx? M2RUHW Rm, Vr, u2                         */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU16 = GPVR_Read_HalfWord(r, u2);

	tmpU32 = ZeroEx(32, 16, tmpU16);

	/* Move to GPR */

	if(x_guard[0] | x_guard[1] | x_guard[2] | x_guard[3]) {

	  x_result = tmpU32;

	}

/* tmp32[31:0] = x_result[31:0]; */
/* GPR_Update(m, x, tmp32[31:24], tmp32[23:16], tmp32[15:8], tmp32[7:0]); */

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from x_result */

        return(x_result);

}


/**************** Vx: ADB ***********************/
/* Move Word to GPR                             */
/* Gx? M2RW<wp> Rm, Vr                          */
/************************************************/

long vx_m2rw(uint8 attr_wr, long long var_Vr) {

        /* local variables used in the instruction description */

        uint1 wr;

	uint32 tmpU32;

        GPVR tmpVr;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */
        tmpVr.LL = var_Vr;
        GPVR_Write(r, tmpVr);

	/* Operands to the extension interface */

        /* Immediate data */
	wr = attr_wr;

        /* --------------------------------------------- */
        /* Gx? M2RW<wp> Rm, Vr                           */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */

	/* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
	/* x_guard[0] = GR[x]; */
	/* x_guard[1] = GR[x+8]; */
	/* x_guard[2] = GR[x+16]; */
	/* x_guard[3] = GR[x+24]; */

	/* Operands Fetch */

	tmpU32 = GPVR_Read_Word(r, wr);

	/* Move to GPR */

	if(x_guard[0] | x_guard[1] | x_guard[2] | x_guard[3]) {

		x_result = tmpU32;

	}

/* tmp32[31:0] = x_result[31:0]; */
/* GPR_Update(m, x, tmp32[31:24], tmp32[23:16], tmp32[15:8], tmp32[7:0]); */

        /* -------------------------- cut here BOTTOM --- */
        /**************************************************/

        /* Results from x_result */

        return(x_result);

}

long builtvx_m2rw0(long long var_Vr) {

  return(vx_m2rw(0, var_Vr));

}

long builtvx_m2rw1(long long var_Vr) {

  return(vx_m2rw(1, var_Vr));

}




/**************** Vx: ADB ***********************/
/* Clear                                        */
/* Gx? CLR Vk                                   */
/************************************************/

long long builtvx_clr(void) {

        /* local variables used in the instruction description */

        uint8 j;  /* byte indice */

        GPVR tmpVk;

        /* GPVR Register indices: k, r, s, f, h, q */
        GPVR_indices_init();

        /* Operands to GPVR */

	/* Operands to the extension interface */

        /* Immediate data */

        /* --------------------------------------------- */
        /* Gx? CLR Vk                                    */
        /* Execution Field in adb:                       */
        /* -------------------------- cut here TOP ----- */


        /* x_inst[31:0] = current_inst[31:0]; */ /* The 32 bits of the current instruction */
        /* x_guard[0] = GR[x]; */
        /* x_guard[1] = GR[x+8]; */
        /* x_guard[2] = GR[x+16]; */
        /* x_guard[3] = GR[x+24]; */

	/* Clear */

	if(x_guard[0]) {

	        for (j=0; j<8; j++) {
			tmpVk.B[j] = 0;
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
