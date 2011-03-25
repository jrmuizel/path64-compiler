ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_sqrtpd256",ftype,IX86_BUILTIN_VSQRTPD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendpd256",ftype,IX86_BUILTIN_VBLENDPD256);

ftype = build_function_type_list(V4SF_type_node, V4SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilps",ftype,IX86_BUILTIN_VPERMILPS128);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addsubps256",ftype,IX86_BUILTIN_VADDSUBPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_unpcklpd256",ftype,IX86_BUILTIN_VUNPCKLPD256);

ftype = build_function_type_list(void_type_node, void_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vzeroall",ftype,IX86_BUILTIN_VZEROALL256);

ftype = build_function_type_list(V4SI_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvttpd2dq256",ftype,IX86_BUILTIN_VCVTTPD2DQ256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V4SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vinsertf128_ps256",ftype,IX86_BUILTIN_VINSERTF128D);

ftype = build_function_type_list(V8SI_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvttps2dq256",ftype,IX86_BUILTIN_VCVTTPS2DQ256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_mulps256",ftype,IX86_BUILTIN_VMULPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_unpckhpd256",ftype,IX86_BUILTIN_VUNPCKHPD256);

ftype = build_function_type_list(void_type_node, pv4sf_type_node, V4SF_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskstoreps",ftype,IX86_BUILTIN_VMASKMOVPS128ST);

ftype = build_function_type_list(V4DF_type_node, pdouble_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_loadupd256",ftype,IX86_BUILTIN_VMOVUPD256);

ftype = build_function_type_list(V2DF_type_node, V2DF_type_node, V2DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cmpsd",ftype,IX86_BUILTIN_VCMPSD);

ftype = build_function_type_list(V8SF_type_node, pfloat_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_loadups256",ftype,IX86_BUILTIN_VMOVUPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_subps256",ftype,IX86_BUILTIN_VSUBPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_haddpd256",ftype,IX86_BUILTIN_VHADDPD256);

ftype = build_function_type_list(integer_type_node, V2DF_type_node, V2DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestcpd",ftype,IX86_BUILTIN_VTESTPD128C);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addps256",ftype,IX86_BUILTIN_VADDPS256);

ftype = build_function_type_list(integer_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestzps256",ftype,IX86_BUILTIN_VTESTPS256Z);

ftype = build_function_type_list(V2DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vextractf128_pd256",ftype,IX86_BUILTIN_VEXTRACTF128D);

ftype = build_function_type_list(V2DF_type_node, V2DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilpd",ftype,IX86_BUILTIN_VPERMILPD128);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andps256",ftype,IX86_BUILTIN_VANDPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cmpps",ftype,IX86_BUILTIN_VCMPPS256);

ftype = build_function_type_list(V8SI_type_node, pchar_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_loaddqu256",ftype,IX86_BUILTIN_VMOVDQA256);

ftype = build_function_type_list(V4SI_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvtpd2dq256",ftype,IX86_BUILTIN_VCVTPD2DQ256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andpd256",ftype,IX86_BUILTIN_VANDPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_rcpps256",ftype,IX86_BUILTIN_VRCPPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_haddps256",ftype,IX86_BUILTIN_VHADDPS);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maxpd256",ftype,IX86_BUILTIN_VMAXPD256);

ftype = build_function_type_list(V4DF_type_node, pv4df_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskloadpd256",ftype,IX86_BUILTIN_VMASKMOVPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendps256",ftype,IX86_BUILTIN_VBLENDPS256);

ftype = build_function_type_list(V8SI_type_node, V4SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_si256_si",ftype,IX86_BUILTIN_VCASTSI128TOSI256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_unpckhps256",ftype,IX86_BUILTIN_VUNPCKHPS256);

ftype = build_function_type_list(integer_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestcps256",ftype,IX86_BUILTIN_VTESTPS256C);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V2DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vinsertf128_pd256",ftype,IX86_BUILTIN_VINSERTF128F);

ftype = build_function_type_list(V4SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_ps_ps256",ftype,IX86_BUILTIN_VCASTPS256TOPS128);

ftype = build_function_type_list(void_type_node, pv2df_type_node, V2DF_type_node, V2DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskstorepd",ftype,IX86_BUILTIN_VMASKMOVPD128ST);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_hsubps256",ftype,IX86_BUILTIN_VHSUBPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilpd256",ftype,IX86_BUILTIN_VPERMILPD256);

ftype = build_function_type_list(V2DF_type_node, pv2df_type_node, V2DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskloadpd",ftype,IX86_BUILTIN_VMASKMOVPD128);

ftype = build_function_type_list(void_type_node, pchar_type_node, V8SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_storedqu256",ftype,IX86_BUILTIN_VMOVDQA256ST);

ftype = build_function_type_list(V4DF_type_node, V4SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvtdq2pd256",ftype,IX86_BUILTIN_VCVTDQ2PD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addpd256",ftype,IX86_BUILTIN_VADDPD256);

ftype = build_function_type_list(V8SI_type_node, V8SI_type_node, V4SI_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vinsertf128_si256",ftype,IX86_BUILTIN_VINSERTF128I);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_movsldup256",ftype,IX86_BUILTIN_VMOVSLDUP256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilps256",ftype,IX86_BUILTIN_VPERMILPS256);

ftype = build_function_type_list(V4SF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvtpd2ps256",ftype,IX86_BUILTIN_VCVTPD2PS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_roundpd256",ftype,IX86_BUILTIN_VROUNDPD256);

ftype = build_function_type_list(V2DF_type_node, V2DF_type_node, V4SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilvarpd",ftype,IX86_BUILTIN_VPERMILPD128VAR);

ftype = build_function_type_list(void_type_node, pv4df_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskstorepd256",ftype,IX86_BUILTIN_VMASKMOVPD256ST);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vperm2f128_ps256",ftype,IX86_BUILTIN_VPERM2F128S);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_mulpd256",ftype,IX86_BUILTIN_VMULPD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_subpd256",ftype,IX86_BUILTIN_VSUBPD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vperm2f128_pd256",ftype,IX86_BUILTIN_VPERM2F128D);

ftype = build_function_type_list(integer_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestnzcpd256",ftype,IX86_BUILTIN_VTESTPD256NZC);

ftype = build_function_type_list(V4DI_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvtps2dq256",ftype,IX86_BUILTIN_VCVTPS2DQ256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_rsqrtps256",ftype,IX86_BUILTIN_VRSQRTPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_dpps256",ftype,IX86_BUILTIN_VDPPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maxps256",ftype,IX86_BUILTIN_VMAXPS256);

ftype = build_function_type_list(V8SF_type_node, pv4sf_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vbroadcastf128_ps256",ftype,IX86_BUILTIN_VBROADCAST128S);

ftype = build_function_type_list(integer_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestcpd256",ftype,IX86_BUILTIN_VTESTPD256C);

ftype = build_function_type_list(V4SF_type_node, pv4sf_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskloadps",ftype,IX86_BUILTIN_VMASKMOVPS128);

ftype = build_function_type_list(V8SF_type_node, pfloat_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vbroadcastss256",ftype,IX86_BUILTIN_VBROADCASTSS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_movshdup256",ftype,IX86_BUILTIN_VMOVSHDUP256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addsubpd256",ftype,IX86_BUILTIN_VADDSUBPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_sqrtps256",ftype,IX86_BUILTIN_VSQRTPS256);

ftype = build_function_type_list(integer_type_node, V4SF_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestnzcps",ftype,IX86_BUILTIN_VTESTPS128NZC);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendvpd256",ftype,IX86_BUILTIN_VBLENDVPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_minps256",ftype,IX86_BUILTIN_VMINPS256);

ftype = build_function_type_list(integer_type_node, V2DF_type_node, V2DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestnzcpd",ftype,IX86_BUILTIN_VTESTPD128NZC);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_xorps256",ftype,IX86_BUILTIN_VXORPS256);

ftype = build_function_type_list(V8SI_type_node, V8SI_type_node, V8SI_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_shufps256",ftype,IX86_BUILTIN_VSHUFPS256);

ftype = build_function_type_list(V2DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_pd_pd256",ftype,IX86_BUILTIN_VCASTPD256TOPD128);

ftype = build_function_type_list(V4SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vextractf128_ps256",ftype,IX86_BUILTIN_VEXTRACTF128S);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V8SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilvarpd256",ftype,IX86_BUILTIN_VPERMILPD256VAR);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_hsubpd256",ftype,IX86_BUILTIN_VHSUBPD256);

ftype = build_function_type_list(void_type_node, void_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vzeroupper",ftype,IX86_BUILTIN_VZEROUPPER256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cmppd256",ftype,IX86_BUILTIN_VCMPPD256);

ftype = build_function_type_list(V4DF_type_node, pv2df_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vbroadcastf128_pd256",ftype,IX86_BUILTIN_VBROADCAST128D);

ftype = build_function_type_list(integer_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_movmskpd256",ftype,IX86_BUILTIN_VMOVMSKPD256);

ftype = build_function_type_list(V8SF_type_node, pv8sf_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskloadps256",ftype,IX86_BUILTIN_VMASKMOVPS256);

ftype = build_function_type_list(integer_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_movmskps256",ftype,IX86_BUILTIN_VMOVMSKPS256);

ftype = build_function_type_list(V4DF_type_node, V2DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_pd256_pd",ftype,IX86_BUILTIN_VCASTPD128TOPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_divps256",ftype,IX86_BUILTIN_VDIVPS256);

ftype = build_function_type_list(V8SF_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_ps256_ps",ftype,IX86_BUILTIN_VCASTPS128TOPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_minpd256",ftype,IX86_BUILTIN_VMINPD256);

ftype = build_function_type_list(V8SI_type_node, pv8si_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_lddqu256",ftype,IX86_BUILTIN_VLDDQU256);

ftype = build_function_type_list(V8SI_type_node, V8SI_type_node, V8SI_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vperm2f128_si256",ftype,IX86_BUILTIN_VPERM2F128SI);

ftype = build_function_type_list(integer_type_node, V4SF_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestcps",ftype,IX86_BUILTIN_VTESTPS128C);

ftype = build_function_type_list(V4SI_type_node, V8SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_si_si256",ftype,IX86_BUILTIN_VCASTSI256TOSI128);

ftype = build_function_type_list(V4SF_type_node, pfloat_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vbroadcastss",ftype,IX86_BUILTIN_VROADCASTSS128);

ftype = build_function_type_list(V4SF_type_node, V4SF_type_node, V4SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cmpps",ftype,IX86_BUILTIN_VCMPPS128);

ftype = build_function_type_list(void_type_node, pv8si_type_node, V8SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_movntdq256",ftype,IX86_BUILTIN_VMOVNTDQ256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_unpcklps256",ftype,IX86_BUILTIN_VUNPCKLPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_orps256",ftype,IX86_BUILTIN_VORPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendvps256",ftype,IX86_BUILTIN_VBLENDVPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilvarps256",ftype,IX86_BUILTIN_VPERMILPS256VAR);

ftype = build_function_type_list(V4SF_type_node, V4SF_type_node, V4SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cmpss",ftype,IX86_BUILTIN_VCMPSS128);

ftype = build_function_type_list(V4SI_type_node, V8SI_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vextractf128_si256",ftype,IX86_BUILTIN_VEXTRACTF128I);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_orpd256",ftype,IX86_BUILTIN_VORPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andnps256",ftype,IX86_BUILTIN_VANDNPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_divpd256",ftype,IX86_BUILTIN_VDIVPD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andnpd256",ftype,IX86_BUILTIN_VANDNPD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_xorpd256",ftype,IX86_BUILTIN_VXORPD256);

ftype = build_function_type_list(V8SF_type_node, V8SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvtdq2ps256",ftype,IX86_BUILTIN_VCVTDQ2PS256);

ftype = build_function_type_list(V4SF_type_node, V4SF_type_node, V4SI_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vpermilvarps",ftype,IX86_BUILTIN_VPERMILPS128VAR);

ftype = build_function_type_list(void_type_node, pv8sf_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_maskstoreps256",ftype,IX86_BUILTIN_VMASKMOVPS256ST);

ftype = build_function_type_list(integer_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestzpd256",ftype,IX86_BUILTIN_VTESTPD256);

ftype = build_function_type_list(integer_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestnzcps256",ftype,IX86_BUILTIN_VTESTPS256NZC);

ftype = build_function_type_list(V2DF_type_node, V2DF_type_node, V2DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cmppd",ftype,IX86_BUILTIN_VCMPPD128);

ftype = build_function_type_list(integer_type_node, V2DF_type_node, V2DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestzpd",ftype,IX86_BUILTIN_VTESTPD128);

ftype = build_function_type_list(V4DF_type_node, pdouble_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vbroadcastsd256",ftype,IX86_BUILTIN_VBROADCASTSD256);

ftype = build_function_type_list(V4DF_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_cvtps2pd256",ftype,IX86_BUILTIN_VCVTPS2PD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_movddup256",ftype,IX86_BUILTIN_VMOVDDUP256);

ftype = build_function_type_list(integer_type_node, V4SF_type_node, V4SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_vtestzps",ftype,IX86_BUILTIN_VTESTPS128Z);

