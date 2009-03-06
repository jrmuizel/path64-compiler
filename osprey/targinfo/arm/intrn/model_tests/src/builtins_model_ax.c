/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version ST/HPC/STS (2006/Oct/18)
 */

#ifdef __AX

#error "builtins_model_ax must not be used on SX+AX Core"

#else

#include "builtins_model_common.h"

long long __builtin_ax_mpss(int s1, int s2)
{
    long long t0 = EXT64(s1, 32);
    long long t1 = EXT64(s2, 32);
    long long t2 = t0*t1;
    return t2;
}

long long __builtin_ax_mnss(int s1, int s2)
{
    long long t0 = EXT64(s1, 32);
    long long t1 = EXT64(s2, 32);
    long long t2 = -(t0*t1);
    return t2;
}

long long __builtin_ax_mpf(int s1, int s2)
{
    long long t0 = EXT64(s1, 32);
    long long t1 = EXT64(s2, 32);
    long long t2 = (t0*t1)<<1;
    return t2;
}

long long __builtin_ax_mnf(int s1, int s2)
{
    long long t0 = EXT64(s1, 32);
    long long t1 = EXT64(s2, 32);
    long long t2 = - ( (t0*t1)<<1 );
    return t2;
}

long long __builtin_ax_mpfr(int s1, int s2)
{
    long long t0 = __builtin_ax_mpf(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_mnfr(int s1, int s2)
{
    long long t0 = __builtin_ax_mnf(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_shlf(int s1, int s2)
{
    long long t0 = EXT64(s1, 32) << 32 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    return t2;
}

long long __builtin_ax_shrf(int s1, int s2)
{
    long long t0 = EXT64(s1, 32) << 32 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    return t2;
}

long long __builtin_ax_m2xhl(int s1, int s2)
{
//Stores 32-bit word s1   and   32-bit word s2 into respectively :  
    long long t0 = EXT64(s1, 32) << 32 ;	 // the High Word (32 MSB)
    long long t1 = EXTU64(s2, 32) ;			 // the Low  Word (32 LSB)
    long long t2 = t0 | t1 ;
    return t2;
}


long long __builtin_ax_mpssl(long long s1, int s2)
{
    long long t0 = EXT64(s1, 32);
    long long t1 = EXT64(s2, 32);
    long long t2 = t0*t1;
    return t2;
}

long long __builtin_ax_mnssl(long long s1, int s2)
{
    long long t0 = EXT64(s1, 32);
    long long t1 = EXT64(s2, 32);
    long long t2 = -(t0*t1);
    return t2;
}

long long __builtin_ax_mpfh(long long s1, int s2)
{
    long long t0 = (s1>>32);
    long long t1 = EXT64(s2, 32);
    long long t2 = (t0*t1) << 1;
    return t2;
}

long long __builtin_ax_mnfh(long long s1, int s2)
{
    long long t0 = (s1>>32);
    long long t1 = EXT64(s2, 32);
    long long t2 = -( (t0*t1) << 1 ) ;
    return t2;
}

long long __builtin_ax_mpfhr(long long s1, int s2)
{
    long long t0 = __builtin_ax_mpfh(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_mnfhr(long long s1, int s2)
{
    long long t0 = __builtin_ax_mnfh(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;

}


long long __builtin_ax_addf(long long s1, int s2)
{
    long long t0 = s1 ;
    long long t1 = EXT64(s2, 32) << 32 ;
    long long t2 = t0+t1;
    return t2;
}

long long __builtin_ax_subf(long long s1, int s2)
{
    long long t0 = s1 ;
    long long t1 = EXT64(s2, 32) << 32 ;
    long long t2 = t0-t1;
    return t2;
}

long long __builtin_ax_addfr(long long s1, int s2)
{
    long long t0 = __builtin_ax_addf(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_subfr(long long s1, int s2)
{
    long long t0 = __builtin_ax_subf(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_addi(long long s1, int s2)
{
    long long t0 = s1 ;
    long long t1 = EXT64(s2, 32)  ;
    long long t2 = t0+t1;
    return t2;
}

long long __builtin_ax_subi(long long s1, int s2)
{
    long long t0 = s1 ;
    long long t1 = EXT64(s2, 32)  ;
    long long t2 = t0-t1;
    return t2;
}

long long __builtin_ax_addiu(long long s1, unsigned int s2)
{
    long long t0 = s1 ;
    long long t1 = EXTU64(s2, 32)  ;
    long long t2 = t0+t1;
    return t2;
}

long long __builtin_ax_subiu(long long s1, unsigned int s2)
{
    long long t0 = s1 ;
    long long t1 = EXTU64(s2, 32)  ;
    long long t2 = t0-t1;
    return t2;
}



long long __builtin_ax_shl_r(long long s1, int s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    return t2;
}

long long __builtin_ax_shr_r(long long s1, int s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    return t2;
}

long long __builtin_ax_shlr_r(long long s1, int s2)
{
    long long t0 = __builtin_ax_shl_r(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_shrr_r(long long s1, int s2)
{
    long long t0 = __builtin_ax_shr_r(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}


unsigned long long __builtin_ax_shru_r(unsigned long long s1, unsigned int s2)
{
    unsigned long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    unsigned long long t2 = t0 >>t1 ;
    return t2;
}

long long __builtin_ax_add(long long s1, long long s2) 
{
    long long t0 = s1+s2;
    return t0;
}
long long __builtin_ax_sub(long long s1, long long s2) 
{
    long long t0 = s1-s2;
    return t0;
}
long long __builtin_ax_addr(long long s1, long long s2)
{
    long long t0 = __builtin_ax_add(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}
long long __builtin_ax_subr(long long s1, long long s2)
{
    long long t0 = __builtin_ax_sub(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

int __builtin_ax_shlm2rl_r(long long s1, int s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    long long t3 = EXT64(t2, 32);
    return (int)t3;
}

int __builtin_ax_shrm2rl_r(long long s1, int s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    long long t3 = EXT64(t2, 32);
    return (int)t3;
}

int __builtin_ax_shlm2rh_r(long long s1, int s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    long long t3 = t2>>32;
    return (int)t3;
}

int __builtin_ax_shrm2rh_r(long long s1, int s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    long long t3 = t2>>32;
    return (int)t3;
}

unsigned long long __builtin_ax_abs(long long s)
{
 	return (unsigned long long) llabs(s) ;
}


long long __builtin_ax_rnd16(long long s)	
{
    long long t0 = (s + 0x08000)   ;	  //round16(1)
    long long t1 = (t0 >> 16) << 16   ;	//round16(2)
	return t1 ;
}



long long __builtin_ax_rndc(long long s)	
{ // convergent round -> inferior value when at medium value

    long long t0 = (s + 0x080000000LL)   ;	  //round(1)
	unsigned int t0low = (unsigned int) EXTU64((t0 & 0x0FFFFFFFF),32)  ;

	if (t0low == 0x0)
	{ //round(2) bit32=0 (inferior value)
    	long long t1 = (t0 >> 33) << 33   ;	
  		return t1 ;
	}
	else
	{  //round(2)
    	long long t2 = (t0 >> 32) << 32   ;	//round(2)
  		return t2 ;
	}
}



long long __builtin_ax_rndc16(long long s)
{ // convergent round16 

    long long t0 = (s + 0x08000)   ;	  //round(1)
	unsigned int t0low = (unsigned int) EXTU64((t0 & 0x0FFFF),16)  ;

	if (t0low == 0x0)
	{ //round(2) bit16_to_0 =0 
    	long long t1 = (t0 >> 17) << 17   ;	
  		return t1 ;
	}
	else
	{  //round(2)  bit15_to_0 =0
    	long long t2 = (t0 >> 16) << 16   ;	//round(2)
  		return t2 ;
	}
}



long long __builtin_ax_clp32(long long s)	
{
    long long t0 = CLAMP64(s, 32) ;
    long long t1 = EXT64(t0, 32)   ;
  	return t1 ;
}


long long __builtin_ax_neg(long long s)		
{
 return (-s) ;
}


long long __builtin_ax_m2xf(int s)
{
//Stores 32-bit word s   into  :  
    long long t0 = EXT64(s, 32) << 32 ;	 // High Word (32 MSB)
    return t0;
}
	
long long __builtin_ax_m2xi(int s)	
{
    long long t0 = EXT64(s, 32) ;
    return t0;
}
	
long long __builtin_ax_m2xiu(unsigned int s)
{ 
    unsigned long long t0 = EXTU64(s, 32) ;
    return t0;
}

long long __builtin_ax_m2xl(long long s1, int s2)
{
   long long t0 = (s1 >> 32) << 32 ;	 // the High Word (32 MSB) remains unchanged
    long long t1 = EXTU64(s2, 32) ;		 // the Low  Word (32 LSB)  is s2
    long long t2 = t0 | t1 ;
    return t2;
}


long long __builtin_ax_m2xh(long long s1, int s2)
{
    long long t0 = EXTU64(s1, 32);	 		// the Low Word (32 MSB) remains unchanged
    long long t1 = EXT64(s2, 32) << 32 ;	// the High  Word (32 LSB)  is s2
    long long t2 = t0 | t1 ;
    return t2;
}







long long __builtin_ax_shl_i6(long long s1, unsigned short s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    return t2;
}

long long __builtin_ax_shr_i6(long long s1, unsigned short s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    return t2;
}
	
unsigned long long __builtin_ax_shru_i6(unsigned long long s1, unsigned short s2)
{
    unsigned long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    unsigned long long t2 = t0 >>t1 ;
    return t2;
}

long long __builtin_ax_shlr_i6(long long s1, unsigned short s2)
{
    long long t0 = __builtin_ax_shl_i6(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_shrr_i6(long long s1, unsigned short s2)
{
    long long t0 = __builtin_ax_shr_i6(s1,s2)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}


int __builtin_ax_shlm2rl_i6(long long s1, unsigned short s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    long long t3 = EXT64(t2, 32);
    return (int)t3;
}

int __builtin_ax_shrm2rl_i6(long long s1, unsigned short s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    long long t3 = EXT64(t2, 32);
    return (int)t3;
}

int __builtin_ax_shlm2rh_i6(long long s1, unsigned short s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    long long t3 = t2>>32;
    return (int)t3;
}

int __builtin_ax_shrm2rh_i6(long long s1, unsigned short s2)
{
    long long t0 = s1 ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    long long t3 = t2>>32;
    return (int)t3;
}


long long __builtin_ax_maf(long long s1, int s2, int s3)
{
    long long t0 = EXT64(s2, 32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1)<<1;
    long long t3 = s1 + t2 ;
    return t3;
}
long long __builtin_ax_mafr(long long s1, int s2, int s3)
{
    long long t0 = __builtin_ax_maf(s1,s2,s3)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_mafh(long long s1, long long s2, int s3)
{
    long long t0 = (s2>>32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1)<<1;
    long long t3 = s1 + t2 ;
    return t3;
}

long long __builtin_ax_mafhr(long long s1, long long s2, int s3)
{
    long long t0 = __builtin_ax_mafh(s1,s2,s3)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}


long long __builtin_ax_mass(long long s1, int s2, int s3)
{
    long long t0 = EXT64(s2, 32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1);
    long long t3 = s1 + t2 ;
    return t3;
}

long long __builtin_ax_massl(long long s1, long long s2, int s3)
{
    long long t0 = EXT64(s2, 32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1);
    long long t3 = s1 + t2 ;
    return t3;
}

long long __builtin_ax_msf(long long s1, int s2, int s3)
{
    long long t0 = EXT64(s2, 32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1)<<1;
    long long t3 = s1 - t2 ;
    return t3;
}

long long __builtin_ax_msfr(long long s1, int s2, int s3)
{
    long long t0 = __builtin_ax_msf(s1,s2,s3)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_msfh(long long s1, long long s2, int s3)
{
    long long t0 = (s2>>32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1)<<1;
    long long t3 = s1 - t2 ;
    return t3;
}

long long __builtin_ax_msfhr(long long s1, long long s2, int s3)
{
    long long t0 = __builtin_ax_msfh(s1,s2,s3)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_msss(long long s1, int s2, int s3)
{
    long long t0 = EXT64(s2, 32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1);
    long long t3 = s1 - t2 ;
    return t3;
}

long long __builtin_ax_msssl(long long s1, long long s2, int s3)
{
    long long t0 = EXT64(s2, 32);
    long long t1 = EXT64(s3, 32);
    long long t2 = (t0*t1);
    long long t3 = s1 - t2 ;
    return t3;
}


//----------------------------------------------------

long long __builtin_ax_shli_r(int s1, unsigned int s2)
{
    long long t0 = EXT64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    return t2;
}

long long __builtin_ax_shri_r(int s1, unsigned int s2)
{
    long long t0 = EXT64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    return t2;
}

unsigned long long __builtin_ax_shliu_r(unsigned int s1, unsigned int s2)
{
    unsigned long long t0 = EXTU64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    unsigned long long t2 = t0 <<t1 ;
    return t2;
}

unsigned long long __builtin_ax_shriu_r(unsigned int s1, unsigned int s2)
{
    unsigned long long t0 = EXTU64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    unsigned long long t2 = t0 >>t1 ;
    return t2;
}

long long __builtin_ax_shli_i6(int s1, unsigned short s2)
{
    long long t0 = EXT64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 <<t1 ;
    return t2;
}

long long __builtin_ax_shri_i6(int s1, unsigned short s2)
{
    long long t0 = EXT64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    long long t2 = t0 >>t1 ;
    return t2;
}

unsigned long long __builtin_ax_shliu_i6(unsigned int s1, unsigned short s2)
{
    unsigned long long t0 = EXTU64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    unsigned long long t2 = t0 <<t1 ;
    return t2;
}

unsigned long long __builtin_ax_shriu_i6(unsigned int s1, unsigned short s2)
{
    unsigned long long t0 = EXTU64(s1, 32) ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 6)  ;		 // we keep only 6 lower bits
    unsigned long long t2 = t0 >>t1 ;
    return t2;
}

//----------------------------------------------------

int __builtin_ax_m2rl(long long s)
{
    long long t0 = EXT64(s, 32);
    return (int)t0;
}

	
int __builtin_ax_m2rh(long long s)	
{
    long long t0 = s >> 32 ;
    return (int)t0;
}

//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------


long long __builtin_ax_addhl(long long s1, int s2, int s3)
{
    long long t0 = s1 ;
    unsigned long long t1 = EXT64(s2, 32) << 32 ;
    unsigned long long t2 = EXTU64(s3, 32)  ;
	long long t3 = (long long)(t1 |t2) ;
    long long t4 = t0 + t3 ;
    return t4 ;
}

long long __builtin_ax_subhl(long long s1, int s2, int s3)
{
    long long t0 = s1 ;
    unsigned long long t1 = EXT64(s2, 32) << 32 ;
    unsigned long long t2 = EXTU64(s3, 32)  ;
	long long t3 = (long long)(t1 |t2) ;
    long long t4 = t0 - t3 ;
    return t4 ;
}

long long __builtin_ax_addhlr(long long s1, int s2, int s3)
{
    long long t0 = __builtin_ax_addhl(s1,s2,s3)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_subhlr(long long s1, int s2, int s3)
{
    long long t0 = __builtin_ax_subhl(s1,s2,s3)  ;
    long long t1 = (t0 + 0x080000000LL)   ;	  //round(1)
    long long t2 = (t1 >> 32) << 32   ;		  //round(2)
	return t2 ;
}

long long __builtin_ax_clr(long long s1)
{
 s1 = 0x00LL	;
 return s1 ;
}


#endif

