/*
 *      Copyright 2006, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version   ST/HPC/STS (2006/Oct/17)
 */

#ifdef __X3

#error "builtins_model_X3 must not be used on X3 Core"

#else

#include "builtins_model_common.h"

#ifndef EXTERNAL_TIMING_BUILTINS_IMPLEMENTATION
void __cmodel_x3_clrcc(void)	 {}	
void __cmodel_x3_startcc(void)	 {}	
void __cmodel_x3_stopcc(void)	 {}	
void __cmodel_x3_clrcc1(void)	 {} 
void __cmodel_x3_startcc1(void) {}  
void __cmodel_x3_stopcc1(void)	 {}	
void __cmodel_x3_clrcca(void)	 {} 
void __cmodel_x3_startcca(void) {} 
void __cmodel_x3_stopcca(void)	 {}	
#endif	
						 
void __cmodel_x3_terminate(void){}  	
void __cmodel_x3_ptouch(int x)  {}	
void __cmodel_x3_pinval(int x)  {}	
void __cmodel_x3_pinvalr(int x) {}	
void __cmodel_x3_plock(int x)	 {} 	
void __cmodel_x3_plockr(int x)	 {}	
void __cmodel_x3_pflushw(void)  {}	
void __cmodel_x3_dflush(int x)  {}	
void __cmodel_x3_dflushw(void)	 {}	
void __cmodel_x3_dtouch(int x)  {}	
void __cmodel_x3_dlock(int x )	 {}	
void __cmodel_x3_dinval(int x)  {}	
void __cmodel_x3_pstata(int x)  {}	
void __cmodel_x3_pstatwi(int x) {}	
void __cmodel_x3_dstata(int x)	 {}	
void __cmodel_x3_dstatwi(int x) {}	


/* MULTIPLICATION */

int __cmodel_x3_mpssll(int s1, int s2)
{
    int t0 = EXT32(s1,16);
    int t1 = EXT32(s2,16);
	int t2 = t0*t1 ;
    return t2;	 
}
int __cmodel_x3_mpsslh(int s1, int s2)
{
    int t0 = EXT32(s1,16);
    int t1 = s2>>16;
	int t2 = t0*t1 ;
    return t2;	 
}
int __cmodel_x3_mpsshl(int s1, int s2)
{
    int t0 = s1>>16;
    int t1 = EXT32(s2,16);
	int t2 = t0*t1 ;
    return t2;	 
}
int __cmodel_x3_mpsshh(int s1, int s2)
{
    int t0 = s1>>16;
    int t1 = s2>>16;
	int t2 = t0*t1 ;
    return t2;	 
}
//------------------------------------------------------

int __cmodel_x3_mpuull(unsigned int s1, unsigned int s2)
{
    unsigned int t0 = EXTU32(s1,16);
    unsigned int t1 = EXTU32(s2,16);
	unsigned int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpuulh(unsigned int s1, unsigned int s2)
{
    unsigned int t0 = EXTU32(s1,16);
    unsigned int t1 = s2>>16;
	unsigned int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpuuhl(unsigned int s1, unsigned int s2)
{
    unsigned int t0 = s1>>16;
    unsigned int t1 = EXTU32(s2,16);
	unsigned int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpuuhh(unsigned int s1, unsigned int s2)
{
    unsigned int t0 = s1>>16;
    unsigned int t1 = s2>>16;
	unsigned int t2 = t0*t1 ;
    return (int) t2;	 
}
//------------------------------------------------------
int __cmodel_x3_mpusll(unsigned int s1, int s2)
{
    unsigned int t0 = EXTU32(s1,16);
    int t1 = EXT32(s2,16);
	int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpuslh(unsigned int s1, int s2)
{
    unsigned int t0 = EXTU32(s1,16);
    int t1 = s2>>16;
	int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpushl(unsigned int s1, int s2)
{
    unsigned int t0 = s1>>16;
    int t1 = EXT32(s2,16);
	int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpushh(unsigned int s1, int s2)
{
    unsigned int t0 = s1>>16;
    int t1 = s2>>16;
	int t2 = t0*t1 ;
    return (int) t2;	 
}

//------------------------------------------------------
int __cmodel_x3_mpsull(int s1, unsigned int s2)
{
    int t0 = EXT32(s1,16);
    unsigned int t1 = EXTU32(s2,16);
	int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpsulh(int s1, unsigned int s2)
{
    int t0 = EXT32(s1,16);
    unsigned int t1 = s2>>16;
	int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpsuhl(int s1, unsigned int s2)
{
    int t0 = s1>>16;
    unsigned int t1 = EXTU32(s2,16);
	int t2 = t0*t1 ;
    return (int) t2;	 
}
int __cmodel_x3_mpsuhh(int s1, unsigned int s2)
{
    int t0 = s1>>16;
    unsigned int t1 = s2>>16;
	int t2 = t0*t1 ;
    return (int) t2;	 
}

#endif	/* __X3	*/
