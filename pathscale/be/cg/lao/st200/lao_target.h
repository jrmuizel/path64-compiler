

#define i386 1
#define linux 1
#define unix 1



#define Except_REQUIRE(t) 


//
//



//
//
typedef enum {
#define Platform(NAME,CHARWIDTH,ADDRWIDTH,ENDIAN) Platform_ ##NAME,

  Platform_,

/*
  st200/Platform.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Platform_st200_arch,

#undef Platform


#define PlatformInt_TYPEDEF typedef int32_t PlatformInt;



#define PlatformUInt_TYPEDEF typedef uint32_t PlatformUInt;


  Platform__
#undef Platform
} enum_Platform;

/*
 * Platform_INIT --	Initialize the Platform module.
 */
#define Platform_INIT() 

/*
 * Platform_FINI --	Finalize the Platform module.
 */
#define Platform_FINI() 






//
//
typedef enum {
#define Resource(NAME,AVAILABILITY) Resource_ ##NAME,

  Resource_,

/*
  st200/Resource.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Resource_st200_ISSUE,

Resource_st200_MEM,

Resource_st200_CTL,

Resource_st200_ODD,

Resource_st200_EVEN,

Resource_st200_LANE0,


#undef Resource

  Resource__
#undef Resource
} enum_Resource;

/*
 * Resource_INIT --	Initialize the Resource module.
 */
#define Resource_INIT() 

/*
 * Resource_FINI --	Finalize the Resource module.
 */
#define Resource_FINI() 






//
//
typedef enum {
#define Processor(NAME,MINTAKEN,INTERLOCKS,AVAILABILITY) Processor_ ##NAME,

  Processor_,

/*
  st200/Processor.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Processor_st220_cpu,

Processor_st231_cpu,

Processor_st240_cpu,


#undef Processor

  Processor__
#undef Processor
} enum_Processor;

/*
 * Processor_setMinTaken --	Change the minimum branch taken penalty.
 */
void
Processor_setMinTaken(Processor THIS, int minTaken);

/*
 * Processor_setMaxIssue --	Change the maxIssue.
 */
void
Processor_setMaxIssue(Processor THIS, int maxIssue);

/*
 * Processor_INIT --	Initialize the Processor module.
 */
#define Processor_INIT() 

/*
 * Processor_FINI --	Finalize the Processor module.
 */
#define Processor_FINI() 






//
//
typedef enum {
#define Encoding(NAME,PROCESSOR,WORDTYPE,WORDCOUNT) Encoding_ ##NAME,

  Encoding_,

/*
  st200/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Encoding_st220_ANY,
Encoding_st220_ANYX,
Encoding_st231_ANY,
Encoding_st231_ANYX,
Encoding_st240_ANY,
Encoding_st240_ANYX,

#undef Encoding


#define Encoding_SIZE_MAX 8


  Encoding__
#undef Encoding
} enum_Encoding;

/*
 * Encoding word types.
 */
#define WORDTYPE(type) type
#define Encoding(NAME,PROCESSOR,WORDTYPE,WORDCOUNT) typedef WORDTYPE Encoding_ ##NAME ##_t;


/*
  st200/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



typedef uint32_t Encoding_st220_ANY_t;
typedef uint32_t Encoding_st220_ANYX_t;
typedef uint32_t Encoding_st231_ANY_t;
typedef uint32_t Encoding_st231_ANYX_t;
typedef uint32_t Encoding_st240_ANY_t;
typedef uint32_t Encoding_st240_ANYX_t;

#undef Encoding





#undef WORDTYPE

/*
 * Encoding word counts.
 */
enum {
#define WORDCOUNT(d) d
#define Encoding(NAME,PROCESSOR,WORDTYPE,WORDCOUNT) Encoding_ ##NAME ##_count = WORDCOUNT,


/*
  st200/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Encoding_st220_ANY_count = 1,
Encoding_st220_ANYX_count = 2,
Encoding_st231_ANY_count = 1,
Encoding_st231_ANYX_count = 2,
Encoding_st240_ANY_count = 1,
Encoding_st240_ANYX_count = 2,

#undef Encoding





#undef WORDCOUNT
};

/*
 * Encoding_INIT --	Initialize the Encoding module.
 */
#define Encoding_INIT() 

/*
 * Encoding_FINI --	Finalize the Encoding module.
 */
#define Encoding_FINI() 






//
//
typedef enum {
#define NativeType(NAME,WIDTH,SIGNED,SIZEOF,ALIGN,FORMAT,SLICE) NativeType_ ##NAME,

  NativeType_,

/*
  st200/NativeType.enum --
  Automatically generated from the Machine Description System (MDS).
*/



NativeType_st200_Bool,
NativeType_st200_UInt8,
NativeType_st200_Int8,
NativeType_st200_UInt16,
NativeType_st200_Int16,
NativeType_st200_UInt32,
NativeType_st200_Int32,
NativeType_st200_UInt64,
NativeType_st200_Int64,
NativeType_st200_UIntPtr,
NativeType_st200_IntPtr,
NativeType_st200_Float32,
NativeType_st200_Float64,

#undef NativeType

  NativeType__
#undef NativeType
} enum_NativeType;

/*
 * NativeType_INIT --	Initialize the NativeType module.
 */
#define NativeType_INIT() 

/*
 * NativeType_FINI --	Finalize the NativeType module.
 */
#define NativeType_FINI() 






//
//
typedef enum {
#define StorageCell(NAME,KIND,WIDTH) StorageCell_ ##NAME,

  StorageCell_,

/*
  st200/Storage.enum --
  Automatically generated from the Machine Description System (MDS).
*/



StorageCell_st200_PC,
StorageCell_st200_BR0,
StorageCell_st200_BR1,
StorageCell_st200_BR2,
StorageCell_st200_BR3,
StorageCell_st200_BR4,
StorageCell_st200_BR5,
StorageCell_st200_BR6,
StorageCell_st200_BR7,
StorageCell_st200_GR0,
StorageCell_st200_GR1,
StorageCell_st200_GR2,
StorageCell_st200_GR3,
StorageCell_st200_GR4,
StorageCell_st200_GR5,
StorageCell_st200_GR6,
StorageCell_st200_GR7,
StorageCell_st200_GR8,
StorageCell_st200_GR9,
StorageCell_st200_GR10,
StorageCell_st200_GR11,
StorageCell_st200_GR12,
StorageCell_st200_GR13,
StorageCell_st200_GR14,
StorageCell_st200_GR15,
StorageCell_st200_GR16,
StorageCell_st200_GR17,
StorageCell_st200_GR18,
StorageCell_st200_GR19,
StorageCell_st200_GR20,
StorageCell_st200_GR21,
StorageCell_st200_GR22,
StorageCell_st200_GR23,
StorageCell_st200_GR24,
StorageCell_st200_GR25,
StorageCell_st200_GR26,
StorageCell_st200_GR27,
StorageCell_st200_GR28,
StorageCell_st200_GR29,
StorageCell_st200_GR30,
StorageCell_st200_GR31,
StorageCell_st200_GR32,
StorageCell_st200_GR33,
StorageCell_st200_GR34,
StorageCell_st200_GR35,
StorageCell_st200_GR36,
StorageCell_st200_GR37,
StorageCell_st200_GR38,
StorageCell_st200_GR39,
StorageCell_st200_GR40,
StorageCell_st200_GR41,
StorageCell_st200_GR42,
StorageCell_st200_GR43,
StorageCell_st200_GR44,
StorageCell_st200_GR45,
StorageCell_st200_GR46,
StorageCell_st200_GR47,
StorageCell_st200_GR48,
StorageCell_st200_GR49,
StorageCell_st200_GR50,
StorageCell_st200_GR51,
StorageCell_st200_GR52,
StorageCell_st200_GR53,
StorageCell_st200_GR54,
StorageCell_st200_GR55,
StorageCell_st200_GR56,
StorageCell_st200_GR57,
StorageCell_st200_GR58,
StorageCell_st200_GR59,
StorageCell_st200_GR60,
StorageCell_st200_GR61,
StorageCell_st200_GR62,
StorageCell_st200_GR63,
StorageCell_st200_MEM,
StorageCell_Volatile,

#undef Storage


#define StorageCell_Control StorageCell_st200_PC



#define StorageCell_Memory StorageCell_st200_MEM


  StorageCell__
#undef StorageCell
} enum_StorageCell;

/*
 * Storage_INIT --	Initialize the Storage module.
 */
#define Storage_INIT() 

/*
 * Storage_FINI --	Finalize the Storage module.
 */
#define Storage_FINI() 






//
//
typedef enum {
#define Register(NAME,NAMES,REGFILE,WIRED,STORAGECELLS,MINREADSTAGES) Register_ ##NAME,

  Register_,

/*
  st200/Register.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Register_st200_BR0,


Register_st200_BR1,


Register_st200_BR2,


Register_st200_BR3,


Register_st200_BR4,


Register_st200_BR5,


Register_st200_BR6,


Register_st200_BR7,


Register_st200_GR0,


Register_st200_GR1,


Register_st200_GR2,


Register_st200_GR3,


Register_st200_GR4,


Register_st200_GR5,


Register_st200_GR6,


Register_st200_GR7,


Register_st200_GR8,


Register_st200_GR9,


Register_st200_GR10,


Register_st200_GR11,


Register_st200_GR12,


Register_st200_GR13,


Register_st200_GR14,


Register_st200_GR15,


Register_st200_GR16,


Register_st200_GR17,


Register_st200_GR18,


Register_st200_GR19,


Register_st200_GR20,


Register_st200_GR21,


Register_st200_GR22,


Register_st200_GR23,


Register_st200_GR24,


Register_st200_GR25,


Register_st200_GR26,


Register_st200_GR27,


Register_st200_GR28,


Register_st200_GR29,


Register_st200_GR30,


Register_st200_GR31,


Register_st200_GR32,


Register_st200_GR33,


Register_st200_GR34,


Register_st200_GR35,


Register_st200_GR36,


Register_st200_GR37,


Register_st200_GR38,


Register_st200_GR39,


Register_st200_GR40,


Register_st200_GR41,


Register_st200_GR42,


Register_st200_GR43,


Register_st200_GR44,


Register_st200_GR45,


Register_st200_GR46,


Register_st200_GR47,


Register_st200_GR48,


Register_st200_GR49,


Register_st200_GR50,


Register_st200_GR51,


Register_st200_GR52,


Register_st200_GR53,


Register_st200_GR54,


Register_st200_GR55,


Register_st200_GR56,


Register_st200_GR57,


Register_st200_GR58,


Register_st200_GR59,


Register_st200_GR60,


Register_st200_GR61,


Register_st200_GR62,


Register_st200_GR63,


Register_st200_LR,


Register_st200_PR0,


Register_st200_PR1,


Register_st200_PR2,


Register_st200_PR3,


Register_st200_PR4,


Register_st200_PR5,


Register_st200_PR6,


Register_st200_PR7,


Register_st200_PR8,


Register_st200_PR9,


Register_st200_PR10,


Register_st200_PR11,


Register_st200_PR12,


Register_st200_PR13,


Register_st200_PR14,


Register_st200_PR15,


Register_st200_PR16,


Register_st200_PR17,


Register_st200_PR18,


Register_st200_PR19,


Register_st200_PR20,


Register_st200_PR21,


Register_st200_PR22,


Register_st200_PR23,


Register_st200_PR24,


Register_st200_PR25,


Register_st200_PR26,


Register_st200_PR27,


Register_st200_PR28,


Register_st200_PR29,


Register_st200_PR30,



#undef Register


#define RegisterStorageCells_COUNT_MAX 2



#define RegisterList_COUNT_MAX 104



#define RegisterSet_WORDTYPE uint32_t


#define RegisterSet_WORDBITS (sizeof(RegisterSet_WORDTYPE)*8)


#define RegisterSet_MAJOR(member) ((Register)(member)/(RegisterSet_WORDBITS))


#define RegisterSet_MINOR(member) ((Register)(member)&(RegisterSet_WORDBITS - 1))


#define RegisterSet_WORDCOUNT (RegisterSet_MAJOR(104) + 1)



#define Register_NAMES_COUNT_MAX 1


  Register__,
#undef Register
} enum_Register;
#define Register__BIAS (Register_ + 1)

/*
 * Register_INIT --	Initialize the Register module.
 */
#define Register_INIT() 

/*
 * Register_FINI --	Finalize the Register module.
 */
#define Register_FINI() 






//
//
typedef enum {
#define RegFile(NAME,WIDTH,LOWREGISTER,HIGHREGISTER,NATIVETYPES) RegFile_ ##NAME,

  RegFile_,

/*
  st200/RegFile.enum --
  Automatically generated from the Machine Description System (MDS).
*/



RegFile_st200_BR,

RegFile_st200_GR,

RegFile_st200_PR,


#undef RegFile

  RegFile__
#undef RegFile
} enum_RegFile;

/*
 * RegFile_INIT --	Initialize the RegFile module.
 */
void
RegFile_INIT(void);

/*
 * RegFile_FINI --	Finalize the RegFile module.
 */
#define RegFile_FINI() 






//
//
typedef enum {
#define RegClass(NAME,REGFILE,REGISTERS,ENCODE,DECODE,MINREADSTAGES) RegClass_ ##NAME,

  RegClass_,

/*
  st200/RegClass.enum --
  Automatically generated from the Machine Description System (MDS).
*/



RegClass_st200_branch,





RegClass_st200_general,





RegClass_st200_nolink,





RegClass_st200_link,





RegClass_st200_predicate,





RegClass_st200_paired,





RegClass_st200_pairedfirst,





RegClass_st200_pairedsecond,





RegClass_st200_nzpaired,





RegClass_st200_nzpairedfirst,





RegClass_st200_nzpairedsecond,






#undef RegClass

  RegClass__
#undef RegClass
} enum_RegClass;

/*
 * RegClassEncode functions.
 */
#define ENCODE(e) e
#define RegClass(NAME,REGFILE,REGISTERS,ENCODE,DECODE,MINREADSTAGES) static inline OperandEncoded RegClassEncode_ ##NAME(OperandDecoded decoded) { OperandEncoded VALUE = decoded - Register__BIAS; ENCODE; return VALUE; }
/*
  st200/RegClass.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline OperandEncoded RegClassEncode_st200_branch(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); /**/; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_general(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 8; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_nolink(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 8; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_link(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 71; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_predicate(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 1; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_paired(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 73; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_pairedfirst(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 8; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_pairedsecond(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 8; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_nzpaired(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 74; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_nzpairedfirst(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 10; return VALUE; }





static inline OperandEncoded RegClassEncode_st200_nzpairedsecond(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 11; return VALUE; }






#undef RegClass

#undef ENCODE

/*
 * RegClass_INIT --	Initialize the RegClass module.
 */
void
RegClass_INIT(void);

/*
 * RegClass_FINI --	Finalize the RegClass module.
 */
#define RegClass_FINI() 






//
//
typedef enum {
#define RegMask(NAME,REGISTERS,REGFILE,ENCODE,DECODE) RegMask_ ##NAME,

  RegMask_,

/*
  st200/RegMask.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef RegMask

  RegMask__
#undef RegMask
} enum_RegMask;

/*
 * RegMaskEncode functions.
 */
#define ENCODE(e) e
#define RegMask(NAME,REGISTERS,REGFILE,ENCODE,DECODE) static inline OperandEncoded RegMaskEncode_ ##NAME(OperandDecoded decoded) { RegisterSet registerSet = (RegisterSet)decoded; RegisterSetWord *WORDS = RegisterSet_WORDS(registerSet); RegisterSetWord WORD = 0; ENCODE; return (OperandEncoded)WORD; }
/*
  st200/RegMask.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef RegMask

#undef ENCODE

/*
 * RegMask_INIT --	Initialize the RegMask module.
 */
void
RegMask_INIT(void);

/*
 * RegMask_FINI --	Finalize the RegMask module.
 */
#define RegMask_FINI() 






//
//
typedef enum {
#define Modifier(NAME,MEMBERS,ENCODE,DECODE) Modifier_ ##NAME,

  Modifier_,

/*
  st200/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Modifier


#define Modifier_NAMES_COUNT_MAX 0


  Modifier__
#undef Modifier
} enum_Modifier;

//
typedef enum {
#define ALIAS(alias,name) ModifierMember_ ##alias = ModifierMember_ ##name,
#define ALIASES(count,items) items
#define MEMBER(name,ENCODED,NAMES,ALIASES) ModifierMember_ ##name, ALIASES

#define MEMBERS(count,items) items
#define Modifier(NAME,MEMBERS,ENCODE,DECODE) MEMBERS

  ModifierMember_,

/*
  st200/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Modifier





  ModifierMember__
#undef Modifier
#undef MEMBERS
#undef MEMBER
#undef ALIASES
#undef ALIAS
#undef ENCODED
} enum_ModifierMember;

/*
 * ModifierEncode functions.
 */
#define ENCODE(e) e
#define Modifier(NAME,VALUES,ENCODE,DECODE) static inline OperandEncoded ModifierEncode_ ##NAME(OperandDecoded decoded) { OperandEncoded VALUE = decoded; ENCODE; return VALUE; }
/*
  st200/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Modifier





#undef ENCODE

/*
 * Modifier_INIT --	Initialize the Modifier module.
 */
#define Modifier_INIT() 

/*
 * Modifier_FINI --	Finalize the Modifier module.
 */
#define Modifier_FINI() 






//
//
typedef enum {
#define Relocation(NAME,RELATIVE,ENCODE,DECODE,LINKER) Relocation_ ##NAME,

  Relocation_,

/*
  st200/Relocation.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Relocation_st200_btarg,



Relocation_st200_isrc2,



Relocation_st200_xsrc2,



Relocation_st200_xsrc2_gprel,



Relocation_st200_xsrc2_gotoff,



Relocation_st200_xsrc2_neggprel,




#undef Relocation

  Relocation__
#undef Relocation
} enum_Relocation;

/*
 * Relocation_INIT --	Initialize the Relocation module.
 */
#define Relocation_INIT() 

/*
 * Relocation_FINI --	Finalize the Relocation module.
 */
#define Relocation_FINI() 






//
//
typedef enum {
#define Immediate(NAME,MINVALUE,MAXVALUE,SIGNED,RELOCATIONS,ENCODE,DECODE) Immediate_ ##NAME,

  Immediate_,

/*
  st200/Immediate.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Immediate_st200_btarg,



Immediate_st200_isrc2,



Immediate_st200_imm,



Immediate_st200_sbrknum,



Immediate_st200_xsrc2,



Immediate_st200_brknum,




#undef Immediate


#define ImmediateRelocations_COUNT_MAX 4


  Immediate__
#undef Immediate
} enum_Immediate;

/*
 * ImmediateEncode functions.
 */
#define ENCODE(e) e
#define Immediate(NAME,MINVALUE,MAXVALUE,SIGNED,RELOCATIONS,ENCODE,DECODE) static inline OperandEncoded ImmediateEncode_ ##NAME(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { ENCODE; } return (OperandEncoded)VALUE; }
/*
  st200/Immediate.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline OperandEncoded ImmediateEncode_st200_btarg(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value >>= 2; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_st200_isrc2(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_st200_imm(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_st200_sbrknum(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<21)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_st200_xsrc2(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_st200_brknum(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<12)-1; VALUE = __value; } return (OperandEncoded)VALUE; }




#undef Immediate





#undef ENCODE

/*
 * Immediate_INIT --	Initialize the Immediate module.
 */
#define Immediate_INIT() 

/*
 * Immediate_FINI --	Finalize the Immediate module.
 */
#define Immediate_FINI() 






//
//
typedef enum {
#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) Operand_ ##NAME,

  Operand_,

/*
  st200/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Operand_st200_bcond,


Operand_st200_bdest,


Operand_st200_bdest2,


Operand_st200_btarg,


Operand_st200_dest,


Operand_st200_nldest,


Operand_st200_ibdest,


Operand_st200_idest,


Operand_st200_nlidest,


Operand_st200_isrc2,


Operand_st200_imm,


Operand_st200_scond,


Operand_st200_src1,


Operand_st200_src2,


Operand_st200_sbrknum,


Operand_st200_xsrc2,


Operand_st200_bsrc1,


Operand_st200_bsrc2,


Operand_st200_idestp,


Operand_st200_pcond,


Operand_st200_src2p,


Operand_st200_brknum,



#undef Operand


#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) 
#undef Operands


#define Operands_COUNT_MAX 5



#define Operands_IMMEDIATE_COUNT_MAX 1



#define Operands_REGMASK_COUNT_MAX 0


  Operand__
#undef Operand
} enum_Operand;

//
typedef enum {
#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) Operands_ ##NAME,

  Operands_,

/*
  st200/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/

#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) 
#undef Operand




Operands_st200_dest_src1_src2,


Operands_st200_idest_src1_isrc2,


Operands_st200_idest_src1_xsrc2,


Operands_st200_dest_bdest_src1_src2_scond,


Operands_st200_bdest_src1_src2,


Operands_st200_ibdest_src1_isrc2,


Operands_st200_ibdest_src1_xsrc2,


Operands_st200_bcond_btarg,


Operands_st200_idest_src1,


Operands_st200_btarg,


Operands_st200_imm,


Operands_st200_nlidest_isrc2_src1,


Operands_st200_nlidest_xsrc2_src1,


Operands_st200_idest_isrc2_src1,


Operands_st200_idest_xsrc2_src1,


Operands_st200_nldest_src1_src2,


Operands_st200_nlidest_src1_isrc2,


Operands_st200_nlidest_src1_xsrc2,


Operands_st200_isrc2_src1,


Operands_st200_xsrc2_src1,


Operands_st200_sbrknum,


Operands_st200_dest_scond_src1_src2,


Operands_st200_idest_scond_src1_isrc2,


Operands_st200_idest_scond_src1_xsrc2,


Operands_st200_isrc2_src1_src2,


Operands_st200_xsrc2_src1_src2,


Operands_st200_dest_src2_src1,


Operands_st200_idest_scond,


Operands_st200_bdest_src1,


Operands_st200_dest_src2,


Operands_st200_idest_isrc2,


Operands_st200_idest_xsrc2,


Operands_st200_src2,


Operands_st200_bdest2_src1_src2,


Operands_st200_dest_src1,


Operands_st200_nldest_scond_src1_src2,


Operands_st200_bdest2_src1,


Operands_st200_dest_scond,


Operands_st200_bdest2_bsrc1_bsrc2,


Operands_st200_nldest_src1,


Operands_st200_brknum,


Operands_st200_nlidest_pcond_isrc2_src1,


Operands_st200_nlidest_pcond_xsrc2_src1,


Operands_st200_idestp_isrc2_src1,


Operands_st200_idestp_xsrc2_src1,


Operands_st200_idestp_pcond_isrc2_src1,


Operands_st200_idestp_pcond_xsrc2_src1,


Operands_st200_idest_pcond_isrc2_src1,


Operands_st200_idest_pcond_xsrc2_src1,


Operands_st200_bdest2_bsrc1,


Operands_st200_pcond_isrc2_src1,


Operands_st200_pcond_xsrc2_src1,


Operands_st200_isrc2_src1_pcond_src2,


Operands_st200_xsrc2_src1_pcond_src2,


Operands_st200_isrc2_src1_src2p,


Operands_st200_xsrc2_src1_src2p,


Operands_st200_isrc2_src1_pcond_src2p,


Operands_st200_xsrc2_src1_pcond_src2p,


Operands_st200_bdest2_src2_src1,


Operands_st200_dest_scond_src2_src1,



#undef Operands
  Operands__
#undef Operand
} enum_Operands;

/*
  Decode_Operand function prototypes.
 */
#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) void Decode_Operand_ ##NAME(const void *opcodes, void *restrict result);



/*
  st200/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/



void Decode_Operand_st200_bcond(const void *opcodes, void *restrict result);


void Decode_Operand_st200_bdest(const void *opcodes, void *restrict result);


void Decode_Operand_st200_bdest2(const void *opcodes, void *restrict result);


void Decode_Operand_st200_btarg(const void *opcodes, void *restrict result);


void Decode_Operand_st200_dest(const void *opcodes, void *restrict result);


void Decode_Operand_st200_nldest(const void *opcodes, void *restrict result);


void Decode_Operand_st200_ibdest(const void *opcodes, void *restrict result);


void Decode_Operand_st200_idest(const void *opcodes, void *restrict result);


void Decode_Operand_st200_nlidest(const void *opcodes, void *restrict result);


void Decode_Operand_st200_isrc2(const void *opcodes, void *restrict result);


void Decode_Operand_st200_imm(const void *opcodes, void *restrict result);


void Decode_Operand_st200_scond(const void *opcodes, void *restrict result);


void Decode_Operand_st200_src1(const void *opcodes, void *restrict result);


void Decode_Operand_st200_src2(const void *opcodes, void *restrict result);


void Decode_Operand_st200_sbrknum(const void *opcodes, void *restrict result);


void Decode_Operand_st200_xsrc2(const void *opcodes, void *restrict result);


void Decode_Operand_st200_bsrc1(const void *opcodes, void *restrict result);


void Decode_Operand_st200_bsrc2(const void *opcodes, void *restrict result);


void Decode_Operand_st200_idestp(const void *opcodes, void *restrict result);


void Decode_Operand_st200_pcond(const void *opcodes, void *restrict result);


void Decode_Operand_st200_src2p(const void *opcodes, void *restrict result);


void Decode_Operand_st200_brknum(const void *opcodes, void *restrict result);



#undef Operand


#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) 
#undef Operands
/*
 * OperandsEncode function prototypes.
 */
#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) void OperandsEncode_ ##NAME(const_OperandsBuffer buffer, void *restrict opcodes);



/*
  st200/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/

#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) 
#undef Operand




void OperandsEncode_st200_dest_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_src1_isrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_src1_xsrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_bdest_src1_src2_scond(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_ibdest_src1_isrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_ibdest_src1_xsrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bcond_btarg(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_btarg(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_imm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nlidest_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nlidest_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nldest_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nlidest_src1_isrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nlidest_src1_xsrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_sbrknum(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_scond_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_scond_src1_isrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_scond_src1_xsrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_isrc2_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_xsrc2_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_src2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_scond(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_isrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_xsrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest2_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nldest_scond_src1_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_scond(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest2_bsrc1_bsrc2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nldest_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_brknum(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nlidest_pcond_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_nlidest_pcond_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idestp_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idestp_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idestp_pcond_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idestp_pcond_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_pcond_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_idest_pcond_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest2_bsrc1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_pcond_isrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_pcond_xsrc2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_isrc2_src1_pcond_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_xsrc2_src1_pcond_src2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_isrc2_src1_src2p(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_xsrc2_src1_src2p(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_isrc2_src1_pcond_src2p(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_xsrc2_src1_pcond_src2p(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_bdest2_src2_src1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_st200_dest_scond_src2_src1(const_OperandsBuffer buffer, void *restrict opcodes);



#undef Operands
void
OperandsEncode_(const_OperandsBuffer buffer, void *restrict opcodes);

/*
 * Decode_Operands function prototypes.
 */

#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) void Decode_Operands_ ##NAME(const void *opcodes, OperandsBuffer buffer);



/*
  st200/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/

#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) 
#undef Operand




void Decode_Operands_st200_dest_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_src1_isrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_src1_xsrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_bdest_src1_src2_scond(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_ibdest_src1_isrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_ibdest_src1_xsrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bcond_btarg(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_btarg(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_imm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nlidest_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nlidest_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nldest_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nlidest_src1_isrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nlidest_src1_xsrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_sbrknum(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_scond_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_scond_src1_isrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_scond_src1_xsrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_isrc2_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_xsrc2_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_src2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_scond(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_isrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_xsrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest2_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nldest_scond_src1_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_scond(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest2_bsrc1_bsrc2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nldest_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_brknum(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nlidest_pcond_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_nlidest_pcond_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idestp_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idestp_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idestp_pcond_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idestp_pcond_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_pcond_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_idest_pcond_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest2_bsrc1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_pcond_isrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_pcond_xsrc2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_isrc2_src1_pcond_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_xsrc2_src1_pcond_src2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_isrc2_src1_src2p(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_xsrc2_src1_src2p(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_isrc2_src1_pcond_src2p(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_xsrc2_src1_pcond_src2p(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_bdest2_src2_src1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_st200_dest_scond_src2_src1(const void *opcodes, OperandsBuffer buffer);



#undef Operands
/*
 * Operand_INIT --	Initialize the Operand module.
 */
#define Operand_INIT() 

/*
 * Operand_FINI --	Finalize the Operand module.
 */
#define Operand_FINI() 






//
//
typedef enum {
#define Format(NAME,OPERANDS) Format_ ##NAME,

  Format_,

/*
  st200/Format.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Format

  Format__
#undef Format
} enum_Format;

/*
 * Format_INIT --	Initialize the Format module.
 */
#define Format_INIT() 

/*
 * Format_FINI --	Finalize the Format module.
 */
#define Format_FINI() 






//
//
typedef enum {
#define Syntax(NAME,CONTENTS) Syntax_ ##NAME,

  Syntax_,

/*
  st200/Syntax.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Syntax

  Syntax__
#undef Syntax
} enum_Syntax;

/*
 * Syntax_INIT --	Initialize the Syntax module.
 */
#define Syntax_INIT() 

/*
 * Syntax_FINI --	Finalize the Syntax module.
 */
#define Syntax_FINI() 






//
//
typedef enum {
#define Instance(NAME,SCHEDULING,ENCODING,OPCODES,OPERANDS,INCREMENT,MNEMONIC,SYNTAX,ACTIONARRAY,OPERATOR) Instance_ ##NAME,


  Instance_,

/*
  st200/Instance.enum --
  Automatically generated from the Machine Description System (MDS).
*/




Instance_st220_add_dest_src1_src2,
Instance_st220_add_idest_src1_isrc2,
Instance_st220_add_idest_src1_xsrc2,
Instance_st220_addcg_dest_bdest_src1_src2_scond,
Instance_st220_and_dest_src1_src2,
Instance_st220_and_idest_src1_isrc2,
Instance_st220_and_idest_src1_xsrc2,
Instance_st220_andc_dest_src1_src2,
Instance_st220_andc_idest_src1_isrc2,
Instance_st220_andc_idest_src1_xsrc2,
Instance_st220_andl_dest_src1_src2,
Instance_st220_andl_bdest_src1_src2,
Instance_st220_andl_idest_src1_isrc2,
Instance_st220_andl_ibdest_src1_isrc2,
Instance_st220_andl_idest_src1_xsrc2,
Instance_st220_andl_ibdest_src1_xsrc2,
Instance_st220_asm_0_dest_src1_src2,
Instance_st220_asm_1_dest_src1_src2,
Instance_st220_asm_2_dest_src1_src2,
Instance_st220_asm_3_dest_src1_src2,
Instance_st220_asm_4_dest_src1_src2,
Instance_st220_asm_5_dest_src1_src2,
Instance_st220_asm_6_dest_src1_src2,
Instance_st220_asm_7_dest_src1_src2,
Instance_st220_asm_8_dest_src1_src2,
Instance_st220_asm_9_dest_src1_src2,
Instance_st220_asm_10_dest_src1_src2,
Instance_st220_asm_11_dest_src1_src2,
Instance_st220_asm_12_dest_src1_src2,
Instance_st220_asm_13_dest_src1_src2,
Instance_st220_asm_14_dest_src1_src2,
Instance_st220_asm_15_dest_src1_src2,
Instance_st220_asm_16_idest_src1_isrc2,
Instance_st220_asm_16_idest_src1_xsrc2,
Instance_st220_asm_17_idest_src1_isrc2,
Instance_st220_asm_17_idest_src1_xsrc2,
Instance_st220_asm_18_idest_src1_isrc2,
Instance_st220_asm_18_idest_src1_xsrc2,
Instance_st220_asm_19_idest_src1_isrc2,
Instance_st220_asm_19_idest_src1_xsrc2,
Instance_st220_asm_20_idest_src1_isrc2,
Instance_st220_asm_20_idest_src1_xsrc2,
Instance_st220_asm_21_idest_src1_isrc2,
Instance_st220_asm_21_idest_src1_xsrc2,
Instance_st220_asm_22_idest_src1_isrc2,
Instance_st220_asm_22_idest_src1_xsrc2,
Instance_st220_asm_23_idest_src1_isrc2,
Instance_st220_asm_23_idest_src1_xsrc2,
Instance_st220_asm_24_idest_src1_isrc2,
Instance_st220_asm_24_idest_src1_xsrc2,
Instance_st220_asm_25_idest_src1_isrc2,
Instance_st220_asm_25_idest_src1_xsrc2,
Instance_st220_asm_26_idest_src1_isrc2,
Instance_st220_asm_26_idest_src1_xsrc2,
Instance_st220_asm_27_idest_src1_isrc2,
Instance_st220_asm_27_idest_src1_xsrc2,
Instance_st220_asm_28_idest_src1_isrc2,
Instance_st220_asm_28_idest_src1_xsrc2,
Instance_st220_asm_29_idest_src1_isrc2,
Instance_st220_asm_29_idest_src1_xsrc2,
Instance_st220_asm_30_idest_src1_isrc2,
Instance_st220_asm_30_idest_src1_xsrc2,
Instance_st220_asm_31_idest_src1_isrc2,
Instance_st220_asm_31_idest_src1_xsrc2,
Instance_st220_br_bcond_btarg,
Instance_st220_break,
Instance_st220_brf_bcond_btarg,
Instance_st220_bswap_idest_src1,
Instance_st220_call_btarg,
Instance_st220_icall,
Instance_st220_clz_idest_src1,
Instance_st220_cmpeq_dest_src1_src2,
Instance_st220_cmpeq_bdest_src1_src2,
Instance_st220_cmpeq_idest_src1_isrc2,
Instance_st220_cmpeq_ibdest_src1_isrc2,
Instance_st220_cmpeq_idest_src1_xsrc2,
Instance_st220_cmpeq_ibdest_src1_xsrc2,
Instance_st220_cmpge_dest_src1_src2,
Instance_st220_cmpge_bdest_src1_src2,
Instance_st220_cmpge_idest_src1_isrc2,
Instance_st220_cmpge_ibdest_src1_isrc2,
Instance_st220_cmpge_idest_src1_xsrc2,
Instance_st220_cmpge_ibdest_src1_xsrc2,
Instance_st220_cmpgeu_dest_src1_src2,
Instance_st220_cmpgeu_bdest_src1_src2,
Instance_st220_cmpgeu_idest_src1_isrc2,
Instance_st220_cmpgeu_ibdest_src1_isrc2,
Instance_st220_cmpgeu_idest_src1_xsrc2,
Instance_st220_cmpgeu_ibdest_src1_xsrc2,
Instance_st220_cmpgt_dest_src1_src2,
Instance_st220_cmpgt_bdest_src1_src2,
Instance_st220_cmpgt_idest_src1_isrc2,
Instance_st220_cmpgt_ibdest_src1_isrc2,
Instance_st220_cmpgt_idest_src1_xsrc2,
Instance_st220_cmpgt_ibdest_src1_xsrc2,
Instance_st220_cmpgtu_dest_src1_src2,
Instance_st220_cmpgtu_bdest_src1_src2,
Instance_st220_cmpgtu_idest_src1_isrc2,
Instance_st220_cmpgtu_ibdest_src1_isrc2,
Instance_st220_cmpgtu_idest_src1_xsrc2,
Instance_st220_cmpgtu_ibdest_src1_xsrc2,
Instance_st220_cmple_dest_src1_src2,
Instance_st220_cmple_bdest_src1_src2,
Instance_st220_cmple_idest_src1_isrc2,
Instance_st220_cmple_ibdest_src1_isrc2,
Instance_st220_cmple_idest_src1_xsrc2,
Instance_st220_cmple_ibdest_src1_xsrc2,
Instance_st220_cmpleu_dest_src1_src2,
Instance_st220_cmpleu_bdest_src1_src2,
Instance_st220_cmpleu_idest_src1_isrc2,
Instance_st220_cmpleu_ibdest_src1_isrc2,
Instance_st220_cmpleu_idest_src1_xsrc2,
Instance_st220_cmpleu_ibdest_src1_xsrc2,
Instance_st220_cmplt_dest_src1_src2,
Instance_st220_cmplt_bdest_src1_src2,
Instance_st220_cmplt_idest_src1_isrc2,
Instance_st220_cmplt_ibdest_src1_isrc2,
Instance_st220_cmplt_idest_src1_xsrc2,
Instance_st220_cmplt_ibdest_src1_xsrc2,
Instance_st220_cmpltu_dest_src1_src2,
Instance_st220_cmpltu_bdest_src1_src2,
Instance_st220_cmpltu_idest_src1_isrc2,
Instance_st220_cmpltu_ibdest_src1_isrc2,
Instance_st220_cmpltu_idest_src1_xsrc2,
Instance_st220_cmpltu_ibdest_src1_xsrc2,
Instance_st220_cmpne_dest_src1_src2,
Instance_st220_cmpne_bdest_src1_src2,
Instance_st220_cmpne_idest_src1_isrc2,
Instance_st220_cmpne_ibdest_src1_isrc2,
Instance_st220_cmpne_idest_src1_xsrc2,
Instance_st220_cmpne_ibdest_src1_xsrc2,
Instance_st220_divs_dest_bdest_src1_src2_scond,
Instance_st220_goto_btarg,
Instance_st220_igoto,
Instance_st220_imml_imm,
Instance_st220_immr_imm,
Instance_st220_ldb_nlidest_isrc2_src1,
Instance_st220_ldb_nlidest_xsrc2_src1,
Instance_st220_ldb_d_nlidest_isrc2_src1,
Instance_st220_ldb_d_nlidest_xsrc2_src1,
Instance_st220_ldbu_nlidest_isrc2_src1,
Instance_st220_ldbu_nlidest_xsrc2_src1,
Instance_st220_ldbu_d_nlidest_isrc2_src1,
Instance_st220_ldbu_d_nlidest_xsrc2_src1,
Instance_st220_ldh_nlidest_isrc2_src1,
Instance_st220_ldh_nlidest_xsrc2_src1,
Instance_st220_ldh_d_nlidest_isrc2_src1,
Instance_st220_ldh_d_nlidest_xsrc2_src1,
Instance_st220_ldhu_nlidest_isrc2_src1,
Instance_st220_ldhu_nlidest_xsrc2_src1,
Instance_st220_ldhu_d_nlidest_isrc2_src1,
Instance_st220_ldhu_d_nlidest_xsrc2_src1,
Instance_st220_ldw_idest_isrc2_src1,
Instance_st220_ldw_idest_xsrc2_src1,
Instance_st220_ldw_d_idest_isrc2_src1,
Instance_st220_ldw_d_idest_xsrc2_src1,
Instance_st220_max_dest_src1_src2,
Instance_st220_max_idest_src1_isrc2,
Instance_st220_max_idest_src1_xsrc2,
Instance_st220_maxu_dest_src1_src2,
Instance_st220_maxu_idest_src1_isrc2,
Instance_st220_maxu_idest_src1_xsrc2,
Instance_st220_min_dest_src1_src2,
Instance_st220_min_idest_src1_isrc2,
Instance_st220_min_idest_src1_xsrc2,
Instance_st220_minu_dest_src1_src2,
Instance_st220_minu_idest_src1_isrc2,
Instance_st220_minu_idest_src1_xsrc2,
Instance_st220_mulh_nldest_src1_src2,
Instance_st220_mulh_nlidest_src1_isrc2,
Instance_st220_mulh_nlidest_src1_xsrc2,
Instance_st220_mulhh_nldest_src1_src2,
Instance_st220_mulhh_nlidest_src1_isrc2,
Instance_st220_mulhh_nlidest_src1_xsrc2,
Instance_st220_mulhhs_nldest_src1_src2,
Instance_st220_mulhhs_nlidest_src1_isrc2,
Instance_st220_mulhhs_nlidest_src1_xsrc2,
Instance_st220_mulhhu_nldest_src1_src2,
Instance_st220_mulhhu_nlidest_src1_isrc2,
Instance_st220_mulhhu_nlidest_src1_xsrc2,
Instance_st220_mulhs_nldest_src1_src2,
Instance_st220_mulhs_nlidest_src1_isrc2,
Instance_st220_mulhs_nlidest_src1_xsrc2,
Instance_st220_mulhu_nldest_src1_src2,
Instance_st220_mulhu_nlidest_src1_isrc2,
Instance_st220_mulhu_nlidest_src1_xsrc2,
Instance_st220_mull_nldest_src1_src2,
Instance_st220_mull_nlidest_src1_isrc2,
Instance_st220_mull_nlidest_src1_xsrc2,
Instance_st220_mullh_nldest_src1_src2,
Instance_st220_mullh_nlidest_src1_isrc2,
Instance_st220_mullh_nlidest_src1_xsrc2,
Instance_st220_mullhu_nldest_src1_src2,
Instance_st220_mullhu_nlidest_src1_isrc2,
Instance_st220_mullhu_nlidest_src1_xsrc2,
Instance_st220_mullhus_nldest_src1_src2,
Instance_st220_mullhus_nlidest_src1_isrc2,
Instance_st220_mullhus_nlidest_src1_xsrc2,
Instance_st220_mulll_nldest_src1_src2,
Instance_st220_mulll_nlidest_src1_isrc2,
Instance_st220_mulll_nlidest_src1_xsrc2,
Instance_st220_mulllu_nldest_src1_src2,
Instance_st220_mulllu_nlidest_src1_isrc2,
Instance_st220_mulllu_nlidest_src1_xsrc2,
Instance_st220_mullu_nldest_src1_src2,
Instance_st220_mullu_nlidest_src1_isrc2,
Instance_st220_mullu_nlidest_src1_xsrc2,
Instance_st220_nandl_dest_src1_src2,
Instance_st220_nandl_bdest_src1_src2,
Instance_st220_nandl_idest_src1_isrc2,
Instance_st220_nandl_ibdest_src1_isrc2,
Instance_st220_nandl_idest_src1_xsrc2,
Instance_st220_nandl_ibdest_src1_xsrc2,
Instance_st220_norl_dest_src1_src2,
Instance_st220_norl_bdest_src1_src2,
Instance_st220_norl_idest_src1_isrc2,
Instance_st220_norl_ibdest_src1_isrc2,
Instance_st220_norl_idest_src1_xsrc2,
Instance_st220_norl_ibdest_src1_xsrc2,
Instance_st220_or_dest_src1_src2,
Instance_st220_or_idest_src1_isrc2,
Instance_st220_or_idest_src1_xsrc2,
Instance_st220_orc_dest_src1_src2,
Instance_st220_orc_idest_src1_isrc2,
Instance_st220_orc_idest_src1_xsrc2,
Instance_st220_orl_dest_src1_src2,
Instance_st220_orl_bdest_src1_src2,
Instance_st220_orl_idest_src1_isrc2,
Instance_st220_orl_ibdest_src1_isrc2,
Instance_st220_orl_idest_src1_xsrc2,
Instance_st220_orl_ibdest_src1_xsrc2,
Instance_st220_pft_isrc2_src1,
Instance_st220_pft_xsrc2_src1,
Instance_st220_prgadd_isrc2_src1,
Instance_st220_prgadd_xsrc2_src1,
Instance_st220_prgset_isrc2_src1,
Instance_st220_prgset_xsrc2_src1,
Instance_st220_prgins,
Instance_st220_rfi,
Instance_st220_sbrk_sbrknum,
Instance_st220_sh1add_dest_src1_src2,
Instance_st220_sh1add_idest_src1_isrc2,
Instance_st220_sh1add_idest_src1_xsrc2,
Instance_st220_sh2add_dest_src1_src2,
Instance_st220_sh2add_idest_src1_isrc2,
Instance_st220_sh2add_idest_src1_xsrc2,
Instance_st220_sh3add_dest_src1_src2,
Instance_st220_sh3add_idest_src1_isrc2,
Instance_st220_sh3add_idest_src1_xsrc2,
Instance_st220_sh4add_dest_src1_src2,
Instance_st220_sh4add_idest_src1_isrc2,
Instance_st220_sh4add_idest_src1_xsrc2,
Instance_st220_shl_dest_src1_src2,
Instance_st220_shl_idest_src1_isrc2,
Instance_st220_shl_idest_src1_xsrc2,
Instance_st220_shr_dest_src1_src2,
Instance_st220_shr_idest_src1_isrc2,
Instance_st220_shr_idest_src1_xsrc2,
Instance_st220_shru_dest_src1_src2,
Instance_st220_shru_idest_src1_isrc2,
Instance_st220_shru_idest_src1_xsrc2,
Instance_st220_slct_dest_scond_src1_src2,
Instance_st220_slct_idest_scond_src1_isrc2,
Instance_st220_slct_idest_scond_src1_xsrc2,
Instance_st220_slctf_dest_scond_src1_src2,
Instance_st220_slctf_idest_scond_src1_isrc2,
Instance_st220_slctf_idest_scond_src1_xsrc2,
Instance_st220_stb_isrc2_src1_src2,
Instance_st220_stb_xsrc2_src1_src2,
Instance_st220_sth_isrc2_src1_src2,
Instance_st220_sth_xsrc2_src1_src2,
Instance_st220_stw_isrc2_src1_src2,
Instance_st220_stw_xsrc2_src1_src2,
Instance_st220_sub_dest_src2_src1,
Instance_st220_sub_idest_isrc2_src1,
Instance_st220_sub_idest_xsrc2_src1,
Instance_st220_sxtb_idest_src1,
Instance_st220_sxth_idest_src1,
Instance_st220_sync,
Instance_st220_syscall_sbrknum,
Instance_st220_xor_dest_src1_src2,
Instance_st220_xor_idest_src1_isrc2,
Instance_st220_xor_idest_src1_xsrc2,
Instance_st220_zxth_idest_src1,
Instance_st220_convbi_idest_scond,
Instance_st220_convib_bdest_src1,
Instance_st220_mov_bsrc_idest_scond,
Instance_st220_mov_bdest_bdest_src1,
Instance_st220_nop,
Instance_st220_mov_dest_src2,
Instance_st220_mov_idest_isrc2,
Instance_st220_mov_idest_xsrc2,
Instance_st220_mtb_bdest_src1,
Instance_st220_mfb_idest_scond,
Instance_st220_zxtb_idest_src1,
Instance_st220_syncins,
Instance_st220_return,
Instance_st231_add_dest_src1_src2,
Instance_st231_add_idest_src1_isrc2,
Instance_st231_add_idest_src1_xsrc2,
Instance_st231_addcg_dest_bdest_src1_src2_scond,
Instance_st231_and_dest_src1_src2,
Instance_st231_and_idest_src1_isrc2,
Instance_st231_and_idest_src1_xsrc2,
Instance_st231_andc_dest_src1_src2,
Instance_st231_andc_idest_src1_isrc2,
Instance_st231_andc_idest_src1_xsrc2,
Instance_st231_andl_dest_src1_src2,
Instance_st231_andl_bdest_src1_src2,
Instance_st231_andl_idest_src1_isrc2,
Instance_st231_andl_ibdest_src1_isrc2,
Instance_st231_andl_idest_src1_xsrc2,
Instance_st231_andl_ibdest_src1_xsrc2,
Instance_st231_asm_0_dest_src1_src2,
Instance_st231_asm_1_dest_src1_src2,
Instance_st231_asm_2_dest_src1_src2,
Instance_st231_asm_3_dest_src1_src2,
Instance_st231_asm_4_dest_src1_src2,
Instance_st231_asm_5_dest_src1_src2,
Instance_st231_asm_6_dest_src1_src2,
Instance_st231_asm_7_dest_src1_src2,
Instance_st231_asm_8_dest_src1_src2,
Instance_st231_asm_9_dest_src1_src2,
Instance_st231_asm_10_dest_src1_src2,
Instance_st231_asm_11_dest_src1_src2,
Instance_st231_asm_12_dest_src1_src2,
Instance_st231_asm_13_dest_src1_src2,
Instance_st231_asm_14_dest_src1_src2,
Instance_st231_asm_15_dest_src1_src2,
Instance_st231_asm_16_idest_src1_isrc2,
Instance_st231_asm_16_idest_src1_xsrc2,
Instance_st231_asm_17_idest_src1_isrc2,
Instance_st231_asm_17_idest_src1_xsrc2,
Instance_st231_asm_18_idest_src1_isrc2,
Instance_st231_asm_18_idest_src1_xsrc2,
Instance_st231_asm_19_idest_src1_isrc2,
Instance_st231_asm_19_idest_src1_xsrc2,
Instance_st231_asm_20_idest_src1_isrc2,
Instance_st231_asm_20_idest_src1_xsrc2,
Instance_st231_asm_21_idest_src1_isrc2,
Instance_st231_asm_21_idest_src1_xsrc2,
Instance_st231_asm_22_idest_src1_isrc2,
Instance_st231_asm_22_idest_src1_xsrc2,
Instance_st231_asm_23_idest_src1_isrc2,
Instance_st231_asm_23_idest_src1_xsrc2,
Instance_st231_asm_24_idest_src1_isrc2,
Instance_st231_asm_24_idest_src1_xsrc2,
Instance_st231_asm_25_idest_src1_isrc2,
Instance_st231_asm_25_idest_src1_xsrc2,
Instance_st231_asm_26_idest_src1_isrc2,
Instance_st231_asm_26_idest_src1_xsrc2,
Instance_st231_asm_27_idest_src1_isrc2,
Instance_st231_asm_27_idest_src1_xsrc2,
Instance_st231_asm_28_idest_src1_isrc2,
Instance_st231_asm_28_idest_src1_xsrc2,
Instance_st231_asm_29_idest_src1_isrc2,
Instance_st231_asm_29_idest_src1_xsrc2,
Instance_st231_asm_30_idest_src1_isrc2,
Instance_st231_asm_30_idest_src1_xsrc2,
Instance_st231_asm_31_idest_src1_isrc2,
Instance_st231_asm_31_idest_src1_xsrc2,
Instance_st231_br_bcond_btarg,
Instance_st231_break,
Instance_st231_brf_bcond_btarg,
Instance_st231_bswap_idest_src1,
Instance_st231_call_btarg,
Instance_st231_icall,
Instance_st231_clz_idest_src1,
Instance_st231_cmpeq_dest_src1_src2,
Instance_st231_cmpeq_bdest_src1_src2,
Instance_st231_cmpeq_idest_src1_isrc2,
Instance_st231_cmpeq_ibdest_src1_isrc2,
Instance_st231_cmpeq_idest_src1_xsrc2,
Instance_st231_cmpeq_ibdest_src1_xsrc2,
Instance_st231_cmpge_dest_src1_src2,
Instance_st231_cmpge_bdest_src1_src2,
Instance_st231_cmpge_idest_src1_isrc2,
Instance_st231_cmpge_ibdest_src1_isrc2,
Instance_st231_cmpge_idest_src1_xsrc2,
Instance_st231_cmpge_ibdest_src1_xsrc2,
Instance_st231_cmpgeu_dest_src1_src2,
Instance_st231_cmpgeu_bdest_src1_src2,
Instance_st231_cmpgeu_idest_src1_isrc2,
Instance_st231_cmpgeu_ibdest_src1_isrc2,
Instance_st231_cmpgeu_idest_src1_xsrc2,
Instance_st231_cmpgeu_ibdest_src1_xsrc2,
Instance_st231_cmpgt_dest_src1_src2,
Instance_st231_cmpgt_bdest_src1_src2,
Instance_st231_cmpgt_idest_src1_isrc2,
Instance_st231_cmpgt_ibdest_src1_isrc2,
Instance_st231_cmpgt_idest_src1_xsrc2,
Instance_st231_cmpgt_ibdest_src1_xsrc2,
Instance_st231_cmpgtu_dest_src1_src2,
Instance_st231_cmpgtu_bdest_src1_src2,
Instance_st231_cmpgtu_idest_src1_isrc2,
Instance_st231_cmpgtu_ibdest_src1_isrc2,
Instance_st231_cmpgtu_idest_src1_xsrc2,
Instance_st231_cmpgtu_ibdest_src1_xsrc2,
Instance_st231_cmple_dest_src1_src2,
Instance_st231_cmple_bdest_src1_src2,
Instance_st231_cmple_idest_src1_isrc2,
Instance_st231_cmple_ibdest_src1_isrc2,
Instance_st231_cmple_idest_src1_xsrc2,
Instance_st231_cmple_ibdest_src1_xsrc2,
Instance_st231_cmpleu_dest_src1_src2,
Instance_st231_cmpleu_bdest_src1_src2,
Instance_st231_cmpleu_idest_src1_isrc2,
Instance_st231_cmpleu_ibdest_src1_isrc2,
Instance_st231_cmpleu_idest_src1_xsrc2,
Instance_st231_cmpleu_ibdest_src1_xsrc2,
Instance_st231_cmplt_dest_src1_src2,
Instance_st231_cmplt_bdest_src1_src2,
Instance_st231_cmplt_idest_src1_isrc2,
Instance_st231_cmplt_ibdest_src1_isrc2,
Instance_st231_cmplt_idest_src1_xsrc2,
Instance_st231_cmplt_ibdest_src1_xsrc2,
Instance_st231_cmpltu_dest_src1_src2,
Instance_st231_cmpltu_bdest_src1_src2,
Instance_st231_cmpltu_idest_src1_isrc2,
Instance_st231_cmpltu_ibdest_src1_isrc2,
Instance_st231_cmpltu_idest_src1_xsrc2,
Instance_st231_cmpltu_ibdest_src1_xsrc2,
Instance_st231_cmpne_dest_src1_src2,
Instance_st231_cmpne_bdest_src1_src2,
Instance_st231_cmpne_idest_src1_isrc2,
Instance_st231_cmpne_ibdest_src1_isrc2,
Instance_st231_cmpne_idest_src1_xsrc2,
Instance_st231_cmpne_ibdest_src1_xsrc2,
Instance_st231_divs_dest_bdest_src1_src2_scond,
Instance_st231_goto_btarg,
Instance_st231_igoto,
Instance_st231_imml_imm,
Instance_st231_immr_imm,
Instance_st231_ldb_nlidest_isrc2_src1,
Instance_st231_ldb_nlidest_xsrc2_src1,
Instance_st231_ldb_d_nlidest_isrc2_src1,
Instance_st231_ldb_d_nlidest_xsrc2_src1,
Instance_st231_ldbu_nlidest_isrc2_src1,
Instance_st231_ldbu_nlidest_xsrc2_src1,
Instance_st231_ldbu_d_nlidest_isrc2_src1,
Instance_st231_ldbu_d_nlidest_xsrc2_src1,
Instance_st231_ldh_nlidest_isrc2_src1,
Instance_st231_ldh_nlidest_xsrc2_src1,
Instance_st231_ldh_d_nlidest_isrc2_src1,
Instance_st231_ldh_d_nlidest_xsrc2_src1,
Instance_st231_ldhu_nlidest_isrc2_src1,
Instance_st231_ldhu_nlidest_xsrc2_src1,
Instance_st231_ldhu_d_nlidest_isrc2_src1,
Instance_st231_ldhu_d_nlidest_xsrc2_src1,
Instance_st231_ldw_idest_isrc2_src1,
Instance_st231_ldw_idest_xsrc2_src1,
Instance_st231_ldw_d_idest_isrc2_src1,
Instance_st231_ldw_d_idest_xsrc2_src1,
Instance_st231_ldwl_idest_src1,
Instance_st231_max_dest_src1_src2,
Instance_st231_max_idest_src1_isrc2,
Instance_st231_max_idest_src1_xsrc2,
Instance_st231_maxu_dest_src1_src2,
Instance_st231_maxu_idest_src1_isrc2,
Instance_st231_maxu_idest_src1_xsrc2,
Instance_st231_min_dest_src1_src2,
Instance_st231_min_idest_src1_isrc2,
Instance_st231_min_idest_src1_xsrc2,
Instance_st231_minu_dest_src1_src2,
Instance_st231_minu_idest_src1_isrc2,
Instance_st231_minu_idest_src1_xsrc2,
Instance_st231_mulh_nldest_src1_src2,
Instance_st231_mulh_nlidest_src1_isrc2,
Instance_st231_mulh_nlidest_src1_xsrc2,
Instance_st231_mulhh_nldest_src1_src2,
Instance_st231_mulhh_nlidest_src1_isrc2,
Instance_st231_mulhh_nlidest_src1_xsrc2,
Instance_st231_mulhhs_nldest_src1_src2,
Instance_st231_mulhhs_nlidest_src1_isrc2,
Instance_st231_mulhhs_nlidest_src1_xsrc2,
Instance_st231_mulhhu_nldest_src1_src2,
Instance_st231_mulhhu_nlidest_src1_isrc2,
Instance_st231_mulhhu_nlidest_src1_xsrc2,
Instance_st231_mulhs_nldest_src1_src2,
Instance_st231_mulhs_nlidest_src1_isrc2,
Instance_st231_mulhs_nlidest_src1_xsrc2,
Instance_st231_mulhu_nldest_src1_src2,
Instance_st231_mulhu_nlidest_src1_isrc2,
Instance_st231_mulhu_nlidest_src1_xsrc2,
Instance_st231_mull_nldest_src1_src2,
Instance_st231_mull_nlidest_src1_isrc2,
Instance_st231_mull_nlidest_src1_xsrc2,
Instance_st231_mullh_nldest_src1_src2,
Instance_st231_mullh_nlidest_src1_isrc2,
Instance_st231_mullh_nlidest_src1_xsrc2,
Instance_st231_mullhu_nldest_src1_src2,
Instance_st231_mullhu_nlidest_src1_isrc2,
Instance_st231_mullhu_nlidest_src1_xsrc2,
Instance_st231_mullhus_nldest_src1_src2,
Instance_st231_mullhus_nlidest_src1_isrc2,
Instance_st231_mullhus_nlidest_src1_xsrc2,
Instance_st231_mulll_nldest_src1_src2,
Instance_st231_mulll_nlidest_src1_isrc2,
Instance_st231_mulll_nlidest_src1_xsrc2,
Instance_st231_mulllu_nldest_src1_src2,
Instance_st231_mulllu_nlidest_src1_isrc2,
Instance_st231_mulllu_nlidest_src1_xsrc2,
Instance_st231_mullu_nldest_src1_src2,
Instance_st231_mullu_nlidest_src1_isrc2,
Instance_st231_mullu_nlidest_src1_xsrc2,
Instance_st231_mul32_nldest_src1_src2,
Instance_st231_mul32_nlidest_src1_isrc2,
Instance_st231_mul32_nlidest_src1_xsrc2,
Instance_st231_mul64h_nldest_src1_src2,
Instance_st231_mul64h_nlidest_src1_isrc2,
Instance_st231_mul64h_nlidest_src1_xsrc2,
Instance_st231_mul64hu_nldest_src1_src2,
Instance_st231_mul64hu_nlidest_src1_isrc2,
Instance_st231_mul64hu_nlidest_src1_xsrc2,
Instance_st231_mulfrac_nldest_src1_src2,
Instance_st231_mulfrac_nlidest_src1_isrc2,
Instance_st231_mulfrac_nlidest_src1_xsrc2,
Instance_st231_nandl_dest_src1_src2,
Instance_st231_nandl_bdest_src1_src2,
Instance_st231_nandl_idest_src1_isrc2,
Instance_st231_nandl_ibdest_src1_isrc2,
Instance_st231_nandl_idest_src1_xsrc2,
Instance_st231_nandl_ibdest_src1_xsrc2,
Instance_st231_norl_dest_src1_src2,
Instance_st231_norl_bdest_src1_src2,
Instance_st231_norl_idest_src1_isrc2,
Instance_st231_norl_ibdest_src1_isrc2,
Instance_st231_norl_idest_src1_xsrc2,
Instance_st231_norl_ibdest_src1_xsrc2,
Instance_st231_or_dest_src1_src2,
Instance_st231_or_idest_src1_isrc2,
Instance_st231_or_idest_src1_xsrc2,
Instance_st231_orc_dest_src1_src2,
Instance_st231_orc_idest_src1_isrc2,
Instance_st231_orc_idest_src1_xsrc2,
Instance_st231_orl_dest_src1_src2,
Instance_st231_orl_bdest_src1_src2,
Instance_st231_orl_idest_src1_isrc2,
Instance_st231_orl_ibdest_src1_isrc2,
Instance_st231_orl_idest_src1_xsrc2,
Instance_st231_orl_ibdest_src1_xsrc2,
Instance_st231_pft_isrc2_src1,
Instance_st231_pft_xsrc2_src1,
Instance_st231_prgadd_isrc2_src1,
Instance_st231_prgadd_xsrc2_src1,
Instance_st231_prgins,
Instance_st231_prginspg_isrc2_src1,
Instance_st231_prginspg_xsrc2_src1,
Instance_st231_prgset_isrc2_src1,
Instance_st231_prgset_xsrc2_src1,
Instance_st231_pswclr_src2,
Instance_st231_pswset_src2,
Instance_st231_rfi,
Instance_st231_sbrk_sbrknum,
Instance_st231_sh1add_dest_src1_src2,
Instance_st231_sh1add_idest_src1_isrc2,
Instance_st231_sh1add_idest_src1_xsrc2,
Instance_st231_sh2add_dest_src1_src2,
Instance_st231_sh2add_idest_src1_isrc2,
Instance_st231_sh2add_idest_src1_xsrc2,
Instance_st231_sh3add_dest_src1_src2,
Instance_st231_sh3add_idest_src1_isrc2,
Instance_st231_sh3add_idest_src1_xsrc2,
Instance_st231_sh4add_dest_src1_src2,
Instance_st231_sh4add_idest_src1_isrc2,
Instance_st231_sh4add_idest_src1_xsrc2,
Instance_st231_shl_dest_src1_src2,
Instance_st231_shl_idest_src1_isrc2,
Instance_st231_shl_idest_src1_xsrc2,
Instance_st231_shr_dest_src1_src2,
Instance_st231_shr_idest_src1_isrc2,
Instance_st231_shr_idest_src1_xsrc2,
Instance_st231_shru_dest_src1_src2,
Instance_st231_shru_idest_src1_isrc2,
Instance_st231_shru_idest_src1_xsrc2,
Instance_st231_slct_dest_scond_src1_src2,
Instance_st231_slct_idest_scond_src1_isrc2,
Instance_st231_slct_idest_scond_src1_xsrc2,
Instance_st231_slctf_dest_scond_src1_src2,
Instance_st231_slctf_idest_scond_src1_isrc2,
Instance_st231_slctf_idest_scond_src1_xsrc2,
Instance_st231_stb_isrc2_src1_src2,
Instance_st231_stb_xsrc2_src1_src2,
Instance_st231_sth_isrc2_src1_src2,
Instance_st231_sth_xsrc2_src1_src2,
Instance_st231_stw_isrc2_src1_src2,
Instance_st231_stw_xsrc2_src1_src2,
Instance_st231_stwl_bdest2_src1_src2,
Instance_st231_sub_dest_src2_src1,
Instance_st231_sub_idest_isrc2_src1,
Instance_st231_sub_idest_xsrc2_src1,
Instance_st231_sxtb_idest_src1,
Instance_st231_sxth_idest_src1,
Instance_st231_sync,
Instance_st231_syscall_sbrknum,
Instance_st231_wmb,
Instance_st231_xor_dest_src1_src2,
Instance_st231_xor_idest_src1_isrc2,
Instance_st231_xor_idest_src1_xsrc2,
Instance_st231_zxth_idest_src1,
Instance_st231_convbi_idest_scond,
Instance_st231_convib_bdest_src1,
Instance_st231_mov_bsrc_idest_scond,
Instance_st231_mov_bdest_bdest_src1,
Instance_st231_nop,
Instance_st231_mov_dest_src2,
Instance_st231_mov_idest_isrc2,
Instance_st231_mov_idest_xsrc2,
Instance_st231_mtb_bdest_src1,
Instance_st231_mfb_idest_scond,
Instance_st231_zxtb_idest_src1,
Instance_st231_syncins,
Instance_st231_idle,
Instance_st231_return,
Instance_st240_add_ph_dest_src1_src2,
Instance_st240_adds_ph_dest_src1_src2,
Instance_st240_abss_ph_dest_src1,
Instance_st240_max_ph_dest_src1_src2,
Instance_st240_min_ph_dest_src1_src2,
Instance_st240_mul_ph_nldest_src1_src2,
Instance_st240_muladd_ph_nldest_src1_src2,
Instance_st240_mulfracadds_ph_nldest_src1_src2,
Instance_st240_mulfracrm_ph_nldest_src1_src2,
Instance_st240_mulfracrne_ph_nldest_src1_src2,
Instance_st240_shl_ph_dest_src1_src2,
Instance_st240_shl_ph_idest_src1_isrc2,
Instance_st240_shl_ph_idest_src1_xsrc2,
Instance_st240_shls_ph_nldest_src1_src2,
Instance_st240_shls_ph_nlidest_src1_isrc2,
Instance_st240_shls_ph_nlidest_src1_xsrc2,
Instance_st240_shr_ph_dest_src1_src2,
Instance_st240_shr_ph_idest_src1_isrc2,
Instance_st240_shr_ph_idest_src1_xsrc2,
Instance_st240_shrrnp_ph_nldest_src1_src2,
Instance_st240_shrrnp_ph_nlidest_src1_isrc2,
Instance_st240_shrrnp_ph_nlidest_src1_xsrc2,
Instance_st240_shrrne_ph_nldest_src1_src2,
Instance_st240_shrrne_ph_nlidest_src1_isrc2,
Instance_st240_shrrne_ph_nlidest_src1_xsrc2,
Instance_st240_sub_ph_dest_src2_src1,
Instance_st240_subs_ph_dest_src2_src1,
Instance_st240_shuff_pbh_dest_src1_src2,
Instance_st240_shuff_pbl_dest_src1_src2,
Instance_st240_shuff_phh_dest_src1_src2,
Instance_st240_shuff_phl_dest_src1_src2,
Instance_st240_shuffodd_pb_dest_src1_src2,
Instance_st240_shuffeve_pb_dest_src1_src2,
Instance_st240_perm_pb_dest_src1_src2,
Instance_st240_perm_pb_idest_src1_isrc2,
Instance_st240_perm_pb_idest_src1_xsrc2,
Instance_st240_ext1_pb_dest_src1_src2,
Instance_st240_ext2_pb_dest_src1_src2,
Instance_st240_ext3_pb_dest_src1_src2,
Instance_st240_packsu_pb_dest_src1_src2,
Instance_st240_pack_pb_dest_src1_src2,
Instance_st240_packrnp_phh_dest_src1_src2,
Instance_st240_packs_ph_dest_src1_src2,
Instance_st240_cmpeq_ph_dest_src1_src2,
Instance_st240_cmpgt_ph_dest_src1_src2,
Instance_st240_sadu_pb_nldest_src1_src2,
Instance_st240_absubu_pb_dest_src1_src2,
Instance_st240_muladdus_pb_nldest_src1_src2,
Instance_st240_avg4u_pb_nldest_scond_src1_src2,
Instance_st240_avgu_pb_dest_scond_src1_src2,
Instance_st240_cmpeq_pb_dest_src1_src2,
Instance_st240_cmpgtu_pb_dest_src1_src2,
Instance_st240_slct_pb_dest_scond_src1_src2,
Instance_st240_slct_pb_idest_scond_src1_isrc2,
Instance_st240_slct_pb_idest_scond_src1_xsrc2,
Instance_st240_slctf_pb_idest_scond_src1_isrc2,
Instance_st240_slctf_pb_idest_scond_src1_xsrc2,
Instance_st240_cmpeq_ph_bdest_bdest2_src1_src2,
Instance_st240_cmpeq_pb_bdest_bdest2_src1_src2,
Instance_st240_cmpgt_ph_bdest_bdest2_src1_src2,
Instance_st240_cmpgtu_pb_bdest_bdest2_src1_src2,
Instance_st240_mov_bdest_bdest2_src1,
Instance_st240_mov_bsrc_dest_scond,
Instance_st240_extl_pb_dest_scond_src1_src2,
Instance_st240_extr_pb_dest_scond_src1_src2,
Instance_st240_add_dest_src1_src2,
Instance_st240_add_idest_src1_isrc2,
Instance_st240_add_idest_src1_xsrc2,
Instance_st240_addcg_dest_bdest_src1_src2_scond,
Instance_st240_addf_n_nldest_src1_src2,
Instance_st240_addpc_idest_isrc2,
Instance_st240_addpc_idest_xsrc2,
Instance_st240_adds_dest_src1_src2,
Instance_st240_addso_dest_src1_src2,
Instance_st240_and_dest_src1_src2,
Instance_st240_and_idest_src1_isrc2,
Instance_st240_and_idest_src1_xsrc2,
Instance_st240_andl_bdest_bdest2_bsrc1_bsrc2,
Instance_st240_andc_dest_src1_src2,
Instance_st240_andc_idest_src1_isrc2,
Instance_st240_andc_idest_src1_xsrc2,
Instance_st240_andl_dest_src1_src2,
Instance_st240_andl_bdest2_src1_src2,
Instance_st240_asm_0_dest_src1_src2,
Instance_st240_asm_1_dest_src1_src2,
Instance_st240_asm_2_dest_src1_src2,
Instance_st240_asm_3_dest_src1_src2,
Instance_st240_asm_4_dest_src1_src2,
Instance_st240_asm_5_dest_src1_src2,
Instance_st240_asm_6_dest_src1_src2,
Instance_st240_asm_7_dest_src1_src2,
Instance_st240_asm_8_dest_src1_src2,
Instance_st240_asm_9_dest_src1_src2,
Instance_st240_asm_10_dest_src1_src2,
Instance_st240_asm_11_dest_src1_src2,
Instance_st240_asm_12_dest_src1_src2,
Instance_st240_asm_13_dest_src1_src2,
Instance_st240_asm_14_dest_src1_src2,
Instance_st240_asm_15_dest_src1_src2,
Instance_st240_asm_16_idest_src1_isrc2,
Instance_st240_asm_16_idest_src1_xsrc2,
Instance_st240_asm_17_idest_src1_isrc2,
Instance_st240_asm_17_idest_src1_xsrc2,
Instance_st240_asm_18_idest_src1_isrc2,
Instance_st240_asm_18_idest_src1_xsrc2,
Instance_st240_asm_19_idest_src1_isrc2,
Instance_st240_asm_19_idest_src1_xsrc2,
Instance_st240_asm_20_idest_src1_isrc2,
Instance_st240_asm_20_idest_src1_xsrc2,
Instance_st240_asm_21_idest_src1_isrc2,
Instance_st240_asm_21_idest_src1_xsrc2,
Instance_st240_asm_22_idest_src1_isrc2,
Instance_st240_asm_22_idest_src1_xsrc2,
Instance_st240_asm_23_idest_src1_isrc2,
Instance_st240_asm_23_idest_src1_xsrc2,
Instance_st240_asm_24_idest_src1_isrc2,
Instance_st240_asm_24_idest_src1_xsrc2,
Instance_st240_asm_25_idest_src1_isrc2,
Instance_st240_asm_25_idest_src1_xsrc2,
Instance_st240_asm_26_idest_src1_isrc2,
Instance_st240_asm_26_idest_src1_xsrc2,
Instance_st240_asm_27_idest_src1_isrc2,
Instance_st240_asm_27_idest_src1_xsrc2,
Instance_st240_asm_28_idest_src1_isrc2,
Instance_st240_asm_28_idest_src1_xsrc2,
Instance_st240_asm_29_idest_src1_isrc2,
Instance_st240_asm_29_idest_src1_xsrc2,
Instance_st240_asm_30_idest_src1_isrc2,
Instance_st240_asm_30_idest_src1_xsrc2,
Instance_st240_asm_31_idest_src1_isrc2,
Instance_st240_asm_31_idest_src1_xsrc2,
Instance_st240_br_bcond_btarg,
Instance_st240_break,
Instance_st240_brf_bcond_btarg,
Instance_st240_call_btarg,
Instance_st240_icall,
Instance_st240_clz_idest_src1,
Instance_st240_cmpeq_dest_src1_src2,
Instance_st240_cmpeq_idest_src1_isrc2,
Instance_st240_cmpeq_bdest2_src1_src2,
Instance_st240_cmpeq_ibdest_src1_isrc2,
Instance_st240_cmpeq_idest_src1_xsrc2,
Instance_st240_cmpeq_ibdest_src1_xsrc2,
Instance_st240_cmpeqf_n_dest_src1_src2,
Instance_st240_cmpeqf_n_bdest2_src1_src2,
Instance_st240_cmpge_idest_src1_isrc2,
Instance_st240_cmpge_ibdest_src1_isrc2,
Instance_st240_cmpge_idest_src1_xsrc2,
Instance_st240_cmpge_ibdest_src1_xsrc2,
Instance_st240_cmpgef_n_dest_src1_src2,
Instance_st240_cmpgef_n_bdest2_src1_src2,
Instance_st240_cmpgeu_idest_src1_isrc2,
Instance_st240_cmpgeu_ibdest_src1_isrc2,
Instance_st240_cmpgeu_idest_src1_xsrc2,
Instance_st240_cmpgeu_ibdest_src1_xsrc2,
Instance_st240_cmpgt_idest_src1_isrc2,
Instance_st240_cmpgt_ibdest_src1_isrc2,
Instance_st240_cmpgt_idest_src1_xsrc2,
Instance_st240_cmpgt_ibdest_src1_xsrc2,
Instance_st240_cmpgtf_n_dest_src1_src2,
Instance_st240_cmpgtf_n_bdest2_src1_src2,
Instance_st240_cmpgtu_idest_src1_isrc2,
Instance_st240_cmpgtu_ibdest_src1_isrc2,
Instance_st240_cmpgtu_idest_src1_xsrc2,
Instance_st240_cmpgtu_ibdest_src1_xsrc2,
Instance_st240_cmple_dest_src1_src2,
Instance_st240_cmple_idest_src1_isrc2,
Instance_st240_cmple_bdest2_src1_src2,
Instance_st240_cmple_ibdest_src1_isrc2,
Instance_st240_cmple_idest_src1_xsrc2,
Instance_st240_cmple_ibdest_src1_xsrc2,
Instance_st240_cmpleu_dest_src1_src2,
Instance_st240_cmpleu_idest_src1_isrc2,
Instance_st240_cmpleu_bdest2_src1_src2,
Instance_st240_cmpleu_ibdest_src1_isrc2,
Instance_st240_cmpleu_idest_src1_xsrc2,
Instance_st240_cmpleu_ibdest_src1_xsrc2,
Instance_st240_cmplt_dest_src1_src2,
Instance_st240_cmplt_idest_src1_isrc2,
Instance_st240_cmplt_bdest2_src1_src2,
Instance_st240_cmplt_ibdest_src1_isrc2,
Instance_st240_cmplt_idest_src1_xsrc2,
Instance_st240_cmplt_ibdest_src1_xsrc2,
Instance_st240_cmpltu_dest_src1_src2,
Instance_st240_cmpltu_idest_src1_isrc2,
Instance_st240_cmpltu_bdest2_src1_src2,
Instance_st240_cmpltu_ibdest_src1_isrc2,
Instance_st240_cmpltu_idest_src1_xsrc2,
Instance_st240_cmpltu_ibdest_src1_xsrc2,
Instance_st240_cmpne_dest_src1_src2,
Instance_st240_cmpne_idest_src1_isrc2,
Instance_st240_cmpne_bdest2_src1_src2,
Instance_st240_cmpne_ibdest_src1_isrc2,
Instance_st240_cmpne_idest_src1_xsrc2,
Instance_st240_cmpne_ibdest_src1_xsrc2,
Instance_st240_convfi_n_nldest_src1,
Instance_st240_convif_n_nldest_src1,
Instance_st240_dbgsbrk_brknum,
Instance_st240_dib,
Instance_st240_div_nldest_src1_src2,
Instance_st240_divu_nldest_src1_src2,
Instance_st240_extract_idest_src1_isrc2,
Instance_st240_extract_idest_src1_xsrc2,
Instance_st240_extractu_idest_src1_isrc2,
Instance_st240_extractu_idest_src1_xsrc2,
Instance_st240_extractl_idest_src1_isrc2,
Instance_st240_extractl_idest_src1_xsrc2,
Instance_st240_extractlu_idest_src1_isrc2,
Instance_st240_extractlu_idest_src1_xsrc2,
Instance_st240_flushadd_isrc2_src1,
Instance_st240_flushadd_xsrc2_src1,
Instance_st240_flushadd_l1_isrc2_src1,
Instance_st240_flushadd_l1_xsrc2_src1,
Instance_st240_goto_btarg,
Instance_st240_igoto,
Instance_st240_imml_imm,
Instance_st240_immr_imm,
Instance_st240_invadd_isrc2_src1,
Instance_st240_invadd_xsrc2_src1,
Instance_st240_invadd_l1_isrc2_src1,
Instance_st240_invadd_l1_xsrc2_src1,
Instance_st240_ldb_nlidest_isrc2_src1,
Instance_st240_ldb_nlidest_xsrc2_src1,
Instance_st240_ldbc_nlidest_pcond_isrc2_src1,
Instance_st240_ldbc_nlidest_pcond_xsrc2_src1,
Instance_st240_ldbu_nlidest_isrc2_src1,
Instance_st240_ldbu_nlidest_xsrc2_src1,
Instance_st240_ldbuc_nlidest_pcond_isrc2_src1,
Instance_st240_ldbuc_nlidest_pcond_xsrc2_src1,
Instance_st240_ldh_nlidest_isrc2_src1,
Instance_st240_ldh_nlidest_xsrc2_src1,
Instance_st240_ldhc_nlidest_pcond_isrc2_src1,
Instance_st240_ldhc_nlidest_pcond_xsrc2_src1,
Instance_st240_ldhu_nlidest_isrc2_src1,
Instance_st240_ldhu_nlidest_xsrc2_src1,
Instance_st240_ldhuc_nlidest_pcond_isrc2_src1,
Instance_st240_ldhuc_nlidest_pcond_xsrc2_src1,
Instance_st240_ldl_idestp_isrc2_src1,
Instance_st240_ldl_idestp_xsrc2_src1,
Instance_st240_ldlc_idestp_pcond_isrc2_src1,
Instance_st240_ldlc_idestp_pcond_xsrc2_src1,
Instance_st240_ldw_idest_isrc2_src1,
Instance_st240_ldw_idest_xsrc2_src1,
Instance_st240_ldwc_idest_pcond_isrc2_src1,
Instance_st240_ldwc_idest_pcond_xsrc2_src1,
Instance_st240_ldwl_idest_src1,
Instance_st240_max_dest_src1_src2,
Instance_st240_max_idest_src1_isrc2,
Instance_st240_max_idest_src1_xsrc2,
Instance_st240_maxu_dest_src1_src2,
Instance_st240_maxu_idest_src1_isrc2,
Instance_st240_maxu_idest_src1_xsrc2,
Instance_st240_min_dest_src1_src2,
Instance_st240_min_idest_src1_isrc2,
Instance_st240_min_idest_src1_xsrc2,
Instance_st240_minu_dest_src1_src2,
Instance_st240_minu_idest_src1_isrc2,
Instance_st240_minu_idest_src1_xsrc2,
Instance_st240_mov_bsrc_bdest_bdest2_bsrc1,
Instance_st240_mulf_n_nldest_src1_src2,
Instance_st240_mulfrac_nldest_src1_src2,
Instance_st240_mulfrac_nlidest_src1_isrc2,
Instance_st240_mulfrac_nlidest_src1_xsrc2,
Instance_st240_mulh_nldest_src1_src2,
Instance_st240_mulhh_nldest_src1_src2,
Instance_st240_mulhhu_nldest_src1_src2,
Instance_st240_mull_nldest_src1_src2,
Instance_st240_mullh_nldest_src1_src2,
Instance_st240_mullhu_nldest_src1_src2,
Instance_st240_mulll_nldest_src1_src2,
Instance_st240_mulll_nlidest_src1_isrc2,
Instance_st240_mulll_nlidest_src1_xsrc2,
Instance_st240_mulllu_nldest_src1_src2,
Instance_st240_mulllu_nlidest_src1_isrc2,
Instance_st240_mulllu_nlidest_src1_xsrc2,
Instance_st240_mul32_nldest_src1_src2,
Instance_st240_mul32_nlidest_src1_isrc2,
Instance_st240_mul32_nlidest_src1_xsrc2,
Instance_st240_mul64h_nldest_src1_src2,
Instance_st240_mul64h_nlidest_src1_isrc2,
Instance_st240_mul64h_nlidest_src1_xsrc2,
Instance_st240_mul64hu_nldest_src1_src2,
Instance_st240_mul64hu_nlidest_src1_isrc2,
Instance_st240_mul64hu_nlidest_src1_xsrc2,
Instance_st240_nandl_bdest_bdest2_bsrc1_bsrc2,
Instance_st240_nandl_dest_src1_src2,
Instance_st240_nandl_bdest2_src1_src2,
Instance_st240_norl_bdest_bdest2_bsrc1_bsrc2,
Instance_st240_norl_dest_src1_src2,
Instance_st240_norl_bdest2_src1_src2,
Instance_st240_or_dest_src1_src2,
Instance_st240_or_idest_src1_isrc2,
Instance_st240_or_idest_src1_xsrc2,
Instance_st240_orl_bdest_bdest2_bsrc1_bsrc2,
Instance_st240_orc_dest_src1_src2,
Instance_st240_orl_dest_src1_src2,
Instance_st240_orl_bdest2_src1_src2,
Instance_st240_pft_isrc2_src1,
Instance_st240_pft_xsrc2_src1,
Instance_st240_pftc_pcond_isrc2_src1,
Instance_st240_pftc_pcond_xsrc2_src1,
Instance_st240_prgadd_isrc2_src1,
Instance_st240_prgadd_xsrc2_src1,
Instance_st240_prgadd_l1_isrc2_src1,
Instance_st240_prgadd_l1_xsrc2_src1,
Instance_st240_prginsadd_isrc2_src1,
Instance_st240_prginsadd_xsrc2_src1,
Instance_st240_prginsadd_l1_isrc2_src1,
Instance_st240_prginsadd_l1_xsrc2_src1,
Instance_st240_prginsset_isrc2_src1,
Instance_st240_prginsset_xsrc2_src1,
Instance_st240_prginsset_l1_isrc2_src1,
Instance_st240_prginsset_l1_xsrc2_src1,
Instance_st240_prgset_isrc2_src1,
Instance_st240_prgset_xsrc2_src1,
Instance_st240_prgset_l1_isrc2_src1,
Instance_st240_prgset_l1_xsrc2_src1,
Instance_st240_pswmask_nlidest_src1_isrc2,
Instance_st240_pswmask_nlidest_src1_xsrc2,
Instance_st240_rem_nldest_src1_src2,
Instance_st240_remu_nldest_src1_src2,
Instance_st240_retention,
Instance_st240_return,
Instance_st240_rfi,
Instance_st240_rotl_dest_src1_src2,
Instance_st240_rotl_idest_src1_isrc2,
Instance_st240_rotl_idest_src1_xsrc2,
Instance_st240_sats_dest_src1,
Instance_st240_satso_dest_src1,
Instance_st240_sbrk_brknum,
Instance_st240_sh1add_dest_src1_src2,
Instance_st240_sh1add_idest_src1_isrc2,
Instance_st240_sh1add_idest_src1_xsrc2,
Instance_st240_sh1adds_dest_src2_src1,
Instance_st240_sh1addso_dest_src2_src1,
Instance_st240_sh1subs_dest_src2_src1,
Instance_st240_sh1subso_dest_src2_src1,
Instance_st240_sh2add_dest_src1_src2,
Instance_st240_sh2add_idest_src1_isrc2,
Instance_st240_sh2add_idest_src1_xsrc2,
Instance_st240_sh3add_dest_src1_src2,
Instance_st240_sh3add_idest_src1_isrc2,
Instance_st240_sh3add_idest_src1_xsrc2,
Instance_st240_shl_dest_src1_src2,
Instance_st240_shl_idest_src1_isrc2,
Instance_st240_shl_idest_src1_xsrc2,
Instance_st240_shls_nldest_src1_src2,
Instance_st240_shls_nlidest_src1_isrc2,
Instance_st240_shls_nlidest_src1_xsrc2,
Instance_st240_shlso_nldest_src1_src2,
Instance_st240_shlso_nlidest_src1_isrc2,
Instance_st240_shlso_nlidest_src1_xsrc2,
Instance_st240_shr_dest_src1_src2,
Instance_st240_shr_idest_src1_isrc2,
Instance_st240_shr_idest_src1_xsrc2,
Instance_st240_shrrnp_nlidest_src1_isrc2,
Instance_st240_shrrnp_nlidest_src1_xsrc2,
Instance_st240_shru_dest_src1_src2,
Instance_st240_shru_idest_src1_isrc2,
Instance_st240_shru_idest_src1_xsrc2,
Instance_st240_slct_rsrc_dest_scond_src1_src2,
Instance_st240_slct_idest_scond_src1_isrc2,
Instance_st240_slct_idest_scond_src1_xsrc2,
Instance_st240_slctf_idest_scond_src1_isrc2,
Instance_st240_slctf_idest_scond_src1_xsrc2,
Instance_st240_stb_isrc2_src1_src2,
Instance_st240_stb_xsrc2_src1_src2,
Instance_st240_stbc_isrc2_src1_pcond_src2,
Instance_st240_stbc_xsrc2_src1_pcond_src2,
Instance_st240_sth_isrc2_src1_src2,
Instance_st240_sth_xsrc2_src1_src2,
Instance_st240_sthc_isrc2_src1_pcond_src2,
Instance_st240_sthc_xsrc2_src1_pcond_src2,
Instance_st240_stl_isrc2_src1_src2p,
Instance_st240_stl_xsrc2_src1_src2p,
Instance_st240_stlc_isrc2_src1_pcond_src2p,
Instance_st240_stlc_xsrc2_src1_pcond_src2p,
Instance_st240_stw_isrc2_src1_src2,
Instance_st240_stw_xsrc2_src1_src2,
Instance_st240_stwc_isrc2_src1_pcond_src2,
Instance_st240_stwc_xsrc2_src1_pcond_src2,
Instance_st240_stwl_bdest2_src1_src2,
Instance_st240_sub_dest_src2_src1,
Instance_st240_sub_idest_isrc2_src1,
Instance_st240_sub_idest_xsrc2_src1,
Instance_st240_subf_n_nldest_src1_src2,
Instance_st240_subs_dest_src2_src1,
Instance_st240_subso_dest_src2_src1,
Instance_st240_sxt_dest_src1_src2,
Instance_st240_sxt_idest_src1_isrc2,
Instance_st240_sxt_idest_src1_xsrc2,
Instance_st240_sync,
Instance_st240_syncins,
Instance_st240_syscall_brknum,
Instance_st240_waitl,
Instance_st240_wmb,
Instance_st240_xor_dest_src1_src2,
Instance_st240_xor_idest_src1_isrc2,
Instance_st240_xor_idest_src1_xsrc2,
Instance_st240_zxt_dest_src1_src2,
Instance_st240_zxt_idest_src1_isrc2,
Instance_st240_zxt_idest_src1_xsrc2,
Instance_st240_bswap_idest_src1,
Instance_st240_convbi_idest_scond,
Instance_st240_convib_bdest2_src1,
Instance_st240_cmpge_dest_src2_src1,
Instance_st240_cmpge_bdest2_src2_src1,
Instance_st240_cmpgeu_dest_src2_src1,
Instance_st240_cmpgeu_bdest2_src2_src1,
Instance_st240_cmpgt_dest_src2_src1,
Instance_st240_cmpgt_bdest2_src2_src1,
Instance_st240_cmpgtu_dest_src2_src1,
Instance_st240_cmpgtu_bdest2_src2_src1,
Instance_st240_cmplef_n_dest_src2_src1,
Instance_st240_cmplef_n_bdest2_src2_src1,
Instance_st240_cmpltf_n_dest_src2_src1,
Instance_st240_cmpltf_n_bdest2_src2_src1,
Instance_st240_idle,
Instance_st240_mfb_idest_scond,
Instance_st240_mov_dest_src2,
Instance_st240_mov_idest_isrc2,
Instance_st240_mov_idest_xsrc2,
Instance_st240_mtb_bdest2_src1,
Instance_st240_mull_nlidest_src1_isrc2,
Instance_st240_mull_nlidest_src1_xsrc2,
Instance_st240_nop,
Instance_st240_slctf_dest_scond_src2_src1,
Instance_st240_sxtb_idest_src1,
Instance_st240_sxth_idest_src1,
Instance_st240_zxtb_idest_src1,
Instance_st240_zxth_idest_src1,
Instance_st240_slctf_pb_dest_scond_src2_src1,
Instance_st240_unpacku_pbh_dest_src1,
Instance_st240_unpacku_pbl_dest_src1,
Instance_st240_pack_ph_dest_src1_src2,
Instance_st220_ADJUST,
Instance_st220_GETPC,
Instance_st220_GNUASM,
Instance_st220_PUSHREGS,
Instance_st231_ADJUST,
Instance_st231_GETPC,
Instance_st231_GNUASM,
Instance_st231_PUSHREGS,
Instance_st240_ADJUST,
Instance_st240_GETPC,
Instance_st240_GNUASM,
Instance_st240_PUSHREGS,
Instance_st240_MOVP,
Instance_st240_COMPOSEP,
Instance_st240_EXTRACTP,
Instance_st240_MOVC,
Instance_st240_MOVCF,

#undef Instance


#define InstanceActions(INDEX,ACTIONS) 
#undef InstanceActions


#define InstanceAction_COUNT_MAX 5


  Instance__
#undef Instance
} enum_Instance;

/*
 * Instance_INIT --	Initialize the Instance module.
 */
void
Instance_INIT(void);

/*
 * Instance_FINI --	Finalize the Instance module.
 */
#define Instance_FINI() 






//
//
typedef enum {
#define Operator(NAME,ATTRIBUTES,INSTANCES,MNEMONIC,PARAMETERARRAY) Operator_ ##NAME,

  Operator_,

/*
  st200/Operator.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Operator__ENTRY,


Operator__LABEL,


Operator__FALL,


Operator__LOOP,


Operator__GOTO,


Operator__GOTRUE,


Operator__GOFALSE,


Operator__JUMP,


Operator__CALL,


Operator__LINK,


Operator__RETURN,


Operator__PHI,


Operator__PSI,


Operator__SIGMA,


Operator__KILL,


Operator__PCOPY,


Operator__ECOPY,


Operator__LCOPY,


Operator__COPY,


Operator__SKIP,


Operator__LNOT,


Operator__BNOT,


Operator__NEG,


Operator__ADD,


Operator__SUB,


Operator__MUL,


Operator__DIV,


Operator__REM,


Operator__SHL,


Operator__ASHR,


Operator__LSHR,


Operator__LAND,


Operator__LIOR,


Operator__LXOR,


Operator__BAND,


Operator__BIOR,


Operator__BXOR,


Operator__MIN,


Operator__MAX,


Operator__SELECT,


Operator__APPLY,


Operator__FWDBAR,


Operator__BWDBAR,


Operator__IFIXUP,


Operator__BEGIN_PREGTN,


Operator__END_PREGTN,


Operator_st200_ADJUST,


Operator_st200_GETPC,


Operator_st200_GNUASM,


Operator_st200_PUSHREGS,


Operator_st200_add_1general_2general_3general,


Operator_st200_add_1general_2general_3isrc2,


Operator_st200_add_1general_2general_3xsrc2,


Operator_st200_addcg_1general_2branch_3general_4general_5branch,


Operator_st200_and_1general_2general_3general,


Operator_st200_and_1general_2general_3isrc2,


Operator_st200_and_1general_2general_3xsrc2,


Operator_st200_andc_1general_2general_3general,


Operator_st200_andc_1general_2general_3isrc2,


Operator_st200_andc_1general_2general_3xsrc2,


Operator_st200_andl_1general_2general_3general,


Operator_st200_andl_1branch_2general_3general,


Operator_st200_andl_1general_2general_3isrc2,


Operator_st200_andl_1branch_2general_3isrc2,


Operator_st200_andl_1general_2general_3xsrc2,


Operator_st200_andl_1branch_2general_3xsrc2,


Operator_st200_asm_0_1general_2general_3general,


Operator_st200_asm_1_1general_2general_3general,


Operator_st200_asm_2_1general_2general_3general,


Operator_st200_asm_3_1general_2general_3general,


Operator_st200_asm_4_1general_2general_3general,


Operator_st200_asm_5_1general_2general_3general,


Operator_st200_asm_6_1general_2general_3general,


Operator_st200_asm_7_1general_2general_3general,


Operator_st200_asm_8_1general_2general_3general,


Operator_st200_asm_9_1general_2general_3general,


Operator_st200_asm_10_1general_2general_3general,


Operator_st200_asm_11_1general_2general_3general,


Operator_st200_asm_12_1general_2general_3general,


Operator_st200_asm_13_1general_2general_3general,


Operator_st200_asm_14_1general_2general_3general,


Operator_st200_asm_15_1general_2general_3general,


Operator_st200_asm_16_1general_2general_3isrc2,


Operator_st200_asm_16_1general_2general_3xsrc2,


Operator_st200_asm_17_1general_2general_3isrc2,


Operator_st200_asm_17_1general_2general_3xsrc2,


Operator_st200_asm_18_1general_2general_3isrc2,


Operator_st200_asm_18_1general_2general_3xsrc2,


Operator_st200_asm_19_1general_2general_3isrc2,


Operator_st200_asm_19_1general_2general_3xsrc2,


Operator_st200_asm_20_1general_2general_3isrc2,


Operator_st200_asm_20_1general_2general_3xsrc2,


Operator_st200_asm_21_1general_2general_3isrc2,


Operator_st200_asm_21_1general_2general_3xsrc2,


Operator_st200_asm_22_1general_2general_3isrc2,


Operator_st200_asm_22_1general_2general_3xsrc2,


Operator_st200_asm_23_1general_2general_3isrc2,


Operator_st200_asm_23_1general_2general_3xsrc2,


Operator_st200_asm_24_1general_2general_3isrc2,


Operator_st200_asm_24_1general_2general_3xsrc2,


Operator_st200_asm_25_1general_2general_3isrc2,


Operator_st200_asm_25_1general_2general_3xsrc2,


Operator_st200_asm_26_1general_2general_3isrc2,


Operator_st200_asm_26_1general_2general_3xsrc2,


Operator_st200_asm_27_1general_2general_3isrc2,


Operator_st200_asm_27_1general_2general_3xsrc2,


Operator_st200_asm_28_1general_2general_3isrc2,


Operator_st200_asm_28_1general_2general_3xsrc2,


Operator_st200_asm_29_1general_2general_3isrc2,


Operator_st200_asm_29_1general_2general_3xsrc2,


Operator_st200_asm_30_1general_2general_3isrc2,


Operator_st200_asm_30_1general_2general_3xsrc2,


Operator_st200_asm_31_1general_2general_3isrc2,


Operator_st200_asm_31_1general_2general_3xsrc2,


Operator_st200_br_1branch_2btarg,


Operator_st200_break,


Operator_st200_brf_1branch_2btarg,


Operator_st200_bswap_1general_2general,


Operator_st200_call_1btarg,


Operator_st200_icall,


Operator_st200_clz_1general_2general,


Operator_st200_cmpeq_1general_2general_3general,


Operator_st200_cmpeq_1branch_2general_3general,


Operator_st200_cmpeq_1general_2general_3isrc2,


Operator_st200_cmpeq_1branch_2general_3isrc2,


Operator_st200_cmpeq_1general_2general_3xsrc2,


Operator_st200_cmpeq_1branch_2general_3xsrc2,


Operator_st200_cmpge_1general_2general_3general,


Operator_st200_cmpge_1branch_2general_3general,


Operator_st200_cmpge_1general_2general_3isrc2,


Operator_st200_cmpge_1branch_2general_3isrc2,


Operator_st200_cmpge_1general_2general_3xsrc2,


Operator_st200_cmpge_1branch_2general_3xsrc2,


Operator_st200_cmpgeu_1general_2general_3general,


Operator_st200_cmpgeu_1branch_2general_3general,


Operator_st200_cmpgeu_1general_2general_3isrc2,


Operator_st200_cmpgeu_1branch_2general_3isrc2,


Operator_st200_cmpgeu_1general_2general_3xsrc2,


Operator_st200_cmpgeu_1branch_2general_3xsrc2,


Operator_st200_cmpgt_1general_2general_3general,


Operator_st200_cmpgt_1branch_2general_3general,


Operator_st200_cmpgt_1general_2general_3isrc2,


Operator_st200_cmpgt_1branch_2general_3isrc2,


Operator_st200_cmpgt_1general_2general_3xsrc2,


Operator_st200_cmpgt_1branch_2general_3xsrc2,


Operator_st200_cmpgtu_1general_2general_3general,


Operator_st200_cmpgtu_1branch_2general_3general,


Operator_st200_cmpgtu_1general_2general_3isrc2,


Operator_st200_cmpgtu_1branch_2general_3isrc2,


Operator_st200_cmpgtu_1general_2general_3xsrc2,


Operator_st200_cmpgtu_1branch_2general_3xsrc2,


Operator_st200_cmple_1general_2general_3general,


Operator_st200_cmple_1branch_2general_3general,


Operator_st200_cmple_1general_2general_3isrc2,


Operator_st200_cmple_1branch_2general_3isrc2,


Operator_st200_cmple_1general_2general_3xsrc2,


Operator_st200_cmple_1branch_2general_3xsrc2,


Operator_st200_cmpleu_1general_2general_3general,


Operator_st200_cmpleu_1branch_2general_3general,


Operator_st200_cmpleu_1general_2general_3isrc2,


Operator_st200_cmpleu_1branch_2general_3isrc2,


Operator_st200_cmpleu_1general_2general_3xsrc2,


Operator_st200_cmpleu_1branch_2general_3xsrc2,


Operator_st200_cmplt_1general_2general_3general,


Operator_st200_cmplt_1branch_2general_3general,


Operator_st200_cmplt_1general_2general_3isrc2,


Operator_st200_cmplt_1branch_2general_3isrc2,


Operator_st200_cmplt_1general_2general_3xsrc2,


Operator_st200_cmplt_1branch_2general_3xsrc2,


Operator_st200_cmpltu_1general_2general_3general,


Operator_st200_cmpltu_1branch_2general_3general,


Operator_st200_cmpltu_1general_2general_3isrc2,


Operator_st200_cmpltu_1branch_2general_3isrc2,


Operator_st200_cmpltu_1general_2general_3xsrc2,


Operator_st200_cmpltu_1branch_2general_3xsrc2,


Operator_st200_cmpne_1general_2general_3general,


Operator_st200_cmpne_1branch_2general_3general,


Operator_st200_cmpne_1general_2general_3isrc2,


Operator_st200_cmpne_1branch_2general_3isrc2,


Operator_st200_cmpne_1general_2general_3xsrc2,


Operator_st200_cmpne_1branch_2general_3xsrc2,


Operator_st200_divs_1general_2branch_3general_4general_5branch,


Operator_st200_goto_1btarg,


Operator_st200_igoto,


Operator_st200_ldb_1nolink_2isrc2_3general,


Operator_st200_ldb_1nolink_2xsrc2_3general,


Operator_st200_ldb_d_1nolink_2isrc2_3general,


Operator_st200_ldb_d_1nolink_2xsrc2_3general,


Operator_st200_ldbu_1nolink_2isrc2_3general,


Operator_st200_ldbu_1nolink_2xsrc2_3general,


Operator_st200_ldbu_d_1nolink_2isrc2_3general,


Operator_st200_ldbu_d_1nolink_2xsrc2_3general,


Operator_st200_ldh_1nolink_2isrc2_3general,


Operator_st200_ldh_1nolink_2xsrc2_3general,


Operator_st200_ldh_d_1nolink_2isrc2_3general,


Operator_st200_ldh_d_1nolink_2xsrc2_3general,


Operator_st200_ldhu_1nolink_2isrc2_3general,


Operator_st200_ldhu_1nolink_2xsrc2_3general,


Operator_st200_ldhu_d_1nolink_2isrc2_3general,


Operator_st200_ldhu_d_1nolink_2xsrc2_3general,


Operator_st200_ldw_1general_2isrc2_3general,


Operator_st200_ldw_1general_2xsrc2_3general,


Operator_st200_ldw_d_1general_2isrc2_3general,


Operator_st200_ldw_d_1general_2xsrc2_3general,


Operator_st200_max_1general_2general_3general,


Operator_st200_max_1general_2general_3isrc2,


Operator_st200_max_1general_2general_3xsrc2,


Operator_st200_maxu_1general_2general_3general,


Operator_st200_maxu_1general_2general_3isrc2,


Operator_st200_maxu_1general_2general_3xsrc2,


Operator_st200_min_1general_2general_3general,


Operator_st200_min_1general_2general_3isrc2,


Operator_st200_min_1general_2general_3xsrc2,


Operator_st200_minu_1general_2general_3general,


Operator_st200_minu_1general_2general_3isrc2,


Operator_st200_minu_1general_2general_3xsrc2,


Operator_st200_mulh_1nolink_2general_3general,


Operator_st200_mulh_1nolink_2general_3isrc2,


Operator_st200_mulh_1nolink_2general_3xsrc2,


Operator_st200_mulhh_1nolink_2general_3general,


Operator_st200_mulhh_1nolink_2general_3isrc2,


Operator_st200_mulhh_1nolink_2general_3xsrc2,


Operator_st200_mulhhs_1nolink_2general_3general,


Operator_st200_mulhhs_1nolink_2general_3isrc2,


Operator_st200_mulhhs_1nolink_2general_3xsrc2,


Operator_st200_mulhhu_1nolink_2general_3general,


Operator_st200_mulhhu_1nolink_2general_3isrc2,


Operator_st200_mulhhu_1nolink_2general_3xsrc2,


Operator_st200_mulhs_1nolink_2general_3general,


Operator_st200_mulhs_1nolink_2general_3isrc2,


Operator_st200_mulhs_1nolink_2general_3xsrc2,


Operator_st200_mulhu_1nolink_2general_3general,


Operator_st200_mulhu_1nolink_2general_3isrc2,


Operator_st200_mulhu_1nolink_2general_3xsrc2,


Operator_st200_mull_1nolink_2general_3general,


Operator_st200_mull_1nolink_2general_3isrc2,


Operator_st200_mull_1nolink_2general_3xsrc2,


Operator_st200_mullh_1nolink_2general_3general,


Operator_st200_mullh_1nolink_2general_3isrc2,


Operator_st200_mullh_1nolink_2general_3xsrc2,


Operator_st200_mullhu_1nolink_2general_3general,


Operator_st200_mullhu_1nolink_2general_3isrc2,


Operator_st200_mullhu_1nolink_2general_3xsrc2,


Operator_st200_mullhus_1nolink_2general_3general,


Operator_st200_mullhus_1nolink_2general_3isrc2,


Operator_st200_mullhus_1nolink_2general_3xsrc2,


Operator_st200_mulll_1nolink_2general_3general,


Operator_st200_mulll_1nolink_2general_3isrc2,


Operator_st200_mulll_1nolink_2general_3xsrc2,


Operator_st200_mulllu_1nolink_2general_3general,


Operator_st200_mulllu_1nolink_2general_3isrc2,


Operator_st200_mulllu_1nolink_2general_3xsrc2,


Operator_st200_mullu_1nolink_2general_3general,


Operator_st200_mullu_1nolink_2general_3isrc2,


Operator_st200_mullu_1nolink_2general_3xsrc2,


Operator_st200_nandl_1general_2general_3general,


Operator_st200_nandl_1branch_2general_3general,


Operator_st200_nandl_1general_2general_3isrc2,


Operator_st200_nandl_1branch_2general_3isrc2,


Operator_st200_nandl_1general_2general_3xsrc2,


Operator_st200_nandl_1branch_2general_3xsrc2,


Operator_st200_norl_1general_2general_3general,


Operator_st200_norl_1branch_2general_3general,


Operator_st200_norl_1general_2general_3isrc2,


Operator_st200_norl_1branch_2general_3isrc2,


Operator_st200_norl_1general_2general_3xsrc2,


Operator_st200_norl_1branch_2general_3xsrc2,


Operator_st200_or_1general_2general_3general,


Operator_st200_or_1general_2general_3isrc2,


Operator_st200_or_1general_2general_3xsrc2,


Operator_st200_orc_1general_2general_3general,


Operator_st200_orc_1general_2general_3isrc2,


Operator_st200_orc_1general_2general_3xsrc2,


Operator_st200_orl_1general_2general_3general,


Operator_st200_orl_1branch_2general_3general,


Operator_st200_orl_1general_2general_3isrc2,


Operator_st200_orl_1branch_2general_3isrc2,


Operator_st200_orl_1general_2general_3xsrc2,


Operator_st200_orl_1branch_2general_3xsrc2,


Operator_st200_pft_1isrc2_2general,


Operator_st200_pft_1xsrc2_2general,


Operator_st200_prgadd_1isrc2_2general,


Operator_st200_prgadd_1xsrc2_2general,


Operator_st200_prgset_1isrc2_2general,


Operator_st200_prgset_1xsrc2_2general,


Operator_st200_prgins,


Operator_st200_rfi,


Operator_st200_sbrk,


Operator_st200_sh1add_1general_2general_3general,


Operator_st200_sh1add_1general_2general_3isrc2,


Operator_st200_sh1add_1general_2general_3xsrc2,


Operator_st200_sh2add_1general_2general_3general,


Operator_st200_sh2add_1general_2general_3isrc2,


Operator_st200_sh2add_1general_2general_3xsrc2,


Operator_st200_sh3add_1general_2general_3general,


Operator_st200_sh3add_1general_2general_3isrc2,


Operator_st200_sh3add_1general_2general_3xsrc2,


Operator_st200_sh4add_1general_2general_3general,


Operator_st200_sh4add_1general_2general_3isrc2,


Operator_st200_sh4add_1general_2general_3xsrc2,


Operator_st200_shl_1general_2general_3general,


Operator_st200_shl_1general_2general_3isrc2,


Operator_st200_shl_1general_2general_3xsrc2,


Operator_st200_shr_1general_2general_3general,


Operator_st200_shr_1general_2general_3isrc2,


Operator_st200_shr_1general_2general_3xsrc2,


Operator_st200_shru_1general_2general_3general,


Operator_st200_shru_1general_2general_3isrc2,


Operator_st200_shru_1general_2general_3xsrc2,


Operator_st200_slct_1general_2branch_3general_4general,


Operator_st200_slct_1general_2branch_3general_4isrc2,


Operator_st200_slct_1general_2branch_3general_4xsrc2,


Operator_st200_slctf_1general_2branch_3general_4general,


Operator_st200_slctf_1general_2branch_3general_4isrc2,


Operator_st200_slctf_1general_2branch_3general_4xsrc2,


Operator_st200_stb_1isrc2_2general_3general,


Operator_st200_stb_1xsrc2_2general_3general,


Operator_st200_sth_1isrc2_2general_3general,


Operator_st200_sth_1xsrc2_2general_3general,


Operator_st200_stw_1isrc2_2general_3general,


Operator_st200_stw_1xsrc2_2general_3general,


Operator_st200_sub_1general_2general_3general,


Operator_st200_sub_1general_2isrc2_3general,


Operator_st200_sub_1general_2xsrc2_3general,


Operator_st200_sxtb_1general_2general,


Operator_st200_sxth_1general_2general,


Operator_st200_sync,


Operator_st200_syscall,


Operator_st200_xor_1general_2general_3general,


Operator_st200_xor_1general_2general_3isrc2,


Operator_st200_xor_1general_2general_3xsrc2,


Operator_st200_zxth_1general_2general,


Operator_st200_convbi_1general_2branch,


Operator_st200_convib_1branch_2general,


Operator_st200_mov_bsrc_1general_2branch,


Operator_st200_mov_bdest_1branch_2general,


Operator_st200_nop,


Operator_st200_mov_1general_2general,


Operator_st200_mov_1general_2isrc2,


Operator_st200_mov_1general_2xsrc2,


Operator_st200_mtb_1branch_2general,


Operator_st200_mfb_1general_2branch,


Operator_st200_zxtb_1general_2general,


Operator_st200_syncins,


Operator_st200_return,


Operator_st200_ldwl_1general_2general,


Operator_st200_mul32_1nolink_2general_3general,


Operator_st200_mul32_1nolink_2general_3isrc2,


Operator_st200_mul32_1nolink_2general_3xsrc2,


Operator_st200_mul64h_1nolink_2general_3general,


Operator_st200_mul64h_1nolink_2general_3isrc2,


Operator_st200_mul64h_1nolink_2general_3xsrc2,


Operator_st200_mul64hu_1nolink_2general_3general,


Operator_st200_mul64hu_1nolink_2general_3isrc2,


Operator_st200_mul64hu_1nolink_2general_3xsrc2,


Operator_st200_mulfrac_1nolink_2general_3general,


Operator_st200_mulfrac_1nolink_2general_3isrc2,


Operator_st200_mulfrac_1nolink_2general_3xsrc2,


Operator_st200_prginspg_1isrc2_2general,


Operator_st200_prginspg_1xsrc2_2general,


Operator_st200_pswclr_1general,


Operator_st200_pswset_1general,


Operator_st200_stwl_1branch_2general_3general,


Operator_st200_wmb,


Operator_st200_idle,


Operator_st200_MOVP,


Operator_st200_COMPOSEP,


Operator_st200_EXTRACTP,


Operator_st200_MOVC,


Operator_st200_MOVCF,


Operator_st200_add_ph_1general_2general_3general,


Operator_st200_adds_ph_1general_2general_3general,


Operator_st200_abss_ph_1general_2general,


Operator_st200_max_ph_1general_2general_3general,


Operator_st200_min_ph_1general_2general_3general,


Operator_st200_mul_ph_1nolink_2general_3general,


Operator_st200_muladd_ph_1nolink_2general_3general,


Operator_st200_mulfracadds_ph_1nolink_2general_3general,


Operator_st200_mulfracrm_ph_1nolink_2general_3general,


Operator_st200_mulfracrne_ph_1nolink_2general_3general,


Operator_st200_shl_ph_1general_2general_3general,


Operator_st200_shl_ph_1general_2general_3isrc2,


Operator_st200_shl_ph_1general_2general_3xsrc2,


Operator_st200_shls_ph_1nolink_2general_3general,


Operator_st200_shls_ph_1nolink_2general_3isrc2,


Operator_st200_shls_ph_1nolink_2general_3xsrc2,


Operator_st200_shr_ph_1general_2general_3general,


Operator_st200_shr_ph_1general_2general_3isrc2,


Operator_st200_shr_ph_1general_2general_3xsrc2,


Operator_st200_shrrnp_ph_1nolink_2general_3general,


Operator_st200_shrrnp_ph_1nolink_2general_3isrc2,


Operator_st200_shrrnp_ph_1nolink_2general_3xsrc2,


Operator_st200_shrrne_ph_1nolink_2general_3general,


Operator_st200_shrrne_ph_1nolink_2general_3isrc2,


Operator_st200_shrrne_ph_1nolink_2general_3xsrc2,


Operator_st200_sub_ph_1general_2general_3general,


Operator_st200_subs_ph_1general_2general_3general,


Operator_st200_shuff_pbh_1general_2general_3general,


Operator_st200_shuff_pbl_1general_2general_3general,


Operator_st200_shuff_phh_1general_2general_3general,


Operator_st200_shuff_phl_1general_2general_3general,


Operator_st200_shuffodd_pb_1general_2general_3general,


Operator_st200_shuffeve_pb_1general_2general_3general,


Operator_st200_perm_pb_1general_2general_3general,


Operator_st200_perm_pb_1general_2general_3isrc2,


Operator_st200_perm_pb_1general_2general_3xsrc2,


Operator_st200_ext1_pb_1general_2general_3general,


Operator_st200_ext2_pb_1general_2general_3general,


Operator_st200_ext3_pb_1general_2general_3general,


Operator_st200_packsu_pb_1general_2general_3general,


Operator_st200_pack_pb_1general_2general_3general,


Operator_st200_packrnp_phh_1general_2general_3general,


Operator_st200_packs_ph_1general_2general_3general,


Operator_st200_cmpeq_ph_1general_2general_3general,


Operator_st200_cmpgt_ph_1general_2general_3general,


Operator_st200_sadu_pb_1nolink_2general_3general,


Operator_st200_absubu_pb_1general_2general_3general,


Operator_st200_muladdus_pb_1nolink_2general_3general,


Operator_st200_avg4u_pb_1nolink_2branch_3general_4general,


Operator_st200_avgu_pb_1general_2branch_3general_4general,


Operator_st200_cmpeq_pb_1general_2general_3general,


Operator_st200_cmpgtu_pb_1general_2general_3general,


Operator_st200_slct_pb_1general_2branch_3general_4general,


Operator_st200_slct_pb_1general_2branch_3general_4isrc2,


Operator_st200_slct_pb_1general_2branch_3general_4xsrc2,


Operator_st200_slctf_pb_1general_2branch_3general_4isrc2,


Operator_st200_slctf_pb_1general_2branch_3general_4xsrc2,


Operator_st200_cmpeq_ph_bdest_1branch_2general_3general,


Operator_st200_cmpeq_pb_bdest_1branch_2general_3general,


Operator_st200_cmpgt_ph_bdest_1branch_2general_3general,


Operator_st200_cmpgtu_pb_bdest_1branch_2general_3general,


Operator_st240_mov_bdest_1branch_2general,


Operator_st240_mov_bsrc_1general_2branch,


Operator_st200_extl_pb_1general_2branch_3general_4general,


Operator_st200_extr_pb_1general_2branch_3general_4general,


Operator_st200_addf_n_1nolink_2general_3general,


Operator_st200_addpc_1general_2isrc2,


Operator_st200_addpc_1general_2xsrc2,


Operator_st200_adds_1general_2general_3general,


Operator_st200_addso_1general_2general_3general,


Operator_st200_andl_bdest_1branch_2branch_3branch,


Operator_st200_cmpeqf_n_1general_2general_3general,


Operator_st200_cmpeqf_n_1branch_2general_3general,


Operator_st200_cmpgef_n_1general_2general_3general,


Operator_st200_cmpgef_n_1branch_2general_3general,


Operator_st200_cmpgtf_n_1general_2general_3general,


Operator_st200_cmpgtf_n_1branch_2general_3general,


Operator_st200_convfi_n_1nolink_2general,


Operator_st200_convif_n_1nolink_2general,


Operator_st200_dbgsbrk,


Operator_st200_dib,


Operator_st200_div_1nolink_2general_3general,


Operator_st200_divu_1nolink_2general_3general,


Operator_st200_extract_1general_2general_3isrc2,


Operator_st200_extract_1general_2general_3xsrc2,


Operator_st200_extractu_1general_2general_3isrc2,


Operator_st200_extractu_1general_2general_3xsrc2,


Operator_st200_extractl_1general_2general_3isrc2,


Operator_st200_extractl_1general_2general_3xsrc2,


Operator_st200_extractlu_1general_2general_3isrc2,


Operator_st200_extractlu_1general_2general_3xsrc2,


Operator_st200_flushadd_1isrc2_2general,


Operator_st200_flushadd_1xsrc2_2general,


Operator_st200_flushadd_l1_1isrc2_2general,


Operator_st200_flushadd_l1_1xsrc2_2general,


Operator_st200_invadd_1isrc2_2general,


Operator_st200_invadd_1xsrc2_2general,


Operator_st200_invadd_l1_1isrc2_2general,


Operator_st200_invadd_l1_1xsrc2_2general,


Operator_st200_ldbc_1nolink_2predicate_3isrc2_4general,


Operator_st200_ldbc_1nolink_2predicate_3xsrc2_4general,


Operator_st200_ldbuc_1nolink_2predicate_3isrc2_4general,


Operator_st200_ldbuc_1nolink_2predicate_3xsrc2_4general,


Operator_st200_ldhc_1nolink_2predicate_3isrc2_4general,


Operator_st200_ldhc_1nolink_2predicate_3xsrc2_4general,


Operator_st200_ldhuc_1nolink_2predicate_3isrc2_4general,


Operator_st200_ldhuc_1nolink_2predicate_3xsrc2_4general,


Operator_st200_ldl_1nzpaired_2isrc2_3general,


Operator_st200_MULTI_ldl_1nzpaired_2isrc2_3general,


Operator_st200_ldl_1nzpaired_2xsrc2_3general,


Operator_st200_MULTI_ldl_1nzpaired_2xsrc2_3general,


Operator_st200_ldlc_1nzpaired_2predicate_3isrc2_4general,


Operator_st200_MULTI_ldlc_1nzpaired_2predicate_3isrc2_4general,


Operator_st200_ldlc_1nzpaired_2predicate_3xsrc2_4general,


Operator_st200_MULTI_ldlc_1nzpaired_2predicate_3xsrc2_4general,


Operator_st200_ldwc_1general_2predicate_3isrc2_4general,


Operator_st200_ldwc_1general_2predicate_3xsrc2_4general,


Operator_st200_mov_bsrc_bdest_1branch_2branch,


Operator_st200_mulf_n_1nolink_2general_3general,


Operator_st200_nandl_bdest_1branch_2branch_3branch,


Operator_st200_norl_bdest_1branch_2branch_3branch,


Operator_st200_orl_bdest_1branch_2branch_3branch,


Operator_st200_pftc_1predicate_2isrc2_3general,


Operator_st200_pftc_1predicate_2xsrc2_3general,


Operator_st200_prgadd_l1_1isrc2_2general,


Operator_st200_prgadd_l1_1xsrc2_2general,


Operator_st200_prginsadd_1isrc2_2general,


Operator_st200_prginsadd_1xsrc2_2general,


Operator_st200_prginsadd_l1_1isrc2_2general,


Operator_st200_prginsadd_l1_1xsrc2_2general,


Operator_st200_prginsset_1isrc2_2general,


Operator_st200_prginsset_1xsrc2_2general,


Operator_st200_prginsset_l1_1isrc2_2general,


Operator_st200_prginsset_l1_1xsrc2_2general,


Operator_st200_prgset_l1_1isrc2_2general,


Operator_st200_prgset_l1_1xsrc2_2general,


Operator_st200_pswmask_1nolink_2general_3isrc2,


Operator_st200_pswmask_1nolink_2general_3xsrc2,


Operator_st200_rem_1nolink_2general_3general,


Operator_st200_remu_1nolink_2general_3general,


Operator_st200_retention,


Operator_st200_rotl_1general_2general_3general,


Operator_st200_rotl_1general_2general_3isrc2,


Operator_st200_rotl_1general_2general_3xsrc2,


Operator_st200_sats_1general_2general,


Operator_st200_satso_1general_2general,


Operator_st240_sbrk,


Operator_st200_sh1adds_1general_2general_3general,


Operator_st200_sh1addso_1general_2general_3general,


Operator_st200_sh1subs_1general_2general_3general,


Operator_st200_sh1subso_1general_2general_3general,


Operator_st200_shls_1nolink_2general_3general,


Operator_st200_shls_1nolink_2general_3isrc2,


Operator_st200_shls_1nolink_2general_3xsrc2,


Operator_st200_shlso_1nolink_2general_3general,


Operator_st200_shlso_1nolink_2general_3isrc2,


Operator_st200_shlso_1nolink_2general_3xsrc2,


Operator_st200_shrrnp_1nolink_2general_3isrc2,


Operator_st200_shrrnp_1nolink_2general_3xsrc2,


Operator_st200_slct_rsrc_1general_2branch_3general_4general,


Operator_st200_stbc_1isrc2_2general_3predicate_4general,


Operator_st200_stbc_1xsrc2_2general_3predicate_4general,


Operator_st200_sthc_1isrc2_2general_3predicate_4general,


Operator_st200_sthc_1xsrc2_2general_3predicate_4general,


Operator_st200_stl_1isrc2_2general_3paired,


Operator_st200_MULTI_stl_1isrc2_2general_3paired,


Operator_st200_stl_1xsrc2_2general_3paired,


Operator_st200_MULTI_stl_1xsrc2_2general_3paired,


Operator_st200_stlc_1isrc2_2general_3predicate_4paired,


Operator_st200_MULTI_stlc_1isrc2_2general_3predicate_4paired,


Operator_st200_stlc_1xsrc2_2general_3predicate_4paired,


Operator_st200_MULTI_stlc_1xsrc2_2general_3predicate_4paired,


Operator_st200_stwc_1isrc2_2general_3predicate_4general,


Operator_st200_stwc_1xsrc2_2general_3predicate_4general,


Operator_st200_subf_n_1nolink_2general_3general,


Operator_st200_subs_1general_2general_3general,


Operator_st200_subso_1general_2general_3general,


Operator_st200_sxt_1general_2general_3general,


Operator_st200_sxt_1general_2general_3isrc2,


Operator_st200_sxt_1general_2general_3xsrc2,


Operator_st240_syscall,


Operator_st200_waitl,


Operator_st200_zxt_1general_2general_3general,


Operator_st200_zxt_1general_2general_3isrc2,


Operator_st200_zxt_1general_2general_3xsrc2,


Operator_st200_cmplef_n_1general_2general_3general,


Operator_st200_cmplef_n_1branch_2general_3general,


Operator_st200_cmpltf_n_1general_2general_3general,


Operator_st200_cmpltf_n_1branch_2general_3general,


Operator_st240_mull_1nolink_2general_3xsrc2,


Operator_st200_slctf_pb_1general_2branch_3general_4general,


Operator_st200_unpacku_pbh_1general_2general,


Operator_st200_unpacku_pbl_1general_2general,


Operator_st200_pack_ph_1general_2general_3general,



#undef Operator


#define Operator_IMMEDIATE_COUNT_MAX 1




#define OperatorParameter_CONFLICTS_COUNT_MAX 0



#define OperatorParameters(INDEX,PARAMETERS) 
#undef OperatorParameters


#define OperatorParameterArray_COUNT_MAX 5


  Operator__
#undef Operator
} enum_Operator;

/*
 * Operator_INIT --	Initialize the Operator module.
 */
#define Operator_INIT() 

/*
 * Operator_FINI --	Finalize the Operator module.
 */
#define Operator_FINI() 






//
//
typedef enum {
#define Bundling(NAME) Bundling_ ##NAME,

  Bundling_,

/*
  st200/Bundling.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Bundling_st200_ANY,
Bundling_st200_ANYX,
Bundling_st200_FIRST,
Bundling_st200_EVEN,
Bundling_st200_MEM,
Bundling_st200_MEMX,
Bundling_st200_ODD,
Bundling_st200_ALONE,
Bundling_st200_ALONEX,

#undef Bundling


#define Bundling_LOG2_MAX 4


  Bundling__
#undef Bundling
} enum_Bundling;

/*
 * Bundling_INIT --	Initialize the Bundling module.
 */
#define Bundling_INIT() 

/*
 * Bundling_FINI --	Finalize the Bundling module.
 */
#define Bundling_FINI() 






//
//
typedef enum {
#define Template(NAME,ALIGNMENT,INCREMENT,DISTANCES,ENCODE,DECODE) Template_ ##NAME,

  Template_,

/*
  st200/Template.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Template_st200_T0,


Template_st200_T1,


Template_st200_T2,


Template_st200_T3,


Template_st200_T4,


Template_st200_T5,


Template_st200_T6,


Template_st200_T7,


Template_st200_T8,


Template_st200_T9,


Template_st200_T10,


Template_st200_T11,


Template_st200_T12,


Template_st200_T13,


Template_st200_T14,


Template_st200_T15,


Template_st200_T16,


Template_st200_T17,


Template_st200_T18,


Template_st200_T19,


Template_st200_T20,


Template_st200_T21,


Template_st200_T22,


Template_st200_T23,


Template_st200_T24,


Template_st200_T25,


Template_st200_T26,


Template_st200_T27,


Template_st200_T28,


Template_st200_T29,


Template_st200_T30,


Template_st200_T31,


Template_st200_T32,


Template_st200_T33,


Template_st200_T34,


Template_st200_T35,


Template_st200_T36,


Template_st200_T37,


Template_st200_T38,


Template_st200_T39,


Template_st200_T40,


Template_st200_T41,


Template_st200_T42,


Template_st200_T43,


Template_st200_T44,


Template_st200_T45,


Template_st200_T46,


Template_st200_T47,


Template_st200_T48,


Template_st200_T49,


Template_st200_T50,


Template_st200_T51,


Template_st200_T52,


Template_st200_T53,


Template_st200_T54,


Template_st200_T55,


Template_st200_T56,


Template_st200_T57,


Template_st200_T58,


Template_st200_T59,


Template_st200_T60,


Template_st200_T61,


Template_st200_T62,


Template_st200_T63,


Template_st200_T64,


Template_st200_T65,


Template_st200_T66,


Template_st200_T67,


Template_st200_T68,


Template_st200_T69,


Template_st200_T70,


Template_st200_T71,


Template_st200_T72,


Template_st200_T73,


Template_st200_T74,


Template_st200_T75,


Template_st200_T76,


Template_st200_T77,


Template_st200_T78,


Template_st200_T79,


Template_st200_T80,


Template_st200_T81,


Template_st200_T82,


Template_st200_T83,


Template_st200_T84,


Template_st200_T85,


Template_st200_T86,


Template_st200_T87,


Template_st200_T88,


Template_st200_T89,


Template_st200_T90,


Template_st200_T91,


Template_st200_T92,


Template_st200_T93,


Template_st200_T94,


Template_st200_T95,


Template_st200_T96,


Template_st200_T97,


Template_st200_T98,


Template_st200_T99,


Template_st200_T100,


Template_st200_T0_NOP_S8,


Template_st200_T0_NOP_S25,


Template_st200_T0_NOP_S36,


Template_st200_T1_NOP_S8,


Template_st200_T1_NOP_S8_S20,


Template_st200_T1_NOP_S8_S36,


Template_st200_T1_NOP_S8_S48,


Template_st200_T1_NOP_S20,


Template_st200_T1_NOP_S20_S8,


Template_st200_T1_NOP_S20_S36,


Template_st200_T1_NOP_S20_S48,


Template_st200_T1_NOP_S36,


Template_st200_T1_NOP_S36_S8,


Template_st200_T1_NOP_S36_S20,


Template_st200_T1_NOP_S36_S48,


Template_st200_T1_NOP_S48,


Template_st200_T1_NOP_S48_S8,


Template_st200_T1_NOP_S48_S20,


Template_st200_T1_NOP_S48_S36,


Template_st200_T2_NOP_S8,


Template_st200_T2_NOP_S8_S25,


Template_st200_T2_NOP_S8_S36,


Template_st200_T2_NOP_S8_S44,


Template_st200_T2_NOP_S25,


Template_st200_T2_NOP_S25_S8,


Template_st200_T2_NOP_S25_S36,


Template_st200_T2_NOP_S25_S44,


Template_st200_T2_NOP_S36,


Template_st200_T2_NOP_S36_S8,


Template_st200_T2_NOP_S36_S25,


Template_st200_T2_NOP_S36_S44,


Template_st200_T2_NOP_S44,


Template_st200_T2_NOP_S44_S8,


Template_st200_T2_NOP_S44_S25,


Template_st200_T2_NOP_S44_S36,


Template_st200_T3_NOP_S36,


Template_st200_T4_NOP_S36,


Template_st200_T4_NOP_S44,


Template_st200_T5_NOP_S36,


Template_st200_T5_NOP_S48,


Template_st200_T6_NOP_S0,


Template_st200_T7_NOP_S5,


Template_st200_T7_NOP_S26,


Template_st200_T8_NOP_S8,


Template_st200_T8_NOP_S25,


Template_st200_T9_NOP_S5,


Template_st200_T9_NOP_S26,


Template_st200_T9_NOP_S38,


Template_st200_T10_NOP_S8,


Template_st200_T10_NOP_S26,


Template_st200_T10_NOP_S37,


Template_st200_T11_NOP_S8,


Template_st200_T11_NOP_S25,


Template_st200_T11_NOP_S38,


Template_st200_T12_NOP_S5,


Template_st200_T12_NOP_S5_S26,


Template_st200_T12_NOP_S5_S38,


Template_st200_T12_NOP_S5_S49,


Template_st200_T12_NOP_S26,


Template_st200_T12_NOP_S26_S5,


Template_st200_T12_NOP_S26_S38,


Template_st200_T12_NOP_S26_S49,


Template_st200_T12_NOP_S38,


Template_st200_T12_NOP_S38_S5,


Template_st200_T12_NOP_S38_S26,


Template_st200_T12_NOP_S38_S49,


Template_st200_T12_NOP_S49,


Template_st200_T12_NOP_S49_S5,


Template_st200_T12_NOP_S49_S26,


Template_st200_T12_NOP_S49_S38,


Template_st200_T13_NOP_S8,


Template_st200_T13_NOP_S8_S26,


Template_st200_T13_NOP_S8_S37,


Template_st200_T13_NOP_S8_S49,


Template_st200_T13_NOP_S26,


Template_st200_T13_NOP_S26_S8,


Template_st200_T13_NOP_S26_S37,


Template_st200_T13_NOP_S26_S49,


Template_st200_T13_NOP_S37,


Template_st200_T13_NOP_S37_S8,


Template_st200_T13_NOP_S37_S26,


Template_st200_T13_NOP_S37_S49,


Template_st200_T13_NOP_S49,


Template_st200_T13_NOP_S49_S8,


Template_st200_T13_NOP_S49_S26,


Template_st200_T13_NOP_S49_S37,


Template_st200_T14_NOP_S8,


Template_st200_T14_NOP_S8_S25,


Template_st200_T14_NOP_S8_S38,


Template_st200_T14_NOP_S8_S49,


Template_st200_T14_NOP_S25,


Template_st200_T14_NOP_S25_S8,


Template_st200_T14_NOP_S25_S38,


Template_st200_T14_NOP_S25_S49,


Template_st200_T14_NOP_S38,


Template_st200_T14_NOP_S38_S8,


Template_st200_T14_NOP_S38_S25,


Template_st200_T14_NOP_S38_S49,


Template_st200_T14_NOP_S49,


Template_st200_T14_NOP_S49_S8,


Template_st200_T14_NOP_S49_S25,


Template_st200_T14_NOP_S49_S38,


Template_st200_T15_NOP_S8,


Template_st200_T15_NOP_S8_S26,


Template_st200_T15_NOP_S8_S38,


Template_st200_T15_NOP_S8_S48,


Template_st200_T15_NOP_S26,


Template_st200_T15_NOP_S26_S8,


Template_st200_T15_NOP_S26_S38,


Template_st200_T15_NOP_S26_S48,


Template_st200_T15_NOP_S38,


Template_st200_T15_NOP_S38_S8,


Template_st200_T15_NOP_S38_S26,


Template_st200_T15_NOP_S38_S48,


Template_st200_T15_NOP_S48,


Template_st200_T15_NOP_S48_S8,


Template_st200_T15_NOP_S48_S26,


Template_st200_T15_NOP_S48_S38,


Template_st200_T16_NOP_S12,


Template_st200_T17_NOP_S2,


Template_st200_T17_NOP_S23,


Template_st200_T18_NOP_S12,


Template_st200_T18_NOP_S26,


Template_st200_T19_NOP_S2,


Template_st200_T19_NOP_S20,


Template_st200_T19_NOP_S36,


Template_st200_T20_NOP_S2,


Template_st200_T20_NOP_S23,


Template_st200_T20_NOP_S38,


Template_st200_T21_NOP_S4,


Template_st200_T21_NOP_S23,


Template_st200_T21_NOP_S37,


Template_st200_T22_NOP_S12,


Template_st200_T22_NOP_S26,


Template_st200_T22_NOP_S34,


Template_st200_T23_NOP_S2,


Template_st200_T23_NOP_S2_S20,


Template_st200_T23_NOP_S2_S33,


Template_st200_T23_NOP_S2_S46,


Template_st200_T23_NOP_S20,


Template_st200_T23_NOP_S20_S2,


Template_st200_T23_NOP_S20_S33,


Template_st200_T23_NOP_S20_S46,


Template_st200_T23_NOP_S33,


Template_st200_T23_NOP_S33_S2,


Template_st200_T23_NOP_S33_S20,


Template_st200_T23_NOP_S33_S46,


Template_st200_T23_NOP_S46,


Template_st200_T23_NOP_S46_S2,


Template_st200_T23_NOP_S46_S20,


Template_st200_T23_NOP_S46_S33,


Template_st200_T24_NOP_S2,


Template_st200_T24_NOP_S2_S20,


Template_st200_T24_NOP_S2_S36,


Template_st200_T24_NOP_S2_S49,


Template_st200_T24_NOP_S20,


Template_st200_T24_NOP_S20_S2,


Template_st200_T24_NOP_S20_S36,


Template_st200_T24_NOP_S20_S49,


Template_st200_T24_NOP_S36,


Template_st200_T24_NOP_S36_S2,


Template_st200_T24_NOP_S36_S20,


Template_st200_T24_NOP_S36_S49,


Template_st200_T24_NOP_S49,


Template_st200_T24_NOP_S49_S2,


Template_st200_T24_NOP_S49_S20,


Template_st200_T24_NOP_S49_S36,


Template_st200_T25_NOP_S2,


Template_st200_T25_NOP_S2_S23,


Template_st200_T25_NOP_S2_S38,


Template_st200_T25_NOP_S2_S49,


Template_st200_T25_NOP_S23,


Template_st200_T25_NOP_S23_S2,


Template_st200_T25_NOP_S23_S38,


Template_st200_T25_NOP_S23_S49,


Template_st200_T25_NOP_S38,


Template_st200_T25_NOP_S38_S2,


Template_st200_T25_NOP_S38_S23,


Template_st200_T25_NOP_S38_S49,


Template_st200_T25_NOP_S49,


Template_st200_T25_NOP_S49_S2,


Template_st200_T25_NOP_S49_S23,


Template_st200_T25_NOP_S49_S38,


Template_st200_T26_NOP_S4,


Template_st200_T26_NOP_S4_S20,


Template_st200_T26_NOP_S4_S37,


Template_st200_T26_NOP_S4_S46,


Template_st200_T26_NOP_S20,


Template_st200_T26_NOP_S20_S4,


Template_st200_T26_NOP_S20_S37,


Template_st200_T26_NOP_S20_S46,


Template_st200_T26_NOP_S37,


Template_st200_T26_NOP_S37_S4,


Template_st200_T26_NOP_S37_S20,


Template_st200_T26_NOP_S37_S46,


Template_st200_T26_NOP_S46,


Template_st200_T26_NOP_S46_S4,


Template_st200_T26_NOP_S46_S20,


Template_st200_T26_NOP_S46_S37,


Template_st200_T27_NOP_S4,


Template_st200_T27_NOP_S4_S23,


Template_st200_T27_NOP_S4_S37,


Template_st200_T27_NOP_S4_S49,


Template_st200_T27_NOP_S23,


Template_st200_T27_NOP_S23_S4,


Template_st200_T27_NOP_S23_S37,


Template_st200_T27_NOP_S23_S49,


Template_st200_T27_NOP_S37,


Template_st200_T27_NOP_S37_S4,


Template_st200_T27_NOP_S37_S23,


Template_st200_T27_NOP_S37_S49,


Template_st200_T27_NOP_S49,


Template_st200_T27_NOP_S49_S4,


Template_st200_T27_NOP_S49_S23,


Template_st200_T27_NOP_S49_S37,


Template_st200_T28_NOP_S8,


Template_st200_T28_NOP_S8_S25,


Template_st200_T28_NOP_S8_S38,


Template_st200_T28_NOP_S8_S46,


Template_st200_T28_NOP_S25,


Template_st200_T28_NOP_S25_S8,


Template_st200_T28_NOP_S25_S38,


Template_st200_T28_NOP_S25_S46,


Template_st200_T28_NOP_S38,


Template_st200_T28_NOP_S38_S8,


Template_st200_T28_NOP_S38_S25,


Template_st200_T28_NOP_S38_S46,


Template_st200_T28_NOP_S46,


Template_st200_T28_NOP_S46_S8,


Template_st200_T28_NOP_S46_S25,


Template_st200_T28_NOP_S46_S38,


Template_st200_T29_NOP_S8,


Template_st200_T29_NOP_S8_S23,


Template_st200_T29_NOP_S8_S38,


Template_st200_T29_NOP_S8_S48,


Template_st200_T29_NOP_S23,


Template_st200_T29_NOP_S23_S8,


Template_st200_T29_NOP_S23_S38,


Template_st200_T29_NOP_S23_S48,


Template_st200_T29_NOP_S38,


Template_st200_T29_NOP_S38_S8,


Template_st200_T29_NOP_S38_S23,


Template_st200_T29_NOP_S38_S48,


Template_st200_T29_NOP_S48,


Template_st200_T29_NOP_S48_S8,


Template_st200_T29_NOP_S48_S23,


Template_st200_T29_NOP_S48_S38,


Template_st200_T30_NOP_S12,


Template_st200_T30_NOP_S12_S26,


Template_st200_T30_NOP_S12_S34,


Template_st200_T30_NOP_S12_S49,


Template_st200_T30_NOP_S26,


Template_st200_T30_NOP_S26_S12,


Template_st200_T30_NOP_S26_S34,


Template_st200_T30_NOP_S26_S49,


Template_st200_T30_NOP_S34,


Template_st200_T30_NOP_S34_S12,


Template_st200_T30_NOP_S34_S26,


Template_st200_T30_NOP_S34_S49,


Template_st200_T30_NOP_S49,


Template_st200_T30_NOP_S49_S12,


Template_st200_T30_NOP_S49_S26,


Template_st200_T30_NOP_S49_S34,


Template_st200_T31_NOP_S2,


Template_st200_T31_NOP_S20,


Template_st200_T32_NOP_S8,


Template_st200_T32_NOP_S20,


Template_st200_T33_NOP_S8,


Template_st200_T33_NOP_S25,


Template_st200_T34_NOP_S2,


Template_st200_T35_NOP_S8,


Template_st200_T36_NOP_S2,


Template_st200_T36_NOP_S44,


Template_st200_T37_NOP_S8,


Template_st200_T37_NOP_S44,


Template_st200_T38_NOP_S8,


Template_st200_T38_NOP_S48,


Template_st200_T40_NOP_S38,


Template_st200_T41_NOP_S31,


Template_st200_T42_NOP_S38,


Template_st200_T42_NOP_S44,


Template_st200_T43_NOP_S31,


Template_st200_T43_NOP_S44,


Template_st200_T44_NOP_S38,


Template_st200_T44_NOP_S48,


Template_st200_T47_NOP_S13,


Template_st200_T47_NOP_S24,


Template_st200_T48_NOP_S3,


Template_st200_T48_NOP_S24,


Template_st200_T48_NOP_S37,


Template_st200_T49_NOP_S13,


Template_st200_T49_NOP_S24,


Template_st200_T49_NOP_S32,


Template_st200_T50_NOP_S3,


Template_st200_T50_NOP_S3_S24,


Template_st200_T50_NOP_S3_S37,


Template_st200_T50_NOP_S3_S49,


Template_st200_T50_NOP_S24,


Template_st200_T50_NOP_S24_S3,


Template_st200_T50_NOP_S24_S37,


Template_st200_T50_NOP_S24_S49,


Template_st200_T50_NOP_S37,


Template_st200_T50_NOP_S37_S3,


Template_st200_T50_NOP_S37_S24,


Template_st200_T50_NOP_S37_S49,


Template_st200_T50_NOP_S49,


Template_st200_T50_NOP_S49_S3,


Template_st200_T50_NOP_S49_S24,


Template_st200_T50_NOP_S49_S37,


Template_st200_T51_NOP_S13,


Template_st200_T51_NOP_S13_S24,


Template_st200_T51_NOP_S13_S32,


Template_st200_T51_NOP_S13_S49,


Template_st200_T51_NOP_S24,


Template_st200_T51_NOP_S24_S13,


Template_st200_T51_NOP_S24_S32,


Template_st200_T51_NOP_S24_S49,


Template_st200_T51_NOP_S32,


Template_st200_T51_NOP_S32_S13,


Template_st200_T51_NOP_S32_S24,


Template_st200_T51_NOP_S32_S49,


Template_st200_T51_NOP_S49,


Template_st200_T51_NOP_S49_S13,


Template_st200_T51_NOP_S49_S24,


Template_st200_T51_NOP_S49_S32,


Template_st200_T52_NOP_S3,


Template_st200_T52_NOP_S24,


Template_st200_T53_NOP_S13,


Template_st200_T53_NOP_S24,


Template_st200_T54_NOP_S11,


Template_st200_T55_NOP_S3,


Template_st200_T55_NOP_S24,


Template_st200_T56_NOP_S11,


Template_st200_T56_NOP_S22,


Template_st200_T57_NOP_S1,


Template_st200_T57_NOP_S26,


Template_st200_T57_NOP_S35,


Template_st200_T58_NOP_S3,


Template_st200_T58_NOP_S19,


Template_st200_T58_NOP_S35,


Template_st200_T59_NOP_S3,


Template_st200_T59_NOP_S24,


Template_st200_T59_NOP_S38,


Template_st200_T60_NOP_S11,


Template_st200_T60_NOP_S22,


Template_st200_T60_NOP_S38,


Template_st200_T61_NOP_S11,


Template_st200_T61_NOP_S26,


Template_st200_T61_NOP_S37,


Template_st200_T62_NOP_S1,


Template_st200_T62_NOP_S1_S26,


Template_st200_T62_NOP_S1_S35,


Template_st200_T62_NOP_S1_S49,


Template_st200_T62_NOP_S26,


Template_st200_T62_NOP_S26_S1,


Template_st200_T62_NOP_S26_S35,


Template_st200_T62_NOP_S26_S49,


Template_st200_T62_NOP_S35,


Template_st200_T62_NOP_S35_S1,


Template_st200_T62_NOP_S35_S26,


Template_st200_T62_NOP_S35_S49,


Template_st200_T62_NOP_S49,


Template_st200_T62_NOP_S49_S1,


Template_st200_T62_NOP_S49_S26,


Template_st200_T62_NOP_S49_S35,


Template_st200_T63_NOP_S3,


Template_st200_T63_NOP_S3_S18,


Template_st200_T63_NOP_S3_S32,


Template_st200_T63_NOP_S3_S47,


Template_st200_T63_NOP_S18,


Template_st200_T63_NOP_S18_S3,


Template_st200_T63_NOP_S18_S32,


Template_st200_T63_NOP_S18_S47,


Template_st200_T63_NOP_S32,


Template_st200_T63_NOP_S32_S3,


Template_st200_T63_NOP_S32_S18,


Template_st200_T63_NOP_S32_S47,


Template_st200_T63_NOP_S47,


Template_st200_T63_NOP_S47_S3,


Template_st200_T63_NOP_S47_S18,


Template_st200_T63_NOP_S47_S32,


Template_st200_T64_NOP_S3,


Template_st200_T64_NOP_S3_S21,


Template_st200_T64_NOP_S3_S35,


Template_st200_T64_NOP_S3_S48,


Template_st200_T64_NOP_S21,


Template_st200_T64_NOP_S21_S3,


Template_st200_T64_NOP_S21_S35,


Template_st200_T64_NOP_S21_S48,


Template_st200_T64_NOP_S35,


Template_st200_T64_NOP_S35_S3,


Template_st200_T64_NOP_S35_S21,


Template_st200_T64_NOP_S35_S48,


Template_st200_T64_NOP_S48,


Template_st200_T64_NOP_S48_S3,


Template_st200_T64_NOP_S48_S21,


Template_st200_T64_NOP_S48_S35,


Template_st200_T65_NOP_S3,


Template_st200_T65_NOP_S3_S19,


Template_st200_T65_NOP_S3_S35,


Template_st200_T65_NOP_S3_S49,


Template_st200_T65_NOP_S19,


Template_st200_T65_NOP_S19_S3,


Template_st200_T65_NOP_S19_S35,


Template_st200_T65_NOP_S19_S49,


Template_st200_T65_NOP_S35,


Template_st200_T65_NOP_S35_S3,


Template_st200_T65_NOP_S35_S19,


Template_st200_T65_NOP_S35_S49,


Template_st200_T65_NOP_S49,


Template_st200_T65_NOP_S49_S3,


Template_st200_T65_NOP_S49_S19,


Template_st200_T65_NOP_S49_S35,


Template_st200_T66_NOP_S3,


Template_st200_T66_NOP_S3_S24,


Template_st200_T66_NOP_S3_S38,


Template_st200_T66_NOP_S3_S45,


Template_st200_T66_NOP_S24,


Template_st200_T66_NOP_S24_S3,


Template_st200_T66_NOP_S24_S38,


Template_st200_T66_NOP_S24_S45,


Template_st200_T66_NOP_S38,


Template_st200_T66_NOP_S38_S3,


Template_st200_T66_NOP_S38_S24,


Template_st200_T66_NOP_S38_S45,


Template_st200_T66_NOP_S45,


Template_st200_T66_NOP_S45_S3,


Template_st200_T66_NOP_S45_S24,


Template_st200_T66_NOP_S45_S38,


Template_st200_T67_NOP_S11,


Template_st200_T67_NOP_S11_S22,


Template_st200_T67_NOP_S11_S38,


Template_st200_T67_NOP_S11_S49,


Template_st200_T67_NOP_S22,


Template_st200_T67_NOP_S22_S11,


Template_st200_T67_NOP_S22_S38,


Template_st200_T67_NOP_S22_S49,


Template_st200_T67_NOP_S38,


Template_st200_T67_NOP_S38_S11,


Template_st200_T67_NOP_S38_S22,


Template_st200_T67_NOP_S38_S49,


Template_st200_T67_NOP_S49,


Template_st200_T67_NOP_S49_S11,


Template_st200_T67_NOP_S49_S22,


Template_st200_T67_NOP_S49_S38,


Template_st200_T68_NOP_S11,


Template_st200_T68_NOP_S11_S26,


Template_st200_T68_NOP_S11_S38,


Template_st200_T68_NOP_S11_S48,


Template_st200_T68_NOP_S26,


Template_st200_T68_NOP_S26_S11,


Template_st200_T68_NOP_S26_S38,


Template_st200_T68_NOP_S26_S48,


Template_st200_T68_NOP_S38,


Template_st200_T68_NOP_S38_S11,


Template_st200_T68_NOP_S38_S26,


Template_st200_T68_NOP_S38_S48,


Template_st200_T68_NOP_S48,


Template_st200_T68_NOP_S48_S11,


Template_st200_T68_NOP_S48_S26,


Template_st200_T68_NOP_S48_S38,


Template_st200_T69_NOP_S11,


Template_st200_T69_NOP_S11_S26,


Template_st200_T69_NOP_S11_S37,


Template_st200_T69_NOP_S11_S49,


Template_st200_T69_NOP_S26,


Template_st200_T69_NOP_S26_S11,


Template_st200_T69_NOP_S26_S37,


Template_st200_T69_NOP_S26_S49,


Template_st200_T69_NOP_S37,


Template_st200_T69_NOP_S37_S11,


Template_st200_T69_NOP_S37_S26,


Template_st200_T69_NOP_S37_S49,


Template_st200_T69_NOP_S49,


Template_st200_T69_NOP_S49_S11,


Template_st200_T69_NOP_S49_S26,


Template_st200_T69_NOP_S49_S37,


Template_st200_T70_NOP_S1,


Template_st200_T70_NOP_S26,


Template_st200_T71_NOP_S3,


Template_st200_T71_NOP_S19,


Template_st200_T72_NOP_S3,


Template_st200_T72_NOP_S26,


Template_st200_T73_NOP_S1,


Template_st200_T74_NOP_S3,


Template_st200_T75_NOP_S1,


Template_st200_T75_NOP_S49,


Template_st200_T76_NOP_S3,


Template_st200_T76_NOP_S43,


Template_st200_T77_NOP_S3,


Template_st200_T77_NOP_S49,


Template_st200_T79_NOP_S32,


Template_st200_T80_NOP_S37,


Template_st200_T81_NOP_S32,


Template_st200_T81_NOP_S49,


Template_st200_T82_NOP_S32,


Template_st200_T82_NOP_S43,


Template_st200_T83_NOP_S37,


Template_st200_T83_NOP_S49,


Template_st200_T86_NOP_S12,


Template_st200_T86_NOP_S25,


Template_st200_T87_NOP_S12,


Template_st200_T87_NOP_S25,


Template_st200_T87_NOP_S38,


Template_st200_T88_NOP_S12,


Template_st200_T88_NOP_S12_S20,


Template_st200_T88_NOP_S12_S38,


Template_st200_T88_NOP_S12_S48,


Template_st200_T88_NOP_S20,


Template_st200_T88_NOP_S20_S12,


Template_st200_T88_NOP_S20_S38,


Template_st200_T88_NOP_S20_S48,


Template_st200_T88_NOP_S38,


Template_st200_T88_NOP_S38_S12,


Template_st200_T88_NOP_S38_S20,


Template_st200_T88_NOP_S38_S48,


Template_st200_T88_NOP_S48,


Template_st200_T88_NOP_S48_S12,


Template_st200_T88_NOP_S48_S20,


Template_st200_T88_NOP_S48_S38,


Template_st200_T89_NOP_S12,


Template_st200_T89_NOP_S12_S25,


Template_st200_T89_NOP_S12_S38,


Template_st200_T89_NOP_S12_S44,


Template_st200_T89_NOP_S25,


Template_st200_T89_NOP_S25_S12,


Template_st200_T89_NOP_S25_S38,


Template_st200_T89_NOP_S25_S44,


Template_st200_T89_NOP_S38,


Template_st200_T89_NOP_S38_S12,


Template_st200_T89_NOP_S38_S25,


Template_st200_T89_NOP_S38_S44,


Template_st200_T89_NOP_S44,


Template_st200_T89_NOP_S44_S12,


Template_st200_T89_NOP_S44_S25,


Template_st200_T89_NOP_S44_S38,


Template_st200_T90_NOP_S12,


Template_st200_T90_NOP_S20,


Template_st200_T91_NOP_S12,


Template_st200_T91_NOP_S25,


Template_st200_T92_NOP_S12,


Template_st200_T93_NOP_S12,


Template_st200_T93_NOP_S44,


Template_st200_T94_NOP_S12,


Template_st200_T94_NOP_S48,


Template_st200_T95_NOP_S3,


Template_st200_T95_NOP_S3_S26,


Template_st200_T95_NOP_S3_S37,


Template_st200_T95_NOP_S3_S47,


Template_st200_T95_NOP_S26,


Template_st200_T95_NOP_S26_S3,


Template_st200_T95_NOP_S26_S37,


Template_st200_T95_NOP_S26_S47,


Template_st200_T95_NOP_S37,


Template_st200_T95_NOP_S37_S3,


Template_st200_T95_NOP_S37_S26,


Template_st200_T95_NOP_S37_S47,


Template_st200_T95_NOP_S47,


Template_st200_T95_NOP_S47_S3,


Template_st200_T95_NOP_S47_S26,


Template_st200_T95_NOP_S47_S37,


Template_st200_T96_NOP_S13,


Template_st200_T96_NOP_S13_S26,


Template_st200_T96_NOP_S13_S32,


Template_st200_T96_NOP_S13_S47,


Template_st200_T96_NOP_S26,


Template_st200_T96_NOP_S26_S13,


Template_st200_T96_NOP_S26_S32,


Template_st200_T96_NOP_S26_S47,


Template_st200_T96_NOP_S32,


Template_st200_T96_NOP_S32_S13,


Template_st200_T96_NOP_S32_S26,


Template_st200_T96_NOP_S32_S47,


Template_st200_T96_NOP_S47,


Template_st200_T96_NOP_S47_S13,


Template_st200_T96_NOP_S47_S26,


Template_st200_T96_NOP_S47_S32,


Template_st200_T97_NOP_S3,


Template_st200_T97_NOP_S47,


Template_st200_T98_NOP_S13,


Template_st200_T98_NOP_S47,


Template_st200_T99_NOP_S32,


Template_st200_T99_NOP_S47,


Template_st200_T100_NOP_S37,


Template_st200_T100_NOP_S47,



#undef Template


#define Template_SYLLABLES_COUNT_MAX 4



#define Template_INCREMENT_MAX 16


  Template__
#undef Template
} enum_Template;

/*
 * Template_INIT --	Initialize the Template module.
 */
#define Template_INIT() 

/*
 * Template_FINI --	Finalize the Template module.
 */
#define Template_FINI() 






//
//
typedef enum {
#define Bundle(NAME,ALIGNMENT,CONTENTS,INVERSE,TEMPLATE) Bundle_ ##NAME,

  Bundle_,

/*
  st200/Bundle.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Bundle_st200_EVEN_ANY_MEM_0_8,
Bundle_st200_ANY_EVEN_MEM_4_8,
Bundle_st200_FIRST_EVEN_MEM_4_8,
Bundle_st200_FIRST_MEM_EVEN_0_16,
Bundle_st200_EVEN_ANY_MEM_ANY_0_8,
Bundle_st200_ANY_EVEN_ANY_MEM_4_8,
Bundle_st200_EVEN_ODD_MEM_ANY_0_8,
Bundle_st200_ODD_EVEN_ANY_MEM_4_8,
Bundle_st200_FIRST_EVEN_ANY_MEM_4_8,
Bundle_st200_FIRST_MEM_EVEN_ANY_0_16,
Bundle_st200_FIRST_EVEN_ODD_MEM_4_8,
Bundle_st200_FIRST_MEM_EVEN_ODD_0_16,
Bundle_st200_MEMX_EVEN_0_16,
Bundle_st200_EVEN_MEMX_4_16,
Bundle_st200_EVEN_MEMX_8_16,
Bundle_st200_MEMX_EVEN_12_16,
Bundle_st200_MEMX_EVEN_ANY_0_16,
Bundle_st200_ANY_EVEN_MEMX_4_16,
Bundle_st200_EVEN_MEMX_ANY_8_16,
Bundle_st200_MEMX_ANY_EVEN_12_16,
Bundle_st200_MEMX_EVEN_ODD_0_16,
Bundle_st200_ODD_EVEN_MEMX_4_16,
Bundle_st200_EVEN_MEMX_ODD_8_16,
Bundle_st200_MEMX_ODD_EVEN_12_16,
Bundle_st200_ANYX_EVEN_MEM_0_16,
Bundle_st200_MEM_EVEN_ANYX_4_16,
Bundle_st200_EVEN_ANYX_MEM_8_16,
Bundle_st200_MEM_ANYX_EVEN_12_16,
Bundle_st200_ANY_0_4,
Bundle_st200_FIRST_0_4,
Bundle_st200_MEM_0_4,
Bundle_st200_ALONE_0_4,
Bundle_st200_ANY_ANY_0_4,
Bundle_st200_FIRST_ANY_0_4,
Bundle_st200_ANY_MEM_0_4,
Bundle_st200_FIRST_MEM_0_4,
Bundle_st200_ANY_ANY_ANY_0_4,
Bundle_st200_FIRST_ANY_ANY_0_4,
Bundle_st200_MEM_ANY_ANY_0_4,
Bundle_st200_FIRST_MEM_ANY_0_4,
Bundle_st200_ANY_ANY_ANY_ANY_0_4,
Bundle_st200_FIRST_ANY_ANY_ANY_0_4,
Bundle_st200_ANY_ANY_ANY_MEM_0_4,
Bundle_st200_FIRST_ANY_ANY_MEM_0_4,
Bundle_st200_EVEN_0_8,
Bundle_st200_EVEN_4_8,
Bundle_st200_ANY_ODD_0_8,
Bundle_st200_ODD_ANY_4_8,
Bundle_st200_FIRST_ODD_0_8,
Bundle_st200_FIRST_ODD_4_8,
Bundle_st200_EVEN_ODD_0_8,
Bundle_st200_ODD_EVEN_4_8,
Bundle_st200_MEM_ODD_0_8,
Bundle_st200_ODD_MEM_4_8,
Bundle_st200_EVEN_ANY_0_8,
Bundle_st200_ANY_EVEN_4_8,
Bundle_st200_EVEN_ANY_ANY_0_8,
Bundle_st200_ANY_EVEN_ANY_4_8,
Bundle_st200_EVEN_ODD_ANY_0_8,
Bundle_st200_ODD_EVEN_ANY_4_8,
Bundle_st200_FIRST_ANY_EVEN_0_8,
Bundle_st200_FIRST_EVEN_ANY_4_8,
Bundle_st200_FIRST_ODD_EVEN_0_8,
Bundle_st200_FIRST_EVEN_ODD_4_8,
Bundle_st200_EVEN_ANY_EVEN_0_8,
Bundle_st200_ANY_EVEN_EVEN_4_8,
Bundle_st200_EVEN_ODD_EVEN_0_8,
Bundle_st200_ODD_EVEN_EVEN_4_8,
Bundle_st200_EVEN_ODD_MEM_0_8,
Bundle_st200_ODD_EVEN_MEM_4_8,
Bundle_st200_ANY_ODD_ANY_0_8,
Bundle_st200_ODD_ANY_ANY_4_8,
Bundle_st200_FIRST_ODD_ANY_0_8,
Bundle_st200_FIRST_ANY_ODD_4_8,
Bundle_st200_ANY_ODD_MEM_0_8,
Bundle_st200_ODD_ANY_MEM_4_8,
Bundle_st200_FIRST_ODD_MEM_0_8,
Bundle_st200_FIRST_MEM_ODD_4_8,
Bundle_st200_ANY_ODD_ANY_ANY_0_8,
Bundle_st200_ODD_ANY_ANY_ANY_4_8,
Bundle_st200_EVEN_ODD_ANY_ANY_0_8,
Bundle_st200_ODD_EVEN_ANY_ANY_4_8,
Bundle_st200_ANY_ODD_ANY_ODD_0_8,
Bundle_st200_ODD_ANY_ODD_ANY_4_8,
Bundle_st200_ANY_ODD_EVEN_ODD_0_8,
Bundle_st200_ODD_EVEN_ODD_ANY_4_8,
Bundle_st200_FIRST_ODD_ANY_ANY_0_8,
Bundle_st200_FIRST_ANY_ODD_ANY_4_8,
Bundle_st200_FIRST_ODD_EVEN_ANY_0_8,
Bundle_st200_FIRST_EVEN_ODD_ANY_4_8,
Bundle_st200_FIRST_ODD_ANY_ODD_0_8,
Bundle_st200_FIRST_ODD_EVEN_ODD_0_8,
Bundle_st200_EVEN_ODD_EVEN_ANY_0_8,
Bundle_st200_ANY_EVEN_ODD_EVEN_4_8,
Bundle_st200_EVEN_ODD_EVEN_ODD_0_8,
Bundle_st200_ODD_EVEN_ODD_EVEN_4_8,
Bundle_st200_ANY_ODD_ANY_MEM_0_8,
Bundle_st200_ODD_ANY_MEM_ANY_4_8,
Bundle_st200_ANY_ODD_MEM_ODD_0_8,
Bundle_st200_ODD_MEM_ODD_ANY_4_8,
Bundle_st200_EVEN_ODD_MEM_ODD_0_8,
Bundle_st200_ODD_EVEN_ODD_MEM_4_8,
Bundle_st200_EVEN_ANY_ANY_ANY_0_8,
Bundle_st200_ANY_EVEN_ANY_ANY_4_8,
Bundle_st200_FIRST_ANY_EVEN_ANY_0_8,
Bundle_st200_FIRST_EVEN_ANY_ANY_4_8,
Bundle_st200_EVEN_ANY_EVEN_ANY_0_8,
Bundle_st200_ANY_EVEN_ANY_EVEN_4_8,
Bundle_st200_FIRST_ODD_ANY_MEM_0_8,
Bundle_st200_FIRST_MEM_ODD_ANY_4_8,
Bundle_st200_FIRST_ODD_MEM_ODD_0_8,
Bundle_st200_ANYX_ANY_ANY_0_8,
Bundle_st200_ANYX_ANY_ANY_4_8,
Bundle_st200_ANYX_ANY_ODD_0_8,
Bundle_st200_ANYX_ODD_ANY_4_8,
Bundle_st200_FIRST_ANYX_ANY_0_8,
Bundle_st200_FIRST_ANYX_ANY_4_8,
Bundle_st200_FIRST_ANYX_ODD_0_8,
Bundle_st200_ANYX_EVEN_ANY_0_8,
Bundle_st200_ANYX_ANY_EVEN_4_8,
Bundle_st200_ANYX_EVEN_ODD_0_8,
Bundle_st200_ANYX_ODD_EVEN_4_8,
Bundle_st200_ANYX_ANY_MEM_0_8,
Bundle_st200_ANYX_MEM_ANY_4_8,
Bundle_st200_ANYX_MEM_ODD_0_8,
Bundle_st200_ANYX_ODD_MEM_4_8,
Bundle_st200_MEMX_ANY_ANY_0_8,
Bundle_st200_MEMX_ANY_ANY_4_8,
Bundle_st200_MEMX_ANY_ODD_0_8,
Bundle_st200_MEMX_ODD_ANY_4_8,
Bundle_st200_FIRST_MEMX_ANY_0_8,
Bundle_st200_FIRST_MEMX_ANY_4_8,
Bundle_st200_FIRST_MEMX_ODD_0_8,
Bundle_st200_FIRST_ANYX_MEM_0_8,
Bundle_st200_FIRST_ANYX_MEM_4_8,
Bundle_st200_ANYX_ANY_0_8,
Bundle_st200_ANYX_ANY_4_8,
Bundle_st200_FIRST_ANYX_0_8,
Bundle_st200_FIRST_ANYX_4_8,
Bundle_st200_ANYX_EVEN_0_8,
Bundle_st200_ANYX_EVEN_4_8,
Bundle_st200_ANYX_MEM_0_8,
Bundle_st200_ANYX_MEM_4_8,
Bundle_st200_MEMX_ANY_0_8,
Bundle_st200_MEMX_ANY_4_8,
Bundle_st200_FIRST_MEMX_0_8,
Bundle_st200_FIRST_MEMX_4_8,
Bundle_st200_ANYX_0_8,
Bundle_st200_ANYX_4_8,
Bundle_st200_MEMX_0_8,
Bundle_st200_MEMX_4_8,
Bundle_st200_ALONEX_0_8,
Bundle_st200_ALONEX_4_8,
Bundle_st200_ANYX_ANYX_0_8,
Bundle_st200_ANYX_ANYX_4_8,
Bundle_st200_ANYX_MEMX_0_8,
Bundle_st200_ANYX_MEMX_4_8,
Bundle_st200_EVEN_MEM_0_8,
Bundle_st200_EVEN_MEM_4_8,
Bundle_st200_FIRST_EVEN_MEMX_4_16,
Bundle_st200_FIRST_MEMX_EVEN_12_16,
Bundle_st200_ODD_0_8,
Bundle_st200_ODD_4_8,
Bundle_st200_FIRST_EVEN_0_8,
Bundle_st200_FIRST_EVEN_4_8,
Bundle_st200_ANY_ODD_ODD_0_8,
Bundle_st200_ODD_ANY_ODD_4_8,
Bundle_st200_EVEN_ODD_ODD_0_8,
Bundle_st200_ODD_EVEN_ODD_4_8,
Bundle_st200_ODD_MEM_ODD_0_8,
Bundle_st200_ODD_MEM_ODD_4_8,
Bundle_st200_FIRST_EVEN_ANY_EVEN_4_8,
Bundle_st200_FIRST_EVEN_ODD_EVEN_4_8,
Bundle_st200_FIRST_ANYX_EVEN_4_8,
Bundle_st200_ANYX_ODD_0_8,
Bundle_st200_ANYX_ODD_4_8,
Bundle_st200_MEMX_ODD_0_8,
Bundle_st200_MEMX_ODD_4_8,
Bundle_st200_0_4,
Bundle_st200_EVEN_EVEN_0_8,
Bundle_st200_EVEN_EVEN_4_8,
Bundle_st200_ODD_ODD_0_8,
Bundle_st200_ODD_ODD_4_8,
Bundle_st200_FIRST_ODD_ODD_0_8,
Bundle_st200_FIRST_EVEN_EVEN_4_8,

#undef Bundle


#define Bundle_ISSUE_MAX 4



#define BundleMatch(HASH,CANONIC,BUNDLES) 
#undef BundleMatch


#define BundleMatch_HASH_MAX 3391



#define BundleMatch_COUNT_MAX 4


  Bundle__
#undef Bundle
} enum_Bundle;

/*
 * Bundle_INIT --	Initialize the Bundle module.
 */

void
Bundle_INIT(void);




/*
 * Bundle_FINI --	Finalize the Bundle module.
 */
#define Bundle_FINI() 






//
//
typedef enum {
#define Reservation(NAME,TABLE) Reservation_ ##NAME,

  Reservation_,

/*
  st200/Reservation.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Reservation_st220_ALL,
Reservation_st220_ALU,
Reservation_st220_ALUX,
Reservation_st220_CTL,
Reservation_st220_ODD,
Reservation_st220_ODDX,
Reservation_st220_MEM,
Reservation_st220_MEMX,
Reservation_st220_PSW,
Reservation_st220_EVEN,
Reservation_st231_ALL,
Reservation_st231_ALU,
Reservation_st231_ALUX,
Reservation_st231_CTL,
Reservation_st231_ODD,
Reservation_st231_ODDX,
Reservation_st231_MEM,
Reservation_st231_MEMX,
Reservation_st231_PSW,
Reservation_st231_EVEN,
Reservation_st240_ALL,
Reservation_st240_ALU,
Reservation_st240_ALUX,
Reservation_st240_CTL,
Reservation_st240_ODD,
Reservation_st240_ODDX,
Reservation_st240_MEM,
Reservation_st240_MEMX,
Reservation_st240_PSW,
Reservation_st240_EVEN,

#undef Reservation


#define ReservationColumn(COLUMNINDEX,REQUIREMENTS) 
#undef ReservationColumn


#define Reservation_COLUMNS_COUNT_MAX 1


  Reservation__
#undef Reservation
} enum_Reservation;

//
extern /*const*/ ReservationTable_
Reservation_Table[];

/*
 * Reservation_InitializeODot --	Initialize the contents of the Reservation_ODot array.
 */
void
Reservation_InitializeODot(void);

/*
 * Reservation_INIT --	Initialize the Reservation module.
 */
void
Reservation_INIT(void);

/*
 * Reservation_FINI --	Finalize the Reservation module.
 */
#define Reservation_FINI() 






//
//
typedef enum {
#define Scheduling(NAME,PROCESSOR,BUNDLING,RESERVATION) Scheduling_ ##NAME,

  Scheduling_,

/*
  st200/Scheduling.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Scheduling_st220_ALL,
Scheduling_st220_ALLX,
Scheduling_st220_ALU,
Scheduling_st220_ALUX,
Scheduling_st220_CTL,
Scheduling_st220_ODD,
Scheduling_st220_ODDX,
Scheduling_st220_MEM,
Scheduling_st220_MEMX,
Scheduling_st220_PSW,
Scheduling_st220_EVEN,
Scheduling_st231_ALL,
Scheduling_st231_ALLX,
Scheduling_st231_ALU,
Scheduling_st231_ALUX,
Scheduling_st231_CTL,
Scheduling_st231_ODD,
Scheduling_st231_ODDX,
Scheduling_st231_MEM,
Scheduling_st231_MEMX,
Scheduling_st231_PSW,
Scheduling_st231_EVEN,
Scheduling_st240_ALL,
Scheduling_st240_ALLX,
Scheduling_st240_ALU,
Scheduling_st240_ALUX,
Scheduling_st240_CTL,
Scheduling_st240_ODD,
Scheduling_st240_ODDX,
Scheduling_st240_MEM,
Scheduling_st240_MEMX,
Scheduling_st240_PSW,
Scheduling_st240_EVEN,

#undef Scheduling

  Scheduling__
#undef Scheduling
} enum_Scheduling;

/*
 * Scheduling_INIT --	Initialize the Scheduling module.
 */
#define Scheduling_INIT() 

/*
 * Scheduling_FINI --	Finalize the Scheduling module.
 */
#define Scheduling_FINI() 






//
//
typedef enum {
#define Convention(NAME,RESERVED,ARGUMENT,RESULT,STRUCT,CALLER,CALLEE,PROGRAM,RETURN,STACK,STATIC,FRAME,GLOBAL,LOCAL,WIRED,ZERO,ONE,ALIGNTEXT,ALIGNDATA,ALIGNHEAP,ALIGNSTACK,NATIVEINT,NATIVEUINT,NATIVEFLOAT,NATIVEPTR) Convention_ ##NAME,




  Convention_,

/*
  st200/Convention.enum --
  Automatically generated from the Machine Description System (MDS).
*/







Convention_st200_embedded,
Convention_st200_PIC,
#undef Convention

  Convention__
#undef Convention
} enum_Convention;

//
// DEPRECATED! This array entries are oudated when register sets are dynamically adjusted.
extern const RegisterList_
Convention_RoleRegisterLists[Convention__][ConventionRole__];
#define Convention_argumentRegisterList(THIS) (&Convention_RoleRegisterLists[THIS][ConventionRole_Argument])

#define Convention_resultRegisterList(THIS) (&Convention_RoleRegisterLists[THIS][ConventionRole_Result])


/*
 * Convention_setRole --	Set a Register ConventionRole for THIS Convention.
 */
void
Convention_setRole(Convention THIS, ConventionRole role, Register registre);

/*
 * Convention_INIT --	Initialize the Convention module.
 */
void
Convention_INIT(void);

/*
 * Convention_FINI --	Finalize the Convention module.
 */
#define Convention_FINI() 






//
/*
 * Decode_Encoding function prototypes.
 */

#define Encoding(NAME,PROCESSOR,WORDTYPE,WORDCOUNT) extern Instance Decode_Encoding_ ##NAME(const void *buffer);



/*
  st200/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



extern Instance Decode_Encoding_st220_ANY(const void *buffer);
extern Instance Decode_Encoding_st220_ANYX(const void *buffer);
extern Instance Decode_Encoding_st231_ANY(const void *buffer);
extern Instance Decode_Encoding_st231_ANYX(const void *buffer);
extern Instance Decode_Encoding_st240_ANY(const void *buffer);
extern Instance Decode_Encoding_st240_ANYX(const void *buffer);

#undef Encoding







/*
 * Decode_Modifier functions.
 */
#define DECODE(e) e
#define Modifier(NAME,VALUES,ENCODE,DECODE) static inline void Decode_Modifier_ ##NAME(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; DECODE; *(OperandDecoded *)result = VALUE; }
/*
  st200/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Modifier





#undef DECODE

/*
 * Decode_Immediate functions.
 */
#define DECODE(e) e
#define Immediate(NAME,MINVALUE,MAXVALUE,SIGNED,RELOCATIONS,ENCODE,DECODE) static inline void Decode_Immediate_ ##NAME(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; DECODE; *ImmediateConstant__VALUE(constant) = VALUE; }
/*
  st200/Immediate.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline void Decode_Immediate_st200_btarg(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-23); __value >>= (sizeof(__value)*8-23); __value <<= 2; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_st200_isrc2(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-9); __value >>= (sizeof(__value)*8-9); VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_st200_imm(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-23); __value >>= (sizeof(__value)*8-23); VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_st200_sbrknum(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_st200_xsrc2(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-32); __value >>= (sizeof(__value)*8-32); VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_st200_brknum(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }




#undef Immediate





#undef DECODE
#undef VALUETYPE

/*
 * Decode_RegClass functions.
 */
#define DECODE(e) e
#define RegClass(NAME,REGISTERS,REGFILE,ENCODE,DECODE,MINREADSTAGES) static inline void Decode_RegClass_ ##NAME(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; DECODE; *(OperandDecoded *)result = VALUE + Register__BIAS; }
/*
  st200/RegClass.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline void Decode_RegClass_st200_branch(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_general(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 8; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_nolink(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 8; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_link(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 71; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_predicate(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 1; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_paired(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 73; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_pairedfirst(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 8; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_pairedsecond(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 8; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_nzpaired(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 74; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_nzpairedfirst(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 10; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_st200_nzpairedsecond(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 11; *(OperandDecoded *)result = VALUE + (Register_ + 1); }






#undef RegClass

#undef DECODE

/*
 * Decode_RegMask functions.
 */
#define DECODE(e) e
#define RegMask(NAME,REGISTERS,REGFILE,ENCODE,DECODE) static inline void Decode_RegMask_ ##NAME(OperandEncoded encoded, void *result) { RegisterSet registerSet = (RegisterSet)result; RegisterSetWord WORD = (RegisterSetWord)encoded; RegisterSetWord *WORDS = RegisterSet_WORDS(registerSet); DECODE; }
/*
  st200/RegMask.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef RegMask

#undef DECODE

/*
 * Decode_INIT --	Initialize the Decode module.
 */
#define Decode_INIT() 

/*
 * Decode_FINI --	Finalize the Decode module.
 */
#define Decode_FINI() 

//

extern Instance
Decode_Encoding_st220_ANY(const void *buffer);


//

extern Instance
Decode_Encoding_st220_ANYX(const void *buffer);


//

extern Instance
Decode_Encoding_st231_ANY(const void *buffer);


//

extern Instance
Decode_Encoding_st231_ANYX(const void *buffer);


//

extern Instance
Decode_Encoding_st240_ANY(const void *buffer);


//

extern Instance
Decode_Encoding_st240_ANYX(const void *buffer);





/*
 * MDS_Test --	Setup and execute a MDS test.
 */
void
MDS_Test(void (*test)(void));

//
//#define MDS_FAMILY "st200"
//#define MDS_FAMILY_st200
#define MDS_CORE_st220 
#define MDS_CORE_st231 
#define MDS_CORE_st240 
