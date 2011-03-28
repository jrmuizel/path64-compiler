#!/usr/bin/perl
use Data::Dumper;

sub fprint{
        my($filename,$content)=@_;
        return if not ($content);
        open(FILE, ">".$filename) or die "can't open file: $filename\n";
        print FILE $content;
        close FILE;
}

sub copy_to{
	my($path,$filename,$path2)=@_;
	return if not ($filename);
	if($path2 eq ""){
	  system("mv -f $filename $path") ;
	}else{
	  system("cp -f $filename $path");
	  system("mv -f $filename $path2");
	}

}

sub cartesian {		#cartesian product, the perl way ]:->
    my @C=[];
    foreach(reverse @_){
        my @A=@$_;
        @C=map{my $n=$_;map{[$n,@$_]} @C} @A;
    }
    return @C;
}

sub is_intrinsic_op {
  my($intr_op)=@_;
  if($intr_op=~/Vovd/){
    return 0;
  }
  return 1;
}
sub builtin_type { ##this define the builtin type of each parameter
    my($arg)=@_;
    if($arg=~/v32f4/){
      if($arg=~/mem/){
        return "pv8sf_type_node";
      }
      return "V8SF_type_node";
    }
    elsif($arg=~/v32f8/){
	if($arg=~/mem/){
	  return "pv4df_type_node";
	}
      return "V4DF_type_node";
    }
    elsif($arg=~/v32i4/){
	if($arg=~/mem/){
	  return "pv8si_type_node";
	}
      return "V8SI_type_node";
    }
    elsif($arg=~/v32i8/){
	if($arg=~/mem/){
	  return "pv4di_type_node";
	}
      return "V4DI_type_node";
    }
    elsif($arg=~/v16f4/){
	if($arg=~/mem/){
	  return "pv4sf_type_node";
	}
      return "V4SF_type_node";
    }
    elsif($arg=~/v16f8/){
	if($arg=~/mem/){
	  return "pv2df_type_node";
	}
      return "V2DF_type_node";
    }
    elsif($arg=~/v16i4/){
	if($arg=~/mem/){
	  return "pv4si_type_node";
	}
      return "V4SI_type_node";
    }
    elsif($arg=~/v16i8/){
	if($arg=~/mem/){
	  return "pv2di_type_node";
	}
 	return "V2DI_type_node";
    }
    elsif($arg=~/simm8/){
    	return "integer_type_node";
    }
    elsif($arg=~/memdb/){
    	return "pdouble_type_node";
    }
    elsif($arg=~/memfl/){
    	return "pfloat_type_node";
    }
    elsif($arg=~/memchar/){
	return "pchar_type_node";
    }
		elsif($arg=~/vd/){
		  return "void_type_node";
		}
		elsif($arg=~/int32/){
		  return "integer_type_node";
		}
    else{
      die "don't support at the moment: ".$arg."\n";
    }
}


#pulling things out of envytools and converting them directly to isa_* interface results in fugly names, but at least it's fast as hell.

#my @mem=[["base64"],["offset32"],[["base64"],["offset64"],["uimm8"],["simm32"]],[["offset64"],["uimm8"],["simm32"]];
#my @mem =["base64","offset32"];
#TODO can't find _mm256_testz_pz and _mm_testz_pz in the include fine;

my @ov32f8_v32f8_v32f8=["VADDPD256 __builtin_ia32_addpd256","VADDSUBPD256 __builtin_ia32_addsubpd256","VHADDPD256 __builtin_ia32_haddpd256","VSUBPD256 __builtin_ia32_subpd256","VHSUBPD256 __builtin_ia32_hsubpd256","VMULPD256 __builtin_ia32_mulpd256","VDIVPD256 __builtin_ia32_divpd256","VANDNPD256 __builtin_ia32_andnpd256","VANDPD256 __builtin_ia32_andpd256","VORPD256 __builtin_ia32_orpd256","VXORPD256 __builtin_ia32_xorpd256","VMAXPD256 __builtin_ia32_maxpd256","VMINPD256 __builtin_ia32_minpd256","VUNPCKHPD256 __builtin_ia32_unpckhpd256","VUNPCKLPD256 __builtin_ia32_unpcklpd256"];
my @ov32f4_v32f4_v32f4=["VADDPS256 __builtin_ia32_addps256","VADDSUBPS256 __builtin_ia32_addsubps256","VHADDPS __builtin_ia32_haddps256","VSUBPS256 __builtin_ia32_subps256","VHSUBPS256 __builtin_ia32_hsubps256","VMULPS256 __builtin_ia32_mulps256","VDIVPS256 __builtin_ia32_divps256","VANDPS256 __builtin_ia32_andps256","VANDNPS256 __builtin_ia32_andnps256","VORPS256 __builtin_ia32_orps256","VXORPS256 __builtin_ia32_xorps256","VMAXPS256 __builtin_ia32_maxps256","VMINPS256 __builtin_ia32_minps256", "VUNPCKHPS256 __builtin_ia32_unpckhps256","VUNPCKLPS256           __builtin_ia32_unpcklps256"];
#my @ov16f8_v16f8_v16f8=["VTESTPD128 __builtin_ia32_vtestzpd"];
#my @ov16f4_v16f4_v16f4=["VTESTPS128C __builtin_ia32_vtestcps"];
my @oint32_v32f8_v32f8=["VTESTPD256NZC __builtin_ia32_vtestnzcpd256", "VTESTPD256 __builtin_ia32_vtestzpd256","VTESTPD256C __builtin_ia32_vtestcpd256"];
my @oint32_v16f8_v16f8=["VTESTPD128NZC __builtin_ia32_vtestnzcpd","VTESTPD128C __builtin_ia32_vtestcpd","VTESTPD128 __builtin_ia32_vtestzpd"];
my @oint32_v32f4_v32f4=["VTESTPS256NZC __builtin_ia32_vtestnzcps256","VTESTPS256Z __builtin_ia32_vtestzps256", "VTESTPS256C __builtin_ia32_vtestcps256"];
my @oint32_v16f4_v16f4=["VTESTPS128NZC __builtin_ia32_vtestnzcps","VTESTPS128Z __builtin_ia32_vtestzps","VTESTPS128C __builtin_ia32_vtestcps"];
my @ov32f8_v32f8_v32f8_v32f8=["VBLENDVPD256 __builtin_ia32_blendvpd256"];
my @ov32f4_v32f4_v32f4_v32f4=["VBLENDVPS256 __builtin_ia32_blendvps256"];
my @ov32f4_v32f4_v32f4_simm8=["VDPPS256 __builtin_ia32_dpps256","VBLENDPS256 __builtin_ia32_blendps256","VCMPPS256 __builtin_ia32_cmpps256"];
my @ov16f4_v16f4_v16f4_simm8=["VCMPPS128 __builtin_ia32_cmpps","VCMPSS128 __builtin_ia32_cmpss"];
my @ov32f8_v32f8_v32f8_simm8=["VBLENDPD256 __builtin_ia32_blendpd256","VCMPPD256 __builtin_ia32_cmppd256"];
my @ov16f8_v16f8_v16f8_simm8=["VCMPPD128 __builtin_ia32_cmppd","VCMPSD __builtin_ia32_cmpsd"];
my @ov32f8_v32f8=["VSQRTPD256 __builtin_ia32_sqrtpd256","VMOVDDUP256 __builtin_ia32_movddup256"];
my @ov32f4_v32f4=["VSQRTPS256 __builtin_ia32_sqrtps256","VRSQRTPS256 __builtin_ia32_rsqrtps256","VRCPPS256 __builtin_ia32_rcpps256","VMOVSLDUP256 __builtin_ia32_movsldup256"," VMOVSHDUP256  __builtin_ia32_movshdup256"];
my @ov32f8_v16i4=["VCVTDQ2PD256 __builtin_ia32_cvtdq2pd256"];
my @ov32f4_v32i4=["VCVTDQ2PS256 __builtin_ia32_cvtdq2ps256"];
my @ov16i4_v32f8=["VCVTPD2DQ256 __builtin_ia32_cvtpd2dq256","VCVTTPD2DQ256 __builtin_ia32_cvttpd2dq256"];
#my @ov32i4_v32f4=["VCVTPS2DQ256 __builtin_ia32_cvtps2dq256"];
my @ov32i8_v32f4=["VCVTPS2DQ256 __builtin_ia32_cvtps2dq256"];
my @ov16f4_v32f8=["VCVTPD2PS256 __builtin_ia32_cvtpd2ps256"];
my @ov32f8_v16f4=["VCVTPS2PD256 __builtin_ia32_cvtps2pd256"];
#my @ov16i4_v32f4=["VCVTTPS2DQ256 __builtin_ia32_cvttps2dq256"];
my @ov32i4_v32f4=["VCVTTPS2DQ256 __builtin_ia32_cvttps2dq256"];

my @ov32f8_v16f8mem=["VBROADCAST128D __builtin_ia32_vbroadcastf128_pd256"];
my @ov32f4_v16f4mem=["VBROADCAST128S __builtin_ia32_vbroadcastf128_ps256" ];
my @ov32f8_dbmem=["VBROADCASTSD256 __builtin_ia32_vbroadcastsd256","VMOVAPD","VMOVUPD256 __builtin_ia32_loadupd256"];
my @ov32f4_flmem=["VBROADCASTSS256 __builtin_ia32_vbroadcastss256","VMOVAPS","VMOVUPS256 __builtin_ia32_loadups256"];
my @ov16f4_flmem=["VROADCASTSS128 __builtin_ia32_vbroadcastss"];
#my @ov32i4_v32i4mem=["VMOVDQA256 __builtin_ia32_loaddqu256"];
my @ov32i4_charmem=["VMOVDQA256 __builtin_ia32_loaddqu256"];
my @ov32f8_v32f8mem_v32f8=["VMASKMOVPD256 __builtin_ia32_maskloadpd256"];
my @ov32f4_v32f4mem_v32f4=["VMASKMOVPS256 __builtin_ia32_maskloadps256"];
#my @ov16f8_dbmem_v16i4=["VMASKMOVPD128 __builtin_ia32_maskloadpd"];
my @ov16f8_v16f8mem_v16f8=["VMASKMOVPD128 __builtin_ia32_maskloadpd"];
#my @ov32f4_flmem_v32i4=["VMASKMOVPS256 __builtin_ia32_maskloadps256"];
#my @ov16f4_flmem_v16i4=["VMASKMOVPS128 __builtin_ia32_maskloadps"];
my @ov16f4_v16f4mem_v16f4=["VMASKMOVPS128 __builtin_ia32_maskloadps"];
#my @ov32f8_dbmem=["VMOVAPD"];
my @ovoid_dbmem_v32f8=["VMOVAPD256 __builtin_ia32_storeupd256","VMOVNTPD256 __builtin_ia32_movntpd256"];
my @ovoid_flmem_v32f4=["VMOVAPS256 __builtin_ia32_storeups256","VMOVNTPS256 __builtin_ia32_movntpd256"];
my @ovoid_v32i4mem_v32i4=["VMOVNTDQ256 __builtin_ia32_movntdq256"];
my @ovoid_charmem_v32i4=["VMOVDQA256ST __builtin_ia32_storedqu256"];
#my @ovoid_dbmem_v32f8=["VMOVNTPD256 __builtin_ia32_movntpd256"];
#my @ovoid_dbmem_v32i4_v32f8=["VMASKMOVPD256ST __builtin_ia32_maskstorepd256"];
my @ovoid_v32f8mem_v32f8_v32f8=["VMASKMOVPD256ST __builtin_ia32_maskstorepd256"];
#my @ovoid_dbmem_v32i4_v16f8=["VMASKMOVPD128ST __builtin_ia32_maskstorepd"];
my @ovoid_v16f8mem_v16f8_v16f8=["VMASKMOVPD128ST __builtin_ia32_maskstorepd"];
#my @ovoid_flmem_v32i4_v32f4=["VMASKMOVPS256ST __builtin_ia32_maskstoreps256"];
my @ovoid_v32f4mem_v32f4_v32f4=["VMASKMOVPS256ST __builtin_ia32_maskstoreps256"];
my @ovoid_v16f4mem_v16f4_v16f4=["VMASKMOVPS128ST __builtin_ia32_maskstoreps"];
my @ov16f8_v32f8_int32=["VEXTRACTF128D __builtin_ia32_vextractf128_pd256"];
my @ov16f4_v32f4_int32=["VEXTRACTF128S __builtin_ia32_vextractf128_ps256"];
my @ov16i4_v32i4_int32=["VEXTRACTF128I __builtin_ia32_vextractf128_si256"];
my @ov32f8_v32f8_v16f8_int32=["VINSERTF128F __builtin_ia32_vinsertf128_pd256"];
my @ov32f4_v32f4_v16f4_int32=["VINSERTF128D __builtin_ia32_vinsertf128_ps256"];
my @ov32i4_v32i4_v16i4_int32=["VINSERTF128I __builtin_ia32_vinsertf128_si256"];
my @ov32i4_v32i4mem=["VLDDQU256 __builtin_ia32_lddqu256"];
#my @ov32f8_v32f8=["VMOVDDUP256 __builtin_ia32_movddup256"];
#my @ov32f4_v32f4=
my @ov16f8_v32f8=["VCASTPD256TOPD128 __builtin_ia32_pd_pd256"];
my @ov16f4_v32f4=["VCASTPS256TOPS128 __builtin_ia32_ps_ps256"];
my @ov16i4_v32i4=["VCASTSI256TOSI128 __builtin_ia32_si_si256"];
my @ov32f8_v16f8=["VCASTPD128TOPD256 __builtin_ia32_pd256_pd"];
my @ov32f4_v16f4=["VCASTPS128TOPS256 __builtin_ia32_ps256_ps"];
my @ov32i4_v16i4=["VCASTSI128TOSI256 __builtin_ia32_si256_si"];

my @oint32_v32f8=["VMOVMSKPD256 __builtin_ia32_movmskpd256"];
my @oint32_v32f4=["VMOVMSKPS256 __builtin_ia32_movmskps256"];
my @ov32f8_v32f8_int32=["VROUNDPD256 __builtin_ia32_roundpd256","VPERMILPD256 __builtin_ia32_vpermilpd256"];
my @ov16f8_v16f8_int32=["VPERMILPD128 __builtin_ia32_vpermilpd"];
my @ov32f4_v32f4_int32=["VPERMILPS256 __builtin_ia32_vpermilps256","VROUNDPS256 __builtin_ia32_roundps256"];
my @ov16f4_v16f4_int32=["VPERMILPS128 __builtin_ia32_vpermilps"];
my @ov32f8_v32f8_v32i4=["VPERMILPD256VAR __builtin_ia32_vpermilvarpd256"];
my @ov16f8_v16f8_v16i4=["VPERMILPD128VAR __builtin_ia32_vpermilvarpd"];
my @ov32f4_v32f4_v32i4=["VPERMILPS256VAR __builtin_ia32_vpermilvarps256"];
my @ov16f4_v16f4_v16i4=["VPERMILPS128VAR __builtin_ia32_vpermilvarps"];
my @ov32i4_v32i4_v32i4_int32=["VPERM2F128SI __builtin_ia32_vperm2f128_si256"];
my @ov32f8_v32f8_v32f8_int32=["VSHUFPD25 __builtin_ia32_shufpd256","VPERM2F128D __builtin_ia32_vperm2f128_pd256"];
my @ov32f4_v32f4_v32f4_int32=["VSHUFPS256 __builtin_ia32_shufps256","VPERM2F128S __builtin_ia32_vperm2f128_ps256"];

#my @ov32f8_double_double_double_double=["V"];
my @ovoid_void=["VZEROALL256 __builtin_ia32_vzeroall","VZEROUPPER256 __builtin_ia32_vzeroupper"];
##Packed Test Operations
my @oint32_v32i4_v32i4=["VPTESTZ256 __builtin_ia32_ptestz256","VPTESTC256 __builtin_ia32_ptestc256","VPTESTNZC256 __builtin_ia32_ptestnzc256"];
##TODO _mm256_store_pd
#my @ov32i4_v16i4_int32_int32=[""];

my @ops=(
		#[@vaddp,["f128","f256"], ["OPS"],["ofloat"],["float"],@float_mem],
		[["NAME"],@ov16f8_v32f8_int32, ["OPS"],["V16F8ov16f8"],["v32f8"],["int32"]],
		[["NAME"],@ov16f4_v32f4_int32, ["OPS"],["V16F4ov16f4"],["v32f4"],["int32"]],
		[["NAME"],@ov16i4_v32i4_int32, ["OPS"],["V16I4ov16i4"],["v32i4"],["int32"]],
		[["NAME"],@ov32f8_v32f8_v16f8_int32, ["OPS"],["V32F8ov32f8"],["v32f8"],["v16f8"],["int32"]],
		[["NAME"],@ov32f4_v32f4_v16f4_int32, ["OPS"],["V32F4ov32f4"],["v32f4"],["v16f4"],["int32"]],
		[["NAME"],@ov32i4_v32i4_v16i4_int32, ["OPS"],["V32I4ov32i4"],["v32i4"],["v16i4"],["int32"]],
		[["NAME"],@ov32i4_v32i4_v32i4_int32, ["OPS"],["V32I4ov32i4"],["v32i4"],["v32i4"],["int32"]],
		[["NAME"],@ov32f8_v32f8_v32f8_int32, ["OPS"],["V32F8ov32f8"],["v32f8"],["v32f8"],["int32"]],
		[["NAME"],@ov32f4_v32f4_v32f4_int32, ["OPS"],["V32F4ov32f4"],["v32f4"],["v32f4"],["int32"]],
		[["NAME"],@ov32i4_v32i4mem,["OPS"],["V32I4ov32i4"],["memv32i4"]],
		[["NAME"],@oint32_v32f8,["OPS"],["I4oint32"],["v32f8"]],
		[["NAME"],@oint32_v32f4,["OPS"],["I4oint32"],["v32f4"]],
		[["NAME"],@ov32f8_v32f8_int32,["OPS"],["V32F8ov32f8"],["v32f8"],["int32"]],
		[["NAME"],@ov16f8_v16f8_int32,["OPS"],["V16F8ov16f8"],["v16f8"],["int32"]],
		[["NAME"],@ov32f4_v32f4_int32,["OPS"],["V32F4ov32f4"],["v32f4"],["int32"]],
		[["NAME"],@ov16f4_v16f4_int32,["OPS"],["V16F4ov16f4"],["v16f4"],["int32"]],
		[["NAME"],@ov32f8_v32f8_v32i4,["OPS"],["V32F8ov32f8"],["v32f8"],["v32i4"]],
		[["NAME"],@ov16f8_v16f8_v16i4,["OPS"],["V16F8ov16f8"],["v16f8"],["v16i4"]],
		[["NAME"],@ov32f4_v32f4_v32i4,["OPS"],["V32F4ov32f4"],["v32f4"],["v32i4"]],
		[["NAME"],@ov16f4_v16f4_v16i4,["OPS"],["V16F4ov16f4"],["v16f4"],["v16i4"]],
		[["NAME"],@ovoid_void,["OPS"],["Vovd"],["vd"]],
		[["NAME"],@ov32f8_v32f8_v32f8_v32f8,["OPS"],["V32F8ov32f8"],["v32f8"],["v32f8"],["v32f8"]],
		[["NAME"],@ov32f4_v32f4_v32f4_v32f4,["OPS"],["V32F4ov32f4"],["v32f4"],["v32f4"],["v32f4"]],
    #[["NAME"],@ov16f8_v16f8_v16f8,["OPS"],["V16F8ov16f8"],["v16f8"],["v16f8"]],
    [["NAME"],@ov16f4_v16f4_v16f4,["OPS"],["V16F4ov16f4"],["v16f4"],["v16f4"]],
    [["NAME"],@oint32_v32f8_v32f8,["OPS"],["I4oint32"],["v32f8"],["v32f8"]],
    [["NAME"],@oint32_v16f8_v16f8,["OPS"],["I4oint32"],["v16f8"],["v16f8"]],
    [["NAME"],@oint32_v32f4_v32f4,["OPS"],["I4oint32"],["v32f4"],["v32f4"]],
    [["NAME"],@oint32_v16f4_v16f4,["OPS"],["I4oint32"],["v16f4"],["v16f4"]],
		[["NAME"],@ov32f8_v32f8_v32f8, ["OPS"],["V32F8ov32f8"],["v32f8"],["v32f8"]],
		[["NAME"],@ov32f4_v32f4_v32f4, ["OPS"],["V32F4ov32f4"],["v32f4"],["v32f4"]],
		[["NAME"],@ov32f4_v32f4_v32f4_simm8,["OPS"],["V32F4ov32f4"],["v32f4"],["v32f4"],["simm8"]],
		[["NAME"],@ov32f8_v32f8_v32f8_simm8,["OPS"],["V32F8ov32f8"],["v32f8"],["v32f8"],["simm8"]],
		[["NAME"],@ov32f4_v32f4, ["OPS"],["V32F8ov32f4"],["V32F4ov32f4"]],
		[["NAME"],@ov32f8_v32f8, ["OPS"],["V32F8ov32f8"],["V32F8ov32f8"]],
		[["NAME"],@ov16f8_v16f8_v16f8_simm8,["OPS"],["V16F8ov16f8"],["v16f8"],["v16f8"],["simm8"]],
		[["NAME"],@ov16f4_v16f4_v16f4_simm8,["OPS"],["V16F4ov16f4"],["v16f4"],["v16f4"],["simm8"]],
		[["NAME"],@ov32f8_v16i4, ["OPS"],["V32F8ov32f8"],["v16i4"]],
		[["NAME"],@ov32f4_v32i4, ["OPS"],["V32F4ov32f4"],["v32i4"]],
		[["NAME"],@ov16i4_v32f8, ["OPS"],["V16I4ov16i4"],["v32f8"]],
		[["NAME"],@ov32i8_v32f4, ["OPS"],["V32I8ov32i8"],["v32f4"]],
		[["NAME"],@ov16f4_v32f8, ["OPS"],["V16F4ov16f4"],["v32f8"]],
		[["NAME"],@ov32f8_v16f4, ["OPS"],["V32F8ov32f8"],["v16f4"]],
		[["NAME"],@ov32i4_v32f4, ["OPS"],["V32I4ov32i4"],["v32f4"]],
		[["NAME"],@ov32f8_v16f8mem, ["OPS"],["V32F8ov32f8"],["memv16f8"]],
		[["NAME"],@ov32f4_v16f4mem, ["OPS"],["V32F4ov32f4"],["memv16f4"]],
		[["NAME"],@ov32f8_dbmem, ["OPS"],["V32F8ov32f8"],["memdb"]],
		[["NAME"],@ov32f4_flmem, ["OPS"],["V32F4ov32f4"],["memfl"]],
		[["NAME"],@ov16f4_flmem, ["OPS"],["V16F4ov16f4"],["memfl"]],
		[["NAME"],@ov32i4_charmem, ["OPS"],["V32I4ov32i4"],["memchar"]],
		[["NAME"],@ov32f8_v32f8mem_v32f8, ["OPS"],["V32F8ov32f8"],["memv32f8"],["v32f8"]],
		[["NAME"],@ov32f4_v32f4mem_v32f4, ["OPS"],["V32F4ov32f4"],["memv32f4"],["v32f4"]],
		[["NAME"],@ov16f8_dbv16f8_v16f8, ["OPS"],["V16F8ov16f8"],["memv16f8"],["v16f8"]],
		#[["NAME"],@ov32f4_flmem_v32i4, ["OPS"],["V32F4ov32f4"],["memfl"],["v32i4"]],
		#[["NAME"],@ov16f4_v16f4_v16f4, ["OPS"],["V16F4ov16f4"],["memv16f4"],["v16f4"]],
		#[@ov32f8_dbmem, ["OPS"],["ov32f8"],["memdb"]],
		[["NAME"],@ovd_dfmem_v32f8,["OPS"],["Vovd"],["memdf"],["v32f8"]],
		[["NAME"],@ovoid_v32i4mem_v32i4, ["OPS"],["Vovd"],["v32i4mem"],["v32i4"]],
		[["NAME"],@ovoid_charmem_v32i4, ["OPS"],["Vovd"],["memchar"],["v32i4"]],
		[["NAME"],@ovoid_v32f8mem_v32f8_v32f8, ["OPS"],["Vovd"],["memv32f8"],["v32f8"],["v32f8"]],
		[["NAME"],@ovoid_v16f8mem_v16f8_v16f8, ["OPS"],["Vovd"],["memv16f8"],["v16f8"],["v16f8"]],
		#[["NAME"],@ovoid_flmem_v32i4_v32f4, ["OPS"],["Vovd"],["memfl"],["v32i4"],["v32f4"]],
		[["NAME"],@ovoid_v32f4mem_v32f4_v32f4, ["OPS"],["Vovd"],["memv32f4"],["v32f4"],["v32f4"]],
		[["NAME"],@ovoid_v16f4mem_v16f4_v16f4, ["OPS"],["Vovd"],["memv16f4"],["v16f4"],["v16f4"]],
#		[["NAME"],@ovoid_dbmem_v32i4, ["OPS"],["Vovd"],["dbmem"],["v32i4"]],
		[["NAME"],@ov16f8_v32f8,["OPS"],["V16F8ov16f8"],["v32f8"]],
		[["NAME"],@ov16f4_v32f4,["OPS"],["V16F4ov16f4"],["v32f4"]],
		[["NAME"],@ov16i4_v32i4,["OPS"],["V16I4ov16i4"],["v32i4"]],
		[["NAME"],@ov32f8_v16f8,["OPS"],["V32F8ov32f8"],["v16f8"]],
		[["NAME"],@ov32f4_v16f4,["OPS"],["V32F4ov32f4"],["v16f4"]],
		[["NAME"],@ov32i4_v16i4,["OPS"],["V32I4ov32i4"],["v16i4"]],
		[["NAME"],@ov16f8_v16f8mem_v16f8,["OPS"],["V16F8ov16f8"],["memv16f8"],["v16f8"]],
		[["NAME"],@ov16f4_v16f4mem_v16f4,["OPS"],["V16F4ov16f4"],["memv16f4"],["v16f4"]],
	);
my @isa;
my @tops; 		#for isa pack and subset
my %isa_operands;
my %isa_print;
foreach $op (@ops){
	my @variants=cartesian(@$op);
	foreach(@variants){
		my $opstring;
		my @opname;
		my @operands;
		my $op_pattern;
		my $is_buildin=0;
		my $isop=0;
		foreach (@$_){
			if($_ eq "OPS"){
				$isop=1;
				$is_buildin=0;
			}
			elsif($_ eq "NAME"){
				$is_buildin=1;
				$isop=0;
			}
			elsif(not $_ eq ""){
				#$opstring.=$_."_";
				#push(@operands,$_) if $isop;
				if($isop){
				  $opstring.=$_."_";
				}
				push(@opname,$_) if $is_buildin;
				#$op_pattern.=$_."_" if $isop;
			}
		}
		#chop($op_pattern);
		chop($opstring);
		#my $topstring="TOP_".$opstring;
		if(scalar(@opname)>1){
		  die "OPNAME should be unique\n".@opname;
		}
		#push @isa,$opstring;
		#push @tops,$topstring;
		push @{$isa_operands{$opname[0]}},$opstring;
		#print "pushed "."opname ".$opname[0]." opstring". $opstring."\n";
		#push @{$isa_print{$op_pattern}},$topstring;
	}
}

my @debug_dup_isa;
my $di=0;

print "isa.cxx:\n";
#foreach(@isa){
#	$isa_isa_print.="\t\"".$_."\",\n";
#	$debug_dup_isa[$di++]=$_;
#}

#print "di = $di\n";

#my $dj;
#my $dk;
#for($dj=0;$dj<$di;$dj++){
	#print "dj=$dj di=$di\n";
#  for($dk=0;$dk<$di;$dk++){
#    if($dj!=$dk){
#      if($debug_dup_isa[$dk]=~/^$debug_dup_isa[$dj]$/i){
#	      print "shit met duplicate:\n".$debug_dup_isa[$dk]."   ".$debug_dup_isa[$dj]."   dk=$dk  dj=$dj\n";
#      }
#    }
#  }
  #print "shit ".$debug_dup_isa[$dj]."\n";
#}

#$isa_print="\nisa_operands.cxx:\n";
my $print_i386_c;
my $print_i386_h;
my $print_tree_c;
my $print_gspin_tree_h;
my $print_gspin_tree_cxx;
my $print_intrn_info_cxx;
my $print_wintrinsic_h;
my $print_wutil_cxx;
my $wgen_expr_cxx;
my $expand_cxx;
foreach (keys %isa_operands){
  #print "keys ".$_."\n";
  #foreach(@{$isa_operands{$_}}){
  #  print "\t\t".$_."\n";
    #}
  print "\t\t".${$isa_operands{$_}}[0]."\n";
  my $intrin_name;
  my $buildin_name;
  my @opnd;
  my @tmp;
  @tmp=split(" ",$_);
  $intrin_name=$tmp[0];
  $buildin_name=$tmp[1];
  if($buildin_name eq ""){
	  #die $intrin_name." doesn't get builtin"."\n";
	  next;
  }

  my @opnd=split("_", ${$isa_operands{$_}}[0]);

  my $i;
  my $ftype;
  for($i=0;$i<scalar(@opnd);$i++){
    $ftype.=builtin_type($opnd[$i]);
    $ftype.=", ";
  }
  $print_i386_c.="ftype = build_function_type_list(";
  $print_i386_c.=$ftype."NULL_TREE);\n";
  $print_i386_c.="def_builtin (MASK_AVX, \"".$buildin_name."\","."ftype,";
  $print_i386_c.="IX86_BUILTIN_".$intrin_name.");\n\n";

  $print_i386_h.="IX86_BUILTIN_".$intrin_name.",\n";

  $print_tree_c.="case IX86_BUILTIN_".$intrin_name.":";
  $print_tree_c.="return GSBI_IX86_BUILTIN_".$intrin_name.";\n";
  
  $print_gspin_tree_h.="GSBI_IX86_BUILTIN_".$intrin_name.",\n";

  my $intrn_ret;
  #$intrn_ret=return_type($opnd[0]);
  $intrn_ret=$opnd[0];
  my @pre_ret;
  @pre_ret=split("o",$intrn_ret);

  if($pre_ret[0] eq ""){
    die "$intrin_name has problem \n";
  }

  $print_intrn_info_cxx.="{\/\*".$intrin_name."\*\/\n";
  $print_intrn_info_cxx.="BYVAL, PURE, NO_SIDEEFFECTS, DOES_RETURN, NOT_ACTUAL, CGINTRINSIC,\n";
  $print_intrn_info_cxx.="IRETURN_".$pre_ret[0].",\"".$intrin_name."\"".",NULL, NULL},\n";
  ##TODO return type should modify accordingly.

  $expand_cxx.="case INTRN_".$intrin_name.":\n";
  $expand_cxx.="break;\n";

  $print_wintrinsic_h.="INTRN_".$intrin_name.",\n";
  $print_wutil_cxx.="INTRN_".$intrin_name.",\t"."\""."INTRN_".$intrin_name."\",\n";
  $print_wgen_expr_cxx.="case "."GSBI_IX86_BUILTIN_".$intrin_name.":"."\n";
  $print_wgen_expr_cxx.="\*iopc = "."INTRN_".$intrin_name.";"."\n";
  if(!(is_intrinsic_op($intrn_ret))){
    $print_wgen_expr_cxx.='*intrinsic_op = FALSE'.";\n";
    print $intrn_ret."met intrinsic_op FALSE".$intrin_name."\n";
  }
  $print_wgen_expr_cxx.="break;"."\n";
}

sub copy_all_file{
  if(0){
  copy_to('../GCC/gcc/config/i386/',"i386_avx.h");
  copy_to('../GCC/gcc/config/i386/',"i386_avx.c");
  copy_to('../GCC/gcc/',"tree_avx.c");
  copy_to('../GCC/libspin/',"gspin-tree_avx.h","../src/wgen/");
  copy_to('../src/common/com/',"intrn_info_avx.cxx");
  copy_to('../src/common/com/',"wintrinsic_avx.h");
  copy_to('../src/common/com',"wutil_avx.cxx");
  }
  #copy_to('../src/wgen/',"gspin-tree_avx.cxx");
  copy_to('../src/wgen/',"wgen_expr_avx.cxx");
}

print "i386.c:\n";
print $print_i386_c;
fprint("i386_avx.c",$print_i386_c);
print "\n\n";
print "i364.h:\n";
print $print_i386_h;
fprint("i386_avx.h",$print_i386_h);
print "\n\n";
print "tree.c\n";
print $print_tree_c;
fprint("tree_avx.c",$print_tree_c);
print "\n\n";
print "gspin_tree.h\n";
print $print_gspin_tree_h;
fprint("gspin-tree_avx.h",$print_gspin_tree_h);
print "\n\n";
print "intrn_info.cxx\n";
print $print_intrn_info_cxx;
fprint("intrn_info_avx.cxx",$print_intrn_info_cxx);
print "\n\n";
print "wintrinsic.h\n";
fprint("wintrinsic_avx.h",$print_wintrinsic_h);
print $print_wintrinsic_h;
print "\n\n";
print "wutil.cxx\n";
print $print_wutil_cxx;
fprint("wutil_avx.cxx",$print_wutil_cxx);
print "\n\n";
print "wgen_expr.cxx\n";
fprint("wgen_expr_avx.cxx",$print_wgen_expr_cxx);
print $print_wgen_expr_cxx;
print "\n\n";
print "expand.cxx\n";
fprint("expand_avx.cxx",$expand_cxx);
print $expand_cxx;
print "\n\n";

copy_all_file();
#copy_to('../src/be/cg/x8664/',"expand_avx.cxx");
