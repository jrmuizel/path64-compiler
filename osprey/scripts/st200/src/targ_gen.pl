#!/usr/bin/perl -w


####################################################################
#
#  Generate target description files for the ST200 processor
#
#  Following CHESS files are required:
#
#    st220/opc.db - describes TOPs, operands, printing format
#    st220/arc.db - describes scheduling latencies, resources
#
####################################################################


use strict;

####################################################################
#                         GLOBAL VARIABLES
####################################################################

my $TRUE = 1;
my $FALSE = 0;
my $UNDEF = -1;

my $SIGNED = "SIGNED";
my $UNSIGNED = "UNSIGNED";
my $PCREL = "PCREL";

# global ISA subset count
my $SUBSET_count = 0;
my @SUBSET_opcodes;
my @SUBSET_parent;
my @SUBSET_name;
my %SUBSET_scd;

# global ISA properties count
my $ISA_PROPERTY_count = 0;
my @ISA_PROPERTY_name;

# RCLASS properties
my $RCLASS_count = 0;
my @RCLASS_name;
my @RCLASS_can_be_stored;
my @RCLASS_mult_store;
my @RCLASS_is_ptr;
my @RCLASS_bits;
my $PTR_TYPE;


# REGSET properties
my $REGSET_count = 0;
my @REGSET_rclass;
my @REGSET_minreg;
my @REGSET_maxreg;
my @REGSET_format;
my @REGSET_names;
my @REGSET_subset;
my @REGSET_subset_count;

# LCLASS properties
my $LCLASS_count = 0;
my @LCLASS_rtype;
my @LCLASS_name;
my @LCLASS_bits;

# ECLASS properties
my $ECLASS_count = 0;
my @ECLASS_name;
my @ECLASS_elt_count;
my @ECLASS_elt;
my @ECLASS_bits;

# REG_OPND properties
my $REG_OPND_count = 0;
my @REG_OPND_name;
my @REG_OPND_rclass;
my @REG_OPND_size;
my @REG_OPND_subclass;
my @REG_OPND_rtype;
my @REG_OPND_fp_type;

# LIR_OPND properties
my $LIT_OPND_count = 0;
my @LIT_OPND_name;
my @LIT_OPND_size;
my @LIT_OPND_rtype;
my @LIT_OPND_lclass;

# ENUM_OPND properties
my $ENUM_OPND_count = 0;
my @ENUM_OPND_name;
my @ENUM_OPND_size;
my @ENUM_OPND_rtype;
my @ENUM_OPND_eclass;

# OPND_USE properties
my $OPND_USE_count = 0;
my @OPND_USE_name;


# global scheduling resource count
my $RES_count = 0;
my @RES_name;
my @RES_avail;

# global SCD_CLASS count
my $SCD_CLASS_count = 0;
my @SCD_CLASS_name;
my @SCD_CLASS_results;
my @SCD_CLASS_opnds;
my @SCD_CLASS_result;
my @SCD_CLASS_opnd;

# global EXEC_SLOT count
my $EXEC_SLOT_count = 0;
my @EXEC_SLOT_name;
my @EXEC_SLOT_scds;

# global BUNDLE count -- can only be 1 for now
my $BUNDLE_count = 0;
my @BUNDLE_name;
my @BUNDLE_field;
my @BUNDLE_slots;
my @BUNDLE_slot;
my @BUNDLE_bits;
my @BUNDLE_temps;
my @BUNDLE_temp;

# global packing groups count (instruction binary compression)
my $PACK_count = 0;
my @PACK_comment;
my @PACK_type;
my @PACK_words;
my @PACK_result;
my @PACK_word_results;
my @PACK_word_opnds;
my @PACK_result_ipos;
my @PACK_result_bits;
my @PACK_results;
my @PACK_opnds;
my @PACK_opnd_start;
my @PACK_opnd_ipos;
my @PACK_opnd_bits;
my @PACK_result_start;

my $ARCH_name;

# Temporary global variables
my $cur_regset;
my $cur_eclass;
my $current_pack;
my $cur_bundle;
my $cur_temp;

####################################################################
#  Operation properties -- 32 bits can map 32 properties:
####################################################################

my $OP_NONE        = 0x00000000;
my $OP_SEXT        = 0x00000001;
my $OP_SIMULATED   = 0x00000002;  # simulated TOP - expends into a seq.
my $OP_DUMMY       = 0x00000004;  # dummy TOP - do not get emitted
my $OP_ZEXT        = 0x00000008;
my $OP_LOAD        = 0x00000010;  
my $OP_STORE       = 0x00000020;
my $OP_BRANCH      = 0x00000040;
my $OP_JUMP        = 0x00000080;
my $OP_IJUMP       = 0x00000100;
my $OP_CALL        = 0x00000200;
my $OP_DISMISSIBLE = 0x00000800;
my $OP_NOOP        = 0x00001000;
my $OP_IMMEDIATE   = 0x00002000;
my $OP_XFER        = 0x00004000;
my $OP_MOVE        = 0x00008000;
my $OP_NOP         = 0x00010000;
my $OP_PREFETCH    = 0x00020000;
my $OP_MUL         = 0x00040000;
my $OP_ADD         = 0x00080000;
my $OP_SUB         = 0x00100000;
my $OP_OR          = 0x00200000;
my $OP_SELECT      = 0x00400000;
my $OP_UNSIGNED    = 0x00800000;
my $OP_INTOP       = 0x01000000;
my $OP_CMP         = 0x02000000;
my $OP_AND         = 0x04000000;
my $OP_XOR         = 0x08000000;
my $OP_SHL         = 0x10000000;
my $OP_SHR         = 0x20000000;
my $OP_SHRU        = 0x40000000;


my @SimulatedOpcodes;
my @DummyOpcodes;
my @SSAOpcodes;

# global TOPs count
my $OP_count = 0;
my @OP_opcode;
my @OP_mnemonic;
my @OP_format;
my @OP_opnds;
my @OP_results;
my @OP_variant;
my @OP_signature;
my @OP_pack;
my @OP_syntax;
my @OP_print;
my @OP_res;
my @OP_opnd;
my @OP_properties;
my @OP_scdclass;
my @OP_opnd_access_time;
my @OP_result_avail_time;
my @OP_subset;
my @OP_bytes;
my @OP_align;

my %SignatureGroup;
my %PackGroup;
my %AttrGroup;
my %PrintGroup;
my %MemBytes;
my %MemAlign;

# Files
my $ISA_F;
my $REG_F;
my $LIT_F;
my $ENUM_F;
my $SUBS_F;
my $PR_F;
my $OPND_F;
my $PRNT_F;
my $PACK_F;
my $DECODE_F;
my $BUNDLE_F;
my $OP_F;
my $SI_F;

# ==================================================================
#    opcode_is_
#
#    check opcode belonging to some groups
# ==================================================================
sub opcode_is_ssa {
    my $opcode;
    my $gname = $_[0];

    foreach $opcode (@SSAOpcodes) {
	if ($gname eq $opcode) {
	    return 1;
	}
    }

    return 0;
}

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

sub opcode_is_subset {
    my $opcode = $_[0];
    my $subset = subset_id($_[1]);

    foreach my $op (@{$SUBSET_opcodes[$subset]}) {
	if ($OP_opcode[$op] eq $OP_opcode[$opcode]) {
	    return 1;
	}
    }

    return 0;
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
    elsif ($mnemonic eq 'prginspg' ||
	   $mnemonic eq 'pswclr' ||
	   $mnemonic eq 'pswset') {
	$bytes = 0;
    }
    else {
	printf STDOUT "ERROR: unknown mnemonic %s in op_bytes\n", $mnemonic;
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

#    printf STDOUT "  > align %s\n", $mnemonic;

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
    elsif ($mnemonic eq 'prginspg' ||
	   $mnemonic eq 'pswclr' ||
	   $mnemonic eq 'pswset') {
	$align = 0;
    }
    else {
	printf STDOUT "ERROR: unknown mnemonic %s in op_align\n", $mnemonic;
	exit(1);
    }

    return $align;
}

# ==================================================================
#    set_variant
# ==================================================================

sub set_variant {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];
    my $variant;

    if ($format eq 'Int3R') {
	$variant = '_r';
    }
    elsif ($format eq 'Int3I') {
	$variant = '_i';
    }
    elsif ($format eq 'Int3E') {
	$variant = '_ii';
    }
    elsif ($format eq 'Mul64R') {
	$variant = '_r';
    }
    elsif ($format eq 'Mul64I') {
	$variant = '_i';
    }
    elsif ($format eq 'Mul64E') {
	$variant = '_ii';
    }
    elsif ($format eq 'Cmp3R_Reg') {
	$variant = '_r_r';
    }
    elsif ($format eq 'Cmp3I_Reg') {
	$variant = '_i_r';
    }
    elsif ($format eq 'Cmp3E_Reg') {
	$variant = '_ii_r';
    }
    elsif ($format eq 'Cmp3R_Br') {
	$variant = '_r_b';
    }
    elsif ($format eq 'Cmp3I_Br') {
	$variant = '_i_b';
    }
    elsif ($format eq 'Cmp3E_Br') {
	$variant = '_ii_b';
    }
    elsif ($format eq 'SelectR') {
	$variant = '_r';
    }
    elsif ($format eq 'SelectI') {
	$variant = '_i';
    }
    elsif ($format eq 'SelectE') {
	$variant = '_ii';
    }
    elsif ($format eq 'Load' ||
	   $format eq 'Store') {
	$variant = "_i";
    }
    elsif ($format eq 'LoadE' ||
	   $format eq 'StoreE') {
	$variant = "_ii";
    }
    elsif ($format eq 'Jump' ||
	   $format eq 'Branch' ||
	   $format eq 'Call' ||
	   $format eq 'Ijump' ||
	   $format eq 'cgen' ||
	   $format eq 'Imm' ||
	   $format eq 'SBreak' ||
	   $format eq 'SysOp') {
	$variant = "";
    }
    elsif ($format eq 'MoveR') {
	$variant = "_r";
    }
    elsif ($format eq 'MoveI') {
	$variant = "_i";
    }
    elsif ($format eq 'MoveE') {
	$variant = "_ii";
    }
    elsif ($format eq 'Sync') {
	$variant = "";
    }
    elsif ($format eq 'mtb') {
	$variant = "";
    }
    elsif ($format eq 'mfb') {
	$variant = "";
    }
    elsif ($format eq 'Asm') {
	$variant = "";
    }
    elsif ($format eq 'Noop') {
	$variant = "";
    }
    elsif ($format eq 'Monadic') {
	$variant = "_r";
    }
    else {
	printf STDOUT "ERROR: unknown format %s in set_variant\n", $format;
	exit(1);
    }

    $OP_variant[$opcode] = $variant;
    return;
}

# ==================================================================
#    set_operands
# ==================================================================
sub set_operands {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];

    if ($format eq 'Int3R' ||
	$format eq 'Int3I' ||
	$format eq 'Int3E' ||
	$format eq 'Mul64R' ||
	$format eq 'Mul64I' ||
	$format eq 'Mul64E' ||
	$format eq 'Cmp3R_Reg' ||
	$format eq 'Cmp3R_Br' ||
	$format eq 'Cmp3I_Reg' ||
	$format eq 'Cmp3E_Reg' ||
	$format eq 'Cmp3I_Br' ||
	$format eq 'Cmp3E_Br') {
	$OP_opnds[$opcode] = 2;
	$OP_results[$opcode] = 1;
    }
    elsif ($format eq 'Monadic') {
	$OP_opnds[$opcode] = 1;
	$OP_results[$opcode] = 1;
    }
    elsif ($format eq 'SelectR' ||
	   $format eq 'SelectI' ||
	   $format eq 'SelectE') {

	$OP_results[$opcode] = 1;
	$OP_opnds[$opcode] = 3;
    }
    elsif ($format eq 'cgen') {

	$OP_results[$opcode] = 2;
	$OP_opnds[$opcode] = 3;
    }
    elsif ($format eq 'Imm') {

	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 1;
    }
    elsif ($format eq 'Load' ||
	   $format eq 'LoadE') {

	$OP_results[$opcode] = 1;
	$OP_opnds[$opcode] = 2;
    }
    elsif ($format eq 'Store' ||
	   $format eq 'StoreE') {

	$OP_results[$opcode] = 0;
	if ($OP_mnemonic[$opcode] eq 'pft' ||
	    $OP_mnemonic[$opcode] eq 'prgset' ||
	    $OP_mnemonic[$opcode] eq 'prginspg' ||
	    $OP_mnemonic[$opcode] eq 'prgadd') {
	    $OP_opnds[$opcode] = 2;
	}
	elsif ($OP_mnemonic[$opcode] eq 'pswclr' ||
	       $OP_mnemonic[$opcode] eq 'pswset') {
	    $OP_opnds[$opcode] = 0;
	}
	else {
	    $OP_opnds[$opcode] = 3;
	}
    }
    elsif ($format eq 'Branch') {

	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 2;
    }
    elsif ($format eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'call' ||
	    $OP_mnemonic[$opcode] eq 'icall') {

	    $OP_results[$opcode] = 1;
	    $OP_opnds[$opcode] = 1;
	}
	elsif ($OP_mnemonic[$opcode] eq 'rfi') {
	    $OP_results[$opcode] = 0;
	    $OP_opnds[$opcode] = 0;
	}
	else {
	    printf STDOUT "ERROR: unknown call format %s in set_operands\n", $OP_mnemonic[$opcode];
	    exit(1);
	}
    }
    elsif ($format eq 'Jump') {
	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 1;
    }
    elsif ($format eq 'Ijump') {
	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 1;
    }
    elsif ($format eq 'SysOp' ||
	   $format eq 'SBreak') {
	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 0;
    }
    elsif ($format eq 'MoveR' ||
	   $format eq 'MoveI' ||
	   $format eq 'MoveE') {
	$OP_results[$opcode] = 1;
	$OP_opnds[$opcode] = 1;
    }
    elsif ($format eq 'Sync') {
	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 0;
    }
    elsif ($format eq 'mtb') {
	$OP_results[$opcode] = 1;
	$OP_opnds[$opcode] = 1;
    }
    elsif ($format eq 'mfb') {
	$OP_results[$opcode] = 1;
	$OP_opnds[$opcode] = 1;
    }
    elsif ($format eq 'Asm') {
	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 0;
    }
    elsif ($format eq 'Noop') {
	$OP_results[$opcode] = 0;
	$OP_opnds[$opcode] = 0;
    }
    else {
	printf STDOUT "ERROR: unknown format %s in set_operands\n", $format;
	exit(1);
    }
}

# ==================================================================
#    set_signature
# ==================================================================

sub set_signature {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];
    my $signature;

    if ($format eq 'Int3R') {
	if ($OP_mnemonic[$opcode] eq 'mull' ||
	    $OP_mnemonic[$opcode] eq 'mullu' ||
	    $OP_mnemonic[$opcode] eq 'mulh' ||
	    $OP_mnemonic[$opcode] eq 'mulhu' ||
	    $OP_mnemonic[$opcode] eq 'mulll' ||
	    $OP_mnemonic[$opcode] eq 'mullu' ||
	    $OP_mnemonic[$opcode] eq 'mullh' ||
	    $OP_mnemonic[$opcode] eq 'mullhu' ||
	    $OP_mnemonic[$opcode] eq 'mulhh' ||
	    $OP_mnemonic[$opcode] eq 'mulhhu' ||
	    $OP_mnemonic[$opcode] eq 'mulhs' ||
	    $OP_mnemonic[$opcode] eq 'mulhhs' ||
	    $OP_mnemonic[$opcode] eq 'mulllu' ||
	    $OP_mnemonic[$opcode] eq 'mullhus') {
	    $signature = 'dest2:Int3R:src1_opnd1,src2_opnd2';
	} else {
	    $signature = 'dest:Int3R:src1_opnd1,src2_opnd2';
	}
    }
    elsif ($format eq 'Int3I' ||
	   $format eq 'Int3E') {
	# special case of sub_i:
	if ($OP_mnemonic[$opcode] eq 'sub') {
	    $signature = 'idest:Int3I:isrc2_opnd1,src1_opnd2';
	} elsif ($OP_mnemonic[$opcode] eq 'mull' ||
		 $OP_mnemonic[$opcode] eq 'mullu' ||
		 $OP_mnemonic[$opcode] eq 'mulh' ||
		 $OP_mnemonic[$opcode] eq 'mulhu' ||
		 $OP_mnemonic[$opcode] eq 'mulll' ||
		 $OP_mnemonic[$opcode] eq 'mullu' ||
		 $OP_mnemonic[$opcode] eq 'mullh' ||
		 $OP_mnemonic[$opcode] eq 'mullhu' ||
		 $OP_mnemonic[$opcode] eq 'mulhh' ||
		 $OP_mnemonic[$opcode] eq 'mulhhu' ||
		 $OP_mnemonic[$opcode] eq 'mulhs' ||
		 $OP_mnemonic[$opcode] eq 'mulhhs' ||
		 $OP_mnemonic[$opcode] eq 'mulllu' ||
		 $OP_mnemonic[$opcode] eq 'mullhus') {
	    $signature = 'idest2:Int3I:src1_opnd1,isrc2_opnd2';
	} else {
	    $signature = 'idest:Int3I:src1_opnd1,isrc2_opnd2';
	}
    }
    elsif ($format eq 'Mul64R') {
      $signature = 'dest2:Mul64R:src1_opnd1,src2_opnd2';
    }
    elsif ($format eq 'Mul64I' ||
	   $format eq 'Mul64E') {
      $signature = 'idest2:Mul64I:src1_opnd1,isrc2_opnd2';
    }
    elsif ($format eq 'Cmp3R_Reg') {
	$signature = 'dest:Cmp3R_Reg:src1_opnd1,src2_opnd2';
    }
    elsif ($format eq 'Cmp3I_Reg' ||
	   $format eq 'Cmp3E_Reg') {
	$signature = 'idest:Cmp3I_Reg:src1_opnd1,isrc2_opnd2';
    }
    elsif ($format eq 'Cmp3R_Br') {
	$signature = 'bdest:Cmp3R_Br:src1_opnd1,src2_opnd2';
    }
    elsif ($format eq 'Cmp3I_Br' ||
	   $format eq 'Cmp3E_Br') {
	$signature = 'ibdest:Cmp3I_Br:src1_opnd1,isrc2_opnd2';
    }
    elsif ($format eq 'SelectR') {
	$signature = 'dest:SelectR:bcond_condition,src1_opnd1,src2_opnd2';
    }
    elsif ($format eq 'SelectI' ||
	   $format eq 'SelectE') {
	$signature = 'idest:SelectI:bcond_condition,src1_opnd1,isrc2_opnd2';
    }
    elsif ($format eq 'cgen') {
	$signature = 'dest,bdest:cgen:src1,src2,bcond';
    }
    elsif ($format eq 'Imm') {
	$signature = ':Imm:imm';
    }
    elsif ($format eq 'Load' ||
	   $format eq 'LoadE') {
	if ($OP_mnemonic[$opcode] eq 'ldw_d' ||
	    $OP_mnemonic[$opcode] eq 'ldw') {
	    $signature = 'dest:Load:isrc2_offset,src1_base';
	} else {
	    $signature = 'dest2:Load:isrc2_offset,src1_base';
	}
    }
    elsif ($format eq 'Store' ||
	   $format eq 'StoreE') {
	if ($OP_mnemonic[$opcode] eq 'pft' ||
	    $OP_mnemonic[$opcode] eq 'prgset' ||
	    $OP_mnemonic[$opcode] eq 'prgadd') {
	    $signature = ':cache:isrc2_offset,src1_base';
	}
	else {
	    $signature = ':Store:isrc2_offset,src1_base,src2_storeval';
	}
    }
    elsif ($format eq 'Branch') {
	$signature = ':Branch:bcond_condition,btarg_target';
    }
    elsif ($format eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'call') {
	    $signature = 'lr:call:btarg';
	}
	elsif ($OP_mnemonic[$opcode] eq 'icall') {
	    $signature = 'lr:icall:lr';
	}
	elsif ($OP_mnemonic[$opcode] eq 'rfi') {
	    $signature = ':rfi:';
	}
	else {
	    printf STDOUT "ERROR: unknown call format %s in op_signature\n", $OP_mnemonic[$opcode];
	    exit(1);
	}
    }
    elsif ($format eq 'Jump') {
	$signature = ':jump:btarg_target';
    }
    elsif ($format eq 'Ijump') {
	$signature = ':ijump:lr';
    }
    elsif ($format eq 'SysOp' || $format eq 'SBreak') {
	$signature = ':SysOp:';
    }
    elsif ($format eq 'MoveR') {
	$signature = 'dest:move:src2_opnd1';
    }
    elsif ($format eq 'MoveI' ||
	   $format eq 'MoveE') {
	$signature = 'idest:move:isrc2_opnd1';
    }
    elsif ($format eq 'Sync') {
	$signature = ':Sync:';
    }
    elsif ($format eq 'mtb') {
	$signature = 'bdest:mtb:src1';
    }
    elsif ($format eq 'mfb') {
	$signature = 'idest:mfb:scond';
    }
    elsif ($format eq 'Asm') {
	$signature = ':asm:';
    }
    elsif ($format eq 'Noop') {
	$signature = ':nop:';
    }
    elsif ($format eq 'Monadic') {
	$signature = 'dest:Monadic:src2_opnd1';
    }
    else {
	printf STDOUT "ERROR: unknown format %s in op_signature\n", $format;
	exit(1);
    }

    $OP_signature[$opcode] = $signature;
    return;
}

# ==================================================================
#    set_pack
# ==================================================================

sub set_pack {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];
    my $pack;

    if ($format eq 'Int3R') {
	$pack = 'Int3R';
    }
    elsif ($format eq 'Int3I') {
	# special case of sub_i:
	if ($OP_mnemonic[$opcode] eq 'sub') {
	    $pack = 'sub_i';
	}
	else {
	    $pack = 'Int3I';
	}
    }
    elsif ($format eq 'Int3E') {
	# special case of sub_ii:
	if ($OP_mnemonic[$opcode] eq 'sub') {
	    $pack = 'sub_ii';
	}
	else {
	    $pack = 'Int3E';
	}
    }
    elsif ($format eq 'Mul64R') {
	$pack = 'Mul64R';
    }
    elsif ($format eq 'Mul64I') {
	$pack = 'Mul64I';
    }
    elsif ($format eq 'Mul64E') {
	$pack = 'Mul64E';
    }
    elsif ($format eq 'Cmp3R_Reg') {
	$pack = 'Cmp3R_Reg';
    }
    elsif ($format eq 'Cmp3I_Reg') {
	$pack = 'Cmp3I_Reg';
    }
    elsif ($format eq 'Cmp3E_Reg') {
	$pack = 'Cmp3E_Reg';
    }
    elsif ($format eq 'Cmp3R_Br') {
	$pack = 'Cmp3R_Br';
    }
    elsif ($format eq 'Cmp3I_Br') {
	$pack = 'Cmp3I_Br';
    }
    elsif ($format eq 'Cmp3E_Br') {
	$pack = 'Cmp3E_Br';
    }
    elsif ($format eq 'SelectR') {
	$pack = 'SelectR';
    }
    elsif ($format eq 'SelectI') {
	$pack = 'SelectI';
    }
    elsif ($format eq 'SelectE') {
	$pack = 'SelectE';
    }
    elsif ($format eq 'cgen') {
	$pack = 'cgen';
    }
    elsif ($format eq 'Imm') {
	$pack = 'Imm';
    }
    elsif ($format eq 'Load') {
	$pack = 'Load';
    }
    elsif ($format eq 'LoadE') {
	$pack = 'LoadE';
    }
    elsif ($format eq 'Store') {
	if ($OP_mnemonic[$opcode] eq 'pft' ||
	    $OP_mnemonic[$opcode] eq 'prgset' ||
	    $OP_mnemonic[$opcode] eq 'prgadd') {
	    $pack = 'Cache';
	}
	else {
	    $pack = 'Store';
	}
    }
    elsif ($format eq 'StoreE') {
	if ($OP_mnemonic[$opcode] eq 'pft' ||
	    $OP_mnemonic[$opcode] eq 'prgset' ||
	    $OP_mnemonic[$opcode] eq 'prgadd') {
	    $pack = 'CacheE';
	}
	else {
	    $pack = 'StoreE';
	}
    }
    elsif ($format eq 'Branch') {
	$pack = 'Branch';
    }
    elsif ($format eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'call') {
	    $pack = 'Call';
	}
	elsif ($OP_mnemonic[$opcode] eq 'icall') {
	    $pack = 'Icall';
	}
	elsif ($OP_mnemonic[$opcode] eq 'rfi') {
	    $pack = 'rfi';
	}
	else {
	    printf STDOUT "ERROR: unknown call opcode %s in op_pack\n", $OP_mnemonic[$opcode];
	    exit(1);
	}
    }
    elsif ($format eq 'Jump') {
	$pack = 'Jump';
    }
    elsif ($format eq 'Ijump') {
	$pack = 'Ijump';
    }
    elsif ($format eq 'SysOp' || $format eq 'SBreak') {
	$pack = 'SysOp';
    }
    elsif ($format eq 'MoveR') {
	$pack = 'MoveR';
    }
    elsif ($format eq 'MoveI') {
	$pack = 'MoveI';
    }
    elsif ($format eq 'MoveE') {
	$pack = 'MoveE';
    }
    elsif ($format eq 'Sync') {
	$pack = 'Sync';
    }
    elsif ($format eq 'mtb') {
	$pack = 'mtb';
    }
    elsif ($format eq 'mfb') {
	$pack = 'mfb';
    }
    elsif ($format eq 'Asm') {
	$pack = 'asm';
    }
    elsif ($format eq 'Noop') {
	$pack = 'nop';
    }
    elsif ($format eq 'Monadic') {
	$pack = 'Monadic';
    }
    else {
	printf STDOUT "ERROR: unknown format %s in op_pack\n", $format;
	exit(1);
    }

    $OP_pack[$opcode] = $pack;
    return;
}

# ==================================================================
#    set_print
#
#    make a printing signature similar to:
#
#          %R?_NAME_%R_,_%R_,_%R(O_NAME_R_O_O) for each TOP
# ==================================================================
sub set_print {
    my $opcode = $_[0];
    my $format = $OP_format[$opcode];
    my $syntax = $OP_syntax[$opcode];
    my $signature = "";

    if ($format eq 'Int3R' || 
	$format eq 'Int3I' ||
	$format eq 'Int3E') {
	# special case of sub_i:
	if ($OP_mnemonic[$opcode] eq 'sub') {
	    $OP_res[$opcode][0]{'fmt'} = '%s';
	    $OP_opnd[$opcode][0]{'fmt'} = '%s';
	    $OP_opnd[$opcode][1]{'fmt'} = '%s';
	    $signature = "R0:%s_=_O0:%s_,_O1:%s";
	    $OP_print[$opcode] = $signature;
	    return;
	}
	else {
	    $OP_res[$opcode][0]{'fmt'} = '%s';
	    $OP_opnd[$opcode][0]{'fmt'} = '%s';
	    $OP_opnd[$opcode][1]{'fmt'} = '%s';
	}
    }
    elsif ($format eq 'Mul64R' || 
	$format eq 'Mul64I' ||
	$format eq 'Mul64E') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
    }
    elsif ($format eq 'Cmp3R_Reg' ||
	   $format eq 'Cmp3R_Br') {

	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
    }
    elsif ($format eq 'Cmp3I_Reg' ||
	   $format eq 'Cmp3E_Reg' ||
	   $format eq 'Cmp3I_Br' ||
	   $format eq 'Cmp3E_Br') {

	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%d';
    }
    elsif ($format eq 'SelectR') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
	$OP_opnd[$opcode][2]{'fmt'} = '%s';
    }
    elsif ($format eq 'SelectI' ||
	   $format eq 'SelectE') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
	$OP_opnd[$opcode][2]{'fmt'} = '%d';
    }
    elsif ($format eq 'cgen') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_res[$opcode][1]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
	$OP_opnd[$opcode][2]{'fmt'} = '%s';
    }
    elsif ($format eq 'Imm') {
	$OP_opnd[$opcode][0]{'fmt'} = '%d';
    }
    elsif ($format eq 'Load' ||
	   $format eq 'LoadE') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%d';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
    }
    elsif ($format eq 'Store' ||
	   $format eq 'StoreE') {
	$OP_opnd[$opcode][0]{'fmt'} = '%d';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
	unless ($OP_mnemonic[$opcode] eq 'pft' ||
		$OP_mnemonic[$opcode] eq 'prgset' ||
		$OP_mnemonic[$opcode] eq 'prgadd') {
	    $OP_opnd[$opcode][2]{'fmt'} = '%s';
	}
    }
    elsif ($format eq 'Branch') {
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][1]{'fmt'} = '%s';
    }
    elsif ($format eq 'Call') {
	if ($OP_mnemonic[$opcode] eq 'call' ||
	       $OP_mnemonic[$opcode] eq 'icall') {
	    $OP_res[$opcode][0]{'fmt'} = '%s';
	    $OP_opnd[$opcode][0]{'fmt'} = '%s';
	    $signature = "R0:%s_=_O0:%s";
#	    print STDOUT " syntax: $syntax \n";
#	    print STDOUT " signature: $signature \n";
	}
	elsif ($OP_mnemonic[$opcode] eq 'rfi') {
	    $signature = "";
	}
	else {
	    printf STDOUT "ERROR: unknown call opcode %s in set_print\n", $OP_mnemonic[$opcode];
	    exit(1);
	}

	$OP_print[$opcode] = $signature;
	return;
    }
    elsif ($format eq 'Jump' ||
	   $format eq 'Ijump') {
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
	$signature = "O0:%s";
	$OP_print[$opcode] = $signature;
	return;
    }
    elsif ($format eq 'SysOp' || $format eq 'SBreak') {
    }
    elsif ($format eq 'MoveR' ||
	   $format eq 'MoveI' ||
	   $format eq 'MoveE') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
    }
    elsif ($format eq 'Sync') {
    }
    elsif ($format eq 'mtb') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
    }
    elsif ($format eq 'mfb') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
    }
    elsif ($format eq 'Asm') {
    }
    elsif ($format eq 'Noop') {
    }
    elsif ($format eq 'Monadic') {
	$OP_res[$opcode][0]{'fmt'} = '%s';
	$OP_opnd[$opcode][0]{'fmt'} = '%s';
    }
    else {
	printf STDOUT "ERROR: unknown format %s in set_print\n", $format;
	exit(1);
    }

    # process syntax:
#    print STDOUT " syntax: $syntax \n";
    my @st = split(' ',$syntax);
#    print STDOUT " list: @st \n";

    my $count = 0;
    foreach my $elt (@st) {
	my $idx;
	my $fmt;

#	print STDOUT "   elt: $elt \n";

        if ($elt =~ /%(\d),/) {
	    if ($count < $OP_results[$opcode]) {
		$idx = $1 - 1;
		$fmt = $OP_res[$opcode][$idx]{'fmt'};
		$signature = $signature."R".$idx.":".$fmt
	    }
	    else {
		$idx = $1 - $OP_results[$opcode] - 1;
		$fmt = $OP_opnd[$opcode][$idx]{'fmt'};
		$signature = $signature."O".$idx.":".$fmt;
	    }
	    $signature = $signature."_,_";
	    $count++;
	}
	elsif ($elt =~ /%(\d)\[%(\d)]/) {
	    # these must be operands
	    my $idx1 = $1 - $OP_results[$opcode] - 1;
	    my $idx2 = $2 - $OP_results[$opcode] - 1;
	    my $fmt1 = $OP_opnd[$opcode][$idx1]{'fmt'};
	    my $fmt2 = $OP_opnd[$opcode][$idx2]{'fmt'};
	    $signature = $signature."O".$idx1.":".$fmt1."_[_"."O".$idx2.":".$fmt2."_]_";
	    $count = $count + 2;
	}
        elsif ($elt =~ /%(\d)/) {
	    if ($count < $OP_results[$opcode]) {
		$idx = $1 - 1;
		$fmt = $OP_res[$opcode][$idx]{'fmt'};
		$signature = $signature."R".$idx.":".$fmt
	    }
	    else {
		$idx = $1 - $OP_results[$opcode] - 1;
		$fmt = $OP_opnd[$opcode][$idx]{'fmt'};
#		print STDOUT "   opformat: $fmt \n";
		$signature = $signature."O".$idx.":".$fmt
	    }
	    $count++;
	}
	elsif ($elt eq '[') {
	    $signature = $signature.'_[_';
	}
	elsif ($elt eq ']') {
	    $signature = $signature.'_]';
	}
	elsif ($elt eq 'LR') {
	    printf STDOUT "ERROR: LR in set_print\n", $format;
	    exit(1);
	}
	elsif ($elt eq '=') {
	    $signature = $signature.'_=_';
	}
	else {
	    # must be the opcode
	    $signature = $signature;
	}
    }

#    print STDOUT " signature: $signature  \n";

    $OP_print[$opcode] = $signature;
    return;
}

# for each subset
# if opcode exists in parent, same properties, same sched infos. Remove it.
sub update_subsets {

    for (my $i = $SUBSET_count-1; $i>0; $i--) {
	my $parent = $SUBSET_parent[$i];
	foreach my $opcode (@{$SUBSET_opcodes[$i]}) {
	    foreach my $opcode2 (@{$SUBSET_opcodes[subset_id($parent)]}) {
		if ($OP_opcode[$opcode] eq $OP_opcode[$opcode2]) {
		    foreach my $signature (keys(%SignatureGroup)) {
			my $found=$FALSE;
			foreach my $op (@{$SignatureGroup{$signature}}) {
			    if ($op eq $OP_opcode[$opcode2]) {
				if ($found eq $TRUE) {
				    $op = $UNDEF;
				}
				$found = $TRUE;
			    }
			}
		    }

		    foreach my $pack (keys(%PackGroup)) {
			my $found=$FALSE;
			foreach my $op (@{$PackGroup{$pack}}) {
			    if ($op eq $OP_opcode[$opcode2]) {
				if ($found eq $TRUE) {
				    $op = $UNDEF;
				}
				$found = $TRUE;
			    }
			}
		    }

		    foreach my $attr (keys(%AttrGroup)) {
			my $found=$FALSE;
			foreach my $op (@{$AttrGroup{$attr}}) {
			    if ($op eq $OP_opcode[$opcode2]) {
				if ($found eq $TRUE) {
				    $op = $UNDEF;
				} 
				$found = $TRUE;
			    }
			}
		    }

		    foreach my $print (keys(%PrintGroup)) {
			my $found=$FALSE;
			foreach my $op (@{$PrintGroup{$print}}) {
			    if ($op eq $OP_opcode[$opcode2]) {
				if ($found eq $TRUE) {
				    $op = $UNDEF;
				} 
				$found = $TRUE;
			    }
			}
		    }

		    foreach my $mem (keys(%MemBytes)) {
			my $found=$FALSE;
			foreach my $op (@{$MemBytes{$mem}}) {
			    if ($op eq $OP_opcode[$opcode2]) {
				if ($found eq $TRUE) {
				    $op = $UNDEF;
				} 
				$found = $TRUE;
			    }
			}
		    }

		    foreach my $mem (keys(%MemAlign)) {
			my $found=$FALSE;
			foreach my $op (@{$MemAlign{$mem}}) {
			    if ($op eq $OP_opcode[$opcode2]) {
				if ($found eq $TRUE) {
				    $op = $UNDEF;
				} 
				$found = $TRUE;
			    }
			}
		    }

		    $OP_opcode[$opcode2] = $UNDEF;
		    $opcode2 = $UNDEF;
		}
	    }
	}
    }
}

# ==================================================================
#    initialize_required_opcodes
#
#    The Pro64 back end requires a number of Opcodes to be defined:
#
#    // Simulated instructions: these may be translated into
#    // a number of target instructions, and also may depend
#    // on an execution unit, eg. noop on IA64.
#      "asm",                 /* a GNU ASM statement */
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

    my $opcode;

    push (@SimulatedOpcodes, "asm");
    push (@SimulatedOpcodes, "intrncall");
    push (@SimulatedOpcodes, "spadjust");
#    push (@SimulatedOpcodes, "copy_br");
    push (@SimulatedOpcodes, "noop");
# (sc) 
    push (@SimulatedOpcodes, "getpc");

    push (@SSAOpcodes, "phi");
    push (@SSAOpcodes, "psi");

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
	push(@{$AttrGroup{'simulated'}}, $opcode);
	push (@{$SUBSET_opcodes[subset_id("st200")]}, $OP_count);

	if ($opcode eq 'getpc') {
	    $OP_scdclass[$OP_count] = "getpc";
	    $OP_results[$OP_count] = 1;
	    $OP_opnds[$OP_count] = 1;
	    $OP_result_avail_time[$OP_count][0] = 2;
	    $OP_opnd_access_time[$OP_count][0] = 1;

	    my $subset;
	    for ($subset = 0; $subset < $SUBSET_count; $subset++) {
		$OP_subset[$OP_count] = $SUBSET_name[$subset];   
		&process_scdinfo($OP_count);
	    }
	}

	if ($opcode eq 'spadjust' || $opcode eq 'asm') {
	    $OP_scdclass[$OP_count] = "simulated" if $opcode eq 'spadjust';
	    $OP_scdclass[$OP_count] = "asm" if $opcode eq 'asm';
	    $OP_results[$OP_count] = 1;
	    $OP_opnds[$OP_count] = 1;
	    $OP_result_avail_time[$OP_count][0] = 3;
	    $OP_opnd_access_time[$OP_count][0] = 2;

	    my $subset;
	    for ($subset = 0; $subset < $SUBSET_count; $subset++) {
		$OP_subset[$OP_count] = $SUBSET_name[$subset];   
		&process_scdinfo($OP_count);
	    }
	}

	$OP_count++;
    }

    foreach $opcode (@SSAOpcodes) {
	$OP_opcode[$OP_count] = $opcode;
	$OP_properties[$OP_count] = $OP_SIMULATED;
	$OP_scdclass[$OP_count] = "ssa";
	$OP_results[$OP_count] = 1;
	$OP_opnds[$OP_count] = 1;
	$OP_result_avail_time[$OP_count][0] = 0;
	$OP_opnd_access_time[$OP_count][0] = 0;
	push(@{$AttrGroup{'simulated'}}, $opcode);
	push (@{$SUBSET_opcodes[subset_id("st200")]}, $OP_count);

	my $subset;
	for ($subset = 0; $subset < $SUBSET_count; $subset++) {
	    $OP_subset[$OP_count] = $SUBSET_name[$subset];   
	    &process_scdinfo($OP_count);
	}
	$OP_count++;
    }

    foreach $opcode (@DummyOpcodes) {
	$OP_opcode[$OP_count] = $opcode;
	$OP_properties[$OP_count] = $OP_DUMMY;
	push(@{$AttrGroup{'dummy'}}, $opcode);
	push (@{$SUBSET_opcodes[subset_id("st200")]}, $OP_count);

	$OP_count++;
    }

    #    add required opcodes to all isa subsets, so they are 
    #    also emitted by the emit_subsets.
    #

    # isa_properties: add required opcodes to AttrGroup, so they are 
    # also emitted by the emit_properties.

    # var_operands:
    push(@{$AttrGroup{'var_opnds'}}, 'asm');
    push(@{$AttrGroup{'var_opnds'}}, 'intrncall');
    push(@{$AttrGroup{'var_opnds'}}, 'phi');
    push(@{$AttrGroup{'var_opnds'}}, 'psi');

    push(@{$AttrGroup{'noop'}}, 'noop');

    # isa_operands: Create SignatureGroup entry for required opcodes.
    # TODO: generalize so that target-dependent types are not
    #       hardwired here !

    push(@{$SignatureGroup{'lr:getpc:btarg:'}}, 'getpc');

    push(@{$SignatureGroup{':asm:'}}, 'asm');
    push(@{$SignatureGroup{':ssa:'}}, 'phi');
    push(@{$SignatureGroup{':ssa:'}}, 'psi');
    push(@{$SignatureGroup{':intrncall:'}}, 'intrncall');

    push(@{$SignatureGroup{'idest:spadjust:src1,isrc2'}}, 'spadjust');
#    push(@{$SignatureGroup{'bdest:copy_br:scond'}}, 'copy_br');
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

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($ISA_F, "> $dir/isa.cxx");

    &copyright_notice($ISA_F);

    print $ISA_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n";
    print $ISA_F "// Generate an ISA containing the instructions specified. \n";
    print $ISA_F "///////////////////////////////////////////////////////// \n";
    print $ISA_F "// The instructions are listed by category. The different \n";
    print $ISA_F "// categories of instructions are specified in the ISA manual:\n";
    print $ISA_F "// Within each category, the instructions are in alphabetical order.\n";
    print $ISA_F "// This arrangement of instructions matches the order in the ISA manual.\n";
    print $ISA_F "/////////////////////////////////////\n\n\n";
    print $ISA_F "#include <stddef.h>\n";
    print $ISA_F "#include \"isa_gen.h\"\n\n\n";

    print $ISA_F "main ()\n";
    print $ISA_F "{\n";
    printf $ISA_F "  ISA_Create (\"%s\", \n", $ARCH_name;

    return;
}

# ==================================================================
#    emit_opcode
#
#    Write opcodes into file isa.cxx
# ==================================================================

sub emit_opcode {
    my $opcode = $_[0];

    printf $ISA_F "\t\t \"%s\",", $OP_opcode[$opcode];
    if (opcode_is_ssa($OP_opcode[$opcode]) == 1) {
	printf $ISA_F "\t // ssa";
    }
    if (opcode_is_simulated($OP_opcode[$opcode]) == 1) {
	printf $ISA_F "\t // simulated";
    }
    if (opcode_is_dummy($OP_opcode[$opcode]) == 1) {
	printf $ISA_F "\t // dummy";
    }
    printf $ISA_F "\n";
}

# ==================================================================
#    finalize_isa_file
# ==================================================================

sub finalize_isa_file {

    print $ISA_F "      NULL);\n";
    print $ISA_F "}\n";

    close ($ISA_F);
    return;
}

# ==================================================================
#    initialize_reg_file
# ==================================================================

sub initialize_reg_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($REG_F, "> $dir/isa_registers.cxx");

    &copyright_notice($REG_F);

    printf $REG_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n";
    printf $REG_F "//  \n";
    printf $REG_F "//  Generate ISA registers information \n";
    printf $REG_F "/////////////////////////////////////// \n";
    printf $REG_F "\n";

    printf $REG_F "#include <stddef.h>\n";
    printf $REG_F "#include \"isa_registers_gen.h\" \n";
    printf $REG_F "#include \"targ_isa_subset.h\"\n\n\n";

    printf $REG_F "static int ISA_Mask(ISA_SUBSET isa) \n";
    printf $REG_F "{ \n";
    printf $REG_F "  return 1 << (int)isa; \n";
    printf $REG_F "} \n";
    printf $REG_F "\n";

    printf $REG_F "static int All_ISA_Mask(void) \n";
    printf $REG_F "{ \n";
    printf $REG_F "  int i; \n";
    printf $REG_F "  int mask = 0; \n";
    printf $REG_F "  for (i = ISA_SUBSET_MIN; i <= ISA_SUBSET_MAX; ++i) { \n";
    printf $REG_F "    mask |= 1 << i; \n";
    printf $REG_F "  } \n";
    printf $REG_F "  return mask; \n";
    printf $REG_F "} \n";
    printf $REG_F "\n";

    printf $REG_F "static int Range_ISA_Mask(ISA_SUBSET min_isa, ISA_SUBSET max_isa) \n";
    printf $REG_F "{ \n";
    printf $REG_F "  int i; \n";
    printf $REG_F "  int mask = 0; \n";
    printf $REG_F "  for (i = (int)min_isa; i <= (int)max_isa; ++i) { \n";
    printf $REG_F "    mask |= 1 << i; \n";
    printf $REG_F "  } \n";
    printf $REG_F "  return mask; \n";
    printf $REG_F "} \n";
    printf $REG_F "\n";

    return;
}

# ==================================================================
#    emit_registers
# ==================================================================

sub emit_registers {
    my $i;

    for ($i = 0; $i < $REGSET_count; $i++) {
	my $reg;
	printf $REG_F "static const char *%s_reg_names[] = {\n", $REGSET_rclass[$i];
	for ($reg = $REGSET_minreg[$i]; 
	     $reg < $REGSET_maxreg[$i];
	     $reg++) {
	    if (defined($REGSET_names[$i][$reg])) {
		printf $REG_F "\t\"%s\",", $REGSET_names[$i][$reg];
	    }
	    else {
		printf $REG_F "\tNULL,";
	    }
	    if ($reg != 0 && $reg % 4 == 0) {
		printf $REG_F "\n";
	    }
	}
	if (defined($REGSET_names[$i][$REGSET_maxreg[$i]])) {
	    printf $REG_F "\t\"%s\"\n", $REGSET_names[$i][$REGSET_maxreg[$i]];
	}
	else {
	    printf $REG_F "\tNULL\n";
	}
	printf $REG_F "};\n";
	printf $REG_F "\n";

	# subsets of this set:
	my $subset;
	for ($subset = 0; $subset < $REGSET_subset_count[$i]; $subset++) {
	    my $reg;
	    my $found;

	    # Emit members table:
	    printf $REG_F "static const int %s_%s[] = {", 
		   $REGSET_rclass[$i], $REGSET_subset[$i][$subset]{'name'};
	    $found = 0;
	    for ($reg = $REGSET_minreg[$i];
		 $reg <= $REGSET_maxreg[$i];
		 $reg++) {
#		printf STDOUT "%d: %d\n", $reg, $REGSET_subset[$i][$subset]{'regs'}[$reg];
		if ($REGSET_subset[$i][$subset]{'regs'}[$reg] == 1) {
		    if ($found) {
			printf $REG_F ",%d", $reg;
		    }
		    else {
			printf $REG_F "%d", $reg;
		    }
		    $found = 1;
		}
	    }
	    printf $REG_F "}; \n";
	}
	printf $REG_F "\n";

    }

    # Emit a usefull macro:
    printf $REG_F "#define NELEMS(a) (sizeof(a) / sizeof(*(a))) \n";
    printf $REG_F "\n";

    printf $REG_F "main() \n";
    printf $REG_F "{ \n";
    printf $REG_F "  ISA_REGISTER_CLASS \n";
    for ($i = 0; $i < $RCLASS_count-1; $i++) {
	printf $REG_F "\t rc_%s,\n", $RCLASS_name[$i];
    }
    printf $REG_F "\t rc_%s;\n", $RCLASS_name[$i];
    printf $REG_F "\n";

    printf $REG_F "  ISA_Registers_Begin(\"%s\"); \n", $ARCH_name;
    printf $REG_F "\n";

    for ($i = 0; $i < $RCLASS_count; $i++) {
	my $can_be_stored = ($RCLASS_can_be_stored[$i]) ? "true" : "false";
	my $mult_store = ($RCLASS_mult_store[$i]) ? "true" : "false";
	my $is_ptr = ($RCLASS_is_ptr[$i]) ? "true" : "false";
	printf $REG_F "  rc_%s = ISA_Register_Class_Create(\"%s\", %d, %s, %s, %s); \n",
	    $RCLASS_name[$i], $RCLASS_name[$i], $RCLASS_bits[$i], $is_ptr, $can_be_stored, $mult_store;
    }
    printf $REG_F "\n";

    # Emit register sets:
    for ($i = 0; $i < $REGSET_count; $i++) {
	my $reg;
	printf $REG_F "  ISA_Register_Set(rc_%s, %d, %d, \"%s\", NULL, All_ISA_Mask()); \n",
	    $REGSET_rclass[$i], $REGSET_minreg[$i], $REGSET_maxreg[$i], $REGSET_format[$i];
	# subsets of this set:
	my $subset;
	for ($subset = 0; $subset < $REGSET_subset_count[$i]; $subset++) {
	    printf $REG_F "  ISA_Register_Subclass_Create(\"%s\", rc_%s,
			        NELEMS(%s_%s), %s_%s, NULL); \n", 
		   $REGSET_subset[$i][$subset]{'name'}, $REGSET_rclass[$i],
		   $REGSET_rclass[$i], $REGSET_subset[$i][$subset]{'name'},
		   $REGSET_rclass[$i], $REGSET_subset[$i][$subset]{'name'};
	}
	printf $REG_F "\n";
    }

    return;
}

# ==================================================================
#    finalize_reg_file
# ==================================================================

sub finalize_reg_file {

    printf $REG_F "  ISA_Registers_End(); \n";
    printf $REG_F "  return 0; \n";
    printf $REG_F "} \n";

    close ($REG_F);
    return;
}

# ==================================================================
#    initialize_lit_file
# ==================================================================

sub initialize_lit_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($LIT_F, "> $dir/isa_lits.cxx");

    &copyright_notice($LIT_F);

    printf ($LIT_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n");
    printf ($LIT_F "\n");
    printf ($LIT_F "// \n");
    printf ($LIT_F "// Generate a list of literal classes and values for the ISA. \n");
    printf ($LIT_F "///////////////////////////////////////////////////////// \n");
    printf ($LIT_F "\n");

    printf ($LIT_F "#include <stddef.h> \n");
    printf ($LIT_F "#include \"isa_lits_gen.h\" \n");
    printf ($LIT_F "\n");

    printf ($LIT_F "main () \n");
    printf ($LIT_F "{ \n");
    printf ($LIT_F "  ISA_Lits_Begin(); \n");
    printf ($LIT_F "\n");

    printf ($LIT_F "  // ISA_Create_Lit_Class(name, type, [range,...] LIT_RANGE_END) \n");
    printf ($LIT_F "\n");

    return;
}

# ==================================================================
#    emit_literals
# ==================================================================

sub emit_literals {
    my $lclass;

    for ($lclass = 0; $lclass < $LCLASS_count; $lclass++) {
	if ($LCLASS_rtype[$lclass] eq 'SIGNED') {
	    printf($LIT_F "  ISA_Create_Lit_Class(\"%s\", %s, SignedBitRange(%d), LIT_RANGE_END); \n", $LCLASS_name[$lclass], "SIGNED", $LCLASS_bits[$lclass]);
	}
#	else {
#	    printf($LIT_F "  ISA_Create_Lit_Class(\"%s\", %s, UnsignedBitRange(%d), LIT_RANGE_END); \n", $LCLASS_name[$lclass], "UNSIGNED", $LCLASS_bits[$lclass]);
#	}
    }
    printf ($LIT_F "\n");

    return;
}

# ==================================================================
#    finalize_lit_file
# ==================================================================

sub finalize_lit_file {

    printf ($LIT_F "  ISA_Lits_End(); \n");
    printf ($LIT_F "  return 0; \n");
    printf ($LIT_F "} \n");

    close ($LIT_F);
    return;
}

# ==================================================================
#    initialize_enum_file
# ==================================================================

sub initialize_enum_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($ENUM_F, "> $dir/isa_enums.cxx");

    &copyright_notice($ENUM_F);

    printf ($ENUM_F "// AUTOMATICALLY GENERATED FROM CHESS DATA BASE !!! \n\n");
    printf ($ENUM_F "\n");

    printf ($ENUM_F "// \n");
    printf ($ENUM_F "// Generate a list of enumeration classes and values for the ISA. \n");
    printf ($ENUM_F "///////////////////////////////////////////////////////// \n");
    printf ($ENUM_F "\n");

    printf ($ENUM_F "#include <stddef.h> \n");
    printf ($ENUM_F "#include \"isa_enums_gen.h\" \n");
    printf ($ENUM_F "\n");

    printf ($ENUM_F "main () \n");
    printf ($ENUM_F "{ \n");
    printf ($ENUM_F "  ISA_Enums_Begin(); \n");
    printf ($ENUM_F "\n");

    return;
}

# ==================================================================
#    emit_enums
# ==================================================================

sub emit_enums {
    my $eclass;

    for ($eclass = 0; $eclass < $ECLASS_count; $eclass++) {
	printf($ENUM_F "  ISA_Create_Enum_Class(\"%s\", \n", 
	       $ECLASS_name[$eclass]);
	my $elt;
	for ($elt = 0; $elt < $ECLASS_elt_count[$eclass]; $elt++) {
	    printf($ENUM_F "\t \"%s\", \t %d,\n",
		   $ECLASS_elt[$eclass][$elt]{'name'},
		   $ECLASS_elt[$eclass][$elt]{'val'});
	}

	printf($ENUM_F "\t NULL, \t -1); \n");
	printf($ENUM_F "\n");
    }
    printf ($ENUM_F "\n");

    return;
}

# ==================================================================
#    finalize_enum_file
# ==================================================================

sub finalize_enum_file {

    printf ($ENUM_F "  ISA_Enums_End(); \n");
    printf ($ENUM_F "  return 0; \n");
    printf ($ENUM_F "} \n");

    close ($ENUM_F);
    return;
}

# ==================================================================
#    initialize_subset_file
# ==================================================================

sub initialize_subset_file {
    my $i;

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($SUBS_F, "> $dir/isa_subset.cxx");

    &copyright_notice($SUBS_F);

    print $SUBS_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! \n";
    print $SUBS_F "//  Generate ISA subset descriptions \n";
    print $SUBS_F "///////////////////////////////////// \n";

    print $SUBS_F "#include <stddef.h> \n";
    print $SUBS_F "#include \"topcode.h\" \n";
    print $SUBS_F "#include \"isa_subset_gen.h\" \n\n";

    print $SUBS_F "main() \n";
    print $SUBS_F "{ \n";

    for ($i = 0; $i < $SUBSET_count; $i++) {
	my $subset = $SUBSET_name[$i];
	printf ($SUBS_F "  ISA_SUBSET %s; \n", $subset);
    }
    printf ($SUBS_F "\n\n");
    printf ($SUBS_F "  ISA_Subset_Begin(\"%s\"); \n",$ARCH_name);


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
	printf $SUBS_F "  %s = ISA_Subset_Create(%s,\"%s\"); \n\n", $subset, $SUBSET_parent[$i], $subset;
	printf $SUBS_F "  /* ==================================================================== \n";
	printf $SUBS_F "   *             %s_%s Instructions \n", $ARCH_name, $subset;
	printf $SUBS_F "   * ==================================================================== \n";
	printf $SUBS_F "   */ \n";

        printf $SUBS_F "  Instruction_Group(%s, \n", $subset;
	foreach $opcode (@{$SUBSET_opcodes[$i]}) {
	    if($opcode != $UNDEF) {
		printf $SUBS_F "\t\t TOP_%s, \n", $OP_opcode[$opcode];
	    }
	}
	printf $SUBS_F "\t\t TOP_UNDEFINED); \n\n";
    }

    return;
}

# ==================================================================
#    finalize_subset_file
# ==================================================================

sub finalize_subset_file {

    print $SUBS_F "  ISA_Subset_End(); \n";
    print $SUBS_F "  return 0; \n";
    print $SUBS_F "} \n";

    close ($SUBS_F);

    return;
}

# ==================================================================
#    initialize_properties_file
# ==================================================================

sub initialize_properties_file {
    my $prop;

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($PR_F, "> $dir/isa_properties.cxx");

    &copyright_notice($PR_F);

    print $PR_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!!\n\n";
    print $PR_F "//  Generate ISA properties information \n";
    print $PR_F "/////////////////////////////////////// \n";

    print $PR_F "#include <stddef.h> \n";
    print $PR_F "#include \"topcode.h\" \n";
    print $PR_F "#include \"isa_properties_gen.h\" \n\n";

    print $PR_F "main() \n";
    print $PR_F "{ \n";
    print $PR_F "  ISA_PROPERTY \n";
    for ($prop = 0; $prop < $ISA_PROPERTY_count-1; $prop++) {
	printf $PR_F "    %s,\n", $ISA_PROPERTY_name[$prop];
    }
    printf $PR_F "    %s;\n", $ISA_PROPERTY_name[$ISA_PROPERTY_count-1];
    printf $PR_F "\n";

    printf $PR_F "  ISA_Properties_Begin (\"%s\"); \n\n\n",$ARCH_name;

    print $PR_F "  /* ==================================================================== \n";
    print $PR_F "   *              Operator attributes descriptors \n";
    print $PR_F "   * ==================================================================== \n";
    print $PR_F "   */ \n\n";

    return;
}

# ==================================================================
#    sort_by_properties
# ==================================================================

sub sort_by_properties {
    my $opcode = $_[0];


    if ($OP_format[$opcode] eq 'Load' || $OP_format[$opcode] eq 'LoadE') {
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
	if ($OP_mnemonic[$opcode] eq 'ldbu' ||
	    $OP_mnemonic[$opcode] eq 'ldbu_d' ||
	    $OP_mnemonic[$opcode] eq 'ldhu' ||
	    $OP_mnemonic[$opcode] eq 'ldhu_d') {
	  $OP_properties[$opcode] ^= $OP_UNSIGNED;
	}
    }

    if ($OP_format[$opcode] eq 'Store' || $OP_format[$opcode] eq 'StoreE') {
	$OP_properties[$opcode] ^= $OP_STORE;
	$OP_bytes[$opcode] = &op_bytes($opcode);
	$OP_align[$opcode] = &op_align($opcode);
    }

    if ($OP_format[$opcode] eq 'Branch') {
	if ($OP_mnemonic[$opcode] eq 'br' ||
	    $OP_mnemonic[$opcode] eq 'brf') {
	    $OP_properties[$opcode] ^= $OP_BRANCH;
	    $OP_properties[$opcode] ^= $OP_XFER;
	}
    }

    if ($OP_format[$opcode] eq 'Call') {
	$OP_properties[$opcode] ^= $OP_CALL;
	$OP_properties[$opcode] ^= $OP_XFER;
    }

    if ($OP_format[$opcode] eq 'Jump') {
	$OP_properties[$opcode] ^= $OP_JUMP;
	$OP_properties[$opcode] ^= $OP_XFER;
    }

    if ($OP_format[$opcode] eq 'Ijump') {
	$OP_properties[$opcode] ^= $OP_IJUMP;
	$OP_properties[$opcode] ^= $OP_XFER;
    }

    # has immediate operand ??
    if ($OP_format[$opcode] eq 'Int3I' ||
	$OP_format[$opcode] eq 'Int3E' ||
	$OP_format[$opcode] eq 'Mul64I' ||
	$OP_format[$opcode] eq 'Mul64E' ||
	$OP_format[$opcode] eq 'Cmp3I_Reg' ||
	$OP_format[$opcode] eq 'Cmp3I_Br' ||
	$OP_format[$opcode] eq 'SelectI' ||
	$OP_format[$opcode] eq 'SelectE' ||
	$OP_format[$opcode] eq 'Store' ||
	$OP_format[$opcode] eq 'Load' ||
	$OP_format[$opcode] eq 'Branch') {
	$OP_properties[$opcode] ^= $OP_IMMEDIATE;
    }

    if ($OP_format[$opcode] eq 'MoveR' ||
	$OP_format[$opcode] eq 'MoveI' ||
	$OP_format[$opcode] eq 'MoveE') {
	$OP_properties[$opcode] ^= $OP_MOVE;
	$OP_properties[$opcode] ^= $OP_INTOP;
    }

    if ($OP_format[$opcode] eq 'SelectI' ||
	$OP_format[$opcode] eq 'SelectE' || 
	$OP_format[$opcode] eq 'SelectR') {
	$OP_properties[$opcode] ^= $OP_SELECT;
	$OP_properties[$opcode] ^= $OP_INTOP;
    }

    if ($OP_format[$opcode] eq 'Int3I' ||
	$OP_format[$opcode] eq 'Int3R' ||
	$OP_format[$opcode] eq 'Int3E') {
        if ($OP_mnemonic[$opcode] eq 'mull' ||
	    $OP_mnemonic[$opcode] eq 'mulll' ||
	    $OP_mnemonic[$opcode] eq 'mulllu' ||
	    $OP_mnemonic[$opcode] eq 'mullu') {
	    $OP_properties[$opcode] ^= $OP_MUL;
	}

	if ($OP_mnemonic[$opcode] eq 'sub') {
	    $OP_properties[$opcode] ^= $OP_SUB;
	}

	if ($OP_mnemonic[$opcode] eq 'add' ||
	    $OP_mnemonic[$opcode] eq 'addcg') {
	    $OP_properties[$opcode] ^= $OP_ADD;
	}

	if ($OP_mnemonic[$opcode] eq 'or') {
	    $OP_properties[$opcode] ^= $OP_OR;
	}

	if ($OP_mnemonic[$opcode] eq 'and') {
	    $OP_properties[$opcode] ^= $OP_AND;
	}

	if ($OP_mnemonic[$opcode] eq 'xor') {
	    $OP_properties[$opcode] ^= $OP_XOR;
	}

	if ($OP_mnemonic[$opcode] eq 'shl') {
	    $OP_properties[$opcode] ^= $OP_SHL;
	}

	if ($OP_mnemonic[$opcode] eq 'shr') {
	    $OP_properties[$opcode] ^= $OP_SHR;
	}

	if ($OP_mnemonic[$opcode] eq 'shru') {
	    $OP_properties[$opcode] ^= $OP_SHRU;
	}

	if ($OP_mnemonic[$opcode] eq 'maxu' ||
	    $OP_mnemonic[$opcode] eq 'minu' ||
	    $OP_mnemonic[$opcode] eq 'shru' ||
	    $OP_mnemonic[$opcode] eq 'mulhhu' ||
	    $OP_mnemonic[$opcode] eq 'mulhu' ||
	    $OP_mnemonic[$opcode] eq 'mulllu' ||
	    $OP_mnemonic[$opcode] eq 'mullhu' ||
	    $OP_mnemonic[$opcode] eq 'mulllu' ||
	    $OP_mnemonic[$opcode] eq 'mullu') {
	    $OP_properties[$opcode] ^= $OP_UNSIGNED;
	}

	$OP_properties[$opcode] ^= $OP_INTOP;
    }

    if ($OP_format[$opcode] eq 'Mul64I' ||
	$OP_format[$opcode] eq 'Mul64R' ||
	$OP_format[$opcode] eq 'Mul64E') {
      if ($OP_mnemonic[$opcode] eq 'mul32') {
	  $OP_properties[$opcode] ^= $OP_MUL;
      }
    }

    if ($OP_format[$opcode] eq 'Monadic') {
	if ($OP_mnemonic[$opcode] eq 'sxth') {
	    $OP_properties[$opcode] ^= $OP_SEXT;
	}
	if ($OP_mnemonic[$opcode] eq 'sxtb') {
	    $OP_properties[$opcode] ^= $OP_SEXT;
	}
	elsif ($OP_mnemonic[$opcode] eq 'zxth') {
	    $OP_properties[$opcode] ^= $OP_ZEXT;
	}

	$OP_properties[$opcode] ^= $OP_INTOP;
    }

    if ($OP_format[$opcode] eq 'Cmp3I_Reg' ||
	$OP_format[$opcode] eq 'Cmp3E_Reg' ||
	$OP_format[$opcode] eq 'Cmp3R_Reg' ||
	$OP_format[$opcode] eq 'Cmp3I_Br'  ||
	$OP_format[$opcode] eq 'Cmp3E_Br'  ||
	$OP_format[$opcode] eq 'Cmp3R_Br') {

	if ($OP_mnemonic[$opcode] eq 'cmpgeu' ||
	    $OP_mnemonic[$opcode] eq 'cmpgtu' ||
	    $OP_mnemonic[$opcode] eq 'cmpleu' ||
	    $OP_mnemonic[$opcode] eq 'cmpltu') {
	    $OP_properties[$opcode] ^= $OP_UNSIGNED;
	}

	$OP_properties[$opcode] ^= $OP_CMP;
	$OP_properties[$opcode] ^= $OP_INTOP;
    }

    return;
}

# ==================================================================
#    emit_properties
# ==================================================================

sub emit_properties {
    my $group;
    my $opcode;

    foreach $group (sort(keys(%AttrGroup))) {
	my $group_str = $group;

	if ($group eq 'xor') {
	    $group_str = 'Xor';
	}
	elsif ($group eq 'and') {
    	    $group_str = 'And';
	}
	elsif ($group eq 'or') {
    	    $group_str = 'Or';
	}

	printf $PR_F "  /* ====================================== */ \n";
        printf $PR_F "  %s = ISA_Property_Create (\"%s\"); \n", $group_str, $group;

	printf $PR_F "  Instruction_Group (%s, \n", $group_str;
	foreach $opcode (@{$AttrGroup{$group}}) {
	    unless ($opcode eq $UNDEF) {
		printf $PR_F "\t\t TOP_%s, \n", $opcode;
	    }
	}
	printf $PR_F "\t\t TOP_UNDEFINED); \n\n";
    }

  # Needed to compile. no op associated. should be removed
    foreach $group qw(defs_fcc defs_fcr refs_fcr branch_predict access_reg_bank side_effects unalign mem_fill_type likely l_group f_group flop madd guard_t guard_f div unsafe) {
	printf $PR_F "  /* ====================================== */ \n";
        printf $PR_F "  %s = ISA_Property_Create (\"%s\"); \n", $group, $group;
	printf $PR_F "  Instruction_Group (%s, \n", $group;
	printf $PR_F "\t\t TOP_UNDEFINED); \n\n";
    }

    foreach $group (sort(keys(%MemBytes))) {
	printf $PR_F "  /* ====================================== */ \n";
	printf $PR_F "  /*         Memory Access Size %s          */ \n", $group;
	printf $PR_F "  /* ====================================== */ \n";
	printf $PR_F "  ISA_Memory_Access (%s, \n", $group;
	foreach $opcode (@{$MemBytes{$group}}) {
	    unless ($opcode eq $UNDEF) {
		printf $PR_F "\t\t TOP_%s, \n", $opcode;
	    }
	}
	printf $PR_F "\t\t TOP_UNDEFINED); \n\n";
    }

    foreach $group (sort(keys(%MemAlign))) {
	printf $PR_F "  /* ====================================== */ \n";
	printf $PR_F "  /*          Memory Alignment %s           */ \n", $group;
	printf $PR_F "  /* ====================================== */ \n";
	printf $PR_F "  ISA_Memory_Alignment (%s, \n", $group;
	foreach $opcode (@{$MemAlign{$group}}) {
	    unless ($opcode eq $UNDEF) {
		printf $PR_F "\t\t TOP_%s, \n", $opcode;
	    }
	}
	printf $PR_F "\t\t TOP_UNDEFINED); \n\n";
    }

    return;
}

# ==================================================================
#    finalize_properties_file
# ==================================================================

sub finalize_properties_file {

    print $PR_F "  ISA_Properties_End();\n";
    print $PR_F "  return 0;\n";
    print $PR_F "}\n";

    close ($PR_F);
    return;
}

# ==================================================================
#    initialize_operands_file
# ==================================================================

sub initialize_operands_file {
    my $i;

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($OPND_F, "> $dir/isa_operands.cxx");

    print $OPND_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! \n";
    print $OPND_F "// Group TOPS with similar operands/results format. \n";
    print $OPND_F "///////////////////////////////////////////////////////// \n";
    print $OPND_F "// Within each category, the instructions are arranged roughly in order \n";
    print $OPND_F "// of increasing numbers of operands. \n";
    print $OPND_F "///////////////////////////////////// \n\n\n";

    print $OPND_F "#include <stddef.h> \n";
    print $OPND_F "#include \"topcode.h\" \n";
    print $OPND_F "#include \"isa_operands_gen.h\" \n\n";

    print $OPND_F "main() \n";
    print $OPND_F "{ \n\n";

    # Specify operand value types:
    if ($REG_OPND_count > 0) {
	printf $OPND_F "  OPERAND_VALUE_TYPE ";
	for ($i = 0; $i < $REG_OPND_count-1; $i++) {
	    printf $OPND_F "%s, ", $REG_OPND_name[$i];
	}
	printf $OPND_F "%s; \n", $REG_OPND_name[$REG_OPND_count-1];
    }
    if ($LIT_OPND_count > 0) {
	printf $OPND_F "  OPERAND_VALUE_TYPE ";
	for ($i = 0; $i < $LIT_OPND_count-1; $i++) {
	    printf $OPND_F "%s, ", $LIT_OPND_name[$i];
	}
	printf $OPND_F "%s; \n", $LIT_OPND_name[$LIT_OPND_count-1];
    }
    if ($ENUM_OPND_count > 0) {
	printf $OPND_F "  OPERAND_VALUE_TYPE ";
	for ($i = 0; $i < $ENUM_OPND_count-1; $i++) {
	    printf $OPND_F "%s, ", $ENUM_OPND_name[$i];
	}
	printf $OPND_F "%s; \n", $ENUM_OPND_name[$ENUM_OPND_count-1];
    }
    print $OPND_F "\n";

    # Specify operand use type:
    printf $OPND_F "  /* ------------------------------------------------------\n";
    printf $OPND_F "   *   Following built-in use types must be specified: \n";
    printf $OPND_F "   *     1. base operand use for TOP_load, TOP_store; \n";
    printf $OPND_F "   *     2. offset operand use for TOP_load, TOP_store; \n";
    printf $OPND_F "   *     3. storeval operand use for TOP_store; \n";
    printf $OPND_F "   *     4. condition operand use for TOP_select; \n";
    printf $OPND_F "   * \n";
    printf $OPND_F "   *   Following built-in use types may be specified: \n";
    printf $OPND_F "   *     5. implicit operand use for TOPs when the operand is implicitely used; \n";
    printf $OPND_F "   * \n";
    printf $OPND_F "   *   Here you can specify any additional operand uses. \n";
    printf $OPND_F "   * ------------------------------------------------------\n";
    printf $OPND_F "   */\n";

    if ($OPND_USE_count > 0) {
	printf $OPND_F "  OPERAND_USE_TYPE \n";
	for ($i = 0; $i < $OPND_USE_count-1; $i++) {
	    printf $OPND_F "        %s, \n", $OPND_USE_name[$i];
	}
	printf $OPND_F "        %s; \n", $OPND_USE_name[$OPND_USE_count-1];
    }
    printf $OPND_F "\n";

    printf $OPND_F "  ISA_Operands_Begin(\"%s\"); \n",$ARCH_name;

    if ($REG_OPND_count > 0) {
	printf $OPND_F "  /* Create the register operand types: */ \n";
	for ($i = 0; $i < $REG_OPND_count; $i++) {
	    printf $OPND_F "  %s = ISA_Reg_Opnd_Type_Create(\"%s\", \n",
		    $REG_OPND_name[$i], $REG_OPND_name[$i];
            printf $OPND_F "                ISA_REGISTER_CLASS_%s, \n",
		    $REG_OPND_rclass[$i];
	    printf $OPND_F "                ISA_REGISTER_SUBCLASS_%s, \n",
		    $REG_OPND_subclass[$i];
	    printf $OPND_F "                %d, %s, %s); \n",
		    $REG_OPND_size[$i], $REG_OPND_rtype[$i], $REG_OPND_fp_type[$i];
	}
	printf $OPND_F "\n";
    }
    if ($LIT_OPND_count > 0) {
	printf $OPND_F "  /* Create the literal operand types: */ \n";
	for ($i = 0; $i < $LIT_OPND_count; $i++) {
	    printf $OPND_F "  %s = ISA_Lit_Opnd_Type_Create(\"%s\", %d, %s, LC_%s); \n",
		    $LIT_OPND_name[$i], $LIT_OPND_name[$i], 
		    $LIT_OPND_size[$i],$LIT_OPND_rtype[$i], 
		    $LIT_OPND_lclass[$i];
	}
	printf $OPND_F "\n";
    }

    if ($ENUM_OPND_count > 0) {
	printf $OPND_F "  /* Create the enum operand types: */ \n";
	for ($i = 0; $i < $ENUM_OPND_count; $i++) {
	    printf $OPND_F "  %s = ISA_Enum_Opnd_Type_Create(\"%s\", %d, %s, EC_%s); \n",
		    $ENUM_OPND_name[$i], $ENUM_OPND_name[$i], $ENUM_OPND_size[$i],$ENUM_OPND_rtype[$i], $ENUM_OPND_eclass[$i];
	}
	printf $OPND_F "\n";
    }
    printf $OPND_F "\n";

    if ($OPND_USE_count > 0) {
	printf $OPND_F "  /* Create the operand uses: */ \n";
	printf $OPND_F "\n";
	for ($i = 0; $i < $OPND_USE_count; $i++) {
	    printf $OPND_F "  %s = Create_Operand_Use(\"%s\"); \n",
		    $OPND_USE_name[$i], $OPND_USE_name[$i];
	}
	printf ($OPND_F "\n");
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
    my $gname;
    my @results;
    my @operands;

    foreach $signature (sort(keys(%SignatureGroup))) {
	($rests, $gname, $opnds) = split(":", $signature);
	print $OPND_F "  /* ====================================== */ \n";
	if ($gname eq '*') {
	    print $OPND_F "  Instruction_Group(\"O_$group\", \n";
	    $group++;
	}
	else {
	    print $OPND_F "  Instruction_Group(\"O_$gname\", \n";
	}

	my @PushOpcode;
	for (my $i = 0; $i < $OP_count; $i++) {
	    $PushOpcode[$i] = 0;
	}
	foreach $opcode (@{$SignatureGroup{$signature}}) {
	    if ($opcode ne $UNDEF) {
		print $OPND_F "\t\t TOP_$opcode, \n";
	    }
	}
	print $OPND_F "\t\t TOP_UNDEFINED); \n\n";
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
		print $OPND_F "  Result ($count, $name); \n";
	    }
	    else {
		foreach $attribute (@attributes) {
		    if ($attribute =~ /same(\d*)/) {
			printf ($OPND_F "  Result (%d, %s); \n", $count, $name);
			printf ($OPND_F "  Same_Res (%d); \n", $1);
		    }
		    else {
			print $OPND_F "  Result ($count, $name, $attribute); \n";
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
		print $OPND_F "  Operand ($count, $name); \n";
	    }
	    else {
		foreach $attribute (@attributes) {
		    print $OPND_F "  Operand ($count, $name, $attribute); \n";
		}
	    }
	    $count++;
	}

	printf($OPND_F "\n");
    }

    return;
}

# ==================================================================
#    finalize_operands_file
# ==================================================================

sub finalize_operands_file {

    print $OPND_F "\n\n";
    print $OPND_F "  ISA_Operands_End(); \n";
    print $OPND_F "  return 0; \n";
    print $OPND_F "} \n";

    close ($OPND_F);
    return;
}

# ==================================================================
#    initialize_print_file
# ==================================================================

sub initialize_print_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($PRNT_F, "> $dir/isa_print.cxx");

    print $PRNT_F "//  AUTOMATICALLY GENERATED FROM CHESS DATABASE !!! \n";
    print $PRNT_F "// Group TOPS with similar printing format. \n";
    print $PRNT_F "//////////////////////////////////////////////////\n\n\n";

    print $PRNT_F "#include <stdio.h> \n";
    print $PRNT_F "#include <stddef.h> \n";
    print $PRNT_F "#include <string.h> \n";
    print $PRNT_F "#include <ctype.h> \n";
    print $PRNT_F "#include \"topcode.h\" \n";
    print $PRNT_F "#include \"isa_print_gen.h\" \n\n";

    return;
}

# ==================================================================
#    emit_printing_formats
# ==================================================================

sub emit_printing_formats {
    my $i;

    # printout a table with mnemonic names:
    printf($PRNT_F "static const char *mnemonic_names[%d] = {\n", $OP_count);
CONTINUE:
    for ($i = 0; $i < $OP_count-1; $i++) {
	if ($OP_opcode[$i] eq $UNDEF) {
	    next CONTINUE;
	}
	if (opcode_is_dummy($OP_opcode[$i]) || 
	    opcode_is_ssa($OP_opcode[$i]) || 
	    opcode_is_simulated($OP_opcode[$i])) {
	    printf $PRNT_F "  \"%s\",\t /* TOP_%s */\n", $OP_opcode[$i], $OP_opcode[$i];
	}
	# mnemonics is corrupted for icall,igoto
	elsif ($OP_mnemonic[$i] eq 'icall') {
	    printf $PRNT_F "  \"call\",\t /* TOP_%s */ \n", $OP_opcode[$i];
	}
	elsif ($OP_mnemonic[$i] eq 'igoto') {
	    printf $PRNT_F "  \"goto\",\t /* TOP_%s */ \n", $OP_opcode[$i];
	}
	elsif ($OP_properties[$i] & $OP_DISMISSIBLE) {
	    my $mnemonic = $OP_mnemonic[$i];
	    $mnemonic =~ tr/_/./;
	    printf $PRNT_F "  \"%s\",\t /* TOP_%s */ \n", $mnemonic, , $OP_opcode[$i];
	}
	else {
	    printf $PRNT_F "  \"%s\",\t /* TOP_%s */ \n", $OP_mnemonic[$i], $OP_opcode[$i];
	}
    }

    if (opcode_is_dummy($OP_opcode[$OP_count-1]) || 
	opcode_is_ssa($OP_opcode[$OP_count-1]) || 
	opcode_is_simulated($OP_opcode[$OP_count-1])) {

	printf $PRNT_F "  \"%s\" \t /* TOP_%s */\n", $OP_opcode[$OP_count-1], $OP_opcode[$OP_count-1];
    }
    # mnemonics is corrupted for icall,igoto
    elsif ($OP_mnemonic[$i] eq 'icall') {
	printf $PRNT_F "  \"call\",\t /* TOP_%s */ \n", $OP_opcode[$OP_count-1];
    }
    elsif ($OP_mnemonic[$i] eq 'igoto') {
	printf $PRNT_F "  \"goto\",\t /* TOP_%s */ \n", $OP_opcode[$OP_count-1];
    }
    else {
	printf $PRNT_F "  \"%s\" \t /* TOP_%s */ \n", $OP_mnemonic[$OP_count-1], $OP_opcode[$OP_count-1];
    }
    printf $PRNT_F "};\n";

    printf $PRNT_F "\n";

#    print $PRNT_F "// Multiple topcodes map to the same assembly name. To disambiguate the \n";
#    print $PRNT_F "// topcodes, we append a signature to the basename. To get the assembly \n";
#    print $PRNT_F "// name we must strip off the suffix. \n\n";

    print $PRNT_F "static const char *asmname(TOP topcode) \n";
    print $PRNT_F "{ \n";

#    print $PRNT_F "  int c; \n";
#    print $PRNT_F "  int i; \n";
#    print $PRNT_F "  const char *name = TOP_Name(topcode); \n";
#    print $PRNT_F "  char buf[100]; \n";

#    print $PRNT_F "  /* \n";
#    print $PRNT_F "   * First handle simulated and dummy instructions: \n";
#    print $PRNT_F "   * Handle movl and movr instructions: \n";
#    print $PRNT_F "   */ \n";
#    print $PRNT_F "  switch (topcode) { \n";

#    my $opcode;
#    foreach $opcode (@SimulatedOpcodes) {
#	printf $PRNT_F "  case TOP_%s: return \"%s\"; \n", $opcode, $opcode;
#    }

#    foreach $opcode (@DummyOpcodes) {
#	printf $PRNT_F "  case TOP_%s: return \"%s\"; \n", $opcode, $opcode;
#    }
    # special cases (I really should generate a table):
#    print $PRNT_F "  case TOP_icall: return \"call\"; \n";

#    print $PRNT_F "  default: break; \n";
#    print $PRNT_F "  } \n";
#    print $PRNT_F "\n";

#    print $PRNT_F "  for (i = 0; c != '\\0'; ++i) { \n";
#    print $PRNT_F "    c = name[i]; \n";
#    print $PRNT_F "    if (c == '_') { \n";
#    print $PRNT_F "      // if next is _i or _r, get out, else it's a _d \n";
#    print $PRNT_F "      if (name[i+1] == 'd') { \n";
#    print $PRNT_F "	buf[i] = '.'; \n";
#    print $PRNT_F "	buf[i+1] = 'd'; \n";
#    print $PRNT_F "        buf[i+3] = '\\0'; \n";
#    print $PRNT_F "      } \n";
#    print $PRNT_F "      else { \n";
#    print $PRNT_F "	buf[i] = '\\0'; \n";
#    print $PRNT_F "      } \n";
#    print $PRNT_F "      break; \n";
#    print $PRNT_F "    } \n";
#    print $PRNT_F "    buf[i] = c; \n";
#    print $PRNT_F "  } \n\n";

#    print $PRNT_F "  return strdup(buf); \n";

    printf $PRNT_F "  return mnemonic_names[topcode]; \n";
    printf $PRNT_F "} \n\n";

    print $PRNT_F "main() \n";
    print $PRNT_F "{ \n";

    printf $PRNT_F "  ISA_Print_Begin(\"%s\"); \n\n",$ARCH_name;
    printf $PRNT_F "  Set_AsmName_Func(asmname); \n\n";

    # First specify bundle/grouping stuff
    printf $PRNT_F "  Define_Macro(\"END_GROUP\", \";;\");\t// end-of-group marker \n";
    printf $PRNT_F "  Define_Macro(\"BEGIN_BUNDLE\", \"## {\t %s:\");\t// bundle introducer \n", "%s";
    printf $PRNT_F "  Define_Macro(\"END_BUNDLE\", \"## };\");\t// bundle terminator \n";
    printf $PRNT_F "\n";

    my $count = 0;
    foreach my $signature (sort(keys(%PrintGroup))) {
	my @pattern = split ('_',$signature);
	my @fmt;
	my @instr;
	my @args;

#	printf STDOUT "trying signature %s\n", $signature;

	# push %s for name in the format
	push (@instr, "%s");

	my $i = 0;
	foreach my $elt (@pattern) {
	    if ($elt =~ /(.*):(\S*)/) {
		# for now always %s in the instrn format
		push (@instr, "%s");
		$args[$i++] = $1;
		push (@fmt, $2);
	    }
	    else {
		# just push
		push (@instr, $elt);
	    }
	}

	my $new_ft = join(' ',@instr);

	# print print group header
	print $PRNT_F "  /* ================================= */ \n";
	print $PRNT_F "  ISA_PRINT_TYPE print_$count; \n";
	print $PRNT_F "  print_$count = ISA_Print_Type_Create(\"print_$count\", \"$new_ft\"); \n";

	printf $PRNT_F "  Name(); \n", "%s";

	my $j;
	for ($j = 0; $j < $i; $j++) {
	    if ($args[$j] eq 'R0') { 
		printf $PRNT_F "  Result(0); \n", shift (@fmt);
	    }
	    elsif ($args[$j] eq 'R1') { 
		printf $PRNT_F "  Result(1); \n", shift (@fmt);
	    }
	    elsif ($args[$j] eq 'O0') { 
		printf $PRNT_F "  Operand(0); \n", shift (@fmt);
	    }
	    elsif ($args[$j] eq 'O1') { 
		printf $PRNT_F "  Operand(1); \n", shift (@fmt);
	    }
	    elsif ($args[$j] eq 'O2') { 
		printf $PRNT_F "  Operand(2); \n", shift (@fmt);
	    }
	    elsif ($args[$j] eq 'O3') { 
		printf $PRNT_F "  Operand(3); \n", shift (@fmt);
	    }
	    else {
		printf STDOUT "ERROR: unknown element %s in emit_printing_formats\n", $args[$j];
		exit(1);
	    }
	}

	# print instructions:
	print $PRNT_F "\n";
	print $PRNT_F "  Instruction_Print_Group(print_$count, \n";
	foreach my $opcode (@{$PrintGroup{$signature}}) {
	    unless ($opcode eq $UNDEF) {
		print $PRNT_F "\t\t TOP_$opcode, \n";
	    }
	}
	print $PRNT_F "\t\t TOP_UNDEFINED); \n";
	print $PRNT_F "\n";

	$count++;
    }

    return;
}

# ==================================================================
#    finalize_print_file
# ==================================================================

sub finalize_print_file {

    print $PRNT_F "\n\n";
    print $PRNT_F "  ISA_Print_End(); \n";
    print $PRNT_F "  return 0; \n";
    print $PRNT_F "} \n";

    close ($PRNT_F);
    return;
}

# ==================================================================
#    initialize_pack_file
# ==================================================================

sub initialize_pack_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($PACK_F, "> $dir/isa_pack.cxx");
    &copyright_notice ($PACK_F);

    printf $PACK_F "// \n";
    printf $PACK_F "// Group TOPs with similar packing format together.  \n";
    printf $PACK_F "///////////////////////////////////////////////////////// \n";

    printf $PACK_F "// Instructions may be packed (compressed) in binary files. The packing rules \n";
    printf $PACK_F "// are specified in this file. \n\n";

    printf $PACK_F "#include <stddef.h> \n";
    printf $PACK_F "#include \"topcode.h\" \n";
    printf $PACK_F "#include \"isa_pack_gen.h\" \n\n";
 
    printf $PACK_F "main() \n";
    printf $PACK_F "{ \n";
}

# ==================================================================
#    emit_pack_info
# ==================================================================

sub emit_pack_info {
    my $subset = shift;
    my $group;
    my $count;

    # Use the same groups as operand/result description.
    # TODO: should reflect things like operand scaling, etc.

    printf $PACK_F "  ISA_PACK_TYPE \n\t\t";
    my $i;
    for ($i = 0; $i < $PACK_count-1; $i++) {
	printf $PACK_F "p%d, \t// %s\n\t\t", $i, $PACK_comment[$i];
    }
    printf $PACK_F "p%d; \t// %s\n\n", $i, $PACK_comment[$i];

    # This is just a copy of what I found in the ia64/isa_pack.cxx
    printf $PACK_F "  OPND_ADJ_TYPE	no_adj; \n\n";

    # An instruction is 32-bit wide on the ST200
    printf $PACK_F "  ISA_Pack_Begin(\"%s\", 32); \n\n", $ARCH_name;

    printf $PACK_F "  /* Create the various adjustments that need to be performed between \n";
    printf $PACK_F "   * assembly language form and packed form. Note that simple shift  \n";
    printf $PACK_F "   * adjustments have been accomplished directly in the operand packing \n";
    printf $PACK_F "   * specification. \n";
    printf $PACK_F "   */ \n\n";
    printf $PACK_F "  no_adj = Create_Operand_Adjustment(\"no adjustment\", \"O_VAL\"); \n\n";

    $count = 0;
    foreach $group (sort(keys(%PackGroup))) {
	my $rests;
	my $gname;
	my $opnds;

	# find corresponding PACK
	my $i;
	for ($i = 0; $i < $PACK_count; $i++) {
	    if ($PACK_type[$i] eq $group) {
		goto BREAK;
	    }
	}

	printf STDOUT "ERROR: couldn't find PACK info for %s\n", $group;
	exit(1);

BREAK:
	# $i -- index of corresponding PACK info
	printf $PACK_F "  /* =====  p%d: ===== */ \n", $i;
	printf $PACK_F "  p%d = ISA_Pack_Type_Create(\"p%d\"); \n", $i, $i;
	my $word;
	for ($word = 0; $word < $PACK_words[$i]; $word++) {
	    my $res;
	    my $opnd;

	    # issue Next_Word()
	    if ($word > 0) {
		printf $PACK_F "  Next_Word(); \n";
	    }

	    # results
	    for ($res = 0; $res < $PACK_results[$i]; $res++) {
		if ($PACK_word_results[$i][$res][$word] == 1) {
		    printf $PACK_F "  Result(%s, %d, %d); \n", 
			    $res, 
			    $PACK_result_ipos[$i][$res][$word],
			    $PACK_result_bits[$i][$res][$word];
		}
	    }

	    # operands now
	    for ($opnd = 0; $opnd < $PACK_opnds[$i]; $opnd++) {
		if ($PACK_word_opnds[$i][$opnd][$word] == 1) {
		    printf $PACK_F "  Operand(%s, %d, %d, %d); \n", 
			    $opnd, 
			    $PACK_opnd_start[$i][$opnd][$word],
			    $PACK_opnd_ipos[$i][$opnd][$word],
			    $PACK_opnd_bits[$i][$opnd][$word];
		}
	    }
	}

	# instruction group:
	printf $PACK_F "  Instruction_Pack_Group(p%d, \n", $i;
	foreach my $opcode (@{$PackGroup{$group}}) {
	    unless ($opcode eq $UNDEF) {
		printf $PACK_F "\tTOP_%s, \t", $opcode;

		for ($word = 0; $word < $PACK_words[$i]; $word++) {
		    # coding is dummy for now
		    printf $PACK_F " 0x10000000UL,";
		}
		printf $PACK_F "\n";
	    }
	}
	printf $PACK_F "\tTOP_UNDEFINED); \n\n";
    }
}

# ==================================================================
#    finalize_pack_file
# ==================================================================

sub finalize_pack_file {

    printf $PACK_F "  ISA_Pack_End(); \n";
    printf $PACK_F "  return 0; \n";
    printf $PACK_F "} \n";

    close ($PACK_F);
    return;
}

# ==================================================================
#    initialize_decode_file
# ==================================================================

sub initialize_decode_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($DECODE_F, "> $dir/isa_decode.cxx");
    &copyright_notice ($DECODE_F);

    printf $DECODE_F "// \n";
    printf $DECODE_F "// Generate instruction decoding information. \n";
    printf $DECODE_F "///////////////////////////////////// \n";
    printf $DECODE_F "///////////////////////////////////// \n\n";

    printf $DECODE_F "#include \"topcode.h\" \n";
    printf $DECODE_F "#include \"isa_decode_gen.h\" \n";
    printf $DECODE_F "#include \"targ_isa_bundle.h\" \n\n";
 
    printf $DECODE_F "main() \n";
    printf $DECODE_F "{ \n\n";
}

# ==================================================================
#    emit_decode_info
# ==================================================================

sub emit_decode_info {
    my $subset = shift;
    my $i;
    printf $DECODE_F "  ISA_Decode_Begin(\"%s\"); \n\n", $ARCH_name;

    # a completely dummy decoding description: all units go to alu
    printf $DECODE_F "  STATE ex_unit = Create_Unit_State(\"ex_unit\", 0, 4); \n\n";
    printf $DECODE_F "  STATE alu = Create_Inst_State(\"alu\", 0, 0, 15); \n\n";

    printf $DECODE_F "  Transitions(ex_unit, \n";
    for ($i = 0; $i < $EXEC_SLOT_count; $i++) {
	printf $DECODE_F "        ISA_EXEC_%s_Unit, alu, \n", $EXEC_SLOT_name[$i];
    }
    printf $DECODE_F "	      END_TRANSITIONS); \n\n";

    printf $DECODE_F "  Transitions(alu, \n";
    # all instructions:
CONTINUE:
    for ($i = 0; $i < $OP_count; $i++) {
	my $opcode = $OP_opcode[$i];
        # skip if it's an simulated/dummy:
	if (!$opcode ||
	    ($OP_properties[$i] & $OP_DUMMY) ||
	    ($OP_properties[$i] & $OP_SIMULATED)) {
	    next CONTINUE;
	}
	printf $DECODE_F "	   %d,\t Final(TOP_%s),\n", $i, $opcode;
    }
    printf $DECODE_F "	      END_TRANSITIONS); \n\n";

    printf $DECODE_F "  Initial_State(ex_unit); \n\n";

    printf $DECODE_F "  ISA_Decode_End(); \n";
}

# ==================================================================
#    finalize_decode_file
# ==================================================================

sub finalize_decode_file {

    printf $DECODE_F "  return 0; \n";
    printf $DECODE_F "} \n";

    close ($DECODE_F);
    return;
}

# ==================================================================
#    initialize_bundle_file
# ==================================================================

sub initialize_bundle_file {

    my $dir = shift || '.';
    $dir .= "/isa";
    system "mkdir -p $dir";
    open ($BUNDLE_F, "> $dir/isa_bundle.cxx");
    &copyright_notice ($BUNDLE_F);

    printf $BUNDLE_F "//  \n";
    printf $BUNDLE_F "//  Generate ISA bundle information \n";
    printf $BUNDLE_F "/////////////////////////////////////// \n\n";

    printf $BUNDLE_F "#include <stddef.h> \n";
    printf $BUNDLE_F "#include \"topcode.h\" \n";
    printf $BUNDLE_F "#include \"isa_bundle_gen.h\" \n\n";

    printf $BUNDLE_F "main() \n";
    printf $BUNDLE_F "{ \n";
}

# ==================================================================
#    emit_bundle_info
# ==================================================================

sub emit_bundle_info {
    my $subset = shift;
    my $unit;
    my $slot;
    my $bundle;

    # Emit the Exec_Unit types - these are EXEC_SLOTs:
    printf $BUNDLE_F "  ISA_EXEC_UNIT_TYPE \n";

    for ($unit = 0; $unit < $EXEC_SLOT_count-1; $unit++) {
	printf $BUNDLE_F "\t\t    %s_Unit, \n", $EXEC_SLOT_name[$unit];
    }
    printf $BUNDLE_F "\t\t    %s_Unit; \n" , $EXEC_SLOT_name[$EXEC_SLOT_count-1];
    printf $BUNDLE_F "\n";

    for ($bundle = 0; $bundle < $BUNDLE_count; $bundle++) {
	my $f_id;
	my $f_begins;
	my $f_bits;

	printf $BUNDLE_F "  ISA_Bundle_Begin(\"%s\", %d); \n",
                           $BUNDLE_name[$bundle], $BUNDLE_bits[$bundle];
	printf $BUNDLE_F "\n";

	printf $BUNDLE_F "  /* ===== Specification for bundle packing  ===== */ \n";
	printf $BUNDLE_F "  ISA_Bundle_Pack_Create(ISA_Bundle_Pack_Little_Endian); \n";
	($f_id, $f_begins, $f_bits) = split(",", $BUNDLE_field[$bundle]);
	printf $BUNDLE_F "  Pack_Template(%d, %d, %d); \n", $f_id, $f_begins, $f_bits;
        for ($slot = 0; $slot < $BUNDLE_slots[$bundle]; $slot++) {
	    printf $BUNDLE_F "  Pack_Slot(%d, 0, %d, %d); \n",
		    $BUNDLE_slot[$bundle][$slot]{'id'},
		    $BUNDLE_slot[$bundle][$slot]{'start'},
		    $BUNDLE_slot[$bundle][$slot]{'bits'};
	}
	printf $BUNDLE_F "\n";
    }

    # Emit instructions to Exec_Unit mapping information.
    for ($unit = 0; $unit < $EXEC_SLOT_count; $unit++) {
	printf $BUNDLE_F "  /* ===== Specification for %s_Unit Type ===== */ \n", $EXEC_SLOT_name[$unit];
	printf $BUNDLE_F "  %s_Unit = ISA_Exec_Unit_Type_Create(\"%s_Unit\", NULL); \n",
	    $EXEC_SLOT_name[$unit], $EXEC_SLOT_name[$unit];
	printf $BUNDLE_F "  Instruction_Exec_Unit_Group(%s_Unit, \n", $EXEC_SLOT_name[$unit];
	my @scdclasses = split(",",$EXEC_SLOT_scds[$unit]);
	my $scdclass;
	foreach $scdclass (@scdclasses) {
	    my $opcode;
	    foreach $opcode (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}) {
		printf $BUNDLE_F "\t\t TOP_%s, \n", $opcode;
	    }
	}
	printf $BUNDLE_F "\t\t TOP_UNDEFINED); \n\n";
    }

    # Emit allowed bundle combinations:
    for ($bundle = 0; $bundle < $BUNDLE_count; $bundle++) {
	my $temp;
	printf $BUNDLE_F "  /* === All legal bundle orderings (%s of them) are specified below. */ \n\n", $BUNDLE_temps[$bundle];
	for ($temp = 0; $temp < $BUNDLE_temps[$bundle]; $temp++) {
	    my $i;
	    my $name = ${BUNDLE_temp[$bundle][$temp]}{'name'};
	    my $slots = ${BUNDLE_temp[$bundle][$temp]}{'slots'};
	    my $stops = ${BUNDLE_temp[$bundle][$temp]}{'stops'};

	    printf $BUNDLE_F "  /* ===== Template 0x00 (%d) ===== */ \n", $temp;
	    printf $BUNDLE_F "  ISA_Bundle_Type_Create(\"%s\", \".%s\", %d); \n", $name, $name, $slots;
	    for ($i = 0; $i < $slots; $i++) {
		my $unt = ${BUNDLE_temp[$bundle][$temp]}{'slot'}[$i]{'unit'};
		my $stp = ${BUNDLE_temp[$bundle][$temp]}{'slot'}[$i]{'stop'};
		printf $BUNDLE_F "  Slot(%d, %s_Unit); \n", $i, $unt;
		if ($stp == 1) {
		    printf $BUNDLE_F "  Stop(%d); \n", $i;
		}
	    }
            printf $BUNDLE_F "\n";
	}

        # Arthur: eventually the instr->exec_unit mapping is a
        #         part of a current bundle that start with
        #         Bundle_Begin() and ends with Bundle_End().
	printf $BUNDLE_F "\n";
	printf $BUNDLE_F "  ISA_Bundle_End(); \n";
    }

}

# ==================================================================
#    finalize_bundle_file
# ==================================================================

sub finalize_bundle_file {

    printf $BUNDLE_F "  return 0; \n";
    printf $BUNDLE_F "} \n";

    close ($BUNDLE_F);
    return;
}

# ==================================================================
#    initialize_op_file
# ==================================================================

sub initialize_op_file {

    my $dir = shift || '.';
    open ($OP_F, "> $dir/targ_op.h");
    &copyright_notice ($OP_F);

    printf $OP_F "//  \n";
    printf $OP_F "//  Generate TOP information \n";
    printf $OP_F "/////////////////////////////////////// \n\n";

    printf $OP_F "#ifndef targ_op_INCLUDED \n";
    printf $OP_F "#define targ_op_INCLUDED \n";
    printf $OP_F "\n";

    printf $OP_F "// Some of the defines are already defined here \n";
    printf $OP_F "// Set the rest to FALSE \n";
    printf $OP_F "#include \"targ_isa_properties.h\" \n";
    printf $OP_F "\n";

    printf $OP_F "// This should be on a per OP basis ?  \n";
    printf $OP_F "#define OP_COPY_OPND 1 \n";
    printf $OP_F "#define OP_PREDICATE_OPND -1     // not supported on this target \n";
    printf $OP_F "\n";
}

# ==================================================================
#    emit_top_define
# ==================================================================
sub emit_top_define {
    my $def = $_[0];
    my $val = $_[1];
    printf $OP_F "#ifndef %s \n", $def;
    printf $OP_F "#define %s(t) \t (FALSE) \n", $def;
    printf $OP_F "#endif \n";
    printf $OP_F "\n";
}

# ==================================================================
#    emit_op_info
#
#    emits a number of defines and inline functions required by the
#    cg.
# ==================================================================
sub emit_op_info {

    printf $OP_F "/* _fixed_results and _fixed_opnds return how many fixed \n";
    printf $OP_F " * results/operands an instruction has (OP_result/OP_opnds includes \n";
    printf $OP_F " * any variable operands in the count). \n";
    printf $OP_F " */ \n";
    printf $OP_F "#define TOP_fixed_results(t)	(ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(t))) \n";
    printf $OP_F "#define TOP_fixed_opnds(t)	(ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(t))) \n";
    printf $OP_F "\n";

    # the following is not clear for now:
    printf $OP_F "/* ??? */ \n";
    printf $OP_F "#define OP_inst_words(o)        (1) \n";
    printf $OP_F "\n";

    printf $OP_F "inline TOP CGTARG_Noop_Top (ISA_EXEC_UNIT_PROPERTY unit) { \n";
    printf $OP_F "  return TOP_noop; \n";
    printf $OP_F "} \n";
    printf $OP_F "\n";

    #---------------------------------------------------------------------
    # TOP_is_same_res
    #---------------------------------------------------------------------
    printf $OP_F "/* Result must not be same as operand */ \n";
    printf $OP_F "inline BOOL TOP_is_uniq_res(TOP topcode, INT i) { \n";
    printf $OP_F "  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(topcode); \n";
    printf $OP_F "  ISA_OPERAND_USE this_def = ISA_OPERAND_INFO_Def(oinfo, i); \n";
    printf $OP_F "  if (this_def & OU_uniq_res)  \n";
    printf $OP_F "    return TRUE; \n";
    printf $OP_F " \n";
    printf $OP_F "  return FALSE; \n";
    printf $OP_F "} \n";
    printf $OP_F "\n";

    #---------------------------------------------------------------------
    # TOP_is_same_res
    #---------------------------------------------------------------------
    printf $OP_F "/* Returns index of operand that must be same register as result i */ \n";
    printf $OP_F "inline INT TOP_is_same_res(TOP topcode, INT i) { \n";
    printf $OP_F "  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(topcode); \n";
    printf $OP_F "  return ISA_OPERAND_INFO_Same_Res(oinfo, i); \n";
    printf $OP_F "} \n";
    printf $OP_F "\n";

    return;
}

# ==================================================================
#    finalize_op_file
# ==================================================================

sub finalize_op_file {

    printf $OP_F "#endif /* targ_op_INCLUDED */ \n";

    close ($OP_F);
    return;
}


# ==================================================================
#    initialize_si_file
# ==================================================================

sub initialize_si_file {

    my $dir = shift || '.';
    $dir .= "/proc";
    system "mkdir -p $dir";
    open ($SI_F, "> $dir/st200_si.cxx");

    &copyright_notice ($SI_F);

    printf $SI_F "//  %s processor scheduling information \n", $ARCH_name;
    printf $SI_F "///////////////////////////////////// \n";
    printf $SI_F "//   \n";
    printf $SI_F "//  Description:  \n";
    printf $SI_F "//  \n";
    printf $SI_F "//  Generate a scheduling description of a %s processor  \n", $ARCH_name;
    printf $SI_F "//  via the si_gen interface.  \n";
    printf $SI_F "//  \n";
    printf $SI_F "/////////////////////////////////////  \n\n";

    printf $SI_F "#include \"si_gen.h\" \n";
    printf $SI_F "#include \"targ_isa_subset.h\" \n";
    printf $SI_F "#include \"topcode.h\" \n\n";

    my $format = "";
    printf $SI_F "static RESOURCE ";
    my $rid;
    for ($rid = 0; $rid < $RES_count; $rid++) {
	printf $SI_F "%s res_%s", $format, $RES_name[$rid];
	$format = ",\n	       ";
    }
    printf $SI_F "; \n\n";
    printf $SI_F "int \n";
    printf $SI_F "main (int argc, char *argv[]) \n";
    printf $SI_F "{ \n";

    for ($rid = 0; $rid < $RES_count; $rid++) {
	printf $SI_F "  res_%s = RESOURCE_Create(\"%s\", %d); \n",
		$RES_name[$rid], $RES_name[$rid], $RES_avail[$rid];
    }

    printf $SI_F "\n";
}

# ==================================================================
#    process_scdinfo
# ==================================================================
sub process_scdinfo {
    my $opcode = $_[0];

    my $subset = subset_id ($OP_subset[$opcode]);

    # Assumptions:
    # I am only using the Any_Operand_Access_Time() and
    # Any_Result_Available_Time(). So, just check that all opcodes
    # in a given scdclass satisfy this requirement.

    # Sanity check: make sure that all operands/results have same
    # access/available time
    my $res = -1;
    for (my $i = 0; $i < $OP_results[$opcode]; $i++) {
	if ($res != -1 && $res != $OP_result_avail_time[$opcode][$i]) {
	    printf STDOUT "ERROR: results of opcode %s have different available time\n", $OP_opcode[$opcode];
	exit(1);
	}
	$res = $OP_result_avail_time[$opcode][$i];
    }

#    printf STDOUT "  > any result avail time %d\n", $res;

    my $opnd = -1;
    for (my $i = 0; $i < $OP_opnds[$opcode]; $i++) {
      if ($opnd == -1) {
	$opnd = $OP_opnd_access_time[$opcode][$i];
      }
      if ($opnd != $OP_opnd_access_time[$opcode][$i]) {
	printf STDOUT "WARNING: opcode %s: operand %d have different access time %d than first operand (access time %d). Taking first operand access time %d\n", $OP_opcode[$opcode], $i, $OP_opnd_access_time[$opcode][$i], $opnd, $opnd;
	$OP_opnd_access_time[$opcode][$i] = $opnd;
      }
    }

    my $scdclass = $OP_scdclass[$opcode];

    # Sanity check and id:
    my $sc;
    for ($sc = 0; $sc < $SCD_CLASS_count; $sc++) {
	if ($scdclass eq $SCD_CLASS_name[$sc]) {
	    goto OK;
	}
    }
    printf STDOUT "ERROR: scd class %s in process_scdinfo has not been declared\n", $scdclass;
    exit(1);

OK:

    # Sanity checks !
    if ($SCD_CLASS_results[$sc] < $OP_results[$opcode]) {
	printf STDOUT "ERROR: opcode %s has more results than scd class %s\n", $OP_opcode[$opcode], $scdclass;
	exit(1);
    }
    if ($SCD_CLASS_opnds[$sc] < $OP_opnds[$opcode]) {
	printf STDOUT "ERROR: opcode %s has more operands than scd class %s\n", $OP_opcode[$opcode], $scdclass;
	exit(1);
    }

    for (my $i = 0; $i < $OP_results[$opcode]; $i++) {
	if ($SCD_CLASS_result[$sc][$subset][$i] != -1 &&
	    $SCD_CLASS_result[$sc][$subset][$i] != $OP_result_avail_time[$opcode][$i]) {
	    printf STDOUT "ERROR: result %d for scdclass %s is being reset\n", $i, $scdclass;
	    printf STDOUT "scdclass %d, results %d\n", $SCD_CLASS_result[$sc][$subset][$i], $OP_result_avail_time[$opcode][$i];

	    exit(1);
	}
	$SCD_CLASS_result[$sc][$subset][$i] = $OP_result_avail_time[$opcode][$i];
    }

    for (my $i = 0; $i < $OP_opnds[$opcode]; $i++) {
	if ($SCD_CLASS_opnd[$sc][$subset][$i] != -1 &&
	    $SCD_CLASS_opnd[$sc][$subset][$i] != $OP_opnd_access_time[$opcode][$i]) {
	    printf STDOUT "ERROR: operand %d for opcode %s is being reset in subset %s\n",
		$i, $OP_opcode[$opcode], $SUBSET_name[$subset];
	    printf STDOUT "%d scdclass %d, opns %d\n",
		$opcode, $SCD_CLASS_opnd[$sc][$subset][$i], $OP_opnd_access_time[$opcode][$i];
	    exit(1);
	}
	$SCD_CLASS_opnd[$sc][$subset][$i] = $OP_opnd_access_time[$opcode][$i];
    }

    $subset = $OP_subset[$opcode];

    # ISSUE: TOPs are issued at cycle 0
    $SUBSET_scd{$subset}{$scdclass}{'res'} = [];

    if ($scdclass eq 'ALU') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['INT', 0 ]);
    } elsif ($scdclass eq 'ALU_IMM') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['INT', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    } elsif ($scdclass eq 'MUL') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['MUL', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    } elsif ($scdclass eq 'MUL_IMM') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['MUL', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    } elsif ($scdclass eq 'LOAD' ||
	     $scdclass eq 'LOAD2' ||
	     $scdclass eq 'STORE') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['MEM', 0 ]);
    } elsif ($scdclass eq 'LOAD_IMM' ||
	     $scdclass eq 'LOAD2_IMM' ||
	     $scdclass eq 'STORE_IMM') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['MEM', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    }
    elsif ($scdclass eq 'EXT') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    }
    elsif ($scdclass eq 'BRANCH') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['XFER', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    }
    elsif ($scdclass eq 'CALL') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    }
    elsif ($scdclass eq 'JUMP') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
    }
    elsif ($scdclass eq 'SYNC') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
    }
    elsif ($scdclass eq 'asm' ||
	   $scdclass eq 'PRGINS') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
    }
    elsif ($scdclass eq 'getpc') {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ODD', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['XFER', 0 ]);
    }
    else {
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['ISSUE', 0 ]);
      push(@{$SUBSET_scd{$subset}{$scdclass}{'res'}}, ['INT', 0 ]);
    }

    # MEM: used at cycle 0.
 
    # Insert this TOP into the list:
    push (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}, $OP_opcode[$opcode]);
#    printf STDOUT "  > pushing %s (%s,%s)\n", $OP_opcode[$opcode], $subset, $scdclass;

    return;
}

# ==================================================================
#    read_scdinfo
#
#    Information is in st220/arc.db file
# ==================================================================

sub read_scdinfo {
    my $opcode = $_[0];
    my $subset = $OP_subset[$opcode];
    my $line;

    my $opc_file_name = "../src/$subset/arc.db";

#    printf STDOUT "  *** reading schedinfo ***\n";
#    printf STDOUT "  > read_scdinfo: for op %s subset %s\n", $OP_opcode[$opcode], $subset;

    if (!open (ARCH_F, "$opc_file_name")) {
	printf STDOUT "error: can't open file \"$opc_file_name\" \n";
	exit(1);
    }

    #------------------------------------------------------
    # + =======...======= + starts a new record
    # +=add
    # ...
    # <blank line> ends it
    #------------------------------------------------------


    # look for line +=<opcode>

    my $mnemonic = "";
    my $resources;
    my $scdclass;
    my $results;
    my $opnds;

    my $MAX_RESULTS = 2;
    my $MAX_OPNDS = 3;
    my @results;
    my @opnds;

    while ($line = <ARCH_F>) {

	if ($line =~ /^\+=(\S*)/) {
	    # found a new record, read the info until next record:
	    $mnemonic = $1;
	    $mnemonic =~ tr/./_/;

	    # initialize results/opnds for this opcode.
	    # we will be able to check whether all have been specified.
	    for (my $i = 0; $i < $MAX_RESULTS; $i++) {
		$results[$i] = -1;
	    }
	    for (my $i = 0; $i < $MAX_OPNDS; $i++) {
		$opnds[$i] = -1;
	    }

	}

	# resource usage information:
	if ($line =~ /^\+resources\s(\S*)/) {
	    $resources = $1;
	}

	# pipeline latencies information:
	if ($line =~ /^\+op(\d)\sname=result(\d),\sstage=(\d)/) {
	    my $ridx = $2-1;
	    my $cycle = $3;

	    $results[$ridx] = $cycle;

#	    printf STDOUT "  result[%d] = %d\n", $ridx, $results[$ridx];

	}

	if ($line =~ /^\+op(\d)\sname=operand(\d),\sstage=(\d)/) {
	    my $oidx = $2-1;
	    my $cycle = $3;
	    $opnds[$oidx] = $cycle;

#	    printf STDOUT " line = %s\n", $line;
#	    printf STDOUT "  operand[%d] = %d\n", $oidx, $opnds[$oidx];

	}

	if ($line =~ /^\n/) {
	    # end of record -- check whether it was the right opcode ?
	    # if not, go to next record.
	    if ($OP_mnemonic[$opcode] eq $mnemonic) {
		goto FOUND_OPCODE;
	    }
	}
    }

    printf STDOUT "ERROR: read_scdinfo -- couldn't find record for opcode %s \n", $opcode;
    exit(1);

FOUND_OPCODE:
    # some additional latency information that's not in 
    # the arc file
    if ($OP_opcode[$opcode] eq 'call' ||
	$OP_opcode[$opcode] eq 'icall') {
	$results[0] = 5;
    }
    if ($OP_opcode[$opcode] eq 'call' ||
	$OP_opcode[$opcode] eq 'icall' ||
	$OP_opcode[$opcode] eq 'imml' ||
	$OP_opcode[$opcode] eq 'immr') {
	$opnds[0] = 1;
    }
    if ($OP_opcode[$opcode] eq 'igoto') {
	$opnds[0] = 1;
    }

    # Sanity check:
    for (my $i = 0; $i < $OP_results[$opcode]; $i++) {
	if ($results[$i] == -1) {
	    printf STDOUT "ERROR: result %d for opcode %s not defined\n", $i, $OP_opcode[$opcode];
	    exit(1);
	}
	$OP_result_avail_time[$opcode][$i] = $results[$i];
    }
    for (my $i = 0; $i < $OP_opnds[$opcode]; $i++) {
	if ($opnds[$i] == -1) {
	    printf STDOUT "ERROR: operand %d for opcode %s not defined\n", $i, $OP_opcode[$opcode];
	    exit(1);
	}
	$OP_opnd_access_time[$opcode][$i] = $opnds[$i];
    }

    # determine this opcode's scdclass:
#    printf STDOUT " %s > %s\n", $resources, $OP_format[$opcode];

    # resources corresponds to a scheduling class:
    if ($resources eq 'ALU') {
	$scdclass = 'ALU';
    }
    elsif ($resources eq 'ASM') {
	$scdclass = 'ALU';
    }
    elsif ($resources eq 'MUL') {
	$scdclass = 'MUL';
    }
    elsif ($resources eq 'LOAD') {
	if ($subset eq "st210" && ($mnemonic eq 'ldw' || $mnemonic eq 'ldw_d')) {
	    $scdclass = 'LOAD2';	    
	}
	else {
	    $scdclass = 'LOAD';
	}
    }
    elsif ($resources eq 'STORE') {
	$scdclass = 'STORE';
    }
    elsif ($resources eq 'BRANCH') {
	$scdclass = 'BRANCH';
    }
    elsif ($resources eq 'CALL') {
	if ($mnemonic eq 'goto' || $mnemonic eq 'igoto') {
	    $scdclass = 'JUMP';
	} else {
	    $scdclass = 'CALL';
	}
    }
    elsif ($resources eq 'IMM') {
	$scdclass = 'EXT';
    }
    else {
	printf STDOUT "ERROR: unknown resource %s in read_scdinfo\n", $resources;
	exit(1);
    }

    # Special scdclass cases
    # treat special instructions such as sync, prgins
    if ($mnemonic eq 'sync' ||
	$mnemonic eq 'syncins') {
      $scdclass = 'SYNC';
    } elsif ($mnemonic eq 'prgins') {
      $scdclass = 'PRGINS';
    }

    $OP_scdclass[$opcode] = $scdclass;
    &process_scdinfo($opcode);

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
#    &read_scdinfo();

    # For each ISA subset:
    my $sub;
CONTINUE:
    for ($sub = 0; $sub < $SUBSET_count; $sub++) {
	my $subset = $SUBSET_name[$sub];

	printf $SI_F "  /* ======================================================\n";
	printf $SI_F "   * Resource description for the ISA_SUBSET_%s \n", $subset;
	printf $SI_F "   * ======================================================\n";
	printf $SI_F "   */ \n\n";

	printf $SI_F "  Machine(\"%s\", ISA_SUBSET_%s, argc, argv); \n", $ARCH_name, $subset;

	# emit groups of instructions with similar resource constraints:
	my $i;
	for ($i = 0; $i < $SCD_CLASS_count; $i++) {
	    $scdclass = $SCD_CLASS_name[$i];
            printf $SI_F "\n  ///////////////////////////////////////// \n";
            printf $SI_F "  //   Instructions for Scd Class %s \n", $scdclass;
            printf $SI_F "  ///////////////////////////////////////// \n\n";

	    printf $SI_F "  Instruction_Group(\"%s\", \n", $scdclass;
            foreach $opcode (@{$SUBSET_scd{$subset}{$scdclass}{'ops'}}) {
		printf $SI_F "\t\t TOP_%s, \n", $opcode;
	    }
	    printf $SI_F "\t\t TOP_UNDEFINED); \n\n";

            # define latencies:

	    # since I suppose they're all same:
	    if ($SCD_CLASS_results[$i] > 0) {
		printf $SI_F "  Any_Result_Available_Time(%d); \n", $SCD_CLASS_result[$i][$sub][0];
	    }

	    if ($SCD_CLASS_opnds[$i] > 0) {
		printf $SI_F "  Any_Operand_Access_Time(%d); \n", $SCD_CLASS_opnd[$i][$sub][0];
	    }

#	    printf $SI_F "  Any_Operand_Access_Time(0); \n";
#	    printf $SI_F "  Any_Result_Available_Time(1);		// ??? not sure \n";

            # define resource requirements:
	    my $rid;
	    foreach my $pair (@{$SUBSET_scd{$subset}{$scdclass}{'res'}}) {
		my $rname = $pair->[0];
		my $cycle = $pair->[1];
		printf $SI_F "  Resource_Requirement(res_%s, %d); \n", $rname, $cycle;
            }
	    printf $SI_F "\n";
	}
	printf $SI_F "  Machine_Done(\"%s.c\"); \n\n", $subset;
    }

    return;
}

# ==================================================================
#    finalize_si_file
# ==================================================================

sub finalize_si_file {

    printf ($SI_F "}\n");

    close ($SI_F);
    return;
}




# ==================================================================
#    copyright_notice
# ==================================================================

sub copyright_notice {

    my $FILE = $_[0];

    printf $FILE "/* -- This file is automatically generated -- */ \n";
    printf $FILE "/* \n\n";

    printf $FILE "  Copyright (C) 2002 ST Microelectronics, Inc.  All Rights Reserved. \n\n";

    printf $FILE "  This program is free software; you can redistribute it and/or modify it \n";
    printf $FILE "  under the terms of version 2 of the GNU General Public License as \n";
    printf $FILE "  published by the Free Software Foundation. \n";

    printf $FILE "  This program is distributed in the hope that it would be useful, but \n";
    printf $FILE "  WITHOUT ANY WARRANTY; without even the implied warranty of \n";
    printf $FILE "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. \n\n";

    printf $FILE "  Further, this software is distributed without any warranty that it is \n";
    printf $FILE "  free of the rightful claim of any third person regarding infringement \n";
    printf $FILE "  or the like.  Any license provided herein, whether implied or \n";
    printf $FILE "  otherwise, applies only to this software file.  Patent licenses, if \n";
    printf $FILE "  any, provided herein do not apply to combinations of this program with \n";
    printf $FILE "  other software, or any other product whatsoever. \n";

    printf $FILE "  You should have received a copy of the GNU General Public License along \n";
    printf $FILE "  with this program; if not, write the Free Software Foundation, Inc., 59 \n";
    printf $FILE "  Temple Place - Suite 330, Boston MA 02111-1307, USA. \n\n";

    printf $FILE "  Contact information:  ST Microelectronics, Inc., \n";
    printf $FILE "  , or: \n\n";

    printf $FILE "  http://www.st.com \n\n";

    printf $FILE "  For further information regarding this notice, see: \n\n";

    printf $FILE "  http: \n";

    printf $FILE "*/ \n\n";
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
    $SUBSET_parent[$SUBSET_count] = $_[0];
    $SUBSET_name[$SUBSET_count] = $_[1];
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
    printf STDOUT "ERROR: subset %s undeclared\n", $name;
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
    printf STDOUT "ERROR: invalid rclass name %s\n", $rclass_name;
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

#    printf STDOUT "reg_subset %s %d idx %d\n", $name, $cur_regset, $idx;

    $REGSET_subset[$cur_regset][$idx]{'name'} = $name;
    $REGSET_subset[$cur_regset][$idx]{'rclass'} = $REGSET_rclass[$cur_regset];

    for ($reg = $REGSET_minreg[$cur_regset];
	 $reg <= $REGSET_maxreg[$cur_regset];
	 $reg++) {
	$REGSET_subset[$cur_regset][$idx]{'regs'}[$reg] = 0;
    }

    while ((my $elt = shift(@regs)) != -1) {
	for ($reg = $REGSET_minreg[$cur_regset];
	     $reg <= $REGSET_maxreg[$cur_regset];
	     $reg++) {
	    if ($reg == $elt) {
		$REGSET_subset[$cur_regset][$idx]{'regs'}[$reg] = 1;
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

#    for ($i = 0; $i < scalar(@names); $i++) {
#	print STDOUT "$REGSET_names[$REGSET_count-1][$i] \n";
#    }

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
    printf STDOUT "ERROR: invalid lclass name %s\n", $lclass_name;
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
    printf STDOUT "ERROR: invalid eclass name %s\n", $eclass_name;
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
    my $name = $_[0];
    my $results = $_[1];
    my $opnds = $_[2];
    my $i;

    $SCD_CLASS_name[$SCD_CLASS_count] = $name;
    $SCD_CLASS_results[$SCD_CLASS_count] = $results;
    for (my $sub = 0; $sub < $SUBSET_count; $sub++) {
	for ($i = 0; $i < $results; $i++) {
	    $SCD_CLASS_result[$SCD_CLASS_count][$sub][$i] = -1;
	}
	$SCD_CLASS_opnds[$SCD_CLASS_count] = $opnds;
	for ($i = 0; $i < $opnds; $i++) {
	    $SCD_CLASS_opnd[$SCD_CLASS_count][$sub][$i] = -1;
	}
    }
    $SCD_CLASS_count++;
}

sub scdclass_id {
    my $name = $_[0];
    my $id;

    for ($id = 0; $id < $SCD_CLASS_count; $id++) {
	if ($SCD_CLASS_name[$id] eq $name) {
	    return $id;
	}
    }

    printf STDOUT "ERROR: invalid scdclass name %s\n", $name;
    exit(1);
}

# ==================================================================
#    DECL_PACK_TYPE
# ==================================================================

sub DECL_PACK_TYPE {
    my $type = $_[0];
    my $results = $_[1];
    my $opnds = $_[2];
    my $comment = $_[3];

    $PACK_type[$PACK_count] = $type;
    $PACK_results[$PACK_count] = $results;
    $PACK_opnds[$PACK_count] = $opnds;
    $PACK_comment[$PACK_count] = $comment;
    $PACK_words[$PACK_count] = 1;

    # mark operands/results that go to each word (till max_words = 2)
    my $word;
    for ($word = 0; $word < 2; $word++) {
	my $i;
	for ($i = 0; $i < $results; $i++) {
	    $PACK_word_results[$PACK_count][$i][$word] = 0;
	}
	for ($i = 0; $i < $opnds; $i++) {
	    $PACK_word_opnds[$PACK_count][$i][$word] = 0;
	}
    }

    $current_pack = $PACK_count;
    $PACK_count++;
}

# ==================================================================
#    NEXT_WORD
# ==================================================================
sub NEXT_WORD {
    $PACK_words[$current_pack]++;
}

# ==================================================================
#    PACK_OPERAND
#
#    operand 'idx'; take 'bits' bits starting from bit 'start' and
#    place starting from 'instr' into instruction word.
# ==================================================================
sub PACK_OPERAND {
    my $idx = $_[0];
    my $start = $_[1];
    my $instr = $_[2];
    my $bits = $_[3];
    # current instruction word:
    my $word = $PACK_words[$current_pack]-1;

    $PACK_word_opnds[$current_pack][$idx][$word] = 1;

    $PACK_opnd_start[$current_pack][$idx][$word] = $start;
    $PACK_opnd_bits[$current_pack][$idx][$word] = $bits;
    $PACK_opnd_ipos[$current_pack][$idx][$word] = $instr;
}

# ==================================================================
#    PACK_RESULT
#
#    result 'idx'; take 'bits' bits starting from bit 'start' and
#    place starting from 'instr' into instruction word.
# ==================================================================
sub PACK_RESULT {
    my $idx = $_[0];
    my $start = $_[1];
    my $instr = $_[2];
    my $bits = $_[3];
    # current instruction word:
    my $word = $PACK_words[$current_pack]-1;

    $PACK_word_results[$current_pack][$idx][$word] = 1;

    $PACK_result_start[$current_pack][$idx][$word] = $start;
    $PACK_result_bits[$current_pack][$idx][$word] = $bits;
    $PACK_result_ipos[$current_pack][$idx][$word] = $instr;
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

#    printf STDOUT "BUNDLE %s %d slots: \n",
#	   $BUNDLE_name[$BUNDLE_count],
#	   $BUNDLE_slots[$BUNDLE_count];

    for ($i = 0; $i < $_[3]; $i++) {
	my ($id, $start, $bits) = split(",", $_[$i+4]);
	$BUNDLE_slot[$BUNDLE_count][$i]{'id'} = $id;
	$BUNDLE_slot[$BUNDLE_count][$i]{'start'} = $start;
	$BUNDLE_slot[$BUNDLE_count][$i]{'bits'} = $bits;

#	printf STDOUT "  DECL_SLOT %s -- (%d, 0, %d, %d)\n", $_[$i+4],
#	       $BUNDLE_slot[$BUNDLE_count][$i]{'id'},
#	       $BUNDLE_slot[$BUNDLE_count][$i]{'start'},
#	       $BUNDLE_slot[$BUNDLE_count][$i]{'bits'};

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
	printf STDOUT "ERROR: invalid slot number %d\n", $_[0];
	exit(1);
    }
    # slot refers to non-existing FU
    for ($i = 0; $i < $EXEC_SLOT_count; $i++) {
	if ($_[1] eq $EXEC_SLOT_name[$i]) {
	    goto BREAK;
	}
    }
    printf STDOUT "ERROR: invalid slot kind %d\n", $_[1];
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
	printf STDOUT "ERROR: invalid stop number %d\n", $_[0];
	exit(1);
    }

    ${BUNDLE_temp[$cur_bundle][$cur_temp]}{'slot'}[$_[0]]{'stop'} = 1;
}

# ==================================================================
#    process_opcode
# ==================================================================
sub process_opcode {
    my $opcode = $_[0];
    my $subset = $_[1];

    # variant:
    &set_variant($opcode);

    $OP_opcode[$opcode] = $OP_mnemonic[$opcode].$OP_variant[$opcode];

    # depending on OP format, set number of opnds/results
    &set_operands($opcode);
    # operand groups:
    &set_signature($opcode);
    # pack groups:
    &set_pack($opcode);
    # print groups:
    &set_print($opcode);

    # Figure out OPcode properties:
    if ($OP_mnemonic[$opcode] eq 'pft') {
	$OP_properties[$opcode] ^= $OP_PREFETCH;
    }
    elsif ($OP_mnemonic[$opcode] eq 'prgadd' ||
	   $OP_mnemonic[$opcode] eq 'prginspg' ||
	   $OP_mnemonic[$opcode] eq 'prgset') {
	# purge cache is marked as Store but is not
    }
    elsif ($OP_mnemonic[$opcode] eq 'sync') {
	# sync is marked as Store but is not
    }
    else {
	# set up properties:
	&sort_by_properties($opcode);
    }

    # only one subset for now:
    $OP_subset[$opcode] = $SUBSET_name[$subset];
    push(@{$SUBSET_opcodes[$subset]}, $opcode);

    # operand groups:
    push(@{$SignatureGroup{$OP_signature[$opcode]}}, $OP_opcode[$opcode]);

    # pack groups:
    push(@{$PackGroup{$OP_pack[$opcode]}}, $OP_opcode[$opcode]);

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
	push(@{$AttrGroup{'cond'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_JUMP) {
	push(@{$AttrGroup{'jump'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_IJUMP) {
	push(@{$AttrGroup{'ijump'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_XFER) {
	push(@{$AttrGroup{'xfer'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_MOVE) {
	push(@{$AttrGroup{'move'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_NOP) {
	push(@{$AttrGroup{'noop'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_PREFETCH) {
	push(@{$AttrGroup{'prefetch'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_MUL) {
	push(@{$AttrGroup{'mul'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_ADD) {
	push(@{$AttrGroup{'add'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_SUB) {
	push(@{$AttrGroup{'sub'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_OR) {
	push(@{$AttrGroup{'or'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_AND) {
	push(@{$AttrGroup{'and'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_XOR) {
	push(@{$AttrGroup{'xor'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_SHL) {
	push(@{$AttrGroup{'shl'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_SHR) {
	push(@{$AttrGroup{'shr'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_SHRU) {
	push(@{$AttrGroup{'shru'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_SEXT) {
	push(@{$AttrGroup{'sext'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_ZEXT) {
	push(@{$AttrGroup{'zext'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_SELECT) {
	push(@{$AttrGroup{'select'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_UNSIGNED) {
	push(@{$AttrGroup{'unsign'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_INTOP) {
	push(@{$AttrGroup{'intop'}}, $OP_opcode[$opcode]);
    }

    if ($OP_properties[$opcode] & $OP_CMP) {
	push(@{$AttrGroup{'cmp'}}, $OP_opcode[$opcode]);
    }

#    if ($OP_properties[$opcode] & $OP_IMMEDIATE) {
#	push(@{$AttrGroup{'immediate'}}, $OP_opcode[$opcode]);
#    }

    return;
}


# ==================================================================
#    read_opcodes
# ==================================================================

sub read_opcodes {
    my $subset = $_[0];
    my $subset_name = $SUBSET_name[$subset];
    my $opc_file_name = "../src/$subset_name/opc.db";

    printf STDOUT " reading subset %s from %s\n", $subset_name, $opc_file_name;
      
    if (!open (OPCODE_F, "$opc_file_name")) {
	printf STDOUT "error: can't open file \"$opc_file_name\" \n";
	exit(1);
    }

    # read OPCODE_F file:
    my $line;
    my $i;

    my $property;
    my $format;
    my $mnemonic;
    my $syntax;
    my $scdclass;

  CONTINUE:
    while ($line = <OPCODE_F>) {
	# Read one OPeration record:

	# Records start with 
	#      +=s. ... 
	# and end with
	#      +format
	#
	if ($line =~ /^\+=s./) {
	    # beginning of new record:
	    # reset OP_properties:
	    $property = $OP_NONE;
	}

#	printf STDOUT "line = %s\n", $line;

	# opcode's mnemonic:
	if ($line =~ /^\+mnemonic\s(\S*)\n/) {
	    $mnemonic = $1;
	    $mnemonic =~ tr/./_/;
	}
      
	# opcode's assembly printing format
	if ($line =~ /^\+syntax\s(.*)/) {
	    $syntax = $1;
	}

	# opcode's format is the last record, process opcode
	if ($line =~ /^\+format\s(\w*)/) {
	    # skip processing of some opcodes:
	    if ($mnemonic eq 'asm' ||
		$mnemonic eq 'recv' ||
		$mnemonic eq 'send') {
		goto CONTINUE;
	    }
	    elsif ($mnemonic eq 'sync') {
		$format = 'Sync';
	    }
	    elsif ($mnemonic eq 'goto') {
		$format = 'Jump';
	    }
	    elsif ($mnemonic eq 'igoto') {
		$format = 'Ijump';
	    }
	    elsif ($mnemonic eq 'bswap' ||
		   $mnemonic eq 'sxth' ||
		   $mnemonic eq 'sxtb' ||
		   $mnemonic eq 'zxth') {
		$format = 'Monadic';
	    }
	    else {
		$format = $1;
	    }


	    $OP_properties[$OP_count] = $property;
	    $OP_mnemonic[$OP_count] = $mnemonic;
	    $OP_syntax[$OP_count] = $syntax;
	    $OP_format[$OP_count] = $format;

	    # process the opcode:
	    &process_opcode($OP_count, $subset);
	    &read_scdinfo($OP_count);
	    $OP_count++;

	    # if it is one of opcodes that accepts an immediate
	    # as operand, create a "twin" opcode of extended format

	    my $format;
	    if ($OP_format[$OP_count-1] eq 'Int3I') {
		$format = 'Int3E';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'Mul64I') {
		$format = 'Mul64E';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'MoveI') {
		$format = 'MoveE';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'Cmp3I_Reg') {
		$format = 'Cmp3E_Reg';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'Cmp3I_Br') {
		$format = 'Cmp3E_Br';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'SelectI') {
		$format = 'SelectE';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'Store') {
		$format = 'StoreE';
	    }
	    elsif ($OP_format[$OP_count-1] eq 'Load') {
		$format = 'LoadE';
	    }
	    else {
		goto CONTINUE;
	    }

	    # make a "twin" opcode:

	    $OP_mnemonic[$OP_count] = $OP_mnemonic[$OP_count-1];
	    $OP_properties[$OP_count] = $OP_NONE;
	    $OP_format[$OP_count] = $format;
	    $OP_syntax[$OP_count] = $OP_syntax[$OP_count-1];

	    &process_opcode($OP_count, $subset);

	    # initialize the scheduling info:

	    for ($i = 0; $i < $OP_results[$OP_count]; $i++) {
		$OP_result_avail_time[$OP_count][$i] = 
		    $OP_result_avail_time[$OP_count-1][$i];
	    }

	    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
		$OP_opnd_access_time[$OP_count][$i] = 
		    $OP_opnd_access_time[$OP_count-1][$i];
	    }

	    $scdclass = $OP_scdclass[$OP_count-1];
	    if ($scdclass eq 'ALU') {
		$scdclass = 'ALU_IMM';
	    } elsif ($scdclass eq 'STORE') {
		$scdclass = 'STORE_IMM';
	    } elsif ($scdclass eq 'STORE') {
		$scdclass = 'STORE_IMM';
	    } elsif ($scdclass eq 'LOAD') {
		$scdclass = 'LOAD_IMM';
	    } elsif ($scdclass eq 'LOAD2') {
		$scdclass = 'LOAD2_IMM';
	    } elsif ($scdclass eq 'MUL') {
		$scdclass = 'MUL_IMM';
	    }
	    $OP_scdclass[$OP_count] = $scdclass;
	    
	    &process_scdinfo($OP_count);

	    $OP_count++;

	} # finish current record
    } # while there are records

    close(OPCODE_F);

    # There is a number of opcodes that I need to specify manually:

    # ----------------- nop ------------------

    # beginning of a new record

    $OP_mnemonic[$OP_count] = 'nop';
    $OP_format[$OP_count] = 'Noop';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_properties[$OP_count] ^= $OP_NOP;
    $OP_syntax[$OP_count] = 'nop';
    &process_opcode($OP_count, $subset);

    $OP_scdclass[$OP_count] = "ALU";
    &process_scdinfo($OP_count);

    $OP_count++;

    # ----------------- mov ------------------

    # beginning of a new recore

    $OP_mnemonic[$OP_count] = 'mov';
    $OP_format[$OP_count] = 'MoveR';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_syntax[$OP_count] = 'mov %1 = %2';
    &process_opcode($OP_count, $subset);

    for ($i = 0; $i < $OP_results[$OP_count]; $i++) {
	$OP_result_avail_time[$OP_count][$i] = 3;
    }

    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
	$OP_opnd_access_time[$OP_count][$i] = 2;
    }

    $OP_scdclass[$OP_count] = "ALU";
    &process_scdinfo($OP_count);

    $OP_count++;

    # ----------------- mov ------------------

    # beginning of a new recore
    $OP_mnemonic[$OP_count] = 'mov';
    $OP_format[$OP_count] = 'MoveI';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_syntax[$OP_count] = 'mov %1 = %2';
    &process_opcode($OP_count, $subset);

    for ($i = 0; $i < $OP_results[$OP_count]; $i++) {
	$OP_result_avail_time[$OP_count][$i] = 3;
    }

    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
	$OP_opnd_access_time[$OP_count][$i] = 2;
    }

    $OP_scdclass[$OP_count] = "ALU";
    &process_scdinfo($OP_count);

    $OP_count++;

    # ----------------- mov (extended) ------------------

    # beginning of a new recore
    $OP_mnemonic[$OP_count] = 'mov';
    $OP_format[$OP_count] = 'MoveE';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_syntax[$OP_count] = 'mov %1 = %2';
    &process_opcode($OP_count, $subset);

    for ($i = 0; $i < $OP_results[$OP_count]; $i++) {
	$OP_result_avail_time[$OP_count][$i] = 3;
    }

    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
	$OP_opnd_access_time[$OP_count][$i] = 2;
    }

    $OP_scdclass[$OP_count] = "ALU_IMM";
    &process_scdinfo($OP_count);
    
    $OP_count++;

    # ----------------- mtb ------------------

    # beginning of a new recore
    $OP_mnemonic[$OP_count] = 'mtb';
    $OP_format[$OP_count] = 'mtb';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_syntax[$OP_count] = 'mtb %1 = %2';
    &process_opcode($OP_count, $subset);

    for ($i = 0; $i < $OP_results[$OP_count]; $i++) {
	$OP_result_avail_time[$OP_count][$i] = 3;
    }

    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
	$OP_opnd_access_time[$OP_count][$i] = 2;
    }

    $OP_scdclass[$OP_count] = "ALU";
    &process_scdinfo($OP_count);

    $OP_count++;

    # ----------------- mfb ------------------

    # beginning of a new recore
    $OP_mnemonic[$OP_count] = 'mfb';
    $OP_format[$OP_count] = 'mfb';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_syntax[$OP_count] = 'mfb %1 = %2';
    &process_opcode($OP_count, $subset);

    for ($i = 0; $i < $OP_results[$OP_count]; $i++) {
	$OP_result_avail_time[$OP_count][$i] = 3;
    }

    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
	$OP_opnd_access_time[$OP_count][$i] = 2;
    }

    $OP_scdclass[$OP_count] = "ALU";
    &process_scdinfo($OP_count);

    $OP_count++;

    # ----------------- return ------------------

    # beginning of a new recore
    $OP_mnemonic[$OP_count] = 'return';
    $OP_format[$OP_count] = 'Ijump';
    $OP_properties[$OP_count] = $OP_NONE;
    $OP_syntax[$OP_count] = 'return LR';

    &process_opcode($OP_count, $subset);

    for ($i = 0; $i < $OP_opnds[$OP_count]; $i++) {
	$OP_opnd_access_time[$OP_count][$i] = 1;
    }

    $OP_scdclass[$OP_count] = "JUMP";
    &process_scdinfo($OP_count);
    
    $OP_count++;
}

# ==================================================================
#    main
# ==================================================================

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

  my %options;
  use Getopt::Std;
  getopts('o:s:', \%options);
  my $subset = $options{s} || '';
  my $archdir = $options{o} || "st200";
  
  unless ($subset =~ /^st2/) {
    print STDERR "usage: $0 -s subsetdir [-o outputdir]\n";
    exit 1;
  }

  &DECL_ARCHITECTURE("st200");

  &DECL_ISA_SUBSET ("NULL", $subset);
  &DECL_ISA_SUBSET ($subset, "st200");

#  &DECL_ISA_SUBSET (st220, "st210");
#  &DECL_ISA_SUBSET (st210, "st200");

  # Following properties are required by the Pro64:
  &DECL_ISA_PROPERTY("noop");
  &DECL_ISA_PROPERTY("load");
  &DECL_ISA_PROPERTY("store");
  &DECL_ISA_PROPERTY("jump");
  &DECL_ISA_PROPERTY("ijump");
  &DECL_ISA_PROPERTY("cond");
  &DECL_ISA_PROPERTY("call");
  &DECL_ISA_PROPERTY("simulated");
  &DECL_ISA_PROPERTY("dummy");
  &DECL_ISA_PROPERTY("var_opnds");

  # Some additional properties:
  &DECL_ISA_PROPERTY("dismissible");
  &DECL_ISA_PROPERTY("xfer");
  &DECL_ISA_PROPERTY("move");
  &DECL_ISA_PROPERTY("prefetch");
  &DECL_ISA_PROPERTY("mul");
  &DECL_ISA_PROPERTY("sub");
  &DECL_ISA_PROPERTY("add");

  &DECL_ISA_PROPERTY("Xor");
  &DECL_ISA_PROPERTY("And");
  &DECL_ISA_PROPERTY("Or");
  &DECL_ISA_PROPERTY("shl");
  &DECL_ISA_PROPERTY("shr");
  &DECL_ISA_PROPERTY("shru");
  &DECL_ISA_PROPERTY("zext");
  &DECL_ISA_PROPERTY("sext");

  &DECL_ISA_PROPERTY("intop");
  &DECL_ISA_PROPERTY("select");
  &DECL_ISA_PROPERTY("unsign");
  &DECL_ISA_PROPERTY("cmp");

  # Needed to compile. no op associated.
  &DECL_ISA_PROPERTY("l_group");
  &DECL_ISA_PROPERTY("f_group");
  &DECL_ISA_PROPERTY("flop");
  &DECL_ISA_PROPERTY("madd");
  &DECL_ISA_PROPERTY("mem_fill_type");
  &DECL_ISA_PROPERTY("guard_t");
  &DECL_ISA_PROPERTY("guard_f");
  &DECL_ISA_PROPERTY("div");
  &DECL_ISA_PROPERTY("unsafe");
  &DECL_ISA_PROPERTY("likely");
  &DECL_ISA_PROPERTY("unalign");
  &DECL_ISA_PROPERTY("access_reg_bank");
  &DECL_ISA_PROPERTY("side_effects");
  &DECL_ISA_PROPERTY("branch_predict");
  &DECL_ISA_PROPERTY("refs_fcr");
  &DECL_ISA_PROPERTY("defs_fcc");
  &DECL_ISA_PROPERTY("defs_fcr");

  # rclass (name, bit_size, is_ptr, can_be_stored, mult_store):

  &DECL_RCLASS("integer", 32, $TRUE, $TRUE, $FALSE);
  &DECL_RCLASS("branch", 1, $FALSE, $TRUE, $FALSE);

  # Register Set (rclass, minreg, maxreg, format, reg_names):
  # For each set define also Reg subset (name, list_of_members):
  my @int_regs = ('$r0', '$r1', '$r2');
  &DECL_REG_SET("integer", 0, 63, "\$r%d", @int_regs);
    &DECL_REG_SUBSET("r0", (0));
    &DECL_REG_SUBSET("lr", (63));
    &DECL_REG_SUBSET("no_lr", (0, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
			       17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
			       33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
			       49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62));

  my @br_regs = ('$b0');
  &DECL_REG_SET("branch", 0, 7, "\$b%d", @br_regs);

  # lclass (name, bit_size, signed?):

  &DECL_LCLASS("s32", 32, $SIGNED);
  &DECL_LCLASS("u32", 32, $UNSIGNED);
  &DECL_LCLASS("s23", 23, $SIGNED);
  &DECL_LCLASS("u23", 23, $UNSIGNED);
  &DECL_LCLASS("s9", 9, $SIGNED);

#  &DECL_ECLASS("ibus", 9);
#    &ENUM("c0", 0);
#    &ENUM("c1", 1);
#    &ENUM("c2", 2);
#    &ENUM("c3", 3);
#    &ENUM("c4", 4);
#    &ENUM("c5", 5);
#    &ENUM("c6", 6);
#    &ENUM("c7", 7);
#    &ENUM("c8", 8);

  # register operand (name, rclass, reg_subclass, rtype):

  &DECL_REG_OPND("bcond", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("scond", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("bdest", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("ibdest", "branch", "UNDEFINED", $UNSIGNED);
  &DECL_REG_OPND("dest", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("dest2", "integer", "no_lr", $SIGNED);
  &DECL_REG_OPND("idest", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("idest2", "integer", "no_lr", $SIGNED);
  &DECL_REG_OPND("src1", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("src2", "integer", "UNDEFINED", $SIGNED);
  &DECL_REG_OPND("lr", "integer", "lr", $UNSIGNED);

  # literal operand (name, lclass, rtype):

  &DECL_LIT_OPND("btarg","s23", $SIGNED);
  &DECL_LIT_OPND("isrc2","s32", $SIGNED);  
#  &DECL_LIT_OPND("usrc2","u32", $UNSIGNED);
  &DECL_LIT_OPND("s9","s9", $SIGNED);
  &DECL_LIT_OPND("imm","s23", $SIGNED);
  &DECL_LIT_OPND("pcrel","s32", $PCREL);

  # enumeration operand (name, eclass, rtype):

#  &DECL_ENUM_OPND("icbus","ibus", $UNSIGNED);

  # operand use types:
  &DECL_OPND_USE("base");
  &DECL_OPND_USE("offset");
  &DECL_OPND_USE("storeval");
  &DECL_OPND_USE("condition");
  &DECL_OPND_USE("predicate");
  &DECL_OPND_USE("target");
  &DECL_OPND_USE("postincr");
  &DECL_OPND_USE("uniq_res");
  &DECL_OPND_USE("opnd1");
  &DECL_OPND_USE("opnd2");

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
  &DECL_SCD_RESOURCE ("MEM", 1);       # 1 load/store unit
  &DECL_SCD_RESOURCE ("ODD", 2);       # 2 odd slots
  &DECL_SCD_RESOURCE ("XFER", 1);      # 1 branch unit

  &DECL_SCD_CLASS ("ALU", 2, 3);             # ALU TOPs
  &DECL_SCD_CLASS ("ALU_IMM", 2, 3);         # ALU TOPs
  &DECL_SCD_CLASS ("MUL", 1, 2);             # 3 cycle MUL TOPs
  &DECL_SCD_CLASS ("MUL_IMM", 1, 2);         # 3 cycle MUL TOPs
  &DECL_SCD_CLASS ("LOAD", 1, 2);            # 3 cycle LOAD TOPs
  &DECL_SCD_CLASS ("LOAD_IMM", 1, 2);        # 3 cycle LOAD TOPs
  &DECL_SCD_CLASS ("LOAD2", 1, 2);           # 2 cycle LOAD TOPs
  &DECL_SCD_CLASS ("LOAD2_IMM", 1, 2);       # 2 cycle LOAD TOPs
  &DECL_SCD_CLASS ("STORE", 0, 3);           # STORE TOPs
  &DECL_SCD_CLASS ("STORE_IMM", 0, 3);       # STORE TOPs
  &DECL_SCD_CLASS ("EXT", 0, 1);             # Immediate extentions
  &DECL_SCD_CLASS ("BRANCH", 0, 2);          # conditional branch
  &DECL_SCD_CLASS ("CALL", 1, 1);            # function call
  &DECL_SCD_CLASS ("JUMP", 0, 1);            # direct, indirect jump
  &DECL_SCD_CLASS ("SYNC", 0, 0);            # sync instructions
  &DECL_SCD_CLASS ("PRGINS", 0, 0);          # prgins instruction
  &DECL_SCD_CLASS ("simulated", 1, 1);       # simulated
  &DECL_SCD_CLASS ("asm", 1, 1);          # asm
  &DECL_SCD_CLASS ("ssa", 1, 1);             # ssa
  &DECL_SCD_CLASS ("getpc", 2, 1);             # getpc

  &DECL_EXEC_SLOT ("S0", "ALU,MUL,LOAD,LOAD_2,STORE,EXT,BRANCH,JUMP,CALL,SYNC,PRGINS,getpc"); # Slot 0
  &DECL_EXEC_SLOT ("S1", "ALU,MUL,LOAD,LOAD_2,STORE,EXT,BRANCH,JUMP,CALL,SYNC,PRGINS,getpc"); # Slot 1
  &DECL_EXEC_SLOT ("S2", "ALU,MUL,LOAD,LOAD2_IMM,STORE,EXT,BRANCH,JUMP,CALL,PRGINS,SYNC,getpc"); # Slot 2
  &DECL_EXEC_SLOT ("S3", "ALU,MUL,LOAD,LOAD2_IMM,STORE,EXT,BRANCH,JUMP,CALL,SYNC,PRGINS,getpc"); # Slot 3
  &DECL_EXEC_SLOT ("EXT0", "ALU_IMM,MUL_IMM,LOAD_IMM,LOAD2_IMM,STORE_IMM"); # Long immediate in slot 0-1
  &DECL_EXEC_SLOT ("EXT1", "ALU_IMM,MUL_IMM,LOAD_IMM,LOAD2_IMM,STORE_IMM"); # Long immediate in slot 1-2
  &DECL_EXEC_SLOT ("EXT2", "ALU_IMM,MUL_IMM,LOAD_IMM,LOAD2_IMM,STORE_IMM"); # Long immediate in slot 2-3
  &DECL_EXEC_SLOT ("Odd",  "MUL");                # Multiplies in slot 1 or 3
  &DECL_EXEC_SLOT ("Even", "EXT");               # Multiplies in slot 1 or 3
  &DECL_EXEC_SLOT ("ReqS0", "BRANCH,JUMP,CALL,getpc"); 

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

  &DECL_TEMPLATE("temp1",1);
    &SLOT(0, "S0");
    &STOP(0);

  &DECL_TEMPLATE("temp2",2);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &STOP(1);

  &DECL_TEMPLATE("temp3",3);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &SLOT(2, "S2");
    &STOP(2);

  &DECL_TEMPLATE("temp4",4);
    &SLOT(0, "EXT0");
    &SLOT(1, "EXT0");
    &SLOT(2, "S2");
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp5",2);
    &SLOT(0, "EXT0");
    &SLOT(1, "EXT0");
    &STOP(1);

  &DECL_TEMPLATE("temp6",3);
    &SLOT(0, "EXT0");
    &SLOT(1, "EXT0");
    &SLOT(2, "S2");
    &STOP(2);

  &DECL_TEMPLATE("temp7",4);
    &SLOT(0, "EXT0");
    &SLOT(1, "EXT0");
    &SLOT(2, "EXT2");
    &SLOT(3, "EXT2");
    &STOP(3);

  &DECL_TEMPLATE("temp8",4);
    &SLOT(0, "S0");
    &SLOT(1, "EXT1");
    &SLOT(2, "EXT1");
    &SLOT(3, "S3");
    &STOP(3);

  &DECL_TEMPLATE("temp9",3);
    &SLOT(0, "S0");
    &SLOT(1, "EXT1");
    &SLOT(2, "EXT1");
    &STOP(2);

  &DECL_TEMPLATE("temp10",4);
    &SLOT(0, "S0");
    &SLOT(1, "S1");
    &SLOT(2, "EXT2");
    &SLOT(3, "EXT2");
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

  &DECL_PACK_TYPE ("Int3R", 1, 2, "Int3R: dest = src1, src2");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 6, 6);
  &DECL_PACK_TYPE ("Int3I", 1, 2, "Int3I: idest = src1, isrc2");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
  &DECL_PACK_TYPE ("Int3E", 1, 1, "Int3E: idest = src1, isrc2 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Mul64R", 1, 2, "Mul64R: dest = src1, src2");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 6, 6);
  &DECL_PACK_TYPE ("Mul64I", 1, 2, "Mul64I: idest = src1, isrc2");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
  &DECL_PACK_TYPE ("Mul64E", 1, 1, "Mul64E: idest = src1, isrc2 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Cmp3R_Reg", 1, 2, "Cmp3R_Reg: dest = src1, src2");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 6, 6);
  &DECL_PACK_TYPE ("Cmp3R_Br", 1, 2, "Cmp3R_Br: bdest = src1, src2");
    &PACK_RESULT (0, 0, 18, 3);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 6, 6);
  &DECL_PACK_TYPE ("Cmp3I_Reg", 1, 2, "Cmp3I_Reg: idest = src1, isrc2");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
  &DECL_PACK_TYPE ("Cmp3E_Reg", 1, 2, "Cmp3E_Reg: idest = src1, isrc2 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Cmp3I_Br", 1, 2, "Cmp3I_Br: ibdest = src1, isrc2");
    &PACK_RESULT (0, 0, 6, 3);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
  &DECL_PACK_TYPE ("Cmp3E_Br", 1, 2, "Cmp3E_Br: ibdest = src1, isrc2 / imml");
    &PACK_RESULT (0, 0, 6, 3);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 12, 9);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Imm", 0, 1, "Imm: imm");
    &PACK_OPERAND (0, 0, 0, 23);
  &DECL_PACK_TYPE ("SelectR", 1, 3, "SelectR: dest = scond, src1, src2");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_OPERAND (0, 0, 21, 3);
    &PACK_OPERAND (1, 0, 0, 6);
    &PACK_OPERAND (2, 0, 6, 6);
  &DECL_PACK_TYPE ("SelectI", 1, 3, "SelectI: idest = scond, src1, isrc2");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 21, 3);
    &PACK_OPERAND (1, 0, 0, 6);
    &PACK_OPERAND (2, 0, 12, 9);
  &DECL_PACK_TYPE ("SelectE", 1, 3, "SelectE: idest = scond, src1, isrc2 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 21, 3);
    &PACK_OPERAND (1, 0, 0, 6);
    &PACK_OPERAND (2, 0, 12, 9);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("cgen", 2, 3, "cgen: dest, bdest = src1, src2, scond");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_RESULT (1, 0, 18, 3);
    &PACK_OPERAND (0, 0, 0, 6);
    &PACK_OPERAND (1, 0, 6, 6);
    &PACK_OPERAND (2, 0, 21, 3);
  &DECL_PACK_TYPE ("SysOp", 0, 0, "SysOp: nada");
  &DECL_PACK_TYPE ("Load", 1, 2, "Load: idest = isrc2, src1");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (1, 0, 0, 6);
  &DECL_PACK_TYPE ("LoadE", 1, 2, "LoadE: idest = isrc2, src1 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (1, 0, 0, 6);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Store", 0, 3, "Store: isrc2, src1, src2");
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (1, 0, 0, 6);
    &PACK_OPERAND (2, 0, 6, 6);
  &DECL_PACK_TYPE ("StoreE", 0, 3, "StoreE: isrc2, src1, src2 / imml");
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (1, 0, 0, 6);
    &PACK_OPERAND (2, 0, 6, 6);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Call", 0, 1, "Call: lr = btarg");
    &PACK_OPERAND (0, 0, 0, 23);
  &DECL_PACK_TYPE ("Branch", 0, 2, "Branch: bcond, btarg");
    &PACK_OPERAND (0, 0, 23, 3);
    &PACK_OPERAND (1, 0, 0, 23);
  &DECL_PACK_TYPE ("Jump", 0, 1, "Jump: btarg");
    &PACK_OPERAND (0, 0, 0, 23);
  &DECL_PACK_TYPE ("Extr", 1, 1, "Extr: immr / idest = isrc2");
    &PACK_OPERAND (0, 9, 0, 23);
    &NEXT_WORD();
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
  &DECL_PACK_TYPE ("sub_i", 1, 2, "sub_i: idest = isrc2, src1");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (1, 0, 0, 6);
  &DECL_PACK_TYPE ("sub_ii", 1, 2, "sub_ii: idest = isrc2, src1 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (1, 0, 0, 6);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Icall", 0, 0, "Icall: lr = lr");
  &DECL_PACK_TYPE ("Ijump", 0, 0, "Ijump: igoto lr");
  &DECL_PACK_TYPE ("rfi", 0, 0, "rfi");
  &DECL_PACK_TYPE ("nop", 0, 0, "nop");
  &DECL_PACK_TYPE ("MoveR", 1, 1, "MoveR: dest = src2 ");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_OPERAND (0, 0, 6, 6);
  &DECL_PACK_TYPE ("MoveI", 1, 1, "MoveI: idest = isrc2 ");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
  &DECL_PACK_TYPE ("MoveE", 1, 1, "MoveE: idest = isrc2 / imml");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 12, 9);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("mtb", 1, 1, "mtb: bdest = src1 ");
    &PACK_RESULT (0, 0, 18, 3);
    &PACK_OPERAND (0, 0, 0, 6);
  &DECL_PACK_TYPE ("mfb", 1, 1, "mfb: idest = scond ");
    &PACK_RESULT (0, 0, 6, 6);
    &PACK_OPERAND (0, 0, 21, 3);
  &DECL_PACK_TYPE ("Cache", 0, 2, "cache opcodes ");
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (0, 0, 0, 6);
  &DECL_PACK_TYPE ("CacheE", 0, 2, "xcache opcodes ");
    &PACK_OPERAND (0, 0, 12, 9);
    &PACK_OPERAND (0, 0, 0, 6);
    &NEXT_WORD();
    &PACK_OPERAND (0, 9, 0, 23);
  &DECL_PACK_TYPE ("Sync", 0, 0, "sync");
  &DECL_PACK_TYPE ("Monadic", 1, 1, "Monadic: dest = src1");
    &PACK_RESULT (0, 0, 12, 6);
    &PACK_OPERAND (0, 0, 0, 6);
  &DECL_PACK_TYPE ("asm", 0, 1, "asm: imm");
    &PACK_OPERAND (0, 0, 0, 23);

  ###############################################################
  #                       PROCESSING
  ###############################################################

  # open and emit head info into all concerned files:
  &initialize_isa_file ($archdir);
  &initialize_reg_file ($archdir);
  &initialize_lit_file ($archdir);
  &initialize_enum_file ($archdir);
  &initialize_si_file ($archdir);
  &initialize_operands_file ($archdir);
  &initialize_subset_file ($archdir);
  &initialize_properties_file ($archdir);
  &initialize_print_file ($archdir);
  &initialize_pack_file ($archdir);
  &initialize_bundle_file ($archdir);
  &initialize_decode_file ($archdir);

  #BD3 TODO: Is it necessary?
  &initialize_op_file($archdir);

  my $sub;
  for ($sub = 0; $sub < $SUBSET_count-1; $sub++) {
      printf STDOUT "Generating targinfos for subset %s\n", $SUBSET_name[$sub];
      &read_opcodes ($sub);
  } # for each subset

  # The Pro64 compiler needs a number of simulated and dummy 
  # opcodes. They get added after all the real ISA opcodes have
  # been sorted because simulated/dummy do not need to be.
  &initialize_required_opcodes ();

  # Each opcode common to all subset can be moved to the parent subset.
  &update_subsets ();

  # Emit ISA information
  for (my $opcode = 0; $opcode < $OP_count; $opcode++) {
      if ($OP_opcode[$opcode]) {
	  &emit_opcode ($opcode);
      }
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
  &emit_scdinfo ($subset);
  &emit_bundle_info ($subset);

  # these are completely dummy, not used for now in the compiler:
  &emit_pack_info ($subset);
  &emit_decode_info ($subset);

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


