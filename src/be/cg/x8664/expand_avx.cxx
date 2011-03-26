case INTRN_VSQRTPD256:
break;
case INTRN_VBLENDPD256:
break;
case INTRN_VPERMILPS128:
break;
case INTRN_VADDSUBPS256:
  Build_OP(TOP_vaddsubps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VUNPCKLPD256:
  Build_OP(TOP_vunpcklpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VZEROALL256:
break;
case INTRN_VCVTTPD2DQ256:
break;
case INTRN_VINSERTF128D:
break;
case INTRN_VCVTTPS2DQ256:
break;
case INTRN_VMULPS256:
  Build_OP(TOP_vmulps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VUNPCKHPD256:
  Build_OP(TOP_vunpckhpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMASKMOVPS128ST:
break;
case INTRN_VMOVUPD256:
break;
case INTRN_VCMPSD:
break;
case INTRN_VMOVUPS256:
break;
case INTRN_VSUBPS256:
  Build_OP(TOP_vsubps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VHADDPD256:
  Build_OP(TOP_vhaddpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPD128C:
break;
case INTRN_VADDPS256:
  Build_OP(TOP_vaddps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPS256Z:
break;
case INTRN_VEXTRACTF128D:
break;
case INTRN_VPERMILPD128:
break;
case INTRN_VANDPS256:
  Build_OP(TOP_vandps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VCMPPS256:
break;
case INTRN_VMOVDQA256:
break;
case INTRN_VCVTPD2DQ256:
break;
case INTRN_VANDPD256:
  Build_OP(TOP_vandpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VRCPPS256:
break;
case INTRN_VHADDPS:
  Build_OP(TOP_vhaddps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMAXPD256:
  Build_OP(TOP_vmaxpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VMASKMOVPD256:
break;
case INTRN_VBLENDPS256:
break;
case INTRN_VCASTSI128TOSI256:
break;
case INTRN_VUNPCKHPS256:
  Build_OP(TOP_vunpckhps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPS256C:
break;
case INTRN_VINSERTF128F:
break;
case INTRN_VCASTPS256TOPS128:
break;
case INTRN_VMASKMOVPD128ST:
break;
case INTRN_VHSUBPS256:
  Build_OP(TOP_vhsubps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VPERMILPD256:
break;
case INTRN_VMASKMOVPD128:
break;
case INTRN_VMOVDQA256ST:
break;
case INTRN_VCVTDQ2PD256:
break;
case INTRN_VADDPD256:
  Build_OP(TOP_vaddpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VINSERTF128I:
break;
case INTRN_VMOVSLDUP256:
break;
case INTRN_VPERMILPS256:
break;
case INTRN_VCVTPD2PS256:
break;
case INTRN_VROUNDPD256:
break;
case INTRN_VPERMILPD128VAR:
break;
case INTRN_VMASKMOVPD256ST:
break;
case INTRN_VPERM2F128S:
break;
case INTRN_VMULPD256:
  Build_OP(TOP_vmulpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VSUBPD256:
  Build_OP(TOP_vsubpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VPERM2F128D:
break;
case INTRN_VTESTPD256NZC:
break;
case INTRN_VCVTPS2DQ256:
break;
case INTRN_VRSQRTPS256:
break;
case INTRN_VDPPS256:
break;
case INTRN_VMAXPS256:
  Build_OP(TOP_vmaxps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VBROADCAST128S:
break;
case INTRN_VTESTPD256C:
break;
case INTRN_VMASKMOVPS128:
break;
case INTRN_VBROADCASTSS256:
break;
case INTRN_VMOVSHDUP256:
break;
case INTRN_VADDSUBPD256:
  Build_OP(TOP_vaddsubpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VSQRTPS256:
break;
case INTRN_VTESTPS128NZC:
break;
case INTRN_VBLENDVPD256:
break;
case INTRN_VMINPS256:
  Build_OP(TOP_vminps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VTESTPD128NZC:
break;
case INTRN_VXORPS256:
  Build_OP(TOP_vxorps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VSHUFPS256:
break;
case INTRN_VCASTPD256TOPD128:
break;
case INTRN_VEXTRACTF128S:
break;
case INTRN_VPERMILPD256VAR:
break;
case INTRN_VHSUBPD256:
  Build_OP(TOP_vhsubpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VZEROUPPER256:
break;
case INTRN_VCMPPD256:
break;
case INTRN_VBROADCAST128D:
break;
case INTRN_VMOVMSKPD256:
break;
case INTRN_VMASKMOVPS256:
break;
case INTRN_VMOVMSKPS256:
break;
case INTRN_VCASTPD128TOPD256:
break;
case INTRN_VDIVPS256:
  Build_OP(TOP_vdivps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VCASTPS128TOPS256:
break;
case INTRN_VMINPD256:
  Build_OP(TOP_vminpd_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VLDDQU256:
break;
case INTRN_VPERM2F128SI:
break;
case INTRN_VTESTPS128C:
break;
case INTRN_VCASTSI256TOSI128:
break;
case INTRN_VROADCASTSS128:
break;
case INTRN_VCMPPS128:
break;
case INTRN_VMOVNTDQ256:
break;
case INTRN_VUNPCKLPS256:
  Build_OP(TOP_vunpcklps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VORPS256:
  Build_OP(TOP_vorps_f256_ofloat_float_float, result, op0, op1, ops);
break;
case INTRN_VBLENDVPS256:
break;
case INTRN_VPERMILPS256VAR:
break;
case INTRN_VCMPSS128:
break;
case INTRN_VEXTRACTF128I:
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
break;
case INTRN_VPERMILPS128VAR:
break;
case INTRN_VMASKMOVPS256ST:
break;
case INTRN_VTESTPD256:
break;
case INTRN_VTESTPS256NZC:
break;
case INTRN_VCMPPD128:
break;
case INTRN_VTESTPD128:
break;
case INTRN_VBROADCASTSD256:
break;
case INTRN_VCVTPS2PD256:
break;
case INTRN_VMOVDDUP256:
break;
case INTRN_VTESTPS128Z:
break;
