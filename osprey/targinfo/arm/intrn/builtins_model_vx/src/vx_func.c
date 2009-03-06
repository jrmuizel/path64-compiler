/*****************/
/* vx_func.c     */
/*****************/
/* to be included in model_vx.c */

/*
 * Vx GPVR update
 */

/* Write Back to Vk */
void GPVR_Write(uint8 k, GPVR tmpGPVR) {
  Vk = tmpGPVR;
}

/* Read from Vk */
GPVR GPVR_Read(uint8 k) {
  return(Vk);
}

/* Write Back to registers pair Df */
void GPVR_Write_Pair(uint8 f, GPVR tmpGPVRo, GPVR tmpGPVRe)  {
    GPVR_Write((2*f+1), tmpGPVRo);
    GPVR_Write((2*f), tmpGPVRe);
}

/* Write Back Word "windex" of Vk, windex=0..(GPVR_SIZE-1). */
void GPVR_Write_Word(uint8 k, uint8 windex, uint32 tmpU32) {

    Vk.W[windex] = tmpU32;

}

/* Read Word "windex" from Vk, windex=0..(GPVR_SIZE-1). */
uint32 GPVR_Read_Word(uint8 k, uint8 windex) {

  uint32 tmpU32;

  tmpU32 = Vk.W[windex];

  return(tmpU32);

}

/* Write Back Half-Word "hwindex" of Vk */
void GPVR_Write_HalfWord(uint8 k, uint8 hwindex, uint16 tmpU16) {

    Vk.HW[hwindex] = tmpU16;

}

/* Read Half-Word "hwindex" from Vk */
uint16 GPVR_Read_HalfWord(uint8 k, uint8 hwindex) {

  uint16 tmpU16;

  tmpU16 = Vk.HW[hwindex];

  return(tmpU16);

}

/* Write Back byte "bindex" of Vk */
void GPVR_Write_Byte(uint8 k, uint8 bindex, uint8 tmpU8) {

    Vk.B[bindex] = tmpU8;

}

/* Read byte "bindex" from Vk */
uint8 GPVR_Read_Byte(uint8 k, uint8 bindex) {

  uint8 tmpU8;

  tmpU8 = Vk.B[bindex];

  return(tmpU8);

}


/*
 * Arithmetic/Logic functions
 */

/* Absolute and Difference */
uint8 absdiffU8(uint8 tmpU8a, uint8 tmpU8b) {

  uint8 tmpU8;
  sint9 tmp9a, tmp9b;

  tmp9a = ZeroEx(9, 8, tmpU8a);
  tmp9b = ZeroEx(9, 8, tmpU8b);

  tmp9a = tmp9a - tmp9b;

  if ( tmp9a < 0 )
    tmp9a = - tmp9a;

  tmpU8 = BF_range(tmp9a, 7, 0);

  return(tmpU8);
}

/* Addition */
uint9 addU9(uint8 tmpU8a, uint8 tmpU8b) {

  uint9 tmpU9a, tmpU9b;

  tmpU9a = ZeroEx(9, 8, tmpU8a);
  tmpU9b = ZeroEx(9, 8, tmpU8b);

  tmpU9a = tmpU9a + tmpU9b;

  return(tmpU9a);
}

uint10 addU10(uint9 tmpU9a, uint9 tmpU9b) {

  uint10 tmpU10;

  tmpU10 = ZeroEx(10, 9, tmpU9a) + ZeroEx(10, 9, tmpU9b);

  return(tmpU10);
}

/* Subtraction */
sint10 subS10(uint9 tmpU9a, uint9 tmpU9b) {

  sint10 tmp10;

  tmp10 = ZeroEx(10, 9, tmpU9a) - ZeroEx(10, 9, tmpU9b);

  return(tmp10);
}

/* Clamping */
uint8 clampU8(sint11 tmp11) {

uint8 tmpU8;

 if (tmp11 < 0) {
   tmpU8 = 0;
 } else {
   if (tmp11 > 255) {
     tmpU8 = 255;
   } else {
     tmpU8 = BF_range(tmp11, 7, 0);
   }
 }

 return(tmpU8);
}


/* Fractional multiplication (n/16) and rounding, n=0..15. */
uint8 mfU8(uint1 round_attr, uint8 tmpU8, uint4 tmpU4) {

  uint12 tmpU12;
  uint8 tmpU8a;

  tmpU12 = tmpU4*tmpU8;
  if(round_attr) {
    tmpU12 = tmpU12 + 0x8; /* for rounding */
  }
  tmpU12 = tmpU12 >> 4;
  tmpU8a = BF_range(tmpU12, 7, 0);

  return(tmpU8a);
}

/* Fractional multiplication (n/8) and rounding, n=0..15. */
uint9 mpshr3U9(uint1 round_attr, uint8 tmpU8, uint4 tmpU4) {

  uint12 tmpU12;
  uint9 tmpU9;

  tmpU12 = tmpU4*tmpU8;
  if(round_attr) {
    tmpU12 = tmpU12 + 0x4; /* for rounding */
  }
  tmpU12 = tmpU12 >> 3;
  tmpU9 = BF_range(tmpU12, 8, 0);

  return(tmpU9);
}

/* Fractional multiplication (n/16), addition and rounding */
uint8 mfaddU8(uint1 round_attr, uint9 tmpU9a, uint4 tmpU4a, uint9 tmpU9b, uint4 tmpU4b) {

  uint14 tmpU14;
  uint8 tmpU8;

  tmpU14 = tmpU4a*tmpU9a + tmpU4b*tmpU9b;
  if(round_attr) {
    tmpU14 = tmpU14 + 0x8; /* for rounding */
  }
  tmpU14 = tmpU14 >> 4;
  tmpU8 = BF_range(tmpU14, 7, 0);

  return(tmpU8);
}

/* Mean of two values with or without rounding */

uint8
mean2U8(uint1 round_attr, uint8 tmpU8a, uint8 tmpU8b) {

uint8 tmpU8;
uint9 tmpU9;

  tmpU9 = ZeroEx(9, 8, tmpU8a) +  ZeroEx(9, 8, tmpU8b);
  if(round_attr) {
    tmpU9 = tmpU9 + 0x1; /* for rounding */
  }
  tmpU9 = tmpU9 >> 1;
  tmpU8 = BF_range(tmpU9, 7, 0);

  return(tmpU8);
}

/* Mean of four values with or without rounding */

uint8
mean4U8(uint1 round_attr, uint8 tmpU8a, uint8 tmpU8b, uint8 tmpU8c, uint8 tmpU8d) {

uint8 tmpU8;
uint10 tmpU10;

  tmpU10 = ZeroEx(10, 8, tmpU8a) + ZeroEx(10, 8, tmpU8b) + ZeroEx(10, 8, tmpU8c) + ZeroEx(10, 8, tmpU8d);
  if(round_attr) {
    tmpU10 = tmpU10 + 0x2; /* for rounding */
  }
  tmpU10 = tmpU10 >> 2;
  tmpU8 = BF_range(tmpU10, 7, 0);

  return(tmpU8);
}

/* Median of three values */
uint8 medianU8(uint8 tmpU8a, uint8 tmpU8b, uint8 tmpU8c) {

uint8 tmpU8;

 tmpU8 = M_MEDIAN(tmpU8a, tmpU8b, tmpU8c);

 return(tmpU8);
}

/* multiplication sint8*sint9 given sint16 where -256 excluded for sint9 */
/* if overflow, wrap-arounded. */
sint16 mps8s9_m256(sint8 tmp8, sint9 tmp9) {

  sint16 tmp16;

  tmp16 = tmp8*tmp9;

  return(tmp16);
}

/* generation of a 32-bit random number */
uint32 random(uint32 tmpU32seed) {

    uint32 newbit;
    uint32 tmpU32new;

    /* XOR bit 31 with bit 3 */
    newbit = ((tmpU32seed >> 30) & 1) ^ ((tmpU32seed >> 2) & 1);

    /* Shift the seed one bit to the left and put the new bit as LSB */
    tmpU32new = (tmpU32seed << 1) | newbit;

  return(tmpU32new);
}


sint8 shrs16s8r(uint1 round, sint16 tmp16, uint4 tmpU4) {

sint8 tmp8;
sint16 tmp16a;

  if(round) {  /* 2's complement rounding */
    tmp16a = tmp16 + (1<<(tmpU4-1));
    tmp8 = tmp16a>>tmpU4;
  } else {
    tmp8 = tmp16>>tmpU4;
  }

   return(tmp8);
}

uint8 shru16u8r(uint1 round, uint16 tmpU16, uint4 tmpU4) {

uint8 tmpU8;
uint16 tmpU16a;

  if(round) {  /* 2's complement rounding */
    tmpU16a = tmpU16 + (1<<(tmpU4-1));
    tmpU8 = tmpU16a>>tmpU4;
  } else {
    tmpU8 = tmpU16>>tmpU4;
  }

   return(tmpU8);
}

sint8 shr7s16s8rcrs(uint2 round, sint16 tmp16in) {

sint7 tmp7;
sint8 tmp8;
sint16 tmp16a, tmp16b;

   tmp16a = tmp16in;

   if(round==0) {  /* convergnt rounding */

     tmp16b = tmp16a+64;
     tmp8 = tmp16b>>7;

     if ((tmp16b & 0x7F) == 0x0000)  /* if half-way */
       tmp8 = tmp8 & 0xFE;  /* tmp8[0] = 0b0; */
     
   } else {  /* symetric rounding */

     if ((tmp16a<0) && ((tmp16a&0x7F) == 0x40))  /* if half-way and negative */
       tmp7 = -64;
     else
       tmp7 = 64;
     tmp16b = tmp16a + tmp7;
     tmp8 = tmp16b >> 7;

   }

   return(tmp8);
}

uint8 cmpU8(uint3 cmpU3, uint8 tmpU8a, uint8 tmpU8b) {

  if ( cmpU3 == 0x0 ) {
    return ( (tmpU8a == tmpU8b) ? 0xFF : 0x00 );
  } else {
    if ( cmpU3 == 0x1 ) {
      return ( (tmpU8a != tmpU8b) ? 0xFF : 0x00 );
    } else {
      if ( cmpU3 == 0x2 ) {
	return ( (tmpU8a >= tmpU8b) ? 0xFF : 0x00);
      } else {
	if ( cmpU3 == 0x3 ) {
	  return ( (tmpU8a < tmpU8b) ? 0xFF : 0x00 );
	} else {
	  if ( cmpU3 == 0x4 ) {
	    return ( (tmpU8a <= tmpU8b) ? 0xFF : 0x00 );
	  } else {
	    if ( cmpU3 == 0x5 ) {
	      return ( (tmpU8a > tmpU8b) ? 0xFF : 0x00 );
	    }
	  }
	}
      }
    }
  }

  return(0x00); /* should never happen */

}

/*
 * Misc functions
 */

uint64
setBitRange(uint64 objValue, const uint32 msb, const uint32 lsb, uint64 val) {

  // put source at the right position
  uint64 tmp = val << lsb;
  uint64 srcMask = ~((uint64) 0) << lsb;
  // discard unused bits at the left of msb
  srcMask &= DATA_VALUE_MASK(msb + 1);
  
  // insert
  return (objValue & ~srcMask) | (tmp & srcMask);
}
