		 ###BUILTIN_NAME###(
   	   out1 ###OUTPUT_DATA_1 ,
		   inp1[index] ###INPUT_DATA_1_IS_NOT_IMMEDIATE###	 
		   ###INPUT_DATA_1_IS_IMMEDIATE###
		 , inp2[index] ###INPUT_DATA_2_IS_NOT_IMMEDIATE###
		 , ###INPUT_DATA_2_IS_IMMEDIATE###
		 , inp3[index] ###INPUT_DATA_3_IS_NOT_IMMEDIATE###
		 , ###INPUT_DATA_3_IS_IMMEDIATE###
		 , inp4[index] ###INPUT_DATA_4_IS_NOT_IMMEDIATE###
		 , ###INPUT_DATA_4_IS_IMMEDIATE###
		 ) ;

		 #ifdef __CHECK_CARRY__ 
		 if ( (getSR() >> 12) & 0x1  ) 
		 {
		 	printf("builtin_model: WARNING CARRY detected \n") ;
		 }
		 #endif

		 printf ("###INPUT_DATA_1_PRINTF### )	; 
		 printf ("###INPUT_DATA_2_PRINTF### )	; 
		 printf ("###INPUT_DATA_3_PRINTF### )	; 
		 printf ("###INPUT_DATA_4_PRINTF### )	; 
		 printf ("out1=###OUTPUT_DATA_1_PRINTF### )	; 
		 printf ("\n----------\n")	;

		 index ++ ; 
		 if (index < nb_data_tab) 
		 {
		 	goto loop ;  _IS_NOT_IMMEDIATE###
		 }
		 else
		 {
		 	exit(0) ; 	 _IS_IMMEDIATE###

		 }
