
	Instruction_Group("avx",
		TOP_vaddpd,
	        TOP_UNDEFINED);
        Any_Operand_Access_Time(0);
	Any_Result_Available_Time(2);
	Resource_Requirement(res_issue, 0);

	
	Instruction_Group("avx store",
   		  TOP_vstdqa,
          TOP_vstdqax,
          TOP_vstdqaxx,
          TOP_vstdqa_n32,
          TOP_vstaps,
              TOP_vstapsx,
              TOP_vstapsxx,
              TOP_vstaps_n32,
          TOP_vstapd,
              TOP_vstapdx,
              TOP_vstapdxx,
              TOP_vstapd_n32,
	        TOP_UNDEFINED);
        Any_Operand_Access_Time(0);
	Any_Result_Available_Time(2);
	Resource_Requirement(res_issue, 0);
	Resource_Requirement(res_fstore,0);


	Instruction_Group("avx load",
          TOP_vlddqa,
              TOP_vlddqax,
              TOP_vlddqaxx,
              TOP_vlddqa_n32,
          TOP_vldaps,
              TOP_vldapsx,
              TOP_vldapsxx,
              TOP_vldaps_n32,
          TOP_vldapd,
              TOP_vldapdx,
              TOP_vldapdxx,
              TOP_vldapd_n32,
	        TOP_UNDEFINED);
        Any_Operand_Access_Time(0);
	Any_Result_Available_Time(2);
	Resource_Requirement(res_issue, 0);
	Resource_Requirement(res_fstore, 0);
