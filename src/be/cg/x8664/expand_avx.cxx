case INTRN_VSQRTPD256:
  Build_OP(TOP_vsqrtpd_f256_ofloat_float, result,op0,ops);
break;
case INTRN_VBLENDPD256:
  Expand_Intrinsic_Imm_Param(TOP_vblendpd_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VPERMILPS128:
  Expand_Intrinsic_Imm_Opnd2(TOP_vpermilps_f128_ofloat_float_simm8,result,op0,op1,ops,4);
break;
case INTRN_VADDSUBPS256:
  Build_OP(TOP_vaddsubps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VUNPCKLPD256:
  Build_OP(TOP_vunpcklpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VZEROALL256:
  Build_OP(TOP_vzeroall_null,ops);
break;
case INTRN_VCVTTPD2DQ256:
  Build_OP(TOP_vcvttpd2dq_f256_ofloat_float, result,op0,ops);
break;
case INTRN_VINSERTF128D:
  Expand_Intrinsic_Imm_Param(TOP_vinsertf128_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VCVTTPS2DQ256:
  Build_OP(TOP_vcvttps2dq_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VMULPS256:
  Build_OP(TOP_vmulps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VUNPCKHPD256:
  Build_OP(TOP_vunpckhpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMASKMOVPS128ST:
  Build_OP(TOP_vmaskmovps_f128_obase64_simm32_float_float, op0,Gen_Literal_TN(0,1),op2,op3,ops);
break;
case INTRN_VMOVUPD256:
  Build_OP(TOP_vmovupd_f256_ofloat_base64_simm32,result,op0,Gen_Literal_TN(0,1), ops);
break;
case INTRN_VCMPSD:
  Expand_Intrinsic_Imm_Param(TOP_vcmpsd_f128_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VMOVUPS256:
  Build_OP(TOP_vmovups_f256_ofloat_base64_simm32, result,op0,Gen_Literal_TN(0,1),ops);
break;
case INTRN_VSUBPS256:
  Build_OP(TOP_vsubps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VHADDPD256:
  Build_OP(TOP_vhaddpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPD128C:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestpd_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_setb,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VADDPS256:
  Build_OP(TOP_vaddps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPS256Z:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestps_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_sete, Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VEXTRACTF128D:
  Expand_Intrinsic_Imm_Opnd2(TOP_vextractf128_f256_ofloat_float_simm8,result,op0,op1,ops,4);
break;
case INTRN_VPERMILPD128:
  Expand_Intrinsic_Imm_Opnd2(TOP_vpermilpd_f256_ofloat_float_simm8, result, op0, op1, ops, 4);
break;
case INTRN_VANDPS256:
  Build_OP(TOP_vandps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VCMPPS256:
  Expand_Intrinsic_Imm_Param(TOP_vcmpps_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VMOVDQA256:
  //TODO not a intrinsic op
break;
case INTRN_VCVTPD2DQ256:
  Build_OP(TOP_vcvtpd2dq_f128_f256_ofloat_float,result,op0,ops);
break;
case INTRN_VANDPD256:
  Build_OP(TOP_vandpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VRCPPS256:
  Build_OP(TOP_vrcpps_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VHADDPS:
  Build_OP(TOP_vhaddps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMAXPD256:
  Build_OP(TOP_vmaxpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMASKMOVPD256:
  Build_OP(TOP_vmaskmovpd_f256_ofloat_float_base64_simm32,result,op0,op1,Gen_Literal_TN(0,1),ops);
break;
case INTRN_VBLENDPS256:
  Expand_Intrinsic_Imm_Param(TOP_vblendpd_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VCASTSI128TOSI256:
 Build_OP(TOP_vmovdqu_f256_ofloat_float, result,op0, ops); 
break;
case INTRN_VUNPCKHPS256:
  Build_OP(TOP_vunpckhps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPS256C:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestps_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_setb,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VINSERTF128F:
  Expand_Intrinsic_Imm_Param(TOP_vinsertf128_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VCASTPS256TOPS128:
  Build_OP(TOP_vmovups_f256_ofloat_float,result,op0,ops);
break;
case INTRN_VMASKMOVPD128ST:
  //TODO store op
break;
case INTRN_VHSUBPS256:
  Build_OP(TOP_vhsubps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VPERMILPD256:
  Expand_Intrinsic_Imm_Opnd2(TOP_vpermilpd_f256_ofloat_float_simm8,result,op0,op1,ops,4);
break;
case INTRN_VMASKMOVPD128:
  Build_OP(TOP_vmaskmovpd_f128_ofloat_float_base64_simm32, result, op0,op1, Gen_Literal_TN(0,4),ops );
break;
case INTRN_VMOVDQA256ST:
  //TODO store op
break;
case INTRN_VCVTDQ2PD256:
  Build_OP(TOP_vcvtdq2pd_f256_ofloat_float, result,op0, ops);
break;
case INTRN_VADDPD256:
  Build_OP(TOP_vaddpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VINSERTF128I:
  Expand_Intrinsic_Imm_Param(TOP_vinsertf128_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VMOVSLDUP256:
  Build_OP(TOP_vmovsldup_f256_ofloat_base64_simm32, result, op0, Gen_Literal_TN(0,1),ops);
break;
case INTRN_VPERMILPS256:
  Expand_Intrinsic_Imm_Opnd2(TOP_vpermilps_f256_ofloat_float_simm8, result, op0, op1, ops,4);
break;
case INTRN_VCVTPD2PS256:
  Build_OP(TOP_vcvtpd2ps_f128_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VROUNDPD256:
  Expand_Intrinsic_Imm_Opnd2(TOP_vroundpd_f256_ofloat_float_float_simm8, result, op0, op1, ops, 4);
break;
case INTRN_VROUNDPS256:
  Expand_Intrinsic_Imm_Opnd2(TOP_vroundps_f256_ofloat_float_float_simm8, result, op0, op1, ops, 4);
break;
case INTRN_VPERMILPD128VAR:
  Build_OP(TOP_vpermilpd_f128_ofloat_float_float, result,op0, op1,ops);
break;
case INTRN_VMASKMOVPD256ST:
  //TODO store
break;
case INTRN_VPERM2F128S:
  Expand_Intrinsic_Imm_Param(TOP_vperm2f128_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VMULPD256:
  Build_OP(TOP_vmulpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VSUBPD256:
  Build_OP(TOP_vsubpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VPERM2F128D:
  Expand_Intrinsic_Imm_Param(TOP_vperm2f128_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VTESTPD256NZC:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestpd_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_seta,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VCVTPS2DQ256:
  Build_OP(TOP_vcvtps2dq_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VRSQRTPS256:
  Build_OP(TOP_vrsqrtps_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VDPPS256:
  Expand_Intrinsic_Imm_Param(TOP_vdpps_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VMAXPS256:
  Build_OP(TOP_vmaxps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VBROADCAST128S:
  Build_OP(TOP_vbroadcastf128_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VTESTPD256C:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestpd_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_setb,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VMASKMOVPS128:
  Build_OP(TOP_vmaskmovps_f256_ofloat_float_base64_simm32, result,op0, op1, Gen_Literal_TN(0, 1),ops);
break;
case INTRN_VBROADCASTSS256:
  Build_OP(TOP_vbroadcastss_f256_ofloat_base64_simm32, result, op0, Gen_Literal_TN(0,1),ops);
break;
case INTRN_VMOVSHDUP256:
  Build_OP(TOP_vmovshdup_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VADDSUBPD256:
  Build_OP(TOP_vaddsubpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VSQRTPS256:
  Build_OP(TOP_vsqrtps_f256_ofloat_float, result,op0, op1, ops);
break;
case INTRN_VTESTPS128NZC:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestps_f128_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_seta, Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VBLENDVPD256:
  Build_OP(TOP_vblendvpd_f256_ofloat_float_float_float, result, op0, op1,op2, ops);
break;
case INTRN_VMINPS256:
  Build_OP(TOP_vminps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPD128NZC:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestpd_f128_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_seta,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VXORPS256:
  Build_OP(TOP_vxorps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VSHUFPS256:
  Expand_Intrinsic_Imm_Param(TOP_vshufps_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VSHUFPD25:
  Expand_Intrinsic_Imm_Param(TOP_vshufpd_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VCASTPD256TOPD128:
  Build_OP(TOP_vmovupd_f256_ofloat_float, result, op0,ops);
break;
case INTRN_VEXTRACTF128S:
  Expand_Intrinsic_Imm_Opnd2(TOP_vextractf128_f256_ofloat_float_simm8, result, op0, op1, ops,4);
break;
case INTRN_VPERMILPD256VAR:
  Build_OP(TOP_vpermilpd_f128_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VHSUBPD256:
  Build_OP(TOP_vhsubpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VZEROUPPER256:
  //TODO same situation with store
break;
case INTRN_VCMPPD256:
  Expand_Intrinsic_Imm_Param(TOP_vcmppd_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VBROADCAST128D:
  Build_OP(TOP_vbroadcastf128_f256_ofloat_base64_simm32, result, op0, Gen_Literal_TN(0,1),ops);
break;
case INTRN_VMOVMSKPD256:
  Build_OP(TOP_vmovmskpd_f256_oint32_float, result, op0, ops);
break;
case INTRN_VMASKMOVPS256:
  //TODO store
break;
case INTRN_VMOVMSKPS256:
  Build_OP(TOP_vmovmskps_f256_oint32_float, result, op0, ops);
break;
case INTRN_VCASTPD128TOPD256:
  Build_OP(TOP_vmovapd_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VDIVPS256:
  Build_OP(TOP_vdivps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VCASTPS128TOPS256:
  Build_OP(TOP_vmovups_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VMINPD256:
  Build_OP(TOP_vminpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VLDDQU256:
  Build_OP(TOP_vlddqu_f256_ofloat_base64_simm32, result, op0, Gen_Literal_TN(0,1),ops);
break;
case INTRN_VPERM2F128SI:
  Expand_Intrinsic_Imm_Param(TOP_vperm2f128_f256_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VTESTPS128C:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestps_f128_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_setb, Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VCASTSI256TOSI128:
  Build_OP(TOP_vmovdqu_f256_ofloat_float,result, op0, ops);
break;
case INTRN_VROADCASTSS128:
  Build_OP(TOP_vbroadcastss_f256_ofloat_base64_simm32, result, op0, Gen_Literal_TN(0,1),ops);
break;
case INTRN_VCMPPS128:
  Expand_Intrinsic_Imm_Param(TOP_vcmpps_f128_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VMOVNTDQ256:
  //TODO STORE
break;
case INTRN_VUNPCKLPS256:
  Build_OP(TOP_vunpcklps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VORPS256:
  Build_OP(TOP_vorps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VBLENDVPS256:
  Build_OP(TOP_vblendvps_f256_ofloat_float_float_float, result, op0, op1,op2, ops);
break;
case INTRN_VPERMILPS256VAR:
  Build_OP(TOP_vpermilps_f256_ofloat_float_float, result, op0, op1, op2, ops);
break;
case INTRN_VCMPSS128:
  Expand_Intrinsic_Imm_Param(TOP_vcmpss_f128_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VEXTRACTF128I:
  Expand_Intrinsic_Imm_Opnd2(TOP_vextractf128_f256_ofloat_float_simm8, result, op0, op1 ,ops, 4);
break;
case INTRN_VORPD256:
  Build_OP(TOP_vorpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VANDNPS256:
  Build_OP(TOP_vandnps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VDIVPD256:
  Build_OP(TOP_vdivpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VANDNPD256:
  Build_OP(TOP_vandnpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VXORPD256:
  Build_OP(TOP_vxorpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VCVTDQ2PS256:
  Build_OP(TOP_vcvtdq2ps256_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VPERMILPS128VAR:
  Build_OP(TOP_vpermilps_f128_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMASKMOVPS256ST:
  //TODO store;
break;
case INTRN_VTESTPD256:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestpd_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_sete,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VTESTPS256NZC:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestps_f256_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_seta, Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VCMPPD128:
  Expand_Intrinsic_Imm_Param(TOP_vcmppd_f128_ofloat_float_float_simm8,result, op0, op1, op2, ops, 5);
break;
case INTRN_VTESTPD128:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestpd_f128_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_sete,Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
case INTRN_VBROADCASTSD256:
  Build_OP(TOP_vbroadcastsd_f256_ofloat_base64_simm32, result, op0, Gen_Literal_TN(0,1),ops);
break;
case INTRN_VCVTPS2PD256:
  Build_OP(TOP_vcvtps2pd_f256_ofloat_float, result, op0,ops);
break;
case INTRN_VMOVDDUP256:
  Build_OP(TOP_vmovddup_f256_ofloat_float, result, op0, ops);
break;
case INTRN_VTESTPS128Z:{
	TN *tneax = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 4);
	TN *tn0 = Gen_Literal_TN(0,4);
  Build_OP(TOP_vtestps_f128_oint32rflag_float_float,result,op0,op1,ops);
	Build_OP(TOP_sete, Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, RAX, 2), Rflags_TN(), ops);
	Build_OP(TOP_mov32,result,tneax,tn0,ops);
}
break;
