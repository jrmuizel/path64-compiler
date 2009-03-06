#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#include "OS_Types.h"

// uint64 is the implementation-independent name for the unsigned 64-bit integer base type
//typedef unsigned long long uint64;
typedef uint64 uint63;
typedef uint64 uint57;
typedef uint64 uint41;
typedef uint64 uint40;
typedef uint64 uint34;
typedef uint64 uint33;

// sint64 is the implementation-independent name for the signed 64-bit integer base type
//typedef sint64          sint64;
typedef sint64          sint63;
typedef sint64          sint57;
typedef sint64          sint41;
typedef sint64          sint40;
typedef sint64          sint34;
typedef sint64          sint33;

// uint32 is the implementation-independent name for the unsigned 32-bit integer base type
//typedef unsigned long      uint32;
typedef uint32      uint31;
typedef uint32      uint30;
typedef uint32      uint29;
typedef uint32      uint28;
typedef uint32      uint27;
typedef uint32      uint26;
typedef uint32      uint25;
typedef uint32      uint24;
typedef uint32      uint23;
typedef uint32      uint22;
typedef uint32      uint21;
typedef uint32      uint19;
typedef uint32      uint18;
typedef uint32      uint17;

// sint32 is the implementation-independent name for the signed 32-bit integer base type
//typedef signed long      sint32;
typedef sint32      sint31;
typedef sint32      sint30;
typedef sint32      sint29;
typedef sint32      sint28;
typedef sint32      sint27;
typedef sint32      sint26;
typedef sint32      sint25;
typedef sint32      sint24;
typedef sint32      sint23;
typedef sint32      sint22;
typedef sint32      sint21;
typedef sint32      sint19;
typedef sint32      sint18;
typedef sint32      sint17;
  
// uint16 is the implementation-independent name for the unsigned 16-bit integer base type
//typedef unsigned short     uint16;
typedef uint16     uint15;
typedef uint16     uint14;
typedef uint16     uint13;
typedef uint16     uint12;
typedef uint16     uint11;
typedef uint16     uint10;
typedef uint16     uint9;

// uint16 is the implementation-independent name for the signed 16-bit integer base type
//typedef signed short     sint16;
typedef sint16     sint15;
typedef sint16     sint14;
typedef sint16     sint13;
typedef sint16     sint12;
typedef sint16     sint11;
typedef sint16     sint10;
typedef sint16     sint9;

// uint8 is the implementation-independent name for the unsigned 8-bit integer base type
//typedef unsigned char      uint8;
typedef uint8      uint7;
typedef uint8      uint6;
typedef uint8      uint5;
typedef uint8      uint4;
typedef uint8      uint3;
typedef uint8      uint2;
typedef uint8      uint1;

// sint8 is the implementation-independent name for the signed 8-bit integer base type
//typedef sint8      sint8;
typedef sint8      sint7;
typedef sint8      sint6;
typedef sint8      sint5;
typedef sint8      sint4;
typedef sint8      sint3;
typedef sint8      sint2;
typedef sint8      sint1;

typedef enum {
  BIT_CLEAR = 0,
  BIT_SET = 1,
  BIT_NOT = 2
} BIT_OPER;

typedef enum {
  CODE_PLUS  = 1,
  CODE_MINUS = -1
} CODE_ASCII;

typedef enum {
  MISALIGN  = 0,
  PROTECT   = 1,
  OPCODE    = 2,
  PEXECUTE  = 4,
  POUTOFMEM = 5,
  DOVERMEM  = 8,
  DSYSERR   = 9,
  DSYSMISALIGN = 10,
  DREAD     = 11,
  DMISALIGN = 12,
  DOUTOFMEM = 13,
  DVOLATILE = 14,
  DWRITE    = 15
} CODE_optrap;

typedef enum {
  SVW = 1,
  SVE = 2,
  SVH = 4,
  SVL = 8,
  SCL = 16,
  SNR = 32,
} CODE_FLAG;

typedef enum {
  EQ         = 0,
  NEQ        = 1,
  GTE        = 2,
  LT         = 3,
  LTE        = 4,
  GT         = 5,
} CODE_COMPARE;

//------------------------------------------------------------------------------
// Unused and raise incompatibility with Visual C++ ( WORD is already used for different purpose )
//------------------------------------------------------------------------------
// typedef enum {
//   EXTENDED = 0,
//   WORD     = 1,
//   PACKED   = 2,
//   BIT      = 3
// } CMPOP_SIZE;
//------------------------------------------------------------------------------

typedef enum {
  modulo_8	 = 3,
  modulo_16	 = 4,
  modulo_32	 = 5,
  modulo_64	 = 6,
  modulo_128	 = 7,
  modulo_256	 = 8,
  modulo_512	 = 9,
  modulo_1024	 = 10,
  modulo_2048	 = 11,
  modulo_4096	 = 12,
  modulo_8192	 = 13,
  modulo_16384	 = 14,
  modulo_32768	 = 15,
  modulo_65536	 = 16
} f_AM_modulo;

typedef enum {
  CC_CODE   = 0,
  LK_CODE   = 1,

  LC0_CODE  = 2,
  LCR0_CODE = 3,

  LC1_CODE  = 4,
  LCR1_CODE = 5,

  LC2_CODE  = 6,
  LCR2_CODE = 7,

  FR_CODE   = 8,
  GR_CODE   = 9,

  LSR0_CODE = 10,
  LER0_CODE = 11,

  LSR1_CODE = 12,
  LER1_CODE = 13,

  LSR2_CODE = 14,
  LER2_CODE = 15,

  PCR_CODE  = 28,
  IVBR_CODE = 29,
  PSR_CODE  = 30
} CR_CODE;

typedef enum {
  UNKNOWN_SCHED = 0,
  AU_PIPE = 1,
  DU_PIPE = 2,
  AU_DU_PIPE = 3,
  AU_CU_PIPE = 4,
  CU_PIPE = 5,
  DU_CU_PIPE = 6
} SHEDULE_TYPE;

typedef enum {
  INVALID_I      = 0x00000000,

  ARITH1_I       = 0x00000002,
  GMI_I          = 0x00000003,
  GMI_LZCTTCLR_I = 0x00000004,

  STORE_I        = 0x0000000A,	// 10
  BARRIER_I      = 0x0000000B,	// 11
  LOAD_I         = 0x00000014,	// 20
  LD_SET_I       = 0x00000015,	// 21

  // CACHE
  PCACHE_I       = 0x00000032,	// 50
  DCACHE_I       = 0x00000037,	// 55

  X3_I           = 0x0000003C,	// 60

  BKP_I          = 0x00000050,	// 80

  // EFU
  EFU_I          = 0x00000100,
  EFU_WB_I       = 0x00000101,	// those EFU inst. perform WB in the drf
  EFU_SX_I       = 0x0000010A,	// EFU_I | STORE_I
  EFU_LX_I       = 0x00000114	// EFU_I | LOAD_I (reserved, unused so far)
} INSTRUCTION_TYPE;

typedef enum {
  UNKNOWN_BUNDLE,
  G0_G1_G2,
  G2_G3_G4,
  G0_G1_G2_G3_G4,
  G1_G4,
  G3,
  G5
} SLIW_GRP_TYPE;

typedef enum {
  EARLY,
  LATE,
  VERY_LATE
}  SLIW_TYPE;

#define LOC_TMP \
    sint64 tmpS64;\
    sint64 tmpS63;\
    sint64 tmpS62;\
    sint64 tmpS61;\
    sint64 tmpS60;\
    sint64 tmpS59;\
    sint64 tmpS58;\
    sint64 tmpS57;\
    sint64 tmpS56;\
    sint64 tmpS55;\
    sint64 tmpS54;\
    sint64 tmpS53;\
    sint64 tmpS52;\
    sint64 tmpS51;\
    sint64 tmpS50;\
    sint64 tmpS49;\
    sint64 tmpS48;\
    sint64 tmpS47;\
    sint64 tmpS46;\
    sint64 tmpS45;\
    sint64 tmpS44;\
    sint64 tmpS43;\
    sint64 tmpS42;\
    sint64 tmpS41;\
    sint64 tmpS40;\
    sint64 tmpS39;\
    sint64 tmpS38;\
    sint64 tmpS37;\
    sint64 tmpS36;\
    sint64 tmpS35;\
    sint64 tmpS34;\
    sint64 tmpS33;\
    sint32 tmpS32;\
    sint32 tmpS31;\
    sint32 tmpS30;\
    sint32 tmpS29;\
    sint32 tmpS28;\
    sint32 tmpS27;\
    sint32 tmpS26;\
    sint32 tmpS25;\
    sint32 tmpS24;\
    sint32 tmpS23;\
    sint32 tmpS22;\
    sint32 tmpS21;\
    sint32 tmpS20;\
    sint32 tmpS19;\
    sint32 tmpS18;\
    sint32 tmpS17;\
    sint16 tmpS16;\
    sint16 tmpS15;\
    sint16 tmpS14;\
    sint16 tmpS13;\
    sint16 tmpS12;\
    sint16 tmpS11;\
    sint16 tmpS10;\
    sint16 tmpS9;\
    sint8 tmpS8;\
    sint8 tmpS7;\
    sint8 tmpS6;\
    sint8 tmpS5;\
    sint8 tmpS4;\
    sint8 tmpS3;\
    sint8 tmpS2;\
    sint8 tmpS1;

#define LOC_UTMP \
    uint64 tmp64;\
    uint64 tmp63;\
    uint64 tmp62;\
    uint64 tmp61;\
    uint64 tmp60;\
    uint64 tmp59;\
    uint64 tmp58;\
    uint64 tmp57;\
    uint64 tmp56;\
    uint64 tmp55;\
    uint64 tmp54;\
    uint64 tmp53;\
    uint64 tmp52;\
    uint64 tmp51;\
    uint64 tmp50;\
    uint64 tmp49;\
    uint64 tmp48;\
    uint64 tmp47;\
    uint64 tmp46;\
    uint64 tmp45;\
    uint64 tmp44;\
    uint64 tmp43;\
    uint64 tmp42;\
    uint64 tmp41;\
    uint64 tmp40;\
    uint64 tmp39;\
    uint64 tmp38;\
    uint64 tmp37;\
    uint64 tmp36;\
    uint64 tmp35;\
    uint64 tmp34;\
    uint64 tmp33;\
    uint32 tmp32;\
    uint32 tmp31;\
    uint32 tmp30;\
    uint32 tmp29;\
    uint32 tmp28;\
    uint32 tmp27;\
    uint32 tmp26;\
    uint32 tmp25;\
    uint32 tmp24;\
    uint32 tmp23;\
    uint32 tmp22;\
    uint32 tmp21;\
    uint32 tmp20;\
    uint32 tmp19;\
    uint32 tmp18;\
    uint32 tmp17;\
    uint16 tmp16;\
    uint16 tmp15;\
    uint16 tmp14;\
    uint16 tmp13;\
    uint16 tmp12;\
    uint16 tmp11;\
    uint16 tmp10;\
    uint16 tmp9;\
    uint8 tmp8;\
    uint8 tmp7;\
    uint8 tmp6;\
    uint8 tmp5;\
    uint8 tmp4;\
    uint8 tmp3;\
    uint8 tmp2;\
    uint8 tmp1;

#ifdef WIN32
#define MUTE_WARNING_UNUSED
#else
#define MUTE_WARNING_UNUSED __attribute__((unused))
#endif

#define BB_SPECIFIC_LOCALS \
    uint32 opA        MUTE_WARNING_UNUSED; \
    uint32 opB        MUTE_WARNING_UNUSED; \
    uint32 h1_opA     MUTE_WARNING_UNUSED; \
    uint32 h0_opA     MUTE_WARNING_UNUSED; \
    uint32 h1_opB     MUTE_WARNING_UNUSED; \
    uint32 h0_opB     MUTE_WARNING_UNUSED; \
    uint32 b3_opA     MUTE_WARNING_UNUSED; \
    uint32 b2_opA     MUTE_WARNING_UNUSED; \
    uint32 b1_opA     MUTE_WARNING_UNUSED; \
    uint32 b0_opA     MUTE_WARNING_UNUSED; \
    uint32 b3_opB     MUTE_WARNING_UNUSED; \
    uint32 b2_opB     MUTE_WARNING_UNUSED; \
    uint32 b1_opB     MUTE_WARNING_UNUSED; \
    uint32 b0_opB     MUTE_WARNING_UNUSED; \
    uint32 opB5       MUTE_WARNING_UNUSED; \
    uint1 b0_condu1   MUTE_WARNING_UNUSED; \
    uint1 b1_condu1   MUTE_WARNING_UNUSED; \
    uint1 b2_condu1   MUTE_WARNING_UNUSED; \
    uint1 b3_condu1   MUTE_WARNING_UNUSED; \
    uint1 condu1      MUTE_WARNING_UNUSED; \
    uint32 opA16      MUTE_WARNING_UNUSED; \
    uint32 opB16      MUTE_WARNING_UNUSED; \
    uint32 opA8       MUTE_WARNING_UNUSED; \
    uint32 opB8       MUTE_WARNING_UNUSED; \
    uint1 h0_condu1   MUTE_WARNING_UNUSED; \
    uint1 h1_condu1   MUTE_WARNING_UNUSED; \
    uint1 cond        MUTE_WARNING_UNUSED;
//    uint1 bp_hit0   MUTE_WARNING_UNUSED;
//    uint1 bp_hit1   MUTE_WARNING_UNUSED;
//    uint1 bp_hit2   MUTE_WARNING_UNUSED;

#define DECLARE_LOCALS BB_SPECIFIC_LOCALS

#define BB_TRUE__ 1
#define BB_FALSE__ 0
#define BB_NOT__(a) ((~(a))&1)

#endif // _DATA_TYPE_H_
