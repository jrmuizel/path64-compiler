#!/usr/bin/perl

# Generate target description files for the ST200 processor


# ==================================================================
#    opcode_is_simulated
# ==================================================================

sub opcode_is_simulated {
    my $opcode;
    my $gname = $_[0];

    foreach $opcode (@SimulatedOpcodes) {
	if ($gname eq $opcode) {
	    return 1;
	}
    }

    return 0;
}

# ==================================================================
#    opcode_is_dummy
# ==================================================================

sub opcode_is_dummy {
    my $opcode;
    my $gname = $_[0];

    foreach $opcode (@DummyOpcodes) {
	if ($gname eq $opcode) {
	    return 1;
	}
    }

    return 0;
}

# ==================================================================
#    op_variant
# ==================================================================

sub op_variant {
    my $format = $_[0];
    my $variant;

    if ($format eq 'Int3R') {
	$variant = '_r';
    }
    elsif ($format eq 'Int3I') {
	$variant = '_i';
    }
    elsif ($format eq 'Cmp3R_Reg') {
	$variant = '_r_r';
    }
    elsif ($format eq 'Cmp3I_Reg') {
	$variant = '_i_r';
    }
    elsif ($format eq 'Cmp3R_Br') {
	$variant = '_r_b';
    }
    elsif ($format eq 'Cmp3I_Br') {
	$variant = '_i_b';
    }
    elsif ($format eq 'SelectR') {
	$variant = '_r';
    }
    elsif ($format eq 'SelectI') {
	$variant = '_i';
    }
    elsif ($format eq 'cgen' ||
	   $format eq 'Imm' ||
	   $format eq 'Load' ||
	   $format eq 'Store' ||
	   $format eq 'Branch' ||
	   $format eq 'SysOp') {
	$variant = "";
    }
    elsif ($format eq 'Call') {
	$variant = "";
    }
    else {
	printf(STDOUT "ERROR: unknown format %s in op_variant\n", 
	       $format);
	exit(1);
    }

    return $variant;
}

# ==================================================================
#    op_bytes
#
#    Do not know how to get this from CHESS
# ==================================================================

sub op_bytes {
    my $mnemonic = $OP_mnemonic[$_[0]];
    my $bytes;

    if ($mnemonic eq 'ldw' ||
	$mnemonic eq 'ldw_d' ||
	$mnemonic eq 'stw') {
	$bytes = 4;
    }
    elsif ($mnemonic eq 'ldh' ||
	   $mnemonic eq 'ldh_d' ||
	   $mnemonic eq 'ldhu' ||
	   $mnemonic eq 'ldhu_d' ||
	   $mnemonic eq 'sth') {
	$bytes = 2;
    }
    elsif ($mnemonic eq 'ldb' ||
	   $mnemonic eq 'ldb_d' ||
	   $mnemonic eq 'ldbu' ||
	   $mnemonic eq 'ldbu_d' ||
	   $mnemonic eq 'stb') {
	$bytes = 1;
    }
    else {
	printf(STDOUT "ERROR: unknown mnemonic %s in op_bytes\n", 
	       $mnemonic);
	exit(1);
    }

    return $bytes;
}

# ==================================================================
#    op_align
#
#    Do not know how to get this from CHESS
# ==================================================================

sub op_align {
    my $mnemonic = $OP_mnemonic[$_[0]];
    my $align;

    if ($mnemonic eq 'ldw' ||
	$mnemonic eq 'ldw_d' ||
	$mnemonic eq 'stw') {
	$align = 4;
    }
    elsif ($mnemonic eq 'ldh' ||
	   $mnemonic eq 'ldh_d' ||
	   $mnemonic eq 'ldhu' ||
	   $mnemonic eq 'ldhu_d' ||
	   $mnemonic eq 'sth') {
	$align = 2;
    }
    elsif ($mnemonic eq 'ldb' ||
	   $mnemonic eq 'ldb_d' ||
	   $mnemonic eq 'ldbu' ||
	   $mnemonic eq 'ldbu_d' ||
	   $mnemonic eq 'stb') {
	$align = 1;
    }
    else {
	printf(STDOUT "ERROR: unknown mnemonic %s in op_align\n", 
	       $mnemonic);
	exit(1);
    }

    return $align;
}

# ==================================================================
#    sort_branch
#
#    Do not know how to get this from CHESS
# ==================================================================

sub sort_branch {
    my $opcode = $_[0];
    my $mnemonic = $OP_mnemonic[$opcode];

    if ($mnemonic eq 'br' ||
	$mnemonic eq 'brf') {
	$OP_properties[$opcode] ^= $OP_BRANCH;
    }
    else {
	printf(STDOUT "ERROR: unknown mnemonic %s in sort_branch\n", 
	       $mnemonic);
	exit(1);
    }

    return;
}

# ==================================================================
#    op_signature
# ==================================================================

sub op_signature {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];
    my $signature;

    if ($format eq 'Int3R') {
	$signature = 'dest:Int3R:src1,src2';
    }
    elsif ($format eq 'Int3I') {
	$signature = 'idest:Int3I:src1,isrc2';
    }
    elsif ($format eq 'Cmp3R_Reg') {
	$signature = 'dest:Cmp3R_Reg:src1,src2';
    }
    elsif ($format eq 'Cmp3I_Reg') {
	$signature = 'idest:Cmp3I_Reg:src1,isrc2';
    }
    elsif ($format eq 'Cmp3R_Br') {
	$signature = 'bdest:Cmp3R_Br:src1,src2';
    }
    elsif ($format eq 'Cmp3I_Br') {
	$signature = 'ibdest:Cmp3I_Br:src1,isrc2';
    }
    elsif ($format eq 'SelectR') {
	$signature = 'dest:SelectR:bcond,src1,src2';
    }
    elsif ($format eq 'SelectI') {
	$signature = 'idest:SelectI:bcond,src1,isrc2';
    }
    elsif ($format eq 'cgen') {
	$signature = 'dest,bdest:cgen:src1,src2,bcond';
    }
    elsif ($format eq 'Imm') {
	$signature = ':Imm:imm';
    }
    elsif ($format eq 'Load') {
	$signature = 'dest:Load:isrc2_offset,src1_base';
    }
    elsif ($format eq 'Store') {
	$signature = ':Store:isrc2_offset,src1_base,src2_storeval';
    }
    elsif ($format eq 'Branch') {
	$signature = ':Branch:bcond,btarg_target';
    }
    elsif ($format eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'call') {
	    $signature = 'lr:call:btarg';
	}
	elsif ($OP_mnemonic[$opcode] eq 'goto') {
	    $signature = 'lr:jump:btarg_target';
	}
	elsif ($OP_mnemonic[$opcode] eq 'icall') {
	    $signature = 'lr:icall:lr';
	}
	elsif ($OP_mnemonic[$opcode] eq 'igoto') {
	    $signature = ':ijump:lr';
	}
	elsif ($OP_mnemonic[$opcode] eq 'rfi') {
	    $signature = ':rfi:';
	}
	else {
	    printf(STDOUT "ERROR: unknown call format %s in op_signature\n", 
	       $OP_mnemonic[$opcode]);
	    exit(1);
	}
    }
    elsif ($format eq 'SysOp') {
	$signature = ':SysOp:s9';
    }
    else {
	printf(STDOUT "ERROR: unknown format %s in op_signature\n", 
	       $format);
	exit(1);
    }

    return $signature;
}

# ==================================================================
#    op_print
#
#    make a printing signature similar to:
#
#          %R?_NAME_%R_,_%R_,_%R(O_R_O_O) for each TOP
# ==================================================================

sub op_print {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];
    my $signature;

    if ($format eq 'Int3R' ||
	$format eq 'Cmp3R_Reg' ||
	$format eq 'Cmp3R_Br') {
	$signature = 'NAME_%R_=_%R,_%R(NAME_R_O_O)';
    }
    elsif ($format eq 'Int3I' ||
	   $format eq 'Cmp3I_Reg' ||
	   $format eq 'Cmp3I_Br') {
	$signature = 'NAME_%R_=_%R,_%I(NAME_R_O_O)';
    }
    elsif ($format eq 'SelectR') {
	$signature = 'NAME_%R_=_%R,_%R,_%R(NAME_R_O_O_O)';
    }
    elsif ($format eq 'SelectI') {
	$signature = 'NAME_%R_=_%R,_%R,_%I(NAME_R_O_O_O)';
    }
    elsif ($format eq 'cgen') {
	$signature = 'NAME_%R,_%R_=_%R,_%R,_%R(NAME_R_R_O_O_O)';
    }
    elsif ($format eq 'Imm') {
	$signature = 'NAME_%I(NAME_O)';
    }
    elsif ($format eq 'Load') {
	$signature = 'NAME_%R_=_%I_[_%R_](NAME_R_O_O)';
    }
    elsif ($format eq 'Store') {
	$signature = 'NAME_%I_[_%R_]_=_%R(NAME_O_O_O)';
    }
    elsif ($format eq 'Branch') {
	$signature = 'NAME_%R,_%R(NAME_O_O)';
    }
    elsif ($format eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'call' ||
	    $OP_mnemonic[$opcode] eq 'goto') {
	    $signature = 'NAME_LR_=_%R(NAME_O)';
	}
	elsif ($OP_mnemonic[$opcode] eq 'icall') {
	    $signature = 'NAME_LR_=_LR(NAME)';
	}
	elsif ($OP_mnemonic[$opcode] eq 'igoto') {
	    $signature = 'NAME_LR(NAME)';
	}
	elsif ($OP_mnemonic[$opcode] eq 'rfi') {
	    $signature = 'NAME(NAME)';
	}
	else {
	    printf(STDOUT "ERROR: unknown call format %s in op_signature\n", 
	       $OP_mnemonic[$opcode]);
	    exit(1);
	}
    }
    elsif ($format eq 'SysOp') {
	$signature = 'NAME_%I(NAME_O)';
    }
    else {
	printf(STDOUT "ERROR: unknown format %s in op_signature\n", 
	       $format);
	exit(1);
    }

    return $signature;
}

# ==================================================================
#    initialize_required_opcodes
#
#    The Pro64 back end requires a number of Opcodes to be defined:
#
#    // Simulated instructions: these may be translated into
#    // a number of target instructions, and also may depend
#    // on an execution unit, eg. noop on IA64.
#      "asm",                    /* a ASM statement */
#      "intrncall",
#      "spadjust",               /* SP adjustment OP */
#      "copy.br",
#      "noop"
#
#    // Dummy instructions: these do not get emitted.
#      "begin.pregtn",
#      "end.pregtn",
#      "bwd.bar",
#      "fwd.bar",
#      "dfixup",
#      "ffixup",
#      "ifixup",
#      "label",
#
# ==================================================================

sub initialize_required_opcodes {

    @SimulatedOpcodes;
    @DummyOpcodes;

    my $subset;
    my $opcode;

    push (@SimulatedOpcodes, "asm");
    push (@SimulatedOpcodes, "intrncall");
    push (@SimulatedOpcodes, "spadjust");
    push (@SimulatedOpcodes, "copy_br");
    push (@SimulatedOpcodes, "noop");

    push (@DummyOpcodes, "begin_pregtn");
    push (@DummyOpcodes, "end_pregtn");
    push (@DummyOpcodes, "bwd_bar");
    push (@DummyOpcodes, "fwd_bar");
    push (@DummyOpcodes, "dfixup");
    push (@DummyOpcodes, "ffixup");
    push (@DummyOpcodes, "ifixup");
    push (@DummyOpcodes, "label");

    # Add to the OP_xxx[] tables:
    foreach $opcode (@SimulatedOpcodes) {
	$OP_opcode[$OP_count] = $opcode;
	$OP_properties[$OP_count] = $OP_SIMULATED;
	$OP_count++;
    }

    foreach $opcode (@DummyOpcodes) {
	$OP_opcode[$OP_count] = $opcode;
	$OP_properties[$OP_count] = $OP_DUMMY;
	$OP_count++;
    }

    #    add required opcodes to all isa subsets, so they are 
    #    also emitted by the emit_subsets.
    #
    #    TODO: now subset generating skripts do not allow one instruction
    #    to be part of more than 1 ISA subset. I need to allow this for 
    #    the simulated and dummy instructions.

    for ($subset = 0; $subset < $SUBSET_count; $subset++) {
	push (@{$SUBSET_opcodes[$subset]}, 'asm');
	push (@{$SUBSET_opcodes[$subset]}, 'intrncall');
	push (@{$SUBSET_opcodes[$subset]}, 'spadjust');
	push (@{$SUBSET_opcodes[$subset]}, 'copy_br');
	push (@{$SUBSET_opcodes[$subset]}, 'noop');

	push (@{$SUBSET_opcodes[$subset]}, 'begin_pregtn');
	push (@{$SUBSET_opcodes[$subset]}, 'end_pregtn');
	push (@{$SUBSET_opcodes[$subset]}, 'bwd_bar');
	push (@{$SUBSET_opcodes[$subset]}, 'fwd_bar');
	push (@{$SUBSET_opcodes[$subset]}, 'dfixup');
	push (@{$SUBSET_opcodes[$subset]}, 'ffixup');
	push (@{$SUBSET_opcodes[$subset]}, 'ifixup');
	push (@{$SUBSET_opcodes[$subset]}, 'label');
    }

    # isa_properties: add required opcodes to AttrGroup, so they are 
    # also emitted by the emit_properties.

    # simulated:
    push(@{$AttrGroup{'simulated'}}, 'asm');
    push(@{$AttrGroup{'simulated'}}, 'intrncall');
    push(@{$AttrGroup{'simulated'}}, 'spadjust');
    push(@{$AttrGroup{'simulated'}}, 'copy_br');
    push(@{$AttrGroup{'simulated'}}, 'noop');

    # dummy:
    push(@{$AttrGroup{'dummy'}}, 'begin_pregtn');
    push(@{$AttrGroup{'dummy'}}, 'end_pregtn');
    push(@{$AttrGroup{'dummy'}}, 'bwd_bar');
    push(@{$AttrGroup{'dummy'}}, 'fwd_bar');
    push(@{$AttrGroup{'dummy'}}, 'dfixup');
    push(@{$AttrGroup{'dummy'}}, 'ffixup');
    push(@{$AttrGroup{'dummy'}}, 'ifixup');
    push(@{$AttrGroup{'dummy'}}, 'label');

    # var_operands:
    push(@{$AttrGroup{'var_opnds'}}, 'asm');
    push(@{$AttrGroup{'var_opnds'}}, 'intrncall');

    push(@{$AttrGroup{'noop'}}, 'noop');

    # isa_operands: Create SignatureGroup entry for required opcodes.
    # TODO: generalize so that target-dependent types are not
    #       hardwired here !

    push(@{$SignatureGroup{':asm:'}}, 'asm');
    push(@{$SignatureGroup{':intrncall:'}}, 'intrncall');

    push(@{$SignatureGroup{'idest:spadjust:src1,isrc2'}}, 'spadjust');
    push(@{$SignatureGroup{'bdest:copy_br:scond'}}, 'copy_br');
    push(@{$SignatureGroup{':noop:'}}, 'noop');

    push(@{$SignatureGroup{':pregtn:src1,isrc2'}}, 'begin_pregtn');
    push(@{$SignatureGroup{':pregtn:src1,isrc2'}}, 'end_pregtn');
    push(@{$SignatureGroup{':barrier:'}}, 'bwd_bar');
    push(@{$SignatureGroup{':barrier:'}}, 'fwd_bar');
    push(@{$SignatureGroup{'dest:fixup:'}}, 'dfixup');
    push(@{$SignatureGroup{'dest:fixup:'}}, 'ffixup');
    push(@{$SignatureGroup{'dest:fixup:'}}, 'ifixup');
    push(@{$SignatureGroup{':label:pcrel'}}, 'label');

}

# ==================================================================
#    initialize_isa_file
# ==================================================================

sub initialize_isa_file {

    open (ISA_F, "> isa.cxx");

    &copyright_notice(ISA_F);

    print ISA_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n";
    print ISA_F "// Generate an ISA containing the instructions specified. \n";
    print ISA_F "///////////////////////////////////////////////////////// \n";
    print ISA_F "// The instructions are listed by category. The different \n";
    print ISA_F "// categories of instructions are specified in the ISA manual:\n";
    print ISA_F "// Within each category, the instructions are in alphabetical order.\n";
    print ISA_F "// This arrangement of instructions matches the order in the ISA manual.\n";
    print ISA_F "/////////////////////////////////////\n\n\n";
    print ISA_F "#include <stddef.h>\n";
    print ISA_F "#include \"isa_gen.h\"\n\n\n";

    print ISA_F "main ()\n";
    print ISA_F "{\n";
    printf (ISA_F "  ISA_Create (\"%s\", \n", $ARCH_name);

    return;
}

# ==================================================================
#    emit_opcode
#
#    Write opcodes into file isa.cxx
# ==================================================================

sub emit_opcode {
    my $opcode = $_[0];

    printf (ISA_F "\t\t \"%s\",", $OP_opcode[$opcode]);
    if (opcode_is_simulated($OP_opcode[$opcode]) == 1) {
	printf(ISA_F "\t // simulated");
    }
    if (opcode_is_dummy($OP_opcode[$opcode]) == 1) {
	printf(ISA_F "\t // dummy");
    }
    printf(ISA_F "\n");
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
#    initialize_reg_file
# ==================================================================

sub initialize_reg_file {

    open (REG_F, "> isa_registers.cxx");

    &copyright_notice(REG_F);

    printf (REG_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n");
    printf (REG_F "//  \n");
    printf (REG_F "//  Generate ISA registers information \n");
    printf (REG_F "/////////////////////////////////////// \n");
    printf (REG_F "\n");

    printf (REG_F "#include <stddef.h>\n");
    printf (REG_F "#include \"isa_registers_gen.h\" \n");
    printf (REG_F "#include \"targ_isa_subset.h\"\n\n\n");

    printf (REG_F "static int ISA_Mask(ISA_SUBSET isa) \n");
    printf (REG_F "{ \n");
    printf (REG_F "  return 1 << (int)isa; \n");
    printf (REG_F "} \n");
    printf (REG_F "\n");

    printf (REG_F "static int All_ISA_Mask(void) \n");
    printf (REG_F "{ \n");
    printf (REG_F "  int i; \n");
    printf (REG_F "  int mask = 0; \n");
    printf (REG_F "  for (i = ISA_SUBSET_MIN; i <= ISA_SUBSET_MAX; ++i) { \n");
    printf (REG_F "    mask |= 1 << i; \n");
    printf (REG_F "  } \n");
    printf (REG_F "  return mask; \n");
    printf (REG_F "} \n");
    printf (REG_F "\n");

    printf (REG_F "static int Range_ISA_Mask(ISA_SUBSET min_isa, ISA_SUBSET max_isa) \n");
    printf (REG_F "{ \n");
    printf (REG_F "  int i; \n");
    printf (REG_F "  int mask = 0; \n");
    printf (REG_F "  for (i = (int)min_isa; i <= (int)max_isa; ++i) { \n");
    printf (REG_F "    mask |= 1 << i; \n");
    printf (REG_F "  } \n");
    printf (REG_F "  return mask; \n");
    printf (REG_F "} \n");
    printf (REG_F "\n");

    return;
}

# ==================================================================
#    emit_registers
# ==================================================================

sub emit_registers {
    my $i;

    for ($i = 0; $i < $REGSET_count; $i++) {
	my $reg;
	printf(REG_F "static const char *%s_reg_names[] = {\n", 
	       $REGSET_rclass[$i]);
	for ($reg = $REGSET_minreg[$i]; 
	     $reg < $REGSET_maxreg[$i];
	     $reg++) {
	    if (defined($REGSET_names[$i][$reg])) {
		printf(REG_F "\t\"%s\",", $REGSET_names[$i][$reg]);
	    }
	    else {
		printf(REG_F "\tNULL,");
	    }
	    if ($reg != 0 && $reg % 4 == 0) {
		printf(REG_F "\n");
	    }
	}
	if (defined($REGSET_names[$i][$REGSET_maxreg[$i]])) {
	    printf(REG_F "\t\"%s\"\n", $REGSET_names[$i][$REGSET_maxreg[$i]]);
	}
	else {
	    printf(REG_F "\tNULL\n");
	}
	printf(REG_F "};\n");
	printf(REG_F "\n");

	# subsets of this set:
	my $subset;
	for ($subset = 0; $subset < $REGSET_subset_count[$i]; $subset++) {
	    my $reg;
	    my $found;

	    # Emit members table:
	    printf(REG_F "static const int %s_%s[] = {", 
		   $REGSET_rclass[$i], $REGSET_subset[$i][$subset]{'name'});
	    $found = 0;
	    for ($reg = $REGSET_minreg[$i];
		 $reg <= $REGSET_maxreg[$i];
		 $reg++) {

		if ($REGSET_subset[$i][$subset]{'regs'}[$reg] == 1) {
		    if ($found) {
			printf(REG_F ",%d", $reg);
		    }
		    else {
			printf(REG_F "%d", $reg);
		    }
		    $found = 1;
		}
	    }
	    printf(REG_F "}; \n");
	}
	printf(REG_F "\n");

    }

    # Emit a usefull macro:
    printf(REG_F "#define NELEMS(a) (sizeof(a) / sizeof(*(a))) \n");
    printf(REG_F "\n");

    printf(REG_F "main() \n");
    printf(REG_F "{ \n");
    printf(REG_F "  ISA_REGISTER_CLASS \n");
    for ($i = 0; $i < $RCLASS_count-1; $i++) {
	printf(REG_F "\t rc_%s,\n", $RCLASS_name[$i]);
    }
    printf(REG_F "\t rc_%s;\n", $RCLASS_name[$i]);
    printf(REG_F "\n");

    printf(REG_F "  ISA_Registers_Begin(\"%s\"); \n", $ARCH_name);
    printf(REG_F "\n");

    for ($i = 0; $i < $RCLASS_count; $i++) {
	my $can_be_stored = ($RCLASS_can_be_stored[$i]) ? "true" : "false";
	my $mult_store = ($RCLASS_mult_store[$i]) ? "true" : "false";
	my $is_ptr = ($RCLASS_is_ptr[$i]) ? "true" : "false";
	printf(REG_F "  rc_%s = ISA_Register_Class_Create(\"%s\", %d, %s, %s, %s); \n", $RCLASS_name[$i], $RCLASS_name[$i], $RCLASS_bits[$i], $is_ptr, $can_be_stored, $mult_store);
    }
    printf(REG_F "\n");

    # Emit register sets:
    for ($i = 0; $i < $REGSET_count; $i++) {
	my $reg;
	printf(REG_F "  ISA_Register_Set(rc_%s, %d, %d, \"%s\", NULL, All_ISA_Mask()); \n", $REGSET_rclass[$i], $REGSET_minreg[$i], $REGSET_maxreg[$i], $REGSET_format[$i]);
	# subsets of this set:
	my $subset;
	for ($subset = 0; $subset < $REGSET_subset_count[$i]; $subset++) {
	    printf(REG_F "  ISA_Register_Subclass_Create(\"%s\", rc_%s,
			        NELEMS(%s_%s), %s_%s, NULL); \n", 
		   $REGSET_subset[$i][$subset]{'name'}, $REGSET_rclass[$i],
		   $REGSET_rclass[$i], $REGSET_subset[$i][$subset]{'name'},
		   $REGSET_rclass[$i], $REGSET_subset[$i][$subset]{'name'});
	}
	printf(REG_F "\n");
    }

    return;
}

# ==================================================================
#    finalize_reg_file
# ==================================================================

sub finalize_reg_file {

    printf (REG_F "  ISA_Registers_End(); \n");
    printf (REG_F "  return 0; \n");
    printf (REG_F "} \n");

    close (REG_F);
    return;
}

# ==================================================================
#    initialize_lit_file
# ==================================================================

sub initialize_lit_file {

    open (LIT_F, "> isa_lits.cxx");

    &copyright_notice(LIT_F);

    printf (LIT_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n");
    printf (LIT_F "\n");
    printf (LIT_F "// \n");
    printf (LIT_F "// Generate a list of literal classes and values for the ISA. \n");
    printf (LIT_F "///////////////////////////////////////////////////////// \n");
    printf (LIT_F "\n");

    printf (LIT_F "#include <stddef.h> \n");
    printf (LIT_F "#include \"isa_lits_gen.h\" \n");
    printf (LIT_F "\n");

    printf (LIT_F "main () \n");
    printf (LIT_F "{ \n");
    printf (LIT_F "  ISA_Lits_Begin(); \n");
    printf (LIT_F "\n");

    printf (LIT_F "  // ISA_Create_Lit_Class(name, type, [range,...] LIT_RANGE_END) \n");
    printf (LIT_F "\n");

    return;
}

# ==================================================================
#    emit_literals
# ==================================================================

sub emit_literals {
    my $lclass;

    for ($lclass = 0; $lclass < $LCLASS_count; $lclass++) {
	my $signed = ($LCLASS_rtype[$lclass] eq 'SIGNED') ? "SIGNED" : "UNSIGNED";
	printf(LIT_F "  ISA_Create_Lit_Class(\"%s\", %s, SignedBitRange(%d), LIT_RANGE_END); \n", $LCLASS_name[$lclass], $signed, $LCLASS_bits[$lclass]);
    }
    printf (LIT_F "\n");

    return;
}

# ==================================================================
#    finalize_lit_file
# ==================================================================

sub finalize_lit_file {

    printf (LIT_F "  ISA_Lits_End(); \n");
    printf (LIT_F "  return 0; \n");
    printf (LIT_F "} \n");

    close (LIT_F);
    return;
}

# ==================================================================
#    initialize_enum_file
# ==================================================================

sub initialize_enum_file {

    open (ENUM_F, "> isa_enums.cxx");

    &copyright_notice(ENUM_F);

    printf (ENUM_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n");
    printf (ENUM_F "\n");

    printf (ENUM_F "// \n");
    printf (ENUM_F "// Generate a list of enumeration classes and values for the ISA. \n");
    printf (ENUM_F "///////////////////////////////////////////////////////// \n");
    printf (ENUM_F "\n");

    printf (ENUM_F "#include <stddef.h> \n");
    printf (ENUM_F "#include \"isa_enums_gen.h\" \n");
    printf (ENUM_F "\n");

    printf (ENUM_F "main () \n");
    printf (ENUM_F "{ \n");
    printf (ENUM_F "  ISA_Enums_Begin(); \n");
    printf (ENUM_F "\n");

    return;
}

# ==================================================================
#    emit_enums
# ==================================================================

sub emit_enums {
    my $eclass;

    for ($eclass = 0; $eclass < $ECLASS_count; $eclass++) {
	printf(ENUM_F "  ISA_Create_Enum_Class(\"%s\", \n", 
	       $ECLASS_name[$eclass]);
	my $elt;
	for ($elt = 0; $elt < $ECLASS_elt_count[$eclass]; $elt++) {
	    printf(ENUM_F "\t \"%s\", \t %d,\n",
		   $ECLASS_elt[$eclass][$elt]{'name'},
		   $ECLASS_elt[$eclass][$elt]{'val'});
	}

	printf(ENUM_F "\t NULL, \t -1); \n");
	printf(ENUM_F "\n");
    }
    printf (ENUM_F "\n");

    return;
}

# ==================================================================
#    finalize_enum_file
# ==================================================================

sub finalize_enum_file {

    printf (ENUM_F "  ISA_Enums_End(); \n");
    printf (ENUM_F "  return 0; \n");
    printf (ENUM_F "} \n");

    close (ENUM_F);
    return;
}

# ==================================================================
#    initialize_subset_file
# ==================================================================

sub initialize_subset_file {
    my $i;

    open (SUBS_F, "> isa_subset.cxx");

    &copyright_notice(SUBS_F);

    print SUBS_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! \n";
    print SUBS_F "//  Generate ISA subset descriptions \n";
    print SUBS_F "///////////////////////////////////// \n";

    print SUBS_F "#include <stddef.h> \n";
    print SUBS_F "#include \"topcode.h\" \n";
    print SUBS_F "#include \"isa_subset_gen.h\" \n\n";

    print SUBS_F "main() \n";
    print SUBS_F "{ \n";

    for ($i = 0; $i < $SUBSET_count; $i++) {
	my $subset = $SUBSET_name[$i];
	printf (SUBS_F "  ISA_SUBSET %s; \n", $subset);
    }
    printf (SUBS_F "\n\n");
    printf (SUBS_F "  ISA_Subset_Begin(\"%s\"); \n",$ARCH_name);


    return;
}

# ==================================================================
#    emit_subsets
# ==================================================================

sub emit_subsets {
    my $opcode;
    my $i;

    for ($i = 0; $i < $SUBSET_count; $i++) {
	my $subset = $SUBSET_name[$i];
	printf (SUBS_F "  %s = ISA_Subset_Create(NULL,\"%s\"); \n\n", 
		$subset, $subset);
	printf (SUBS_F "  /* ==================================================================== \n");
	printf (SUBS_F "   *             %s_%s Instructions \n", 
		$ARCH_name, $subset);
	printf (SUBS_F "   * ==================================================================== \n");
	printf (SUBS_F "   */ \n");

        printf (SUBS_F "  Instruction_Group(%s, \n", $subset);
	foreach $opcode (@{$SUBSET_opcodes[$i]}) {
	    printf (SUBS_F "\t\t TOP_%s, \n", $opcode);
	}
	printf (SUBS_F "\t\t TOP_UNDEFINED); \n\n");
    }

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
#    initialize_properties_file
# ==================================================================

sub initialize_properties_file {
    my $prop;

    open (PROP_F, "> isa_properties.cxx");

    &copyright_notice(PROP_F);

    print PROP_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!!\n\n";
    print PROP_F "//  Generate ISA properties information \n";
    print PROP_F "/////////////////////////////////////// \n";

    print PROP_F "#include <stddef.h> \n";
    print PROP_F "#include \"topcode.h\" \n";
    print PROP_F "#include \"isa_properties_gen.h\" \n\n";

    print PROP_F "main() \n";
    print PROP_F "{ \n";
    print PROP_F "  ISA_PROPERTY \n";
    for ($prop = 0; $prop < $ISA_PROPERTY_count-1; $prop++) {
	printf (PROP_F "    %s,\n", $ISA_PROPERTY_name[$prop]);
    }
    printf (PROP_F "    %s;\n", $ISA_PROPERTY_name[$ISA_PROPERTY_count-1]);
    printf (PROP_F "\n");

    printf (PROP_F "  ISA_Properties_Begin (\"%s\"); \n\n\n",$ARCH_name);

    print PROP_F "  /* ==================================================================== \n";
    print PROP_F "   *              Operator attributes descriptors \n";
    print PROP_F "   * ==================================================================== \n";
    print PROP_F "   */ \n\n";

    return;
}

# ==================================================================
#    sort_by_properties
# ==================================================================

sub sort_by_properties {
    my $opcode = $_[0];

    if ($OP_format[$opcode] eq 'Load') {
	$OP_properties[$opcode] ^= $OP_LOAD;
	$OP_bytes[$opcode] = &op_bytes($opcode);
	$OP_align[$opcode] = &op_align($opcode);
	if ($OP_mnemonic[$opcode] eq 'ldw_d' ||
	    $OP_mnemonic[$opcode] eq 'ldh_d' ||
	    $OP_mnemonic[$opcode] eq 'ldhu_d' ||
	    $OP_mnemonic[$opcode] eq 'ldb_d' ||
	    $OP_mnemonic[$opcode] eq 'ldbu_d') {
	    $OP_properties[$opcode] ^= $OP_DISMISSIBLE;
	}
    }

    if ($OP_format[$opcode] eq 'Store') {
	$OP_properties[$opcode] ^= $OP_STORE;
	$OP_bytes[$opcode] = &op_bytes($opcode);
	$OP_align[$opcode] = &op_align($opcode);
    }

    if ($OP_format[$opcode] eq 'Branch') {
	if ($OP_mnemonic[$opcode] eq 'br' ||
	    $OP_mnemonic[$opcode] eq 'brf') {
	    $OP_properties[$opcode] ^= $OP_BRANCH;
	}
    }

    if ($OP_format[$opcode] eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'goto') {
	    $OP_properties[$opcode] ^= $OP_JUMP;
	}
	elsif ($OP_mnemonic[$opcode] eq 'igoto') {
	    $OP_properties[$opcode] ^= $OP_IJUMP;
	}
	else {
	    $OP_properties[$opcode] ^= $OP_CALL;
	}
    }

    return;
}

# ==================================================================
#    emit_properties
# ==================================================================

sub emit_properties {
    my $group;
    my $opcode;

    foreach $group (keys(%AttrGroup)) {
	printf (PROP_F "  /* ====================================== */ \n");
        printf (PROP_F "  %s = ISA_Property_Create (\"%s\"); \n",
		$group, $group);
	printf (PROP_F "  Instruction_Group (%s, \n", $group);
	foreach $opcode (@{$AttrGroup{$group}}) {
	    printf (PROP_F "\t\t TOP_%s, \n", $opcode);
	}
	printf (PROP_F "\t\t TOP_UNDEFINED); \n\n");
    }

    foreach $group (keys(%MemBytes)) {
	printf (PROP_F "  /* ====================================== */ \n");
	printf (PROP_F "  /*         Memory Access Size %s          */ \n",
                                                                    $group);
	printf (PROP_F "  /* ====================================== */ \n");
	printf (PROP_F "  ISA_Memory_Access (%s, \n", $group);
	foreach $opcode (@{$MemBytes{$group}}) {
	    printf (PROP_F "\t\t TOP_%s, \n", $opcode);
	}
	printf (PROP_F "\t\t TOP_UNDEFINED); \n\n");
    }

    foreach $group (keys(%MemAlign)) {
	printf (PROP_F "  /* ====================================== */ \n");
	printf (PROP_F "  /*          Memory Alignment %s           */ \n",
                                                                    $group);
	printf (PROP_F "  /* ====================================== */ \n");
	printf (PROP_F "  ISA_Memory_Alignment (%s, \n", $group);
	foreach $opcode (@{$MemAlign{$group}}) {
	    printf (PROP_F "\t\t TOP_%s, \n", $opcode);
	}
	printf (PROP_F "\t\t TOP_UNDEFINED); \n\n");
    }

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
#    initialize_operands_file
# ==================================================================

sub initialize_operands_file {
    my $i;

    open (OPND_F, "> isa_operands.cxx");

    print OPND_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! \n";
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

    # Specify operand value types:
    if ($REG_OPND_count > 0) {
	printf (OPND_F "  OPERAND_VALUE_TYPE ");
	for ($i = 0; $i < $REG_OPND_count-1; $i++) {
	    printf (OPND_F "%s, ", $REG_OPND_name[$i]);
	}
	printf (OPND_F "%s; \n", $REG_OPND_name[$REG_OPND_count-1]);
    }
    if ($LIT_OPND_count > 0) {
	printf (OPND_F "  OPERAND_VALUE_TYPE ");
	for ($i = 0; $i < $LIT_OPND_count-1; $i++) {
	    printf (OPND_F "%s, ", $LIT_OPND_name[$i]);
	}
	printf (OPND_F "%s; \n", $LIT_OPND_name[$LIT_OPND_count-1]);
    }
    if ($ENUM_OPND_count > 0) {
	printf (OPND_F "  OPERAND_VALUE_TYPE ");
	for ($i = 0; $i < $ENUM_OPND_count-1; $i++) {
	    printf (OPND_F "%s, ", $ENUM_OPND_name[$i]);
	}
	printf (OPND_F "%s; \n", $ENUM_OPND_name[$ENUM_OPND_count-1]);
    }
    print OPND_F "\n";

    # Specify operand use type:
    printf (OPND_F "  /* ------------------------------------------------------\n");
    printf (OPND_F "   *   Following built-in use types must be specified: \n");
    printf (OPND_F "   *     1. base operand use for TOP_load, TOP_store; \n");
    printf (OPND_F "   *     2. offset operand use for TOP_load, TOP_store; \n");
    printf (OPND_F "   *     3. storeval operand use for TOP_store; \n");
    printf (OPND_F "   * \n");
    printf (OPND_F "   *   Following built-in use types may be specified: \n");
    printf (OPND_F "   *     4. implicit operand use for TOPs when the operand is implicitely used; \n");
    printf (OPND_F "   * \n");
    printf (OPND_F "   *   Here you can specify any additional operand uses. \n");
    printf (OPND_F "   * ------------------------------------------------------\n");
    printf (OPND_F "   */\n");

    if ($OPND_USE_count > 0) {
	printf (OPND_F "  OPERAND_USE_TYPE \n");
	for ($i = 0; $i < $OPND_USE_count-1; $i++) {
	    printf (OPND_F "        %s, \n", $OPND_USE_name[$i]);
	}
	printf (OPND_F "        %s; \n", $OPND_USE_name[$OPND_USE_count-1]);
    }
    printf (OPND_F "\n");

    printf (OPND_F "  ISA_Operands_Begin(\"%s\"); \n",$ARCH_name);

    if ($REG_OPND_count > 0) {
	printf (OPND_F "  /* Create the register operand types: */ \n");
	for ($i = 0; $i < $REG_OPND_count; $i++) {
	    printf (OPND_F "  %s = ISA_Reg_Opnd_Type_Create(\"%s\", \n",
		    $REG_OPND_name[$i], $REG_OPND_name[$i]);
            printf (OPND_F "                ISA_REGISTER_CLASS_%s, \n",
		    $REG_OPND_rclass[$i]);
	    printf (OPND_F "                ISA_REGISTER_SUBCLASS_%s, \n",
		    $REG_OPND_subclass[$i]);
	    printf (OPND_F "                %d, %s, %s); \n",
		    $REG_OPND_size[$i], $REG_OPND_rtype[$i], $REG_OPND_fp_type[$i]);
	}
	printf (OPND_F "\n");
    }
    if ($LIT_OPND_count > 0) {
	printf (OPND_F "  /* Create the literal operand types: */ \n");
	for ($i = 0; $i < $LIT_OPND_count; $i++) {
	    printf (OPND_F "  %s = ISA_Lit_Opnd_Type_Create(\"%s\", %d, %s, LC_%s); \n",
		    $LIT_OPND_name[$i], $LIT_OPND_name[$i], 
		    $LIT_OPND_size[$i],$LIT_OPND_rtype[$i], 
		    $LIT_OPND_lclass[$i]);
	}
	printf (OPND_F "\n");
    }

    if ($ENUM_OPND_count > 0) {
	printf (OPND_F "  /* Create the enum operand types: */ \n");
	for ($i = 0; $i < $ENUM_OPND_count; $i++) {
	    printf (OPND_F "  %s = ISA_Enum_Opnd_Type_Create(\"%s\", %d, %s, EC_%s); \n",
		    $ENUM_OPND_name[$i], $ENUM_OPND_name[$i], $ENUM_OPND_size[$i],$ENUM_OPND_rtype[$i], $ENUM_OPND_eclass[$i]);
	}
	printf (OPND_F "\n");
    }
    printf (OPND_F "\n");

    if ($OPND_USE_count > 0) {
	printf (OPND_F "  /* Create the operand uses: */ \n");
	printf (OPND_F "\n");
	for ($i = 0; $i < $OPND_USE_count; $i++) {
	    printf (OPND_F "  %s = Create_Operand_Use(\"%s\"); \n",
		    $OPND_USE_name[$i], $OPND_USE_name[$i]);
	}
	printf (OPND_F "\n");
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

    foreach $signature (keys(%SignatureGroup)) {
	($rests, $gname, $opnds) = split(":", $signature);
	print OPND_F "  /* ====================================== */ \n";
	if ($gname eq '*') {
	    print OPND_F "  Instruction_Group(\"O_$group\", \n";
	    $group++;
	}
	else {
	    print OPND_F "  Instruction_Group(\"O_$gname\", \n";
	}
	foreach $opcode (@{$SignatureGroup{$signature}}) {
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
		    if ($attribute =~ /same(\d*)/) {
			printf (OPND_F "  Result (%d, %s); \n", $count, $name);
			printf (OPND_F "  Same_Res (%d); \n", $1);
		    }
		    else {
			print OPND_F "  Result ($count, $name, $attribute); \n";
		    }
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

	printf(OPND_F "\n");
    }

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

    print PRNT_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! \n";
    print PRNT_F "// Group TOPS with similar printing format. \n";
    print PRNT_F "//////////////////////////////////////////////////\n\n\n";

    print PRNT_F "#include <stdio.h> \n";
    print PRNT_F "#include <stddef.h> \n";
    print PRNT_F "#include <string.h> \n";
    print PRNT_F "#include <ctype.h> \n";
    print PRNT_F "#include \"topcode.h\" \n";
    print PRNT_F "#include \"isa_print_gen.h\" \n\n";

    return;
}

# ==================================================================
#    emit_printing_formats
# ==================================================================

sub emit_printing_formats {

    print PRNT_F "// Multiple topcodes map to the same assembly name. To disambiguate the \n";
    print PRNT_F "// topcodes, we append a signature to the basename. To get the assembly \n";
    print PRNT_F "// name we must strip off the suffix. \n\n";

    print PRNT_F "static const char *asmname(TOP topcode) \n";
    print PRNT_F "{ \n";
    print PRNT_F "  int c; \n";
    print PRNT_F "  int i; \n";
    print PRNT_F "  const char *name = TOP_Name(topcode); \n";
    print PRNT_F "  char buf[100]; \n";

    print PRNT_F "  /* \n";
    print PRNT_F "   * First handle simulated and dummy instructions: \n";
    print PRNT_F "   */ \n";
    print PRNT_F "  switch (topcode) { \n";

    my $opcode;
    foreach $opcode (@SimulatedOpcodes) {
	printf (PRNT_F "  case TOP_%s: return \"%s\"; \n", $opcode, $opcode);
    }

    foreach $opcode (@DummyOpcodes) {
	printf (PRNT_F "  case TOP_%s: return \"%s\"; \n", $opcode, $opcode);
    }

    print PRNT_F "  default: break; \n";
    print PRNT_F "  } \n";
    print PRNT_F "\n";

    print PRNT_F "  for (i = 0; c != '\\0'; ++i) { \n";
    print PRNT_F "    c = name[i]; \n";
    print PRNT_F "    if (c == '_') { \n";
    print PRNT_F "      // if next is _i or _r, get out, else it's a _d \n";
    print PRNT_F "      if (name[i+1] == 'd') { \n";
    print PRNT_F "	buf[i] = '.'; \n";
    print PRNT_F "	buf[i+1] = 'd'; \n";
    print PRNT_F "        buf[i+3] = '\\0'; \n";
    print PRNT_F "      } \n";
    print PRNT_F "      else { \n";
    print PRNT_F "	buf[i] = '\\0'; \n";
    print PRNT_F "      } \n";
    print PRNT_F "      break; \n";
    print PRNT_F "    } \n";
    print PRNT_F "    buf[i] = c; \n";
    print PRNT_F "  } \n\n";

    print PRNT_F "  return strdup(buf); \n";
    print PRNT_F "} \n\n";

    print PRNT_F "main() \n";
    print PRNT_F "{ \n";

    printf (PRNT_F "  ISA_Print_Begin(\"%s\"); \n\n",$ARCH_name);
    printf (PRNT_F "  Set_AsmName_Func(asmname); \n\n");

    # First specify bundle/grouping stuff
    printf(PRNT_F "  Define_Macro(\"END_GROUP\", \";;\");\t// end-of-group marker \n");
    printf(PRNT_F "  Define_Macro(\"BEGIN_BUNDLE\", \"## bundle %s:\");\t// bundle introducer \n", "%s");
    printf(PRNT_F "  Define_Macro(\"END_BUNDLE\", \";;\");\t// bundle terminator \n");
    printf(PRNT_F "\n");

    my $count = 0;
    foreach my $group (keys(%PrintGroup)) {
	$group =~ /(.*)\((\w*)\)/;
	my $ft = $1;
	my $pt = $2;
	my @format = split ('_',$ft);
	my @pattern = split ('_',$pt);
	my @new_format;
	my @new_pattern;
	my @args;
	my $opcode;

	foreach my $elt (@format) {

	    if ($elt eq "%R?") {
		push (@new_format, "%s");
		push (@args, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "%R!") {
		push (@new_format, "%s");
		push (@args, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif (($elt eq "%R") ||
		   ($elt eq "%S") ||
		   ($elt eq "%M") ) {
		push (@new_format, "%s");
		push (@args, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "%I") {
		push (@new_format, "%s");
		push (@args, "%d");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "%R,") {
		push (@new_format, "%s,");
		push (@args, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq ",%I") {
		push (@new_format, ", %s");
		push (@args, "%d");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "\\\\%pr5") {
		push (@new_format, "%s");
		push (@args, "%s");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt eq "0x%05x") {
		push (@new_format, "%s");
		push (@args, "0x%05x");
		push (@new_pattern, shift (@pattern));
	    }
	    elsif ($elt =~ /\%/) {
		print "ERROR: unknown print format $elt !!!\n";
		exit (1);
	    }
	    elsif ($elt eq "NAME") {
		push (@new_format, "\t%s");
		push (@args, "%s");
		push (@new_pattern, "NAME");
		shift (@pattern);
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
	print PRNT_F "  print_$count = ISA_Print_Type_Create(\"print_$count\", \"c0$new_ft\"); \n";

	my $opnd = 0;
	my $res = 0;
	foreach my $sym (@new_pattern) {
	    if ($sym eq "O") {
		printf (PRNT_F "  Operand(%d, \"%s\"); \n", $opnd, shift (@args));
		$opnd++;
	    }
	    elsif ($sym eq "R") {
		printf (PRNT_F "  Result(%d, \"%s\"); \n", $res, shift (@args));
		$res++;
	    }
	    else {
		printf (PRNT_F "  Name(\"%s\"); \n", shift (@args));
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
#    initialize_pack_file
# ==================================================================

sub initialize_pack_file {
    open (PACK_F, "> isa_pack.cxx");
    &copyright_notice (PACK_F);

    printf (PACK_F "// \n");
    printf (PACK_F "// Group TOPs with similar packing format together.  \n");
    printf (PACK_F "///////////////////////////////////////////////////////// \n");

    printf (PACK_F "// Instructions may be packed (compressed) in binary files. The packing rules \n");
    printf (PACK_F "// are specified in this file. \n\n");

    printf (PACK_F "#include <stddef.h> \n");
    printf (PACK_F "#include \"topcode.h\" \n");
    printf (PACK_F "#include \"isa_pack_gen.h\" \n\n");
 
    printf (PACK_F "main() \n");
    printf (PACK_F "{ \n");
}

# ==================================================================
#    emit_pack_info
# ==================================================================

sub emit_pack_info {
    my $group;
    my $count;

    # for now I use the same groups as operand/result description.
    # placement of operands/results is completely fake for now.
    # TODO: should reflect things like operand scaling, etc.

    printf (PACK_F "  ISA_PACK_TYPE \n\t\t");

    # for now reuse operand/result groups:
    $count = 0;
    foreach $group (keys(%SignatureGroup)) {
	my $rests;
	my $gname;
	my $opnds;

	($rests, $gname, $opnds) = split(":", $group);

	printf (PACK_F "p%d, \t// %s\n\t\t", $count, $gname);
	$count++;
    }
    printf (PACK_F "p%d; \t// %s\n\n", $count, $gname);

    # This is just a copy of what I found in the ia64/isa_pack.cxx
    printf (PACK_F "  OPND_ADJ_TYPE	no_adj; \n\n");

    # An instruction is 32-bit wide on the ST100
    printf (PACK_F "  ISA_Pack_Begin(\"%s\", 32); \n\n", $ARCH_name);

    printf (PACK_F "  /* Create the various adjustments that need to be performed between \n");
    printf (PACK_F "   * assembly language form and packed form. Note that simple shift  \n");
    printf (PACK_F "   * adjustments have been accomplished directly in the operand packing \n");
    printf (PACK_F "   * specification. \n");
    printf (PACK_F "   */ \n\n");
    printf (PACK_F "  no_adj = Create_Operand_Adjustment(\"no adjustment\", \"O_VAL\"); \n\n");

    $count = 0;
    foreach $group (keys(%SignatureGroup)) {
	my $rests;
	my $gname;
	my $opnds;
	my $opcode;

	($rests, $gname, $opnds) = split(":", $group);

	printf (PACK_F "/* =====  p%d: ===== */ \n", $count);
	printf (PACK_F "  p%d = ISA_Pack_Type_Create(\"p%d\"); \n", 
		                                         $count, $count);

	my @results = split(",", $rests);
	my @operands = split(",", $opnds);
	my $bpos = 0;

	my $result;
	my $rcount = 0;
	foreach $result (@results) {
	    my @attributes = split("_", $result);
	    my $name = shift(@attributes);
	    my $attribute;
	    if (scalar(@attributes) == 0) {
		print PACK_F "  Result($rcount, $bpos, 5); \n";
	    }
	    else {
		foreach $attribute (@attributes) {
		    print PACK_F "  Result ($rcount, $bpos, 5); \n";
		}
	    }
	    $rcount++;
	    $bpos = $rcount * 5 + 1;
	}

	my $operand;
	my $ocount = 0;
	foreach $operand (@operands) {
	    my @attributes = split("_", $operand);
	    my $name = shift(@attributes);
	    my $attribute;
	    if (scalar(@attributes) == 0) {
		print PACK_F "  Operand ($ocount, 0, $bpos, 5); \n";
	    }
	    else {
		foreach $attribute (@attributes) {
		    if ($attribute eq "predicate") {
			print PACK_F "  Operand ($ocount, 0, $bpos, 1); \n";
			$bpos += 1;
		    }
		    else {
			print PACK_F "  Operand ($ocount, 0, $bpos, 5); \n";
			$bpos += 5;
		    }
		}
	    }
	    $ocount++;
	}

	printf (PACK_F "  Instruction_Pack_Group(p%d, \n", $count);
	my $opcode;
	foreach $opcode (@{$SignatureGroup{$group}}) {
	    # do not emit for simulated/dummy opcodes
	    if (opcode_is_simulated($opcode) == 0 &&
		opcode_is_dummy($opcode) == 0) {
		printf (PACK_F "\tTOP_%s, \t 0x10000000UL, \n",
		                                          $opcode);
	    }
	}
	print PACK_F "\tTOP_UNDEFINED); \n\n";

	$count++;
    }
}

# ==================================================================
#    finalize_pack_file
# ==================================================================

sub finalize_pack_file {

    printf (PACK_F "  ISA_Pack_End(); \n");
    printf (PACK_F "  return 0; \n");
    printf (PACK_F "} \n");

    close (PACK_F);
    return;
}

# ==================================================================
#    initialize_decode_file
# ==================================================================

sub initialize_decode_file {
    open (DECODE_F, "> isa_decode.cxx");
    &copyright_notice (DECODE_F);

    printf (DECODE_F "// \n");
    printf (DECODE_F "// Generate instruction decoding information. \n");
    printf (DECODE_F "///////////////////////////////////// \n");
    printf (DECODE_F "///////////////////////////////////// \n\n");

    printf (DECODE_F "#include \"topcode.h\" \n");
    printf (DECODE_F "#include \"isa_decode_gen.h\" \n");
    printf (DECODE_F "#include \"targ_isa_bundle.h\" \n\n");
 
    printf (DECODE_F "main() \n");
    printf (DECODE_F "{ \n\n");
}

# ==================================================================
#    emit_decode_info
# ==================================================================

sub emit_decode_info {
    my $i;
    printf (DECODE_F "  ISA_Decode_Begin(\"%s\"); \n\n", $ARCH_name);

    # a completely dummy decoding description: all units go to alu
    printf (DECODE_F "  STATE ex_unit = Create_Unit_State(\"ex_unit\", 0, 4); \n\n");
    printf (DECODE_F "  STATE alu = Create_Inst_State(\"alu\", 0, 0, 15); \n\n");

    printf (DECODE_F "  Transitions(ex_unit, \n");
    for ($i = 0; $i < $EXEC_SLOT_count; $i++) {
	printf (DECODE_F "        ISA_EXEC_%s_Unit, alu, \n", 
                                              $EXEC_SLOT_name[$i]);
    }
    printf (DECODE_F "	      END_TRANSITIONS); \n\n");

    printf (DECODE_F "  Transitions(alu, \n");
    # all instructions:
CONTINUE:
    for ($i = 1; $i < $OP_count; $i++) {
	my $opcode = $OP_opcode[$i];
        # skip if it's an simulated/dummy:
	if (($OP_properties[$i] & $OP_DUMMY) ||
	    ($OP_properties[$i] & $OP_SIMULATED)) {
	    next CONTINUE;
	}
	printf (DECODE_F "	   %d,\t Final(TOP_%s),\n", $i, $opcode);
    }
    printf (DECODE_F "	      END_TRANSITIONS); \n\n");

    printf (DECODE_F "  Initial_State(ex_unit); \n\n");

    printf (DECODE_F "  ISA_Decode_End(); \n");
}

# ==================================================================
#    finalize_decode_file
# ==================================================================

sub finalize_decode_file {

    printf (DECODE_F "  return 0; \n");
    printf (DECODE_F "} \n");

    close (DECODE_F);
    return;
}

# ==================================================================
#    initialize_bundle_file
# ==================================================================

sub initialize_bundle_file {
    open (BUNDLE_F, "> isa_bundle.cxx");
    &copyright_notice (BUNDLE_F);

    printf (BUNDLE_F "//  \n");
    printf (BUNDLE_F "//  Generate ISA bundle information \n");
    printf (BUNDLE_F "/////////////////////////////////////// \n\n");

    printf (BUNDLE_F "#include <stddef.h> \n");
    printf (BUNDLE_F "#include \"topcode.h\" \n");
    printf (BUNDLE_F "#include \"isa_bundle_gen.h\" \n\n");

    printf (BUNDLE_F "main() \n");
    printf (BUNDLE_F "{ \n");
}

# ==================================================================
#    emit_bundle_info
# ==================================================================

sub emit_bundle_info {
    my $unit;
    my $slot;
    my $bundle;

    # Emit the Exec_Unit types - these are EXEC_SLOTs:
    printf (BUNDLE_F "  ISA_EXEC_UNIT_TYPE \n");

    for ($unit = 0; $unit < $EXEC_SLOT_count-1; $unit++) {
	printf (BUNDLE_F "\t\t    %s_Unit, \n", $EXEC_SLOT_name[$unit]);
    }
    printf (BUNDLE_F "\t\t    %s_Unit; \n", 
	                        $EXEC_SLOT_name[$EXEC_SLOT_count-1]);
    printf (BUNDLE_F "\n");

    for ($bundle = 0; $bundle < $BUNDLE_count; $bundle++) {
	my $f_id;
	my $f_begins;
	my $f_bits;

	printf (BUNDLE_F "  ISA_Bundle_Begin(\"%s\", %d); \n",
                           $BUNDLE_name[$bundle], $BUNDLE_bits[$bundle]);
	printf (BUNDLE_F "\n");

	printf (BUNDLE_F "  /* ===== Specification for bundle packing  ===== */ \n");
	printf (BUNDLE_F "  ISA_Bundle_Pack_Create(ISA_Bundle_Pack_Little_Endian); \n");
	($f_id, $f_begins, $f_bits) = split(",", $BUNDLE_field[$bundle]);
	printf (BUNDLE_F "  Pack_Template(%d, %d, %d); \n",
                                              $f_id, $f_begins, $f_bits);
        for ($slot = 0; $slot < $BUNDLE_slots[$bundle]; $slot++) {
	    printf (BUNDLE_F "  Pack_Slot(%d, 0, %d, %d); \n",
		    $BUNDLE_slot[$bundle][$slot]{'id'},
		    $BUNDLE_slot[$bundle][$slot]{'start'},
		    $BUNDLE_slot[$bundle][$slot]{'bits'});
	}
	printf (BUNDLE_F "\n");
    }

    # Emit instructions to Exec_Unit mapping information.
    for ($unit = 0; $unit < $EXEC_SLOT_count; $unit++) {
	printf (BUNDLE_F "  /* ===== Specification for %s_Unit Type ===== */ \n", $EXEC_SLOT_name[$unit]);
	printf (BUNDLE_F "  %s_Unit = ISA_Exec_Unit_Type_Create(\"%s_Unit\", NULL); \n", $EXEC_SLOT_name[$unit], $EXEC_SLOT_name[$unit]);
	printf (BUNDLE_F "  Instruction_Exec_Unit_Group(%s_Unit, \n", 
	                                        $EXEC_SLOT_name[$unit]);
	my @scdclasses = split(",",$EXEC_SLOT_scds[$unit]);
	my $scdclass;
	foreach $scdclass (@scdclasses) {
	    my $opcode;
	    foreach $opcode (@{$SUBSET_scd{'st220'}{$scdclass}{'ops'}}) {
		printf (BUNDLE_F "\t\t TOP_%s, \n", $opcode);
	    }
	}
	printf (BUNDLE_F "\t\t TOP_UNDEFINED); \n\n");
    }

    # Emit allowed bundle combinations:
    for ($bundle = 0; $bundle < $BUNDLE_count; $bundle++) {
	my $temp;
	printf (BUNDLE_F "  /* === All legal bundle orderings (%s of them) are specified below. */ \n\n", $BUNDLE_temps[$bundle]);
	for ($temp = 0; $temp < $BUNDLE_temps[$bundle]; $temp++) {
	    my $i;
	    my $name = ${BUNDLE_temp[$bundle][$temp]}{'name'};
	    my $slots = ${BUNDLE_temp[$bundle][$temp]}{'slots'};
	    my $stops = ${BUNDLE_temp[$bundle][$temp]}{'stops'};

	    printf (BUNDLE_F "  /* ===== Template 0x00 (%d) ===== */ \n",
		                                                     $temp);
	    printf (BUNDLE_F "  ISA_Bundle_Type_Create(\"%s\", \".%s\", %d); \n", $name, $name, $slots);
	    for ($i = 0; $i < $slots; $i++) {
		my $unt = ${BUNDLE_temp[$bundle][$temp]}{'slot'}[$i]{'unit'};
		my $stp = ${BUNDLE_temp[$bundle][$temp]}{'slot'}[$i]{'stop'};
		printf (BUNDLE_F "  Slot(%d, %s_Unit); \n", $i, $unt);
		if ($stp == 1) {
		    printf (BUNDLE_F "  Stop(%d); \n", $i);
		}
	    }
            printf (BUNDLE_F "\n");
	}

        # Arthur: eventually the instr->exec_unit mapping is a
        #         part of a current bundle that start with
        #         Bundle_Begin() and ends with Bundle_End().
	printf (BUNDLE_F "\n");
	printf (BUNDLE_F "  ISA_Bundle_End(); \n");
    }

}

# ==================================================================
#    finalize_bundle_file
# ==================================================================

sub finalize_bundle_file {

    printf (BUNDLE_F "  return 0; \n");
    printf (BUNDLE_F "} \n");

    close (BUNDLE_F);
    return;
}

# ==================================================================
#    initialize_op_file
# ==================================================================

sub initialize_op_file {
    open (OP_F, "> targ_op.h");
    &copyright_notice (OP_F);

    printf (OP_F "//  \n");
    printf (OP_F "//  Generate TOP information \n");
    printf (OP_F "/////////////////////////////////////// \n\n");

    printf (OP_F "#ifndef targ_op_INCLUDED \n");
    printf (OP_F "#define targ_op_INCLUDED \n");
    printf (OP_F "\n");

    printf (OP_F "// Some of the defines are already defined here \n");
    printf (OP_F "// Set the rest to FALSE \n");
    printf (OP_F "#include \"targ_isa_properties.h\" \n");
    printf (OP_F "\n");

    printf (OP_F "// This should be on a per OP basis ?  \n");
    printf (OP_F "#define OP_COPY_OPND 1 \n");
    printf (OP_F "#define OP_PREDICATE_OPND -1     // not supported on this target \n");
    printf (OP_F "\n");
}

# ==================================================================
#    emit_top_define
# ==================================================================
sub emit_top_define {
    my $def = $_[0];
    my $val = $_[1];
    printf (OP_F "#ifndef %s \n", $def);
    printf (OP_F "#define %s(t) \t (FALSE) \n", $def);
    printf (OP_F "#endif \n");
    printf (OP_F "\n");
}

# ==================================================================
#    emit_op_info
#
#    emits a number of defines and inline functions required by the
#    cg.
# ==================================================================
sub emit_op_info {

#    printf (OP_F "/* pseudo NOP */ \n");
#    printf (OP_F "#define TOP_is_noop(t) \t (t == TOP_noop) \n");
#    printf (OP_F "\n");

#    emit_top_define ("TOP_is_likely");
#    emit_top_define ("TOP_is_branch_predict");
#    emit_top_define ("TOP_is_side_effects");
#    printf (OP_F "/* Memory instructions which are fill/spill type */\n");
#    emit_top_define ("TOP_is_mem_fill_type");
#    emit_top_define ("TOP_is_unalign_ld");
#    emit_top_define ("TOP_is_unalign_store");
#    emit_top_define ("TOP_is_defs_fcr");
#    emit_top_define ("TOP_is_defs_fcc");
#    emit_top_define ("TOP_is_refs_fcr");
#    emit_top_define ("TOP_is_select");
#    emit_top_define ("TOP_is_isub");
#    emit_top_define ("TOP_is_ior");
#    emit_top_define ("TOP_is_flop");
#    emit_top_define ("TOP_is_fadd");
#    emit_top_define ("TOP_is_fsub");
#    emit_top_define ("TOP_is_fmul");
#    emit_top_define ("TOP_is_fdiv");
#    emit_top_define ("TOP_is_imul");
#    emit_top_define ("TOP_is_idiv");
#    emit_top_define ("TOP_is_icmp");
#    emit_top_define ("TOP_is_madd");
#    printf (OP_F "/* Instruction must be first in an instruction group */ \n");    emit_top_define ("TOP_is_f_group");
#    printf (OP_F "/* Instruction must be last in an instruction group */ \n");
#    emit_top_define ("TOP_is_l_group");
#    printf (OP_F "/* Instruction accesses rotating register bank */ \n");
#    emit_top_define ("TOP_is_access_reg_bank");
#    emit_top_define ("TOP_is_unsafe");
#    emit_top_define ("TOP_save_predicates");
#    emit_top_define ("TOP_restore_predicates");

    printf (OP_F "/* _fixed_results and _fixed_opnds return how many fixed \n");
    printf (OP_F " * results/operands an instruction has (OP_result/OP_opnds includes \n");
    printf (OP_F " * any variable operands in the count). \n");
    printf (OP_F " */ \n");
    printf (OP_F "#define TOP_fixed_results(t)	(ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(t))) \n");
    printf (OP_F "#define TOP_fixed_opnds(t)	(ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(t))) \n");
    printf (OP_F "\n");

    # the following is not clear for now:
    printf (OP_F "/* ??? */ \n");
    printf (OP_F "#define OP_inst_words(o)        (1) \n");
    printf (OP_F "\n");

    printf (OP_F "inline TOP CGTARG_Noop_Top (ISA_EXEC_UNIT_PROPERTY unit) { \n");
    printf (OP_F "  return TOP_nop; \n");
    printf (OP_F "} \n");
    printf (OP_F "\n");

    #---------------------------------------------------------------------
    # TOP_is_same_res
    #---------------------------------------------------------------------
    printf (OP_F "/* Result must not be same as operand */ \n");
    printf (OP_F "inline BOOL TOP_is_uniq_res(TOP topcode, INT i) { \n");
    printf (OP_F "  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(topcode); \n");
    printf (OP_F "  ISA_OPERAND_USE this_def = ISA_OPERAND_INFO_Def(oinfo, i); \n");
    printf (OP_F "  if (this_def & OU_uniq_res)  \n");
    printf (OP_F "    return TRUE; \n");
    printf (OP_F " \n");
    printf (OP_F "  return FALSE; \n");
    printf (OP_F "} \n");
    printf (OP_F "\n");

    #---------------------------------------------------------------------
    # TOP_is_same_res
    #---------------------------------------------------------------------
    printf (OP_F "/* Returns index of operand that must be same register as result i */ \n");
    printf (OP_F "inline INT TOP_is_same_res(TOP topcode, INT i) { \n");
    printf (OP_F "  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(topcode); \n");
    printf (OP_F "  return ISA_OPERAND_INFO_Same_Res(oinfo, i); \n");
    printf (OP_F "} \n");
    printf (OP_F "\n");

    return;
}

# ==================================================================
#    finalize_op_file
# ==================================================================

sub finalize_op_file {

    printf (OP_F "#endif /* targ_op_INCLUDED */ \n");

    close (OP_F);
    return;
}


# ==================================================================
#    initialize_si_file
# ==================================================================

sub initialize_si_file {

    open (SI_F, "> st220_si.cxx");

    &copyright_notice (SI_F);

    printf (SI_F "//  %s processor scheduling information \n", $ARCH_name);
    printf (SI_F "///////////////////////////////////// \n");
    printf (SI_F "//   \n");
    printf (SI_F "//  Description:  \n");
    printf (SI_F "//  \n");
    printf (SI_F "//  Generate a scheduling description of a %s processor  \n", $ARCH_name);
    printf (SI_F "//  via the si_gen interface.  \n");
    printf (SI_F "//  \n");
    printf (SI_F "/////////////////////////////////////  \n\n");

    printf (SI_F "#include \"si_gen.h\" \n");
    printf (SI_F "#include \"targ_isa_subset.h\" \n");
    printf (SI_F "#include \"topcode.h\" \n\n");

    my $format = "";
    printf (SI_F "static RESOURCE ");
    my $rid;
    for ($rid = 0; $rid < $RES_count; $rid++) {
	printf (SI_F "%s res_%s", $format, $RES_name[$rid]);
	$format = ",\n	       ";
    }
    printf (SI_F "; \n\n");
    printf (SI_F "int \n");
    printf (SI_F "main (int argc, char *argv[]) \n");
    printf (SI_F "{ \n");

    for ($rid = 0; $rid < $RES_count; $rid++) {
	printf (SI_F "  res_%s = RESOURCE_Create(\"%s\", %d); \n",
		$RES_name[$rid], $RES_name[$rid], $RES_avail[$rid]);
    }

    printf (SI_F "\n");
}

# ==================================================================
#    read_scdinfo
#
#    Information is in st220_arc.db file
# ==================================================================

sub read_scdinfo {
    my $sset;
    my $subset;
    my $line;

    if (!open (ARCH_F, "< ../ST200/st220_arc.db")) {
	printf (STDOUT "ERROR: can't open file \"st220_arc.db\" \n");
	exit(1);
    }

    printf (STDOUT "  *** reading schedinfo ***\n");
    
CONTINUE:
    for ($sset = 0; $sset < $SUBSET_count; $sset++) {
	$subset = $SUBSET_name[$sset];

	printf (STDOUT "read_scdinfo: inspecting subset %s\n", $subset);

	#------------------------------------------------------
	# + =======...======= + starts a new record
	# ...
	# <blank line> ends it
	#------------------------------------------------------
	while ($line = <ARCH_F>) {  
	    if ($line =~ /^\+\s========/) {
		# found beginning of a new record
		goto RECORDS;
	    }
	}

	printf (STDOUT "ERROR: couldn't find first record \n");
	exit(1);

RECORDS:
	my $mnemonic;
	my $resources;
	my $scdclass;
	my $opcode;

	# Read all the info untill next record:
	while ($line = <ARCH_F>) {
	    if ($line =~ /^\+=(\S*)/) {
		$mnemonic = $1;
		$mnemonic =~ tr/./_/;
	    }

	    if ($line =~ /^\+resources\s(\S*)/) {
		$resources = $1;
	    }

	    if ($line =~ /^\n/) {
		# end of record
		printf (STDOUT " %s: %s\n", $mnemonic, $resources);

		# resources corresponds to a scheduling class:
		if ($resources eq 'ALU') {
		    $scdclass = 'ALU';
		}
		elsif ($resources eq 'MUL') {
		    $scdclass = 'MUL';
		}
		elsif ($resources eq 'LOAD') {
		    $scdclass = 'LOAD';
		}
		elsif ($resources eq 'STORE') {
		    $scdclass = 'STORE';
		}
		elsif ($resources eq 'BRANCH') {
		    $scdclass = 'BRANCH';
		}
		elsif ($resources eq 'CALL') {
		    $scdclass = 'CALL';
		}
		elsif ($resources eq 'IMM') {
		    $scdclass = 'IMM';
		}
		else {
		    printf(STDOUT "ERROR: unknown resource %s in scdinfo\n",
			   $resources);
		    exit(1);
		}

		# Sanity check:
		my $sc;
		for ($sc = 0; $sc < $SCD_CLASS_count; $sc++) {
		    if ($scdclass eq $SCD_CLASS_name[$sc]) {
			goto OK;
		    }
		}
		printf (STDOUT "ERROR: scd class %s in scdinfo has not been declared\n", 
			$scdclass);
		exit(1);

OK:
		# ISSUE: TOPs are issued at cycle 0
		# MEM: used at cycle 0.
		$SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
		if ($scdclass eq 'LOAD' ||
		    $scdclass eq 'STORE') {
		    $SUBSET_scd{$subset}{$scdclass}{'res'}{'MEM'} = 0;
		}

		# Insert this TOP into the list:
		for ($opcode = 1; $opcode < $OP_count; $opcode++) {
		    if ($OP_mnemonic[$opcode] eq $mnemonic) {
			push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$opcode]);

			print STDOUT " pushing $OP_opcode[$opcode] ($subset,$scdclass)\n";
		    }
		}
	    } # end of record
	} # while it's not END_OF_FILE

	# ----------------------------------------------------------
	# The send_i and recv_i are not in the arc.db file, add info
	# here:

        $mnemonic = "send";
        $scdclass = "ALU";
	# Only one machine resource for now, ISSUE
	# TOPs are issued at cycle 0
	$SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
	# Insert this TOP into the list:
	for ($opcode = 1; $opcode < $OP_count; $opcode++) {
	    if ($OP_mnemonic[$opcode] eq $mnemonic) {
		push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$opcode]);

		print STDOUT " pushing $OP_opcode[$opcode] ($subset,$scdclass)\n";
	    }
	}

        $mnemonic = "recv";
        $scdclass = "ALU";
	# Only one machine resource for now, ISSUE
	# TOPs are issued at cycle 0
	$SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
	# Insert this TOP into the list:
	for ($opcode = 1; $opcode < $OP_count; $opcode++) {
	    if ($OP_mnemonic[$opcode] eq $mnemonic) {
		push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$opcode]);

		print STDOUT " pushing $OP_opcode[$opcode] ($subset,$scdclass)\n";
	    }
	}


    } # foreach $subset

    close(ARCH_F);
}

# ==================================================================
#    emit_scdinfo
#
#    Write scheduling info into file st200_si.cxx
# ==================================================================

sub emit_scdinfo {
    my $scdclass;
    my $opcode;

    # Read scheduling info for each TOP:
    &read_scdinfo();

    # For each ISA subset:
    my $i;
CONTINUE:
    for ($i = 0; $i < $SUBSET_count; $i++) {
	my $subset = $SUBSET_name[$i];

	printf (SI_F "  /* ======================================================\n");
	printf (SI_F "   * Resource description for the ISA_SUBSET_%s \n",
		$subset);
	printf (SI_F "   * ======================================================\n");
	printf (SI_F "   */ \n\n");

	printf (SI_F "  Machine(\"%s\", ISA_SUBSET_%s, argc, argv); \n",
		$ARCH_name, $subset);

	# emit groups of instructions with similar resource constraints:
	my $i;
	for ($i = 0; $i < $SCD_CLASS_count; $i++) {
	    $scdclass = $SCD_CLASS_name[$i];
            printf (SI_F "\n  ///////////////////////////////////////// \n");
            printf (SI_F "  //   Instructions for Scd Class %s \n", $scdclass);
            printf (SI_F "  ///////////////////////////////////////// \n\n");

	    printf (SI_F "  Instruction_Group(\"%s\", \n", $scdclass);
            foreach $opcode (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}) {
		printf (SI_F "\t\t TOP_%s, \n", $opcode);
	    }
	    printf (SI_F "\t\t TOP_UNDEFINED); \n\n");

            # define latencies:
	    printf (SI_F "  Any_Operand_Access_Time(0); \n");
	    printf (SI_F "  Any_Result_Available_Time(1);		// ??? not sure \n");

            # define resource requirements:
	    my $rid;
	    for ($rid = 0; $rid < $RES_count; $rid++) {
		my $rname = $RES_name[$rid];
                if (defined($SUBSET_scd{$subset}{$scdclass}{'res'}{$rname})) {
                    printf (SI_F "  Resource_Requirement(res_%s, %d); \n",
		            $rname, 
                            $SUBSET_scd{$subset}{$scdclass}{'res'}{$rname});
                }
            }
	    printf (SI_F "\n");
	}

	printf (SI_F "  Machine_Done(\"st220.c\"); \n\n");
    }

    return;
}

# ==================================================================
#    finalize_si_file
# ==================================================================

sub finalize_si_file {

    printf (SI_F "}\n");

    close (SI_F);
    return;
}




# ==================================================================
#    copyright_notice
# ==================================================================

sub copyright_notice {

    my $FILE = $_[0];

    printf($FILE "/* -- This file is automatically generated -- */ \n");
    printf($FILE "/* \n\n");

    printf($FILE "  Copyright (C) 2002 ST Microelectronics, Inc.  All Rights Reserved. \n\n");

    printf($FILE "  This program is free software; you can redistribute it and/or modify it \n");
    printf($FILE "  under the terms of version 2 of the GNU General Public License as \n");
    printf($FILE "  published by the Free Software Foundation. \n");

    printf($FILE "  This program is distributed in the hope that it would be useful, but \n");
    printf($FILE "  WITHOUT ANY WARRANTY; without even the implied warranty of \n");
    printf($FILE "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. \n\n");

    printf($FILE "  Further, this software is distributed without any warranty that it is \n");
    printf($FILE "  free of the rightful claim of any third person regarding infringement \n");
    printf($FILE "  or the like.  Any license provided herein, whether implied or \n");
    printf($FILE "  otherwise, applies only to this software file.  Patent licenses, if \n");
    printf($FILE "  any, provided herein do not apply to combinations of this program with \n");
    printf($FILE "  other software, or any other product whatsoever. \n");

    printf($FILE "  You should have received a copy of the GNU General Public License along \n");
    printf($FILE "  with this program; if not, write the Free Software Foundation, Inc., 59 \n");
    printf($FILE "  Temple Place - Suite 330, Boston MA 02111-1307, USA. \n\n");

    printf($FILE "  Contact information:  ST Microelectronics, Inc., \n");
    printf($FILE "  , or: \n\n");

    printf($FILE "  http://www.st.com \n\n");

    printf($FILE "  For further information regarding this notice, see: \n\n");

    printf($FILE "  http: \n");

    printf($FILE "*/ \n\n");
}

# ==================================================================
#    DECL_ARCHITECTURE
# ==================================================================

sub DECL_ARCHITECTURE {
    $ARCH_name = $_[0];
}

# ==================================================================
#    DECL_ISA_SUBSET
#
#    An ISA subset is composed of a list of TOPs.
#    To each subset there are corresponding SUBSET_res and SUBSET_scd
#
#    array SUBSET_scd{$subset} = { int id; char *name; array res }
# ==================================================================

sub DECL_ISA_SUBSET {
    $SUBSET_name[$SUBSET_count] = $_[0];
    $SUBSET_count++;
}

# ==================================================================
#    subset_id
# ==================================================================

sub subset_id {
    my $name = $_[0];
    my $i;
    for ($i = 0; $i < $SUBSET_count; $i++) {
	if ($SUBSET_name[$i] eq $name) {
	    return $i;
	}
    }

    # couldn't find this subset:
    printf(STDOUT "ERROR: subset %s undeclared\n", $name);
    exit(1);
}

# ==================================================================
#    DECL_ISA_PROPERTY
# ==================================================================

sub DECL_ISA_PROPERTY {
    $ISA_PROPERTY_name[$ISA_PROPERTY_count] = $_[0];
    $ISA_PROPERTY_count++;
}

# ==================================================================
#    DECL_RCLASS
# ==================================================================

sub DECL_RCLASS {
    $RCLASS_name[$RCLASS_count] = $_[0];
    $RCLASS_bits[$RCLASS_count] = $_[1];
    $RCLASS_is_ptr[$RCLASS_count] = $_[2];
    if ($RCLASS_is_ptr[$RCLASS_count] == $TRUE) {
	$PTR_TYPE = $RCLASS_name[$RCLASS_count];
    }
    $RCLASS_can_be_stored[$RCLASS_count] = $_[3];
    $RCLASS_mult_store[$RCLASS_count] = $_[4];
    $RCLASS_count++;
}

sub rclass_id {
    my $rclass_name = $_[0];
    my $id;

    for ($id = 0; $id < $RCLASS_count; $id++) {
        if ($RCLASS_name[$id] eq $rclass_name) {
	    goto BREAK;
	}
    }
    printf(STDOUT "ERROR: invalid rclass name %s\n", $rclass_name);
    exit(1);

BREAK:
    return $id;
}

# ==================================================================
#    DECL_SUBCLASS
# ==================================================================

sub DECL_REG_SUBSET {
    my $name = shift(@_);
    my @regs = @_;  
    push (@regs,-1); # must end with -1
    my $reg;

    my $idx = $REGSET_subset_count[$cur_regset];

    $REGSET_subset[$cur_regset][$idx]{'name'} = $name;
    $REGSET_subset[$cur_regset][$idx]{'rclass'} = $REGSET_rclass[$cur_regset];

    while (($elt = shift(@regs)) != -1) {
	for ($reg = $REGSET_minreg[$cur_regset];
	     $reg <= $REGSET_maxreg[$cur_regset];
	     $reg++) {
	    if ($reg == $elt) {
		$REGSET_subset[$cur_regset][$idx]{'regs'}[$reg] = 1;
	    }
	    else {
		$REGSET_subset[$cur_regset][$idx]{'regs'}[$reg] = 0;
	    }
	}
    }

    $REGSET_subset_count[$cur_regset]++;
    return;
}

# ==================================================================
#    DECL_REG_SET
# ==================================================================

sub DECL_REG_SET {
    my $i;
    my $rclass = shift(@_);
    my $minreg = shift(@_);
    my $maxreg = shift(@_);
    my $format = shift(@_);
    my @names = @_;

    $REGSET_rclass[$REGSET_count] = $rclass;
    $REGSET_minreg[$REGSET_count] = $minreg;
    $REGSET_maxreg[$REGSET_count] = $maxreg;
    $REGSET_format[$REGSET_count] = $format;
    for ($i = 0; $i < scalar(@names); $i++) {
	$REGSET_names[$REGSET_count][$i] = $names[$i];
    }
    $REGSET_subset_count[$REGSET_count] = 0;
    $cur_regset = $REGSET_count;
    $REGSET_count++;

    for ($i = 0; $i < scalar(@names); $i++) {
	print STDOUT "$REGSET_names[$REGSET_count-1][$i] \n";
    }

    return;
}

# ==================================================================
#    DECL_LCLASS
# ==================================================================

sub DECL_LCLASS {
    $LCLASS_name[$LCLASS_count] = $_[0];
    $LCLASS_bits[$LCLASS_count] = $_[1];
    $LCLASS_rtype[$LCLASS_count] = $_[2];
    $LCLASS_count++;
}

sub lclass_id {
    my $lclass_name = $_[0];
    my $id;

    for ($id = 0; $id < $LCLASS_count; $id++) {
        if ($LCLASS_name[$id] eq $lclass_name) {
	    goto BREAK;
	}
    }
    printf(STDOUT "ERROR: invalid lclass name %s\n", $lclass_name);
    exit(1);

BREAK:
    return $id;
}

# ==================================================================
#    DECL_ECLASS
# ==================================================================

sub DECL_ECLASS {
    $ECLASS_name[$ECLASS_count] = $_[0];
    $ECLASS_bits[$ECLASS_count] = $_[1];
    $ECLASS_elt_count[$ECLASS_count] = 0;
    $cur_eclass = $ECLASS_count;
    $ECLASS_count++;
}

sub ENUM {
    my $idx = $ECLASS_elt_count[$cur_eclass];
    $ECLASS_elt[$cur_eclass][$idx]{'name'} = $_[0];
    $ECLASS_elt[$cur_eclass][$idx]{'val'} = $_[1];
    $ECLASS_elt_count[$cur_eclass]++;
}

sub eclass_id {
    my $eclass_name = $_[0];
    my $id;

    for ($id = 0; $id < $ECLASS_count; $id++) {
        if ($ECLASS_name[$id] eq $eclass_name) {
	    goto BREAK;
	}
    }
    printf(STDOUT "ERROR: invalid eclass name %s\n", $eclass_name);
    exit(1);

BREAK:
    return $id;
}

# ==================================================================
#    DECL_REG_OPND
# ==================================================================

sub DECL_REG_OPND {
    my $rclass_id = &rclass_id($_[1]);

    $REG_OPND_name[$REG_OPND_count] = $_[0];
    $REG_OPND_rclass[$REG_OPND_count] = $_[1];
    $REG_OPND_subclass[$REG_OPND_count] = $_[2];
    $REG_OPND_size[$REG_OPND_count] = $RCLASS_bits[$rclass_id];
    $REG_OPND_rtype[$REG_OPND_count] = $_[3];
    $REG_OPND_fp_type[$REG_OPND_count] = "INVALID";
    $REG_OPND_count++;
}

# ==================================================================
#    DECL_LIT_OPND
# ==================================================================

sub DECL_LIT_OPND {
    my $lclass_id = &lclass_id($_[1]);

    $LIT_OPND_name[$LIT_OPND_count] = $_[0];
    $LIT_OPND_lclass[$LIT_OPND_count] = $_[1];
    $LIT_OPND_size[$LIT_OPND_count] = $LCLASS_bits[$lclass_id];
    $LIT_OPND_rtype[$LIT_OPND_count] = $_[2];
    $LIT_OPND_count++;
}

# ==================================================================
#    DECL_ENUM_OPND
# ==================================================================
sub DECL_ENUM_OPND {
    my $eclass_id = &eclass_id($_[1]);

    $ENUM_OPND_name[$ENUM_OPND_count] = $_[0];
    $ENUM_OPND_eclass[$ENUM_OPND_count] = $_[1];
    $ENUM_OPND_size[$ENUM_OPND_count] = $ECLASS_bits[$eclass_id];
    $ENUM_OPND_rtype[$ENUM_OPND_count] = $_[2];
    $ENUM_OPND_count++;
}

# ==================================================================
#    DECL_OPND_USE
# ==================================================================
sub DECL_OPND_USE {
    my $name = $_[0];

    $OPND_USE_name[$OPND_USE_count] = $name;
    $OPND_USE_count++;
}

# ==================================================================
#    DECL_SCD_RESOURCE
# ==================================================================
sub DECL_SCD_RESOURCE {
    my $name = $_[0];                       # resource name
    my $avail = $_[1];                      # available units

    $RES_name[$RES_count] = $name;
    $RES_avail[$RES_count] = $avail;
    $RES_count++;
}

# ==================================================================
#    DECL_SCD_CLASS
# ==================================================================
sub DECL_SCD_CLASS {
    $SCD_CLASS_name[$SCD_CLASS_count] = $_[0];
    $SCD_CLASS_count++;
}

# ==================================================================
#    DECL_PACK_TYPE
# ==================================================================

sub DECL_PACK_TYPE {
    $PACK_type[$PACK_count] = $_[0];
    $PACK_comment[$PACK_count] = $_[1];
    $PACK_count++;
}

# ==================================================================
#    DECL_EXEC_SLOT
# ==================================================================

sub DECL_EXEC_SLOT {
    $EXEC_SLOT_name[$EXEC_SLOT_count] = $_[0];
    $EXEC_SLOT_scds[$EXEC_SLOT_count] = $_[1];
    $EXEC_SLOT_count++;
}

# ==================================================================
#    DECL_BUNDLE
# ==================================================================

sub DECL_BUNDLE {
    my $i;

    $BUNDLE_name[$BUNDLE_count] = $_[0];
    $BUNDLE_bits[$BUNDLE_count] = $_[1];
    $BUNDLE_field[$BUNDLE_count] = $_[2];
    $BUNDLE_slots[$BUNDLE_count] = $_[3];

    printf(STDOUT "BUNDLE %s %d slots: \n",
	   $BUNDLE_name[$BUNDLE_count],
	   $BUNDLE_slots[$BUNDLE_count]);

    for ($i = 0; $i < $_[3]; $i++) {
	my ($id, $start, $bits) = split(",", $_[$i+4]);
	$BUNDLE_slot[$BUNDLE_count][$i]{'id'} = $id;
	$BUNDLE_slot[$BUNDLE_count][$i]{'start'} = $start;
	$BUNDLE_slot[$BUNDLE_count][$i]{'bits'} = $bits;

	printf(STDOUT "  DECL_SLOT %s -- (%d, 0, %d, %d)\n", $_[$i+4],
	       $BUNDLE_slot[$BUNDLE_count][$i]{'id'},
	       $BUNDLE_slot[$BUNDLE_count][$i]{'start'},
	       $BUNDLE_slot[$BUNDLE_count][$i]{'bits'});

    }
    $BUNDLE_temps[$BUNDLE_count] = 0;
    $cur_bundle = $BUNDLE_count;
    $BUNDLE_count++;
}

# ==================================================================
#    DECL_TEMPLATE
# ==================================================================

sub DECL_TEMPLATE {
    my $i;
    my $temp;

    $temp = $BUNDLE_temps[$cur_bundle];
    ${BUNDLE_temp[$cur_bundle][$temp]}{'name'} = $_[0];
    ${BUNDLE_temp[$cur_bundle][$temp]}{'slots'} = $_[1]; 

    # Initialize stops to no stops.
    for ($i = 0; $i < $_[1]; $i++) {
	${BUNDLE_temp[$cur_bundle][$temp]}{'slot'}[$i]{'stop'} = 0;
    }
    $cur_temp = $BUNDLE_temps[$cur_bundle];
    $BUNDLE_temps[$cur_bundle]++;
}

# ==================================================================
#    SLOT
# ==================================================================

sub SLOT {
    my $i;

    # slot exceeds the number of allowed slots:
    if (${BUNDLE_temp[$cur_bundle][$cur_temp]}{'slots'} <= $_[0]) {
	printf(STDOUT "ERROR: invalid slot number %d\n", $_[0]);
	exit(1);
    }
    # slot refers to non-existing FU
    for ($i = 0; $i < $EXEC_SLOT_count; $i++) {
	if ($_[1] eq $EXEC_SLOT_name[$i]) {
	    goto BREAK;
	}
    }
    printf(STDOUT "ERROR: invalid slot kind %d\n", $_[1]);
    exit(1);

BREAK:
    ${BUNDLE_temp[$cur_bundle][$cur_temp]}{'slot'}[$_[0]]{'unit'} = $_[1];
}

# ==================================================================
#    STOP
# ==================================================================

sub STOP {
    # stop exceeds the number of allowed slots:
    if (${BUNDLE_temp[$cur_bundle][$cur_temp]}{'slots'} <= $_[0]) {
	printf(STDOUT "ERROR: invalid stop number %d\n", $_[0]);
	exit(1);
    }

    ${BUNDLE_temp[$cur_bundle][$cur_temp]}{'slot'}[$_[0]]{'stop'} = 1;
}

# ==================================================================
#    finish_opcode_processing
# ==================================================================
sub finish_opcode_processing {
    my $opcode = $_[0];

    $OP_opcode[$opcode] = 
                  $OP_mnemonic[$opcode].$OP_variant[$opcode];
    printf (STDOUT " TOP = %s\n", $OP_opcode[$opcode]);

    # only one subset for now:
    $OP_subset[$opcode] = "st220";
    push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$opcode]);

    # operand groups:
    push(@{$SignatureGroup{$OP_signature[$opcode]}}, $OP_opcode[$opcode]);

    # print groups:
    push(@{$PrintGroup{$OP_print[$opcode]}}, $OP_opcode[$opcode]);

    # properties:
    if ($OP_properties[$opcode] & $OP_LOAD) {
	push(@{$AttrGroup{'load'}}, $OP_opcode[$opcode]);
	push(@{$MemBytes{$OP_bytes[$opcode]}}, $OP_opcode[$opcode]);
	push(@{$MemAlign{$OP_align[$opcode]}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_STORE) {
	push(@{$AttrGroup{'store'}}, $OP_opcode[$opcode]);
	push(@{$MemBytes{$OP_bytes[$opcode]}}, $OP_opcode[$opcode]);
	push(@{$MemAlign{$OP_align[$opcode]}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_DISMISSIBLE) {
	push(@{$AttrGroup{'dismissible'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_CALL) {
	push(@{$AttrGroup{'call'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_BRANCH) {
	push(@{$AttrGroup{'branch'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_JUMP) {
	push(@{$AttrGroup{'jump'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_IJUMP) {
	push(@{$AttrGroup{'ijump'}}, $OP_opcode[$opcode]);
    }

    return;
}

# ==================================================================
#    main
# ==================================================================

####################################################################
#
#  Following CHESS files are required:
#
#    st220_opc.db - describes TOPs, operands, printing format
#    st220_fmt.db - describes groups of TOPs by properties and 
#                   printing format
#    st220_arc.db - describes scheduling latencies, resources
#
####################################################################

  if (!open (OPCODE_F, "< ../ST200/st220_opc.db")) {
      printf (STDOUT "ERROR: can't open file \"st220_opc.db\" \n");
      exit(1);
  }

  if (!open (FORMAT_F, "< ../ST200/st220_fmt.db")) {
      printf (STDOUT "ERROR: can't open file \"st220_fmt.db\" \n");
      exit(1);
  }

  ####################################################################
  #                         GLOBAL VARIABLES
  ####################################################################

  $TRUE = 1;
  $FALSE = 0;

  $SIGNED = "SIGNED";
  $UNSIGNED = "UNSIGNED";
  $PCREL = "PCREL";

  # global ISA subset count
  $SUBSET_count = 0;
  # global ISA properties count
  $ISA_PROPERTY_count = 0;

  $RCLASS_count = 0;
  $REGSET_count = 0;
  $LCLASS_count = 0;
  $ECLASS_count = 0;

  $REG_OPND_count = 0;
  $LIT_OPND_count = 0;
  $ENUM_OPND_count = 0;
  $OPND_USE_count = 0;

  # global TOPs count
  $OP_count = 0;
  # global scheduling resource count
  $RES_count = 0;
  # global SCD_CLASS count
  $SCD_CLASS_count = 0;
  # global EXEC_SLOT count
  $EXEC_SLOT_count = 0;
  # global BUNDLE count -- can only be 1 for now
  $BUNDLE_count = 0;
  # global packing groups count (instruction binary compression)
  $PACK_count = 0;

  ####################################################################
  #  Operation properties -- 32 bits can map 32 properties:
  ####################################################################

  $OP_NONE        = 0x00000000;
  $OP_SIMULATED   = 0x00000002;  # simulated TOP - expends into a seq.
  $OP_DUMMY       = 0x00000004;  # dummy TOP - do not get emitted
  $OP_LOAD        = 0x00000010;  
  $OP_STORE       = 0x00000020;
  $OP_BRANCH      = 0x00000040;
  $OP_JUMP        = 0x00000080;
  $OP_IJUMP       = 0x00000100;
  $OP_CALL        = 0x00000200;
  $OP_DISMISSIBLE = 0x00000800;
  $OP_NOOP        = 0x00001000;

  ####################################################################
  #                        ISA DESCRIPTION
  #
  #     Declare the ISA subsets available on the machine.
  #     OPerations can belong to only one subset for now. 
  #     TODO: the compiler pseudo OPerations should be able to belong 
  #           to all ISA subsets ??.
  #
  #     2. Declare TOP properties
  #
  #     3. Declare register classes, immediate and enumerate 
  #        classes.
  #
  #     4. Declare register sets, subsets. Declare operand types.
  #
  ####################################################################

  &DECL_ARCHITECTURE("st200");

  &DECL_ISA_SUBSET ("st220");

  # Following properties are required by the Pro64:
  &DECL_ISA_PROPERTY("noop");
  &DECL_ISA_PROPERTY("load");
  &DECL_ISA_PROPERTY("store");
  &DECL_ISA_PROPERTY("jump");
  &DECL_ISA_PROPERTY("ijump");
  &DECL_ISA_PROPERTY("branch");
  &DECL_ISA_PROPERTY("call");
  &DECL_ISA_PROPERTY("simulated");
  &DECL_ISA_PROPERTY("dummy");
  &DECL_ISA_PROPERTY("var_opnds");

  # Some additional properties:
  &DECL_ISA_PROPERTY("dismissible");

  # rclass (name, bit_size, is_ptr, can_be_stored, mult_store):

  &DECL_RCLASS("integer", 32, $TRUE, $TRUE, $FALSE);
  &DECL_RCLASS("branch", 1, $FALSE, $TRUE, $FALSE);

  # Register Set (rclass, minreg, maxreg, format, reg_names):
  # For each set define also Reg subset (name, list_of_members):
  @int_regs = ('$r0.0', '$r0.1', '$r0.2');
  &DECL_REG_SET("integer", 0, 63, "\$r0.%d", @int_regs);
    &DECL_REG_SUBSET("r0", (0));
    &DECL_REG_SUBSET("lr", (63));

  @br_regs = ('$b0.0');
  &DECL_REG_SET("branch", 0, 7, "\$b0.%d", @br_regs);

  # lclass (name, bit_size, signed?):

  &DECL_LCLASS("s32", 32, $SIGNED);
  &DECL_LCLASS("u32", 32, $UNSIGNED);
  &DECL_LCLASS("s23", 23, $SIGNED);
  &DECL_LCLASS("u23", 23, $UNSIGNED);
  &DECL_LCLASS("s9", 9, $SIGNED);

  &DECL_ECLASS("ibus", 9);
    &ENUM("c0", 0);
    &ENUM("c1", 1);
    &ENUM("c2", 2);
    &ENUM("c3", 3);
    &ENUM("c4", 4);
    &ENUM("c5", 5);
    &ENUM("c6", 6);
    &ENUM("c7", 7);
    &ENUM("c8", 8);

  # register operand (name, rclass, reg_subclass, rtype):

  &DECL_REG_OPND("bcond", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("scond", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("bdest", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("ibdest", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("dest", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("idest", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("src1", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("src2", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("lr", "integer", "lr", $UNSIGNED);

  # literal operand (name, lclass, rtype):

  &DECL_LIT_OPND("btarg","s23", $SIGNED);
  &DECL_LIT_OPND("isrc2","s32", $SIGNED);
  &DECL_LIT_OPND("s9","s9", $SIGNED);
  &DECL_LIT_OPND("imm","u23", $UNSIGNED);
  &DECL_LIT_OPND("pcrel","s32", $PCREL);
  &DECL_LIT_OPND("absadr","u32", $UNSIGNED);

  # enumeration operand (name, eclass, rtype):

  &DECL_ENUM_OPND("icbus","ibus", $UNSIGNED);

  # operand use types:
#  &DECL_OPND_USE("base");
#  &DECL_OPND_USE("offset");
#  &DECL_OPND_USE("storeval");
  &DECL_OPND_USE("target");
  &DECL_OPND_USE("postincr");
#  &DECL_OPND_USE("uniq_res");

  ####################################################################
  #                       SCHEDULING MODEL
  #
  #     1. Declare resources available on the machine. This corresponds
  #        to the compiler scheduling abstraction. 
  #
  #     2. An instruction belongs to a unique SCD_CLASS. 
  #        Instructions that belong to one scheduling class have
  #        similar resource usage.
  #        DECL_SCD_CLASS () function declares a SCD_CLASS.
  #
  #     3. A SCD_CLASS may execute in one or more EXEC_SLOTs.
  #        DECL_EXEC_SLOT () function declares EXEC_SLOTs and
  #        SCD_CLASSes that can execute in this slot. 
  #
  #     4. Specify bundle information.
  #
  ####################################################################

  &DECL_SCD_RESOURCE ("ISSUE", 4);     # 4 issue slots
  &DECL_SCD_RESOURCE ("INT", 4);       # 4 integer units
  &DECL_SCD_RESOURCE ("MUL", 2);       # 2 multipliers
  &DECL_SCD_RESOURCE ("MEM", 1);       # 2 load/store units
  &DECL_SCD_RESOURCE ("XFER", 1);      # 1 branch unit

  &DECL_SCD_CLASS ("ALU");             # Simple 1 cycle ALU TOPs
  &DECL_SCD_CLASS ("MUL");             # 3 cycle MUL TOPs
  &DECL_SCD_CLASS ("LOAD");            # 3 cycle LOAD TOPs
  &DECL_SCD_CLASS ("STORE");           # STORE TOPs
  &DECL_SCD_CLASS ("IMM");             # Immediate operand extend
  &DECL_SCD_CLASS ("BRANCH");          # control flow transfer
  &DECL_SCD_CLASS ("CALL");            # function call

  &DECL_EXEC_SLOT ("S0", "ALU,,MUL,LOAD,STORE,BRANCH,CALL"); # Slot 0
  &DECL_EXEC_SLOT ("S1", "ALU,LOAD,STORE,IMM");             # Slot 1
  &DECL_EXEC_SLOT ("S2", "ALU,MUL,LOAD,STORE");            # Slot 2
  &DECL_EXEC_SLOT ("S3", "ALU,LOAD,STORE,IMM");             # Slot 3

#  &DECL_EXEC_SLOT ("ALU", "INT,LOAD,STORE");      # Slot 0,1,2,3
#  &DECL_EXEC_SLOT ("BR", "XFER,CALL");            # Branch 0
#  &DECL_EXEC_SLOT ("MUL", "MULT");                # Multiply 0,2
#  &DECL_EXEC_SLOT ("EXT", "IMM");                 # Immediate 1,3

  # Declare bundle (name, size_in_bits, temp_bits, num_slots, [slots 0..N]).
  &DECL_BUNDLE("st200", 128, "0,0,0", 4, "0,0,32", "1,32,32", "2,64,32", "3,96,32");

  # Define possible templates (combinations of EXEC_SLOTs in BUNDLEs).
  # ----------------------------------------------------------

  &DECL_TEMPLATE("temp0",4);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &SLOT(2, "S2");
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp1",4);
    &SLOT(0, "S0");
    &STOP(0);
    &SLOT(1, "S1");
    &SLOT(2, "S2");
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp2",4);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &STOP(1);
    &SLOT(2, "S2");
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp3",4);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &SLOT(2, "S2");
    &STOP(2);
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp4",4);
    &SLOT(0, "S0");
    &STOP(0);
    &SLOT(1, "S1");
    &STOP(1);
    &SLOT(2, "S2");
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp5",4);
    &SLOT(0, "S0");
    &STOP(0);
    &SLOT(1, "S1");
    &SLOT(2, "S2");
    &STOP(2);
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp6",4);
    &SLOT(0, "S0");
    &STOP(0);
    &SLOT(1, "S1");
    &STOP(1);
    &SLOT(2, "S2");
    &STOP(2);
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp7",4);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &STOP(1);
    &SLOT(2, "S2");
    &STOP(2);
    &SLOT(3, "S3");
    &STOP(3);

  ###############################################################
  #                ASSEMBLER/DESASSEMBLER MODEL
  #
  #  1. Specify the decoding transition rules ?? They are somehow
  #     related to execution unit types ?? see isa_decode.cxx
  #
  #  For now this is a completely dummy part.
  #
  ###############################################################

  &DECL_PACK_TYPE ("a1", "Integer ALU -- Register-Register");

  ###############################################################
  #                       PROCESSING
  ###############################################################

  # open and emit head info into all concerned files:
  &initialize_isa_file ();
  &initialize_reg_file ();
  &initialize_lit_file ();
  &initialize_enum_file ();
  &initialize_si_file ();
  &initialize_operands_file ();
  &initialize_subset_file ();
  &initialize_properties_file ();
  &initialize_print_file ();
  &initialize_pack_file ();
  &initialize_bundle_file ();
  &initialize_decode_file ();

  &initialize_op_file();

  # read OPCODE_F file:
  my $line;

  while ($line = <OPCODE_F>) {
      # Read one OPeration record:
      if ($line =~ /^\+=s./) {
          if ($OP_count > 0 && $OP_mnemonic[$OP_count] ne 'asm') {
	      # finish processing of the previous record:
	      &finish_opcode_processing($OP_count);
	  }

          # beginning of a new recore
          printf (STDOUT " ----- new record %d ----\n", $OP_count);

          # reset OP_properties:
          $OP_properties[$OP_count] = $OP_NONE;

          # if it was an asm, overwrite it:
          if ($OP_mnemonic[$OP_count] ne 'asm') {
            $OP_count++;
	  }
      }

      if ($line =~ /^\+mnemonic\s(\S*)\n/) {
          my $mnemonic = $1;
          $mnemonic =~ tr/./_/;
          $OP_mnemonic[$OP_count] = $mnemonic;
          printf (STDOUT " mnemonic %s\n", $1);
      }

      if ($line =~ /^\+format\s(\w*)/) {
          $OP_format[$OP_count] = $1;
          printf (STDOUT " format %s\n", $1);

          $OP_variant[$OP_count] = &op_variant($OP_format[$OP_count]);

          # operand groups:
          $OP_signature[$OP_count] = &op_signature($OP_count);

          # print groups:
          $OP_print[$OP_count] = &op_print($OP_count);
          print STDOUT "  $OP_print[$OP_count] \n";

          # special cases:
          if ($OP_mnemonic[$OP_count] eq 'pft') {
	      # prefetch is marked as Store but is not
	  }
          elsif ($OP_mnemonic[$OP_count] eq 'prgadd' ||
		 $OP_mnemonic[$OP_count] eq 'prgset') {
	      # purge cache is marked as Store but is not
	  }
          elsif ($OP_mnemonic[$OP_count] eq 'sync') {
	      # sync is marked as Store but is not
	  }
          else {
	      # set up properties:
	      &sort_by_properties($OP_count);
	  }
      }
  }

  # I assume that at least 1 record is found:
  if ($OP_mnemonic[$OP_count] ne 'asm') {
      &finish_opcode_processing($OP_count);
      $OP_count++;
  }

  # There is a number of Assembler macroes that are not included
  # into the Chess DB:

  my $scdclass;
  my $subset;

  # ----------------- nop ------------------

  # beginning of a new recore
  printf (STDOUT " ----- new record %d ----\n", $OP_count);
  # reset OP_properties:
  $OP_properties[$OP_count] = $OP_NONE;
  # mnemonic:
  $OP_mnemonic[$OP_count] = 'nop';
  printf (STDOUT " mnemonic %s\n", $OP_mnemonic[$OP_count]);
  $OP_format[$OP_count] = 'Macro';
  printf (STDOUT " format %s\n", $OP_format[$OP_count]);
  $OP_variant[$OP_count] = "";
  $OP_opcode[$OP_count] = 
                  $OP_mnemonic[$OP_count].$OP_variant[$OP_count];
  printf (STDOUT " TOP = %s\n", $OP_opcode[$OP_count]);
  #properties:
  push(@{$AttrGroup{'noop'}}, $OP_opcode[$OP_count]);
  # subset:
  $OP_subset[$OP_count] = "st220";
  push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$OP_count]);
  # operand groups:
  $OP_signature[$OP_count] = ':nop:';
  push(@{$SignatureGroup{$OP_signature[$OP_count]}}, $OP_opcode[$OP_count]);
  # print groups:
  $OP_print[$OP_count] = 'NAME(NAME)';
  push(@{$PrintGroup{$OP_print[$OP_count]}}, $OP_opcode[$OP_count]);
  print STDOUT "  $OP_print[$OP_count] \n";

  $scdclass = "ALU";
  $subset = "st220";
  # Only one machine resource for now, ISSUE
  # TOPs are issued at cycle 0
  $SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
  push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$OP_count]);
  print STDOUT " pushing $OP_opcode[$OP_count] ($subset,$scdclass)\n";

  $OP_count++;

  # ----------------- mov ------------------

  # beginning of a new recore
  printf (STDOUT " ----- new record %d ----\n", $OP_count);
  # reset OP_properties:
  $OP_properties[$OP_count] = $OP_NONE;
  # mnemonic:
  $OP_mnemonic[$OP_count] = 'mov';
  printf (STDOUT " mnemonic %s\n", $OP_mnemonic[$OP_count]);
  $OP_format[$OP_count] = 'Macro';
  printf (STDOUT " format %s\n", $OP_format[$OP_count]);
  $OP_variant[$OP_count] = "_r";
  $OP_opcode[$OP_count] = 
                  $OP_mnemonic[$OP_count].$OP_variant[$OP_count];
  printf (STDOUT " TOP = %s\n", $OP_opcode[$OP_count]);
  # subset:
  $OP_subset[$OP_count] = "st220";
  push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$OP_count]);
  # operand groups:
  $OP_signature[$OP_count] = 'dest:mov:src2';
  push(@{$SignatureGroup{$OP_signature[$OP_count]}}, $OP_opcode[$OP_count]);
  # print groups:
  $OP_print[$OP_count] = 'NAME_%R_=_%R(NAME_R_O)';
  push(@{$PrintGroup{$OP_print[$OP_count]}}, $OP_opcode[$OP_count]);
  print STDOUT "  $OP_print[$OP_count] \n";

  $scdclass = "ALU";
  $subset = "st220";
  # Only one machine resource for now, ISSUE
  # TOPs are issued at cycle 0
  $SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
  push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$OP_count]);
  print STDOUT " pushing $OP_opcode[$OP_count] ($subset,$scdclass)\n";

  $OP_count++;

  # ----------------- mov ------------------

  # beginning of a new recore
  printf (STDOUT " ----- new record %d ----\n", $OP_count);
  # reset OP_properties:
  $OP_properties[$OP_count] = $OP_NONE;
  # mnemonic:
  $OP_mnemonic[$OP_count] = 'mov';
  printf (STDOUT " mnemonic %s\n", $OP_mnemonic[$OP_count]);
  $OP_format[$OP_count] = 'Macro';
  printf (STDOUT " format %s\n", $OP_format[$OP_count]);
  $OP_variant[$OP_count] = "_i";
  $OP_opcode[$OP_count] = 
                  $OP_mnemonic[$OP_count].$OP_variant[$OP_count];
  printf (STDOUT " TOP = %s\n", $OP_opcode[$OP_count]);
  # subset:
  $OP_subset[$OP_count] = "st220";
  push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$OP_count]);
  # operand groups:
  $OP_signature[$OP_count] = 'idest:mov:isrc2';
  push(@{$SignatureGroup{$OP_signature[$OP_count]}}, $OP_opcode[$OP_count]);
  # print groups:
  $OP_print[$OP_count] = 'NAME_%R_=_%I(NAME_R_O)';
  push(@{$PrintGroup{$OP_print[$OP_count]}}, $OP_opcode[$OP_count]);
  print STDOUT "  $OP_print[$OP_count] \n";

  $scdclass = "ALU";
  $subset = "st220";
  # Only one machine resource for now, ISSUE
  # TOPs are issued at cycle 0
  $SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
  push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$OP_count]);
  print STDOUT " pushing $OP_opcode[$OP_count] ($subset,$scdclass)\n";

  $OP_count++;

  # ----------------- mtb ------------------

  # beginning of a new recore
  printf (STDOUT " ----- new record %d ----\n", $OP_count);
  # reset OP_properties:
  $OP_properties[$OP_count] = $OP_NONE;
  # mnemonic:
  $OP_mnemonic[$OP_count] = 'mtb';
  printf (STDOUT " mnemonic %s\n", $OP_mnemonic[$OP_count]);
  $OP_format[$OP_count] = 'Macro';
  printf (STDOUT " format %s\n", $OP_format[$OP_count]);
  $OP_variant[$OP_count] = "";
  $OP_opcode[$OP_count] = 
                  $OP_mnemonic[$OP_count].$OP_variant[$OP_count];
  printf (STDOUT " TOP = %s\n", $OP_opcode[$OP_count]);
  # subset:
  $OP_subset[$OP_count] = "st220";
  push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$OP_count]);
  # operand groups:
  $OP_signature[$OP_count] = 'bdest:mov:src1';
  push(@{$SignatureGroup{$OP_signature[$OP_count]}}, $OP_opcode[$OP_count]);
  # print groups:
  $OP_print[$OP_count] = 'NAME_%R_=_%R(NAME_R_O)';
  push(@{$PrintGroup{$OP_print[$OP_count]}}, $OP_opcode[$OP_count]);
  print STDOUT "  $OP_print[$OP_count] \n";

  $scdclass = "ALU";
  $subset = "st220";
  # Only one machine resource for now, ISSUE
  # TOPs are issued at cycle 0
  $SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
  push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$OP_count]);
  print STDOUT " pushing $OP_opcode[$OP_count] ($subset,$scdclass)\n";

  $OP_count++;

  # ----------------- mfb ------------------

  # beginning of a new recore
  printf (STDOUT " ----- new record %d ----\n", $OP_count);
  # reset OP_properties:
  $OP_properties[$OP_count] = $OP_NONE;
  # mnemonic:
  $OP_mnemonic[$OP_count] = 'mfb';
  printf (STDOUT " mnemonic %s\n", $OP_mnemonic[$OP_count]);
  $OP_format[$OP_count] = 'Macro';
  printf (STDOUT " format %s\n", $OP_format[$OP_count]);
  $OP_variant[$OP_count] = "";
  $OP_opcode[$OP_count] = 
                  $OP_mnemonic[$OP_count].$OP_variant[$OP_count];
  printf (STDOUT " TOP = %s\n", $OP_opcode[$OP_count]);
  # subset:
  $OP_subset[$OP_count] = "st220";
  push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$OP_count]);
  # operand groups:
  $OP_signature[$OP_count] = 'dest:mov:scond';
  push(@{$SignatureGroup{$OP_signature[$OP_count]}}, $OP_opcode[$OP_count]);
  # print groups:
  $OP_print[$OP_count] = 'NAME_%R_=_%R(NAME_R_O)';
  push(@{$PrintGroup{$OP_print[$OP_count]}}, $OP_opcode[$OP_count]);
  print STDOUT "  $OP_print[$OP_count] \n";

  $scdclass = "ALU";
  $subset = "st220";
  # Only one machine resource for now, ISSUE
  # TOPs are issued at cycle 0
  $SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
  push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$OP_count]);
  print STDOUT " pushing $OP_opcode[$OP_count] ($subset,$scdclass)\n";

  $OP_count++;


  # ----------------- return ------------------

  # beginning of a new recore
  printf (STDOUT " ----- new record %d ----\n", $OP_count);
  # reset OP_properties:
  $OP_properties[$OP_count] = $OP_NONE;
  # mnemonic:
  $OP_mnemonic[$OP_count] = 'return';
  printf (STDOUT " mnemonic %s\n", $OP_mnemonic[$OP_count]);
  $OP_format[$OP_count] = 'Macro';
  printf (STDOUT " format %s\n", $OP_format[$OP_count]);
  $OP_variant[$OP_count] = "";
  $OP_opcode[$OP_count] = 
                  $OP_mnemonic[$OP_count].$OP_variant[$OP_count];
  printf (STDOUT " TOP = %s\n", $OP_opcode[$OP_count]);
  #properties:
  push(@{$AttrGroup{'ijump'}}, $OP_opcode[$OP_count]);
  # subset:
  $OP_subset[$OP_count] = "st220";
  push(@{$SUBSET_opcodes[&subset_id("st220")]}, $OP_opcode[$OP_count]);
  # operand groups:
  $OP_signature[$OP_count] = ':return:lr';
  push(@{$SignatureGroup{$OP_signature[$OP_count]}}, $OP_opcode[$OP_count]);
  # print groups:
  $OP_print[$OP_count] = 'NAME_%R(NAME_O)';
  push(@{$PrintGroup{$OP_print[$OP_count]}}, $OP_opcode[$OP_count]);
  print STDOUT "  $OP_print[$OP_count] \n";

  $scdclass = "CALL";
  $subset = "st220";
  # Only one machine resource for now, ISSUE
  # TOPs are issued at cycle 0
  $SUBSET_scd{$subset}{$scdclass}{'res'}{'ISSUE'} = 0;
  push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$OP_count]);
  print STDOUT " pushing $OP_opcode[$OP_count] ($subset,$scdclass)\n";

  $OP_count++;


  # The Pro64 compiler needs a number of simulated and dummy 
  # opcodes. They get added after all the real ISA opcodes have
  # been sorted because simulated/dummy do not need to be.
  &initialize_required_opcodes ();

  # Emit ISA information
  for ($opcode = 1; $opcode < $OP_count; $opcode++) {
      &emit_opcode ($opcode);
  }

  # Emit the information for all opcodes including simulated/dummy:
  &emit_registers();
  &emit_literals();
  &emit_enums();
  &emit_subsets ();
  &emit_properties ();
  &emit_operands ();
  &emit_printing_formats ();

  # scheduling info:
  &emit_scdinfo ();
  &emit_bundle_info ();

  # these are completely dummy, not used for now in the compiler:
  &emit_pack_info ();
  &emit_decode_info ();

  # target info:
  &emit_op_info();

  # end of generation:
  &finalize_isa_file ();
  &finalize_reg_file ();
  &finalize_lit_file ();
  &finalize_enum_file ();
  &finalize_si_file ();
  &finalize_operands_file ();
  &finalize_subset_file ();
  &finalize_properties_file ();
  &finalize_print_file ();
  &finalize_pack_file ();
  &finalize_bundle_file ();
  &finalize_decode_file ();

  &finalize_op_file();

  close(OPCODE_F);
  close(FORMAT_F);

