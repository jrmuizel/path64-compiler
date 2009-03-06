/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version  ST/HPC/STS (2006/Oct/17)
 */

#ifdef __SX

#error "builtins_model_sx must not be used on SX Core"

#else

#include "builtins_model_common.h"

//*****************************************************
//************** BUILTIN EMPTY *************************
//*****************************************************

void __cmodel_sx_barrier(void) {} ;	
void __cmodel_sx_nop(void)	    {} ;	
void __cmodel_sx_bkp(void)	    {} ;
void __cmodel_sx_rte(void)	    {} ;


//*****************************************************
//************** BSET/BCLR ... *************************
//*****************************************************

int __cmodel_sx_bclr_r(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = (unsigned int)1<<t1 ;
    unsigned int t3 = t0 & ~t2 ;
    return (int)t3;
} 

int __cmodel_sx_bset_r(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)s1				;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)	;	 // we keep only 5 lower bits
    unsigned int t2 = (unsigned int)1<<t1 ;
    unsigned int t3 = t0 | t2 ;
    return (int)t3;
} 

int __cmodel_sx_bnot_r(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = (unsigned int)1<<t1 ;
    unsigned int t3 = t0 ^ t2 ;
    return (int)t3;
}  

int __cmodel_sx_bclr_i5(int s1, unsigned short s2)
{ 
    unsigned int t0 = (unsigned int)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = (unsigned int)1<<t1 ;
    unsigned int t3 = t0 & ~t2 ;
    return (int)t3;
} 

int __cmodel_sx_bset_i5(int s1, unsigned short s2)
{ 
    unsigned int t0 = (unsigned int)s1				;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)	;	 // we keep only 5 lower bits
    unsigned int t2 = (unsigned int)1<<t1 ;
    unsigned int t3 = t0 | t2 ;
    return (int)t3;
} 

int __cmodel_sx_bnot_i5(int s1, unsigned short s2)
{ 
    unsigned int t0 = (unsigned int)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = (unsigned int)1<<t1 ;
    unsigned int t3 = t0 ^ t2 ;
    return (int)t3;
} 
 
//*****************************************************
//**************     MAX/MIN  *************************
//*****************************************************
int __cmodel_sx_max_r(int s1, int s2)
{
    if (s1 > s2)
    	return s1;
	else 
    	return s2;

}
	
//----------------------------------------------------
unsigned int __cmodel_sx_maxu_r(unsigned int s1, unsigned int s2)
{
    if (s1 > s2)
    	return s1;
	else 
    	return s2;

}	
//----------------------------------------------------
int __cmodel_sx_min_r(int s1, int s2)
{
    if (s1 < s2)
    	return s1;
	else 
    	return s2;

}
	
//----------------------------------------------------
unsigned int __cmodel_sx_minu_r(unsigned int s1, unsigned int s2)
{
    if (s1 < s2)
    	return s1;
	else 
    	return s2;

}	
//----------------------------------------------------
int __cmodel_sx_max_i8(int s1, unsigned short s2)
{
    int t0 = s1;
    int t1 = EXTU32(s2, 8);
    if (t0 > t1)
    	return t0;
	else 
    	return t1;
}
//----------------------------------------------------
unsigned int __cmodel_sx_maxu_i8(unsigned int s1, unsigned short s2)
{
    unsigned int t0 = s1;
    unsigned int t1 = EXTU32(s2, 8);
    if (t0 > t1)
    	return t0;
	else 
    	return t1;
}
//----------------------------------------------------
int __cmodel_sx_min_i8(int s1, unsigned short s2)
{
    int t0 = s1;
    int t1 = EXTU32(s2, 8);
    if (t0 < t1)
    	return t0;
	else 
    	return t1;
}
//----------------------------------------------------
unsigned int __cmodel_sx_minu_i8(unsigned int s1, unsigned short s2)
{
    unsigned int t0 = s1;
    unsigned int t1 = EXTU32(s2, 8);
    if (t0 < t1)
    	return t0;
	else 
    	return t1;
}

//*****************************************************
//***************    ADD/SUB  *************************
//*****************************************************

int __cmodel_sx_add_r(int s1, int s2)
{
    long long t0 = EXTU34(s1, 32);
    long long t1 = EXTU34(s2, 32);
    long long t2 = t0 + t1;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    _isCARRY(t2);
    return t3;

}	

int __cmodel_sx_sub_r(int s1, int s2)
{
    long long t0 = EXTU34(s1, 32);
    long long t1 = EXTU34(~s2, 32);
    long long t2 = t0 + t1 +1;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    _isCARRY(t2);
    return t3;

}	

unsigned int __cmodel_sx_addu_r(unsigned int s1, unsigned int s2)
{
    unsigned long long t0 = EXTU34(s1, 32);
    unsigned long long t1 = EXTU34(s2, 32);
    unsigned long long t2 = t0 + t1;
    unsigned long long t3 = EXTU64(t2, 32);
    _isCARRY(t2);
    return (unsigned int)t3;

}	

unsigned int __cmodel_sx_subu_r(unsigned int s1, unsigned int s2)
{
    unsigned long long t0 = EXTU34(s1, 32);
    unsigned long long t1 = EXTU34(~s2, 32);
    unsigned long long t2 = t0 + t1 +1;
    unsigned long long t3 = EXTU64(t2, 32);
    _isCARRY(t2);
    return (unsigned int)t3;

}	

int __cmodel_sx_add_i8(int s1, unsigned short s2)
{
    long long t0 = EXTU34(s1, 32);
    long long t1 = EXTU34(s2, 8);
    long long t2 = t0 + t1;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    _isCARRY(t2);
    return t3;
}	


int __cmodel_sx_sub_i8(int s1, unsigned short s2)
{
    long long t0 = EXTU34(s1, 32);
    long long t1 = EXTU34(~EXTU64(s2, 8),32);
    long long t2 = t0 + t1 +1;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    _isCARRY(t2);
    return t3;
}
	
unsigned int __cmodel_sx_addu_i8(unsigned int s1, unsigned short s2)
{
    unsigned long long t0 = EXTU34(s1, 32);
    unsigned long long t1 = EXTU34(s2, 8);
    unsigned long long t2 = t0 + t1;
    unsigned long long t3 = EXTU64(t2, 32);
    _isCARRY(t2);
    return (unsigned int) t3;
}	


unsigned int __cmodel_sx_subu_i8(unsigned int s1, unsigned short s2)
{
    unsigned long long t0 = EXTU34(s1, 32);
    unsigned long long t1 = EXTU34(~EXTU64(s2, 8),32);
    unsigned long long t2 = t0 + t1 +1;
    unsigned long long t3 = EXTU64(t2, 32);
    _isCARRY(t2);
    return (unsigned int) t3;
}	
//*****************************************************
//******************  AND/OR  *************************
//*****************************************************

int __cmodel_sx_and_r(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 & t1 ;
    return (int)t2;
} 

int __cmodel_sx_and_i8(int s1, unsigned short s2)
{ 
    unsigned int t0 = (unsigned int)s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 & t1 ;
    return (int)t2;
}
 
int __cmodel_sx_andn(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)~s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 & t1 ;
    return (int)t2;
}
 
int __cmodel_sx_xor_r(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 ^ t1 ;
    return (int)t2;
} 

int __cmodel_sx_xor_i8(int s1, unsigned short s2)
{ 
    unsigned int t0 = (unsigned int)s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 ^ t1 ;
    return (int)t2;
}

int __cmodel_sx_or_r(int s1, int s2)
{ 
    unsigned int t0 = (unsigned int)s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 | t1 ;
    return (int)t2;
} 

int __cmodel_sx_or_i8(int s1, unsigned short s2)
{ 
    unsigned int t0 = (unsigned int)s1	;
    unsigned int t1 = (unsigned int)s2  ;
    unsigned int t2 = t0 | t1 ;
    return (int)t2;
}

int __cmodel_sx_norn(int s1, unsigned short s2)
{ 
    unsigned int t0 = s1	;
    unsigned int t1 = EXTU32(s2,8)  ;
    unsigned int t2 = t0 & (~t1) ;
    return (int)t2;
}

//*****************************************************
//******************  SHIFT     ***********************
//*****************************************************

int __cmodel_sx_shl_r(int s1, int s2)
{ 
    long long t0 = (long long)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    long long t2 = t0 <<t1 ;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    return (int)t3;
} 

int __cmodel_sx_shl_i5(int s1, unsigned short s2)
{ 
    long long t0 = (long long)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    long long t2 = t0 <<t1 ;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    return (int)t3;
} 

int __cmodel_sx_shr_r(int s1, int s2)
{ 
    long long t0 = (long long)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    long long t2 = t0 >>t1 ;
    long long t3 = EXT64(t2, 32);
    return (int)t3;
} 

int __cmodel_sx_shr_i5(int s1, unsigned short s2)
{ 
    long long t0 = (long long)s1			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    long long t2 = t0 >>t1 ;
    long long t3 = EXT64(t2, 32);
    return (int)t3;
} 

unsigned int __cmodel_sx_shlu_r(unsigned int s1, unsigned int s2)
{ 
    unsigned long long t0 = EXTU64(s1,32)			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned long long t2 = t0 <<t1 ;
    unsigned long long t3 = EXTU64(t2, 32);
    return (unsigned int)t3;
} 

unsigned int __cmodel_sx_shlu_i5(unsigned int s1, unsigned short s2)
{ 
    unsigned long long t0 = EXTU64(s1,32)			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned long long t2 = t0 <<t1 ;
    unsigned long long t3 = EXTU64(t2, 32);
    return (unsigned int)t3;
} 

unsigned int __cmodel_sx_shru_r(unsigned int s1, unsigned int s2)
{ 
    unsigned long long t0 = EXTU64(s1,32)			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned long long t2 = t0 >>t1 ;
    unsigned long long t3 = EXTU64(t2, 32);
    return (unsigned int)t3;
} 

unsigned int __cmodel_sx_shru_i5(unsigned int s1, unsigned short s2)
{ 
    unsigned long long t0 = EXTU64(s1,32)			   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned long long t2 = t0 >>t1 ;
    unsigned long long t3 = EXTU64(t2, 32);
    return (unsigned int)t3;
}
 
int __cmodel_sx_rotl_r(int s1, int s2)
{ 
    unsigned long long t0 = EXTU64(s1, 32)	;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned long long t2 = t0 << t1 ;
    unsigned long long t3 = t0 >> (32-t1) ;
    long long t4 = EXT64(t2|t3, 32);
    return (int)t4;
}
 
int __cmodel_sx_rotl_i5(int s1, unsigned short s2)
{ 
    unsigned long long t0 = EXTU64(s1, 32)	;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    unsigned long long t2 = t0 << t1 ;
    unsigned long long t3 = t0 >> (32-t1) ;
    long long t4 = EXT64(t2|t3, 32);
    return (int)t4;
} 

unsigned int __cmodel_sx_shlnu(unsigned int s1, unsigned int s2)
{ 
    unsigned long long t0 = (unsigned long long)s1	   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    if (t1 != 0)
    {			 
    	unsigned long long t2 = t0 << (32-t1) ;
    	unsigned long long t3 = EXT64(t2, 32);
    	return (unsigned int)t3;
	}
	else
	{
    	return s1;
	}
}

unsigned int __cmodel_sx_shrnu(unsigned int s1, unsigned int s2)
{ 
    unsigned long long t0 = (unsigned long long)s1	   ;
    unsigned int t1 = (unsigned int)EXTU32(s2, 5)  ;		 // we keep only 5 lower bits
    if (t1 != 0)
    {			 
    	unsigned long long t2 = t0 >> (32-t1) ;
    	unsigned long long t3 = EXT64(t2, 32);
    	return (unsigned int)t3;
	}
	else
	{
    	return s1;
	}
}

unsigned int __cmodel_sx_shlmu_r(unsigned int s)
{
    unsigned int t0 = 0xFFFFFFFF      ;
    unsigned int t1 = (unsigned int)EXTU32(s, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = t0 << t1 ;
    return t2;
}

unsigned int __cmodel_sx_shrmu_r(unsigned int s)
{
    unsigned int t0 = 0xFFFFFFFF      ;
    unsigned int t1 = (unsigned int)EXTU32(s, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = EXTU32(t0 >>t1,32) ;
    return t2;
}

unsigned int __cmodel_sx_shlmu_i5(unsigned short s)
{
    unsigned int t0 = 0xFFFFFFFF      ;
    unsigned int t1 = (unsigned int)EXTU32(s, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = t0 << t1 ;
    return t2;
}

unsigned int __cmodel_sx_shrmu_i5(unsigned short s)
{
    unsigned int t0 = 0xFFFFFFFF      ;
    unsigned int t1 = (unsigned int)EXTU32(s, 5)  ;		 // we keep only 5 lower bits
    unsigned int t2 = EXTU32(t0 >>t1,32) ;
    return t2;
}








//*****************************************************
//********************  ABS/CLAMP ... *****************
//*****************************************************

unsigned int __cmodel_sx_abs(int s)
{
    long long t0 = EXT64(s, 32);
    long long t2 = abs(t0);
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    return t3;
 
}

unsigned int __cmodel_sx_absu(int s)
{
    long long t0 = EXT64(s, 32);
    unsigned long long t2 = abs(t0);
    unsigned long long t3 = EXTU64(t2, 32);
    return t3;
 
}

int __cmodel_sx_prior(int s)
{
    long long t0 = EXT64(s, 32);
    int i = 32 - 1, prior = 32 - 32;
    while (i && (~(t0>>i)^(t0>>(i-1)))&1) i--, prior++;
    return EXT32(prior, 32);
}


int __cmodel_sx_lzc(int s)
{
    unsigned i = 32-1, lzcnt = 0;
    unsigned long long t0 = EXTU64(s, 32);
    while (i+1 && !((t0>>i)&1))  
    { 
		lzcnt++, i--;
    }
    return EXTU32(lzcnt, 32);
}


int __cmodel_sx_clamph(int s)
{
    int t0 = s;
    int t1 = CLAMP32(t0, 16) ;
    int t2 = EXT32(t1, 16)   ;
	return t2 ;
}

int __cmodel_sx_extb(int s)
{
    int t0 = s & 0xFF;
    int t1 = EXT32(t0, 8);
    return t1;
}

int __cmodel_sx_exth(int s)
{
    int t0 = s & 0xFFFF;
    int t1 = EXT32(t0, 16);
    return t1;
}

unsigned int __cmodel_sx_extub(int s)
{
    unsigned int t0 = s & 0xFF;
    unsigned int t1 = EXTU32(t0, 8);
    return t1;
}

unsigned int __cmodel_sx_extuh(int s)
{
    unsigned int t0 = s & 0xFFFF;
    unsigned int t1 = EXTU32(t0, 16);
    return t1;
}

//*****************************************************
//********************   NEG/NOT      *****************
//*****************************************************

int __cmodel_sx_neg(int s)
{
    long long t0 = EXT64(s, 32);
    long long t2 = - t0;
    long long t3 = EXT64(t2, 32);
    _isOVFLOW(EXT64(t2,33),t3);
    return t3;
}

int __cmodel_sx_negu(int s) 
{
    long long t0 = EXT64(s, 32);
    long long t2 = - t0;
    long long t3 = EXT64(t2, 32);
    return t3;
}

int __cmodel_sx_not(int s)
{
    long long t0 = EXT64(s, 32);
    long long t2 = ~t0;
    long long t3 = EXT64(t2, 32);
    return t3;
}

//*****************************************************
//********************   MAKE/MORE    *****************
//*****************************************************

int __cmodel_sx_make(short s)
{
    int t0 = EXT32(s, 16);
	return t0 ;
}

int __cmodel_sx_more(int s1, unsigned int s2)
{
    int t0 = s1 << 16;
    int t1 = EXTU32(s2, 16);
	int t2 = t0 | t1 ;
	return t2 ;
}

//*****************************************************
//********************   MOVE2         *****************
//*****************************************************

int __cmodel_sx_moveh2h(int s1, int s2)
{
//Stores the 16-bit High Half-Word into the 16-bit High Half-Word of register Rm;
    int t0 = EXTU32(s1, 16);
    int t1 = (s2>>16) << 16;
	int t2 = t0 | t1 ;
	return t2 ;
}

int __cmodel_sx_moveh2l(int s1, int s2)
{
//Stores the 16-bit High Half-Word into the 16-bit Low Half-Word of register Rm;
    int t0 = (s1>>16) << 16;
    unsigned int t1 = (unsigned int) s2 >> 16;
	int t2 = t0 | t1 ;
	return t2 ;
}

int __cmodel_sx_movel2h(int s1, int s2)
{
//Stores the 16-bit Low Half-Word into the 16-bit High Half-Word of register Rm;
    int t0 = EXTU32(s1, 16);
    int t1 = EXT32(s2, 16) << 16;
	int t2 = t0 | t1 ;
	return t2 ;
}


int __cmodel_sx_movel2l(int s1, int s2)
{
//Stores the 16-bit Low Half-Word into the 16-bit Low Half-Word of register Rm
    int t0 = (s1>>16) << 16;
    int t1 = EXTU32(s2, 16);
	int t2 = t0 | t1 ;
	return t2 ;
}









//*****************************************************
//*****************************************************
//********************   PACKED       *****************
//*****************************************************
//*****************************************************
//*****************************************************

//*****************************************************
//******************** PACKED  ABS    *****************
//*****************************************************

unsigned int __cmodel_sx_absbp(int s)
{
 
    int t0_b0 = EXT32(s, 8);
    int t1_b0 = abs(t0_b0);
    int t2_b0 = EXT32(t1_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s_b1  = (s & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s_b1, 8);
    int t1_b1 = abs(t0_b1);
    int t2_b1 = EXT32(t1_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s_b2  = (s & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s_b2, 8);
    int t1_b2 = abs(t0_b2);
    int t2_b2 = EXT32(t1_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s_b3  = (s & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s_b3, 8);
    int t1_b3 = abs(t0_b3);
    int t2_b3 = EXT32(t1_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;
	
    _isOVFLOW(EXT32(t1_b0,9),t2_b0);
    _isOVFLOW(EXT32(t1_b1,9),t2_b1);
    _isOVFLOW(EXT32(t1_b2,9),t2_b2);
    _isOVFLOW(EXT32(t1_b3,9),t2_b3);

    return t3;
 
}

unsigned int __cmodel_sx_abshp(int s)
{
    int t0_h0 = EXT32(s, 16);
    int t1_h0 = abs(t0_h0);
    int t2_h0 = EXT32(t1_h0, 16);
    unsigned int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);

    int s_h1  = (s & 0xFFFF0000)  >> 16 ;
    int t0_h1 = EXT32(s_h1, 16);
    int t1_h1 = abs(t0_h1);
    int t2_h1 = EXT32(t1_h1, 16);
    unsigned int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);


	unsigned int t3 =  (unsigned int)( t3_h1 | t3_h0)  ;

    _isOVFLOW(EXT32(t1_h0,17),t2_h0);
    _isOVFLOW(EXT32(t1_h1,17),t2_h1);

    return t3;
}

unsigned int __cmodel_sx_absubp(int s)
{
    int t0_b0 = EXT32(s, 8);
    unsigned int t1_b0 = abs(t0_b0);
    unsigned int t2_b0 = EXT32(t1_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s_b1  = (s & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s_b1, 8);
    unsigned int t1_b1 = abs(t0_b1);
    unsigned int t2_b1 = EXT32(t1_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s_b2  = (s & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s_b2, 8);
    unsigned int t1_b2 = abs(t0_b2);
    unsigned int t2_b2 = EXT32(t1_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s_b3  = (s & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s_b3, 8);
    unsigned int t1_b3 = abs(t0_b3);
    unsigned int t2_b3 = EXT32(t1_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return t3;
 
}
unsigned int __cmodel_sx_absuhp(int s)
{
    int t0_h0 = EXT32(s, 16);
    unsigned int t1_h0 = abs(t0_h0);
    unsigned int t2_h0 = (unsigned int) (t1_h0 & 0x0000FFFF);

    int s_h1  = (s & 0xFFFF0000)  >> 16 ;
    int t0_h1 = EXT32(s_h1, 16);
    unsigned int t1_h1 = abs(t0_h1);
    unsigned int t2_h1 = (unsigned int) ((t1_h1<<16) & 0xFFFF0000);


	unsigned int t2 =  (unsigned int)( t2_h1 | t2_h0)  ;

    return t2;
}
//*****************************************************
//******************* PACKED ADD/SUB   ****************
//*****************************************************


int __cmodel_sx_addhp_i8(int s1, unsigned short s2)
{ 
    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXTU32(s2, 8);
    int t2_h0 = t0_h0 + t1_h0;
    int t2_0 = EXT32(t2_h0, 16);
    int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = t1_h0  ;
    int t2_h1 = t0_h1 + t1_h1;
    int t2_1 = EXT32(t2_h1, 16);
    int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;

    _isOVFLOW(EXT32(t2_h0,17),t2_0);
    _isOVFLOW(EXT32(t2_h1,17),t2_1);

    return t3;
}


unsigned int __cmodel_sx_adduhp_i8(unsigned int s1, unsigned short s2)
{ 
    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 8);
    unsigned int t2_h0 = t0_h0 + t1_h0;
    unsigned int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = t1_h0  ;
    unsigned int t2_h1 = t0_h1 + t1_h1;
    unsigned int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	unsigned int t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return (unsigned int) t3;
}


int __cmodel_sx_subhp_i8(int s1, unsigned short s2)
{ 
    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXTU32(s2, 8);
    int t2_h0 = t0_h0 - t1_h0;
    int t2_0 = EXT32(t2_h0, 16);
    int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = t1_h0  ;
    int t2_h1 = t0_h1 - t1_h1;
    int t2_1 = EXT32(t2_h1, 16);
    int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;

    _isOVFLOW(EXT32(t2_h0,17),t2_0);
    _isOVFLOW(EXT32(t2_h1,17),t2_1);

    return t3;
}

int __cmodel_sx_subuhp_i8(int s1, unsigned short s2)
{ 
    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 8);
    unsigned int t2_h0 = t0_h0 - t1_h0;
    unsigned int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = t1_h0  ;
    unsigned int t2_h1 = t0_h1 - t1_h1;
    unsigned int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	unsigned int t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return (int) t3;
}

int __cmodel_sx_addhp_r(int s1, int s2)
{ 
    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXT32(s2, 16);
    int t2_h0 = t0_h0 + t1_h0;
    int t2_0 = EXT32(t2_h0, 16);
    int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int h2  = (s2 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = EXT32(h2, 16)  ;
    int t2_h1 = t0_h1 + t1_h1;
    int t2_1 = EXT32(t2_h1, 16);
    int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;

    _isOVFLOW(EXT32(t2_h0,17),t2_0);
    _isOVFLOW(EXT32(t2_h1,17),t2_1);
    
    return t3;
}


unsigned int __cmodel_sx_adduhp_r(unsigned int s1, unsigned int s2)
{ 
    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 16);
    unsigned int t2_h0 = t0_h0 + t1_h0;
    unsigned int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int h2  = (s2 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = EXTU32(h2, 16)  ;
    unsigned int t2_h1 = t0_h1 + t1_h1;
    unsigned int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	unsigned int t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return t3;
}



int __cmodel_sx_subhp_r(int s1, int s2)
{ 
    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXT32(s2, 16);
    int t2_h0 = t0_h0 - t1_h0;
    int t2_0 = EXT32(t2_h0, 16);
    int t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int h2  = (s2 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = EXT32(h2, 16);
    int t2_h1 = t0_h1 - t1_h1;
    int t2_1 = EXT32(t2_h1, 16);
    int t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;

    _isOVFLOW(EXT32(t2_h0,17),t2_0);
    _isOVFLOW(EXT32(t2_h1,17),t2_1);

    return t3;
}

int __cmodel_sx_subuhp_r(int s1, int s2)
{ 
    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXT32(s2, 16);
    int t2_h0 = t0_h0 - t1_h0;
    int t3_h0 = (int) (t2_h0 & 0x0000FFFF);


    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int h2  = (s2 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = EXT32(h2, 16)  ;
    int t2_h1 = t0_h1 - t1_h1;
    int t3_h1 = (int) ((t2_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;

    return t3;
}




int __cmodel_sx_addbp_i8(int s1, unsigned short s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXTU32(s2, 8);
    int t2_b0 = t0_b0 + t1_b0;
    int t2_0 = EXT32(t2_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t1_b1 = t1_b0  ;
    int t2_b1 = t0_b1 + t1_b1;
    int t2_1 = EXT32(t2_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t1_b2 = t1_b0 ;
    int t2_b2 = t0_b2 + t1_b2;
    int t2_2 = EXT32(t2_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t1_b3 = t1_b0  ;
    int t2_b3 = t0_b3 + t1_b3;
    int t2_3 = EXT32(t2_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    _isOVFLOW(EXT32(t2_b0,9),t2_0);
    _isOVFLOW(EXT32(t2_b1,9),t2_1);
    _isOVFLOW(EXT32(t2_b2,9),t2_2);
    _isOVFLOW(EXT32(t2_b3,9),t2_3);

    return t3;	 
}

unsigned int __cmodel_sx_addubp_i8(int s1, unsigned short s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t0_b0 + t1_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    unsigned int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8)  ;
    unsigned int t1_b1 = t1_b0  ;
    unsigned int t2_b1 = t0_b1 + t1_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8) ;
    unsigned int t1_b2 = t1_b0 ;
    unsigned int t2_b2 = t0_b2 + t1_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8)  ;
    unsigned int t1_b3 = t1_b0  ;
    unsigned int t2_b3 = t0_b3 + t1_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return (unsigned int) t3;
}

int __cmodel_sx_subbp_i8(int s1, unsigned short s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t0_b0 - t1_b0;
    int t2_0 = EXT32(t2_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t1_b1 = t1_b0  ;
    int t2_b1 = t0_b1 - t1_b1;
    int t2_1 = EXT32(t2_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t1_b2 = t1_b0 ;
    int t2_b2 = t0_b2 - t1_b2;
    int t2_2 = EXT32(t2_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t1_b3 = t1_b0  ;
    int t2_b3 = t0_b3 - t1_b3;
    int t2_3 = EXT32(t2_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    _isOVFLOW(EXT32(t2_b0,9),t2_0);
    _isOVFLOW(EXT32(t2_b1,9),t2_1);
    _isOVFLOW(EXT32(t2_b2,9),t2_2);
    _isOVFLOW(EXT32(t2_b3,9),t2_3);

    return t3;	 
}

int __cmodel_sx_sububp_i8(int s1, unsigned short s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t0_b0 - t1_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    unsigned int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8)  ;
    unsigned int t1_b1 = t1_b0  ;
    unsigned int t2_b1 = t0_b1 - t1_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8) ;
    unsigned int t1_b2 = t1_b0 ;
    unsigned int t2_b2 = t0_b2 - t1_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8)  ;
    unsigned int t1_b3 = t1_b0  ;
    unsigned int t2_b3 = t0_b3 - t1_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return (int) t3;
}

int __cmodel_sx_addbp_r(int s1, int s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t0_b0 + t1_b0;
    int t2_0 = EXT32(t2_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t1_b1 = EXT32(s2_b1, 8)  ;
    int t2_b1 = t0_b1 + t1_b1;
    int t2_1 = EXT32(t2_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t1_b2 = EXT32(s2_b2, 8) ;
    int t2_b2 = t0_b2 + t1_b2;
    int t2_2 = EXT32(t2_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t1_b3 = EXT32(s2_b3, 8)  ;
    int t2_b3 = t0_b3 + t1_b3;
    int t2_3 = EXT32(t2_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    _isOVFLOW(EXT32(t2_b0,9),t2_0);
    _isOVFLOW(EXT32(t2_b1,9),t2_1);
    _isOVFLOW(EXT32(t2_b2,9),t2_2);
    _isOVFLOW(EXT32(t2_b3,9),t2_3);

    return t3;	 
}

unsigned int __cmodel_sx_addubp_r(unsigned int s1, unsigned int s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t0_b0 + t1_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    unsigned int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    unsigned int s2_b1  = (s2 & 0x0000FF00)  >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8)  ;
    unsigned int t1_b1 = EXTU32(s2_b1, 8)  ;
    unsigned int t2_b1 = t0_b1 + t1_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8) ;
    unsigned int t1_b2 = EXTU32(s2_b2, 8) ;
    unsigned int t2_b2 = t0_b2 + t1_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8)  ;
    unsigned int t1_b3 = EXTU32(s2_b3, 8)  ;
    unsigned int t2_b3 = t0_b3 + t1_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return t3;	 
}

int __cmodel_sx_subbp_r(int s1, int s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t0_b0 - t1_b0;
    int t2_0 = EXT32(t2_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t1_b1 = EXT32(s2_b1, 8)  ;
    int t2_b1 = t0_b1 - t1_b1;
    int t2_1 = EXT32(t2_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t1_b2 = EXT32(s2_b2, 8) ;
    int t2_b2 = t0_b2 - t1_b2;
    int t2_2 = EXT32(t2_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t1_b3 = EXT32(s2_b3, 8)  ;
    int t2_b3 = t0_b3 - t1_b3;
    int t2_3 = EXT32(t2_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    _isOVFLOW(EXT32(t2_b0,9),t2_0);
    _isOVFLOW(EXT32(t2_b1,9),t2_1);
    _isOVFLOW(EXT32(t2_b2,9),t2_2);
    _isOVFLOW(EXT32(t2_b3,9),t2_3);

    return t3;	 
}

int __cmodel_sx_sububp_r(int s1, int s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t0_b0 - t1_b0;
    int t3_b0 = (int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s1 & 0x0000FF00)  >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t1_b1 = EXT32(s2_b1, 8)  ;
    int t2_b1 = t0_b1 - t1_b1;
    int t3_b1 = (int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t1_b2 = EXT32(s2_b2, 8) ;
    int t2_b2 = t0_b2 - t1_b2;
    int t3_b2 = (int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t1_b3 = EXT32(s2_b3, 8)  ;
    int t2_b3 = t0_b3 - t1_b3;
    int t3_b3 = (int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return t3;	 
}


//*****************************************************
//****************** PACKED MAX/MIN   *****************
//*****************************************************


int __cmodel_sx_maxhp_i8(int s1, unsigned short s2)
{
    int t2_h0 ;
    int t2_h1 ;

    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXTU32(s2, 8);

    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = t1_h0  ;

    int t3_h0 ;
    int t3_h1 ;
    int t3 ;
    
    
    if (t0_h0 > t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    if (t0_h1 > t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (int)(t3_h1 | t3_h0)  ;

    return t3;
}




int __cmodel_sx_minhp_i8(int s1, unsigned short s2)
{
    int t2_h0 ;
    int t2_h1 ;

    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXTU32(s2, 8);
    
    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = t1_h0  ;

    int t3_h0 ;
    int t3_h1 ;
    int t3 ;

    if (t0_h0 < t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);

    if (t0_h1 < t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (int)(t3_h1 | t3_h0)  ;

    return t3;
}

unsigned int __cmodel_sx_maxuhp_i8(unsigned int s1, unsigned short s2)
{
    unsigned int t2_h0 ;
    unsigned int t2_h1 ;

    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 8);

    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = t1_h0  ;

    unsigned int t3_h0 ;
    unsigned int t3_h1 ;
    unsigned int t3 ;

    if (t0_h0 > t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);

    if (t0_h1 > t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return t3;
}

unsigned int __cmodel_sx_minuhp_i8(unsigned int s1, unsigned short s2)
{
    unsigned int t2_h0 ;
    unsigned int t2_h1 ;

    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 8);

    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = t1_h0  ;

    unsigned int t3_h0 ;
    unsigned int t3_h1 ;
    unsigned int t3 ;

    if (t0_h0 < t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);

    if (t0_h1 < t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return t3;
}



int __cmodel_sx_maxhp_r(int s1, int s2)
{
    int t2_h0 ;
    int t2_h1 ;

    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXT32(s2, 16);

    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int h2  = (s2 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = EXT32(h2, 16)  ;
    
    int t3_h0 ;
    int t3_h1 ;
    int t3  ;
    
    if (t0_h0 > t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);

    if (t0_h1 > t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (int)(t3_h1 | t3_h0)  ;

    return t3;
}


int __cmodel_sx_minhp_r(int s1, int s2)
{
    int t2_h0 ;
    int t2_h1 ;

    int t0_h0 = EXT32(s1, 16);
    int t1_h0 = EXT32(s2, 16);

    int h1  = (s1 & 0xFFFF0000) >> 16 ;
    int h2  = (s2 & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = EXT32(h2, 16)  ;

    int t3_h0 ;
    int t3_h1 ;     
    int t3 ;     
    
    if (t0_h0 < t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    if (t0_h1 < t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (int)(t3_h1 | t3_h0)  ;

    return t3;
}

unsigned int __cmodel_sx_maxuhp_r(unsigned int s1, unsigned int s2)
{
    unsigned int t2_h0 ;
    unsigned int t2_h1 ;

    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 16);

    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int h2  = (s2 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = EXTU32(h2, 16)  ;

    unsigned int t3_h0 ;
    unsigned int t3_h1 ;
    unsigned int t3 ;        

    if (t0_h0 > t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);


    if (t0_h1 > t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return t3;
}

unsigned int __cmodel_sx_minuhp_r(unsigned int s1, unsigned int s2)
{
    unsigned int t2_h0 ;
    unsigned int t2_h1 ;

    unsigned int t0_h0 = EXTU32(s1, 16);
    unsigned int t1_h0 = EXTU32(s2, 16);

    unsigned int h1  = (s1 & 0xFFFF0000) >> 16 ;
    unsigned int h2  = (s2 & 0xFFFF0000) >> 16 ;
    unsigned int t0_h1 = EXTU32(h1, 16)  ;
    unsigned int t1_h1 = EXTU32(h2, 16)  ;

    unsigned int t3_h0 ;
    unsigned int t3_h1 ;
    unsigned int t3 ;
    
    if (t0_h0 < t1_h0)
    	t2_h0 = t0_h0 ;
	else
    	t2_h0 = t1_h0 ;

    t3_h0 = (unsigned int) (t2_h0 & 0x0000FFFF);

    if (t0_h1 < t1_h1)
    	t2_h1 = t0_h1 ;
	else
    	t2_h1 = t1_h1 ;

    t3_h1 = (unsigned int) ((t2_h1<<16) & 0xFFFF0000);

    t3 =  (unsigned int)(t3_h1 | t3_h0)  ;

    return t3;
}

int __cmodel_sx_maxbp_r(int s1, int s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t1_b0 > t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8);
    int t1_b1 = EXT32(s2_b1, 8);
    int t2_b1 = t1_b1 > t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8);
    int t1_b2 = EXT32(s2_b2, 8);
    int t2_b2 = t1_b2 > t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8);
    int t1_b3 = EXT32(s2_b3, 8);
    int t2_b3 = t1_b3 > t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}	

int __cmodel_sx_minbp_r(int s1, int s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t1_b0 < t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8);
    int t1_b1 = EXT32(s2_b1, 8);
    int t2_b1 = t1_b1 < t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8);
    int t1_b2 = EXT32(s2_b2, 8);
    int t2_b2 = t1_b2 < t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8);
    int t1_b3 = EXT32(s2_b3, 8);
    int t2_b3 = t1_b3 < t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}	

unsigned int __cmodel_sx_maxubp_r(unsigned int s1, unsigned int s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t1_b0 > t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    unsigned int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    unsigned int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8);
    unsigned int t1_b1 = EXTU32(s2_b1, 8);
    unsigned int t2_b1 = t1_b1 > t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8);
    unsigned int t1_b2 = EXTU32(s2_b2, 8);
    unsigned int t2_b2 = t1_b2 > t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8);
    unsigned int t1_b3 = EXTU32(s2_b3, 8);
    unsigned int t2_b3 = t1_b3 > t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}	


unsigned int __cmodel_sx_minubp_r(unsigned int s1, unsigned int s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t1_b0 < t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    unsigned int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    unsigned int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8);
    unsigned int t1_b1 = EXTU32(s2_b1, 8);
    unsigned int t2_b1 = t1_b1 < t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8);
    unsigned int t1_b2 = EXTU32(s2_b2, 8);
    unsigned int t2_b2 = t1_b2 < t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8);
    unsigned int t1_b3 = EXTU32(s2_b3, 8);
    unsigned int t2_b3 = t1_b3 < t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}	


int __cmodel_sx_maxbp_i8(int s1, unsigned short s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t1_b0 > t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8);
    int t1_b1 = t1_b0;
    int t2_b1 = t1_b1 > t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8);
    int t1_b2 = t1_b0;
    int t2_b2 = t1_b2 > t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8);
    int t1_b3 = t1_b0;
    int t2_b3 = t1_b3 > t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}	


int __cmodel_sx_minbp_i8(int s1, unsigned short s2)
{
    int t0_b0 = EXT32(s1, 8);
    int t1_b0 = EXT32(s2, 8);
    int t2_b0 = t1_b0 < t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8);
    int t1_b1 = t1_b0;
    int t2_b1 = t1_b1 < t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8);
    int t1_b2 = t1_b0;
    int t2_b2 = t1_b2 < t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8);
    int t1_b3 = t1_b0;
    int t2_b3 = t1_b3 < t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}
	
unsigned int __cmodel_sx_maxubp_i8(unsigned int s1, unsigned short s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t1_b0 > t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    unsigned int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    unsigned int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8);
    unsigned int t1_b1 = t1_b0;
    unsigned int t2_b1 = t1_b1 > t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8);
    unsigned int t1_b2 = t1_b0;
    unsigned int t2_b2 = t1_b2 > t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8);
    unsigned int t1_b3 = t1_b0;
    unsigned int t2_b3 = t1_b3 > t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}
	
unsigned int __cmodel_sx_minubp_i8(unsigned int s1, unsigned short s2)
{
    unsigned int t0_b0 = EXTU32(s1, 8);
    unsigned int t1_b0 = EXTU32(s2, 8);
    unsigned int t2_b0 = t1_b0 < t0_b0 ? t1_b0 : t0_b0;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);
	
    unsigned int s1_b1  = (s1 & 0x0000FF00) >> 8 ;
    unsigned int s2_b1  = (s2 & 0x0000FF00) >> 8 ;
    unsigned int t0_b1 = EXTU32(s1_b1, 8);
    unsigned int t1_b1 = t1_b0;
    unsigned int t2_b1 = t1_b1 < t0_b1 ? t1_b1 : t0_b1;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);
	
    unsigned int s1_b2  = (s1 & 0x00FF0000) >> 16 ;
    unsigned int s2_b2  = (s2 & 0x00FF0000) >> 16 ;
    unsigned int t0_b2 = EXTU32(s1_b2, 8);
    unsigned int t1_b2 = t1_b0;
    unsigned int t2_b2 = t1_b2 < t0_b2 ? t1_b2 : t0_b2;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);
	
    unsigned int s1_b3  = (s1 & 0xFF000000) >> 24 ;
    unsigned int s2_b3  = (s2 & 0xFF000000) >> 24 ;
    unsigned int t0_b3 = EXTU32(s1_b3, 8);
    unsigned int t1_b3 = t1_b0;
    unsigned int t2_b3 = t1_b3 < t0_b3 ? t1_b3 : t0_b3;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);
	
	
	unsigned int t3 =  (unsigned int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return  t3;
}

//*****************************************************
//********************  PACKED NEG    *****************
//*****************************************************

int __cmodel_sx_neghp(int s)
{
    int t0_h0 = EXT32(s, 16);
    int t1_h0 = - t0_h0;
    int t2_0  = EXT32(t1_h0, 16);
    int t3_h0 = (unsigned int) (t1_h0 & 0x0000FFFF);

    int h1    = (s & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = - t0_h1;
    int t2_1  = EXT32(t1_h1, 16);
    int t3_h1 = (unsigned int) ((t1_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;

    _isOVFLOW(EXT32(t1_h0,17),t2_0);
    _isOVFLOW(EXT32(t1_h1,17),t2_1);

    return t3;
}

int __cmodel_sx_neguhp(int s)
{
    int t0_h0 = EXT32(s, 16);
    int t1_h0 = - t0_h0;
    int t3_h0 = (unsigned int) (t1_h0 & 0x0000FFFF);

    int h1    = (s & 0xFFFF0000) >> 16 ;
    int t0_h1 = EXT32(h1, 16)  ;
    int t1_h1 = - t0_h1;
    int t3_h1 = (unsigned int) ((t1_h1<<16) & 0xFFFF0000);

	int t3 =  (int)(t3_h1 | t3_h0)  ;
    return t3;
}

int __cmodel_sx_negbp(int s)
{
    int t0_b0 = EXT32(s, 8);
    int t2_b0 = - t0_b0 ;
    int t2_0 = EXT32(t2_b0, 8);
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t2_b1 = - t0_b1 ;
    int t2_1 = EXT32(t2_b1, 8);
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t2_b2 = - t0_b2 ;
    int t2_2 = EXT32(t2_b2, 8);
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t2_b3 = - t0_b3 ;
    int t2_3 = EXT32(t2_b3, 8);
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    _isOVFLOW(EXT32(t2_b0,9),t2_0);
    _isOVFLOW(EXT32(t2_b1,9),t2_1);
    _isOVFLOW(EXT32(t2_b2,9),t2_2);
    _isOVFLOW(EXT32(t2_b3,9),t2_3);

    return t3;	 
}

int __cmodel_sx_negubp(int s)
{
    int t0_b0 = EXT32(s, 8);
    int t2_b0 = - t0_b0 ;
    unsigned int t3_b0 = (unsigned int) (t2_b0 & 0x000000FF);

    int s1_b1  = (s & 0x0000FF00)  >> 8 ;
    int t0_b1 = EXT32(s1_b1, 8)  ;
    int t2_b1 = - t0_b1 ;
    unsigned int t3_b1 = (unsigned int) ((t2_b1<<8) & 0x0000FF00);

    int s1_b2  = (s & 0x00FF0000) >> 16 ;
    int t0_b2 = EXT32(s1_b2, 8) ;
    int t2_b2 = - t0_b2 ;
    unsigned int t3_b2 = (unsigned int) ((t2_b2<<16) & 0x00FF0000);

    int s1_b3  = (s & 0xFF000000) >> 24 ;
    int t0_b3 = EXT32(s1_b3, 8)  ;
    int t2_b3 = - t0_b3 ;
    unsigned int t3_b3 = (unsigned int) ((t2_b3<<24) & 0xFF000000);

	int t3 =  (int)( t3_b3 | t3_b2 | t3_b1 | t3_b0)  ;

    return t3;	 
}


#endif
