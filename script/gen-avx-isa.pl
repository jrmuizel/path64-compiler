#!/usr/bin/perl
use Data::Dumper;

sub fprint{
        my($filename,$content)=@_;
        return if not ($content);
        open(FILE, ">".$filename) or die "can't open file: $filename\n";
        print FILE $content;
        close FILE;
}

sub cartesian {		#cartesian product, the perl way ]:->
    my @C=[];
    foreach(reverse @_){
        my @A=@$_;
        @C=map{my $n=$_;map{[$n,@$_]} @C} @A;
    }
    return @C;
}
sub access_memory{
	my($top_instructions)=@_;
	if($top_instructions=~/base64_simm32/ || $top_instructions=~/base64_index64_uimm8_simm32/ || $top_instructions=~/index64_uimm8_simm32/){
		#print "hehe met memory".$top_instructions."\n";
	  return 1;
	}else{
	  return 0;
	}
}

sub type_operation{
	my($top_instr)=@_;
	if($top_instr=~/obase64_simm32/ || $top_instr=~/obase64_index64_uimm8_simm32/ || $top_instr=~/oindex64_uimm8_simm32/){
		print "match store".$top_instr."\n";
	    return "store_only";
		  #die "fuck that isa".$top_instr;
		  #return 0;
	}
	elsif($top_instr=~/base64_simm32/ || $top_instr=~/base64_index64_uimm8_simm32/ || $top_instr=~/index64_uimm8_simm32/){
	  print "load  ".$top_instr."\n";
	  if($top_instr=~/mov/){
	    return "load_only";
  	  }else{
	    return "load_exe";
	  }
  	}
	else{
	  print "else fuck".$top_instr."\n";
	  if($top_instr=~/mov/){
		  print "fuck move".$top_instr."\n";
	    return "move_prop";
  	  }elsif($top_instr=~/add/){
	    return "fadd_prop";
	  }elsif($top_instr=~/sub/){
	    return "fsub_prop";
	  }elsif($top_instr=~/mul/){
	    return "fmul_prop";
	  }else{
	    return "none_prop";
	  }
	}
}

sub copy_to{
	my($path,$filename)=@_;
	return if not ($filename);
	system("mv -f $filename $path");
}

sub how_many_opnds{
	my($memory_op)=@_;
	if($memory_op eq ""){
	  return 0;
	}
	my @mem;
	@mem=split("_",$memory_op);
	return scalar(@mem);

}

#pulling things out of envytools and converting them directly to isa_* interface results in fugly names, but at least it's fast as hell.

#my @mem=[["base64"],["offset32"],[["base64"],["offset64"],["uimm8"],["simm32"]],[["offset64"],["uimm8"],["simm32"]];
#my @mem =["base64","offset32"];
my @only_mem=["base64_simm32", "base64_index64_uimm8_simm32","index64_uimm8_simm32"];
my @float_mem=["float","base64_simm32","base64_index64_uimm8_simm32","index64_uimm8_simm32"];
my @int32_mem=["int32","base64_simm32","base64_index64_uimm8_simm32","index64_uimm8_simm32"];
my @int64_mem=["int64","base64_simm32","base64_index64_uimm8_simm32","index64_uimm8_simm32"];

my @oonly_mem=["obase64_simm32", "obase64_index64_uimm8_simm32","oindex64_uimm8_simm32"];
my @ofloat_mem=["ofloat","obase64_simm32", "obase64_index64_uimm8_simm32","oindex64_uimm8_simm32"];
my @oint32_mem=["oint32","obase64_simm32","obase64_index64_uimm8_simm32","oindex64_uimm8_simm32"];
my @oint64_mem=["oint64","obase64_simm32","obase64_index64_uimm8_simm32","oindex64_uimm8_simm32"];
##opnd(ymm/xmm), opnd(ymm/xmm/mem),result(ymm/xmm) 
my @vaddp=["vaddpd","vaddps","vaddsubpd","vaddsubps","vandpd","vandnpd","vandps","vandnps","vdivpd","vdivps","vhaddpd","vhaddps","vhsubpd","vhsubps","vmaxpd","vmaxps","vminpd","vminps","vmulpd","vmulps","vorpd","vorps","vsubpd","vsubps","vunpckhpd","vunpckhps","vunpcklpd","vunpcklps","vxorpd","vxorps"];
##opnd(ymm),opnd(ymm/mem), result(ymm)
my @ymm_ymmomem_to_ymm=["vpermilps"];
##opnd(xmm),opnd(xmm/mem), result(xmm)
my @vadds=["vaddsd","vaddss","vcvtsd2ss","vcvtss2sd","vdivsd","vdivss","vmaxsd","vmaxss","vminsd","vminss","vmovhpd","vmovhps","vmulsd","vmulss","vpacksswb","vpackssdw","vpackuswb","vpackusdw","vpaddb","vpaddw","vpaddd","vpaddq","vpaddsb","vpaddsw","vpaddusb","vpaddusw","vpand","vpandn","vpavgb","vpavgw","vpcmpeqb","vpcmpeqw","vpcmpeqd","vpcmpeqq","vpcmpgtb","vpcmpgtw","vpcmpgtd","vpcvpgtq","vpermilps","vphaddw","vphaddd","vphaddsw","vphsubw","vphsubd","vphsubsw","vpmaddwd","vpmaddubsw","vpmaxsb","vpmaxsw","vpmaxsd","vpmaxub","vpmaxuw","vpmaxud","vpminsb","vpminsw","vpminsd","vpminub","vpminuw","vpminud","vpmulhuw","vpmulhrsw","vpmulhw","vpmullw","vpmulld","vpmuludq","vpmuldq","vpor","vpsadbw","vpshufb","vpsignb","vpsignw","vpsignd","vpsllw","vpslld","vpsrad","vpsrlw","vpsrld","vpsubb","vpsubw","vpsubd","vpsubq","vpsubsb","vpsubsw","vpsubusb","vpsubusw","vpunpckhbw","vpunpckhwd","vpunpckhdq","vpunpckhqdq","vpunpcklbw","vpunpcklwd","vpunpckldq","vpunpcklqdq","vpxor","vrcpss","vrsqrtss","vsqrtsd","vsqrtss","vsubsd","vsubss"];
##opnd(xmm),opnd(mem),result(xmm)
my @vmovlps_xmm_mem_oxmm=["vmovlps"];
##opnd(ymm/xmm),opnd(ymm/xmm/mem),opnd(imm8), result(ymm/xmm)
my @vblendp=["vblendpd","vblendps","vcmppd","vcmpps","vdpps","vroundpd","vroundps","vshufpd","vshufps"];
#opnd(ymm) opnd(xmm/mem) opnd(imm8), result(ymm)
my @vinsertf128=["vinsertf128"];
#opnd(xmm) opnd(xmm/mem) opnd(imm8), result(xmm)
my @vinsertps=["vinsertps","vmpsadbw","vpalignr","vpblendw","vpclmulqdq","vroundsd","vroundss"];
#opnd(ymm) opnd(ymm/mem) opnd(imm8), result(ymm)
my @ymm_ymmomem_imm_to_ymm=["vperm2f128"];
#opnd(xmm) opnd(xmm/mem) opnd(xmm), result(xmm)
my @xmm_xmmomem_xmm_to_xmm=["vpblendvb"];
##opnd(xmm),opnd(xmm/mem),opnd(imm8),       result(xmm)
my @vcmpsd=["vcmpsd","vcmpss","vdppd"];
##opnd(ymm/xmm),opnd(ymm/xmm/),opnd(ymm/xmm/mem), result(ymm/xmm)
my @vblendvp=["vblendvpd","vblendvps"];
## opnd(xmm/mem),result(xmm)
#my @vbroadcast128=["vbroadcastss","vmovapd","vmovaps"];
## opnd(xmm/mem),opnd(imm8),result(xmm)
my @xmmomem_imm_to_xmm=["vpcmpestri","vpcmpestrm","vpcmpistri","vpcmpistrm","vpermilpd","vpermilps","vpshufd","vpshufhw","vpshuflw"];
## opnd(ymm/mem),opnd(imm8),result(ymm)
my @ymmomem_imm_to_ymm=["vpermilpd","vpermilps"];
## opnd(ymm/mem),result(ymm)
my @vbroadcast256=["vbroadcastss","vbroadcastsd", "vbroadcastf128","vmovapd","vtestps","vtestpd","vrcpps","vrsqrtps","vsqrtpd","vsqrtps","vcvtdq2ps256","vcvtps2dq","vcvttpd2dq","vcvttps2dq","vmovaps","vmovdqa","vmovdqu","vmovddup","vmovshdup","vmovsldup","vmovupd","vmovups"];
## opnd(xmm/mem),result(xmm)
my @vcomisd=["vcomisd","vcomiss","vcvtdq2pd","vcvtdq2ps","vcvtpd2ps","vcvtps2dq","vcvtps2pd","vcvttpd2dq","vcvttps2dq","vmovapd","vmovaps","vmovq","vmovdqa","vmovdqu","vmovddup","vmovshdup","vmovsldup","vmovupd","vmovups","vpabsb","vpabsw","vpabsd","vbroadcastss","vphminposuw","vpmovsxbw","vpmovsxbd","vpmovzxbw","vpmovzxbd","vtestps","vtestpd","vrcpps","vrsqrtps","vsqrtpd","vsqrtps","vucomisd","vucomiss"];
## opnd(xmm/mem),result(ymm)
my @vcvtdq2pd256=["vcvtdq2pd","vcvtpd2dq","vcvtps2pd"];
## opnd(ymm/mem),result(ymm)
#my @vcvtdq2ps256=["vcvtdq2ps256","vcvtps2dq","vcvttpd2dq","vcvttps2dq","vmovapd","vmovaps","vmovdqa","vmovdqu"];
## opnd(xmm), result(xmm/mem)
#my @movq_toxmmormem=["vmovq","vmovupd","vmovups"];
## opnd(ymm), result(ymm/mem)
#my @vmovdqa256=["vmovdqa","vmovdqu","vmovupd","vmovups"];
## opnd(int32/mem), result(xmm)
my @vmovd32_toxmm=["vmovd"];
## opnd(int64/mem), result(xmm)
#my @vmovd64_toxmm=["vmovq"];
## opnd(xmm), result(int32/mem)
#my @vmovd32_tomem=["vmovd"];
## opnd(int32), result(xmm)
my @int32_to_xmm=["vmovd"];
## opnd(int64), result(xmm)
my @int64_to_xmm=["vmovq"];
## opnd(xmm), result(int64/mem)
my @vmovd64_tomem=["vmovq"];
## opnd(xmm), result(int32/int64)
my @xmm_to_int = ["vpmovmskb"];
## opnd(xmm/mem),result(xmm)
#my @vmovddup128=["vmovddup","vmovshdup","vmovsldup","vmovupd","vmovups","vpabsb","vpabsw","vpabsd"];
## opnd(ymm/mem),result(ymm)
my @vmovddup256=["vmovddup","vmovshdup","vmovsldup","vmovupd","vmovups"];
## opnd(ymm/mem),result(xmm)
my @vcvtpd2dq128=["vcvtpd2dq","vcvtpd2ps"];
## opnd(xmm/mem), result(int32)
my @vcvtsd2si32=["vcvtsd2si","vcvtss2si","vcvttsd2si","vcvttss2si"];
## opnd(xmm/mem), result(int64)
my @vcvtsd2si64=["vcvtsd2si","vcvtss2si","vcvttsd2si","vcvttss2si"];
## opnd(xmm/ymm), result(int32)
my @vmovmskpd=["vmovmskpd","vmovmskps"];
## opnd(xmm) opnd(int32/mem), result(xmm)
my @vcvtsi2sd32=["vcvtsi2sd","vcvtsi2ss"];
## opnd(xmm) opnd(int64/mem), result(xmm)
my @vcvtsi2sd64=["vcvtsi2sd","vcvtsi2ss"];
## opnd(xmm) opnd(int32/mem) opnd(imm8) result(xmm)
my @xmm_int32omem_imm8_to_xmm=["vpinsrb","vpinsrw","vpinsrd"];
## opnd(xmm) opnd(int64/mem) opnd(imm8) result(xmm)
my @xmm_int64omem_imm8_to_xmm=["vpinsrq"];
## opnd(ymm) opnd(imm8), result(xmm/mem)
my @vextractf128=["vextractf128"];
## opnd(xmm) opnd(imm8), result(int32/mem)
my @vextractf128=["vextractps"];
## opnd(xmm/ymm) opnd(mem), result(xmm/ymm)
my @vmaskmovps=["vmaskmovps","vmaskmovpd"];
## opnd(xmm/ymm) opnd(xmm/ymm), result(mem)
my @vmaskmovps_tomem=["vmaskmovps","vmaskmovpd"];
## opnd(xmm) opnd(xmm), result(mem)
my @vmovlpd128=["vmovlpd"];

## opnd(mem), result(ymm/xmm)
my @vlddqu=["vlddqu"];
## opnd(mem), result(xmm)
my @vmovhpd=["vmovhpd","vmovhps","vmovntdqa","vmovsd","vmovss"];
## opnd(xmm), result(xmm)
my @vmaskmovdqu=["vmaskmovdqu"];
## opnd(xmm), result(mem)
my @vmovlpd_xmm_to_mem=["vmaskmovdqu","vmovlps","vmovntpd","vmovntps","vmovsd","vmovss","vmovdqa","vmovdqu","vmovq","vmovupd","vmovups","vmovaps","vmovapd"];
## opnd(ymm), result(mem)
my @ymm_to_mem=["vmovntdq","vmovntpd","vmovntps","vmovaps","vmovapd","vmovdqa","vmovdqu","vmovupd","vmovups"];

## opnd(xmm) opnd(xmm), result(xmm)
my @vmovhlps=["vmovhlps","vmovlhps","vmovsd","vmovss"];

## opnd(xmm) opnd(imm), result(int32)
my @xmm_imm_to_int32=["vpextrb","vpextrw"];
## opnd(xmm) opnd(imm), result(mem)
my @xmm_imm_to_mem=["vpextrb"];
## opnd(xmm) opnd(imm), result(xmm)
my @xmm_imm_to_xmm=["vpslldq","vpsrldq","vpsllw","vpsrad","vpsrlw"];

## opnd(mxcsr), result(mem)
my @mxcsr_to_mem=["vstmxcsr"];
## opnd(mem), opnd(mxcsr)
my @mem_to_mxcsr=["vldmxcsr"];
##Fixme##
##@vldmxcsr vstmxcsr
##next is movdqu

my @ops=(
		[@vaddp,["f128","f256"], ["OPS"],["ofloat"],["float"],@float_mem],
		[@vadds,["f128"], ["OPS"],["ofloat"],["float"],@float_mem],
		[@vblendp,["f128","f256"], ["OPS"],["ofloat"],["float"],@float_mem,["simm8"]],
		[@vblendvp,["f128","f256"], ["OPS"],["ofloat"],["float"],@float_mem,["float"]],
		#[@vbroadcast128,["f128"],["OPS"],["ofloat"],@only_mem],
		[@vbroadcast256,["f256"],["OPS"],["ofloat"],@float_mem],
		[@vcmpsd,["f128"], ["OPS"],["ofloat"],["float"],@float_mem,["simm8"]],
		[@vcomisd,["f128"], ["OPS"], ["ofloat"],@float_mem],
		[@vcvtdq2pd256,["f256"], ["OPS"],["ofloat"],@float_mem],
		#[@vcvtdq2ps256,["f256"], ["OPS"],["ofloat"],@float_mem],
		[@vcvtpd2dq128,["f128"],["f256"], ["OPS"], ["ofloat"],@float_mem],
		[@vcvtsd2si32, ["int32"],["OPS"], ["oint32"],@float_mem],
		[@vcvtsd2si64, ["int64"],["OPS"], ["oint64"],@float_mem],
		[@vcvtsi2sd32, ["int32"],["OPS"],["ofloat"],["float"],@int32_mem],
		[@vcvtsi2sd64, ["int64"],["OPS"],["ofloat"],["float"],@int64_mem],
		[@vextractf128, ["f256"],["OPS"],@ofloat_mem, ["float"],["simm8"]],
		[@vextractps, ["f128"], ["OPS"], @oint32_mem, ["float"],["simm8"]],
		[@vinsertf128,["f256"], ["OPS"], ["ofloat"],["float"], @float_mem,["simm8"]],
		[@vinsertps,["f128"], ["OPS"], ["ofloat"],["float"], @float_mem,["simm8"]],
		[@vlddqu,["f128","f256"],["OPS"],["ofloat"], @only_mem],
		[@vmaskmovdqu,["f128"], ["OPS"], ["ofloat"], ["float"]],
		[@vmaskmovps,["f128","f256"], ["OPS"], ["ofloat"],@only_mem, ["float"]],
		[@vmaskmovps_tomem,["f128","f256"], ["OPS"],@oonly_mem,["float"],["float"]],	
		#[@vmovd32_toxmm,["f128"],["OPS"],@oint32_mem,["float"]],
		#[@vmovd64_toxmm,["f128"],["OPS"],@oint64_mem,["float"]],
		[@int32_to_xmm,["f128"],["OPS"],["oint32"],["float"]],
		[@int64_to_xmm,["f128"],["OPS"],["oint64"],["float"]],
		[@vmovd32_tomem,["i32"],["OPS"], ["ofloat"],@int32_mem],
		[@vmovd64_tomem,["i64"],["OPS"], ["ofloat"],@int64_mem],
		#[@movq_toxmmormem,["f128"],["OPS"],@ofloat_mem,["float"]],
		#[@vmovddup128, ["f128","f256"],["OPS"], ["ofloat"], @float_mem],
		#[@vmovdqa256, ["f256"],["OPS"],@ofloat_mem, ["float"]],
		[@vmovhlps, ["f128"],["OPS"],["ofloat"],["float"],["float"]],
		[@vmovhpd, ["f128"],["OPS"],["float"],@only_mem],
		[@vmovlpd128,["f128"],["OPS"],["ofloat"], ["float"],@only_mem],
		[@vmovlpd_xmm_to_mem,["f128"],["OPS"],@oonly_mem,["float"]],
		[@vmovlps_xmm_mem_oxmm,["f128"],["OPS"],["ofloat"],@only_mem,["float"]],
		[@vmovmskpd,["f128","f256"],["OPS"],["oint32"],["float"]],
		[@ymm_to_mem,["f256"],["OPS"],@oonly_mem,["float"]],
		[@xmm_xmmomem_xmm_to_xmm, ["f128"],["OPS"],["ofloat"],["float"],@float_mem,["float"]],
		[@xmmomem_imm_to_xmm,["f128"],["OPS"],["ofloat"],@float_mem,["simm8"]],
		[@ymmomem_imm_to_ymm,["f256"],["OPS"],["ofloat"],@float_mem,["simm8"]],
		[@ymm_ymmomem_to_ymm,["f256"],["OPS"],["ofloat"],["float"],@float_mem],
		[@ymm_ymmomem_imm_to_ymm, ["f256"],["OPS"],["ofloat"],["float"],@float_mem, ["simm8"]],
		[@xmm_imm_to_int32, ["int32"],["OPS"],["oint32"],["float"],["simm8"]],
		[@xmm_imm_to_mem, ["mem"], ["OPS"], @oonly_mem,["float"],["simm8"]],
		[@xmm_imm_to_xmm, ["f128"], ["OPS"], ["float"],["float"],["simm8"]],
		[@xmm_int32omem_imm8_to_xmm, ["f128"],["OPS"],["ofloat"],["float"],@int32_mem,["simm8"]],
		[@xmm_int64omem_imm8_to_xmm, ["f128"],["OPS"],["ofloat"],["float"],@int64_mem,["simm8"]],
		[@xmm_to_int, ["int32"],["OPS"],["oint32","oint64"],["float"]],
		[["vzeroall"],["OPS"],["null"]],
		[["vzeroupper"],["OPS"],["null"]],
		[@mxcsr_to_mem,["OPS"],@oonly_mem,["mxcsr"]],
		[@mem_to_mxcsr,["OPS"],["mxcsr"],@oonly_mem],

	);
my @isa;
my @tops; 		#for isa pack and subset
my %isa_operands;
my %isa_print;
foreach $op (@ops){
	my @variants=cartesian(@$op);
	foreach(@variants){
		my $opstring;
		my @operands;
		my $op_pattern;
		my $isop=0;
		foreach (@$_){
			if($_ eq "OPS"){
				$isop=1;
			}
			elsif(not $_ eq ""){
				$opstring.=$_."_";
				push(@operands,$_) if $isop;
				$op_pattern.=$_."_" if $isop;
			}
		}
		chop($op_pattern);
		chop($opstring);
		my $topstring="TOP_".$opstring;
		push @isa,$opstring;
		push @tops,$topstring;
		push @{$isa_operands{$op_pattern}},[$topstring,\@operands];
		push @{$isa_print{$op_pattern}},$topstring;
	}
}
my $isa_isa_print;
my $isa_operands_print;
my $isa_print_print;
my $isa_subset_print;
my $isa_pack_print;
my $isa_properties_print;
my $isa_si_print;
my $isa_cgemit_avx_print;

my @debug_dup_isa;
my $di=0;

print "isa.cxx:\n";
foreach(@isa){
	$isa_isa_print.="\t\"".$_."\",\n";
	$debug_dup_isa[$di++]=$_;
}

print "di = $di\n";

my $dj;
my $dk;
for($dj=0;$dj<$di;$dj++){
	#print "dj=$dj di=$di\n";
  for($dk=0;$dk<$di;$dk++){
    if($dj!=$dk){
      if($debug_dup_isa[$dk]=~/^$debug_dup_isa[$dj]$/i){
	      print "shit met duplicate:\n".$debug_dup_isa[$dk]."   ".$debug_dup_isa[$dj]."   dk=$dk  dj=$dj\n";
      }
    }
  }
  #print "shit ".$debug_dup_isa[$dj]."\n";
}

#$isa_print="\nisa_operands.cxx:\n";
foreach (keys %isa_operands){
	$isa_operands_print.="\tInstruction_Group(\"".$_."\",\n";
	foreach (@{$isa_operands{$_}}){
		$isa_operands_print.="\t\t".$_->[0].",\n";
	}
	$isa_operands_print.="\t\tTOP_UNDEFINED);\n";
	my $res=0;
	my $pos=0;
	my $opnd=0;
	my $opnd_post=1;
	if($_=~/^obase[0-9]+_/ || $_=~/^oindex[0-9]+_/){
		#die "match store:".$_."\n";
		@tmp_load=split("_",$_);
		$str_store = @{$isa_operands{$_}[0]->[1]};
		#die "debug matched store:".$str_store."\n";
		#my $tmp_last=$tmp_load[scalar(@tmp_load)-1];
		foreach (@{$isa_operands{$_}[0]->[1]}){
		  if($_=~/^obase[0-9]+_/ || $_=~/^oindex[0-9]+_/){
		    next;
		  }
		  if($_=~/^float/){$isa_operands_print.="\tOperand(".$opnd++.", "."fp128".",opnd".$opnd_post++.");\n";}
		  elsif($_=~/^int64/){$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",opnd".$opnd_post++.");\n";}
		  elsif($_=~/^simm8/){$isa_operands_print.="\tOperand(".$opnd++.", "."simm8".",opnd".$opnd_post++.");\n";}
		  elsif($_=~/^mxcsr/){$isa_operands_print.="\tOperand(".$opnd++.", "."mxcsr".",opnd".$opnd_post++.");\n";}
		  else{die "doesn't know wtf when store:".$_."\n";}
		}

		if($_=~/^obase64_simm32_/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",base".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			#$opnd_post++;
		}elsif($_=~/^obase64_index64_uimm8_simm32_/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",base".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",index".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."uimm8".",scale".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			#$opnd_post++;
		}elsif($_=~/^oindex64_uimm8_simm32_/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",index".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."uimm8".",scale".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			#$opnd_post++;
		}else{die "doesn't know wtf base:".$_."\n";}
	}else {
	foreach (@{$isa_operands{$_}[0]->[1]}){
		#$isa_operands_print.='@{$isa_operands{$_}[0]->[1]:';
		#$isa_operands_print.=$_."\n";
		#if($_=~s/^[rbfus](\d+)$/nvc0_r\1/g){$isa_operands_print.="\tOperand(".$pos++.",".$_.",opnd".++$opnd.");\n";}
		#if($_=~s/^(.imm\d+)$/nvc0_\1/g){$isa_operands_print.="\tOperand(".$pos++.",".$_.",opnd".++$opnd.");\n";}
		#if($_=~s/^o(.+)$/nvc0_\1/g){$isa_operands_print.="\tResult(".$res++.",".$_.");\n";}
		#$isa_operands_print.="aaa".$_."bbb\n";
		if($_=~/^ofloat/){$isa_operands_print.="\tResult(".$res++.","."fp128".");\n";}
		if($_=~/^oint64/){$isa_operands_print.="\tResult(".$res++.","."int64".");\n";}
		if($_=~/^oint32/){$isa_operands_print.="\tResult(".$res++.","."int32".");\n";}
		if($_=~/^float/){$isa_operands_print.="\tOperand(".$opnd++.", "."fp128".",opnd".$opnd_post++.");\n";}
		if($_=~/^int64/){$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",opnd".$opnd_post++.");\n";}
		if($_=~/^mxcsr/){$isa_operands_print.="\tOperand(".$opnd++.", "."mxcsr".",opnd".$opnd_post++.");\n";}
		if($_=~/^obase64_simm32$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",base".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			#$opnd_post++;
		}
		if($_=~/^base64_simm32$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",base".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			$opnd_post++;
		}
		if($_=~/^obase64_index64_uimm8_simm32$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",base".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",index".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."uimm8".",scale".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			#$opnd_post++;
		}
		if($_=~/^base64_index64_uimm8_simm32$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",base".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",index".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."uimm8".",scale".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			$opnd_post++;
		}
		if($_=~/^oindex64_uimm8_simm32$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",index".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."uimm8".",scale".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			#$opnd_post++;
		}
		if($_=~/^index64_uimm8_simm32$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."int64".",index".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."uimm8".",scale".");\n";
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm32".",offset".");\n";
			$opnd_post++;
		}
		if($_=~/^simm8$/){
			$isa_operands_print.="\tOperand(".$opnd++.", "."simm8".",opnd".$opnd_post++.");\n";
		}
		
	}
   }
	$isa_operands_print.="\n";
}
print "isa_print.cxx:\n";
foreach (keys %isa_operands){
	my $result_n=0;
	my $opnd_n=0;
	my @res;
	my @opnd;
	$isa_print_print.="ISA_PRINT_TYPE ".$_." = ISA_Print_Type_Create("."\"".$_."\",\n";
	foreach (@{$isa_operands{$_}[0]->[1]}){
		if($_=~/^o/){
		 $res[$result_n++]=$_;
		}else{
		 $opnd[$opnd_n++]=$_;
		}
	}

##the %s 
	$isa_print_print.='"%s ';
	my $i;
	for($i=0;$i<$opnd_n;$i++){
	  if($opnd[$i]=~/^base64_simm32$/){
		  $isa_print_print.='%s%s(%s) ';
		}elsif($opnd[$i]=~/^base64_index64_uimm8_simm32$/){
		  $isa_print_print.='%s%s(%s,%s,%s) ';
		}elsif($opnd[$i]=~/^index64_uimm8_simm32$/){
			$isa_print_print.='%s%s(,%s,%s) ';
		}elsif($opnd[$i]=~/^simm8$/ ){
		  $isa_print_print.='%s ';
		}elsif($opnd[$i]=~/^float/){
		  $isa_print_print.='%s ';
		}elsif($opnd[$i]=~/^int/){
		  $isa_print_print.='%s ';
		}elsif($opnd[$i]=~/^null/||$opnd[$i]=~/^mxcsr/){
		 ##do nothing here;
		}
		else{
		  $isa_print_print.="TODO NYI format opnd ".$opnd[$i]."\n";
		  die "TODO NYI format opnd ".$opnd[$i]."\n";
		}
		if($result_n>0){
			$isa_print_print.=',';
		}
	}

	if($result_n>0){
	  if($res[0]=~/^obase64_simm32$/){
		  $isa_print_print.='%s%s(%s) ';
		}elsif($res[0]=~/^obase64_index64_uimm8_simm32$/){
		  $isa_print_print.='%s%s(%s,%s,%s) ';
		}elsif($res[0]=~/^oindex64_uimm8_simm32$/){
			$isa_print_print.='%s%s(,%s,%s) ';
		}elsif($res[0]=~/^osimm8$/){
		  $isa_print_print.='%s';
		}elsif($res[0]=~/^ofloat/){
			$isa_print_print.='%s';
		}elsif($res[0]=~/^oint/){
			$isa_print_print.='%s'; 
		}
		else{
		  $isa_print_print.="TODO NYI format result".$res[0]."\n";
		}
	}
	$isa_print_print.="\");\n";

###end %s

###Operand()
###Result()
  my $total_opnd=0;
	$isa_print_print.="Name();\n"; 
	#for($i=0;$i<$opnd_n;$i++){
	for($i=$opnd_n-1;$i>=0;$i--){
	  if($opnd[$i]=~/^base64_simm32$/){
			my $first;
			my $second;
			$first=$i+1;
			$second=$i;
			$isa_print_print.="Segment();\n";
			$isa_print_print.="Operand(".$first.");\n";
			$isa_print_print.="Operand(".$second.");\n";
		  #$isa_print_print.='%s%s(%s) ';
			$total_opnd+=2;
		}elsif($opnd[$i]=~/^base64_index64_uimm8_simm32$/){
			my $first;
			my $second;
			my $third;
			my $fourth;
			$first=$i+3;
			$second=$i;
			$third=$i+1;
			$fourth=$i+2;
			$isa_print_print.="Segment();\n";
			$isa_print_print.="Operand(".$first.");\n";
			$isa_print_print.="Operand(".$second.");\n";
			$isa_print_print.="Operand(".$third.");\n";
			$isa_print_print.="Operand(".$fourth.");\n";

		  #$isa_print_print.='%s%s(%s,%s,%s) ';
			$total_opnd+=4;
		}elsif($opnd[$i]=~/^index64_uimm8_simm32$/){
			my $first;
			my $second;
			my $third;
			$first=$i+2;
			$second=$i;
			$third=$i+1;
			$isa_print_print.="Segment();\n";
			$isa_print_print.="Operand(".$first.");\n";
			$isa_print_print.="Operand(".$second.");\n";
			$isa_print_print.="Operand(".$third.");\n";
			#$isa_print_print.='%s%s(,%s,%s) ';
			$total_opnd+=3;
		}elsif($opnd[$i]=~/^simm8$/ || $opnd[$i]=~/^float/ || $opnd[$i]=~/^int/){
		  #$isa_print_print.='%s ';
		  	my $j,$number_opnd_front,$tmp_n;
			$number_opnd_front=0;
			for($j=0;$j<$i;$j++){
			  #determine how many opnds in front;
			  $number_opnd_front+=how_many_opnds($opnd[$j]);
			}
			$isa_print_print.="Operand(".$number_opnd_front.");\n";
		}elsif($opnd[$i]=~/^null/||$opnd[$i]=~/^mxcsr/){
		   #do nothing here;
		}
		else{
		  $isa_print_print.="TODO NYI $isa_print_print.=opnd ".$opnd[$i]."\n";
		}
	}
	
	if($result_n>0){
	  if($res[0]=~/^obase64_simm32$/){
			my $first;
			my $second;
			$first=$opnd_n+1;
			$second=$opnd_n;
			$isa_print_print.="Segment();\n";
			$isa_print_print.="Operand(".$first.");\n";
			$isa_print_print.="Operand(".$second.");\n";
		  #$isa_print_print.='%s%s(%s) ';
		}elsif($res[0]=~/^obase64_index64_uimm8_simm32$/){
			my $first;
			my $second;
			my $third;
			my $fourth;
			$first=$opnd_n+3;
			$second=$opnd_n;
			$third=$opnd_n+1;
			$fourth=$opnd_n+2;
			$isa_print_print.="Segment();\n";
			$isa_print_print.="Operand(".$first.");\n";
			$isa_print_print.="Operand(".$second.");\n";
			$isa_print_print.="Operand(".$third.");\n";
			$isa_print_print.="Operand(".$fourth.");\n";

		  #$isa_print_print.='%s%s(%s,%s,%s) ';
			$total_opnd+=4;
		}elsif($res[0]=~/^oindex64_uimm8_simm32$/){
			my $first;
			my $second;
			my $third;
			$first=$opnd_n+2;
			$second=$opnd_n;
			$third=$opnd_n+1;
			$isa_print_print.="Segment();\n";
			$isa_print_print.="Operand(".$first.");\n";
			$isa_print_print.="Operand(".$second.");\n";
			$isa_print_print.="Operand(".$third.");\n";
			#$isa_print_print.='%s%s(,%s,%s) ';
		}elsif($res[0]=~/^osimm8$/){
			$isa_print_print.="Result(0);\n";
		  #$isa_print_print.='%s';
		}elsif($res[0]=~/^ofloat/){
			$isa_print_print.="Result(0);\n";
			#$isa_print_print.='%s';
		}elsif($res[0]=~/^oint/){
			$isa_print_print.="Result(0);\n";
			#$isa_print_print.='%s'; 
		}
		else{
		  $isa_print_print.="TODO NYI isa_print_print=result".$res[0]."\n";
		}
	}

##end operand()
##end Result

	#print "TODO defined format here\n";
	$isa_print_print.="\tInstruction_Print_Group(".$_.",\n";
	foreach (@{$isa_operands{$_}}){
		$isa_print_print.="\t\t".$_->[0].",\n";
	}
	$isa_print_print.="\t\tTOP_UNDEFINED);\n\n\n";
}

print "\nisa_subset.cxx\n";
foreach (@tops){
	$isa_subset_print.="\t".$_.",\n";
}

my $print_load_only;
my $print_load_exe;
my $print_store_only;
my $print_move_prop;
my $print_fadd_prop;
my $print_fsub_prop;
my $print_fmul_prop;
my $print_flop_prop;
print "\navx_properties.cxx:\n";
foreach (@tops){
	$isa_properties_print.="\t".$_.",\n";
	if(type_operation($_) eq "load_only"){
	  $print_load_only.=$_.",\n";
	}
	if(type_operation($_) eq "load_exe"){
	  $print_load_exe.=$_.",\n";
	}
	if(type_operation($_) eq "store_only"){
	  $print_store_only.=$_.",\n";
	}
	if(type_operation($_) eq "move_prop"){
	  $print_move_prop.=$_.",\n";
	}
	if(type_operation($_) eq "fadd_prop"){
	  $print_fadd_prop.=$_.",\n";
	}
	if(type_operation($_) eq "fsub_prop"){
	  $print_fsub_prop.=$_.",\n";
	}
	if(type_operation($_) eq "fmul_prop"){
	  $print_fmul_prop.=$_.",\n";
	}
	if($_=~/float/){
	  $print_flop_prop.="\t".$_.",\n";
	}
}

print "\navx_pack.cxx:\n";
foreach (@tops){
	$isa_pack_print.="\t".$_.", 0x000000ff,\n";
}


print "\navx_si.cxx:\n";
	$isa_si_print.="//TODO should apart these instructions accordingly!\n\n\n";
	$isa_si_print.="\tInstruction_Group(\""."avx with memory"."\",\n";
foreach (@tops){
	#foreach (@{$isa_operands{$_}}){
	if(access_memory($_)){
		$isa_si_print.="\t\t".$_.",\n";
	}
		#}
}
	$isa_si_print.="\t\tTOP_UNDEFINED);\n";
	$isa_si_print.="Any_Operand_Access_Time(0);\n";
	$isa_si_print.="Any_Result_Available_Time(4);\n";
	$isa_si_print.="Resource_Requirement(res_issue, 0);\n";
	$isa_si_print.="Resource_Requirement(res_fstore, 0);\n";
	
$isa_si_print.="\tInstruction_Group(\""."avx arth"."\",\n";
foreach(@tops){
	if(!access_memory($_)){
		$isa_si_print.="\t\t".$_.",\n";
	}
}
	$isa_si_print.="\t\tTOP_UNDEFINED);\n";
	$isa_si_print.="Any_Operand_Access_Time(0);\n";
	$isa_si_print.="Any_Result_Available_Time(2);\n";
	$isa_si_print.="Resource_Requirement(res_fadd, 0);\n";
	$isa_si_print.="Resource_Requirement(res_issue, 0);\n";

foreach (@tops){
  my @tmp=split("_",$_);
  $isa_cgemit_avx_print.="OP_Name[".$_."] = \""."$tmp[1]"."\";\n";
}

fprint("isa_avx.cxx", $isa_isa_print);
fprint("isa_avx_print.cxx",$isa_print_print);
fprint("isa_avx_operands.cxx",$isa_operands_print);
fprint("isa_avx_pack.cxx",$isa_pack_print);
fprint("isa_avx_subset.cxx",$isa_subset_print);
fprint("avx_si2.cxx", $isa_si_print);
fprint("isa_avx_properties.cxx",$isa_properties_print);
fprint("cgemit_targ_avx.cxx", $isa_cgemit_avx_print);

copy_to('../src/common/targ_info/isa/x8664/',"isa_avx.cxx");
copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_print.cxx");
copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_operands.cxx");
copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_pack.cxx");
copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_subset.cxx");
copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties.cxx");
copy_to('../src/common/targ_info/proc/x8664/',"avx_si2.cxx");
copy_to('../src/be/cg/x8664/cgemit_targ_avx.cxx',"cgemit_targ_avx.cxx");
  
fprint("isa_avx_properties_load_only.cxx", $print_load_only);
  fprint("isa_avx_properties_load_exe.cxx", $print_load_exe);
  fprint("isa_avx_properties_store_only.cxx", $print_store_only);
  fprint("isa_avx_properties_move_prop.cxx", $print_move_prop);
  fprint("isa_avx_properties_fadd_prop.cxx", $print_fadd_prop);
  fprint("isa_avx_properties_fsub_prop.cxx", $print_fsub_prop);
  fprint("isa_avx_properties_fmul_prop.cxx", $print_fmul_prop);
  fprint("isa_avx_properties_flop_prop.cxx", $print_flop_prop);
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_load_only.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_load_exe.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_store_only.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_move_prop.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_fadd_prop.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_fsub_prop.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_fmul_prop.cxx");
  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_flop_prop.cxx");

#sub copy_to_property{
	#fprint("isa_avx_properties_load_only.cxx", $print_load_only);
	# fprint("isa_avx_properties_load_exe.cxx", $print_load_exe);
#  fprint("isa_avx_properties_store_only.cxx", $print_store_only);
#  fprint("isa_avx_properties_move_prop.cxx", $print_move_prop);
#  fprint("isa_avx_properties_fadd_prop.cxx", $print_fadd_prop);
#  fprint("isa_avx_properties_fsub_prop.cxx", $print_fsub_prop);
#  fprint("isa_avx_properties_fmul_prop.cxx", $print_fmul_prop);
#  fprint("isa_avx_properties_flop_prop.cxx", $print_flop_prop);
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_load_only.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_load_exe.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_store_only.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_move_prop.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_fadd_prop.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_fsub_prop.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"isa_avx_properties_fmul_prop.cxx");
#  copy_to('../src/common/targ_info/isa/x8664/',"sa_avx_properties_flop_prop.cxx");
#}

#copy_to_property();
