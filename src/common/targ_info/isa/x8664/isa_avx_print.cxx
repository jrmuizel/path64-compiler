ISA_PRINT_TYPE obase64_simm32_float = ISA_Print_Type_Create("obase64_simm32_float",
"%s %s ,%s%s(%s) ");
Name();
Operand(0);
Segment();
Operand(2);
Operand(1);
	Instruction_Print_Group(obase64_simm32_float,
		TOP_vmaskmovdqu_f128_obase64_simm32_float,
		TOP_vmovlps_f128_obase64_simm32_float,
		TOP_vmovntpd_f128_obase64_simm32_float,
		TOP_vmovntps_f128_obase64_simm32_float,
		TOP_vmovsd_f128_obase64_simm32_float,
		TOP_vmovss_f128_obase64_simm32_float,
		TOP_vmovdqa_f128_obase64_simm32_float,
		TOP_vmovdqu_f128_obase64_simm32_float,
		TOP_vmovq_f128_obase64_simm32_float,
		TOP_vmovupd_f128_obase64_simm32_float,
		TOP_vmovups_f128_obase64_simm32_float,
		TOP_vmovaps_f128_obase64_simm32_float,
		TOP_vmovapd_f128_obase64_simm32_float,
		TOP_vmovntdq_f256_obase64_simm32_float,
		TOP_vmovntpd_f256_obase64_simm32_float,
		TOP_vmovntps_f256_obase64_simm32_float,
		TOP_vmovaps_f256_obase64_simm32_float,
		TOP_vmovapd_f256_obase64_simm32_float,
		TOP_vmovdqa_f256_obase64_simm32_float,
		TOP_vmovdqu_f256_obase64_simm32_float,
		TOP_vmovupd_f256_obase64_simm32_float,
		TOP_vmovups_f256_obase64_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_base64_index64_uimm8_simm32_float = ISA_Print_Type_Create("ofloat_base64_index64_uimm8_simm32_float",
"%s %s%s(%s,%s,%s) ,%s ,%s");
Name();
Operand(4);
Segment();
Operand(3);
Operand(0);
Operand(1);
Operand(2);
Result(0);
	Instruction_Print_Group(ofloat_base64_index64_uimm8_simm32_float,
		TOP_vmaskmovps_f128_ofloat_base64_index64_uimm8_simm32_float,
		TOP_vmaskmovps_f256_ofloat_base64_index64_uimm8_simm32_float,
		TOP_vmaskmovpd_f128_ofloat_base64_index64_uimm8_simm32_float,
		TOP_vmaskmovpd_f256_ofloat_base64_index64_uimm8_simm32_float,
		TOP_vmovlps_f128_ofloat_base64_index64_uimm8_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_index64_uimm8_simm32_simm8 = ISA_Print_Type_Create("ofloat_index64_uimm8_simm32_simm8",
"%s %s%s(,%s,%s) ,%s ,%s");
Name();
Operand(3);
Segment();
Operand(2);
Operand(0);
Operand(1);
Result(0);
	Instruction_Print_Group(ofloat_index64_uimm8_simm32_simm8,
		TOP_vpcmpestri_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpcmpestrm_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpcmpistri_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpcmpistrm_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpermilpd_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpermilps_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpshufd_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpshufhw_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpshuflw_f128_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpermilpd_f256_ofloat_index64_uimm8_simm32_simm8,
		TOP_vpermilps_f256_ofloat_index64_uimm8_simm32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oindex64_uimm8_simm32_float_float = ISA_Print_Type_Create("oindex64_uimm8_simm32_float_float",
"%s %s ,%s ,%s%s(,%s,%s) ");
Name();
Operand(1);
Operand(0);
Segment();
Operand(4);
Operand(2);
Operand(3);
	Instruction_Print_Group(oindex64_uimm8_simm32_float_float,
		TOP_vmaskmovps_f128_oindex64_uimm8_simm32_float_float,
		TOP_vmaskmovps_f256_oindex64_uimm8_simm32_float_float,
		TOP_vmaskmovpd_f128_oindex64_uimm8_simm32_float_float,
		TOP_vmaskmovpd_f256_oindex64_uimm8_simm32_float_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_int64 = ISA_Print_Type_Create("ofloat_float_int64",
"%s %s ,%s ,%s");
Name();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_int64,
		TOP_vcvtsi2sd_int64_ofloat_float_int64,
		TOP_vcvtsi2ss_int64_ofloat_float_int64,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint32_base64_simm32 = ISA_Print_Type_Create("oint32_base64_simm32",
"%s %s%s(%s) ,%s");
Name();
Segment();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(oint32_base64_simm32,
		TOP_vcvtsd2si_int32_oint32_base64_simm32,
		TOP_vcvtss2si_int32_oint32_base64_simm32,
		TOP_vcvttsd2si_int32_oint32_base64_simm32,
		TOP_vcvttss2si_int32_oint32_base64_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE float_base64_simm32 = ISA_Print_Type_Create("float_base64_simm32",
"%s %s %s%s(%s) ");
Name();
Segment();
Operand(2);
Operand(1);
Operand(0);
	Instruction_Print_Group(float_base64_simm32,
		TOP_vmovhpd_f128_float_base64_simm32,
		TOP_vmovhps_f128_float_base64_simm32,
		TOP_vmovntdqa_f128_float_base64_simm32,
		TOP_vmovsd_f128_float_base64_simm32,
		TOP_vmovss_f128_float_base64_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_base64_index64_uimm8_simm32 = ISA_Print_Type_Create("ofloat_float_base64_index64_uimm8_simm32",
"%s %s ,%s%s(%s,%s,%s) ,%s");
Name();
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddsubpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddsubpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddsubps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddsubps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandnpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandnpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandnps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vandnps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vdivpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vdivpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vdivps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vdivps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhaddpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhaddpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhaddps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhaddps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhsubpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhsubpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhsubps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vhsubps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmaxpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmaxpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmaxps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmaxps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vminpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vminpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vminps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vminps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmulpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmulpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmulps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmulps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vorpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vorpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vorps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vorps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsubpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsubpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsubps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsubps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpckhpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpckhpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpckhps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpckhps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpcklpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpcklpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpcklps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vunpcklps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vxorpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vxorpd_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vxorps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vxorps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vaddss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vcvtsd2ss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vcvtss2sd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vdivsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vdivss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmaxsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmaxss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vminsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vminss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmovhpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmovhps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmulsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmulss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpacksswb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpackssdw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpackuswb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpackusdw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddsb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddusb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpaddusw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpand_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpandn_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpavgb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpavgw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpeqb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpeqw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpeqd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpeqq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpgtb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpgtw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcmpgtd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpcvpgtq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpermilps_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vphaddw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vphaddd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vphaddsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vphsubw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vphsubd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vphsubsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaddwd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaddubsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaxsb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaxsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaxsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaxub_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaxuw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmaxud_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpminsb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpminsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpminsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpminub_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpminuw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpminud_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmulhuw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmulhrsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmulhw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmullw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmulld_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmuludq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpmuldq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpor_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsadbw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpshufb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsignb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsignw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsignd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsllw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpslld_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsrad_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsrlw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsrld_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubsb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubsw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubusb_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpsubusw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpckhbw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpckhwd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpckhdq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpckhqdq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpcklbw_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpcklwd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpckldq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpunpcklqdq_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpxor_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vrcpss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vrsqrtss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsqrtsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsqrtss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsubsd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vsubss_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vcvtsi2sd_int32_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vcvtsi2ss_int32_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vcvtsi2sd_int64_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vcvtsi2ss_int64_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vmovlpd_f128_ofloat_float_base64_index64_uimm8_simm32,
		TOP_vpermilps_f256_ofloat_float_base64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint64_base64_simm32 = ISA_Print_Type_Create("oint64_base64_simm32",
"%s %s%s(%s) ,%s");
Name();
Segment();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(oint64_base64_simm32,
		TOP_vcvtsd2si_int64_oint64_base64_simm32,
		TOP_vcvtss2si_int64_oint64_base64_simm32,
		TOP_vcvttsd2si_int64_oint64_base64_simm32,
		TOP_vcvttss2si_int64_oint64_base64_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_index64_uimm8_simm32_mxcsr = ISA_Print_Type_Create("obase64_index64_uimm8_simm32_mxcsr",
"%s ,%s%s(%s,%s,%s) ");
Name();
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
	Instruction_Print_Group(obase64_index64_uimm8_simm32_mxcsr,
		TOP_vstmxcsr_obase64_index64_uimm8_simm32_mxcsr,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_base64_index64_uimm8_simm32_simm8 = ISA_Print_Type_Create("ofloat_float_base64_index64_uimm8_simm32_simm8",
"%s %s ,%s%s(%s,%s,%s) ,%s ,%s");
Name();
Operand(5);
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vblendpd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vblendpd_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vblendps_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vblendps_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vcmppd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vcmppd_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vcmpps_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vcmpps_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vdpps_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vdpps_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vroundpd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vroundpd_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vroundps_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vroundps_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vshufpd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vshufpd_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vshufps_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vshufps_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vcmpsd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vcmpss_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vdppd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vinsertf128_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vinsertps_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vmpsadbw_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpalignr_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpblendw_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpclmulqdq_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vroundsd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vroundss_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vperm2f128_f256_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpinsrb_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpinsrw_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpinsrd_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_vpinsrq_f128_ofloat_float_base64_index64_uimm8_simm32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_int64 = ISA_Print_Type_Create("ofloat_int64",
"%s %s ,%s");
Name();
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_int64,
		TOP_vmovq_i64_ofloat_int64,
		TOP_UNDEFINED);


ISA_PRINT_TYPE float_float_simm8 = ISA_Print_Type_Create("float_float_simm8",
"%s %s %s %s ");
Name();
Operand(2);
Operand(1);
Operand(0);
	Instruction_Print_Group(float_float_simm8,
		TOP_vpslldq_f128_float_float_simm8,
		TOP_vpsrldq_f128_float_float_simm8,
		TOP_vpsllw_f128_float_float_simm8,
		TOP_vpsrad_f128_float_float_simm8,
		TOP_vpsrlw_f128_float_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_index64_uimm8_simm32_float_simm8 = ISA_Print_Type_Create("obase64_index64_uimm8_simm32_float_simm8",
"%s %s ,%s ,%s%s(%s,%s,%s) ");
Name();
Operand(1);
Operand(0);
Segment();
Operand(5);
Operand(2);
Operand(3);
Operand(4);
	Instruction_Print_Group(obase64_index64_uimm8_simm32_float_simm8,
		TOP_vextractps_f256_obase64_index64_uimm8_simm32_float_simm8,
		TOP_f128_obase64_index64_uimm8_simm32_float_simm8,
		TOP_vpextrb_mem_obase64_index64_uimm8_simm32_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_simm32_float_float = ISA_Print_Type_Create("obase64_simm32_float_float",
"%s %s ,%s ,%s%s(%s) ");
Name();
Operand(1);
Operand(0);
Segment();
Operand(3);
Operand(2);
	Instruction_Print_Group(obase64_simm32_float_float,
		TOP_vmaskmovps_f128_obase64_simm32_float_float,
		TOP_vmaskmovps_f256_obase64_simm32_float_float,
		TOP_vmaskmovpd_f128_obase64_simm32_float_float,
		TOP_vmaskmovpd_f256_obase64_simm32_float_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_float_float = ISA_Print_Type_Create("ofloat_float_float_float",
"%s %s ,%s ,%s ,%s");
Name();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_float_float,
		TOP_vblendvpd_f128_ofloat_float_float_float,
		TOP_vblendvpd_f256_ofloat_float_float_float,
		TOP_vblendvps_f128_ofloat_float_float_float,
		TOP_vblendvps_f256_ofloat_float_float_float,
		TOP_vpblendvb_f128_ofloat_float_float_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_int32_simm8 = ISA_Print_Type_Create("ofloat_float_int32_simm8",
"%s %s ,%s ,%s ,%s");
Name();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_int32_simm8,
		TOP_vpinsrb_f128_ofloat_float_int32_simm8,
		TOP_vpinsrw_f128_ofloat_float_int32_simm8,
		TOP_vpinsrd_f128_ofloat_float_int32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE null = ISA_Print_Type_Create("null",
"%s ");
Name();
	Instruction_Print_Group(null,
		TOP_vzeroall_null,
		TOP_vzeroupper_null,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint64_float = ISA_Print_Type_Create("oint64_float",
"%s %s ,%s");
Name();
Operand(0);
Result(0);
	Instruction_Print_Group(oint64_float,
		TOP_vcvtsd2si_int64_oint64_float,
		TOP_vcvtss2si_int64_oint64_float,
		TOP_vcvttsd2si_int64_oint64_float,
		TOP_vcvttss2si_int64_oint64_float,
		TOP_vmovq_f128_oint64_float,
		TOP_vpmovmskb_int32_oint64_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_base64_index64_uimm8_simm32_simm8 = ISA_Print_Type_Create("ofloat_base64_index64_uimm8_simm32_simm8",
"%s %s%s(%s,%s,%s) ,%s ,%s");
Name();
Operand(4);
Segment();
Operand(3);
Operand(0);
Operand(1);
Operand(2);
Result(0);
	Instruction_Print_Group(ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpcmpestri_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpcmpestrm_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpcmpistri_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpcmpistrm_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpermilpd_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpermilps_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpshufd_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpshufhw_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpshuflw_f128_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpermilpd_f256_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_vpermilps_f256_ofloat_base64_index64_uimm8_simm32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_base64_simm32 = ISA_Print_Type_Create("ofloat_base64_simm32",
"%s %s%s(%s) ,%s");
Name();
Segment();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_base64_simm32,
		TOP_vbroadcastss_f256_ofloat_base64_simm32,
		TOP_vbroadcastsd_f256_ofloat_base64_simm32,
		TOP_vbroadcastf128_f256_ofloat_base64_simm32,
		TOP_vmovapd_f256_ofloat_base64_simm32,
		TOP_vtestps_f256_ofloat_base64_simm32,
		TOP_vtestpd_f256_ofloat_base64_simm32,
		TOP_vrcpps_f256_ofloat_base64_simm32,
		TOP_vrsqrtps_f256_ofloat_base64_simm32,
		TOP_vsqrtpd_f256_ofloat_base64_simm32,
		TOP_vsqrtps_f256_ofloat_base64_simm32,
		TOP_vcvtdq2ps256_f256_ofloat_base64_simm32,
		TOP_vcvtps2dq_f256_ofloat_base64_simm32,
		TOP_vcvttpd2dq_f256_ofloat_base64_simm32,
		TOP_vcvttps2dq_f256_ofloat_base64_simm32,
		TOP_vmovaps_f256_ofloat_base64_simm32,
		TOP_vmovdqa_f256_ofloat_base64_simm32,
		TOP_vmovdqu_f256_ofloat_base64_simm32,
		TOP_vmovddup_f256_ofloat_base64_simm32,
		TOP_vmovshdup_f256_ofloat_base64_simm32,
		TOP_vmovsldup_f256_ofloat_base64_simm32,
		TOP_vmovupd_f256_ofloat_base64_simm32,
		TOP_vmovups_f256_ofloat_base64_simm32,
		TOP_vcomisd_f128_ofloat_base64_simm32,
		TOP_vcomiss_f128_ofloat_base64_simm32,
		TOP_vcvtdq2pd_f128_ofloat_base64_simm32,
		TOP_vcvtdq2ps_f128_ofloat_base64_simm32,
		TOP_vcvtpd2ps_f128_ofloat_base64_simm32,
		TOP_vcvtps2dq_f128_ofloat_base64_simm32,
		TOP_vcvtps2pd_f128_ofloat_base64_simm32,
		TOP_vcvttpd2dq_f128_ofloat_base64_simm32,
		TOP_vcvttps2dq_f128_ofloat_base64_simm32,
		TOP_vmovapd_f128_ofloat_base64_simm32,
		TOP_vmovaps_f128_ofloat_base64_simm32,
		TOP_vmovq_f128_ofloat_base64_simm32,
		TOP_vmovdqa_f128_ofloat_base64_simm32,
		TOP_vmovdqu_f128_ofloat_base64_simm32,
		TOP_vmovddup_f128_ofloat_base64_simm32,
		TOP_vmovshdup_f128_ofloat_base64_simm32,
		TOP_vmovsldup_f128_ofloat_base64_simm32,
		TOP_vmovupd_f128_ofloat_base64_simm32,
		TOP_vmovups_f128_ofloat_base64_simm32,
		TOP_vpabsb_f128_ofloat_base64_simm32,
		TOP_vpabsw_f128_ofloat_base64_simm32,
		TOP_vpabsd_f128_ofloat_base64_simm32,
		TOP_vbroadcastss_f128_ofloat_base64_simm32,
		TOP_vphminposuw_f128_ofloat_base64_simm32,
		TOP_vpmovsxbw_f128_ofloat_base64_simm32,
		TOP_vpmovsxbd_f128_ofloat_base64_simm32,
		TOP_vpmovzxbw_f128_ofloat_base64_simm32,
		TOP_vpmovzxbd_f128_ofloat_base64_simm32,
		TOP_vtestps_f128_ofloat_base64_simm32,
		TOP_vtestpd_f128_ofloat_base64_simm32,
		TOP_vrcpps_f128_ofloat_base64_simm32,
		TOP_vrsqrtps_f128_ofloat_base64_simm32,
		TOP_vsqrtpd_f128_ofloat_base64_simm32,
		TOP_vsqrtps_f128_ofloat_base64_simm32,
		TOP_vucomisd_f128_ofloat_base64_simm32,
		TOP_vucomiss_f128_ofloat_base64_simm32,
		TOP_vcvtdq2pd_f256_ofloat_base64_simm32,
		TOP_vcvtpd2dq_f256_ofloat_base64_simm32,
		TOP_vcvtps2pd_f256_ofloat_base64_simm32,
		TOP_vcvtpd2dq_f128_f256_ofloat_base64_simm32,
		TOP_vcvtpd2ps_f128_f256_ofloat_base64_simm32,
		TOP_vlddqu_f128_ofloat_base64_simm32,
		TOP_vlddqu_f256_ofloat_base64_simm32,
		TOP_i32_ofloat_base64_simm32,
		TOP_vmovq_i64_ofloat_base64_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_float_simm8 = ISA_Print_Type_Create("ofloat_float_float_simm8",
"%s %s ,%s ,%s ,%s");
Name();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_float_simm8,
		TOP_vblendpd_f128_ofloat_float_float_simm8,
		TOP_vblendpd_f256_ofloat_float_float_simm8,
		TOP_vblendps_f128_ofloat_float_float_simm8,
		TOP_vblendps_f256_ofloat_float_float_simm8,
		TOP_vcmppd_f128_ofloat_float_float_simm8,
		TOP_vcmppd_f256_ofloat_float_float_simm8,
		TOP_vcmpps_f128_ofloat_float_float_simm8,
		TOP_vcmpps_f256_ofloat_float_float_simm8,
		TOP_vdpps_f128_ofloat_float_float_simm8,
		TOP_vdpps_f256_ofloat_float_float_simm8,
		TOP_vroundpd_f128_ofloat_float_float_simm8,
		TOP_vroundpd_f256_ofloat_float_float_simm8,
		TOP_vroundps_f128_ofloat_float_float_simm8,
		TOP_vroundps_f256_ofloat_float_float_simm8,
		TOP_vshufpd_f128_ofloat_float_float_simm8,
		TOP_vshufpd_f256_ofloat_float_float_simm8,
		TOP_vshufps_f128_ofloat_float_float_simm8,
		TOP_vshufps_f256_ofloat_float_float_simm8,
		TOP_vcmpsd_f128_ofloat_float_float_simm8,
		TOP_vcmpss_f128_ofloat_float_float_simm8,
		TOP_vdppd_f128_ofloat_float_float_simm8,
		TOP_vinsertf128_f256_ofloat_float_float_simm8,
		TOP_vinsertps_f128_ofloat_float_float_simm8,
		TOP_vmpsadbw_f128_ofloat_float_float_simm8,
		TOP_vpalignr_f128_ofloat_float_float_simm8,
		TOP_vpblendw_f128_ofloat_float_float_simm8,
		TOP_vpclmulqdq_f128_ofloat_float_float_simm8,
		TOP_vroundsd_f128_ofloat_float_float_simm8,
		TOP_vroundss_f128_ofloat_float_float_simm8,
		TOP_vperm2f128_f256_ofloat_float_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE mxcsr_obase64_simm32 = ISA_Print_Type_Create("mxcsr_obase64_simm32",
"%s ,%s%s(%s) ");
Name();
Segment();
Operand(2);
Operand(1);
	Instruction_Print_Group(mxcsr_obase64_simm32,
		TOP_vldmxcsr_mxcsr_obase64_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oindex64_uimm8_simm32_mxcsr = ISA_Print_Type_Create("oindex64_uimm8_simm32_mxcsr",
"%s ,%s%s(,%s,%s) ");
Name();
Segment();
Operand(3);
Operand(1);
Operand(2);
	Instruction_Print_Group(oindex64_uimm8_simm32_mxcsr,
		TOP_vstmxcsr_oindex64_uimm8_simm32_mxcsr,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_index64_uimm8_simm32_float = ISA_Print_Type_Create("ofloat_index64_uimm8_simm32_float",
"%s %s%s(,%s,%s) ,%s ,%s");
Name();
Operand(3);
Segment();
Operand(2);
Operand(0);
Operand(1);
Result(0);
	Instruction_Print_Group(ofloat_index64_uimm8_simm32_float,
		TOP_vmaskmovps_f128_ofloat_index64_uimm8_simm32_float,
		TOP_vmaskmovps_f256_ofloat_index64_uimm8_simm32_float,
		TOP_vmaskmovpd_f128_ofloat_index64_uimm8_simm32_float,
		TOP_vmaskmovpd_f256_ofloat_index64_uimm8_simm32_float,
		TOP_vmovlps_f128_ofloat_index64_uimm8_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_base64_simm32 = ISA_Print_Type_Create("ofloat_float_base64_simm32",
"%s %s ,%s%s(%s) ,%s");
Name();
Segment();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_base64_simm32,
		TOP_vaddpd_f128_ofloat_float_base64_simm32,
		TOP_vaddpd_f256_ofloat_float_base64_simm32,
		TOP_vaddps_f128_ofloat_float_base64_simm32,
		TOP_vaddps_f256_ofloat_float_base64_simm32,
		TOP_vaddsubpd_f128_ofloat_float_base64_simm32,
		TOP_vaddsubpd_f256_ofloat_float_base64_simm32,
		TOP_vaddsubps_f128_ofloat_float_base64_simm32,
		TOP_vaddsubps_f256_ofloat_float_base64_simm32,
		TOP_vandpd_f128_ofloat_float_base64_simm32,
		TOP_vandpd_f256_ofloat_float_base64_simm32,
		TOP_vandnpd_f128_ofloat_float_base64_simm32,
		TOP_vandnpd_f256_ofloat_float_base64_simm32,
		TOP_vandps_f128_ofloat_float_base64_simm32,
		TOP_vandps_f256_ofloat_float_base64_simm32,
		TOP_vandnps_f128_ofloat_float_base64_simm32,
		TOP_vandnps_f256_ofloat_float_base64_simm32,
		TOP_vdivpd_f128_ofloat_float_base64_simm32,
		TOP_vdivpd_f256_ofloat_float_base64_simm32,
		TOP_vdivps_f128_ofloat_float_base64_simm32,
		TOP_vdivps_f256_ofloat_float_base64_simm32,
		TOP_vhaddpd_f128_ofloat_float_base64_simm32,
		TOP_vhaddpd_f256_ofloat_float_base64_simm32,
		TOP_vhaddps_f128_ofloat_float_base64_simm32,
		TOP_vhaddps_f256_ofloat_float_base64_simm32,
		TOP_vhsubpd_f128_ofloat_float_base64_simm32,
		TOP_vhsubpd_f256_ofloat_float_base64_simm32,
		TOP_vhsubps_f128_ofloat_float_base64_simm32,
		TOP_vhsubps_f256_ofloat_float_base64_simm32,
		TOP_vmaxpd_f128_ofloat_float_base64_simm32,
		TOP_vmaxpd_f256_ofloat_float_base64_simm32,
		TOP_vmaxps_f128_ofloat_float_base64_simm32,
		TOP_vmaxps_f256_ofloat_float_base64_simm32,
		TOP_vminpd_f128_ofloat_float_base64_simm32,
		TOP_vminpd_f256_ofloat_float_base64_simm32,
		TOP_vminps_f128_ofloat_float_base64_simm32,
		TOP_vminps_f256_ofloat_float_base64_simm32,
		TOP_vmulpd_f128_ofloat_float_base64_simm32,
		TOP_vmulpd_f256_ofloat_float_base64_simm32,
		TOP_vmulps_f128_ofloat_float_base64_simm32,
		TOP_vmulps_f256_ofloat_float_base64_simm32,
		TOP_vorpd_f128_ofloat_float_base64_simm32,
		TOP_vorpd_f256_ofloat_float_base64_simm32,
		TOP_vorps_f128_ofloat_float_base64_simm32,
		TOP_vorps_f256_ofloat_float_base64_simm32,
		TOP_vsubpd_f128_ofloat_float_base64_simm32,
		TOP_vsubpd_f256_ofloat_float_base64_simm32,
		TOP_vsubps_f128_ofloat_float_base64_simm32,
		TOP_vsubps_f256_ofloat_float_base64_simm32,
		TOP_vunpckhpd_f128_ofloat_float_base64_simm32,
		TOP_vunpckhpd_f256_ofloat_float_base64_simm32,
		TOP_vunpckhps_f128_ofloat_float_base64_simm32,
		TOP_vunpckhps_f256_ofloat_float_base64_simm32,
		TOP_vunpcklpd_f128_ofloat_float_base64_simm32,
		TOP_vunpcklpd_f256_ofloat_float_base64_simm32,
		TOP_vunpcklps_f128_ofloat_float_base64_simm32,
		TOP_vunpcklps_f256_ofloat_float_base64_simm32,
		TOP_vxorpd_f128_ofloat_float_base64_simm32,
		TOP_vxorpd_f256_ofloat_float_base64_simm32,
		TOP_vxorps_f128_ofloat_float_base64_simm32,
		TOP_vxorps_f256_ofloat_float_base64_simm32,
		TOP_vaddsd_f128_ofloat_float_base64_simm32,
		TOP_vaddss_f128_ofloat_float_base64_simm32,
		TOP_vcvtsd2ss_f128_ofloat_float_base64_simm32,
		TOP_vcvtss2sd_f128_ofloat_float_base64_simm32,
		TOP_vdivsd_f128_ofloat_float_base64_simm32,
		TOP_vdivss_f128_ofloat_float_base64_simm32,
		TOP_vmaxsd_f128_ofloat_float_base64_simm32,
		TOP_vmaxss_f128_ofloat_float_base64_simm32,
		TOP_vminsd_f128_ofloat_float_base64_simm32,
		TOP_vminss_f128_ofloat_float_base64_simm32,
		TOP_vmovhpd_f128_ofloat_float_base64_simm32,
		TOP_vmovhps_f128_ofloat_float_base64_simm32,
		TOP_vmulsd_f128_ofloat_float_base64_simm32,
		TOP_vmulss_f128_ofloat_float_base64_simm32,
		TOP_vpacksswb_f128_ofloat_float_base64_simm32,
		TOP_vpackssdw_f128_ofloat_float_base64_simm32,
		TOP_vpackuswb_f128_ofloat_float_base64_simm32,
		TOP_vpackusdw_f128_ofloat_float_base64_simm32,
		TOP_vpaddb_f128_ofloat_float_base64_simm32,
		TOP_vpaddw_f128_ofloat_float_base64_simm32,
		TOP_vpaddd_f128_ofloat_float_base64_simm32,
		TOP_vpaddq_f128_ofloat_float_base64_simm32,
		TOP_vpaddsb_f128_ofloat_float_base64_simm32,
		TOP_vpaddsw_f128_ofloat_float_base64_simm32,
		TOP_vpaddusb_f128_ofloat_float_base64_simm32,
		TOP_vpaddusw_f128_ofloat_float_base64_simm32,
		TOP_vpand_f128_ofloat_float_base64_simm32,
		TOP_vpandn_f128_ofloat_float_base64_simm32,
		TOP_vpavgb_f128_ofloat_float_base64_simm32,
		TOP_vpavgw_f128_ofloat_float_base64_simm32,
		TOP_vpcmpeqb_f128_ofloat_float_base64_simm32,
		TOP_vpcmpeqw_f128_ofloat_float_base64_simm32,
		TOP_vpcmpeqd_f128_ofloat_float_base64_simm32,
		TOP_vpcmpeqq_f128_ofloat_float_base64_simm32,
		TOP_vpcmpgtb_f128_ofloat_float_base64_simm32,
		TOP_vpcmpgtw_f128_ofloat_float_base64_simm32,
		TOP_vpcmpgtd_f128_ofloat_float_base64_simm32,
		TOP_vpcvpgtq_f128_ofloat_float_base64_simm32,
		TOP_vpermilps_f128_ofloat_float_base64_simm32,
		TOP_vphaddw_f128_ofloat_float_base64_simm32,
		TOP_vphaddd_f128_ofloat_float_base64_simm32,
		TOP_vphaddsw_f128_ofloat_float_base64_simm32,
		TOP_vphsubw_f128_ofloat_float_base64_simm32,
		TOP_vphsubd_f128_ofloat_float_base64_simm32,
		TOP_vphsubsw_f128_ofloat_float_base64_simm32,
		TOP_vpmaddwd_f128_ofloat_float_base64_simm32,
		TOP_vpmaddubsw_f128_ofloat_float_base64_simm32,
		TOP_vpmaxsb_f128_ofloat_float_base64_simm32,
		TOP_vpmaxsw_f128_ofloat_float_base64_simm32,
		TOP_vpmaxsd_f128_ofloat_float_base64_simm32,
		TOP_vpmaxub_f128_ofloat_float_base64_simm32,
		TOP_vpmaxuw_f128_ofloat_float_base64_simm32,
		TOP_vpmaxud_f128_ofloat_float_base64_simm32,
		TOP_vpminsb_f128_ofloat_float_base64_simm32,
		TOP_vpminsw_f128_ofloat_float_base64_simm32,
		TOP_vpminsd_f128_ofloat_float_base64_simm32,
		TOP_vpminub_f128_ofloat_float_base64_simm32,
		TOP_vpminuw_f128_ofloat_float_base64_simm32,
		TOP_vpminud_f128_ofloat_float_base64_simm32,
		TOP_vpmulhuw_f128_ofloat_float_base64_simm32,
		TOP_vpmulhrsw_f128_ofloat_float_base64_simm32,
		TOP_vpmulhw_f128_ofloat_float_base64_simm32,
		TOP_vpmullw_f128_ofloat_float_base64_simm32,
		TOP_vpmulld_f128_ofloat_float_base64_simm32,
		TOP_vpmuludq_f128_ofloat_float_base64_simm32,
		TOP_vpmuldq_f128_ofloat_float_base64_simm32,
		TOP_vpor_f128_ofloat_float_base64_simm32,
		TOP_vpsadbw_f128_ofloat_float_base64_simm32,
		TOP_vpshufb_f128_ofloat_float_base64_simm32,
		TOP_vpsignb_f128_ofloat_float_base64_simm32,
		TOP_vpsignw_f128_ofloat_float_base64_simm32,
		TOP_vpsignd_f128_ofloat_float_base64_simm32,
		TOP_vpsllw_f128_ofloat_float_base64_simm32,
		TOP_vpslld_f128_ofloat_float_base64_simm32,
		TOP_vpsrad_f128_ofloat_float_base64_simm32,
		TOP_vpsrlw_f128_ofloat_float_base64_simm32,
		TOP_vpsrld_f128_ofloat_float_base64_simm32,
		TOP_vpsubb_f128_ofloat_float_base64_simm32,
		TOP_vpsubw_f128_ofloat_float_base64_simm32,
		TOP_vpsubd_f128_ofloat_float_base64_simm32,
		TOP_vpsubq_f128_ofloat_float_base64_simm32,
		TOP_vpsubsb_f128_ofloat_float_base64_simm32,
		TOP_vpsubsw_f128_ofloat_float_base64_simm32,
		TOP_vpsubusb_f128_ofloat_float_base64_simm32,
		TOP_vpsubusw_f128_ofloat_float_base64_simm32,
		TOP_vpunpckhbw_f128_ofloat_float_base64_simm32,
		TOP_vpunpckhwd_f128_ofloat_float_base64_simm32,
		TOP_vpunpckhdq_f128_ofloat_float_base64_simm32,
		TOP_vpunpckhqdq_f128_ofloat_float_base64_simm32,
		TOP_vpunpcklbw_f128_ofloat_float_base64_simm32,
		TOP_vpunpcklwd_f128_ofloat_float_base64_simm32,
		TOP_vpunpckldq_f128_ofloat_float_base64_simm32,
		TOP_vpunpcklqdq_f128_ofloat_float_base64_simm32,
		TOP_vpxor_f128_ofloat_float_base64_simm32,
		TOP_vrcpss_f128_ofloat_float_base64_simm32,
		TOP_vrsqrtss_f128_ofloat_float_base64_simm32,
		TOP_vsqrtsd_f128_ofloat_float_base64_simm32,
		TOP_vsqrtss_f128_ofloat_float_base64_simm32,
		TOP_vsubsd_f128_ofloat_float_base64_simm32,
		TOP_vsubss_f128_ofloat_float_base64_simm32,
		TOP_vcvtsi2sd_int32_ofloat_float_base64_simm32,
		TOP_vcvtsi2ss_int32_ofloat_float_base64_simm32,
		TOP_vcvtsi2sd_int64_ofloat_float_base64_simm32,
		TOP_vcvtsi2ss_int64_ofloat_float_base64_simm32,
		TOP_vmovlpd_f128_ofloat_float_base64_simm32,
		TOP_vpermilps_f256_ofloat_float_base64_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE mxcsr_oindex64_uimm8_simm32 = ISA_Print_Type_Create("mxcsr_oindex64_uimm8_simm32",
"%s ,%s%s(,%s,%s) ");
Name();
Segment();
Operand(3);
Operand(1);
Operand(2);
	Instruction_Print_Group(mxcsr_oindex64_uimm8_simm32,
		TOP_vldmxcsr_mxcsr_oindex64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_simm32_float_simm8 = ISA_Print_Type_Create("obase64_simm32_float_simm8",
"%s %s ,%s ,%s%s(%s) ");
Name();
Operand(1);
Operand(0);
Segment();
Operand(3);
Operand(2);
	Instruction_Print_Group(obase64_simm32_float_simm8,
		TOP_vextractps_f256_obase64_simm32_float_simm8,
		TOP_f128_obase64_simm32_float_simm8,
		TOP_vpextrb_mem_obase64_simm32_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_index64_uimm8_simm32 = ISA_Print_Type_Create("ofloat_index64_uimm8_simm32",
"%s %s%s(,%s,%s) ,%s");
Name();
Segment();
Operand(2);
Operand(0);
Operand(1);
Result(0);
	Instruction_Print_Group(ofloat_index64_uimm8_simm32,
		TOP_vbroadcastss_f256_ofloat_index64_uimm8_simm32,
		TOP_vbroadcastsd_f256_ofloat_index64_uimm8_simm32,
		TOP_vbroadcastf128_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovapd_f256_ofloat_index64_uimm8_simm32,
		TOP_vtestps_f256_ofloat_index64_uimm8_simm32,
		TOP_vtestpd_f256_ofloat_index64_uimm8_simm32,
		TOP_vrcpps_f256_ofloat_index64_uimm8_simm32,
		TOP_vrsqrtps_f256_ofloat_index64_uimm8_simm32,
		TOP_vsqrtpd_f256_ofloat_index64_uimm8_simm32,
		TOP_vsqrtps_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvtdq2ps256_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvtps2dq_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvttpd2dq_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvttps2dq_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovaps_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovdqa_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovdqu_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovddup_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovshdup_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovsldup_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovupd_f256_ofloat_index64_uimm8_simm32,
		TOP_vmovups_f256_ofloat_index64_uimm8_simm32,
		TOP_vcomisd_f128_ofloat_index64_uimm8_simm32,
		TOP_vcomiss_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvtdq2pd_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvtdq2ps_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvtpd2ps_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvtps2dq_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvtps2pd_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvttpd2dq_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvttps2dq_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovapd_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovaps_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovq_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovdqa_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovdqu_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovddup_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovshdup_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovsldup_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovupd_f128_ofloat_index64_uimm8_simm32,
		TOP_vmovups_f128_ofloat_index64_uimm8_simm32,
		TOP_vpabsb_f128_ofloat_index64_uimm8_simm32,
		TOP_vpabsw_f128_ofloat_index64_uimm8_simm32,
		TOP_vpabsd_f128_ofloat_index64_uimm8_simm32,
		TOP_vbroadcastss_f128_ofloat_index64_uimm8_simm32,
		TOP_vphminposuw_f128_ofloat_index64_uimm8_simm32,
		TOP_vpmovsxbw_f128_ofloat_index64_uimm8_simm32,
		TOP_vpmovsxbd_f128_ofloat_index64_uimm8_simm32,
		TOP_vpmovzxbw_f128_ofloat_index64_uimm8_simm32,
		TOP_vpmovzxbd_f128_ofloat_index64_uimm8_simm32,
		TOP_vtestps_f128_ofloat_index64_uimm8_simm32,
		TOP_vtestpd_f128_ofloat_index64_uimm8_simm32,
		TOP_vrcpps_f128_ofloat_index64_uimm8_simm32,
		TOP_vrsqrtps_f128_ofloat_index64_uimm8_simm32,
		TOP_vsqrtpd_f128_ofloat_index64_uimm8_simm32,
		TOP_vsqrtps_f128_ofloat_index64_uimm8_simm32,
		TOP_vucomisd_f128_ofloat_index64_uimm8_simm32,
		TOP_vucomiss_f128_ofloat_index64_uimm8_simm32,
		TOP_vcvtdq2pd_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvtpd2dq_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvtps2pd_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvtpd2dq_f128_f256_ofloat_index64_uimm8_simm32,
		TOP_vcvtpd2ps_f128_f256_ofloat_index64_uimm8_simm32,
		TOP_vlddqu_f128_ofloat_index64_uimm8_simm32,
		TOP_vlddqu_f256_ofloat_index64_uimm8_simm32,
		TOP_i32_ofloat_index64_uimm8_simm32,
		TOP_vmovq_i64_ofloat_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint64_index64_uimm8_simm32 = ISA_Print_Type_Create("oint64_index64_uimm8_simm32",
"%s %s%s(,%s,%s) ,%s");
Name();
Segment();
Operand(2);
Operand(0);
Operand(1);
Result(0);
	Instruction_Print_Group(oint64_index64_uimm8_simm32,
		TOP_vcvtsd2si_int64_oint64_index64_uimm8_simm32,
		TOP_vcvtss2si_int64_oint64_index64_uimm8_simm32,
		TOP_vcvttsd2si_int64_oint64_index64_uimm8_simm32,
		TOP_vcvttss2si_int64_oint64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_simm8 = ISA_Print_Type_Create("ofloat_float_simm8",
"%s %s ,%s ,%s");
Name();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_simm8,
		TOP_vextractps_f256_ofloat_float_simm8,
		TOP_vpcmpestri_f128_ofloat_float_simm8,
		TOP_vpcmpestrm_f128_ofloat_float_simm8,
		TOP_vpcmpistri_f128_ofloat_float_simm8,
		TOP_vpcmpistrm_f128_ofloat_float_simm8,
		TOP_vpermilpd_f128_ofloat_float_simm8,
		TOP_vpermilps_f128_ofloat_float_simm8,
		TOP_vpshufd_f128_ofloat_float_simm8,
		TOP_vpshufhw_f128_ofloat_float_simm8,
		TOP_vpshuflw_f128_ofloat_float_simm8,
		TOP_vpermilpd_f256_ofloat_float_simm8,
		TOP_vpermilps_f256_ofloat_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE float_base64_index64_uimm8_simm32 = ISA_Print_Type_Create("float_base64_index64_uimm8_simm32",
"%s %s %s%s(%s,%s,%s) ");
Name();
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
Operand(0);
	Instruction_Print_Group(float_base64_index64_uimm8_simm32,
		TOP_vmovhpd_f128_float_base64_index64_uimm8_simm32,
		TOP_vmovhps_f128_float_base64_index64_uimm8_simm32,
		TOP_vmovntdqa_f128_float_base64_index64_uimm8_simm32,
		TOP_vmovsd_f128_float_base64_index64_uimm8_simm32,
		TOP_vmovss_f128_float_base64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_base64_index64_uimm8_simm32 = ISA_Print_Type_Create("ofloat_base64_index64_uimm8_simm32",
"%s %s%s(%s,%s,%s) ,%s");
Name();
Segment();
Operand(3);
Operand(0);
Operand(1);
Operand(2);
Result(0);
	Instruction_Print_Group(ofloat_base64_index64_uimm8_simm32,
		TOP_vbroadcastss_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vbroadcastsd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vbroadcastf128_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovapd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vtestps_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vtestpd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vrcpps_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vrsqrtps_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vsqrtpd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vsqrtps_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtdq2ps256_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtps2dq_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvttpd2dq_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvttps2dq_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovaps_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovdqa_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovdqu_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovddup_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovshdup_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovsldup_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovupd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovups_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcomisd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcomiss_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtdq2pd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtdq2ps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtpd2ps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtps2dq_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtps2pd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvttpd2dq_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvttps2dq_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovapd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovaps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovq_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovdqa_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovdqu_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovddup_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovshdup_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovsldup_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovupd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovups_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpabsb_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpabsw_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpabsd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vbroadcastss_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vphminposuw_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpmovsxbw_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpmovsxbd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpmovzxbw_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vpmovzxbd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vtestps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vtestpd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vrcpps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vrsqrtps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vsqrtpd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vsqrtps_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vucomisd_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vucomiss_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtdq2pd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtpd2dq_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtps2pd_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtpd2dq_f128_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vcvtpd2ps_f128_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_vlddqu_f128_ofloat_base64_index64_uimm8_simm32,
		TOP_vlddqu_f256_ofloat_base64_index64_uimm8_simm32,
		TOP_i32_ofloat_base64_index64_uimm8_simm32,
		TOP_vmovq_i64_ofloat_base64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_simm32_mxcsr = ISA_Print_Type_Create("obase64_simm32_mxcsr",
"%s ,%s%s(%s) ");
Name();
Segment();
Operand(2);
Operand(1);
	Instruction_Print_Group(obase64_simm32_mxcsr,
		TOP_vstmxcsr_obase64_simm32_mxcsr,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_base64_simm32_simm8 = ISA_Print_Type_Create("ofloat_float_base64_simm32_simm8",
"%s %s ,%s%s(%s) ,%s ,%s");
Name();
Operand(3);
Segment();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_base64_simm32_simm8,
		TOP_vblendpd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vblendpd_f256_ofloat_float_base64_simm32_simm8,
		TOP_vblendps_f128_ofloat_float_base64_simm32_simm8,
		TOP_vblendps_f256_ofloat_float_base64_simm32_simm8,
		TOP_vcmppd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vcmppd_f256_ofloat_float_base64_simm32_simm8,
		TOP_vcmpps_f128_ofloat_float_base64_simm32_simm8,
		TOP_vcmpps_f256_ofloat_float_base64_simm32_simm8,
		TOP_vdpps_f128_ofloat_float_base64_simm32_simm8,
		TOP_vdpps_f256_ofloat_float_base64_simm32_simm8,
		TOP_vroundpd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vroundpd_f256_ofloat_float_base64_simm32_simm8,
		TOP_vroundps_f128_ofloat_float_base64_simm32_simm8,
		TOP_vroundps_f256_ofloat_float_base64_simm32_simm8,
		TOP_vshufpd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vshufpd_f256_ofloat_float_base64_simm32_simm8,
		TOP_vshufps_f128_ofloat_float_base64_simm32_simm8,
		TOP_vshufps_f256_ofloat_float_base64_simm32_simm8,
		TOP_vcmpsd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vcmpss_f128_ofloat_float_base64_simm32_simm8,
		TOP_vdppd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vinsertf128_f256_ofloat_float_base64_simm32_simm8,
		TOP_vinsertps_f128_ofloat_float_base64_simm32_simm8,
		TOP_vmpsadbw_f128_ofloat_float_base64_simm32_simm8,
		TOP_vpalignr_f128_ofloat_float_base64_simm32_simm8,
		TOP_vpblendw_f128_ofloat_float_base64_simm32_simm8,
		TOP_vpclmulqdq_f128_ofloat_float_base64_simm32_simm8,
		TOP_vroundsd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vroundss_f128_ofloat_float_base64_simm32_simm8,
		TOP_vperm2f128_f256_ofloat_float_base64_simm32_simm8,
		TOP_vpinsrb_f128_ofloat_float_base64_simm32_simm8,
		TOP_vpinsrw_f128_ofloat_float_base64_simm32_simm8,
		TOP_vpinsrd_f128_ofloat_float_base64_simm32_simm8,
		TOP_vpinsrq_f128_ofloat_float_base64_simm32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float = ISA_Print_Type_Create("ofloat_float",
"%s %s ,%s");
Name();
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float,
		TOP_vbroadcastss_f256_ofloat_float,
		TOP_vbroadcastsd_f256_ofloat_float,
		TOP_vbroadcastf128_f256_ofloat_float,
		TOP_vmovapd_f256_ofloat_float,
		TOP_vtestps_f256_ofloat_float,
		TOP_vtestpd_f256_ofloat_float,
		TOP_vrcpps_f256_ofloat_float,
		TOP_vrsqrtps_f256_ofloat_float,
		TOP_vsqrtpd_f256_ofloat_float,
		TOP_vsqrtps_f256_ofloat_float,
		TOP_vcvtdq2ps256_f256_ofloat_float,
		TOP_vcvtps2dq_f256_ofloat_float,
		TOP_vcvttpd2dq_f256_ofloat_float,
		TOP_vcvttps2dq_f256_ofloat_float,
		TOP_vmovaps_f256_ofloat_float,
		TOP_vmovdqa_f256_ofloat_float,
		TOP_vmovdqu_f256_ofloat_float,
		TOP_vmovddup_f256_ofloat_float,
		TOP_vmovshdup_f256_ofloat_float,
		TOP_vmovsldup_f256_ofloat_float,
		TOP_vmovupd_f256_ofloat_float,
		TOP_vmovups_f256_ofloat_float,
		TOP_vcomisd_f128_ofloat_float,
		TOP_vcomiss_f128_ofloat_float,
		TOP_vcvtdq2pd_f128_ofloat_float,
		TOP_vcvtdq2ps_f128_ofloat_float,
		TOP_vcvtpd2ps_f128_ofloat_float,
		TOP_vcvtps2dq_f128_ofloat_float,
		TOP_vcvtps2pd_f128_ofloat_float,
		TOP_vcvttpd2dq_f128_ofloat_float,
		TOP_vcvttps2dq_f128_ofloat_float,
		TOP_vmovapd_f128_ofloat_float,
		TOP_vmovaps_f128_ofloat_float,
		TOP_vmovq_f128_ofloat_float,
		TOP_vmovdqa_f128_ofloat_float,
		TOP_vmovdqu_f128_ofloat_float,
		TOP_vmovddup_f128_ofloat_float,
		TOP_vmovshdup_f128_ofloat_float,
		TOP_vmovsldup_f128_ofloat_float,
		TOP_vmovupd_f128_ofloat_float,
		TOP_vmovups_f128_ofloat_float,
		TOP_vpabsb_f128_ofloat_float,
		TOP_vpabsw_f128_ofloat_float,
		TOP_vpabsd_f128_ofloat_float,
		TOP_vbroadcastss_f128_ofloat_float,
		TOP_vphminposuw_f128_ofloat_float,
		TOP_vpmovsxbw_f128_ofloat_float,
		TOP_vpmovsxbd_f128_ofloat_float,
		TOP_vpmovzxbw_f128_ofloat_float,
		TOP_vpmovzxbd_f128_ofloat_float,
		TOP_vtestps_f128_ofloat_float,
		TOP_vtestpd_f128_ofloat_float,
		TOP_vrcpps_f128_ofloat_float,
		TOP_vrsqrtps_f128_ofloat_float,
		TOP_vsqrtpd_f128_ofloat_float,
		TOP_vsqrtps_f128_ofloat_float,
		TOP_vucomisd_f128_ofloat_float,
		TOP_vucomiss_f128_ofloat_float,
		TOP_vcvtdq2pd_f256_ofloat_float,
		TOP_vcvtpd2dq_f256_ofloat_float,
		TOP_vcvtps2pd_f256_ofloat_float,
		TOP_vcvtpd2dq_f128_f256_ofloat_float,
		TOP_vcvtpd2ps_f128_f256_ofloat_float,
		TOP_vmaskmovdqu_f128_ofloat_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oindex64_uimm8_simm32_float_simm8 = ISA_Print_Type_Create("oindex64_uimm8_simm32_float_simm8",
"%s %s ,%s ,%s%s(,%s,%s) ");
Name();
Operand(1);
Operand(0);
Segment();
Operand(4);
Operand(2);
Operand(3);
	Instruction_Print_Group(oindex64_uimm8_simm32_float_simm8,
		TOP_vextractps_f256_oindex64_uimm8_simm32_float_simm8,
		TOP_f128_oindex64_uimm8_simm32_float_simm8,
		TOP_vpextrb_mem_oindex64_uimm8_simm32_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_int32 = ISA_Print_Type_Create("ofloat_float_int32",
"%s %s ,%s ,%s");
Name();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_int32,
		TOP_vcvtsi2sd_int32_ofloat_float_int32,
		TOP_vcvtsi2ss_int32_ofloat_float_int32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_index64_uimm8_simm32_float = ISA_Print_Type_Create("ofloat_float_index64_uimm8_simm32_float",
"%s %s ,%s%s(,%s,%s) ,%s ,%s");
Name();
Operand(4);
Segment();
Operand(3);
Operand(1);
Operand(2);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_index64_uimm8_simm32_float,
		TOP_vblendvpd_f128_ofloat_float_index64_uimm8_simm32_float,
		TOP_vblendvpd_f256_ofloat_float_index64_uimm8_simm32_float,
		TOP_vblendvps_f128_ofloat_float_index64_uimm8_simm32_float,
		TOP_vblendvps_f256_ofloat_float_index64_uimm8_simm32_float,
		TOP_vpblendvb_f128_ofloat_float_index64_uimm8_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_int32 = ISA_Print_Type_Create("ofloat_int32",
"%s %s ,%s");
Name();
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_int32,
		TOP_i32_ofloat_int32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_base64_simm32_simm8 = ISA_Print_Type_Create("ofloat_base64_simm32_simm8",
"%s %s%s(%s) ,%s ,%s");
Name();
Operand(2);
Segment();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_base64_simm32_simm8,
		TOP_vpcmpestri_f128_ofloat_base64_simm32_simm8,
		TOP_vpcmpestrm_f128_ofloat_base64_simm32_simm8,
		TOP_vpcmpistri_f128_ofloat_base64_simm32_simm8,
		TOP_vpcmpistrm_f128_ofloat_base64_simm32_simm8,
		TOP_vpermilpd_f128_ofloat_base64_simm32_simm8,
		TOP_vpermilps_f128_ofloat_base64_simm32_simm8,
		TOP_vpshufd_f128_ofloat_base64_simm32_simm8,
		TOP_vpshufhw_f128_ofloat_base64_simm32_simm8,
		TOP_vpshuflw_f128_ofloat_base64_simm32_simm8,
		TOP_vpermilpd_f256_ofloat_base64_simm32_simm8,
		TOP_vpermilps_f256_ofloat_base64_simm32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_base64_simm32_float = ISA_Print_Type_Create("ofloat_base64_simm32_float",
"%s %s%s(%s) ,%s ,%s");
Name();
Operand(2);
Segment();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_base64_simm32_float,
		TOP_vmaskmovps_f128_ofloat_base64_simm32_float,
		TOP_vmaskmovps_f256_ofloat_base64_simm32_float,
		TOP_vmaskmovpd_f128_ofloat_base64_simm32_float,
		TOP_vmaskmovpd_f256_ofloat_base64_simm32_float,
		TOP_vmovlps_f128_ofloat_base64_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE float_index64_uimm8_simm32 = ISA_Print_Type_Create("float_index64_uimm8_simm32",
"%s %s %s%s(,%s,%s) ");
Name();
Segment();
Operand(3);
Operand(1);
Operand(2);
Operand(0);
	Instruction_Print_Group(float_index64_uimm8_simm32,
		TOP_vmovhpd_f128_float_index64_uimm8_simm32,
		TOP_vmovhps_f128_float_index64_uimm8_simm32,
		TOP_vmovntdqa_f128_float_index64_uimm8_simm32,
		TOP_vmovsd_f128_float_index64_uimm8_simm32,
		TOP_vmovss_f128_float_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint32_float = ISA_Print_Type_Create("oint32_float",
"%s %s ,%s");
Name();
Operand(0);
Result(0);
	Instruction_Print_Group(oint32_float,
		TOP_vcvtsd2si_int32_oint32_float,
		TOP_vcvtss2si_int32_oint32_float,
		TOP_vcvttsd2si_int32_oint32_float,
		TOP_vcvttss2si_int32_oint32_float,
		TOP_vmovd_f128_oint32_float,
		TOP_vmovmskpd_f128_oint32_float,
		TOP_vmovmskpd_f256_oint32_float,
		TOP_vmovmskps_f128_oint32_float,
		TOP_vmovmskps_f256_oint32_float,
		TOP_vpmovmskb_int32_oint32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oindex64_uimm8_simm32_float = ISA_Print_Type_Create("oindex64_uimm8_simm32_float",
"%s %s ,%s%s(,%s,%s) ");
Name();
Operand(0);
Segment();
Operand(3);
Operand(1);
Operand(2);
	Instruction_Print_Group(oindex64_uimm8_simm32_float,
		TOP_vmaskmovdqu_f128_oindex64_uimm8_simm32_float,
		TOP_vmovlps_f128_oindex64_uimm8_simm32_float,
		TOP_vmovntpd_f128_oindex64_uimm8_simm32_float,
		TOP_vmovntps_f128_oindex64_uimm8_simm32_float,
		TOP_vmovsd_f128_oindex64_uimm8_simm32_float,
		TOP_vmovss_f128_oindex64_uimm8_simm32_float,
		TOP_vmovdqa_f128_oindex64_uimm8_simm32_float,
		TOP_vmovdqu_f128_oindex64_uimm8_simm32_float,
		TOP_vmovq_f128_oindex64_uimm8_simm32_float,
		TOP_vmovupd_f128_oindex64_uimm8_simm32_float,
		TOP_vmovups_f128_oindex64_uimm8_simm32_float,
		TOP_vmovaps_f128_oindex64_uimm8_simm32_float,
		TOP_vmovapd_f128_oindex64_uimm8_simm32_float,
		TOP_vmovntdq_f256_oindex64_uimm8_simm32_float,
		TOP_vmovntpd_f256_oindex64_uimm8_simm32_float,
		TOP_vmovntps_f256_oindex64_uimm8_simm32_float,
		TOP_vmovaps_f256_oindex64_uimm8_simm32_float,
		TOP_vmovapd_f256_oindex64_uimm8_simm32_float,
		TOP_vmovdqa_f256_oindex64_uimm8_simm32_float,
		TOP_vmovdqu_f256_oindex64_uimm8_simm32_float,
		TOP_vmovupd_f256_oindex64_uimm8_simm32_float,
		TOP_vmovups_f256_oindex64_uimm8_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint64_base64_index64_uimm8_simm32 = ISA_Print_Type_Create("oint64_base64_index64_uimm8_simm32",
"%s %s%s(%s,%s,%s) ,%s");
Name();
Segment();
Operand(3);
Operand(0);
Operand(1);
Operand(2);
Result(0);
	Instruction_Print_Group(oint64_base64_index64_uimm8_simm32,
		TOP_vcvtsd2si_int64_oint64_base64_index64_uimm8_simm32,
		TOP_vcvtss2si_int64_oint64_base64_index64_uimm8_simm32,
		TOP_vcvttsd2si_int64_oint64_base64_index64_uimm8_simm32,
		TOP_vcvttss2si_int64_oint64_base64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint32_index64_uimm8_simm32 = ISA_Print_Type_Create("oint32_index64_uimm8_simm32",
"%s %s%s(,%s,%s) ,%s");
Name();
Segment();
Operand(2);
Operand(0);
Operand(1);
Result(0);
	Instruction_Print_Group(oint32_index64_uimm8_simm32,
		TOP_vcvtsd2si_int32_oint32_index64_uimm8_simm32,
		TOP_vcvtss2si_int32_oint32_index64_uimm8_simm32,
		TOP_vcvttsd2si_int32_oint32_index64_uimm8_simm32,
		TOP_vcvttss2si_int32_oint32_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE mxcsr_obase64_index64_uimm8_simm32 = ISA_Print_Type_Create("mxcsr_obase64_index64_uimm8_simm32",
"%s ,%s%s(%s,%s,%s) ");
Name();
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
	Instruction_Print_Group(mxcsr_obase64_index64_uimm8_simm32,
		TOP_vldmxcsr_mxcsr_obase64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_float = ISA_Print_Type_Create("ofloat_float_float",
"%s %s ,%s ,%s");
Name();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_float,
		TOP_vaddpd_f128_ofloat_float_float,
		TOP_vaddpd_f256_ofloat_float_float,
		TOP_vaddps_f128_ofloat_float_float,
		TOP_vaddps_f256_ofloat_float_float,
		TOP_vaddsubpd_f128_ofloat_float_float,
		TOP_vaddsubpd_f256_ofloat_float_float,
		TOP_vaddsubps_f128_ofloat_float_float,
		TOP_vaddsubps_f256_ofloat_float_float,
		TOP_vandpd_f128_ofloat_float_float,
		TOP_vandpd_f256_ofloat_float_float,
		TOP_vandnpd_f128_ofloat_float_float,
		TOP_vandnpd_f256_ofloat_float_float,
		TOP_vandps_f128_ofloat_float_float,
		TOP_vandps_f256_ofloat_float_float,
		TOP_vandnps_f128_ofloat_float_float,
		TOP_vandnps_f256_ofloat_float_float,
		TOP_vdivpd_f128_ofloat_float_float,
		TOP_vdivpd_f256_ofloat_float_float,
		TOP_vdivps_f128_ofloat_float_float,
		TOP_vdivps_f256_ofloat_float_float,
		TOP_vhaddpd_f128_ofloat_float_float,
		TOP_vhaddpd_f256_ofloat_float_float,
		TOP_vhaddps_f128_ofloat_float_float,
		TOP_vhaddps_f256_ofloat_float_float,
		TOP_vhsubpd_f128_ofloat_float_float,
		TOP_vhsubpd_f256_ofloat_float_float,
		TOP_vhsubps_f128_ofloat_float_float,
		TOP_vhsubps_f256_ofloat_float_float,
		TOP_vmaxpd_f128_ofloat_float_float,
		TOP_vmaxpd_f256_ofloat_float_float,
		TOP_vmaxps_f128_ofloat_float_float,
		TOP_vmaxps_f256_ofloat_float_float,
		TOP_vminpd_f128_ofloat_float_float,
		TOP_vminpd_f256_ofloat_float_float,
		TOP_vminps_f128_ofloat_float_float,
		TOP_vminps_f256_ofloat_float_float,
		TOP_vmulpd_f128_ofloat_float_float,
		TOP_vmulpd_f256_ofloat_float_float,
		TOP_vmulps_f128_ofloat_float_float,
		TOP_vmulps_f256_ofloat_float_float,
		TOP_vorpd_f128_ofloat_float_float,
		TOP_vorpd_f256_ofloat_float_float,
		TOP_vorps_f128_ofloat_float_float,
		TOP_vorps_f256_ofloat_float_float,
		TOP_vsubpd_f128_ofloat_float_float,
		TOP_vsubpd_f256_ofloat_float_float,
		TOP_vsubps_f128_ofloat_float_float,
		TOP_vsubps_f256_ofloat_float_float,
		TOP_vunpckhpd_f128_ofloat_float_float,
		TOP_vunpckhpd_f256_ofloat_float_float,
		TOP_vunpckhps_f128_ofloat_float_float,
		TOP_vunpckhps_f256_ofloat_float_float,
		TOP_vunpcklpd_f128_ofloat_float_float,
		TOP_vunpcklpd_f256_ofloat_float_float,
		TOP_vunpcklps_f128_ofloat_float_float,
		TOP_vunpcklps_f256_ofloat_float_float,
		TOP_vxorpd_f128_ofloat_float_float,
		TOP_vxorpd_f256_ofloat_float_float,
		TOP_vxorps_f128_ofloat_float_float,
		TOP_vxorps_f256_ofloat_float_float,
		TOP_vaddsd_f128_ofloat_float_float,
		TOP_vaddss_f128_ofloat_float_float,
		TOP_vcvtsd2ss_f128_ofloat_float_float,
		TOP_vcvtss2sd_f128_ofloat_float_float,
		TOP_vdivsd_f128_ofloat_float_float,
		TOP_vdivss_f128_ofloat_float_float,
		TOP_vmaxsd_f128_ofloat_float_float,
		TOP_vmaxss_f128_ofloat_float_float,
		TOP_vminsd_f128_ofloat_float_float,
		TOP_vminss_f128_ofloat_float_float,
		TOP_vmovhpd_f128_ofloat_float_float,
		TOP_vmovhps_f128_ofloat_float_float,
		TOP_vmulsd_f128_ofloat_float_float,
		TOP_vmulss_f128_ofloat_float_float,
		TOP_vpacksswb_f128_ofloat_float_float,
		TOP_vpackssdw_f128_ofloat_float_float,
		TOP_vpackuswb_f128_ofloat_float_float,
		TOP_vpackusdw_f128_ofloat_float_float,
		TOP_vpaddb_f128_ofloat_float_float,
		TOP_vpaddw_f128_ofloat_float_float,
		TOP_vpaddd_f128_ofloat_float_float,
		TOP_vpaddq_f128_ofloat_float_float,
		TOP_vpaddsb_f128_ofloat_float_float,
		TOP_vpaddsw_f128_ofloat_float_float,
		TOP_vpaddusb_f128_ofloat_float_float,
		TOP_vpaddusw_f128_ofloat_float_float,
		TOP_vpand_f128_ofloat_float_float,
		TOP_vpandn_f128_ofloat_float_float,
		TOP_vpavgb_f128_ofloat_float_float,
		TOP_vpavgw_f128_ofloat_float_float,
		TOP_vpcmpeqb_f128_ofloat_float_float,
		TOP_vpcmpeqw_f128_ofloat_float_float,
		TOP_vpcmpeqd_f128_ofloat_float_float,
		TOP_vpcmpeqq_f128_ofloat_float_float,
		TOP_vpcmpgtb_f128_ofloat_float_float,
		TOP_vpcmpgtw_f128_ofloat_float_float,
		TOP_vpcmpgtd_f128_ofloat_float_float,
		TOP_vpcvpgtq_f128_ofloat_float_float,
		TOP_vpermilps_f128_ofloat_float_float,
		TOP_vphaddw_f128_ofloat_float_float,
		TOP_vphaddd_f128_ofloat_float_float,
		TOP_vphaddsw_f128_ofloat_float_float,
		TOP_vphsubw_f128_ofloat_float_float,
		TOP_vphsubd_f128_ofloat_float_float,
		TOP_vphsubsw_f128_ofloat_float_float,
		TOP_vpmaddwd_f128_ofloat_float_float,
		TOP_vpmaddubsw_f128_ofloat_float_float,
		TOP_vpmaxsb_f128_ofloat_float_float,
		TOP_vpmaxsw_f128_ofloat_float_float,
		TOP_vpmaxsd_f128_ofloat_float_float,
		TOP_vpmaxub_f128_ofloat_float_float,
		TOP_vpmaxuw_f128_ofloat_float_float,
		TOP_vpmaxud_f128_ofloat_float_float,
		TOP_vpminsb_f128_ofloat_float_float,
		TOP_vpminsw_f128_ofloat_float_float,
		TOP_vpminsd_f128_ofloat_float_float,
		TOP_vpminub_f128_ofloat_float_float,
		TOP_vpminuw_f128_ofloat_float_float,
		TOP_vpminud_f128_ofloat_float_float,
		TOP_vpmulhuw_f128_ofloat_float_float,
		TOP_vpmulhrsw_f128_ofloat_float_float,
		TOP_vpmulhw_f128_ofloat_float_float,
		TOP_vpmullw_f128_ofloat_float_float,
		TOP_vpmulld_f128_ofloat_float_float,
		TOP_vpmuludq_f128_ofloat_float_float,
		TOP_vpmuldq_f128_ofloat_float_float,
		TOP_vpor_f128_ofloat_float_float,
		TOP_vpsadbw_f128_ofloat_float_float,
		TOP_vpshufb_f128_ofloat_float_float,
		TOP_vpsignb_f128_ofloat_float_float,
		TOP_vpsignw_f128_ofloat_float_float,
		TOP_vpsignd_f128_ofloat_float_float,
		TOP_vpsllw_f128_ofloat_float_float,
		TOP_vpslld_f128_ofloat_float_float,
		TOP_vpsrad_f128_ofloat_float_float,
		TOP_vpsrlw_f128_ofloat_float_float,
		TOP_vpsrld_f128_ofloat_float_float,
		TOP_vpsubb_f128_ofloat_float_float,
		TOP_vpsubw_f128_ofloat_float_float,
		TOP_vpsubd_f128_ofloat_float_float,
		TOP_vpsubq_f128_ofloat_float_float,
		TOP_vpsubsb_f128_ofloat_float_float,
		TOP_vpsubsw_f128_ofloat_float_float,
		TOP_vpsubusb_f128_ofloat_float_float,
		TOP_vpsubusw_f128_ofloat_float_float,
		TOP_vpunpckhbw_f128_ofloat_float_float,
		TOP_vpunpckhwd_f128_ofloat_float_float,
		TOP_vpunpckhdq_f128_ofloat_float_float,
		TOP_vpunpckhqdq_f128_ofloat_float_float,
		TOP_vpunpcklbw_f128_ofloat_float_float,
		TOP_vpunpcklwd_f128_ofloat_float_float,
		TOP_vpunpckldq_f128_ofloat_float_float,
		TOP_vpunpcklqdq_f128_ofloat_float_float,
		TOP_vpxor_f128_ofloat_float_float,
		TOP_vrcpss_f128_ofloat_float_float,
		TOP_vrsqrtss_f128_ofloat_float_float,
		TOP_vsqrtsd_f128_ofloat_float_float,
		TOP_vsqrtss_f128_ofloat_float_float,
		TOP_vsubsd_f128_ofloat_float_float,
		TOP_vsubss_f128_ofloat_float_float,
		TOP_vmovhlps_f128_ofloat_float_float,
		TOP_vmovlhps_f128_ofloat_float_float,
		TOP_vmovsd_f128_ofloat_float_float,
		TOP_vmovss_f128_ofloat_float_float,
		TOP_vpermilps_f256_ofloat_float_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_int64_simm8 = ISA_Print_Type_Create("ofloat_float_int64_simm8",
"%s %s ,%s ,%s ,%s");
Name();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_int64_simm8,
		TOP_vpinsrq_f128_ofloat_float_int64_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_index64_uimm8_simm32_float_float = ISA_Print_Type_Create("obase64_index64_uimm8_simm32_float_float",
"%s %s ,%s ,%s%s(%s,%s,%s) ");
Name();
Operand(1);
Operand(0);
Segment();
Operand(5);
Operand(2);
Operand(3);
Operand(4);
	Instruction_Print_Group(obase64_index64_uimm8_simm32_float_float,
		TOP_vmaskmovps_f128_obase64_index64_uimm8_simm32_float_float,
		TOP_vmaskmovps_f256_obase64_index64_uimm8_simm32_float_float,
		TOP_vmaskmovpd_f128_obase64_index64_uimm8_simm32_float_float,
		TOP_vmaskmovpd_f256_obase64_index64_uimm8_simm32_float_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_index64_uimm8_simm32 = ISA_Print_Type_Create("ofloat_float_index64_uimm8_simm32",
"%s %s ,%s%s(,%s,%s) ,%s");
Name();
Segment();
Operand(3);
Operand(1);
Operand(2);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_index64_uimm8_simm32,
		TOP_vaddpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vaddpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vaddps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vaddps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vaddsubpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vaddsubpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vaddsubps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vaddsubps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vandpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vandpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vandnpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vandnpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vandps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vandps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vandnps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vandnps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vdivpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vdivpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vdivps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vdivps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vhaddpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vhaddpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vhaddps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vhaddps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vhsubpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vhsubpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vhsubps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vhsubps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vmaxpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmaxpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vmaxps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmaxps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vminpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vminpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vminps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vminps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vmulpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmulpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vmulps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmulps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vorpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vorpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vorps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vorps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vsubpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vsubpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vsubps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vsubps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vunpckhpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vunpckhpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vunpckhps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vunpckhps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vunpcklpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vunpcklpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vunpcklps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vunpcklps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vxorpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vxorpd_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vxorps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vxorps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_vaddsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vaddss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vcvtsd2ss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vcvtss2sd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vdivsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vdivss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmaxsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmaxss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vminsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vminss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmovhpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmovhps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmulsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vmulss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpacksswb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpackssdw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpackuswb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpackusdw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddsb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddusb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpaddusw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpand_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpandn_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpavgb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpavgw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpeqb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpeqw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpeqd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpeqq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpgtb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpgtw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcmpgtd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpcvpgtq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpermilps_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vphaddw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vphaddd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vphaddsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vphsubw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vphsubd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vphsubsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaddwd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaddubsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaxsb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaxsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaxsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaxub_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaxuw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmaxud_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpminsb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpminsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpminsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpminub_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpminuw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpminud_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmulhuw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmulhrsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmulhw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmullw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmulld_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmuludq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpmuldq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpor_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsadbw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpshufb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsignb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsignw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsignd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsllw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpslld_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsrad_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsrlw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsrld_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubsb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubsw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubusb_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpsubusw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpckhbw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpckhwd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpckhdq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpckhqdq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpcklbw_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpcklwd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpckldq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpunpcklqdq_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpxor_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vrcpss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vrsqrtss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vsqrtsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vsqrtss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vsubsd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vsubss_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vcvtsi2sd_int32_ofloat_float_index64_uimm8_simm32,
		TOP_vcvtsi2ss_int32_ofloat_float_index64_uimm8_simm32,
		TOP_vcvtsi2sd_int64_ofloat_float_index64_uimm8_simm32,
		TOP_vcvtsi2ss_int64_ofloat_float_index64_uimm8_simm32,
		TOP_vmovlpd_f128_ofloat_float_index64_uimm8_simm32,
		TOP_vpermilps_f256_ofloat_float_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_index64_uimm8_simm32_simm8 = ISA_Print_Type_Create("ofloat_float_index64_uimm8_simm32_simm8",
"%s %s ,%s%s(,%s,%s) ,%s ,%s");
Name();
Operand(4);
Segment();
Operand(3);
Operand(1);
Operand(2);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vblendpd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vblendpd_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vblendps_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vblendps_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vcmppd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vcmppd_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vcmpps_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vcmpps_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vdpps_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vdpps_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vroundpd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vroundpd_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vroundps_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vroundps_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vshufpd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vshufpd_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vshufps_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vshufps_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vcmpsd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vcmpss_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vdppd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vinsertf128_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vinsertps_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vmpsadbw_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpalignr_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpblendw_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpclmulqdq_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vroundsd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vroundss_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vperm2f128_f256_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpinsrb_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpinsrw_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpinsrd_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_vpinsrq_f128_ofloat_float_index64_uimm8_simm32_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint32_base64_index64_uimm8_simm32 = ISA_Print_Type_Create("oint32_base64_index64_uimm8_simm32",
"%s %s%s(%s,%s,%s) ,%s");
Name();
Segment();
Operand(3);
Operand(0);
Operand(1);
Operand(2);
Result(0);
	Instruction_Print_Group(oint32_base64_index64_uimm8_simm32,
		TOP_vcvtsd2si_int32_oint32_base64_index64_uimm8_simm32,
		TOP_vcvtss2si_int32_oint32_base64_index64_uimm8_simm32,
		TOP_vcvttsd2si_int32_oint32_base64_index64_uimm8_simm32,
		TOP_vcvttss2si_int32_oint32_base64_index64_uimm8_simm32,
		TOP_UNDEFINED);


ISA_PRINT_TYPE obase64_index64_uimm8_simm32_float = ISA_Print_Type_Create("obase64_index64_uimm8_simm32_float",
"%s %s ,%s%s(%s,%s,%s) ");
Name();
Operand(0);
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
	Instruction_Print_Group(obase64_index64_uimm8_simm32_float,
		TOP_vmaskmovdqu_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovlps_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovntpd_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovntps_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovsd_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovss_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovdqa_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovdqu_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovq_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovupd_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovups_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovaps_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovapd_f128_obase64_index64_uimm8_simm32_float,
		TOP_vmovntdq_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovntpd_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovntps_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovaps_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovapd_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovdqa_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovdqu_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovupd_f256_obase64_index64_uimm8_simm32_float,
		TOP_vmovups_f256_obase64_index64_uimm8_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_base64_index64_uimm8_simm32_float = ISA_Print_Type_Create("ofloat_float_base64_index64_uimm8_simm32_float",
"%s %s ,%s%s(%s,%s,%s) ,%s ,%s");
Name();
Operand(5);
Segment();
Operand(4);
Operand(1);
Operand(2);
Operand(3);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_base64_index64_uimm8_simm32_float,
		TOP_vblendvpd_f128_ofloat_float_base64_index64_uimm8_simm32_float,
		TOP_vblendvpd_f256_ofloat_float_base64_index64_uimm8_simm32_float,
		TOP_vblendvps_f128_ofloat_float_base64_index64_uimm8_simm32_float,
		TOP_vblendvps_f256_ofloat_float_base64_index64_uimm8_simm32_float,
		TOP_vpblendvb_f128_ofloat_float_base64_index64_uimm8_simm32_float,
		TOP_UNDEFINED);


ISA_PRINT_TYPE oint32_float_simm8 = ISA_Print_Type_Create("oint32_float_simm8",
"%s %s ,%s ,%s");
Name();
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(oint32_float_simm8,
		TOP_f128_oint32_float_simm8,
		TOP_vpextrb_int32_oint32_float_simm8,
		TOP_vpextrw_int32_oint32_float_simm8,
		TOP_UNDEFINED);


ISA_PRINT_TYPE ofloat_float_base64_simm32_float = ISA_Print_Type_Create("ofloat_float_base64_simm32_float",
"%s %s ,%s%s(%s) ,%s ,%s");
Name();
Operand(3);
Segment();
Operand(2);
Operand(1);
Operand(0);
Result(0);
	Instruction_Print_Group(ofloat_float_base64_simm32_float,
		TOP_vblendvpd_f128_ofloat_float_base64_simm32_float,
		TOP_vblendvpd_f256_ofloat_float_base64_simm32_float,
		TOP_vblendvps_f128_ofloat_float_base64_simm32_float,
		TOP_vblendvps_f256_ofloat_float_base64_simm32_float,
		TOP_vpblendvb_f128_ofloat_float_base64_simm32_float,
		TOP_UNDEFINED);


