
#define __CSD_E__

/*!
  @file CSD.e
  This file was automatically generated from CSD.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

#define COMMA ,

#define __MTYPE_E__

/*!
  @file MType.e
  This file was automatically generated from MType.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  MType -- Enumeration of machine types
*/
typedef enum MType {
#define MType(MType, BitWidth, Signed, MemBytes, Comment) \
    MType_##MType,
/*MType*/
MType_U1 , 
MType_I1 , 
MType_U2 , 
MType_I2 , 
MType_U4 , 
MType_I4 , 
MType_U8 , 
MType_I8 , 
MType_F4 , 
MType_F8 , 
/*MType*/
#undef MType

  MType__
} MType;


//__MTYPE_E__


#define __REGKIND_E__

/*!
  @file RegKind.e
  This file was automatically generated from RegKind.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  RegKind -- Enumeration.
*/
typedef enum RegKind {
#define RegKind(RegKind, Registers, Comment) \
    RegKind_##RegKind,
/*RegKind*/
RegKind_Dummy , 
RegKind_Caller , 
RegKind_Callee , 
RegKind_FunArg , 
RegKind_FunRes , 
RegKind_Struct , 
RegKind_Return , 
RegKind_Global , 
RegKind_Static , 
RegKind_Stack , 
RegKind_Frame , 
RegKind_Wired , 
RegKind_Zero , 
RegKind_True , 
RegKind_Sink , 
RegKind_Kernel , 
/*RegKind*/
#undef RegKind

  RegKind__
} RegKind;


//__REGKIND_E__


#define __REGCLASS_E__

/*!
  @file RegClass.e
  This file was automatically generated from RegClass.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  RegClass -- Enumeration.
*/
typedef enum RegClass {
#define RegClass(RegClass, BitWidth, MType, LowReg, HighReg, Registers, Comment) \
    RegClass_##RegClass,
/*RegClass*/
RegClass_GRC , 
RegClass_BRC , 
/*RegClass*/
#undef RegClass

  RegClass__
} RegClass;


//__REGCLASS_E__


#define __REGISTER_E__

/*!
  @file Register.e
  This file was automatically generated from Register.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  Register -- Enumeration.
*/
typedef enum Register {
#define Register(Register, Names, RegClass, RegFile, Number, RegKinds, Comment) \
    Register_##Register,
/*Register*/
Register_B0 , 
Register_B1 , 
Register_B2 , 
Register_B3 , 
Register_B4 , 
Register_B5 , 
Register_B6 , 
Register_B7 , 
Register_R0 , 
Register_R1 , 
Register_R2 , 
Register_R3 , 
Register_R4 , 
Register_R5 , 
Register_R6 , 
Register_R7 , 
Register_R8 , 
Register_R9 , 
Register_R10 , 
Register_R11 , 
Register_R12 , 
Register_R13 , 
Register_R14 , 
Register_R15 , 
Register_R16 , 
Register_R17 , 
Register_R18 , 
Register_R19 , 
Register_R20 , 
Register_R21 , 
Register_R22 , 
Register_R23 , 
Register_R24 , 
Register_R25 , 
Register_R26 , 
Register_R27 , 
Register_R28 , 
Register_R29 , 
Register_R30 , 
Register_R31 , 
Register_R32 , 
Register_R33 , 
Register_R34 , 
Register_R35 , 
Register_R36 , 
Register_R37 , 
Register_R38 , 
Register_R39 , 
Register_R40 , 
Register_R41 , 
Register_R42 , 
Register_R43 , 
Register_R44 , 
Register_R45 , 
Register_R46 , 
Register_R47 , 
Register_R48 , 
Register_R49 , 
Register_R50 , 
Register_R51 , 
Register_R52 , 
Register_R53 , 
Register_R54 , 
Register_R55 , 
Register_R56 , 
Register_R57 , 
Register_R58 , 
Register_R59 , 
Register_R60 , 
Register_R61 , 
Register_R62 , 
Register_R63 , 
/*Register*/
#undef Register

  Register__
} Register;


//__REGISTER_E__


#define __MODIFIER_E__

/*!
  @file Modifier.e
  This file was automatically generated from Modifier.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  Modifier -- Enumeration.
*/
typedef enum Modifier {
#define Modifier(Modifier, Parsing) \
    Modifier_##Modifier,
/*Modifier*/
Modifier_NONE , 
/*Modifier*/
#undef Modifier

  Modifier__
} Modifier;


//__MODIFIER_E__


#define __IMMEDIATE_E__

/*!
  @file Immediate.e
  This file was automatically generated from Immediate.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  Immediate -- Enumeration.
*/
typedef enum Immediate {
#define Immediate(Immediate, Parsing) \
    Immediate_##Immediate,
/*Immediate*/
Immediate_I_signed_8_overflow_dont , 
Immediate_I_signed_16_overflow_dont , 
Immediate_I_signed_32_overflow_dont , 
Immediate_I_signed_64_overflow_dont , 
Immediate_I_unsigned_9_overflow_unsigned , 
Immediate_I_signed_23_overflow_signed , 
Immediate_I_signed_9_overflow_signed , 
Immediate_I_unsigned_23_overflow_unsigned , 
/*Immediate*/
#undef Immediate

  Immediate_LABEL,
  Immediate__
} Immediate;


//__IMMEDIATE_E__


#define __INSTRMODE_E__

/*!
  @file InstrMode.e
  This file was automatically generated from InstrMode.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  InstrMode -- Enumeration.
*/
typedef enum InstrMode {
#define InstrMode(InstrMode, Attributes, IssueLimits, InstrSize) \
    InstrMode_##InstrMode,
/*InstrMode*/
InstrMode_ST220 , 
InstrMode_ST221 , 
InstrMode_DATA , 
InstrMode_INFO , 
/*InstrMode*/
#undef InstrMode

  InstrMode__
} InstrMode;


//__INSTRMODE_E__


#define __OPERATOR_E__

/*!
  @file Operator.e
  This file was automatically generated from Operator.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  Operator -- Enumeration.
*/
typedef enum Operator {
#define Operator(Operator, Parsing, InstrModes, Attributes, IssueClass, Mnemonic, Format, Syntax) \
    Operator_##Operator,
/*Operator*/
Operator_CODE_NOP , 
Operator_CODE_MOV_DEST_SRC2 , 
Operator_CODE_ADD_DEST_SRC1_SRC2 , 
Operator_CODE_SUB_DEST_SRC1_SRC2 , 
Operator_CODE_SHL_DEST_SRC1_SRC2 , 
Operator_CODE_SHR_DEST_SRC1_SRC2 , 
Operator_CODE_SHRU_DEST_SRC1_SRC2 , 
Operator_CODE_SH1ADD_DEST_SRC1_SRC2 , 
Operator_CODE_SH2ADD_DEST_SRC1_SRC2 , 
Operator_CODE_SH3ADD_DEST_SRC1_SRC2 , 
Operator_CODE_SH4ADD_DEST_SRC1_SRC2 , 
Operator_CODE_AND_DEST_SRC1_SRC2 , 
Operator_CODE_ANDC_DEST_SRC1_SRC2 , 
Operator_CODE_OR_DEST_SRC1_SRC2 , 
Operator_CODE_ORC_DEST_SRC1_SRC2 , 
Operator_CODE_XOR_DEST_SRC1_SRC2 , 
Operator_CODE_MULLHUS_DESTL_SRC1_SRC2 , 
Operator_CODE_MAX_DEST_SRC1_SRC2 , 
Operator_CODE_MAXU_DEST_SRC1_SRC2 , 
Operator_CODE_MIN_DEST_SRC1_SRC2 , 
Operator_CODE_MINU_DEST_SRC1_SRC2 , 
Operator_CODE_MULHHS_DESTL_SRC1_SRC2 , 
Operator_CODE_MULL_DESTL_SRC1_SRC2 , 
Operator_CODE_MULLU_DESTL_SRC1_SRC2 , 
Operator_CODE_MULH_DESTL_SRC1_SRC2 , 
Operator_CODE_MULHU_DESTL_SRC1_SRC2 , 
Operator_CODE_MULLL_DESTL_SRC1_SRC2 , 
Operator_CODE_MULLLU_DESTL_SRC1_SRC2 , 
Operator_CODE_MULLH_DESTL_SRC1_SRC2 , 
Operator_CODE_MULLHU_DESTL_SRC1_SRC2 , 
Operator_CODE_MULHH_DESTL_SRC1_SRC2 , 
Operator_CODE_MULHHU_DESTL_SRC1_SRC2 , 
Operator_CODE_MULHS_DESTL_SRC1_SRC2 , 
Operator_CODE_CMPEQ_DEST_SRC1_SRC2 , 
Operator_CODE_CMPNE_DEST_SRC1_SRC2 , 
Operator_CODE_CMPGE_DEST_SRC1_SRC2 , 
Operator_CODE_CMPGEU_DEST_SRC1_SRC2 , 
Operator_CODE_CMPGT_DEST_SRC1_SRC2 , 
Operator_CODE_CMPGTU_DEST_SRC1_SRC2 , 
Operator_CODE_CMPLE_DEST_SRC1_SRC2 , 
Operator_CODE_CMPLEU_DEST_SRC1_SRC2 , 
Operator_CODE_CMPLT_DEST_SRC1_SRC2 , 
Operator_CODE_CMPLTU_DEST_SRC1_SRC2 , 
Operator_CODE_ANDL_DEST_SRC1_SRC2 , 
Operator_CODE_NANDL_DEST_SRC1_SRC2 , 
Operator_CODE_ORL_DEST_SRC1_SRC2 , 
Operator_CODE_NORL_DEST_SRC1_SRC2 , 
Operator_CODE_CMPEQ_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPNE_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPGE_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPGEU_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPGT_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPGTU_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPLE_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPLEU_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPLT_BDEST_SRC1_SRC2 , 
Operator_CODE_CMPLTU_BDEST_SRC1_SRC2 , 
Operator_CODE_ANDL_BDEST_SRC1_SRC2 , 
Operator_CODE_NANDL_BDEST_SRC1_SRC2 , 
Operator_CODE_MTB_BDEST_SRC1 , 
Operator_CODE_ORL_BDEST_SRC1_SRC2 , 
Operator_CODE_NORL_BDEST_SRC1_SRC2 , 
Operator_CODE_MOV_IDEST_ISRC2 , 
Operator_CODE_MOV_IDEST_ISRCX , 
Operator_CODE_ADD_IDEST_SRC1_ISRC2 , 
Operator_CODE_ADD_IDEST_SRC1_ISRCX , 
Operator_CODE_SUB_IDEST_SRC1_ISRC2 , 
Operator_CODE_SUB_IDEST_SRC1_ISRCX , 
Operator_CODE_SHL_IDEST_SRC1_ISRC2 , 
Operator_CODE_SHL_IDEST_SRC1_ISRCX , 
Operator_CODE_SHR_IDEST_SRC1_ISRC2 , 
Operator_CODE_SHR_IDEST_SRC1_ISRCX , 
Operator_CODE_SHRU_IDEST_SRC1_ISRC2 , 
Operator_CODE_SHRU_IDEST_SRC1_ISRCX , 
Operator_CODE_SH1ADD_IDEST_SRC1_ISRC2 , 
Operator_CODE_SH1ADD_IDEST_SRC1_ISRCX , 
Operator_CODE_SH2ADD_IDEST_SRC1_ISRC2 , 
Operator_CODE_SH2ADD_IDEST_SRC1_ISRCX , 
Operator_CODE_SH3ADD_IDEST_SRC1_ISRC2 , 
Operator_CODE_SH3ADD_IDEST_SRC1_ISRCX , 
Operator_CODE_SH4ADD_IDEST_SRC1_ISRC2 , 
Operator_CODE_SH4ADD_IDEST_SRC1_ISRCX , 
Operator_CODE_ZXTB_IDEST_SRC1 , 
Operator_CODE_AND_IDEST_SRC1_ISRC2 , 
Operator_CODE_AND_IDEST_SRC1_ISRCX , 
Operator_CODE_ANDC_IDEST_SRC1_ISRC2 , 
Operator_CODE_ANDC_IDEST_SRC1_ISRCX , 
Operator_CODE_OR_IDEST_SRC1_ISRC2 , 
Operator_CODE_OR_IDEST_SRC1_ISRCX , 
Operator_CODE_ORC_IDEST_SRC1_ISRC2 , 
Operator_CODE_ORC_IDEST_SRC1_ISRCX , 
Operator_CODE_XOR_IDEST_SRC1_ISRC2 , 
Operator_CODE_XOR_IDEST_SRC1_ISRCX , 
Operator_CODE_SXTB_IDEST_SRC1 , 
Operator_CODE_SXTH_IDEST_SRC1 , 
Operator_CODE_BSWAP_IDEST_SRC1 , 
Operator_CODE_ZXTH_IDEST_SRC1 , 
Operator_CODE_CLZ_IDEST_SRC1 , 
Operator_CODE_MULLHUS_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULLHUS_IDESTL_SRC1_ISRCX , 
Operator_CODE_MAX_IDEST_SRC1_ISRC2 , 
Operator_CODE_MAX_IDEST_SRC1_ISRCX , 
Operator_CODE_MAXU_IDEST_SRC1_ISRC2 , 
Operator_CODE_MAXU_IDEST_SRC1_ISRCX , 
Operator_CODE_MIN_IDEST_SRC1_ISRC2 , 
Operator_CODE_MIN_IDEST_SRC1_ISRCX , 
Operator_CODE_MINU_IDEST_SRC1_ISRC2 , 
Operator_CODE_MINU_IDEST_SRC1_ISRCX , 
Operator_CODE_MULHHS_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULHHS_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULL_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULL_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULLU_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULLU_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULH_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULH_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULHU_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULHU_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULLL_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULLL_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULLLU_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULLLU_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULLH_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULLH_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULLHU_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULLHU_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULHH_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULHH_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULHHU_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULHHU_IDESTL_SRC1_ISRCX , 
Operator_CODE_MULHS_IDESTL_SRC1_ISRC2 , 
Operator_CODE_MULHS_IDESTL_SRC1_ISRCX , 
Operator_CODE_CMPEQ_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPEQ_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPNE_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPNE_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPGE_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGE_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPGEU_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGEU_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPGT_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGT_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPGTU_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGTU_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPLE_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLE_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPLEU_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLEU_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPLT_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLT_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPLTU_IDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLTU_IDEST_SRC1_ISRCX , 
Operator_CODE_ANDL_IDEST_SRC1_ISRC2 , 
Operator_CODE_ANDL_IDEST_SRC1_ISRCX , 
Operator_CODE_NANDL_IDEST_SRC1_ISRC2 , 
Operator_CODE_NANDL_IDEST_SRC1_ISRCX , 
Operator_CODE_ORL_IDEST_SRC1_ISRC2 , 
Operator_CODE_ORL_IDEST_SRC1_ISRCX , 
Operator_CODE_NORL_IDEST_SRC1_ISRC2 , 
Operator_CODE_NORL_IDEST_SRC1_ISRCX , 
Operator_CODE_CMPEQ_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPEQ_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPNE_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPNE_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPGE_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGE_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPGEU_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGEU_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPGT_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGT_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPGTU_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPGTU_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPLE_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLE_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPLEU_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLEU_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPLT_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLT_IBDEST_SRC1_ISRCX , 
Operator_CODE_CMPLTU_IBDEST_SRC1_ISRC2 , 
Operator_CODE_CMPLTU_IBDEST_SRC1_ISRCX , 
Operator_CODE_ANDL_IBDEST_SRC1_ISRC2 , 
Operator_CODE_ANDL_IBDEST_SRC1_ISRCX , 
Operator_CODE_NANDL_IBDEST_SRC1_ISRC2 , 
Operator_CODE_NANDL_IBDEST_SRC1_ISRCX , 
Operator_CODE_ORL_IBDEST_SRC1_ISRC2 , 
Operator_CODE_ORL_IBDEST_SRC1_ISRCX , 
Operator_CODE_NORL_IBDEST_SRC1_ISRC2 , 
Operator_CODE_NORL_IBDEST_SRC1_ISRCX , 
Operator_CODE_SLCT_DEST_SCOND_SRC1_SRC2 , 
Operator_CODE_SLCTF_DEST_SCOND_SRC1_SRC2 , 
Operator_CODE_ADDCG_DEST_BDEST_SRC1_SRC2_SCOND , 
Operator_CODE_DIVS_DEST_BDEST_SRC1_SRC2_SCOND , 
Operator_CODE_IMML_IMM , 
Operator_CODE_IMMR_IMM , 
Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRC2 , 
Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRCX , 
Operator_CODE_MFB_IDEST_SCOND , 
Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRC2 , 
Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRCX , 
Operator_CODE_PRGINS , 
Operator_CODE_SBRK , 
Operator_CODE_SYSCALL , 
Operator_CODE_BREAK , 
Operator_CODE_LDW_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDW_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDWD_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDWD_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDH_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDH_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDHD_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDHD_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDHU_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDHU_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDHUD_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDHUD_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDB_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDB_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDBD_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDBD_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDBU_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDBU_IDESTL_ISRCX_SRC1 , 
Operator_CODE_LDBUD_IDESTL_ISRC2_SRC1 , 
Operator_CODE_LDBUD_IDESTL_ISRCX_SRC1 , 
Operator_CODE_STW_ISRC2_SRC1_SRC2 , 
Operator_CODE_STW_ISRCX_SRC1_SRC2 , 
Operator_CODE_STH_ISRC2_SRC1_SRC2 , 
Operator_CODE_STH_ISRCX_SRC1_SRC2 , 
Operator_CODE_STB_ISRC2_SRC1_SRC2 , 
Operator_CODE_STB_ISRCX_SRC1_SRC2 , 
Operator_CODE_PFT_ISRC2_SRC1 , 
Operator_CODE_PFT_ISRCX_SRC1 , 
Operator_CODE_PRGADD_ISRC2_SRC1 , 
Operator_CODE_PRGADD_ISRCX_SRC1 , 
Operator_CODE_PRGSET_ISRC2_SRC1 , 
Operator_CODE_PRGSET_ISRCX_SRC1 , 
Operator_CODE_SYNC , 
Operator_CODE_SEND_ICBUS_SRC2 , 
Operator_CODE_RECV_IDEST_ICBUS , 
Operator_CODE_ASM0_DEST_SRC1_SRC2 , 
Operator_CODE_ASM1_DEST_SRC1_SRC2 , 
Operator_CODE_ASM2_DEST_SRC1_SRC2 , 
Operator_CODE_ASM3_DEST_SRC1_SRC2 , 
Operator_CODE_ASM4_DEST_SRC1_SRC2 , 
Operator_CODE_ASM5_DEST_SRC1_SRC2 , 
Operator_CODE_ASM6_DEST_SRC1_SRC2 , 
Operator_CODE_ASM7_DEST_SRC1_SRC2 , 
Operator_CODE_ASM8_DEST_SRC1_SRC2 , 
Operator_CODE_ASM9_DEST_SRC1_SRC2 , 
Operator_CODE_ASM10_DEST_SRC1_SRC2 , 
Operator_CODE_ASM11_DEST_SRC1_SRC2 , 
Operator_CODE_ASM12_DEST_SRC1_SRC2 , 
Operator_CODE_ASM13_DEST_SRC1_SRC2 , 
Operator_CODE_ASM14_DEST_SRC1_SRC2 , 
Operator_CODE_ASM15_DEST_SRC1_SRC2 , 
Operator_CODE_ASM0_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM0_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM1_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM1_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM2_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM2_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM3_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM3_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM4_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM4_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM5_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM5_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM6_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM6_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM7_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM7_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM8_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM8_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM9_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM9_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM10_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM10_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM11_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM11_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM12_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM12_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM13_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM13_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM14_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM14_IDEST_SRC1_ISRCX , 
Operator_CODE_ASM15_IDEST_SRC1_ISRC2 , 
Operator_CODE_ASM15_IDEST_SRC1_ISRCX , 
Operator_CODE_CALL_BTARG , 
Operator_CODE_ICALL , 
Operator_CODE_SYNCINS , 
Operator_CODE_GOTO_BTARG , 
Operator_CODE_IGOTO , 
Operator_CODE_RFI , 
Operator_CODE_BR_BCOND_BTARG , 
Operator_CODE_BRF_BCOND_BTARG , 
Operator_DATA_BYTE_I8V , 
Operator_DATA_HALF_I16V , 
Operator_DATA_WORD_I32V , 
Operator_DATA_DWORD_I64V , 
Operator_DATA_FLOAT_I32V , 
Operator_DATA_DOUBLE_I64V , 
Operator_DATA_STRING_STR , 
Operator_DATA_ASCII_STR , 
Operator_DATA_ASCIIZ_STR , 
Operator_DATA_SPACE_I32 , 
Operator_DATA_BSS_ID_I32 , 
Operator_DATA_BSS_ID_I32_I32 , 
Operator_DATA_COMM_ID_I32 , 
Operator_DATA_COMM_ID_I32_I32 , 
Operator_DATA_COMMON_ID_I32 , 
Operator_DATA_COMMON_ID_I32_I32 , 
Operator_DATA_IDENT_STR , 
Operator_INFO_ALIGN_I16 , 
Operator_INFO_ALIGN_I16_I16 , 
Operator_INFO_TEXT , 
Operator_INFO_DATA , 
Operator_INFO_SECTION_STR , 
Operator_INFO_SECTION_ID , 
Operator_INFO_SECTION_STR_STR , 
Operator_INFO_SECTION_STR_ID , 
Operator_INFO_PUSHSECTION , 
Operator_INFO_POPSECTION , 
Operator_INFO_NODEBUG , 
Operator_INFO_FILE_STR , 
Operator_INFO_TYPE_ID_ID , 
Operator_INFO_SIZE_ID_I32 , 
Operator_INFO_GLOBAL_ID , 
Operator_INFO_GLOBL_ID , 
Operator_INFO_EXPORT_ID , 
Operator_INFO_IMPORT_ID , 
Operator_INFO_WEAK_ID , 
Operator_INFO_LEAVE_ID , 
Operator_MACRO_ADJUST , 
Operator_MACRO_MOVE , 
Operator_MACRO_GOTO , 
Operator_MACRO_JUMP , 
Operator_MACRO_CALL , 
Operator_MACRO_IFRCALL , 
Operator_MACRO_RETURN , 
Operator_MACRO_GNUASM , 
Operator_MACRO_RELOAD , 
Operator_MACRO_SPILL , 
Operator_PSEUDO_NOP , 
Operator_PSEUDO_PHI , 
Operator_PSEUDO_PSI , 
Operator_PSEUDO_LOOP , 
Operator_PSEUDO_FALL , 
Operator_PSEUDO_LABEL , 
/*Operator*/
#undef Operator

  Operator__
} Operator;


//__OPERATOR_E__


#define __ISSUEUNIT_E__

/*!
  @file IssueUnit.e
  This file was automatically generated from IssueUnit.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  IssueUnit -- Enumeration.
*/
typedef enum IssueUnit {
#define IssueUnit(IssueUnit) \
    IssueUnit_##IssueUnit,
/*IssueUnit*/
IssueUnit_Width , 
IssueUnit_IMX , 
IssueUnit_MUL , 
IssueUnit_MEM , 
IssueUnit_CTL , 
IssueUnit_RX1 , 
IssueUnit_RX2 , 
/*IssueUnit*/
#undef IssueUnit

  IssueUnit__
} IssueUnit;


//__ISSUEUNIT_E__


#define __ISSUECLASS_E__

/*!
  @file IssueClass.e
  This file was automatically generated from IssueClass.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  IssueClass -- Enumeration.
*/
typedef enum IssueClass {
#define IssueClass(IssueClass, Reservations, Counters, Comment) \
    IssueClass_##IssueClass,
/*IssueClass*/
IssueClass_ALL , 
IssueClass_ALU , 
IssueClass_ALUX , 
IssueClass_MUL , 
IssueClass_MULX , 
IssueClass_MEM , 
IssueClass_MEMX , 
IssueClass_CTL , 
IssueClass_NOP , 
/*IssueClass*/
#undef IssueClass

  IssueClass__
} IssueClass;


//__ISSUECLASS_E__



//__CSD_E__
