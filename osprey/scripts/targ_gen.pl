#!/usr/bin/perl

# Operator record lines (see LAO/LIR/Operator.src file):
#
# 0  -- not in a record
# 1  -- operator name
# 2  -- operator syntax
# 3  -- operator format
# 4  -- operator attributes
# 5  -- operator operand count
# 6  -- operator predicate index (-1 means not predicated)
# 7  -- operator target index (for branches)
# 8  -- operator addressing mode (may be empty)
# 9  -- load/store base operand index (-1 if not applicable)
# 10 -- load/store offset operand index (-1 if not applicable)
# 11 -- access width (0 for not load/store)
# 12 -- access alignment (0 for not load/store)
# 13 --
# 14 --
# 15 --
# 16 -- relocation type
# 17 --

# ==================================================================
#    sort_by_attributes ()
#
#    It is not enough at this point to parse just the attributes
#    line. I need to parse the semantics file also, which for the
#    moment is LAO/LIR/SemTrees.src
# ==================================================================

sub sort_by_attributes {

    $line5 =~ /<td>(.*)<\/td>/;
    my $attributes = $1;
    my $simulated = 0;

    foreach my $attribute (split(/\|/,$attributes)) {
	if ($attribute =~ /OpAtt_(\w*)/) {
        }
	else {
	    print "ERROR: corrupted attribute list !\n";
	    exit(1);
	}

	if ($1 eq "GP32") { 
	    push(@{$AttrGroup{'gp32'}}, $Opcode);
	    push(@{$Subsets{'gp32'}}, $Opcode);
	}
	elsif ($1 eq "GP16") { 
	    push(@{$AttrGroup{'gp16'}}, $Opcode);
	    push(@{$Subsets{'gp16'}}, $Opcode);
	}
	elsif ($1 eq "GUARDED") {
	    push(@{$AttrGroup{'predicated'}}, $Opcode);
	}
	elsif ($1 eq "SYNTHETIC") {
	    push(@{$AttrGroup{'synthetic'}}, $Opcode);
#	    $simulated = 1;
	}
	elsif ($1 eq "LOAD") {
	    push(@{$AttrGroup{'load'}}, $Opcode);
	}
	elsif ($1 eq "STORE") {
	    push(@{$AttrGroup{'store'}}, $Opcode);
	}
	elsif ($1 eq "DU_GMI") {
	    push(@{$AttrGroup{'dugmi'}}, $Opcode);
	}
	elsif ($1 eq "AU_GMI") {
	    push(@{$AttrGroup{'augmi'}}, $Opcode);
	}
	elsif ($1 eq "GCI") {
	    push(@{$AttrGroup{'gci'}}, $Opcode);
	}
	elsif ($1 eq "MAC") {
	    push(@{$AttrGroup{'madd'}}, $Opcode);
	}
	elsif ($1 eq "GOTO") {
	    push(@{$AttrGroup{'jump'}}, $Opcode);
	}
	elsif ($1 eq "JUMP") {
	    push(@{$AttrGroup{'ijump'}}, $Opcode);
	}
	elsif ($1 eq "BRANCH") {
	    push(@{$AttrGroup{'branch'}}, $Opcode);
	}
	elsif ($1 eq "CALL") {
	    push(@{$AttrGroup{'call'}}, $Opcode);
	}
	elsif ($1 eq "RETURN") {
	    push(@{$AttrGroup{'ret'}}, $Opcode);
	}
	elsif ($1 eq "LINK") {
	    push(@{$AttrGroup{'link'}}, $Opcode);
	}
	elsif ($1 eq "TRAP") {
	    push(@{$AttrGroup{'trap'}}, $Opcode);
	}
	elsif ($1 eq "PACKED") {
	    push(@{$AttrGroup{'packed'}}, $Opcode);
	}
	elsif ($1 eq "POP") {
	    push(@{$AttrGroup{'pop'}}, $Opcode);
	}
	elsif ($1 eq "PUSH") {
	    push(@{$AttrGroup{'push'}}, $Opcode);
	}
	elsif ($1 eq "SENSITIVE") {
	    push(@{$AttrGroup{'unsafe'}}, $Opcode);
	}
	elsif ($1 eq "INTRINSIC") {
	    push(@{$AttrGroup{'intrinsic'}}, $Opcode);
	    push(@{$Subsets{'gp32'}}, $Opcode);
	}
	elsif ($1 eq "PSEUDO") {
	    push(@{$AttrGroup{'dummy'}}, $Opcode);
	    push(@{$Subsets{'gp32'}}, $Opcode);
	}
	elsif ($1 eq "AUTOMOD") {
	    # this will be taken care of by AddrMode processing
	    ;
	}
	elsif ($1 eq "LAO") {
	    push(@{$AttrGroup{'lao'}}, $Opcode);
	}
	elsif ($1 eq "INFO") {
	    push(@{$AttrGroup{'info'}}, $Opcode);
	}
	elsif ($1 eq "DATA") {
	    push(@{$AttrGroup{'data'}}, $Opcode);
	}
	elsif ($1 eq "HLIB") {
	    push(@{$AttrGroup{'hlib'}}, $Opcode);
	}
	elsif ($1 eq "LOOPSTART") {
	    push(@{$AttrGroup{'loopstart'}}, $Opcode);
	}
	elsif ($1 eq "LOOPEND") {
	    push(@{$AttrGroup{'loopend'}}, $Opcode);
	}
	elsif ($1 eq "PARTIAL") {
	    push(@{$AttrGroup{'partial'}}, $Opcode);
	}
	else {
	    print "ERROR: unknown attribute $1\n";
	    exit(1);
	}
    }

#    print "AttrGroups : \n";
#    foreach $group (keys(%AttrGroup)) {
#	print "$group: ";
#	foreach $attr (@{$AttrGroup{$group}}) {
#	    print "$attr, ";
#	}
#	print "\n";
#    }

    $simulated;
}

# ==================================================================
#    get_type_with_index (index)
#
#    It is not enough at this point to parse just the attributes
#    line. I need to parse the semantics file also, which for the
#    moment is LAO/LIR/SemTrees.src
# ==================================================================

sub get_type_with_index {

    my $operand;
    my $count = 1;
    my $type;
    my $operand_index = $_[0];
    my $kind = $_[1];

    $Opcode =~ /([a-zA-Z0-9]*)_([a-zA-Z0-9]*)_(\w*)/;
    my @signature = split(/_/, $3);

CONTINUE:
    foreach my $opnd (@signature) {
	$operand = $opnd;

	if (($operand eq "M") ||
	    ($operand eq "P") ||
	    ($operand eq "BM") ||
	    ($operand eq "BP") ||
	    ($operand eq "QM") ||
	    ($operand eq "QP") ||
	    ($operand eq "MQ") ) {

	    # addressing mode -- ignore:
	    next CONTINUE;
	}

	if ($count == $operand_index) {
	    goto BREAK;
	}
	$count++;
    }

  BREAK:

    if (($operand eq "GT") ||
	($operand eq "GF") ) {
	$type = "pr";
    }
    elsif (($operand eq "G0T") ||
	   ($operand eq "G0F") ||
	   ($operand eq "G0") ) {
	$type = "g0";
    }
    elsif ($operand eq "MD") {
	$type = "md";
    }
    elsif ($operand eq "U4") {
	$type = "u4";
    }
    elsif ($operand eq "U5") {
      # Some ST100 instructions, eg. ADDWA, ADDHA, LDW, LDH, etc.
      # scale their second operand. If the operand is an immediate
      # compiler can use a larger immediate that fits into U9 for
      # the ADDWA_GT_AR_AR_U9, for example, because the immediate
      # is scaled: 0x204 fits into U9 because scaled it corresponds
      # to 0x041. It corresponds to an illusion that ADDWA is able
      # to take a U11. I will create such illusion here. Ugly hack !
      # but the Operator.ent is not too well designed.
	if ($Opcode eq 'GP32_LDH_GT_DR_AR_BM_U5' ||
	    $Opcode eq 'GP32_LDH_GT_DR_AR_BP_U5' ||
	    $Opcode eq 'GP32_LDH_GT_DR_AR_MQ_U5' ||
	    $Opcode eq 'GP32_LDH_GT_DR_AR_QP_U5') {
	    $type = "u6";
	}
	elsif ($Opcode eq 'GP32_LDW_GT_DR_AR_BM_U5' ||
	       $Opcode eq 'GP32_LDW_GT_DR_AR_BP_U5' ||
	       $Opcode eq 'GP32_LDW_GT_DR_AR_MQ_U5' ||
	       $Opcode eq 'GP32_LDW_GT_DR_AR_QM_U5' ||
	       $Opcode eq 'GP32_LDW_GT_DR_AR_QP_U5' ||
	       $Opcode eq 'GP32_LDW_GT_MD_DR_AR_M_U5' ||
	       $Opcode eq 'GP32_LDW_GT_MD_DR_AR_P_U5' ||
	       $Opcode eq 'GP32_LDW_GT_MD_DR_AR_QM_U5' ||
	       $Opcode eq 'GP32_LDW_GT_MD_DR_AR_QP_U5') {
	    $type = "u7";
	}
	else {
	  $type = "u5";
        }
    }
    elsif ($operand eq "U6") {
	$type = "u6";
    }
    elsif ($operand eq "U7") {
	$type = "u7";
    }
    elsif ($operand eq "U8") {
	$type = "u8";
    }
    elsif ($operand eq "U9") {
      # Some ST100 instructions, eg. ADDWA, ADDHA, LDW, LDH, etc.
      # scale their second operand. If the operand is an immediate
      # compiler can use a larger immediate that fits into U9 for
      # the ADDWA_GT_AR_AR_U9, for example, because the immediate
      # is scaled: 0x204 fits into U9 because scaled it corresponds
      # to 0x041. It corresponds to an illusion that ADDWA is able
      # to take a U11. I will create such illusion here. Ugly hack !
      # but the Operator.ent is not too well designed.
	if ($Opcode eq 'GP32_LDH_GT_DR_AR_M_U9' ||
	    $Opcode eq 'GP32_LDH_GT_DR_AR_P_U9' ||
	    $Opcode eq 'GP32_ADDHA_GT_AR_AR_U9') {
	    $type = "u10";
	}
	elsif ($Opcode eq 'GP32_LDW_GT_DR_AR_M_U9' ||
	       $Opcode eq 'GP32_LDW_GT_DR_AR_P_U9' ||
	       $Opcode eq 'GP32_ADDWA_GT_AR_AR_U9') {
	    $type = "u11";
	}
	else {
	  $type = "u9";
        }
    }
    elsif ($operand eq "U10") {
	$type = "u10";
    }
    elsif ($operand eq "U12") {
	$type = "u12";
    }
    elsif ($operand eq "U15") {
	$type = "u15";
    }
    elsif ($operand eq "U16") {
	$type = "u16";
    }
    elsif (($operand eq "U20") ||
	   ($operand eq "RSET")) {
	$type = "u20";
    }
    elsif ($operand eq "U32") {
	$type = "u32";
    }
    elsif ($operand eq "S7") {
	$type = "s7";
    }
    elsif ($operand eq "S9") {
	$type = "s9";
    }
    elsif ($operand eq "S11") {
	$type = "s11";
    }
    elsif ($operand eq "S16") {
	$type = "s16";
    }
    elsif ($operand eq "S21") {
	$type = "s21";
    }
    elsif ($operand eq "S25") {
	$type = "s25";
    }
    elsif ($operand eq "S32") {
	$type = "s32";
    }
    elsif ($operand eq "S40") {
	$type = "s40";
    }
    elsif (($operand eq "P3") ||
	   ($operand eq "PR")) {
	$type = "p3_implicit";
    }
    elsif ($operand eq "P13") {
	$type = "p13";
    }
    elsif ($operand eq "P15") {
	$type = "p15";
    }
    elsif ($operand eq "DR") {
	$type = "int40";
    }
    elsif ($operand eq "DRL") {
	$type = "int40l";
    }
    elsif ($operand eq "DRH") {
	$type = "int40h";
    }
    elsif ($operand eq "AR") {
	$type = "ptr32";
    }
    elsif ($operand eq "ARL") {
	$type = "ptr32l";
    }
    elsif ($operand eq "ARH") {
	$type = "ptr32h";
    }
    elsif ($operand eq "CR") {
	$type = "ctrl";
    }
    elsif ($operand eq "CRL") {
	$type = "ctrll";
    }
    elsif ($operand eq "CRH") {
	$type = "ctrlh";
    }
    elsif ($operand eq "BR") {
	$type = "pr";
    }
    elsif ($operand eq "LR") {
	$type = "lr";
    }
    else {
	print "ERROR: unknown operand type of $operand!\n";
	exit(1);
    }

    # add attributes if it's a use:
    if ($kind eq "use") {
	# predicate ?
	$line7 =~ /<td>(\d+|-\d)<\/td>/;
	if ($count-1 == $1) {
	    $type = $type."_predicate";
	}
	# target ?
	$line8 =~ /<td>(\d+|-\d+)<\/td>/;
	if ($count-1 == $1) {
	    $type = $type."_target";
	}
	# base ?
	$line10 =~ /<td>(\d+|-\d)<\/td>/;
	if ($count-1 == $1) {
	    $type = $type."_base";
	}
	# offset ?
	$line11 =~ /<td>(\d+|-\d)<\/td>/;
	if ($count-1 == $1) {
	    $type = $type."_offset";
	}
	# stored value ???
    }

    $type;
}

# ==================================================================
#    add_operand_type
# ==================================================================

sub add_operand_type {

    my $type;
    my $operand = $_[0];
    my $kind = $_[1];

    if ($operand =~ /OP(\d*)/) {
	$type = &get_type_with_index ($1, $kind);
    }
    elsif ($operand eq "BR0") {
	$type = "g0";
    }
    elsif ($operand eq "DR3") {
	$type = "r3";
    }
    elsif ($operand eq "AR3") {
	$type = "p3";
    }
    elsif ($operand eq "AR11") {
	$type = "p11";
    }
    elsif ($operand eq "AR15") {
	$type = "p15";
    }
    elsif ($operand eq "CR8") {
	$type = "cr8";
    }
    elsif ($operand eq "CR9") {
	$type = "cr9";
    }
    elsif ($operand eq "CR29") {
	$type = "cr29";
    }
    elsif ($operand eq "LR0") {
	$type = "lr0";
    }
    elsif ($operand eq "LR1") {
	$type = "lr1";
    }
    elsif ($operand eq "LR2") {
	$type = "lr2";
    }
    elsif ($operand =~ /SR(\d*)/) {
	# sticky flags -- ignore for now
	$type = "";
    }
    else {
	print "ERROR: unknown operand type $operand\n";
	exit(1);
    }

    $type;
}

# ==================================================================
#    sort_by_operands
#
#    sort Opcodes by their signature.
# ==================================================================

sub sort_by_operands {

    my $type;

    my @operands;
    my @oplist;
    my @results;
    my @reslist;

    # first some special cases. Operator.ent does not satisfy:
    if ($Opcode eq 'GP32_CALL_S25') {
	push(@{$Signature{':GP32_CALL_S25:s25'}}, $Opcode);
	return;
    }
    elsif ($Opcode eq 'GP32_LINK_GT') {
	push(@{$Signature{':GP32_LINK_GT:pr_predicate,p3_implicit_target'}}, $Opcode);
	return;
    }

    # results:
    $line15 =~ /UsesDefs_(\w*)/;
    @results = split(/_/, $1);
    foreach my $result (@results) {
	$type = &add_operand_type ($result,"def");
	if ($type ne "") {
	    push (@reslist, $type);
	}
    }

    my $ressig = join(',', @reslist);

    # operands:
    $line14 =~ /UsesDefs_(\w*)/;
    @operands = split(/_/, $1);
    foreach my $operand (@operands) {
	$type = &add_operand_type ($operand,"use");
	if ($type ne "") {
	    push (@oplist, $type);
	}
    }

    my $opsig = join(',', @oplist);
    my $signature = $ressig.":*:".$opsig;
    print "SIGNATURE: $signature\n";

    push(@{$Signature{$signature}}, $Opcode);

    return;
}

# ==================================================================
#    sort_by_format
#
#    I replace <name> in the format line by "NAME"; I also add O/R
#    pattern to the end -- these serve as the printing group keys.
# ==================================================================

sub sort_by_format {

    my $i;

    $line6 =~ /<td>(\d*)<\/td>/;
    my $count = $1;
    $line14 =~ /UsesDefs_(\w*)/;
    my @operands = split(/_/, $1);

    my @string;
    my $found;

    for ($i = 0; $i < $count; $i++) {

	# form a group identifying string
	$found = 0;
	foreach my $operand (@operands) {
	    if ($operand =~ /OP(\d*)/) {
		if ($1 == $i+1) {
		    push (@string, "O");
		    $found = 1;
		    goto BREAK;
		}
	    }
	}
BREAK:
	if ($found == 0) {
	    push (@string, "R");
	}
    }

    my $pattern = join ('_',@string);
    $line4 =~ /<td>\"(.*)\"<\/td>/;
    my $format = $1;
    $line2 =~ /<td>\"(\w*)\"<\/td>/;
    my $name = $1;

    my @ft = split(' ',$format);
    my @new_ft;
    foreach my $elt (@ft) {
	if ($elt ne $name) {
	    push (@new_ft, $elt);
	}
	else {
	    push (@new_ft, "NAME");
	}
    }

    my $group = join('_',@new_ft);
    $group = $group."(".$pattern.")";

    print "FORMAT: $group ($Opcode)\n";

    push(@{$PrintGroup{$group}}, $Opcode);

    return;
}

# ==================================================================
#    emit_opcode
# ==================================================================

sub emit_opcode {

    my $skip = 0;

    if ($line1 =~ /<td>(\w*)<\/td>/) {
	$Opcode = $1;
	print "$Opcode\n";
    }
    else {
	print "  ERROR: can't find operator name !\n";
	exit (1);
    }

    # I need to skip some operators:
    if (($Opcode =~ /PSEUDO/) ||
	($Opcode =~ /INFO/) ||
	($Opcode =~ /DATA/) ) {
	$skip = 1;
    }

    if ($skip == 0) {
	print ISA_F "\t\t \"$Opcode\",\n";
    }

    $skip;
}

# ==================================================================
#    emit_properties
# ==================================================================

sub emit_properties {

    my $group;
    my $opcode;

    foreach $group (keys(%AttrGroup)) {
	print PROP_F "/* ====================================== */ \n";
        print PROP_F "  $group = ISA_Property_Create (\"$group\"); \n";
	print PROP_F "  Instruction_Group ($group, \n";
	foreach $opcode (@{$AttrGroup{$group}}) {
	    print PROP_F "\t\t TOP_$opcode, \n";
	}
	print PROP_F "\t\t TOP_UNDEFINED); \n\n";
    }

    return;
}

# ==================================================================
#    emit_subsets
# ==================================================================

sub emit_subsets {

    my $group;
    my $opcode;

    foreach $group (keys(%Subsets)) {
	print SUBS_F "  ISA_SUBSET st100_$group; \n";
    }
    print SUBS_F "\n\n";
    print SUBS_F "  ISA_Subset_Begin(\"st100\"); \n";

    foreach $group (keys(%Subsets)) {
	print SUBS_F "  st100_$group = ISA_Subset_Create(NULL,\"st100_$group\"); \n\n";
	print SUBS_F "  /* ==================================================================== \n";
	print SUBS_F "   *             ST100_$group Instructions \n";
	print SUBS_F "   * ==================================================================== \n";
	print SUBS_F "   */ \n";

        print SUBS_F "  Instruction_Group(st100_$group, \n";
	foreach $opcode (@{$Subsets{$group}}) {
	    print SUBS_F "\t\t TOP_$opcode, \n";
	}
	print SUBS_F "\t\t TOP_UNDEFINED); \n\n";
    }

    return;
}

# ==================================================================
#    emit_operands
# ==================================================================

sub emit_operands {

    
    my $signature;
    my $opcode;
    my $group = 0;

    my $rests;
    my $opnds;
    my @results;
    my @operands;

    foreach $signature (keys(%Signature)) {
	($rests, $gname, $opnds) = split(":", $signature);
	print OPND_F "  /* ====================================== */ \n";
	if ($gname eq '*') {
	    print OPND_F "  Instruction_Group(\"O_$group\", \n";
	    $group++;
	}
	else {
	    print OPND_F "  Instruction_Group(\"O_$gname\", \n";
	}
	foreach $opcode (@{$Signature{$signature}}) {
	    print OPND_F "\t\t TOP_$opcode, \n";
	}
	print OPND_F "\t\t TOP_UNDEFINED); \n\n";
	@results = split(",", $rests);
	@operands = split(",", $opnds);

	my $count;
	my $result;
	my $operand;

	$count = 0;
	foreach $result (@results) {
	    my @attributes = split("_", $result);
	    my $name = shift(@attributes);
	    my $attribute;
	    if (scalar(@attributes) == 0) {
		print OPND_F "  Result ($count, $name); \n";
	    }
	    else {
		foreach $attribute (@attributes) {
		    print OPND_F "  Result ($count, $name, $attribute); \n";
		}
	    }
	    $count++;
	}

	$count = 0;
	foreach $operand (@operands) {
	    my @attributes = split("_", $operand);
	    my $name = shift(@attributes);
	    my $attribute;
	    if (scalar(@attributes) == 0) {
		print OPND_F "  Operand ($count, $name); \n";
	    }
	    else {
		foreach $attribute (@attributes) {
		    print OPND_F "  Operand ($count, $name, $attribute); \n";
		}
	    }
	    $count++;
	}
    }

    return;
}

# ==================================================================
#    emit_printing_formats
# ==================================================================

sub emit_printing_formats {

    my $count = 0;

    foreach my $group (keys(%PrintGroup)) {
	$group =~ /(.*)\((\w*)\)/;
	my $ft = $1;
	my $pt = $2;
	my @format = split ('_',$ft);
	my @pattern = split ('_',$pt);
	my @new_format;
	my @new_pattern;
	my $opcode;

	foreach my $elt (@format) {

	    if ($elt eq "%R?") {
		push (@new_format, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "%R!") {
		push (@new_format, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif (($elt eq "%R") ||
		   ($elt eq "%S") ||
		   ($elt eq "%I") ||
		   ($elt eq "%M") ) {
		push (@new_format, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "%R,") {
		push (@new_format, "%s,");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq ",%I") {
		push (@new_format, ", %s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "\\\\%pr5") {
		push (@new_format, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt =~ /\%/) {
		print "ERROR: unknown print format $elt !!!\n";
		exit (1);
	    }
	    elsif ($elt eq "NAME") {
		push (@new_format, "\t%s");
		push (@new_pattern, "NAME");
	    }
	    else {
		# it should be OK ??
		push (@new_format, $elt);
	    }
	}

	my $new_ft = join (' ', @new_format);

	# print print group header
	print PRNT_F "  /* ================================= */ \n";
	print PRNT_F "  ISA_PRINT_TYPE print_$count; \n";
	print PRNT_F "  print_$count = ISA_Print_Type_Create(\"print_$count\", \"$new_ft\"); \n";

	my $opnd = 0;
	my $res = 0;
	foreach my $sym (@new_pattern) {
	    if ($sym eq "O") {
		print PRNT_F "  Operand($opnd); \n";
		$opnd++;
	    }
	    elsif ($sym eq "R") {
		print PRNT_F "  Result($res); \n";
		$res++;
	    }
	    else {
		print PRNT_F "  Name(); \n";
	    }
	}

	print PRNT_F "\n";
	print PRNT_F "  Instruction_Print_Group(print_$count, \n";
	foreach $opcode (@{$PrintGroup{$group}}) {
	    print PRNT_F "\t\t TOP_$opcode, \n";
	}
	print PRNT_F "\t\t TOP_UNDEFINED); \n";
	print PRNT_F "\n";

	$count++;
    }

    return;
}

# ==================================================================
#    emit_required_opcodes
#
#    The Pro64 back end requires a number of Opcodes to be defined:
#
#    // Simulated instructions
#      "asm",                    /* a ASM statement */
#      "intrncall",
#      "spadjust",               /* SP adjustment OP */
#      "copy.br",
#
#    // Dummy instructions
#      "begin.pregtn",
#      "end.pregtn",
#      "bwd.bar",
#      "fwd.bar",
#      "dfixup",
#      "ffixup",
#      "ifixup",
#      "label",
#      "noop"
#
# ==================================================================

sub emit_required_opcodes {

    print ISA_F "\t\t // Simulated: \n";
    print ISA_F "\t\t \"asm\", \n";
    print ISA_F "\t\t \"intrncall\", \n";
    print ISA_F "\t\t \"spadjust\", \n";
    print ISA_F "\t\t \"copy_br\", \n\n";

    print ISA_F "\t\t // Dummy instructions: \n";
    print ISA_F "\t\t \"begin_pregtn\", \n";
    print ISA_F "\t\t \"end_pregtn\", \n";
    print ISA_F "\t\t \"bwd_bar\", \n";
    print ISA_F "\t\t \"fwd_bar\", \n";
    print ISA_F "\t\t \"dfixup\", \n";
    print ISA_F "\t\t \"ffixup\", \n";
    print ISA_F "\t\t \"ifixup\", \n";
    print ISA_F "\t\t \"label\", \n";
    print ISA_F "\t\t \"noop\", \n\n";
}

# ==================================================================
#    add_required_to_subsets
#
#    add required opcodes to isa subsets, so they are 
#    also emitted by the emit_subsets.
#
#    TODO: now subset generating skripts do not allow one instruction
#    to be part of more than 1 ISA subset. I need to allow this for 
#    the simulated and dummy instructions.
# ==================================================================

sub add_required_to_subsets {

    push(@{$Subsets{'gp32'}}, 'asm');
    push(@{$Subsets{'gp32'}}, 'intrncall');
    push(@{$Subsets{'gp32'}}, 'spadjust');
    push(@{$Subsets{'gp32'}}, 'copy_br');
    push(@{$Subsets{'gp32'}}, 'begin_pregtn');
    push(@{$Subsets{'gp32'}}, 'end_pregtn');
    push(@{$Subsets{'gp32'}}, 'bwd_bar');
    push(@{$Subsets{'gp32'}}, 'fwd_bar');
    push(@{$Subsets{'gp32'}}, 'dfixup');
    push(@{$Subsets{'gp32'}}, 'ffixup');
    push(@{$Subsets{'gp32'}}, 'ifixup');
    push(@{$Subsets{'gp32'}}, 'label');
    push(@{$Subsets{'gp32'}}, 'noop');
}

# ==================================================================
#    add_required_to_properties
#
#    add required opcodes to isa properties group, so they are 
#    also emitted by the emit_properties.
# ==================================================================

sub add_required_to_properties {

    # simulated:
    push(@{$AttrGroup{'simulated'}}, 'asm');
    push(@{$AttrGroup{'simulated'}}, 'intrncall');
    push(@{$AttrGroup{'simulated'}}, 'spadjust');
    push(@{$AttrGroup{'simulated'}}, 'copy_br');

    # dummy:
    push(@{$AttrGroup{'dummy'}}, 'begin_pregtn');
    push(@{$AttrGroup{'dummy'}}, 'end_pregtn');
    push(@{$AttrGroup{'dummy'}}, 'bwd_bar');
    push(@{$AttrGroup{'dummy'}}, 'fwd_bar');
    push(@{$AttrGroup{'dummy'}}, 'dfixup');
    push(@{$AttrGroup{'dummy'}}, 'ffixup');
    push(@{$AttrGroup{'dummy'}}, 'ifixup');
    push(@{$AttrGroup{'dummy'}}, 'label');
    push(@{$AttrGroup{'dummy'}}, 'noop');

    #var_operands:
    push(@{$AttrGroup{'var_opnds'}}, 'asm');
}

# ==================================================================
#    add_required_to_operands
# ==================================================================

sub add_required_to_operands {

    push(@{$Signature{':asm:'}}, 'asm');
    push(@{$Signature{'int40:intrncall:int40,int40,int40'}}, 'intrncall');

    push(@{$Signature{'ptr32:spadjust:pr_predicate,ptr32,s32'}}, 'spadjust');
    push(@{$Signature{'ptr32:copy_br:pr_predicate,ptr32'}}, 'copy_br');

    push(@{$Signature{':pregtn:int40,s16'}}, 'begin_pregtn');
    push(@{$Signature{':pregtn:int40,s16'}}, 'end_pregtn');
    push(@{$Signature{':noop:'}}, 'bwd_bar');
    push(@{$Signature{':noop:'}}, 'fwd_bar');
    push(@{$Signature{'int40:fixup:'}}, 'dfixup');
    push(@{$Signature{'ptr32:fixup:'}}, 'ffixup');
    push(@{$Signature{'int40:fixup:'}}, 'ifixup');
    push(@{$Signature{':label:pcrel'}}, 'label');
    push(@{$Signature{':noop:'}}, 'noop');
}

# ==================================================================
#    initialize_isa_file
# ==================================================================

sub initialize_isa_file {

    open (ISA_F, "> isa.cxx");

    print ISA_F "// AUTOMATICALLY GENERATED FROM ARC DATA BASE !!! \n\n";
    print ISA_F "// Generate an ISA containing the instructions specified. \n";
    print ISA_F "///////////////////////////////////////////////////////// \n";
    print ISA_F "// The instructions are listed by category. The different \n";
    print ISA_F "// categories of instructions are:\n";
    print ISA_F "// \n";
    print ISA_F "//   1. DU instructions \n";
    print ISA_F "//   2. AU instructions \n";
    print ISA_F "//   3. Intrinsic instructions \n";
    print ISA_F "// Following must be declared, back end needs them: \n";
    print ISA_F "//   4. Simulated instructions \n";
    print ISA_F "//   5. Dummy instructions \n";
    print ISA_F "// \n";
    print ISA_F "// Within each category, the instructions are in alphabetical order.\n";
    print ISA_F "// This arrangement of instructions matches the order in the ISA manual.\n";
    print ISA_F "/////////////////////////////////////\n\n\n";
    print ISA_F "#include <stddef.h>\n";
    print ISA_F "#include \"isa_gen.h\"\n\n\n";

    print ISA_F "main ()\n";
    print ISA_F "{\n";
    print ISA_F "  ISA_Create (\"st100\", \n";

    return;
}

# ==================================================================
#    finalize_isa_file
# ==================================================================

sub finalize_isa_file {

    print ISA_F "      NULL);\n";
    print ISA_F "}\n";

    close (ISA_F);
    return;
}

# ==================================================================
#    initialize_properties_file
# ==================================================================

sub initialize_properties_file {

    open (PROP_F, "> isa_properties.cxx");

    print PROP_F "//  AUTOMATICALLY GENERATED FROM ARC DATABASE !!!\n\n";
    print PROP_F "//  Generate ISA properties information \n";
    print PROP_F "/////////////////////////////////////// \n";
    print PROP_F "// The different operators are listed as having the\n";
    print PROP_F "// following ISA_PROPERTIES: \n";
    print PROP_F "// \n";
    print PROP_F "///////////////////////////////////// \n";

    print PROP_F "#include <stddef.h> \n";
    print PROP_F "#include \"topcode.h\" \n";
    print PROP_F "#include \"isa_properties_gen.h\" \n\n";

    print PROP_F "main() \n";
    print PROP_F "{ \n";
    print PROP_F "  ISA_PROPERTY \n";
    print PROP_F "    gp32,            /* a GP32 operator */ \n";
    print PROP_F "    gp16,            /* a GP16 operator */ \n";
    print PROP_F "    duadd,            /* DU add */ \n";
    print PROP_F "    dusub,            /* DU sub */ \n";
    print PROP_F "    dumul,            /* DU mul */ \n";
    print PROP_F "    dugmi,           /* DU GMI operator */ \n";
    print PROP_F "    dumisc,           /* other DU operator */ \n";
    print PROP_F "    auadd,            /* AU add */ \n";
    print PROP_F "    ausub,            /* AU sub */ \n";
    print PROP_F "    aumul,            /* AU mul */ \n";
    print PROP_F "    augmi,           /* AU GMI operator */ \n";
    print PROP_F "    aumisc,           /* other AU operator */ \n";
    print PROP_F "    load,             /* Memory load operator */ \n";
    print PROP_F "    store,   		/* Memory store operator */ \n";
    print PROP_F "    jump,		/* Goto operator */ \n";
    print PROP_F "    ijump,		/* Indirect jump operator */ \n";
    print PROP_F "    branch,           /* Conditional branch */ \n";
    print PROP_F "    call, 		/* Subprogram call operator */ \n";
    print PROP_F "    link,             /* Link ??? */ \n";
    print PROP_F "    ret,           /* return from subroutine operator */ \n";
    print PROP_F "    trap,             /* trap operator */ \n";
    print PROP_F "    madd,             /* Operator is a MAC */ \n";
    print PROP_F "    gci,              /* ?? */ \n";
    print PROP_F "    packed,           /* Packed operator */ \n";
    print PROP_F "    pop,              /* ?? operator */ \n";
    print PROP_F "    push,             /* ?? operator */ \n";
    print PROP_F "    predicated,       /* Operator is predicated */ \n";
    print PROP_F "    unalign_ld, 	/* Unaligned load operator */ \n";
    print PROP_F "    unalign_store,	/* Unaligned store operator */ \n";
    print PROP_F "    same_res,		/* Result must be same as opnd */ \n";
    print PROP_F "    noop, 		/* No-op operator */ \n";
    print PROP_F "    info,             /* ?? */ \n";
    print PROP_F "    safe,		/* Never traps -- always safe */ \n";
    print PROP_F "    unsafe,		/* Unsafe always */ \n";
    print PROP_F "    memtrap,		/* Memory trap potential */ \n";
    print PROP_F "    intrinsic,        /* Instruction is intrinsic */ \n";
    print PROP_F "    side_effects,	/* Instruction has side effects */ \n";
    print PROP_F "    synthetic,        /* Synthetic ?? */ \n";

    print PROP_F "    lao,              /* ?? */ \n";
    print PROP_F "    data,             /* ?? */ \n";
    print PROP_F "    hlib,             /* ?? */ \n";
    print PROP_F "    loopstart,        /* ?? */ \n";
    print PROP_F "    loopend,          /* ?? */ \n";
    print PROP_F "    partial,          /* ?? */ \n\n";

    print PROP_F "    /* Following properties must be declared, back end \n";
    print PROP_F "     * requires this: \n";
    print PROP_F "     */ \n";
    print PROP_F "    simulated,        /* a macro */ \n";
    print PROP_F "    dummy,            /* Instruction is pseudo */  \n";
    print PROP_F "    var_opnds;        /* Variable number of operands AND/OR results */ \n";
    print PROP_F "\n";

    print PROP_F "  ISA_Properties_Begin (\"st100\"); \n\n\n";

    print PROP_F "/* ==================================================================== \n";
    print PROP_F " *              Operator attributes descriptors \n";
    print PROP_F " * ==================================================================== \n";
    print PROP_F " */ \n\n";

    return;
}

# ==================================================================
#    finalize_properties_file
# ==================================================================

sub finalize_properties_file {

    print PROP_F "  ISA_Properties_End();\n";
    print PROP_F "  return 0;\n";
    print PROP_F "}\n";

    close (PROP_F);
    return;
}

# ==================================================================
#    initialize_subset_file
# ==================================================================

sub initialize_subset_file {

    open (SUBS_F, "> isa_subset.cxx");

    print SUBS_F "//  AUTOMATICALLY GENERATED FROM ARC DATABASE !!! \n";
    print SUBS_F "//  Generate ISA subset descriptions \n";
    print SUBS_F "///////////////////////////////////// \n";
    print SUBS_F "// The different categories of instructions are: \n";
    print SUBS_F "// \n";
    print SUBS_F "//   1. GP32 instructions; \n";
    print SUBS_F "//   2. GP16 instructions; \n";
    print SUBS_F "// \n";
    print SUBS_F "// as shown in the ISA manual \n";
    print SUBS_F "///////////////////////////////////// \n\n";

    print SUBS_F "#include <stddef.h> \n";
    print SUBS_F "#include \"topcode.h\" \n";
    print SUBS_F "#include \"isa_subset_gen.h\" \n\n";

    print SUBS_F "main() \n";
    print SUBS_F "{ \n";

    return;
}

# ==================================================================
#    finalize_subset_file
# ==================================================================

sub finalize_subset_file {

    print SUBS_F "  ISA_Subset_End(); \n";
    print SUBS_F "  return 0; \n";
    print SUBS_F "} \n";

    close (SUBS_F);

    return;
}

# ==================================================================
#    initialize_operands_file
# ==================================================================

sub initialize_operands_file {

    open (OPND_F, "> isa_operands.cxx");

    print OPND_F "//  AUTOMATICALLY GENERATED FROM ARC DATABASE !!! \n";
    print OPND_F "// Group TOPS with similar operands/results format. \n";
    print OPND_F "///////////////////////////////////////////////////////// \n";
    print OPND_F "// Within each category, the instructions are arranged roughly in order \n";
    print OPND_F "// of increasing numbers of operands. \n";
    print OPND_F "///////////////////////////////////// \n\n\n";

    print OPND_F "#include <stddef.h> \n";
    print OPND_F "#include \"topcode.h\" \n";
    print OPND_F "#include \"isa_operands_gen.h\" \n\n";

    print OPND_F "main() \n";
    print OPND_F "{ \n\n";

    print OPND_F "  OPERAND_VALUE_TYPE int40, ptr32; \n";
    print OPND_F "  OPERAND_VALUE_TYPE lr, lr0, lr1, lr2; \n";
    print OPND_F "  OPERAND_VALUE_TYPE p3, p11, p13, p15; \n";
    print OPND_F "  OPERAND_VALUE_TYPE pr, g0; \n";
    print OPND_F "  OPERAND_VALUE_TYPE ctrl, ctrll, ctrlh, cr8, cr9, cr29; \n";
    print OPND_F "  OPERAND_VALUE_TYPE int40l, int40h, r3; \n";
    print OPND_F "  OPERAND_VALUE_TYPE ptr32l, ptr32h; \n";
    print OPND_F "  OPERAND_VALUE_TYPE md; \n";
    print OPND_F "  OPERAND_VALUE_TYPE u4, u5, u6, u7, u8, u9, u10, u11, u12, u15, u16, u20, u32; \n";
    print OPND_F "  OPERAND_VALUE_TYPE s7, s9, s11, s16, s21, s25, s32, s40; \n";
    print OPND_F "  OPERAND_VALUE_TYPE absadr, pcrel; \n";
    print OPND_F "\n";

    print OPND_F "  OPERAND_USE_TYPE \n";
    print OPND_F "	  predicate,	// a qualifying predicate \n";
    print OPND_F "	  base,		// a base address (memory insts) \n";
    print OPND_F "	  offset,	// an offset added to a base (imm) \n";
    print OPND_F "	  target,	// the target of a branch \n";
    print OPND_F "	  storeval,	// value to be stored \n";
    print OPND_F "        implicit;     // implicitely used by instruction \n";
    print OPND_F "\n";

    print OPND_F "  ISA_Operands_Begin(\"st100\"); \n";

    print OPND_F "  /* Create the register operand types: */ \n";

    print OPND_F "  pr = ISA_Reg_Opnd_Type_Create(\"pr\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_guard, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_UNDEFINED, \n";
    print OPND_F "		  1, UNSIGNED, INVALID); \n";
    print OPND_F "  lr = ISA_Reg_Opnd_Type_Create(\"lr\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_loop, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_UNDEFINED, \n";
    print OPND_F "		  1, UNSIGNED, INVALID); \n";
    print OPND_F "  lr0 = ISA_Reg_Opnd_Type_Create(\"lr0\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_loop, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_lr0, \n";
    print OPND_F "		  1, UNSIGNED, INVALID); \n";
    print OPND_F "  lr1 = ISA_Reg_Opnd_Type_Create(\"lr1\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_loop, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_lr1, \n";
    print OPND_F "		  1, UNSIGNED, INVALID); \n";
    print OPND_F "  lr2 = ISA_Reg_Opnd_Type_Create(\"lr2\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_loop, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_lr2, \n";
    print OPND_F "		  1, UNSIGNED, INVALID); \n";
    print OPND_F "  g0 = ISA_Reg_Opnd_Type_Create(\"g0\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_guard, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_g0, \n";
    print OPND_F "		  1, UNSIGNED, INVALID); \n";
    print OPND_F "  int40 = ISA_Reg_Opnd_Type_Create(\"int40\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_du, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_UNDEFINED, \n";
    print OPND_F "		  40, SIGNED, INVALID); \n";
    print OPND_F "  r3 = ISA_Reg_Opnd_Type_Create(\"r3\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_du, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_r3, \n";
    print OPND_F "		  40, SIGNED, INVALID); \n";
    print OPND_F "  ptr32 = ISA_Reg_Opnd_Type_Create(\"ptr32\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_UNDEFINED, \n";
    print OPND_F "		  32, SIGNED, INVALID); \n";
    print OPND_F "  p3 = ISA_Reg_Opnd_Type_Create(\"p3\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_p3, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  p11 = ISA_Reg_Opnd_Type_Create(\"p11\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_p11, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  p13 = ISA_Reg_Opnd_Type_Create(\"p13\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_p13, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  p15 = ISA_Reg_Opnd_Type_Create(\"p15\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_p15, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  cr8 = ISA_Reg_Opnd_Type_Create(\"cr8\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_control, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_cr8, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  cr9 = ISA_Reg_Opnd_Type_Create(\"cr9\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_control, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_cr9, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  cr29 = ISA_Reg_Opnd_Type_Create(\"cr29\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_control, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_cr29, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  ctrl = ISA_Reg_Opnd_Type_Create(\"ctrl\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_control, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_UNDEFINED, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  int40l = ISA_Reg_Opnd_Type_Create(\"int40_l\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_du, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_drl, \n";
    print OPND_F "		  40, SIGNED, INVALID); \n";
    print OPND_F "  int40h = ISA_Reg_Opnd_Type_Create(\"int40_h\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_du, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_drh, \n";
    print OPND_F "		  40, SIGNED, INVALID); \n";
    print OPND_F "  ptr32l = ISA_Reg_Opnd_Type_Create(\"ptr32_l\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_arl, \n";
    print OPND_F "		  32, SIGNED, INVALID); \n";
    print OPND_F "  ptr32h = ISA_Reg_Opnd_Type_Create(\"ptr32_h\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_au, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_arh, \n";
    print OPND_F "		  32, SIGNED, INVALID); \n";
    print OPND_F "  ctrll = ISA_Reg_Opnd_Type_Create(\"ctrl_l\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_control, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_crl, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "  ctrlh = ISA_Reg_Opnd_Type_Create(\"ctrl_h\", \n";
    print OPND_F "                ISA_REGISTER_CLASS_control, \n";
    print OPND_F "		  ISA_REGISTER_SUBCLASS_crh, \n";
    print OPND_F "		  32, UNSIGNED, INVALID); \n";
    print OPND_F "\n";

    print OPND_F "  /* Create the enum operand types: */ \n";
    print OPND_F "\n";
    print OPND_F "  md = ISA_Enum_Opnd_Type_Create(\"md\", 8, UNSIGNED, EC_amod); \n";
    print OPND_F "\n";

    print OPND_F "  /* Create the literal operand types: */ \n";
    print OPND_F "\n";
    print OPND_F "  u4   = ISA_Lit_Opnd_Type_Create(\"u4\",   4, UNSIGNED, LC_u4); \n";
    print OPND_F "  u5   = ISA_Lit_Opnd_Type_Create(\"u5\",   5, UNSIGNED, LC_u5); \n";
    print OPND_F "  u6   = ISA_Lit_Opnd_Type_Create(\"u6\",   6, UNSIGNED, LC_u6); \n";
    print OPND_F "  u7   = ISA_Lit_Opnd_Type_Create(\"u7\",   7, UNSIGNED, LC_u7); \n";
    print OPND_F "  u8   = ISA_Lit_Opnd_Type_Create(\"u8\",   8, UNSIGNED, LC_u8); \n";
    print OPND_F "  u9   = ISA_Lit_Opnd_Type_Create(\"u9\",   9, UNSIGNED, LC_u9); \n";
    print OPND_F "  u10   = ISA_Lit_Opnd_Type_Create(\"u10\",   10, UNSIGNED, LC_u10); \n";
    print OPND_F "  u11   = ISA_Lit_Opnd_Type_Create(\"u11\",   11, UNSIGNED, LC_u11); \n";
    print OPND_F "  u12   = ISA_Lit_Opnd_Type_Create(\"u12\",   12, UNSIGNED, LC_u12); \n";
    print OPND_F "  u15   = ISA_Lit_Opnd_Type_Create(\"u15\",   15, UNSIGNED, LC_u15); \n";
    print OPND_F "  u16   = ISA_Lit_Opnd_Type_Create(\"u16\",   16, UNSIGNED, LC_u16); \n";
    print OPND_F "  u20   = ISA_Lit_Opnd_Type_Create(\"u20\",   20, UNSIGNED, LC_u20); \n";
    print OPND_F "  u32   = ISA_Lit_Opnd_Type_Create(\"u32\",   32, UNSIGNED, LC_u32); \n";

    print OPND_F "  s7   = ISA_Lit_Opnd_Type_Create(\"s7\",   7, SIGNED, LC_s7); \n";
    print OPND_F "  s9   = ISA_Lit_Opnd_Type_Create(\"s9\",   9, SIGNED, LC_s9); \n";
    print OPND_F "  s11   = ISA_Lit_Opnd_Type_Create(\"s11\",   11, SIGNED, LC_s11); \n";
    print OPND_F "  s16   = ISA_Lit_Opnd_Type_Create(\"s16\",   16, SIGNED, LC_s16); \n";
    print OPND_F "  s21   = ISA_Lit_Opnd_Type_Create(\"s21\",   21, SIGNED, LC_s21); \n";
    print OPND_F "  s25   = ISA_Lit_Opnd_Type_Create(\"s25\",   25, SIGNED, LC_s25); \n";
    print OPND_F "  s32   = ISA_Lit_Opnd_Type_Create(\"s32\",   32, SIGNED, LC_s32); \n";
    print OPND_F "  s40   = ISA_Lit_Opnd_Type_Create(\"s40\",   40, SIGNED, LC_s40); \n";

    print OPND_F "  pcrel   = ISA_Lit_Opnd_Type_Create(\"pcrel\",   32, PCREL, LC_s32); \n";
    print OPND_F "  absadr   = ISA_Lit_Opnd_Type_Create(\"absadr\",   32, UNSIGNED, LC_u32); \n";

    print OPND_F "\n";

    print OPND_F "  /* Create the operand uses: */ \n";
    print OPND_F "\n";

    print OPND_F "  predicate  = Create_Operand_Use(\"predicate\"); \n";
    print OPND_F "  offset     = Create_Operand_Use(\"offset\"); \n";
    print OPND_F "  base       = Create_Operand_Use(\"base\"); \n";
    print OPND_F "  target     = Create_Operand_Use(\"target\"); \n";
    print OPND_F "  storeval   = Create_Operand_Use(\"storeval\"); \n";
    print OPND_F "  implicit   = Create_Operand_Use(\"implicit\"); \n";

    print OPND_F "\n";

    return;
}

# ==================================================================
#    finalize_operands_file
# ==================================================================

sub finalize_operands_file {

    print OPND_F "\n\n";
    print OPND_F "  ISA_Operands_End(); \n";
    print OPND_F "  return 0; \n";
    print OPND_F "} \n";

    close (OPND_F);
    return;
}

# ==================================================================
#    initialize_print_file
# ==================================================================

sub initialize_print_file {

    open (PRNT_F, "> isa_print.cxx");

    print PRNT_F "//  AUTOMATICALLY GENERATED FROM ARC DATABASE !!! \n";
    print PRNT_F "// Group TOPS with similar printing format. \n";
    print PRNT_F "//////////////////////////////////////////////////\n\n\n";

    print PRNT_F "#include <stdio.h> \n";
    print PRNT_F "#include <stddef.h> \n";
    print PRNT_F "#include <string.h> \n";
    print PRNT_F "#include <ctype.h> \n";
    print PRNT_F "#include \"topcode.h\" \n";
    print PRNT_F "#include \"isa_print_gen.h\" \n\n";

    print PRNT_F "// Multiple topcodes map to the same assembly name. To disambiguate the \n";
    print PRNT_F "// topcodes, we append a signature to the basename. To get the assembly \n";
    print PRNT_F "// name we must strip off the suffix. \n\n";

    print PRNT_F "static const char *asmname(TOP topcode) \n";
    print PRNT_F "{ \n";
    print PRNT_F "  int c; \n";
    print PRNT_F "  int i; \n";
    print PRNT_F "  int j; \n";
    print PRNT_F "  int k; \n";
    print PRNT_F "  const char *name = TOP_Name(topcode); \n";
    print PRNT_F "  char buf[100]; \n";
    print PRNT_F "  char lower_buf[100]; \n\n";

    print PRNT_F "  /* \n";
    print PRNT_F "   * By convention we have GP32 or GP16 followed by _<name>_ \n";
    print PRNT_F "   */ \n";
    print PRNT_F "  for (i = 0; ; i++) { \n";
    print PRNT_F "    c = name[i]; \n";
    print PRNT_F "    if (c == '_') break; \n";
    print PRNT_F "    buf[i] = c; \n";
    print PRNT_F "  } \n\n";

    print PRNT_F "  buf[i] = '\\0'; \n";
    print PRNT_F "  k = 0; \n";
    print PRNT_F "  // if this is an intrinsic, prepend __ to the name: \n";
    print PRNT_F "  if (!strcmp (buf, \"IFR\")) { \n";
    print PRNT_F "    buf[k++] = '_'; \n";
    print PRNT_F "    buf[k++] = '_'; \n";
    print PRNT_F "  } \n\n";

    print PRNT_F "  for (j = k; ; j++, i++) { \n";
    print PRNT_F "    c = name[i+1]; \n";
    print PRNT_F "    if (c == '\\0' || c == '_') break; \n";
    print PRNT_F "    buf[j] = c; \n";
    print PRNT_F "  } \n\n";

    print PRNT_F "  buf[j] = '\\0'; \n\n";

    print PRNT_F "  // convert to lower case \n";
    print PRNT_F "  for (i = 0; i <= j; i++) { \n";
    print PRNT_F "    lower_buf[i] = tolower(buf[i]); \n";
    print PRNT_F "  } \n\n";

    print PRNT_F "  return strdup(lower_buf); \n";
    print PRNT_F "} \n\n";

    print PRNT_F "main() \n";
    print PRNT_F "{ \n";

    print PRNT_F "  ISA_Print_Begin(\"st100\"); \n\n";

    print PRNT_F "  Set_AsmName_Func(asmname); \n\n";

    return;
}

# ==================================================================
#    finalize_print_file
# ==================================================================

sub finalize_print_file {

    print PRNT_F "\n\n";
    print PRNT_F "  ISA_Print_End(); \n";
    print PRNT_F "  return 0; \n";
    print PRNT_F "} \n";

    close (PRNT_F);
    return;
}

# ==================================================================
#    main
# ==================================================================

$state = 0;

&initialize_isa_file ();
&initialize_operands_file ();
&initialize_subset_file ();
&initialize_properties_file ();
&initialize_print_file ();

CONTINUE:

while ($line = <ARGV>) {

    if ($line =~ /<tr>/) {
	$state = 1;
    }
    if ($line =~ /<\/tr>/) {
	$state = 0;
    }

    if ($state == 1) {

	$line1 = <ARGV>;
	$line2 = <ARGV>;
	$line3 = <ARGV>;
	$line4 = <ARGV>;
	$line5 = <ARGV>;
	$line6 = <ARGV>;
	#read operator predicate operand index
	$line7 = <ARGV>;
	#read operator target operand index
	$line8 = <ARGV>;
	#read operator addressing mode
	$line9 = <ARGV>;
	#read operator base operand index
	$line10 = <ARGV>;
	#read operator offset operand index
	$line11 = <ARGV>;
	#read operator memory access width
	$line12 = <ARGV>;
	#read operator memory alignment
	$line13 = <ARGV>;
	#read operator uses
	$line14 = <ARGV>;
	#read operator defs
	$line15 = <ARGV>;

	$line16 = <ARGV>;
	$line17 = <ARGV>;
	$line18 = <ARGV>;

	my $skip = &emit_opcode ();
	if ($skip == 1) {
	    next CONTINUE;
	}

	my $simulated = &sort_by_attributes ();
	&sort_by_operands ();

	if (!($Opcode =~ /PSEUDO/) &&
	    ($simulated != 1) ) {
	    &sort_by_format ();
	}
    }
}

# Opcodes that are required by the back end:
&emit_required_opcodes ();

# Properties for required opcodes:
&add_required_to_subsets ();
&add_required_to_properties ();
&add_required_to_operands ();

# Emit the information to corresponding files:
&emit_properties ();
&emit_subsets ();
&emit_operands ();
&emit_printing_formats ();

# end of generation

&finalize_isa_file ();
&finalize_operands_file ();
&finalize_subset_file ();
&finalize_properties_file ();
&finalize_print_file ();



