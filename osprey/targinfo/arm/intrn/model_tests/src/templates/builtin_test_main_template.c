/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/*=======================================================================
 * Project component : 	ST - SX builtin model
 * File name : 	 		test_###BUILTIN_NAME###.c
 * Author : 		 	T.TORRET									   
 * Organization : 		ST/HPC/STS
 *-----------------------------------------------------------------------
 * Purpose : 	###BUILTIN_NAME### Immediate Builtin model test file 
 *-----------------------------------------------------------------------
 * HISTORY:
 *---------
 * Creation by T.TORRET ST/HPC/STS (2005/Apr/19)
 *--------------------------------------------------------------------------------
 * v2.0 T.TORRET (2005/May/26)
 *    new StxP70 arch 1_3 toolset : 2 header files  "builtin_core.h","builtin_AX.h"
 *--------------------------------------------------------------------------------
 * v3.0 T.TORRET (2005/Oct/17)
 *    New Compiler STCXP70cc => nex builtin files
 * Date of builtin file : ###GENERATION_DATE###
 *=======================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __SX_BUILTIN_TEST
#include "sx.h"
#ifdef __SX
#define __CHECK_CARRY__	  1
#endif
#endif

#ifdef  __AX_BUILTIN_TEST
#include "ax.h"
#endif

#ifdef  __X3_BUILTIN_TEST
#include "x3.h"
#endif

#ifdef __SX
extern int getSR();
#endif

#define NB_PARAM_IN    ###INPUT_PARAM_NB###
#define NB_PARAM_OUT   ###OUTPUT_PARAM_NB###

###INPUT_DATA_1_IS_NOT_IMMEDIATE###  ###INPUT_DATA_1_TYPE### 	inp1[###INPUT_DATA_TAB_SIZE###] = { ###INPUT_DATA_1_LIST### } ;
 
###INPUT_DATA_2_IS_NOT_IMMEDIATE###  ###INPUT_DATA_2_TYPE### 	inp2[###INPUT_DATA_TAB_SIZE###] = { ###INPUT_DATA_2_LIST### } ;

###INPUT_DATA_3_IS_NOT_IMMEDIATE###  ###INPUT_DATA_3_TYPE### 	inp3[###INPUT_DATA_TAB_SIZE###] = { ###INPUT_DATA_3_LIST### } ;

###INPUT_DATA_4_IS_NOT_IMMEDIATE###  ###INPUT_DATA_3_TYPE### 	inp4[###INPUT_DATA_TAB_SIZE###] = { ###INPUT_DATA_4_LIST### } ;

 ###OUTPUT_DATA_1_TYPE###   out1  ;

int nb_data_tab	 =	###INPUT_DATA_TAB_SIZE### ;


main ()
{
	 int EOF_Data= 0  ;
	 int index;

	 printf ( "###BUILTIN_NAME###\n")	;
	 printf ( "\t Nb of Input  Parameter: %d\n",NB_PARAM_IN)		;
	 printf ( "\t\t Type of Input  Parameter 1: ###INPUT_DATA_1_TYPE###\n")		;
	 printf ( "\t\t Type of Input  Parameter 2: ###INPUT_DATA_2_TYPE###\n")		;
	 printf ( "\t\t Type of Input  Parameter 3: ###INPUT_DATA_3_TYPE###\n")		;
	 printf ( "\t\t Type of Input  Parameter 4: ###INPUT_DATA_4_TYPE###\n")		;
	 printf ( "\t Nb of Output  Parameter: %d\n",NB_PARAM_OUT)		;
	 printf ( "\t\t Type of Output  Parameter 1: ###OUTPUT_DATA_1_TYPE###\n")		;
	 printf ("----------\n")	;

	 index=0 ;
	 loop :											  
	 ###TAB_BUILTIN###									  

}


