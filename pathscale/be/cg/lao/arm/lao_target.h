

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
  arm/Platform.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Platform_arm_arch,

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
  arm/Resource.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Resource_arm_ISSUE,

Resource_arm_ALU,

Resource_arm_MAC,

Resource_arm_MEM,


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
  arm/Processor.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Processor_armv5e_cpu,

Processor_armv6_cpu,


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
  arm/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Encoding_armv5e_ANY,
Encoding_armv6_ANY,

#undef Encoding


#define Encoding_SIZE_MAX 4


  Encoding__
#undef Encoding
} enum_Encoding;

/*
 * Encoding word types.
 */
#define WORDTYPE(type) type
#define Encoding(NAME,PROCESSOR,WORDTYPE,WORDCOUNT) typedef WORDTYPE Encoding_ ##NAME ##_t;


/*
  arm/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



typedef uint32_t Encoding_armv5e_ANY_t;
typedef uint32_t Encoding_armv6_ANY_t;

#undef Encoding





#undef WORDTYPE

/*
 * Encoding word counts.
 */
enum {
#define WORDCOUNT(d) d
#define Encoding(NAME,PROCESSOR,WORDTYPE,WORDCOUNT) Encoding_ ##NAME ##_count = WORDCOUNT,


/*
  arm/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Encoding_armv5e_ANY_count = 1,
Encoding_armv6_ANY_count = 1,

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
  arm/NativeType.enum --
  Automatically generated from the Machine Description System (MDS).
*/



NativeType_arm_Bool,
NativeType_arm_UInt8,
NativeType_arm_Int8,
NativeType_arm_UInt16,
NativeType_arm_Int16,
NativeType_arm_UInt32,
NativeType_arm_Int32,
NativeType_arm_UInt64,
NativeType_arm_Int64,
NativeType_arm_UIntPtr,
NativeType_arm_IntPtr,
NativeType_arm_Float32,
NativeType_arm_Float64,

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
  arm/Storage.enum --
  Automatically generated from the Machine Description System (MDS).
*/



StorageCell_arm_MEM,
StorageCell_arm_PC,
StorageCell_arm_GR0,
StorageCell_arm_GR1,
StorageCell_arm_GR2,
StorageCell_arm_GR3,
StorageCell_arm_GR4,
StorageCell_arm_GR5,
StorageCell_arm_GR6,
StorageCell_arm_GR7,
StorageCell_arm_GR8,
StorageCell_arm_GR9,
StorageCell_arm_GR10,
StorageCell_arm_GR11,
StorageCell_arm_GR12,
StorageCell_arm_GR13,
StorageCell_arm_GR14,
StorageCell_arm_CPSR0,
StorageCell_arm_CPSR1,
StorageCell_arm_CPSR2,
StorageCell_arm_CPSR3,
StorageCell_arm_CPSR4,
StorageCell_arm_CPSR5,
StorageCell_arm_CPSR6,
StorageCell_arm_CPSR7,
StorageCell_arm_CPSR8,
StorageCell_arm_CPSR9,
StorageCell_arm_CPSR10,
StorageCell_arm_CPSR11,
StorageCell_arm_CPSR12,
StorageCell_arm_CPSR13,
StorageCell_arm_CPSR14,
StorageCell_arm_CPSR15,
StorageCell_arm_CPSR16,
StorageCell_arm_CPSR17,
StorageCell_arm_CPSR18,
StorageCell_arm_CPSR19,
StorageCell_arm_CPSR20,
StorageCell_arm_CPSR21,
StorageCell_arm_CPSR22,
StorageCell_arm_CPSR23,
StorageCell_arm_CPSR24,
StorageCell_arm_CPSR25,
StorageCell_arm_CPSR26,
StorageCell_arm_CPSR27,
StorageCell_arm_CPSR28,
StorageCell_arm_CPSR29,
StorageCell_arm_CPSR30,
StorageCell_arm_CPSR31,
StorageCell_arm_SPSR0,
StorageCell_arm_SPSR1,
StorageCell_arm_SPSR2,
StorageCell_arm_SPSR3,
StorageCell_arm_SPSR4,
StorageCell_arm_SPSR5,
StorageCell_arm_SPSR6,
StorageCell_arm_SPSR7,
StorageCell_arm_SPSR8,
StorageCell_arm_SPSR9,
StorageCell_arm_SPSR10,
StorageCell_arm_SPSR11,
StorageCell_arm_SPSR12,
StorageCell_arm_SPSR13,
StorageCell_arm_SPSR14,
StorageCell_arm_SPSR15,
StorageCell_arm_SPSR16,
StorageCell_arm_SPSR17,
StorageCell_arm_SPSR18,
StorageCell_arm_SPSR19,
StorageCell_arm_SPSR20,
StorageCell_arm_SPSR21,
StorageCell_arm_SPSR22,
StorageCell_arm_SPSR23,
StorageCell_arm_SPSR24,
StorageCell_arm_SPSR25,
StorageCell_arm_SPSR26,
StorageCell_arm_SPSR27,
StorageCell_arm_SPSR28,
StorageCell_arm_SPSR29,
StorageCell_arm_SPSR30,
StorageCell_arm_SPSR31,
StorageCell_Volatile,

#undef Storage


#define StorageCell_Control StorageCell_arm_PC



#define StorageCell_Memory StorageCell_arm_MEM


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
  arm/Register.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Register_arm_GR0,


Register_arm_GR1,


Register_arm_GR2,


Register_arm_GR3,


Register_arm_GR4,


Register_arm_GR5,


Register_arm_GR6,


Register_arm_GR7,


Register_arm_GR8,


Register_arm_GR9,


Register_arm_GR10,


Register_arm_GR11,


Register_arm_IP,


Register_arm_SP,


Register_arm_LR,


Register_arm_PC,


Register_arm_PR0,


Register_arm_PR1,


Register_arm_PR2,


Register_arm_PR3,


Register_arm_PR4,


Register_arm_PR5,


Register_arm_CPSR,


Register_arm_SPSR,


Register_arm_CPSRn,


Register_arm_CPSRz,


Register_arm_CPSRc,


Register_arm_CPSRv,


Register_arm_CPSRq,


Register_arm_CPSRi,


Register_arm_CPSRf,


Register_arm_CPSRt,


Register_arm_CPSRnz,


Register_arm_CPSRnzc,


Register_arm_CPSRnzcv,


Register_arm_CPSRcond,


Register_arm_CPSRmode,


Register_arm_CPSRflags,


Register_arm_CPSRstatus,


Register_arm_CPSRext,


Register_arm_CPSRcontrol,


Register_arm_SPSRn,


Register_arm_SPSRz,


Register_arm_SPSRc,


Register_arm_SPSRv,


Register_arm_SPSRq,


Register_arm_SPSRi,


Register_arm_SPSRf,


Register_arm_SPSRt,


Register_arm_SPSRnz,


Register_arm_SPSRnzc,


Register_arm_SPSRnzcv,


Register_arm_SPSRcond,


Register_arm_SPSRmode,


Register_arm_SPSRflags,


Register_arm_SPSRstatus,


Register_arm_SPSRext,


Register_arm_SPSRcontrol,


Register_arm_CPSRj,


Register_arm_CPSRge3,


Register_arm_CPSRge2,


Register_arm_CPSRge1,


Register_arm_CPSRge0,


Register_arm_CPSRe,


Register_arm_CPSRa,


Register_arm_CPSRge,


Register_arm_SPSRj,


Register_arm_SPSRge3,


Register_arm_SPSRge2,


Register_arm_SPSRge1,


Register_arm_SPSRge0,


Register_arm_SPSRe,


Register_arm_SPSRa,


Register_arm_SPSRge,



#undef Register


#define RegisterStorageCells_COUNT_MAX 32



#define RegisterList_COUNT_MAX 74



#define RegisterSet_WORDTYPE uint32_t


#define RegisterSet_WORDBITS (sizeof(RegisterSet_WORDTYPE)*8)


#define RegisterSet_MAJOR(member) ((Register)(member)/(RegisterSet_WORDBITS))


#define RegisterSet_MINOR(member) ((Register)(member)&(RegisterSet_WORDBITS - 1))


#define RegisterSet_WORDCOUNT (RegisterSet_MAJOR(74) + 1)



#define Register_NAMES_COUNT_MAX 2


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
  arm/RegFile.enum --
  Automatically generated from the Machine Description System (MDS).
*/



RegFile_arm_GR,

RegFile_arm_PR,

RegFile_arm_CPSR,

RegFile_arm_SPSR,

RegFile_armv6_CPSR,

RegFile_armv6_SPSR,


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
  arm/RegClass.enum --
  Automatically generated from the Machine Description System (MDS).
*/



RegClass_arm_general,





RegClass_arm_noPC,





RegClass_arm_lr,





RegClass_arm_pc,





RegClass_arm_paired,





RegClass_arm_pairedFirst,





RegClass_arm_pairedSecond,






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
  arm/RegClass.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline OperandEncoded RegClassEncode_arm_general(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); /**/; return VALUE; }





static inline OperandEncoded RegClassEncode_arm_noPC(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); /**/; return VALUE; }





static inline OperandEncoded RegClassEncode_arm_lr(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); /**/; return VALUE; }





static inline OperandEncoded RegClassEncode_arm_pc(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); /**/; return VALUE; }





static inline OperandEncoded RegClassEncode_arm_paired(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 16; return VALUE; }





static inline OperandEncoded RegClassEncode_arm_pairedFirst(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); /**/; return VALUE; }





static inline OperandEncoded RegClassEncode_arm_pairedSecond(OperandDecoded decoded) { OperandEncoded VALUE = decoded - (Register_ + 1); VALUE -= 1; return VALUE; }






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
  arm/RegMask.enum --
  Automatically generated from the Machine Description System (MDS).
*/



RegMask_arm_reglist,





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
  arm/RegMask.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline OperandEncoded RegMaskEncode_arm_reglist(OperandDecoded decoded) { RegisterSet registerSet = (RegisterSet)decoded; RegisterSetWord *WORDS = RegisterSet_WORDS(registerSet); RegisterSetWord WORD = 0; WORD = (WORDS[0]>>0)&65535; return (OperandEncoded)WORD; }





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
  arm/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Modifier_arm_cond,
Modifier_arm_S,





Modifier_arm_cpnum,
Modifier_arm_cpreg,
Modifier_arm_shift,







Modifier_arm_amode4,







Modifier_arm_amode4L,







Modifier_arm_amode4S,







Modifier_arm_psrfld,
#undef Modifier


#define Modifier_NAMES_COUNT_MAX 2


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
  arm/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/



ModifierMember_arm_cond_eq, /**/ ModifierMember_arm_cond_ne, /**/ ModifierMember_arm_cond_cs, ModifierMember_arm_cond_hs = ModifierMember_arm_cond_cs, ModifierMember_arm_cond_cc, ModifierMember_arm_cond_lo = ModifierMember_arm_cond_cc, ModifierMember_arm_cond_mi, /**/ ModifierMember_arm_cond_pl, /**/ ModifierMember_arm_cond_vs, /**/ ModifierMember_arm_cond_vc, /**/ ModifierMember_arm_cond_hi, /**/ ModifierMember_arm_cond_ls, /**/ ModifierMember_arm_cond_ge, /**/ ModifierMember_arm_cond_lt, /**/ ModifierMember_arm_cond_gt, /**/ ModifierMember_arm_cond_le, /**/ ModifierMember_arm_cond_none, ModifierMember_arm_cond_al = ModifierMember_arm_cond_none,
ModifierMember_arm_S_none, /**/ ModifierMember_arm_S_s, /**/





ModifierMember_arm_cpnum_p0, /**/ ModifierMember_arm_cpnum_p1, /**/ ModifierMember_arm_cpnum_p2, /**/ ModifierMember_arm_cpnum_p3, /**/ ModifierMember_arm_cpnum_p4, /**/ ModifierMember_arm_cpnum_p5, /**/ ModifierMember_arm_cpnum_p6, /**/ ModifierMember_arm_cpnum_p7, /**/ ModifierMember_arm_cpnum_p8, /**/ ModifierMember_arm_cpnum_p9, /**/ ModifierMember_arm_cpnum_p10, /**/ ModifierMember_arm_cpnum_p11, /**/ ModifierMember_arm_cpnum_p12, /**/ ModifierMember_arm_cpnum_p13, /**/ ModifierMember_arm_cpnum_p14, /**/ ModifierMember_arm_cpnum_p15, /**/
ModifierMember_arm_cpreg_cr0, /**/ ModifierMember_arm_cpreg_cr1, /**/ ModifierMember_arm_cpreg_cr2, /**/ ModifierMember_arm_cpreg_cr3, /**/ ModifierMember_arm_cpreg_cr4, /**/ ModifierMember_arm_cpreg_cr5, /**/ ModifierMember_arm_cpreg_cr6, /**/ ModifierMember_arm_cpreg_cr7, /**/ ModifierMember_arm_cpreg_cr8, /**/ ModifierMember_arm_cpreg_cr9, /**/ ModifierMember_arm_cpreg_cr10, /**/ ModifierMember_arm_cpreg_cr11, /**/ ModifierMember_arm_cpreg_cr12, /**/ ModifierMember_arm_cpreg_cr13, /**/ ModifierMember_arm_cpreg_cr14, /**/ ModifierMember_arm_cpreg_cr15, /**/
ModifierMember_arm_shift_lsl, /**/ ModifierMember_arm_shift_lsr, /**/ ModifierMember_arm_shift_asr, /**/ ModifierMember_arm_shift_ror, /**/







ModifierMember_arm_amode4_da, /**/ ModifierMember_arm_amode4_ia, /**/ ModifierMember_arm_amode4_db, /**/ ModifierMember_arm_amode4_ib, /**/







ModifierMember_arm_amode4L_da, ModifierMember_arm_amode4L_fa = ModifierMember_arm_amode4L_da, ModifierMember_arm_amode4L_fd, ModifierMember_arm_amode4L_ia = ModifierMember_arm_amode4L_fd, ModifierMember_arm_amode4L_db, ModifierMember_arm_amode4L_ea = ModifierMember_arm_amode4L_db, ModifierMember_arm_amode4L_ed, ModifierMember_arm_amode4L_ib = ModifierMember_arm_amode4L_ed,







ModifierMember_arm_amode4S_da, ModifierMember_arm_amode4S_ed = ModifierMember_arm_amode4S_da, ModifierMember_arm_amode4S_ea, ModifierMember_arm_amode4S_ia = ModifierMember_arm_amode4S_ea, ModifierMember_arm_amode4S_db, ModifierMember_arm_amode4S_fd = ModifierMember_arm_amode4S_db, ModifierMember_arm_amode4S_fa, ModifierMember_arm_amode4S_ib = ModifierMember_arm_amode4S_fa,







ModifierMember_arm_psrfld_c, /**/ ModifierMember_arm_psrfld_x, /**/ ModifierMember_arm_psrfld_xc, /**/ ModifierMember_arm_psrfld_s, /**/ ModifierMember_arm_psrfld_sc, /**/ ModifierMember_arm_psrfld_sx, /**/ ModifierMember_arm_psrfld_sxc, /**/ ModifierMember_arm_psrfld_f, /**/ ModifierMember_arm_psrfld_fc, /**/ ModifierMember_arm_psrfld_fx, /**/ ModifierMember_arm_psrfld_fxc, /**/ ModifierMember_arm_psrfld_fs, /**/ ModifierMember_arm_psrfld_fsc, /**/ ModifierMember_arm_psrfld_fsx, /**/ ModifierMember_arm_psrfld_fsxc, /**/
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
  arm/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline OperandEncoded ModifierEncode_arm_cond(OperandDecoded decoded) { OperandEncoded VALUE = decoded; /**/; return VALUE; }
static inline OperandEncoded ModifierEncode_arm_S(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 15; return VALUE; }





static inline OperandEncoded ModifierEncode_arm_cpnum(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 17; return VALUE; }
static inline OperandEncoded ModifierEncode_arm_cpreg(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 33; return VALUE; }
static inline OperandEncoded ModifierEncode_arm_shift(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 49; return VALUE; }







static inline OperandEncoded ModifierEncode_arm_amode4(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 53; return VALUE; }







static inline OperandEncoded ModifierEncode_arm_amode4L(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 57; return VALUE; }







static inline OperandEncoded ModifierEncode_arm_amode4S(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 61; return VALUE; }







static inline OperandEncoded ModifierEncode_arm_psrfld(OperandDecoded decoded) { OperandEncoded VALUE = decoded; VALUE -= 65; return VALUE; }
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
  arm/Relocation.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Relocation_arm_abs32,



Relocation_arm_btarg1,



Relocation_arm_btarg2,



Relocation_arm_offset12,



Relocation_arm_offset12PC,



Relocation_arm_offset8,



Relocation_arm_offset8PC,




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
  arm/Immediate.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Immediate_arm_gen_s32,



Immediate_arm_btarg1,



Immediate_arm_btarg2,



Immediate_arm_immed8,



Immediate_arm_s9,



Immediate_arm_s13,



Immediate_arm_rotimm,



Immediate_arm_shfimml,



Immediate_arm_shfimmr,



Immediate_arm_offset12,



Immediate_arm_offset12PC,



Immediate_arm_offset8,



Immediate_arm_offset8PC,



Immediate_arm_bkptnum,



Immediate_arm_swinum,



Immediate_arm_offset8CP,



Immediate_arm_option,



Immediate_arm_opcode1d,



Immediate_arm_opcode1m,



Immediate_arm_opcode2,



Immediate_arm_rot,



Immediate_arm_immed5,



Immediate_arm_ssat5,



Immediate_arm_usat5,



Immediate_arm_immed4,



Immediate_arm_ssat4,



Immediate_arm_usat4,




#undef Immediate


#define ImmediateRelocations_COUNT_MAX 1


  Immediate__
#undef Immediate
} enum_Immediate;

/*
 * ImmediateEncode functions.
 */
#define ENCODE(e) e
#define Immediate(NAME,MINVALUE,MAXVALUE,SIGNED,RELOCATIONS,ENCODE,DECODE) static inline OperandEncoded ImmediateEncode_ ##NAME(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { ENCODE; } return (OperandEncoded)VALUE; }
/*
  arm/Immediate.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline OperandEncoded ImmediateEncode_arm_gen_s32(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_btarg1(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value -= 8; __value >>= 2; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_btarg2(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value -= 8; __value >>= 1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_immed8(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<8)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_s9(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_s13(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_rotimm(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value >>= 1; __value &= ((int32_t)1<<4)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_shfimml(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<5)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_shfimmr(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<5)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_offset12(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<12)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_offset12PC(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value -= 8; __value &= ((int32_t)1<<12)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_offset8(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<8)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_offset8PC(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value -= 8; __value &= ((int32_t)1<<8)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_bkptnum(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<16)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_swinum(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<24)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_offset8CP(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value >>= 2; __value &= ((int32_t)1<<8)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_option(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<8)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_opcode1d(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<4)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_opcode1m(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<3)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_opcode2(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<4)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_rot(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value >>= 3; __value &= ((int32_t)1<<2)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_immed5(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<5)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_ssat5(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value -= -1; __value &= ((int32_t)1<<5)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_usat5(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<5)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_immed4(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<4)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_ssat4(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value -= -1; __value &= ((int32_t)1<<4)-1; VALUE = __value; } return (OperandEncoded)VALUE; }



static inline OperandEncoded ImmediateEncode_arm_usat4(OperandDecoded decoded) { ImmediateConstant __constant = (ImmediateConstant)decoded; ImmediateValue VALUE = ImmediateConstant_VALUE(__constant); { int32_t __value = VALUE; __value &= ((int32_t)1<<4)-1; VALUE = __value; } return (OperandEncoded)VALUE; }




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
  arm/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Operand_arm_cond,


Operand_arm_S,


Operand_arm_shift,


Operand_arm_amode4,


Operand_arm_amode4L,


Operand_arm_amode4S,


Operand_arm_fields,


Operand_arm_btarg1,


Operand_arm_btarg2,


Operand_arm_immed8,


Operand_arm_rotateimm,


Operand_arm_shfimml,


Operand_arm_shfimmr,


Operand_arm_offset12,


Operand_arm_offset13,


Operand_arm_offset12PC,


Operand_arm_offset8,


Operand_arm_offset9,


Operand_arm_offset8PC,


Operand_arm_bkptnum,


Operand_arm_swinum,


Operand_arm_rn,


Operand_arm_rnNoPC,


Operand_arm_rdmul,


Operand_arm_rdhi,


Operand_arm_rd,


Operand_arm_rdNoPC,


Operand_arm_rdPC,


Operand_arm_rnmul,


Operand_arm_rdlo,


Operand_arm_rdPaired,


Operand_arm_rs,


Operand_arm_rsNoPC,


Operand_arm_rm,


Operand_arm_rmNoPC,


Operand_arm_reglist,


Operand_arm_reglistNoPC,


Operand_arm_reglistWithPC,


Operand_arm_cpnum,


Operand_arm_crd,


Operand_arm_crn,


Operand_arm_crm,


Operand_arm_offset8CP,


Operand_arm_option,


Operand_arm_opcode1d,


Operand_arm_opcode1m,


Operand_arm_opcode2,


Operand_arm_rot,


Operand_arm_immed5,


Operand_arm_ssat5,


Operand_arm_usat5,


Operand_arm_immed4,


Operand_arm_ssat4,


Operand_arm_usat4,



#undef Operand


#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) 
#undef Operands


#define Operands_COUNT_MAX 7



#define Operands_IMMEDIATE_COUNT_MAX 2



#define Operands_REGMASK_COUNT_MAX 1


  Operand__
#undef Operand
} enum_Operand;

//
typedef enum {
#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) Operands_ ##NAME,

  Operands_,

/*
  arm/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/

#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) 
#undef Operand




Operands_arm_cond_btarg1,


Operands_arm_btarg2,


Operands_arm_cond_rmNoPC,


Operands_arm_cond_rm,


Operands_arm_cond_rdNoPC_rm_shfimmr,


Operands_arm_cond_rdNoPC_rmNoPC_rsNoPC,


Operands_arm_cond_rdNoPC_immed8,


Operands_arm_cond_rdNoPC_rm_shfimml,


Operands_arm_cond_rdNoPC_rm,


Operands_arm_cond_rdNoPC_rm_rotateimm,


Operands_arm_cond_S_rdPC_immed8,


Operands_arm_cond_S_rdPC_rm,


Operands_arm_cond_S_rdPC_rm_shfimml,


Operands_arm_cond_S_rdPC_rm_shfimmr,


Operands_arm_cond_S_rdPC_rm_rotateimm,


Operands_arm_cond_rn_rm_shfimmr,


Operands_arm_cond_rnNoPC_rmNoPC_rsNoPC,


Operands_arm_cond_rn_immed8,


Operands_arm_cond_rn_rm_shfimml,


Operands_arm_cond_rn_rm,


Operands_arm_cond_rn_rm_rotateimm,


Operands_arm_cond_rdNoPC_rn_rm_shfimmr,


Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,


Operands_arm_cond_rdNoPC_rn_immed8,


Operands_arm_cond_rdNoPC_rn_rm_shfimml,


Operands_arm_cond_rdNoPC_rn_rm,


Operands_arm_cond_rdNoPC_rn_rm_rotateimm,


Operands_arm_cond_S_rdPC_rn_immed8,


Operands_arm_cond_S_rdPC_rn_rm,


Operands_arm_cond_S_rdPC_rn_rm_shfimml,


Operands_arm_cond_S_rdPC_rn_rm_shfimmr,


Operands_arm_cond_S_rdPC_rn_rm_rotateimm,


Operands_arm_cond_rdmul_rmNoPC_rsNoPC_rnmul,


Operands_arm_cond_rdmul_rmNoPC_rsNoPC,


Operands_arm_cond_rdlo_rdhi_rmNoPC_rsNoPC,


Operands_arm_cond_rdNoPC_rmNoPC,


Operands_arm_cond_rdNoPC,


Operands_arm_cond_fields_immed8,


Operands_arm_cond_immed8,


Operands_arm_cond_fields_rm,


Operands_arm_cond_rdNoPC_rn_offset13,


Operands_arm_cond_rdNoPC_rn_offset12,


Operands_arm_cond_rdNoPC_rn_rmNoPC,


Operands_arm_cond_rdNoPC_rn_rmNoPC_shfimmr,


Operands_arm_cond_rdNoPC_rn_rmNoPC_shfimml,


Operands_arm_cond_rdNoPC_rn_rmNoPC_rotateimm,


Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC,


Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,


Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,


Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,


Operands_arm_cond_rdNoPC_rnNoPC_offset13,


Operands_arm_cond_rdNoPC_rnNoPC_offset12,


Operands_arm_cond_rdNoPC_offset12PC,


Operands_arm_cond_rdPC_rn_offset13,


Operands_arm_cond_rdPC_rn_offset12,


Operands_arm_cond_rdPC_rn_rmNoPC,


Operands_arm_cond_rdPC_rn_rmNoPC_shfimmr,


Operands_arm_cond_rdPC_rn_rmNoPC_shfimml,


Operands_arm_cond_rdPC_rn_rmNoPC_rotateimm,


Operands_arm_cond_rdPC_rnNoPC_rmNoPC,


Operands_arm_cond_rdPC_rnNoPC_rmNoPC_shfimmr,


Operands_arm_cond_rdPC_rnNoPC_rmNoPC_shfimml,


Operands_arm_cond_rdPC_rnNoPC_rmNoPC_rotateimm,


Operands_arm_cond_rdPC_rnNoPC_offset13,


Operands_arm_cond_rdPC_rnNoPC_offset12,


Operands_arm_cond_rdPC_offset12PC,


Operands_arm_cond_rdNoPC_rn_offset9,


Operands_arm_cond_rdNoPC_rn_offset8,


Operands_arm_cond_rdNoPC_rnNoPC_offset9,


Operands_arm_cond_rdNoPC_rnNoPC_offset8,


Operands_arm_cond_rdNoPC_offset8PC,


Operands_arm_cond_amode4L_rnNoPC_reglistNoPC,


Operands_arm_cond_amode4L_rnNoPC_reglistWithPC,


Operands_arm_cond_amode4S_rnNoPC_reglist,


Operands_arm_cond_rdNoPC_rmNoPC_rnNoPC,


Operands_arm_bkptnum,


Operands_arm_cond_swinum,


Operands_arm_cond_cpnum_opcode1d_crd_crn_crm_opcode2,


Operands_arm_cpnum_opcode1d_crd_crn_crm_opcode2,


Operands_arm_cond_cpnum_crd_rn_offset8CP,


Operands_arm_cond_cpnum_crd_rnNoPC_offset8CP,


Operands_arm_cond_cpnum_crd_rn_option,


Operands_arm_cpnum_crd_rn_offset8CP,


Operands_arm_cpnum_crd_rnNoPC_offset8CP,


Operands_arm_cpnum_crd_rn_option,


Operands_arm_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,


Operands_arm_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,


Operands_arm_cond_cpnum_opcode1m_crn_crm_opcode2,


Operands_arm_cpnum_opcode1m_crn_crm_opcode2,


Operands_arm_cond_rdPaired_rn_offset9,


Operands_arm_cond_rdPaired_rn_offset8,


Operands_arm_cond_rdPaired_rn_rmNoPC,


Operands_arm_cond_rdPaired_rnNoPC_rmNoPC,


Operands_arm_cond_rdPaired_rnNoPC_offset9,


Operands_arm_cond_rdPaired_rnNoPC_offset8,


Operands_arm_cond_rdPaired_offset8PC,


Operands_arm_rn_offset13,


Operands_arm_rn_offset12,


Operands_arm_offset12PC,


Operands_arm_rn_rmNoPC,


Operands_arm_rn_rmNoPC_shfimmr,


Operands_arm_rn_rmNoPC_shfimml,


Operands_arm_rn_rmNoPC_rotateimm,


Operands_arm_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm,


Operands_arm_cond_rdNoPC_rn,


Operands_arm_cond_rdPC_rn,


Operands_arm_cond_rdNoPC_rnNoPC,


Operands_arm_rn,


Operands_arm_cond_cpnum_opcode1m_rdNoPC_crn_crm,


Operands_arm_cpnum_opcode1m_rdNoPC_crn_crm,


Operands_arm_rdNoPC_rn,


Operands_arm_rdPC_rn,


Operands_arm_rdNoPC_rnNoPC,


Operands_arm_btarg1,


Operands_arm_rmNoPC,


Operands_arm_rm,


Operands_arm_rdNoPC_rm_shfimmr,


Operands_arm_rdNoPC_rmNoPC_rsNoPC,


Operands_arm_rdNoPC_immed8,


Operands_arm_rdNoPC_rm_shfimml,


Operands_arm_rdNoPC_rm,


Operands_arm_rdNoPC_rm_rotateimm,


Operands_arm_S_rdPC_immed8,


Operands_arm_S_rdPC_rm,


Operands_arm_S_rdPC_rm_shfimml,


Operands_arm_S_rdPC_rm_shfimmr,


Operands_arm_S_rdPC_rm_rotateimm,


Operands_arm_rn_rm_shfimmr,


Operands_arm_rnNoPC_rmNoPC_rsNoPC,


Operands_arm_rn_immed8,


Operands_arm_rn_rm_shfimml,


Operands_arm_rn_rm,


Operands_arm_rn_rm_rotateimm,


Operands_arm_rdNoPC_rn_rm_shfimmr,


Operands_arm_rdNoPC_rnNoPC_rmNoPC_rsNoPC,


Operands_arm_rdNoPC_rn_immed8,


Operands_arm_rdNoPC_rn_rm_shfimml,


Operands_arm_rdNoPC_rn_rm,


Operands_arm_rdNoPC_rn_rm_rotateimm,


Operands_arm_S_rdPC_rn_immed8,


Operands_arm_S_rdPC_rn_rm,


Operands_arm_S_rdPC_rn_rm_shfimml,


Operands_arm_S_rdPC_rn_rm_shfimmr,


Operands_arm_S_rdPC_rn_rm_rotateimm,


Operands_arm_rdmul_rmNoPC_rsNoPC_rnmul,


Operands_arm_rdmul_rmNoPC_rsNoPC,


Operands_arm_rdlo_rdhi_rmNoPC_rsNoPC,


Operands_arm_rdNoPC_rmNoPC,


Operands_arm_rdNoPC,


Operands_arm_fields_immed8,


Operands_arm_immed8,


Operands_arm_fields_rm,


Operands_arm_rdNoPC_rn_offset13,


Operands_arm_rdNoPC_rn_offset12,


Operands_arm_rdNoPC_rn_rmNoPC,


Operands_arm_rdNoPC_rn_rmNoPC_shfimmr,


Operands_arm_rdNoPC_rn_rmNoPC_shfimml,


Operands_arm_rdNoPC_rn_rmNoPC_rotateimm,


Operands_arm_rdNoPC_rnNoPC_rmNoPC,


Operands_arm_rdNoPC_rnNoPC_rmNoPC_shfimmr,


Operands_arm_rdNoPC_rnNoPC_rmNoPC_shfimml,


Operands_arm_rdNoPC_rnNoPC_rmNoPC_rotateimm,


Operands_arm_rdNoPC_rnNoPC_offset13,


Operands_arm_rdNoPC_rnNoPC_offset12,


Operands_arm_rdNoPC_offset12PC,


Operands_arm_rdPC_rn_offset13,


Operands_arm_rdPC_rn_offset12,


Operands_arm_rdPC_rn_rmNoPC,


Operands_arm_rdPC_rn_rmNoPC_shfimmr,


Operands_arm_rdPC_rn_rmNoPC_shfimml,


Operands_arm_rdPC_rn_rmNoPC_rotateimm,


Operands_arm_rdPC_rnNoPC_rmNoPC,


Operands_arm_rdPC_rnNoPC_rmNoPC_shfimmr,


Operands_arm_rdPC_rnNoPC_rmNoPC_shfimml,


Operands_arm_rdPC_rnNoPC_rmNoPC_rotateimm,


Operands_arm_rdPC_rnNoPC_offset13,


Operands_arm_rdPC_rnNoPC_offset12,


Operands_arm_rdPC_offset12PC,


Operands_arm_rdNoPC_rn_offset9,


Operands_arm_rdNoPC_rn_offset8,


Operands_arm_rdNoPC_rnNoPC_offset9,


Operands_arm_rdNoPC_rnNoPC_offset8,


Operands_arm_rdNoPC_offset8PC,


Operands_arm_amode4L_rnNoPC_reglistNoPC,


Operands_arm_amode4L_rnNoPC_reglistWithPC,


Operands_arm_amode4S_rnNoPC_reglist,


Operands_arm_rdNoPC_rmNoPC_rnNoPC,


Operands_arm_swinum,


Operands_arm_rdPaired_rn_offset9,


Operands_arm_rdPaired_rn_offset8,


Operands_arm_rdPaired_rn_rmNoPC,


Operands_arm_rdPaired_rnNoPC_rmNoPC,


Operands_arm_rdPaired_rnNoPC_offset9,


Operands_arm_rdPaired_rnNoPC_offset8,


Operands_arm_rdPaired_offset8PC,


Operands_arm_cpnum_opcode2_rdNoPC_rnNoPC_crm,


Operands_arm_cond_rdNoPC_rmNoPC_rot,


Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_rot,


Operands_arm_cond_rdNoPC_ssat5_rmNoPC_shfimmr,


Operands_arm_cond_rdNoPC_ssat5_rmNoPC_shfimml,


Operands_arm_cond_rdNoPC_ssat5_rmNoPC,


Operands_arm_cond_rdNoPC_usat5_rmNoPC_shfimmr,


Operands_arm_cond_rdNoPC_usat5_rmNoPC_shfimml,


Operands_arm_cond_rdNoPC_usat5_rmNoPC,


Operands_arm_cond_rdNoPC_ssat4_rmNoPC,


Operands_arm_cond_rdNoPC_usat4_rmNoPC,


Operands_arm_rdNoPC_rmNoPC_rot,


Operands_arm_rdNoPC_rnNoPC_rmNoPC_rot,


Operands_arm_rdNoPC_ssat5_rmNoPC_shfimmr,


Operands_arm_rdNoPC_ssat5_rmNoPC_shfimml,


Operands_arm_rdNoPC_ssat5_rmNoPC,


Operands_arm_rdNoPC_usat5_rmNoPC_shfimmr,


Operands_arm_rdNoPC_usat5_rmNoPC_shfimml,


Operands_arm_rdNoPC_usat5_rmNoPC,


Operands_arm_rdNoPC_ssat4_rmNoPC,


Operands_arm_rdNoPC_usat4_rmNoPC,



#undef Operands
  Operands__
#undef Operand
} enum_Operands;

/*
  Decode_Operand function prototypes.
 */
#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) void Decode_Operand_ ##NAME(const void *opcodes, void *restrict result);



/*
  arm/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/



void Decode_Operand_arm_cond(const void *opcodes, void *restrict result);


void Decode_Operand_arm_S(const void *opcodes, void *restrict result);


void Decode_Operand_arm_shift(const void *opcodes, void *restrict result);


void Decode_Operand_arm_amode4(const void *opcodes, void *restrict result);


void Decode_Operand_arm_amode4L(const void *opcodes, void *restrict result);


void Decode_Operand_arm_amode4S(const void *opcodes, void *restrict result);


void Decode_Operand_arm_fields(const void *opcodes, void *restrict result);


void Decode_Operand_arm_btarg1(const void *opcodes, void *restrict result);


void Decode_Operand_arm_btarg2(const void *opcodes, void *restrict result);


void Decode_Operand_arm_immed8(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rotateimm(const void *opcodes, void *restrict result);


void Decode_Operand_arm_shfimml(const void *opcodes, void *restrict result);


void Decode_Operand_arm_shfimmr(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset12(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset13(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset12PC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset8(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset9(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset8PC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_bkptnum(const void *opcodes, void *restrict result);


void Decode_Operand_arm_swinum(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rn(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rnNoPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rdmul(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rdhi(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rd(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rdNoPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rdPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rnmul(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rdlo(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rdPaired(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rs(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rsNoPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rm(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rmNoPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_reglist(const void *opcodes, void *restrict result);


void Decode_Operand_arm_reglistNoPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_reglistWithPC(const void *opcodes, void *restrict result);


void Decode_Operand_arm_cpnum(const void *opcodes, void *restrict result);


void Decode_Operand_arm_crd(const void *opcodes, void *restrict result);


void Decode_Operand_arm_crn(const void *opcodes, void *restrict result);


void Decode_Operand_arm_crm(const void *opcodes, void *restrict result);


void Decode_Operand_arm_offset8CP(const void *opcodes, void *restrict result);


void Decode_Operand_arm_option(const void *opcodes, void *restrict result);


void Decode_Operand_arm_opcode1d(const void *opcodes, void *restrict result);


void Decode_Operand_arm_opcode1m(const void *opcodes, void *restrict result);


void Decode_Operand_arm_opcode2(const void *opcodes, void *restrict result);


void Decode_Operand_arm_rot(const void *opcodes, void *restrict result);


void Decode_Operand_arm_immed5(const void *opcodes, void *restrict result);


void Decode_Operand_arm_ssat5(const void *opcodes, void *restrict result);


void Decode_Operand_arm_usat5(const void *opcodes, void *restrict result);


void Decode_Operand_arm_immed4(const void *opcodes, void *restrict result);


void Decode_Operand_arm_ssat4(const void *opcodes, void *restrict result);


void Decode_Operand_arm_usat4(const void *opcodes, void *restrict result);



#undef Operand


#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) 
#undef Operands
/*
 * OperandsEncode function prototypes.
 */
#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) void OperandsEncode_ ##NAME(const_OperandsBuffer buffer, void *restrict opcodes);



/*
  arm/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/

#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) 
#undef Operand




void OperandsEncode_arm_cond_btarg1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_btarg2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rn_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rnNoPC_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rn_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rn_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rn_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rn_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rn_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rn_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rn_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rn_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_S_rdPC_rn_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdmul_rmNoPC_rsNoPC_rnmul(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdmul_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdlo_rdhi_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_fields_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_fields_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_offset12PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rnNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rnNoPC_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rnNoPC_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rnNoPC_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rnNoPC_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rnNoPC_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_offset12PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_offset8PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_amode4L_rnNoPC_reglistNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_amode4L_rnNoPC_reglistWithPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_amode4S_rnNoPC_reglist(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rmNoPC_rnNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_bkptnum(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_swinum(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_opcode1d_crd_crn_crm_opcode2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_opcode1d_crd_crn_crm_opcode2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_crd_rn_offset8CP(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_crd_rnNoPC_offset8CP(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_crd_rn_option(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_crd_rn_offset8CP(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_crd_rnNoPC_offset8CP(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_crd_rn_option(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_opcode1m_rdNoPC_crn_crm_opcode2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_opcode1m_crn_crm_opcode2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_opcode1m_crn_crm_opcode2(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_rn_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_rn_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_rnNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_rnNoPC_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_rnNoPC_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPaired_offset8PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_offset12PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rn(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdPC_rn(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_cpnum_opcode1m_rdNoPC_crn_crm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_opcode1m_rdNoPC_crn_crm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_btarg1(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rnNoPC_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rn_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rn_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rn_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rn_rm_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rn_rm_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_S_rdPC_rn_rm_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdmul_rmNoPC_rsNoPC_rnmul(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdmul_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdlo_rdhi_rmNoPC_rsNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_fields_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_immed8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_fields_rm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_offset12PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rn_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rnNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rnNoPC_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rnNoPC_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rnNoPC_rmNoPC_rotateimm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rnNoPC_offset13(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_rnNoPC_offset12(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPC_offset12PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rn_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_offset8PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_amode4L_rnNoPC_reglistNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_amode4L_rnNoPC_reglistWithPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_amode4S_rnNoPC_reglist(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rmNoPC_rnNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_swinum(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_rn_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_rn_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_rn_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_rnNoPC_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_rnNoPC_offset9(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_rnNoPC_offset8(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdPaired_offset8PC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cpnum_opcode2_rdNoPC_rnNoPC_crm(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rmNoPC_rot(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_rnNoPC_rmNoPC_rot(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_ssat5_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_ssat5_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_ssat5_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_usat5_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_usat5_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_usat5_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_ssat4_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_cond_rdNoPC_usat4_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rmNoPC_rot(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_rnNoPC_rmNoPC_rot(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_ssat5_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_ssat5_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_ssat5_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_usat5_rmNoPC_shfimmr(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_usat5_rmNoPC_shfimml(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_usat5_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_ssat4_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);


void OperandsEncode_arm_rdNoPC_usat4_rmNoPC(const_OperandsBuffer buffer, void *restrict opcodes);



#undef Operands
void
OperandsEncode_(const_OperandsBuffer buffer, void *restrict opcodes);

/*
 * Decode_Operands function prototypes.
 */

#define Operands(NAME,OPERANDS,RELOCATABLE,ENCODE,DECODE) void Decode_Operands_ ##NAME(const void *opcodes, OperandsBuffer buffer);



/*
  arm/Operand.enum --
  Automatically generated from the Machine Description System (MDS).
*/

#define Operand(NAME,METHOD,WORDTYPE,ENCODE,DECODE) 
#undef Operand




void Decode_Operands_arm_cond_btarg1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_btarg2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rn_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rnNoPC_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rn_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rn_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rn_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rn_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rn_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rn_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rn_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rn_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_S_rdPC_rn_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdmul_rmNoPC_rsNoPC_rnmul(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdmul_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdlo_rdhi_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_fields_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_fields_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_offset12PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rnNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rnNoPC_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rnNoPC_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rnNoPC_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rnNoPC_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rnNoPC_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_offset12PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_offset8PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_amode4L_rnNoPC_reglistNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_amode4L_rnNoPC_reglistWithPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_amode4S_rnNoPC_reglist(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rmNoPC_rnNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_bkptnum(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_swinum(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_opcode1d_crd_crn_crm_opcode2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_opcode1d_crd_crn_crm_opcode2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_crd_rn_offset8CP(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_crd_rnNoPC_offset8CP(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_crd_rn_option(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_crd_rn_offset8CP(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_crd_rnNoPC_offset8CP(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_crd_rn_option(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_opcode1m_rdNoPC_crn_crm_opcode2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_opcode1m_crn_crm_opcode2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_opcode1m_crn_crm_opcode2(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_rn_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_rn_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_rnNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_rnNoPC_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_rnNoPC_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPaired_offset8PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_offset12PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rn(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdPC_rn(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_cpnum_opcode1m_rdNoPC_crn_crm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_opcode1m_rdNoPC_crn_crm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_btarg1(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rnNoPC_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rn_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rn_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rn_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rn_rm_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rn_rm_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_S_rdPC_rn_rm_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdmul_rmNoPC_rsNoPC_rnmul(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdmul_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdlo_rdhi_rmNoPC_rsNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_fields_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_immed8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_fields_rm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_offset12PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rn_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rnNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rnNoPC_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rnNoPC_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rnNoPC_rmNoPC_rotateimm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rnNoPC_offset13(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_rnNoPC_offset12(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPC_offset12PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rn_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_offset8PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_amode4L_rnNoPC_reglistNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_amode4L_rnNoPC_reglistWithPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_amode4S_rnNoPC_reglist(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rmNoPC_rnNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_swinum(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_rn_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_rn_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_rn_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_rnNoPC_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_rnNoPC_offset9(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_rnNoPC_offset8(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdPaired_offset8PC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cpnum_opcode2_rdNoPC_rnNoPC_crm(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rmNoPC_rot(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_rnNoPC_rmNoPC_rot(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_ssat5_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_ssat5_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_ssat5_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_usat5_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_usat5_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_usat5_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_ssat4_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_cond_rdNoPC_usat4_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rmNoPC_rot(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_rnNoPC_rmNoPC_rot(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_ssat5_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_ssat5_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_ssat5_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_usat5_rmNoPC_shfimmr(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_usat5_rmNoPC_shfimml(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_usat5_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_ssat4_rmNoPC(const void *opcodes, OperandsBuffer buffer);


void Decode_Operands_arm_rdNoPC_usat4_rmNoPC(const void *opcodes, OperandsBuffer buffer);



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
  arm/Format.enum --
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
  arm/Syntax.enum --
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
  arm/Instance.enum --
  Automatically generated from the Machine Description System (MDS).
*/




Instance_armv5e_b_cond_btarg1,
Instance_armv5e_bl_cond_btarg1,
Instance_armv5e_blxi_btarg2,
Instance_armv5e_blx_cond_rmNoPC,
Instance_armv5e_bx_cond_rm,
Instance_armv5e_movs_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_movs_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movs_imm_cond_rdNoPC_immed8,
Instance_armv5e_movs_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv5e_movs_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movs_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_movs_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movs_reg_cond_rdNoPC_rm,
Instance_armv5e_movs_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv5e_movs_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movs_rrx_cond_rdNoPC_rm,
Instance_armv5e_mov_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_mov_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mov_imm_cond_rdNoPC_immed8,
Instance_armv5e_mov_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv5e_mov_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mov_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_mov_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mov_reg_cond_rdNoPC_rm,
Instance_armv5e_mov_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv5e_mov_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mov_rrx_cond_rdNoPC_rm,
Instance_armv5e_mov_pc_imm_cond_S_rdPC_immed8,
Instance_armv5e_mov_pc_reg_cond_S_rdPC_rm,
Instance_armv5e_mov_pc_lsl_imm_cond_S_rdPC_rm_shfimml,
Instance_armv5e_mov_pc_lsr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv5e_mov_pc_asr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv5e_mov_pc_ror_imm_cond_S_rdPC_rm_rotateimm,
Instance_armv5e_mov_pc_rrx_cond_S_rdPC_rm,
Instance_armv5e_mvns_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_mvns_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvns_imm_cond_rdNoPC_immed8,
Instance_armv5e_mvns_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv5e_mvns_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvns_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_mvns_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvns_reg_cond_rdNoPC_rm,
Instance_armv5e_mvns_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv5e_mvns_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvns_rrx_cond_rdNoPC_rm,
Instance_armv5e_mvn_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_mvn_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvn_imm_cond_rdNoPC_immed8,
Instance_armv5e_mvn_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv5e_mvn_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvn_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv5e_mvn_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvn_reg_cond_rdNoPC_rm,
Instance_armv5e_mvn_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv5e_mvn_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvn_rrx_cond_rdNoPC_rm,
Instance_armv5e_mvn_pc_imm_cond_S_rdPC_immed8,
Instance_armv5e_mvn_pc_reg_cond_S_rdPC_rm,
Instance_armv5e_mvn_pc_lsl_imm_cond_S_rdPC_rm_shfimml,
Instance_armv5e_mvn_pc_lsr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv5e_mvn_pc_asr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv5e_mvn_pc_ror_imm_cond_S_rdPC_rm_rotateimm,
Instance_armv5e_mvn_pc_rrx_cond_S_rdPC_rm,
Instance_armv5e_cmp_asr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_cmp_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmp_imm_cond_rn_immed8,
Instance_armv5e_cmp_lsl_imm_cond_rn_rm_shfimml,
Instance_armv5e_cmp_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmp_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_cmp_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmp_reg_cond_rn_rm,
Instance_armv5e_cmp_ror_imm_cond_rn_rm_rotateimm,
Instance_armv5e_cmp_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmp_rrx_cond_rn_rm,
Instance_armv5e_cmn_asr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_cmn_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmn_imm_cond_rn_immed8,
Instance_armv5e_cmn_lsl_imm_cond_rn_rm_shfimml,
Instance_armv5e_cmn_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmn_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_cmn_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmn_reg_cond_rn_rm,
Instance_armv5e_cmn_ror_imm_cond_rn_rm_rotateimm,
Instance_armv5e_cmn_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmn_rrx_cond_rn_rm,
Instance_armv5e_tst_asr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_tst_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tst_imm_cond_rn_immed8,
Instance_armv5e_tst_lsl_imm_cond_rn_rm_shfimml,
Instance_armv5e_tst_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tst_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_tst_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tst_reg_cond_rn_rm,
Instance_armv5e_tst_ror_imm_cond_rn_rm_rotateimm,
Instance_armv5e_tst_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tst_rrx_cond_rn_rm,
Instance_armv5e_teq_asr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_teq_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teq_imm_cond_rn_immed8,
Instance_armv5e_teq_lsl_imm_cond_rn_rm_shfimml,
Instance_armv5e_teq_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teq_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv5e_teq_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teq_reg_cond_rn_rm,
Instance_armv5e_teq_ror_imm_cond_rn_rm_rotateimm,
Instance_armv5e_teq_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teq_rrx_cond_rn_rm,
Instance_armv5e_adds_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adds_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adds_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_adds_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_adds_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adds_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adds_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adds_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_adds_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_adds_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adds_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_add_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_add_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_add_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_add_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_add_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_add_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_add_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_add_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_add_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_add_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_add_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_add_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_add_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_add_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_add_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_add_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_add_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_add_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_subs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_subs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subs_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_subs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_subs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_subs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subs_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_subs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_subs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subs_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_sub_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sub_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sub_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_sub_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_sub_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sub_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sub_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sub_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_sub_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_sub_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sub_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_sub_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_sub_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_sub_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_sub_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_sub_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_sub_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_sub_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_rsbs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsbs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbs_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_rsbs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rsbs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsbs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbs_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_rsbs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rsbs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbs_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_rsb_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsb_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsb_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_rsb_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rsb_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsb_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsb_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsb_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_rsb_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rsb_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsb_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_rsb_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_rsb_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_rsb_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_rsb_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rsb_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rsb_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_rsb_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_adcs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adcs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcs_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_adcs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_adcs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adcs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcs_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_adcs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_adcs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcs_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_adc_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adc_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adc_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_adc_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_adc_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adc_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adc_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adc_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_adc_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_adc_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adc_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_adc_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_adc_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_adc_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_adc_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_adc_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_adc_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_adc_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_sbcs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbcs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcs_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_sbcs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_sbcs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbcs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcs_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_sbcs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_sbcs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcs_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_sbc_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbc_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbc_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_sbc_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_sbc_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbc_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbc_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbc_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_sbc_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_sbc_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbc_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_sbc_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_sbc_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_sbc_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_sbc_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_sbc_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_sbc_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_sbc_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_rscs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rscs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscs_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_rscs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rscs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rscs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscs_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_rscs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rscs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscs_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_rsc_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsc_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsc_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_rsc_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rsc_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsc_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsc_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsc_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_rsc_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rsc_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsc_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_rsc_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_rsc_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_rsc_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_rsc_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rsc_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rsc_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_rsc_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_ands_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_ands_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_ands_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_ands_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_ands_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_ands_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_ands_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_ands_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_ands_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_ands_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_ands_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_and_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_and_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_and_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_and_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_and_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_and_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_and_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_and_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_and_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_and_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_and_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_and_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_and_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_and_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_and_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_and_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_and_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_and_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_bics_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bics_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bics_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_bics_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_bics_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bics_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bics_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bics_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_bics_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_bics_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bics_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_bic_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bic_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bic_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_bic_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_bic_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bic_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bic_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bic_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_bic_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_bic_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bic_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_bic_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_bic_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_bic_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_bic_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_bic_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_bic_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_bic_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_eors_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eors_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eors_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_eors_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_eors_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eors_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eors_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eors_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_eors_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_eors_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eors_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_eor_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eor_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eor_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_eor_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_eor_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eor_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eor_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eor_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_eor_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_eor_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eor_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_eor_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_eor_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_eor_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_eor_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_eor_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_eor_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_eor_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_orrs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orrs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrs_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_orrs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_orrs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orrs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrs_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_orrs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_orrs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrs_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_orr_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orr_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orr_imm_cond_rdNoPC_rn_immed8,
Instance_armv5e_orr_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv5e_orr_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orr_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orr_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orr_reg_cond_rdNoPC_rn_rm,
Instance_armv5e_orr_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_orr_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orr_rrx_cond_rdNoPC_rn_rm,
Instance_armv5e_orr_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv5e_orr_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv5e_orr_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv5e_orr_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_orr_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_orr_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_orr_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv5e_mlas_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_mla_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_muls_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_mul_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smlals_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlal_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smulls_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smull_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umlals_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umlal_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umulls_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umull_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_clz_cond_rdNoPC_rmNoPC,
Instance_armv5e_mrs_cpsr_cond_rdNoPC,
Instance_armv5e_mrs_spsr_cond_rdNoPC,
Instance_armv5e_msr_cpsr_imm_cond_fields_immed8,
Instance_armv5e_msr_cpsr_imm_f_cond_immed8,
Instance_armv5e_msr_cpsr_reg_cond_fields_rm,
Instance_armv5e_msr_cpsr_reg_f_cond_rm,
Instance_armv5e_msr_spsr_imm_cond_fields_immed8,
Instance_armv5e_msr_spsr_reg_cond_fields_rm,
Instance_armv5e_ldr_addi_cond_rdNoPC_rn_offset13,
Instance_armv5e_ldr_subi_cond_rdNoPC_rn_offset12,
Instance_armv5e_ldr_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldr_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldr_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldr_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldr_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldr_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldr_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldr_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldr_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldr_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldr_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldr_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldr_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldr_addpc_cond_rdNoPC_offset12PC,
Instance_armv5e_ldr_subpc_cond_rdNoPC_offset12PC,
Instance_armv5e_ldr_pc_addi_cond_rdPC_rn_offset13,
Instance_armv5e_ldr_pc_subi_cond_rdPC_rn_offset12,
Instance_armv5e_ldr_pc_add_cond_rdPC_rn_rmNoPC,
Instance_armv5e_ldr_pc_add_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_add_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldr_pc_add_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_add_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldr_pc_add_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv5e_ldr_pc_addsh2_cond_rdPC_rn_rmNoPC,
Instance_armv5e_ldr_pc_sub_cond_rdPC_rn_rmNoPC,
Instance_armv5e_ldr_pc_sub_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_sub_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldr_pc_sub_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_sub_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldr_pc_sub_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv5e_ldr_pc_post_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_post_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_post_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_pc_post_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_post_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_pc_post_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_post_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv5e_ldr_pc_post_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_post_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_post_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_post_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_pc_post_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_post_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_pc_post_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_post_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv5e_ldr_pc_pre_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_pre_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_pre_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_pc_pre_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_pre_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_pc_pre_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_pre_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv5e_ldr_pc_pre_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_pre_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_pre_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_pre_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldr_pc_pre_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldr_pc_pre_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldr_pc_pre_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldr_pc_pre_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv5e_ldr_pc_addpc_cond_rdPC_offset12PC,
Instance_armv5e_ldr_pc_subpc_cond_rdPC_offset12PC,
Instance_armv5e_ldrb_addi_cond_rdNoPC_rn_offset13,
Instance_armv5e_ldrb_subi_cond_rdNoPC_rn_offset12,
Instance_armv5e_ldrb_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrb_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldrb_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldrb_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldrb_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldrb_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrb_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrb_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrb_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldrb_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldrb_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldrb_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldrb_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrb_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrb_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrb_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldrb_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrb_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrb_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldrb_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrb_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrb_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldrb_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrb_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrb_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrb_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrb_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldrb_addpc_cond_rdNoPC_offset12PC,
Instance_armv5e_ldrb_subpc_cond_rdNoPC_offset12PC,
Instance_armv5e_str_addi_cond_rdNoPC_rn_offset13,
Instance_armv5e_str_subi_cond_rdNoPC_rn_offset12,
Instance_armv5e_str_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_str_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_str_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_str_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_str_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_str_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_str_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_str_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_str_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_str_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_str_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_str_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_str_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_str_addpc_cond_rdNoPC_offset12PC,
Instance_armv5e_str_subpc_cond_rdNoPC_offset12PC,
Instance_armv5e_str_pc_addi_cond_rdPC_rn_offset13,
Instance_armv5e_str_pc_subi_cond_rdPC_rn_offset12,
Instance_armv5e_str_pc_add_cond_rdPC_rn_rmNoPC,
Instance_armv5e_str_pc_add_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_pc_add_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_str_pc_add_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_pc_add_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_str_pc_add_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv5e_str_pc_addsh2_cond_rdPC_rn_rmNoPC,
Instance_armv5e_str_pc_sub_cond_rdPC_rn_rmNoPC,
Instance_armv5e_str_pc_sub_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_pc_sub_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_str_pc_sub_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_str_pc_sub_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_str_pc_sub_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv5e_str_pc_post_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_post_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_post_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_pc_post_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_post_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_pc_post_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_post_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv5e_str_pc_post_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_post_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_post_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_post_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_pc_post_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_post_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_pc_post_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_post_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv5e_str_pc_pre_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_pre_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_pre_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_pc_pre_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_pre_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_pc_pre_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_pre_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv5e_str_pc_pre_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_pre_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_pre_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_pre_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_str_pc_pre_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_str_pc_pre_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_str_pc_pre_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_str_pc_pre_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv5e_str_pc_addpc_cond_rdPC_offset12PC,
Instance_armv5e_str_pc_subpc_cond_rdPC_offset12PC,
Instance_armv5e_strb_addi_cond_rdNoPC_rn_offset13,
Instance_armv5e_strb_subi_cond_rdNoPC_rn_offset12,
Instance_armv5e_strb_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strb_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_strb_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_strb_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_strb_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_strb_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strb_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strb_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strb_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_strb_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_strb_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_strb_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_strb_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strb_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strb_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strb_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_strb_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strb_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strb_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_strb_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strb_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strb_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_strb_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strb_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strb_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strb_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strb_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_strb_addpc_cond_rdNoPC_offset12PC,
Instance_armv5e_strb_subpc_cond_rdNoPC_offset12PC,
Instance_armv5e_ldrbt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrbt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrbt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrbt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrbt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrbt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrbt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldrbt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrbt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrbt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrbt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrbt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrbt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrbt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrbt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldrt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldrt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldrt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldrt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldrt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_strbt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strbt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strbt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strbt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strbt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strbt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strbt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_strbt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strbt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strbt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strbt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strbt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strbt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strbt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strbt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_strt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv5e_strt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_strt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_strt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_strt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldrh_addi_cond_rdNoPC_rn_offset9,
Instance_armv5e_ldrh_subi_cond_rdNoPC_rn_offset8,
Instance_armv5e_ldrh_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrh_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrh_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrh_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldrh_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrh_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldrh_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrh_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldrh_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrh_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldrh_addpc_cond_rdNoPC_offset8PC,
Instance_armv5e_ldrh_subpc_cond_rdNoPC_offset8PC,
Instance_armv5e_ldrsb_addi_cond_rdNoPC_rn_offset9,
Instance_armv5e_ldrsb_subi_cond_rdNoPC_rn_offset8,
Instance_armv5e_ldrsb_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrsb_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrsb_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsb_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldrsb_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsb_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldrsb_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsb_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldrsb_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsb_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldrsb_addpc_cond_rdNoPC_offset8PC,
Instance_armv5e_ldrsb_subpc_cond_rdNoPC_offset8PC,
Instance_armv5e_ldrsh_addi_cond_rdNoPC_rn_offset9,
Instance_armv5e_ldrsh_subi_cond_rdNoPC_rn_offset8,
Instance_armv5e_ldrsh_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrsh_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldrsh_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsh_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldrsh_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsh_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldrsh_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsh_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldrsh_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldrsh_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldrsh_addpc_cond_rdNoPC_offset8PC,
Instance_armv5e_ldrsh_subpc_cond_rdNoPC_offset8PC,
Instance_armv5e_strh_addi_cond_rdNoPC_rn_offset9,
Instance_armv5e_strh_subi_cond_rdNoPC_rn_offset8,
Instance_armv5e_strh_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strh_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv5e_strh_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strh_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_strh_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strh_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_strh_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strh_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv5e_strh_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_strh_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv5e_strh_addpc_cond_rdNoPC_offset8PC,
Instance_armv5e_strh_subpc_cond_rdNoPC_offset8PC,
Instance_armv5e_ldm_cond_amode4L_rnNoPC_reglistNoPC,
Instance_armv5e_ldm_w_cond_amode4L_rnNoPC_reglistNoPC,
Instance_armv5e_ldm_pc_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_ldm_pc_w_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_ldm_s10_cond_amode4L_rnNoPC_reglistNoPC,
Instance_armv5e_ldm_s11_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_ldm_s11_w_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_stm_cond_amode4S_rnNoPC_reglist,
Instance_armv5e_stm_w_cond_amode4S_rnNoPC_reglist,
Instance_armv5e_stm_s1_cond_amode4S_rnNoPC_reglist,
Instance_armv5e_swp_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_swpb_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_bkpt_bkptnum,
Instance_armv5e_swi_cond_swinum,
Instance_armv5e_cdp_cond_cpnum_opcode1d_crd_crn_crm_opcode2,
Instance_armv5e_cdp2_cpnum_opcode1d_crd_crn_crm_opcode2,
Instance_armv5e_ldc_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldc_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldc_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc_unindexed_cond_cpnum_crd_rn_option,
Instance_armv5e_ldcl_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldcl_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldcl_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcl_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcl_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcl_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcl_unindexed_cond_cpnum_crd_rn_option,
Instance_armv5e_stc_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_stc_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_stc_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc_unindexed_cond_cpnum_crd_rn_option,
Instance_armv5e_stcl_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_stcl_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv5e_stcl_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcl_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcl_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcl_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcl_unindexed_cond_cpnum_crd_rn_option,
Instance_armv5e_ldc2_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldc2_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldc2_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2_unindexed_cpnum_crd_rn_option,
Instance_armv5e_ldc2l_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldc2l_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldc2l_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2l_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2l_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2l_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldc2l_unindexed_cpnum_crd_rn_option,
Instance_armv5e_stc2_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_stc2_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_stc2_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2_unindexed_cpnum_crd_rn_option,
Instance_armv5e_stc2l_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_stc2l_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_stc2l_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2l_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2l_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2l_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stc2l_unindexed_cpnum_crd_rn_option,
Instance_armv5e_mcr_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv5e_mcr2_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv5e_mrc_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv5e_mrc_pc_cond_cpnum_opcode1m_crn_crm_opcode2,
Instance_armv5e_mrc2_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv5e_mrc2_pc_cpnum_opcode1m_crn_crm_opcode2,
Instance_armv5e_smlabb_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlabt_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlatb_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlatt_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlalbb_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlalbt_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlaltb_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlaltt_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smulbb_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smulbt_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smultb_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smultt_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smlawb_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlawt_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smulwb_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smulwt_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_qadd_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_qdadd_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_qdsub_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_qsub_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_ldrd_addi_cond_rdPaired_rn_offset9,
Instance_armv5e_ldrd_subi_cond_rdPaired_rn_offset8,
Instance_armv5e_ldrd_add_cond_rdPaired_rn_rmNoPC,
Instance_armv5e_ldrd_sub_cond_rdPaired_rn_rmNoPC,
Instance_armv5e_ldrd_post_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrd_post_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv5e_ldrd_post_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrd_post_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv5e_ldrd_pre_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrd_pre_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv5e_ldrd_pre_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrd_pre_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv5e_ldrd_addpc_cond_rdPaired_offset8PC,
Instance_armv5e_ldrd_subpc_cond_rdPaired_offset8PC,
Instance_armv5e_strd_addi_cond_rdPaired_rn_offset9,
Instance_armv5e_strd_subi_cond_rdPaired_rn_offset8,
Instance_armv5e_strd_add_cond_rdPaired_rn_rmNoPC,
Instance_armv5e_strd_sub_cond_rdPaired_rn_rmNoPC,
Instance_armv5e_strd_post_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strd_post_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv5e_strd_post_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strd_post_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv5e_strd_pre_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strd_pre_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv5e_strd_pre_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strd_pre_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv5e_strd_addpc_cond_rdPaired_offset8PC,
Instance_armv5e_strd_subpc_cond_rdPaired_offset8PC,
Instance_armv5e_pld_addi_rn_offset13,
Instance_armv5e_pld_subi_rn_offset12,
Instance_armv5e_pld_addpc_offset12PC,
Instance_armv5e_pld_subpc_offset12PC,
Instance_armv5e_pld_add_rn_rmNoPC,
Instance_armv5e_pld_add_asr_rn_rmNoPC_shfimmr,
Instance_armv5e_pld_add_lsl_rn_rmNoPC_shfimml,
Instance_armv5e_pld_add_lsr_rn_rmNoPC_shfimmr,
Instance_armv5e_pld_add_ror_rn_rmNoPC_rotateimm,
Instance_armv5e_pld_add_rrx_rn_rmNoPC,
Instance_armv5e_pld_addsh2_rn_rmNoPC,
Instance_armv5e_pld_sub_rn_rmNoPC,
Instance_armv5e_pld_sub_asr_rn_rmNoPC_shfimmr,
Instance_armv5e_pld_sub_lsl_rn_rmNoPC_shfimml,
Instance_armv5e_pld_sub_lsr_rn_rmNoPC_shfimmr,
Instance_armv5e_pld_sub_ror_rn_rmNoPC_rotateimm,
Instance_armv5e_pld_sub_rrx_rn_rmNoPC,
Instance_armv5e_mcrr_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv5e_mrrc_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv5e_ldr_cond_rdNoPC_rn,
Instance_armv5e_ldr_pc_cond_rdPC_rn,
Instance_armv5e_ldrb_cond_rdNoPC_rn,
Instance_armv5e_ldrbt_cond_rdNoPC_rnNoPC,
Instance_armv5e_ldrt_cond_rdNoPC_rnNoPC,
Instance_armv5e_pld_rn,
Instance_armv5e_str_cond_rdNoPC_rn,
Instance_armv5e_str_pc_cond_rdPC_rn,
Instance_armv5e_strb_cond_rdNoPC_rn,
Instance_armv5e_strbt_cond_rdNoPC_rnNoPC,
Instance_armv5e_strt_cond_rdNoPC_rnNoPC,
Instance_armv5e_mcr_x2_cond_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv5e_mcr2_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv5e_mrc_x2_cond_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv5e_mrc2_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv5e_ldral_rdNoPC_rn,
Instance_armv5e_ldral_pc_rdPC_rn,
Instance_armv5e_ldralb_rdNoPC_rn,
Instance_armv5e_ldralbt_rdNoPC_rnNoPC,
Instance_armv5e_ldralt_rdNoPC_rnNoPC,
Instance_armv5e_stral_rdNoPC_rn,
Instance_armv5e_stral_pc_rdPC_rn,
Instance_armv5e_stralb_rdNoPC_rn,
Instance_armv5e_stralbt_rdNoPC_rnNoPC,
Instance_armv5e_stralt_rdNoPC_rnNoPC,
Instance_armv5e_mcral_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv5e_mrcal_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv5e_bal_btarg1,
Instance_armv5e_blal_btarg1,
Instance_armv5e_blxal_rmNoPC,
Instance_armv5e_bxal_rm,
Instance_armv5e_movals_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_movals_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movals_imm_rdNoPC_immed8,
Instance_armv5e_movals_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv5e_movals_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movals_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_movals_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movals_reg_rdNoPC_rm,
Instance_armv5e_movals_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv5e_movals_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_movals_rrx_rdNoPC_rm,
Instance_armv5e_moval_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_moval_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_moval_imm_rdNoPC_immed8,
Instance_armv5e_moval_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv5e_moval_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_moval_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_moval_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_moval_reg_rdNoPC_rm,
Instance_armv5e_moval_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv5e_moval_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_moval_rrx_rdNoPC_rm,
Instance_armv5e_moval_pc_imm_S_rdPC_immed8,
Instance_armv5e_moval_pc_reg_S_rdPC_rm,
Instance_armv5e_moval_pc_lsl_imm_S_rdPC_rm_shfimml,
Instance_armv5e_moval_pc_lsr_imm_S_rdPC_rm_shfimmr,
Instance_armv5e_moval_pc_asr_imm_S_rdPC_rm_shfimmr,
Instance_armv5e_moval_pc_ror_imm_S_rdPC_rm_rotateimm,
Instance_armv5e_moval_pc_rrx_S_rdPC_rm,
Instance_armv5e_mvnals_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_mvnals_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnals_imm_rdNoPC_immed8,
Instance_armv5e_mvnals_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv5e_mvnals_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnals_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_mvnals_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnals_reg_rdNoPC_rm,
Instance_armv5e_mvnals_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv5e_mvnals_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnals_rrx_rdNoPC_rm,
Instance_armv5e_mvnal_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_mvnal_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnal_imm_rdNoPC_immed8,
Instance_armv5e_mvnal_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv5e_mvnal_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnal_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv5e_mvnal_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnal_reg_rdNoPC_rm,
Instance_armv5e_mvnal_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv5e_mvnal_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv5e_mvnal_rrx_rdNoPC_rm,
Instance_armv5e_mvnal_pc_imm_S_rdPC_immed8,
Instance_armv5e_mvnal_pc_reg_S_rdPC_rm,
Instance_armv5e_mvnal_pc_lsl_imm_S_rdPC_rm_shfimml,
Instance_armv5e_mvnal_pc_lsr_imm_S_rdPC_rm_shfimmr,
Instance_armv5e_mvnal_pc_asr_imm_S_rdPC_rm_shfimmr,
Instance_armv5e_mvnal_pc_ror_imm_S_rdPC_rm_rotateimm,
Instance_armv5e_mvnal_pc_rrx_S_rdPC_rm,
Instance_armv5e_cmpal_asr_imm_rn_rm_shfimmr,
Instance_armv5e_cmpal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmpal_imm_rn_immed8,
Instance_armv5e_cmpal_lsl_imm_rn_rm_shfimml,
Instance_armv5e_cmpal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmpal_lsr_imm_rn_rm_shfimmr,
Instance_armv5e_cmpal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmpal_reg_rn_rm,
Instance_armv5e_cmpal_ror_imm_rn_rm_rotateimm,
Instance_armv5e_cmpal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmpal_rrx_rn_rm,
Instance_armv5e_cmnal_asr_imm_rn_rm_shfimmr,
Instance_armv5e_cmnal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmnal_imm_rn_immed8,
Instance_armv5e_cmnal_lsl_imm_rn_rm_shfimml,
Instance_armv5e_cmnal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmnal_lsr_imm_rn_rm_shfimmr,
Instance_armv5e_cmnal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmnal_reg_rn_rm,
Instance_armv5e_cmnal_ror_imm_rn_rm_rotateimm,
Instance_armv5e_cmnal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_cmnal_rrx_rn_rm,
Instance_armv5e_tstal_asr_imm_rn_rm_shfimmr,
Instance_armv5e_tstal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tstal_imm_rn_immed8,
Instance_armv5e_tstal_lsl_imm_rn_rm_shfimml,
Instance_armv5e_tstal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tstal_lsr_imm_rn_rm_shfimmr,
Instance_armv5e_tstal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tstal_reg_rn_rm,
Instance_armv5e_tstal_ror_imm_rn_rm_rotateimm,
Instance_armv5e_tstal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_tstal_rrx_rn_rm,
Instance_armv5e_teqal_asr_imm_rn_rm_shfimmr,
Instance_armv5e_teqal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teqal_imm_rn_immed8,
Instance_armv5e_teqal_lsl_imm_rn_rm_shfimml,
Instance_armv5e_teqal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teqal_lsr_imm_rn_rm_shfimmr,
Instance_armv5e_teqal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teqal_reg_rn_rm,
Instance_armv5e_teqal_ror_imm_rn_rm_rotateimm,
Instance_armv5e_teqal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_teqal_rrx_rn_rm,
Instance_armv5e_addals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_addals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addals_imm_rdNoPC_rn_immed8,
Instance_armv5e_addals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_addals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_addals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addals_reg_rdNoPC_rn_rm,
Instance_armv5e_addals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_addals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addals_rrx_rdNoPC_rn_rm,
Instance_armv5e_addal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_addal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addal_imm_rdNoPC_rn_immed8,
Instance_armv5e_addal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_addal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_addal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addal_reg_rdNoPC_rn_rm,
Instance_armv5e_addal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_addal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_addal_rrx_rdNoPC_rn_rm,
Instance_armv5e_addal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_addal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_addal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_addal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_addal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_addal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_addal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_subals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_subals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subals_imm_rdNoPC_rn_immed8,
Instance_armv5e_subals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_subals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_subals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subals_reg_rdNoPC_rn_rm,
Instance_armv5e_subals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_subals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subals_rrx_rdNoPC_rn_rm,
Instance_armv5e_subal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_subal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subal_imm_rdNoPC_rn_immed8,
Instance_armv5e_subal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_subal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_subal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subal_reg_rdNoPC_rn_rm,
Instance_armv5e_subal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_subal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_subal_rrx_rdNoPC_rn_rm,
Instance_armv5e_subal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_subal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_subal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_subal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_subal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_subal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_subal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_rsbals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsbals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbals_imm_rdNoPC_rn_immed8,
Instance_armv5e_rsbals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rsbals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsbals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbals_reg_rdNoPC_rn_rm,
Instance_armv5e_rsbals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rsbals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbals_rrx_rdNoPC_rn_rm,
Instance_armv5e_rsbal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsbal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbal_imm_rdNoPC_rn_immed8,
Instance_armv5e_rsbal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rsbal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rsbal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbal_reg_rdNoPC_rn_rm,
Instance_armv5e_rsbal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rsbal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rsbal_rrx_rdNoPC_rn_rm,
Instance_armv5e_rsbal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_rsbal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_rsbal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_rsbal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rsbal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rsbal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_rsbal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_adcals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adcals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcals_imm_rdNoPC_rn_immed8,
Instance_armv5e_adcals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_adcals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adcals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcals_reg_rdNoPC_rn_rm,
Instance_armv5e_adcals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_adcals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcals_rrx_rdNoPC_rn_rm,
Instance_armv5e_adcal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adcal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcal_imm_rdNoPC_rn_immed8,
Instance_armv5e_adcal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_adcal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_adcal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcal_reg_rdNoPC_rn_rm,
Instance_armv5e_adcal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_adcal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_adcal_rrx_rdNoPC_rn_rm,
Instance_armv5e_adcal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_adcal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_adcal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_adcal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_adcal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_adcal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_adcal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_sbcals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbcals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcals_imm_rdNoPC_rn_immed8,
Instance_armv5e_sbcals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_sbcals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbcals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcals_reg_rdNoPC_rn_rm,
Instance_armv5e_sbcals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_sbcals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcals_rrx_rdNoPC_rn_rm,
Instance_armv5e_sbcal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbcal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcal_imm_rdNoPC_rn_immed8,
Instance_armv5e_sbcal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_sbcal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_sbcal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcal_reg_rdNoPC_rn_rm,
Instance_armv5e_sbcal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_sbcal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_sbcal_rrx_rdNoPC_rn_rm,
Instance_armv5e_sbcal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_sbcal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_sbcal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_sbcal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_sbcal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_sbcal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_sbcal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_rscals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rscals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscals_imm_rdNoPC_rn_immed8,
Instance_armv5e_rscals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rscals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rscals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscals_reg_rdNoPC_rn_rm,
Instance_armv5e_rscals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rscals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscals_rrx_rdNoPC_rn_rm,
Instance_armv5e_rscal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rscal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscal_imm_rdNoPC_rn_immed8,
Instance_armv5e_rscal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_rscal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_rscal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscal_reg_rdNoPC_rn_rm,
Instance_armv5e_rscal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_rscal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_rscal_rrx_rdNoPC_rn_rm,
Instance_armv5e_rscal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_rscal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_rscal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_rscal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rscal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_rscal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_rscal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_andals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_andals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andals_imm_rdNoPC_rn_immed8,
Instance_armv5e_andals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_andals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_andals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andals_reg_rdNoPC_rn_rm,
Instance_armv5e_andals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_andals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andals_rrx_rdNoPC_rn_rm,
Instance_armv5e_andal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_andal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andal_imm_rdNoPC_rn_immed8,
Instance_armv5e_andal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_andal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_andal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andal_reg_rdNoPC_rn_rm,
Instance_armv5e_andal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_andal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_andal_rrx_rdNoPC_rn_rm,
Instance_armv5e_andal_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_andal_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_andal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_andal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_andal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_andal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_andal_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_bicals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bicals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bicals_imm_rdNoPC_rn_immed8,
Instance_armv5e_bicals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_bicals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bicals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bicals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bicals_reg_rdNoPC_rn_rm,
Instance_armv5e_bicals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_bicals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bicals_rrx_rdNoPC_rn_rm,
Instance_armv5e_bical_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bical_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bical_imm_rdNoPC_rn_immed8,
Instance_armv5e_bical_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_bical_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bical_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_bical_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bical_reg_rdNoPC_rn_rm,
Instance_armv5e_bical_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_bical_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_bical_rrx_rdNoPC_rn_rm,
Instance_armv5e_bical_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_bical_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_bical_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_bical_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_bical_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_bical_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_bical_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_eorals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eorals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eorals_imm_rdNoPC_rn_immed8,
Instance_armv5e_eorals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_eorals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eorals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eorals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eorals_reg_rdNoPC_rn_rm,
Instance_armv5e_eorals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_eorals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eorals_rrx_rdNoPC_rn_rm,
Instance_armv5e_eoral_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eoral_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eoral_imm_rdNoPC_rn_immed8,
Instance_armv5e_eoral_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_eoral_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eoral_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_eoral_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eoral_reg_rdNoPC_rn_rm,
Instance_armv5e_eoral_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_eoral_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_eoral_rrx_rdNoPC_rn_rm,
Instance_armv5e_eoral_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_eoral_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_eoral_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_eoral_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_eoral_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_eoral_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_eoral_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_orrals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orrals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrals_imm_rdNoPC_rn_immed8,
Instance_armv5e_orrals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_orrals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orrals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrals_reg_rdNoPC_rn_rm,
Instance_armv5e_orrals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_orrals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orrals_rrx_rdNoPC_rn_rm,
Instance_armv5e_orral_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orral_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orral_imm_rdNoPC_rn_immed8,
Instance_armv5e_orral_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv5e_orral_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orral_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv5e_orral_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orral_reg_rdNoPC_rn_rm,
Instance_armv5e_orral_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv5e_orral_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv5e_orral_rrx_rdNoPC_rn_rm,
Instance_armv5e_orral_pc_imm_S_rdPC_rn_immed8,
Instance_armv5e_orral_pc_reg_S_rdPC_rn_rm,
Instance_armv5e_orral_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv5e_orral_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_orral_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv5e_orral_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv5e_orral_pc_rrx_S_rdPC_rn_rm,
Instance_armv5e_mlaals_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_mlaal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_mulals_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_mulal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smlalals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlalal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smullals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smullal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umlalals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umlalal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umullals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_umullal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_clzal_rdNoPC_rmNoPC,
Instance_armv5e_mrsal_cpsr_rdNoPC,
Instance_armv5e_mrsal_spsr_rdNoPC,
Instance_armv5e_msral_cpsr_imm_fields_immed8,
Instance_armv5e_msral_cpsr_imm_f_immed8,
Instance_armv5e_msral_cpsr_reg_fields_rm,
Instance_armv5e_msral_cpsr_reg_f_rm,
Instance_armv5e_msral_spsr_imm_fields_immed8,
Instance_armv5e_msral_spsr_reg_fields_rm,
Instance_armv5e_ldral_addi_rdNoPC_rn_offset13,
Instance_armv5e_ldral_subi_rdNoPC_rn_offset12,
Instance_armv5e_ldral_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldral_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldral_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldral_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldral_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldral_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldral_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldral_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldral_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldral_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldral_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldral_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldral_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldral_addpc_rdNoPC_offset12PC,
Instance_armv5e_ldral_subpc_rdNoPC_offset12PC,
Instance_armv5e_ldral_pc_addi_rdPC_rn_offset13,
Instance_armv5e_ldral_pc_subi_rdPC_rn_offset12,
Instance_armv5e_ldral_pc_add_rdPC_rn_rmNoPC,
Instance_armv5e_ldral_pc_add_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_add_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldral_pc_add_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_add_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldral_pc_add_rrx_rdPC_rn_rmNoPC,
Instance_armv5e_ldral_pc_addsh2_rdPC_rn_rmNoPC,
Instance_armv5e_ldral_pc_sub_rdPC_rn_rmNoPC,
Instance_armv5e_ldral_pc_sub_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_sub_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldral_pc_sub_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_sub_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldral_pc_sub_rrx_rdPC_rn_rmNoPC,
Instance_armv5e_ldral_pc_post_add_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_post_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_post_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_pc_post_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_post_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_pc_post_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_post_addi_rdPC_rnNoPC_offset13,
Instance_armv5e_ldral_pc_post_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_post_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_post_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_post_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_pc_post_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_post_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_pc_post_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_post_subi_rdPC_rnNoPC_offset12,
Instance_armv5e_ldral_pc_pre_add_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_pre_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_pre_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_pc_pre_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_pre_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_pc_pre_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_pre_addi_rdPC_rnNoPC_offset13,
Instance_armv5e_ldral_pc_pre_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_pre_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_pre_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_pre_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldral_pc_pre_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldral_pc_pre_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldral_pc_pre_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_ldral_pc_pre_subi_rdPC_rnNoPC_offset12,
Instance_armv5e_ldral_pc_addpc_rdPC_offset12PC,
Instance_armv5e_ldral_pc_subpc_rdPC_offset12PC,
Instance_armv5e_ldralb_addi_rdNoPC_rn_offset13,
Instance_armv5e_ldralb_subi_rdNoPC_rn_offset12,
Instance_armv5e_ldralb_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralb_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldralb_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldralb_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldralb_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldralb_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralb_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralb_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralb_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldralb_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_ldralb_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_ldralb_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_ldralb_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralb_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralb_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralb_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldralb_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralb_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralb_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldralb_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralb_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralb_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldralb_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralb_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralb_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralb_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralb_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldralb_addpc_rdNoPC_offset12PC,
Instance_armv5e_ldralb_subpc_rdNoPC_offset12PC,
Instance_armv5e_stral_addi_rdNoPC_rn_offset13,
Instance_armv5e_stral_subi_rdNoPC_rn_offset12,
Instance_armv5e_stral_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_stral_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_stral_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_stral_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_stral_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv5e_stral_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_stral_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_stral_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_stral_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_stral_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_stral_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_stral_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_stral_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_stral_addpc_rdNoPC_offset12PC,
Instance_armv5e_stral_subpc_rdNoPC_offset12PC,
Instance_armv5e_stral_pc_addi_rdPC_rn_offset13,
Instance_armv5e_stral_pc_subi_rdPC_rn_offset12,
Instance_armv5e_stral_pc_add_rdPC_rn_rmNoPC,
Instance_armv5e_stral_pc_add_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_add_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_stral_pc_add_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_add_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_stral_pc_add_rrx_rdPC_rn_rmNoPC,
Instance_armv5e_stral_pc_addsh2_rdPC_rn_rmNoPC,
Instance_armv5e_stral_pc_sub_rdPC_rn_rmNoPC,
Instance_armv5e_stral_pc_sub_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_sub_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv5e_stral_pc_sub_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_sub_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv5e_stral_pc_sub_rrx_rdPC_rn_rmNoPC,
Instance_armv5e_stral_pc_post_add_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_post_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_post_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_pc_post_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_post_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_pc_post_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_post_addi_rdPC_rnNoPC_offset13,
Instance_armv5e_stral_pc_post_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_post_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_post_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_post_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_pc_post_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_post_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_pc_post_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_post_subi_rdPC_rnNoPC_offset12,
Instance_armv5e_stral_pc_pre_add_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_pre_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_pre_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_pc_pre_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_pre_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_pc_pre_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_pre_addi_rdPC_rnNoPC_offset13,
Instance_armv5e_stral_pc_pre_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_pre_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_pre_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_pre_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stral_pc_pre_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stral_pc_pre_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stral_pc_pre_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv5e_stral_pc_pre_subi_rdPC_rnNoPC_offset12,
Instance_armv5e_stral_pc_addpc_rdPC_offset12PC,
Instance_armv5e_stral_pc_subpc_rdPC_offset12PC,
Instance_armv5e_stralb_addi_rdNoPC_rn_offset13,
Instance_armv5e_stralb_subi_rdNoPC_rn_offset12,
Instance_armv5e_stralb_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralb_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stralb_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_stralb_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stralb_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_stralb_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralb_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralb_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralb_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stralb_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv5e_stralb_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv5e_stralb_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv5e_stralb_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralb_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralb_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralb_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_stralb_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralb_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralb_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_stralb_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralb_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralb_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_stralb_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralb_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralb_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralb_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralb_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_stralb_addpc_rdNoPC_offset12PC,
Instance_armv5e_stralb_subpc_rdNoPC_offset12PC,
Instance_armv5e_ldralbt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralbt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralbt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralbt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralbt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralbt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralbt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldralbt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralbt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralbt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralbt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralbt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralbt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralbt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralbt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldralt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_ldralt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_ldralt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_ldralt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_ldralt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_stralbt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralbt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralbt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralbt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralbt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralbt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralbt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_stralbt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralbt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralbt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralbt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralbt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralbt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralbt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralbt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_stralt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv5e_stralt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv5e_stralt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv5e_stralt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv5e_stralt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv5e_ldralh_addi_rdNoPC_rn_offset9,
Instance_armv5e_ldralh_subi_rdNoPC_rn_offset8,
Instance_armv5e_ldralh_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralh_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralh_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralh_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldralh_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralh_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldralh_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralh_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldralh_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralh_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldralh_addpc_rdNoPC_offset8PC,
Instance_armv5e_ldralh_subpc_rdNoPC_offset8PC,
Instance_armv5e_ldralsb_addi_rdNoPC_rn_offset9,
Instance_armv5e_ldralsb_subi_rdNoPC_rn_offset8,
Instance_armv5e_ldralsb_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralsb_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralsb_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsb_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldralsb_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsb_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldralsb_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsb_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldralsb_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsb_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldralsb_addpc_rdNoPC_offset8PC,
Instance_armv5e_ldralsb_subpc_rdNoPC_offset8PC,
Instance_armv5e_ldralsh_addi_rdNoPC_rn_offset9,
Instance_armv5e_ldralsh_subi_rdNoPC_rn_offset8,
Instance_armv5e_ldralsh_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralsh_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_ldralsh_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsh_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldralsh_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsh_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldralsh_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsh_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_ldralsh_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_ldralsh_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_ldralsh_addpc_rdNoPC_offset8PC,
Instance_armv5e_ldralsh_subpc_rdNoPC_offset8PC,
Instance_armv5e_stralh_addi_rdNoPC_rn_offset9,
Instance_armv5e_stralh_subi_rdNoPC_rn_offset8,
Instance_armv5e_stralh_add_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralh_sub_rdNoPC_rn_rmNoPC,
Instance_armv5e_stralh_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralh_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_stralh_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralh_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_stralh_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralh_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv5e_stralh_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv5e_stralh_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv5e_stralh_addpc_rdNoPC_offset8PC,
Instance_armv5e_stralh_subpc_rdNoPC_offset8PC,
Instance_armv5e_ldmal_amode4L_rnNoPC_reglistNoPC,
Instance_armv5e_ldmal_w_amode4L_rnNoPC_reglistNoPC,
Instance_armv5e_ldmal_pc_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_ldmal_pc_w_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_ldmal_s10_amode4L_rnNoPC_reglistNoPC,
Instance_armv5e_ldmal_s11_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_ldmal_s11_w_amode4L_rnNoPC_reglistWithPC,
Instance_armv5e_stmal_amode4S_rnNoPC_reglist,
Instance_armv5e_stmal_w_amode4S_rnNoPC_reglist,
Instance_armv5e_stmal_s1_amode4S_rnNoPC_reglist,
Instance_armv5e_swpal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_swpalb_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_swial_swinum,
Instance_armv5e_cdpal_cpnum_opcode1d_crd_crn_crm_opcode2,
Instance_armv5e_ldcal_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldcal_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldcal_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcal_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcal_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcal_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcal_unindexed_cpnum_crd_rn_option,
Instance_armv5e_ldcall_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldcall_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_ldcall_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcall_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcall_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcall_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_ldcall_unindexed_cpnum_crd_rn_option,
Instance_armv5e_stcal_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_stcal_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_stcal_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcal_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcal_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcal_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcal_unindexed_cpnum_crd_rn_option,
Instance_armv5e_stcall_add_cpnum_crd_rn_offset8CP,
Instance_armv5e_stcall_sub_cpnum_crd_rn_offset8CP,
Instance_armv5e_stcall_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcall_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcall_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcall_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv5e_stcall_unindexed_cpnum_crd_rn_option,
Instance_armv5e_mcral_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv5e_mrcal_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv5e_mrcal_pc_cpnum_opcode1m_crn_crm_opcode2,
Instance_armv5e_smlabbal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlabtal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlatbal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlattal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlalbbal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlalbtal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlaltbal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smlalttal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv5e_smulbbal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smulbtal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smultbal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smulttal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smlawbal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smlawtal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv5e_smulwbal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_smulwtal_rdmul_rmNoPC_rsNoPC,
Instance_armv5e_qaddal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_qdaddal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_qdsubal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_qsubal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv5e_ldrald_addi_rdPaired_rn_offset9,
Instance_armv5e_ldrald_subi_rdPaired_rn_offset8,
Instance_armv5e_ldrald_add_rdPaired_rn_rmNoPC,
Instance_armv5e_ldrald_sub_rdPaired_rn_rmNoPC,
Instance_armv5e_ldrald_post_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrald_post_addi_rdPaired_rnNoPC_offset9,
Instance_armv5e_ldrald_post_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrald_post_subi_rdPaired_rnNoPC_offset8,
Instance_armv5e_ldrald_pre_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrald_pre_addi_rdPaired_rnNoPC_offset9,
Instance_armv5e_ldrald_pre_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_ldrald_pre_subi_rdPaired_rnNoPC_offset8,
Instance_armv5e_ldrald_addpc_rdPaired_offset8PC,
Instance_armv5e_ldrald_subpc_rdPaired_offset8PC,
Instance_armv5e_strald_addi_rdPaired_rn_offset9,
Instance_armv5e_strald_subi_rdPaired_rn_offset8,
Instance_armv5e_strald_add_rdPaired_rn_rmNoPC,
Instance_armv5e_strald_sub_rdPaired_rn_rmNoPC,
Instance_armv5e_strald_post_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strald_post_addi_rdPaired_rnNoPC_offset9,
Instance_armv5e_strald_post_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strald_post_subi_rdPaired_rnNoPC_offset8,
Instance_armv5e_strald_pre_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strald_pre_addi_rdPaired_rnNoPC_offset9,
Instance_armv5e_strald_pre_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv5e_strald_pre_subi_rdPaired_rnNoPC_offset8,
Instance_armv5e_strald_addpc_rdPaired_offset8PC,
Instance_armv5e_strald_subpc_rdPaired_offset8PC,
Instance_armv5e_mcrral_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv5e_mrrcal_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv6_b_cond_btarg1,
Instance_armv6_bl_cond_btarg1,
Instance_armv6_blxi_btarg2,
Instance_armv6_blx_cond_rmNoPC,
Instance_armv6_bx_cond_rm,
Instance_armv6_bxj_cond_rm,
Instance_armv6_movs_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_movs_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movs_imm_cond_rdNoPC_immed8,
Instance_armv6_movs_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv6_movs_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movs_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_movs_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movs_reg_cond_rdNoPC_rm,
Instance_armv6_movs_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv6_movs_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movs_rrx_cond_rdNoPC_rm,
Instance_armv6_mov_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_mov_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mov_imm_cond_rdNoPC_immed8,
Instance_armv6_mov_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv6_mov_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mov_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_mov_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mov_reg_cond_rdNoPC_rm,
Instance_armv6_mov_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv6_mov_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mov_rrx_cond_rdNoPC_rm,
Instance_armv6_mov_pc_imm_cond_S_rdPC_immed8,
Instance_armv6_mov_pc_reg_cond_S_rdPC_rm,
Instance_armv6_mov_pc_lsl_imm_cond_S_rdPC_rm_shfimml,
Instance_armv6_mov_pc_lsr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv6_mov_pc_asr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv6_mov_pc_ror_imm_cond_S_rdPC_rm_rotateimm,
Instance_armv6_mov_pc_rrx_cond_S_rdPC_rm,
Instance_armv6_mvns_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_mvns_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvns_imm_cond_rdNoPC_immed8,
Instance_armv6_mvns_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv6_mvns_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvns_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_mvns_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvns_reg_cond_rdNoPC_rm,
Instance_armv6_mvns_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv6_mvns_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvns_rrx_cond_rdNoPC_rm,
Instance_armv6_mvn_asr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_mvn_asr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvn_imm_cond_rdNoPC_immed8,
Instance_armv6_mvn_lsl_imm_cond_rdNoPC_rm_shfimml,
Instance_armv6_mvn_lsl_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvn_lsr_imm_cond_rdNoPC_rm_shfimmr,
Instance_armv6_mvn_lsr_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvn_reg_cond_rdNoPC_rm,
Instance_armv6_mvn_ror_imm_cond_rdNoPC_rm_rotateimm,
Instance_armv6_mvn_ror_reg_cond_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvn_rrx_cond_rdNoPC_rm,
Instance_armv6_mvn_pc_imm_cond_S_rdPC_immed8,
Instance_armv6_mvn_pc_reg_cond_S_rdPC_rm,
Instance_armv6_mvn_pc_lsl_imm_cond_S_rdPC_rm_shfimml,
Instance_armv6_mvn_pc_lsr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv6_mvn_pc_asr_imm_cond_S_rdPC_rm_shfimmr,
Instance_armv6_mvn_pc_ror_imm_cond_S_rdPC_rm_rotateimm,
Instance_armv6_mvn_pc_rrx_cond_S_rdPC_rm,
Instance_armv6_cmp_asr_imm_cond_rn_rm_shfimmr,
Instance_armv6_cmp_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmp_imm_cond_rn_immed8,
Instance_armv6_cmp_lsl_imm_cond_rn_rm_shfimml,
Instance_armv6_cmp_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmp_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv6_cmp_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmp_reg_cond_rn_rm,
Instance_armv6_cmp_ror_imm_cond_rn_rm_rotateimm,
Instance_armv6_cmp_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmp_rrx_cond_rn_rm,
Instance_armv6_cmn_asr_imm_cond_rn_rm_shfimmr,
Instance_armv6_cmn_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmn_imm_cond_rn_immed8,
Instance_armv6_cmn_lsl_imm_cond_rn_rm_shfimml,
Instance_armv6_cmn_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmn_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv6_cmn_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmn_reg_cond_rn_rm,
Instance_armv6_cmn_ror_imm_cond_rn_rm_rotateimm,
Instance_armv6_cmn_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmn_rrx_cond_rn_rm,
Instance_armv6_tst_asr_imm_cond_rn_rm_shfimmr,
Instance_armv6_tst_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tst_imm_cond_rn_immed8,
Instance_armv6_tst_lsl_imm_cond_rn_rm_shfimml,
Instance_armv6_tst_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tst_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv6_tst_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tst_reg_cond_rn_rm,
Instance_armv6_tst_ror_imm_cond_rn_rm_rotateimm,
Instance_armv6_tst_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tst_rrx_cond_rn_rm,
Instance_armv6_teq_asr_imm_cond_rn_rm_shfimmr,
Instance_armv6_teq_asr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teq_imm_cond_rn_immed8,
Instance_armv6_teq_lsl_imm_cond_rn_rm_shfimml,
Instance_armv6_teq_lsl_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teq_lsr_imm_cond_rn_rm_shfimmr,
Instance_armv6_teq_lsr_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teq_reg_cond_rn_rm,
Instance_armv6_teq_ror_imm_cond_rn_rm_rotateimm,
Instance_armv6_teq_ror_reg_cond_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teq_rrx_cond_rn_rm,
Instance_armv6_adds_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adds_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adds_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_adds_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_adds_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adds_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adds_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adds_reg_cond_rdNoPC_rn_rm,
Instance_armv6_adds_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_adds_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adds_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_add_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_add_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_add_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_add_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_add_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_add_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_add_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_add_reg_cond_rdNoPC_rn_rm,
Instance_armv6_add_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_add_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_add_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_add_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_add_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_add_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_add_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_add_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_add_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_add_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_subs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_subs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subs_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_subs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_subs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_subs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subs_reg_cond_rdNoPC_rn_rm,
Instance_armv6_subs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_subs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subs_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_sub_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sub_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sub_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_sub_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_sub_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sub_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sub_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sub_reg_cond_rdNoPC_rn_rm,
Instance_armv6_sub_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_sub_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sub_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_sub_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_sub_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_sub_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_sub_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_sub_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_sub_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_sub_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_rsbs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsbs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbs_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_rsbs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_rsbs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsbs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbs_reg_cond_rdNoPC_rn_rm,
Instance_armv6_rsbs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rsbs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbs_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_rsb_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsb_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsb_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_rsb_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_rsb_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsb_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsb_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsb_reg_cond_rdNoPC_rn_rm,
Instance_armv6_rsb_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rsb_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsb_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_rsb_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_rsb_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_rsb_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_rsb_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rsb_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rsb_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_rsb_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_adcs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adcs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcs_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_adcs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_adcs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adcs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcs_reg_cond_rdNoPC_rn_rm,
Instance_armv6_adcs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_adcs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcs_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_adc_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adc_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adc_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_adc_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_adc_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adc_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adc_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adc_reg_cond_rdNoPC_rn_rm,
Instance_armv6_adc_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_adc_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adc_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_adc_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_adc_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_adc_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_adc_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_adc_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_adc_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_adc_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_sbcs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbcs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcs_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_sbcs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_sbcs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbcs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcs_reg_cond_rdNoPC_rn_rm,
Instance_armv6_sbcs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_sbcs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcs_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_sbc_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbc_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbc_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_sbc_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_sbc_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbc_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbc_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbc_reg_cond_rdNoPC_rn_rm,
Instance_armv6_sbc_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_sbc_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbc_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_sbc_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_sbc_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_sbc_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_sbc_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_sbc_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_sbc_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_sbc_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_rscs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rscs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscs_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_rscs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_rscs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rscs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscs_reg_cond_rdNoPC_rn_rm,
Instance_armv6_rscs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rscs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscs_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_rsc_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsc_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsc_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_rsc_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_rsc_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsc_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsc_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsc_reg_cond_rdNoPC_rn_rm,
Instance_armv6_rsc_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rsc_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsc_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_rsc_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_rsc_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_rsc_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_rsc_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rsc_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rsc_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_rsc_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_ands_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_ands_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_ands_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_ands_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_ands_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_ands_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_ands_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_ands_reg_cond_rdNoPC_rn_rm,
Instance_armv6_ands_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_ands_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_ands_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_and_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_and_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_and_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_and_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_and_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_and_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_and_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_and_reg_cond_rdNoPC_rn_rm,
Instance_armv6_and_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_and_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_and_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_and_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_and_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_and_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_and_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_and_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_and_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_and_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_bics_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bics_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bics_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_bics_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_bics_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bics_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bics_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bics_reg_cond_rdNoPC_rn_rm,
Instance_armv6_bics_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_bics_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bics_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_bic_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bic_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bic_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_bic_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_bic_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bic_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bic_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bic_reg_cond_rdNoPC_rn_rm,
Instance_armv6_bic_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_bic_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bic_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_bic_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_bic_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_bic_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_bic_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_bic_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_bic_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_bic_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_eors_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eors_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eors_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_eors_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_eors_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eors_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eors_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eors_reg_cond_rdNoPC_rn_rm,
Instance_armv6_eors_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_eors_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eors_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_eor_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eor_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eor_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_eor_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_eor_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eor_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eor_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eor_reg_cond_rdNoPC_rn_rm,
Instance_armv6_eor_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_eor_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eor_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_eor_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_eor_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_eor_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_eor_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_eor_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_eor_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_eor_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_orrs_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orrs_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrs_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_orrs_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_orrs_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrs_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orrs_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrs_reg_cond_rdNoPC_rn_rm,
Instance_armv6_orrs_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_orrs_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrs_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_orr_asr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orr_asr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orr_imm_cond_rdNoPC_rn_immed8,
Instance_armv6_orr_lsl_imm_cond_rdNoPC_rn_rm_shfimml,
Instance_armv6_orr_lsl_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orr_lsr_imm_cond_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orr_lsr_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orr_reg_cond_rdNoPC_rn_rm,
Instance_armv6_orr_ror_imm_cond_rdNoPC_rn_rm_rotateimm,
Instance_armv6_orr_ror_reg_cond_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orr_rrx_cond_rdNoPC_rn_rm,
Instance_armv6_orr_pc_imm_cond_S_rdPC_rn_immed8,
Instance_armv6_orr_pc_reg_cond_S_rdPC_rn_rm,
Instance_armv6_orr_pc_lsl_imm_cond_S_rdPC_rn_rm_shfimml,
Instance_armv6_orr_pc_lsr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_orr_pc_asr_imm_cond_S_rdPC_rn_rm_shfimmr,
Instance_armv6_orr_pc_ror_imm_cond_S_rdPC_rn_rm_rotateimm,
Instance_armv6_orr_pc_rrx_cond_S_rdPC_rn_rm,
Instance_armv6_mlas_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_mla_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_muls_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_mul_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smlals_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlal_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smulls_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smull_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umlals_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umlal_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umulls_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umull_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_clz_cond_rdNoPC_rmNoPC,
Instance_armv6_mrs_cpsr_cond_rdNoPC,
Instance_armv6_mrs_spsr_cond_rdNoPC,
Instance_armv6_msr_cpsr_imm_cond_fields_immed8,
Instance_armv6_msr_cpsr_imm_f_cond_immed8,
Instance_armv6_msr_cpsr_reg_cond_fields_rm,
Instance_armv6_msr_cpsr_reg_f_cond_rm,
Instance_armv6_msr_spsr_imm_cond_fields_immed8,
Instance_armv6_msr_spsr_reg_cond_fields_rm,
Instance_armv6_ldr_addi_cond_rdNoPC_rn_offset13,
Instance_armv6_ldr_subi_cond_rdNoPC_rn_offset12,
Instance_armv6_ldr_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldr_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldr_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldr_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldr_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldr_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldr_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldr_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldr_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldr_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldr_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldr_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldr_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldr_addpc_cond_rdNoPC_offset12PC,
Instance_armv6_ldr_subpc_cond_rdNoPC_offset12PC,
Instance_armv6_ldr_pc_addi_cond_rdPC_rn_offset13,
Instance_armv6_ldr_pc_subi_cond_rdPC_rn_offset12,
Instance_armv6_ldr_pc_add_cond_rdPC_rn_rmNoPC,
Instance_armv6_ldr_pc_add_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_add_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_ldr_pc_add_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_add_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldr_pc_add_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv6_ldr_pc_addsh2_cond_rdPC_rn_rmNoPC,
Instance_armv6_ldr_pc_sub_cond_rdPC_rn_rmNoPC,
Instance_armv6_ldr_pc_sub_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_sub_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_ldr_pc_sub_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_sub_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldr_pc_sub_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv6_ldr_pc_post_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_post_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_post_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_pc_post_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_post_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_pc_post_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_post_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv6_ldr_pc_post_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_post_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_post_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_post_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_pc_post_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_post_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_pc_post_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_post_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv6_ldr_pc_pre_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_pre_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_pre_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_pc_pre_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_pre_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_pc_pre_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_pre_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv6_ldr_pc_pre_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_pre_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_pre_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_pre_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldr_pc_pre_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldr_pc_pre_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldr_pc_pre_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldr_pc_pre_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv6_ldr_pc_addpc_cond_rdPC_offset12PC,
Instance_armv6_ldr_pc_subpc_cond_rdPC_offset12PC,
Instance_armv6_ldrb_addi_cond_rdNoPC_rn_offset13,
Instance_armv6_ldrb_subi_cond_rdNoPC_rn_offset12,
Instance_armv6_ldrb_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrb_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldrb_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldrb_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldrb_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldrb_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrb_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrb_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrb_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldrb_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldrb_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldrb_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldrb_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrb_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrb_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrb_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldrb_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrb_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrb_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldrb_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrb_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrb_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldrb_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrb_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrb_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrb_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrb_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldrb_addpc_cond_rdNoPC_offset12PC,
Instance_armv6_ldrb_subpc_cond_rdNoPC_offset12PC,
Instance_armv6_str_addi_cond_rdNoPC_rn_offset13,
Instance_armv6_str_subi_cond_rdNoPC_rn_offset12,
Instance_armv6_str_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_str_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_str_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_str_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_str_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_str_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_str_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_str_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_str_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_str_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_str_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_str_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_str_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_str_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_str_addpc_cond_rdNoPC_offset12PC,
Instance_armv6_str_subpc_cond_rdNoPC_offset12PC,
Instance_armv6_str_pc_addi_cond_rdPC_rn_offset13,
Instance_armv6_str_pc_subi_cond_rdPC_rn_offset12,
Instance_armv6_str_pc_add_cond_rdPC_rn_rmNoPC,
Instance_armv6_str_pc_add_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_pc_add_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_str_pc_add_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_pc_add_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_str_pc_add_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv6_str_pc_addsh2_cond_rdPC_rn_rmNoPC,
Instance_armv6_str_pc_sub_cond_rdPC_rn_rmNoPC,
Instance_armv6_str_pc_sub_asr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_pc_sub_lsl_cond_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_str_pc_sub_lsr_cond_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_str_pc_sub_ror_cond_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_str_pc_sub_rrx_cond_rdPC_rn_rmNoPC,
Instance_armv6_str_pc_post_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_post_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_post_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_pc_post_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_post_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_pc_post_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_post_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv6_str_pc_post_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_post_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_post_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_post_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_pc_post_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_post_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_pc_post_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_post_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv6_str_pc_pre_add_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_pre_add_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_pre_add_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_pc_pre_add_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_pre_add_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_pc_pre_add_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_pre_addi_cond_rdPC_rnNoPC_offset13,
Instance_armv6_str_pc_pre_addsh2_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_pre_sub_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_pre_sub_asr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_pre_sub_lsl_cond_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_str_pc_pre_sub_lsr_cond_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_str_pc_pre_sub_ror_cond_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_str_pc_pre_sub_rrx_cond_rdPC_rnNoPC_rmNoPC,
Instance_armv6_str_pc_pre_subi_cond_rdPC_rnNoPC_offset12,
Instance_armv6_str_pc_addpc_cond_rdPC_offset12PC,
Instance_armv6_str_pc_subpc_cond_rdPC_offset12PC,
Instance_armv6_strb_addi_cond_rdNoPC_rn_offset13,
Instance_armv6_strb_subi_cond_rdNoPC_rn_offset12,
Instance_armv6_strb_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strb_add_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_strb_add_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_strb_add_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_strb_add_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_strb_add_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strb_addsh2_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strb_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strb_sub_asr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_strb_sub_lsl_cond_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_strb_sub_lsr_cond_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_strb_sub_ror_cond_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_strb_sub_rrx_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strb_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_post_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_post_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strb_post_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_post_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strb_post_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_post_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_strb_post_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_post_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_post_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strb_post_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_post_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strb_post_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_post_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_strb_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_pre_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_pre_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strb_pre_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_pre_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strb_pre_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_pre_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_strb_pre_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_pre_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_pre_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strb_pre_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strb_pre_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strb_pre_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strb_pre_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_strb_addpc_cond_rdNoPC_offset12PC,
Instance_armv6_strb_subpc_cond_rdNoPC_offset12PC,
Instance_armv6_ldrbt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrbt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrbt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrbt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrbt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrbt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrbt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldrbt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrbt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrbt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrbt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrbt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrbt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrbt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrbt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldrt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldrt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldrt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldrt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldrt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_strbt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strbt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strbt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strbt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strbt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strbt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strbt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_strbt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strbt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strbt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strbt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strbt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strbt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strbt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strbt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_strt_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strt_add_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strt_add_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strt_add_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strt_add_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strt_add_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strt_addi_cond_rdNoPC_rnNoPC_offset13,
Instance_armv6_strt_addsh2_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strt_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strt_sub_asr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strt_sub_lsl_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_strt_sub_lsr_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_strt_sub_ror_cond_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_strt_sub_rrx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strt_subi_cond_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldrh_addi_cond_rdNoPC_rn_offset9,
Instance_armv6_ldrh_subi_cond_rdNoPC_rn_offset8,
Instance_armv6_ldrh_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrh_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrh_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrh_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldrh_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrh_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldrh_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrh_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldrh_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrh_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldrh_addpc_cond_rdNoPC_offset8PC,
Instance_armv6_ldrh_subpc_cond_rdNoPC_offset8PC,
Instance_armv6_ldrsb_addi_cond_rdNoPC_rn_offset9,
Instance_armv6_ldrsb_subi_cond_rdNoPC_rn_offset8,
Instance_armv6_ldrsb_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrsb_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrsb_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsb_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldrsb_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsb_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldrsb_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsb_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldrsb_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsb_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldrsb_addpc_cond_rdNoPC_offset8PC,
Instance_armv6_ldrsb_subpc_cond_rdNoPC_offset8PC,
Instance_armv6_ldrsh_addi_cond_rdNoPC_rn_offset9,
Instance_armv6_ldrsh_subi_cond_rdNoPC_rn_offset8,
Instance_armv6_ldrsh_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrsh_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_ldrsh_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsh_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldrsh_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsh_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldrsh_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsh_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldrsh_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldrsh_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldrsh_addpc_cond_rdNoPC_offset8PC,
Instance_armv6_ldrsh_subpc_cond_rdNoPC_offset8PC,
Instance_armv6_strh_addi_cond_rdNoPC_rn_offset9,
Instance_armv6_strh_subi_cond_rdNoPC_rn_offset8,
Instance_armv6_strh_add_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strh_sub_cond_rdNoPC_rn_rmNoPC,
Instance_armv6_strh_post_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strh_post_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_strh_post_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strh_post_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_strh_pre_add_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strh_pre_addi_cond_rdNoPC_rnNoPC_offset9,
Instance_armv6_strh_pre_sub_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_strh_pre_subi_cond_rdNoPC_rnNoPC_offset8,
Instance_armv6_strh_addpc_cond_rdNoPC_offset8PC,
Instance_armv6_strh_subpc_cond_rdNoPC_offset8PC,
Instance_armv6_ldm_cond_amode4L_rnNoPC_reglistNoPC,
Instance_armv6_ldm_w_cond_amode4L_rnNoPC_reglistNoPC,
Instance_armv6_ldm_pc_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_ldm_pc_w_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_ldm_s10_cond_amode4L_rnNoPC_reglistNoPC,
Instance_armv6_ldm_s11_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_ldm_s11_w_cond_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_stm_cond_amode4S_rnNoPC_reglist,
Instance_armv6_stm_w_cond_amode4S_rnNoPC_reglist,
Instance_armv6_stm_s1_cond_amode4S_rnNoPC_reglist,
Instance_armv6_swp_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_swpb_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_bkpt_bkptnum,
Instance_armv6_swi_cond_swinum,
Instance_armv6_cdp_cond_cpnum_opcode1d_crd_crn_crm_opcode2,
Instance_armv6_cdp2_cpnum_opcode1d_crd_crn_crm_opcode2,
Instance_armv6_ldc_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_ldc_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_ldc_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc_unindexed_cond_cpnum_crd_rn_option,
Instance_armv6_ldcl_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_ldcl_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_ldcl_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcl_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcl_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcl_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcl_unindexed_cond_cpnum_crd_rn_option,
Instance_armv6_stc_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_stc_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_stc_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc_unindexed_cond_cpnum_crd_rn_option,
Instance_armv6_stcl_add_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_stcl_sub_cond_cpnum_crd_rn_offset8CP,
Instance_armv6_stcl_post_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcl_post_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcl_pre_add_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcl_pre_sub_cond_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcl_unindexed_cond_cpnum_crd_rn_option,
Instance_armv6_ldc2_add_cpnum_crd_rn_offset8CP,
Instance_armv6_ldc2_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_ldc2_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2_unindexed_cpnum_crd_rn_option,
Instance_armv6_ldc2l_add_cpnum_crd_rn_offset8CP,
Instance_armv6_ldc2l_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_ldc2l_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2l_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2l_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2l_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldc2l_unindexed_cpnum_crd_rn_option,
Instance_armv6_stc2_add_cpnum_crd_rn_offset8CP,
Instance_armv6_stc2_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_stc2_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2_unindexed_cpnum_crd_rn_option,
Instance_armv6_stc2l_add_cpnum_crd_rn_offset8CP,
Instance_armv6_stc2l_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_stc2l_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2l_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2l_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2l_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stc2l_unindexed_cpnum_crd_rn_option,
Instance_armv6_mcr_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv6_mcr2_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv6_mrc_cond_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv6_mrc_pc_cond_cpnum_opcode1m_crn_crm_opcode2,
Instance_armv6_mrc2_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv6_mrc2_pc_cpnum_opcode1m_crn_crm_opcode2,
Instance_armv6_smlabb_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlabt_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlatb_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlatt_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlalbb_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlalbt_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlaltb_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlaltt_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smulbb_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smulbt_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smultb_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smultt_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smlawb_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlawt_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smulwb_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smulwt_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_qadd_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_qdadd_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_qdsub_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_qsub_cond_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_ldrd_addi_cond_rdPaired_rn_offset9,
Instance_armv6_ldrd_subi_cond_rdPaired_rn_offset8,
Instance_armv6_ldrd_add_cond_rdPaired_rn_rmNoPC,
Instance_armv6_ldrd_sub_cond_rdPaired_rn_rmNoPC,
Instance_armv6_ldrd_post_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrd_post_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv6_ldrd_post_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrd_post_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv6_ldrd_pre_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrd_pre_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv6_ldrd_pre_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrd_pre_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv6_ldrd_addpc_cond_rdPaired_offset8PC,
Instance_armv6_ldrd_subpc_cond_rdPaired_offset8PC,
Instance_armv6_strd_addi_cond_rdPaired_rn_offset9,
Instance_armv6_strd_subi_cond_rdPaired_rn_offset8,
Instance_armv6_strd_add_cond_rdPaired_rn_rmNoPC,
Instance_armv6_strd_sub_cond_rdPaired_rn_rmNoPC,
Instance_armv6_strd_post_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strd_post_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv6_strd_post_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strd_post_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv6_strd_pre_add_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strd_pre_addi_cond_rdPaired_rnNoPC_offset9,
Instance_armv6_strd_pre_sub_cond_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strd_pre_subi_cond_rdPaired_rnNoPC_offset8,
Instance_armv6_strd_addpc_cond_rdPaired_offset8PC,
Instance_armv6_strd_subpc_cond_rdPaired_offset8PC,
Instance_armv6_pld_addi_rn_offset13,
Instance_armv6_pld_subi_rn_offset12,
Instance_armv6_pld_addpc_offset12PC,
Instance_armv6_pld_subpc_offset12PC,
Instance_armv6_pld_add_rn_rmNoPC,
Instance_armv6_pld_add_asr_rn_rmNoPC_shfimmr,
Instance_armv6_pld_add_lsl_rn_rmNoPC_shfimml,
Instance_armv6_pld_add_lsr_rn_rmNoPC_shfimmr,
Instance_armv6_pld_add_ror_rn_rmNoPC_rotateimm,
Instance_armv6_pld_add_rrx_rn_rmNoPC,
Instance_armv6_pld_addsh2_rn_rmNoPC,
Instance_armv6_pld_sub_rn_rmNoPC,
Instance_armv6_pld_sub_asr_rn_rmNoPC_shfimmr,
Instance_armv6_pld_sub_lsl_rn_rmNoPC_shfimml,
Instance_armv6_pld_sub_lsr_rn_rmNoPC_shfimmr,
Instance_armv6_pld_sub_ror_rn_rmNoPC_rotateimm,
Instance_armv6_pld_sub_rrx_rn_rmNoPC,
Instance_armv6_mcrr_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv6_mrrc_cond_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv6_sadd16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_saddsubx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ssub16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ssubaddx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uadd16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uaddsubx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_usub16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_usubaddx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qadd16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qaddsubx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qsub16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qsubaddx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqadd16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqaddsubx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqsub16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqsubaddx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shadd16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shaddsubx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shsub16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shsubaddx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhadd16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhaddsubx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhsub16_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhsubaddx_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sadd8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ssub8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uadd8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_usub8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qadd8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qsub8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqadd8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqsub8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shadd8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shsub8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhadd8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhsub8_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sel_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_pkhbt_lsl_imm_cond_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_pkhbt_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_pkhtb_cond_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_sxtb_reg_cond_rdNoPC_rmNoPC,
Instance_armv6_sxtb_ror_cond_rdNoPC_rmNoPC_rot,
Instance_armv6_sxtb16_reg_cond_rdNoPC_rmNoPC,
Instance_armv6_sxtb16_ror_cond_rdNoPC_rmNoPC_rot,
Instance_armv6_sxth_reg_cond_rdNoPC_rmNoPC,
Instance_armv6_sxth_ror_cond_rdNoPC_rmNoPC_rot,
Instance_armv6_uxtb_reg_cond_rdNoPC_rmNoPC,
Instance_armv6_uxtb_ror_cond_rdNoPC_rmNoPC_rot,
Instance_armv6_uxtb16_reg_cond_rdNoPC_rmNoPC,
Instance_armv6_uxtb16_ror_cond_rdNoPC_rmNoPC_rot,
Instance_armv6_uxth_reg_cond_rdNoPC_rmNoPC,
Instance_armv6_uxth_ror_cond_rdNoPC_rmNoPC_rot,
Instance_armv6_sxtab_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sxtab_ror_cond_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_sxtab16_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sxtab16_ror_cond_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_sxtah_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sxtah_ror_cond_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_uxtab_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uxtab_ror_cond_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_uxtab16_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uxtab16_ror_cond_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_uxtah_reg_cond_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uxtah_ror_cond_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_ssat_asr_imm_cond_rdNoPC_ssat5_rmNoPC_shfimmr,
Instance_armv6_ssat_lsl_imm_cond_rdNoPC_ssat5_rmNoPC_shfimml,
Instance_armv6_ssat_reg_cond_rdNoPC_ssat5_rmNoPC,
Instance_armv6_usat_asr_imm_cond_rdNoPC_usat5_rmNoPC_shfimmr,
Instance_armv6_usat_lsl_imm_cond_rdNoPC_usat5_rmNoPC_shfimml,
Instance_armv6_usat_reg_cond_rdNoPC_usat5_rmNoPC,
Instance_armv6_ssat16_cond_rdNoPC_ssat4_rmNoPC,
Instance_armv6_usat16_cond_rdNoPC_usat4_rmNoPC,
Instance_armv6_usad8_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_usada8_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smuad_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smuadx_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smusd_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smusdx_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smlad_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smladx_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlsd_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlsdx_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlald_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlaldx_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlsld_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlsldx_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smmla_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmlar_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmls_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmlsr_cond_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmul_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smmulr_cond_rdmul_rmNoPC_rsNoPC,
Instance_armv6_umaal_cond_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_rev_cond_rdNoPC_rmNoPC,
Instance_armv6_rev16_cond_rdNoPC_rmNoPC,
Instance_armv6_revsh_cond_rdNoPC_rmNoPC,
Instance_armv6_ldr_cond_rdNoPC_rn,
Instance_armv6_ldr_pc_cond_rdPC_rn,
Instance_armv6_ldrb_cond_rdNoPC_rn,
Instance_armv6_ldrbt_cond_rdNoPC_rnNoPC,
Instance_armv6_ldrt_cond_rdNoPC_rnNoPC,
Instance_armv6_pld_rn,
Instance_armv6_str_cond_rdNoPC_rn,
Instance_armv6_str_pc_cond_rdPC_rn,
Instance_armv6_strb_cond_rdNoPC_rn,
Instance_armv6_strbt_cond_rdNoPC_rnNoPC,
Instance_armv6_strt_cond_rdNoPC_rnNoPC,
Instance_armv6_mcr_x2_cond_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv6_mcr2_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv6_mrc_x2_cond_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv6_mrc2_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv6_ldral_rdNoPC_rn,
Instance_armv6_ldral_pc_rdPC_rn,
Instance_armv6_ldralb_rdNoPC_rn,
Instance_armv6_ldralbt_rdNoPC_rnNoPC,
Instance_armv6_ldralt_rdNoPC_rnNoPC,
Instance_armv6_stral_rdNoPC_rn,
Instance_armv6_stral_pc_rdPC_rn,
Instance_armv6_stralb_rdNoPC_rn,
Instance_armv6_stralbt_rdNoPC_rnNoPC,
Instance_armv6_stralt_rdNoPC_rnNoPC,
Instance_armv6_mcral_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv6_mrcal_x2_cpnum_opcode1m_rdNoPC_crn_crm,
Instance_armv6_bal_btarg1,
Instance_armv6_blal_btarg1,
Instance_armv6_blxal_rmNoPC,
Instance_armv6_bxal_rm,
Instance_armv6_bxjal_rm,
Instance_armv6_movals_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_movals_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movals_imm_rdNoPC_immed8,
Instance_armv6_movals_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv6_movals_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movals_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_movals_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movals_reg_rdNoPC_rm,
Instance_armv6_movals_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv6_movals_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_movals_rrx_rdNoPC_rm,
Instance_armv6_moval_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_moval_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_moval_imm_rdNoPC_immed8,
Instance_armv6_moval_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv6_moval_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_moval_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_moval_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_moval_reg_rdNoPC_rm,
Instance_armv6_moval_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv6_moval_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_moval_rrx_rdNoPC_rm,
Instance_armv6_moval_pc_imm_S_rdPC_immed8,
Instance_armv6_moval_pc_reg_S_rdPC_rm,
Instance_armv6_moval_pc_lsl_imm_S_rdPC_rm_shfimml,
Instance_armv6_moval_pc_lsr_imm_S_rdPC_rm_shfimmr,
Instance_armv6_moval_pc_asr_imm_S_rdPC_rm_shfimmr,
Instance_armv6_moval_pc_ror_imm_S_rdPC_rm_rotateimm,
Instance_armv6_moval_pc_rrx_S_rdPC_rm,
Instance_armv6_mvnals_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_mvnals_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnals_imm_rdNoPC_immed8,
Instance_armv6_mvnals_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv6_mvnals_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnals_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_mvnals_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnals_reg_rdNoPC_rm,
Instance_armv6_mvnals_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv6_mvnals_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnals_rrx_rdNoPC_rm,
Instance_armv6_mvnal_asr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_mvnal_asr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnal_imm_rdNoPC_immed8,
Instance_armv6_mvnal_lsl_imm_rdNoPC_rm_shfimml,
Instance_armv6_mvnal_lsl_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnal_lsr_imm_rdNoPC_rm_shfimmr,
Instance_armv6_mvnal_lsr_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnal_reg_rdNoPC_rm,
Instance_armv6_mvnal_ror_imm_rdNoPC_rm_rotateimm,
Instance_armv6_mvnal_ror_reg_rdNoPC_rmNoPC_rsNoPC,
Instance_armv6_mvnal_rrx_rdNoPC_rm,
Instance_armv6_mvnal_pc_imm_S_rdPC_immed8,
Instance_armv6_mvnal_pc_reg_S_rdPC_rm,
Instance_armv6_mvnal_pc_lsl_imm_S_rdPC_rm_shfimml,
Instance_armv6_mvnal_pc_lsr_imm_S_rdPC_rm_shfimmr,
Instance_armv6_mvnal_pc_asr_imm_S_rdPC_rm_shfimmr,
Instance_armv6_mvnal_pc_ror_imm_S_rdPC_rm_rotateimm,
Instance_armv6_mvnal_pc_rrx_S_rdPC_rm,
Instance_armv6_cmpal_asr_imm_rn_rm_shfimmr,
Instance_armv6_cmpal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmpal_imm_rn_immed8,
Instance_armv6_cmpal_lsl_imm_rn_rm_shfimml,
Instance_armv6_cmpal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmpal_lsr_imm_rn_rm_shfimmr,
Instance_armv6_cmpal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmpal_reg_rn_rm,
Instance_armv6_cmpal_ror_imm_rn_rm_rotateimm,
Instance_armv6_cmpal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmpal_rrx_rn_rm,
Instance_armv6_cmnal_asr_imm_rn_rm_shfimmr,
Instance_armv6_cmnal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmnal_imm_rn_immed8,
Instance_armv6_cmnal_lsl_imm_rn_rm_shfimml,
Instance_armv6_cmnal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmnal_lsr_imm_rn_rm_shfimmr,
Instance_armv6_cmnal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmnal_reg_rn_rm,
Instance_armv6_cmnal_ror_imm_rn_rm_rotateimm,
Instance_armv6_cmnal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_cmnal_rrx_rn_rm,
Instance_armv6_tstal_asr_imm_rn_rm_shfimmr,
Instance_armv6_tstal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tstal_imm_rn_immed8,
Instance_armv6_tstal_lsl_imm_rn_rm_shfimml,
Instance_armv6_tstal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tstal_lsr_imm_rn_rm_shfimmr,
Instance_armv6_tstal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tstal_reg_rn_rm,
Instance_armv6_tstal_ror_imm_rn_rm_rotateimm,
Instance_armv6_tstal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_tstal_rrx_rn_rm,
Instance_armv6_teqal_asr_imm_rn_rm_shfimmr,
Instance_armv6_teqal_asr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teqal_imm_rn_immed8,
Instance_armv6_teqal_lsl_imm_rn_rm_shfimml,
Instance_armv6_teqal_lsl_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teqal_lsr_imm_rn_rm_shfimmr,
Instance_armv6_teqal_lsr_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teqal_reg_rn_rm,
Instance_armv6_teqal_ror_imm_rn_rm_rotateimm,
Instance_armv6_teqal_ror_reg_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_teqal_rrx_rn_rm,
Instance_armv6_addals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_addals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addals_imm_rdNoPC_rn_immed8,
Instance_armv6_addals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_addals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_addals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addals_reg_rdNoPC_rn_rm,
Instance_armv6_addals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_addals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addals_rrx_rdNoPC_rn_rm,
Instance_armv6_addal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_addal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addal_imm_rdNoPC_rn_immed8,
Instance_armv6_addal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_addal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_addal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addal_reg_rdNoPC_rn_rm,
Instance_armv6_addal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_addal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_addal_rrx_rdNoPC_rn_rm,
Instance_armv6_addal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_addal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_addal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_addal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_addal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_addal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_addal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_subals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_subals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subals_imm_rdNoPC_rn_immed8,
Instance_armv6_subals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_subals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_subals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subals_reg_rdNoPC_rn_rm,
Instance_armv6_subals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_subals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subals_rrx_rdNoPC_rn_rm,
Instance_armv6_subal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_subal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subal_imm_rdNoPC_rn_immed8,
Instance_armv6_subal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_subal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_subal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subal_reg_rdNoPC_rn_rm,
Instance_armv6_subal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_subal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_subal_rrx_rdNoPC_rn_rm,
Instance_armv6_subal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_subal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_subal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_subal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_subal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_subal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_subal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_rsbals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsbals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbals_imm_rdNoPC_rn_immed8,
Instance_armv6_rsbals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_rsbals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsbals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbals_reg_rdNoPC_rn_rm,
Instance_armv6_rsbals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rsbals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbals_rrx_rdNoPC_rn_rm,
Instance_armv6_rsbal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsbal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbal_imm_rdNoPC_rn_immed8,
Instance_armv6_rsbal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_rsbal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rsbal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbal_reg_rdNoPC_rn_rm,
Instance_armv6_rsbal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rsbal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rsbal_rrx_rdNoPC_rn_rm,
Instance_armv6_rsbal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_rsbal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_rsbal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_rsbal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rsbal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rsbal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_rsbal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_adcals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adcals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcals_imm_rdNoPC_rn_immed8,
Instance_armv6_adcals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_adcals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adcals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcals_reg_rdNoPC_rn_rm,
Instance_armv6_adcals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_adcals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcals_rrx_rdNoPC_rn_rm,
Instance_armv6_adcal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adcal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcal_imm_rdNoPC_rn_immed8,
Instance_armv6_adcal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_adcal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_adcal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcal_reg_rdNoPC_rn_rm,
Instance_armv6_adcal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_adcal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_adcal_rrx_rdNoPC_rn_rm,
Instance_armv6_adcal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_adcal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_adcal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_adcal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_adcal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_adcal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_adcal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_sbcals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbcals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcals_imm_rdNoPC_rn_immed8,
Instance_armv6_sbcals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_sbcals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbcals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcals_reg_rdNoPC_rn_rm,
Instance_armv6_sbcals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_sbcals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcals_rrx_rdNoPC_rn_rm,
Instance_armv6_sbcal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbcal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcal_imm_rdNoPC_rn_immed8,
Instance_armv6_sbcal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_sbcal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_sbcal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcal_reg_rdNoPC_rn_rm,
Instance_armv6_sbcal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_sbcal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_sbcal_rrx_rdNoPC_rn_rm,
Instance_armv6_sbcal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_sbcal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_sbcal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_sbcal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_sbcal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_sbcal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_sbcal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_rscals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rscals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscals_imm_rdNoPC_rn_immed8,
Instance_armv6_rscals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_rscals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rscals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscals_reg_rdNoPC_rn_rm,
Instance_armv6_rscals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rscals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscals_rrx_rdNoPC_rn_rm,
Instance_armv6_rscal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rscal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscal_imm_rdNoPC_rn_immed8,
Instance_armv6_rscal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_rscal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_rscal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscal_reg_rdNoPC_rn_rm,
Instance_armv6_rscal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_rscal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_rscal_rrx_rdNoPC_rn_rm,
Instance_armv6_rscal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_rscal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_rscal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_rscal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rscal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_rscal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_rscal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_andals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_andals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andals_imm_rdNoPC_rn_immed8,
Instance_armv6_andals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_andals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_andals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andals_reg_rdNoPC_rn_rm,
Instance_armv6_andals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_andals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andals_rrx_rdNoPC_rn_rm,
Instance_armv6_andal_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_andal_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andal_imm_rdNoPC_rn_immed8,
Instance_armv6_andal_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_andal_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andal_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_andal_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andal_reg_rdNoPC_rn_rm,
Instance_armv6_andal_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_andal_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_andal_rrx_rdNoPC_rn_rm,
Instance_armv6_andal_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_andal_pc_reg_S_rdPC_rn_rm,
Instance_armv6_andal_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_andal_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_andal_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_andal_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_andal_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_bicals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bicals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bicals_imm_rdNoPC_rn_immed8,
Instance_armv6_bicals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_bicals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bicals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bicals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bicals_reg_rdNoPC_rn_rm,
Instance_armv6_bicals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_bicals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bicals_rrx_rdNoPC_rn_rm,
Instance_armv6_bical_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bical_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bical_imm_rdNoPC_rn_immed8,
Instance_armv6_bical_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_bical_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bical_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_bical_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bical_reg_rdNoPC_rn_rm,
Instance_armv6_bical_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_bical_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_bical_rrx_rdNoPC_rn_rm,
Instance_armv6_bical_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_bical_pc_reg_S_rdPC_rn_rm,
Instance_armv6_bical_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_bical_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_bical_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_bical_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_bical_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_eorals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eorals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eorals_imm_rdNoPC_rn_immed8,
Instance_armv6_eorals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_eorals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eorals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eorals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eorals_reg_rdNoPC_rn_rm,
Instance_armv6_eorals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_eorals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eorals_rrx_rdNoPC_rn_rm,
Instance_armv6_eoral_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eoral_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eoral_imm_rdNoPC_rn_immed8,
Instance_armv6_eoral_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_eoral_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eoral_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_eoral_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eoral_reg_rdNoPC_rn_rm,
Instance_armv6_eoral_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_eoral_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_eoral_rrx_rdNoPC_rn_rm,
Instance_armv6_eoral_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_eoral_pc_reg_S_rdPC_rn_rm,
Instance_armv6_eoral_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_eoral_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_eoral_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_eoral_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_eoral_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_orrals_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orrals_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrals_imm_rdNoPC_rn_immed8,
Instance_armv6_orrals_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_orrals_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrals_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orrals_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrals_reg_rdNoPC_rn_rm,
Instance_armv6_orrals_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_orrals_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orrals_rrx_rdNoPC_rn_rm,
Instance_armv6_orral_asr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orral_asr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orral_imm_rdNoPC_rn_immed8,
Instance_armv6_orral_lsl_imm_rdNoPC_rn_rm_shfimml,
Instance_armv6_orral_lsl_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orral_lsr_imm_rdNoPC_rn_rm_shfimmr,
Instance_armv6_orral_lsr_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orral_reg_rdNoPC_rn_rm,
Instance_armv6_orral_ror_imm_rdNoPC_rn_rm_rotateimm,
Instance_armv6_orral_ror_reg_rdNoPC_rnNoPC_rmNoPC_rsNoPC,
Instance_armv6_orral_rrx_rdNoPC_rn_rm,
Instance_armv6_orral_pc_imm_S_rdPC_rn_immed8,
Instance_armv6_orral_pc_reg_S_rdPC_rn_rm,
Instance_armv6_orral_pc_lsl_imm_S_rdPC_rn_rm_shfimml,
Instance_armv6_orral_pc_lsr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_orral_pc_asr_imm_S_rdPC_rn_rm_shfimmr,
Instance_armv6_orral_pc_ror_imm_S_rdPC_rn_rm_rotateimm,
Instance_armv6_orral_pc_rrx_S_rdPC_rn_rm,
Instance_armv6_mlaals_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_mlaal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_mulals_rdmul_rmNoPC_rsNoPC,
Instance_armv6_mulal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smlalals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlalal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smullals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smullal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umlalals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umlalal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umullals_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_umullal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_clzal_rdNoPC_rmNoPC,
Instance_armv6_mrsal_cpsr_rdNoPC,
Instance_armv6_mrsal_spsr_rdNoPC,
Instance_armv6_msral_cpsr_imm_fields_immed8,
Instance_armv6_msral_cpsr_imm_f_immed8,
Instance_armv6_msral_cpsr_reg_fields_rm,
Instance_armv6_msral_cpsr_reg_f_rm,
Instance_armv6_msral_spsr_imm_fields_immed8,
Instance_armv6_msral_spsr_reg_fields_rm,
Instance_armv6_ldral_addi_rdNoPC_rn_offset13,
Instance_armv6_ldral_subi_rdNoPC_rn_offset12,
Instance_armv6_ldral_add_rdNoPC_rn_rmNoPC,
Instance_armv6_ldral_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldral_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldral_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_ldral_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv6_ldral_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_ldral_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldral_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldral_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_ldral_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldral_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldral_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldral_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldral_addpc_rdNoPC_offset12PC,
Instance_armv6_ldral_subpc_rdNoPC_offset12PC,
Instance_armv6_ldral_pc_addi_rdPC_rn_offset13,
Instance_armv6_ldral_pc_subi_rdPC_rn_offset12,
Instance_armv6_ldral_pc_add_rdPC_rn_rmNoPC,
Instance_armv6_ldral_pc_add_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_add_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_ldral_pc_add_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_add_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldral_pc_add_rrx_rdPC_rn_rmNoPC,
Instance_armv6_ldral_pc_addsh2_rdPC_rn_rmNoPC,
Instance_armv6_ldral_pc_sub_rdPC_rn_rmNoPC,
Instance_armv6_ldral_pc_sub_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_sub_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_ldral_pc_sub_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_sub_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldral_pc_sub_rrx_rdPC_rn_rmNoPC,
Instance_armv6_ldral_pc_post_add_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_post_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_post_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_pc_post_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_post_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_pc_post_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_post_addi_rdPC_rnNoPC_offset13,
Instance_armv6_ldral_pc_post_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_post_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_post_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_post_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_pc_post_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_post_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_pc_post_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_post_subi_rdPC_rnNoPC_offset12,
Instance_armv6_ldral_pc_pre_add_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_pre_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_pre_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_pc_pre_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_pre_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_pc_pre_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_pre_addi_rdPC_rnNoPC_offset13,
Instance_armv6_ldral_pc_pre_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_pre_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_pre_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_pre_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldral_pc_pre_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldral_pc_pre_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldral_pc_pre_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_ldral_pc_pre_subi_rdPC_rnNoPC_offset12,
Instance_armv6_ldral_pc_addpc_rdPC_offset12PC,
Instance_armv6_ldral_pc_subpc_rdPC_offset12PC,
Instance_armv6_ldralb_addi_rdNoPC_rn_offset13,
Instance_armv6_ldralb_subi_rdNoPC_rn_offset12,
Instance_armv6_ldralb_add_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralb_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldralb_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldralb_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldralb_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldralb_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralb_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralb_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralb_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldralb_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_ldralb_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_ldralb_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_ldralb_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralb_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralb_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralb_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldralb_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralb_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralb_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldralb_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralb_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralb_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldralb_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralb_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralb_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralb_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralb_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldralb_addpc_rdNoPC_offset12PC,
Instance_armv6_ldralb_subpc_rdNoPC_offset12PC,
Instance_armv6_stral_addi_rdNoPC_rn_offset13,
Instance_armv6_stral_subi_rdNoPC_rn_offset12,
Instance_armv6_stral_add_rdNoPC_rn_rmNoPC,
Instance_armv6_stral_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_stral_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_stral_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_stral_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv6_stral_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_stral_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_stral_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_stral_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_stral_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_stral_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_stral_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_stral_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_stral_addpc_rdNoPC_offset12PC,
Instance_armv6_stral_subpc_rdNoPC_offset12PC,
Instance_armv6_stral_pc_addi_rdPC_rn_offset13,
Instance_armv6_stral_pc_subi_rdPC_rn_offset12,
Instance_armv6_stral_pc_add_rdPC_rn_rmNoPC,
Instance_armv6_stral_pc_add_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_pc_add_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_stral_pc_add_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_pc_add_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_stral_pc_add_rrx_rdPC_rn_rmNoPC,
Instance_armv6_stral_pc_addsh2_rdPC_rn_rmNoPC,
Instance_armv6_stral_pc_sub_rdPC_rn_rmNoPC,
Instance_armv6_stral_pc_sub_asr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_pc_sub_lsl_rdPC_rn_rmNoPC_shfimml,
Instance_armv6_stral_pc_sub_lsr_rdPC_rn_rmNoPC_shfimmr,
Instance_armv6_stral_pc_sub_ror_rdPC_rn_rmNoPC_rotateimm,
Instance_armv6_stral_pc_sub_rrx_rdPC_rn_rmNoPC,
Instance_armv6_stral_pc_post_add_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_post_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_post_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_pc_post_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_post_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_pc_post_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_post_addi_rdPC_rnNoPC_offset13,
Instance_armv6_stral_pc_post_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_post_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_post_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_post_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_pc_post_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_post_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_pc_post_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_post_subi_rdPC_rnNoPC_offset12,
Instance_armv6_stral_pc_pre_add_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_pre_add_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_pre_add_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_pc_pre_add_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_pre_add_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_pc_pre_add_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_pre_addi_rdPC_rnNoPC_offset13,
Instance_armv6_stral_pc_pre_addsh2_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_pre_sub_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_pre_sub_asr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_pre_sub_lsl_rdPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stral_pc_pre_sub_lsr_rdPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stral_pc_pre_sub_ror_rdPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stral_pc_pre_sub_rrx_rdPC_rnNoPC_rmNoPC,
Instance_armv6_stral_pc_pre_subi_rdPC_rnNoPC_offset12,
Instance_armv6_stral_pc_addpc_rdPC_offset12PC,
Instance_armv6_stral_pc_subpc_rdPC_offset12PC,
Instance_armv6_stralb_addi_rdNoPC_rn_offset13,
Instance_armv6_stralb_subi_rdNoPC_rn_offset12,
Instance_armv6_stralb_add_rdNoPC_rn_rmNoPC,
Instance_armv6_stralb_add_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stralb_add_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_stralb_add_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stralb_add_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_stralb_add_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_stralb_addsh2_rdNoPC_rn_rmNoPC,
Instance_armv6_stralb_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_stralb_sub_asr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stralb_sub_lsl_rdNoPC_rn_rmNoPC_shfimml,
Instance_armv6_stralb_sub_lsr_rdNoPC_rn_rmNoPC_shfimmr,
Instance_armv6_stralb_sub_ror_rdNoPC_rn_rmNoPC_rotateimm,
Instance_armv6_stralb_sub_rrx_rdNoPC_rn_rmNoPC,
Instance_armv6_stralb_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_post_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_post_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralb_post_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_post_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralb_post_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_post_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_stralb_post_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_post_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_post_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralb_post_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_post_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralb_post_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_post_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_stralb_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_pre_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_pre_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralb_pre_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_pre_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralb_pre_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_pre_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_stralb_pre_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_pre_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_pre_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralb_pre_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralb_pre_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralb_pre_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralb_pre_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_stralb_addpc_rdNoPC_offset12PC,
Instance_armv6_stralb_subpc_rdNoPC_offset12PC,
Instance_armv6_ldralbt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralbt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralbt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralbt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralbt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralbt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralbt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldralbt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralbt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralbt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralbt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralbt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralbt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralbt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralbt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldralt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_ldralt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_ldralt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_ldralt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_ldralt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_stralbt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralbt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralbt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralbt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralbt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralbt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralbt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_stralbt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralbt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralbt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralbt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralbt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralbt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralbt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralbt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_stralt_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralt_add_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralt_add_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralt_add_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralt_add_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralt_add_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralt_addi_rdNoPC_rnNoPC_offset13,
Instance_armv6_stralt_addsh2_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralt_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralt_sub_asr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralt_sub_lsl_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_stralt_sub_lsr_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_stralt_sub_ror_rdNoPC_rnNoPC_rmNoPC_rotateimm,
Instance_armv6_stralt_sub_rrx_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralt_subi_rdNoPC_rnNoPC_offset12,
Instance_armv6_ldralh_addi_rdNoPC_rn_offset9,
Instance_armv6_ldralh_subi_rdNoPC_rn_offset8,
Instance_armv6_ldralh_add_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralh_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralh_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralh_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldralh_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralh_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldralh_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralh_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldralh_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralh_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldralh_addpc_rdNoPC_offset8PC,
Instance_armv6_ldralh_subpc_rdNoPC_offset8PC,
Instance_armv6_ldralsb_addi_rdNoPC_rn_offset9,
Instance_armv6_ldralsb_subi_rdNoPC_rn_offset8,
Instance_armv6_ldralsb_add_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralsb_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralsb_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsb_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldralsb_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsb_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldralsb_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsb_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldralsb_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsb_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldralsb_addpc_rdNoPC_offset8PC,
Instance_armv6_ldralsb_subpc_rdNoPC_offset8PC,
Instance_armv6_ldralsh_addi_rdNoPC_rn_offset9,
Instance_armv6_ldralsh_subi_rdNoPC_rn_offset8,
Instance_armv6_ldralsh_add_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralsh_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_ldralsh_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsh_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldralsh_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsh_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldralsh_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsh_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_ldralsh_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ldralsh_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_ldralsh_addpc_rdNoPC_offset8PC,
Instance_armv6_ldralsh_subpc_rdNoPC_offset8PC,
Instance_armv6_stralh_addi_rdNoPC_rn_offset9,
Instance_armv6_stralh_subi_rdNoPC_rn_offset8,
Instance_armv6_stralh_add_rdNoPC_rn_rmNoPC,
Instance_armv6_stralh_sub_rdNoPC_rn_rmNoPC,
Instance_armv6_stralh_post_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralh_post_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_stralh_post_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralh_post_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_stralh_pre_add_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralh_pre_addi_rdNoPC_rnNoPC_offset9,
Instance_armv6_stralh_pre_sub_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_stralh_pre_subi_rdNoPC_rnNoPC_offset8,
Instance_armv6_stralh_addpc_rdNoPC_offset8PC,
Instance_armv6_stralh_subpc_rdNoPC_offset8PC,
Instance_armv6_ldmal_amode4L_rnNoPC_reglistNoPC,
Instance_armv6_ldmal_w_amode4L_rnNoPC_reglistNoPC,
Instance_armv6_ldmal_pc_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_ldmal_pc_w_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_ldmal_s10_amode4L_rnNoPC_reglistNoPC,
Instance_armv6_ldmal_s11_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_ldmal_s11_w_amode4L_rnNoPC_reglistWithPC,
Instance_armv6_stmal_amode4S_rnNoPC_reglist,
Instance_armv6_stmal_w_amode4S_rnNoPC_reglist,
Instance_armv6_stmal_s1_amode4S_rnNoPC_reglist,
Instance_armv6_swpal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_swpalb_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_swial_swinum,
Instance_armv6_cdpal_cpnum_opcode1d_crd_crn_crm_opcode2,
Instance_armv6_ldcal_add_cpnum_crd_rn_offset8CP,
Instance_armv6_ldcal_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_ldcal_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcal_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcal_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcal_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcal_unindexed_cpnum_crd_rn_option,
Instance_armv6_ldcall_add_cpnum_crd_rn_offset8CP,
Instance_armv6_ldcall_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_ldcall_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcall_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcall_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcall_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_ldcall_unindexed_cpnum_crd_rn_option,
Instance_armv6_stcal_add_cpnum_crd_rn_offset8CP,
Instance_armv6_stcal_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_stcal_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcal_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcal_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcal_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcal_unindexed_cpnum_crd_rn_option,
Instance_armv6_stcall_add_cpnum_crd_rn_offset8CP,
Instance_armv6_stcall_sub_cpnum_crd_rn_offset8CP,
Instance_armv6_stcall_post_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcall_post_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcall_pre_add_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcall_pre_sub_cpnum_crd_rnNoPC_offset8CP,
Instance_armv6_stcall_unindexed_cpnum_crd_rn_option,
Instance_armv6_mcral_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv6_mrcal_cpnum_opcode1m_rdNoPC_crn_crm_opcode2,
Instance_armv6_mrcal_pc_cpnum_opcode1m_crn_crm_opcode2,
Instance_armv6_smlabbal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlabtal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlatbal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlattal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlalbbal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlalbtal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlaltbal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlalttal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smulbbal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smulbtal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smultbal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smulttal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smlawbal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlawtal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smulwbal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smulwtal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_qaddal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_qdaddal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_qdsubal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_qsubal_rdNoPC_rmNoPC_rnNoPC,
Instance_armv6_ldrald_addi_rdPaired_rn_offset9,
Instance_armv6_ldrald_subi_rdPaired_rn_offset8,
Instance_armv6_ldrald_add_rdPaired_rn_rmNoPC,
Instance_armv6_ldrald_sub_rdPaired_rn_rmNoPC,
Instance_armv6_ldrald_post_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrald_post_addi_rdPaired_rnNoPC_offset9,
Instance_armv6_ldrald_post_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrald_post_subi_rdPaired_rnNoPC_offset8,
Instance_armv6_ldrald_pre_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrald_pre_addi_rdPaired_rnNoPC_offset9,
Instance_armv6_ldrald_pre_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_ldrald_pre_subi_rdPaired_rnNoPC_offset8,
Instance_armv6_ldrald_addpc_rdPaired_offset8PC,
Instance_armv6_ldrald_subpc_rdPaired_offset8PC,
Instance_armv6_strald_addi_rdPaired_rn_offset9,
Instance_armv6_strald_subi_rdPaired_rn_offset8,
Instance_armv6_strald_add_rdPaired_rn_rmNoPC,
Instance_armv6_strald_sub_rdPaired_rn_rmNoPC,
Instance_armv6_strald_post_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strald_post_addi_rdPaired_rnNoPC_offset9,
Instance_armv6_strald_post_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strald_post_subi_rdPaired_rnNoPC_offset8,
Instance_armv6_strald_pre_add_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strald_pre_addi_rdPaired_rnNoPC_offset9,
Instance_armv6_strald_pre_sub_rdPaired_rnNoPC_rmNoPC,
Instance_armv6_strald_pre_subi_rdPaired_rnNoPC_offset8,
Instance_armv6_strald_addpc_rdPaired_offset8PC,
Instance_armv6_strald_subpc_rdPaired_offset8PC,
Instance_armv6_mcrral_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv6_mrrcal_cpnum_opcode2_rdNoPC_rnNoPC_crm,
Instance_armv6_sadd16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_saddsubxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ssub16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ssubaddxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uadd16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uaddsubxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_usub16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_usubaddxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qadd16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qaddsubxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qsub16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qsubaddxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqadd16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqaddsubxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqsub16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqsubaddxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shadd16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shaddsubxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shsub16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shsubaddxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhadd16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhaddsubxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhsub16al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhsubaddxal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sadd8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_ssub8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uadd8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_usub8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qadd8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_qsub8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqadd8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uqsub8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shadd8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_shsub8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhadd8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uhsub8al_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_selal_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_pkhbtal_lsl_imm_rdNoPC_rnNoPC_rmNoPC_shfimml,
Instance_armv6_pkhbtal_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_pkhtbal_rdNoPC_rnNoPC_rmNoPC_shfimmr,
Instance_armv6_sxtbal_reg_rdNoPC_rmNoPC,
Instance_armv6_sxtbal_ror_rdNoPC_rmNoPC_rot,
Instance_armv6_sxtb16al_reg_rdNoPC_rmNoPC,
Instance_armv6_sxtb16al_ror_rdNoPC_rmNoPC_rot,
Instance_armv6_sxthal_reg_rdNoPC_rmNoPC,
Instance_armv6_sxthal_ror_rdNoPC_rmNoPC_rot,
Instance_armv6_uxtbal_reg_rdNoPC_rmNoPC,
Instance_armv6_uxtbal_ror_rdNoPC_rmNoPC_rot,
Instance_armv6_uxtb16al_reg_rdNoPC_rmNoPC,
Instance_armv6_uxtb16al_ror_rdNoPC_rmNoPC_rot,
Instance_armv6_uxthal_reg_rdNoPC_rmNoPC,
Instance_armv6_uxthal_ror_rdNoPC_rmNoPC_rot,
Instance_armv6_sxtabal_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sxtabal_ror_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_sxtab16al_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sxtab16al_ror_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_sxtahal_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_sxtahal_ror_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_uxtabal_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uxtabal_ror_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_uxtab16al_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uxtab16al_ror_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_uxtahal_reg_rdNoPC_rnNoPC_rmNoPC,
Instance_armv6_uxtahal_ror_rdNoPC_rnNoPC_rmNoPC_rot,
Instance_armv6_ssatal_asr_imm_rdNoPC_ssat5_rmNoPC_shfimmr,
Instance_armv6_ssatal_lsl_imm_rdNoPC_ssat5_rmNoPC_shfimml,
Instance_armv6_ssatal_reg_rdNoPC_ssat5_rmNoPC,
Instance_armv6_usatal_asr_imm_rdNoPC_usat5_rmNoPC_shfimmr,
Instance_armv6_usatal_lsl_imm_rdNoPC_usat5_rmNoPC_shfimml,
Instance_armv6_usatal_reg_rdNoPC_usat5_rmNoPC,
Instance_armv6_ssat16al_rdNoPC_ssat4_rmNoPC,
Instance_armv6_usat16al_rdNoPC_usat4_rmNoPC,
Instance_armv6_usad8al_rdmul_rmNoPC_rsNoPC,
Instance_armv6_usada8al_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smuadal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smuadxal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smusdal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smusdxal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smladal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smladxal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlsdal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlsdxal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smlaldal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlaldxal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlsldal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smlsldxal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_smmlaal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmlaral_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmlsal_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmlsral_rdmul_rmNoPC_rsNoPC_rnmul,
Instance_armv6_smmulal_rdmul_rmNoPC_rsNoPC,
Instance_armv6_smmulral_rdmul_rmNoPC_rsNoPC,
Instance_armv6_umaalal_rdlo_rdhi_rmNoPC_rsNoPC,
Instance_armv6_reval_rdNoPC_rmNoPC,
Instance_armv6_rev16al_rdNoPC_rmNoPC,
Instance_armv6_revshal_rdNoPC_rmNoPC,
Instance_armv5e_ADJUST,
Instance_armv5e_GNUASM,
Instance_armv5e_SWITCH,
Instance_armv5e_nop,
Instance_armv5e_adr,
Instance_armv5e_adral,
Instance_armv5e_adrl,
Instance_armv5e_adrlal,
Instance_armv5e_ldr,
Instance_armv5e_ldral,
Instance_armv6_ADJUST,
Instance_armv6_GNUASM,
Instance_armv6_SWITCH,
Instance_armv6_nop,
Instance_armv6_adr,
Instance_armv6_adral,
Instance_armv6_adrl,
Instance_armv6_adrlal,
Instance_armv6_ldr,
Instance_armv6_ldral,

#undef Instance


#define InstanceActions(INDEX,ACTIONS) 
#undef InstanceActions


#define InstanceAction_COUNT_MAX 8


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
  arm/Operator.enum --
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


Operator__MAKE,


Operator__SEL,


Operator_arm_ADJUST,


Operator_arm_GNUASM,


Operator_arm_SWITCH,


Operator_arm_nop,


Operator_arm_adr,


Operator_arm_adral,


Operator_arm_adrl,


Operator_arm_adrlal,


Operator_arm_ldr,


Operator_arm_ldral,


Operator_arm_b_1cond_2btarg1,


Operator_arm_bl_1cond_2btarg1,


Operator_arm_blxi_1btarg2,


Operator_arm_blx_1cond_2noPC,


Operator_arm_bx_1cond_2general,


Operator_arm_movs_asr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_movs_asr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_movs_imm_2noPC_1cond_3immed8,


Operator_arm_movs_lsl_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_movs_lsl_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_movs_lsr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_movs_lsr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_movs_reg_2noPC_1cond_3general,


Operator_arm_movs_ror_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_movs_ror_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_movs_rrx_2noPC_1cond_3general,


Operator_arm_mov_asr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_mov_asr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mov_imm_2noPC_1cond_3immed8,


Operator_arm_mov_lsl_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_mov_lsl_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mov_lsr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_mov_lsr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mov_reg_2noPC_1cond_3general,


Operator_arm_mov_ror_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_mov_ror_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mov_rrx_2noPC_1cond_3general,


Operator_arm_mov_pc_imm_3pc_1cond_2S_4immed8,


Operator_arm_mov_pc_reg_3pc_1cond_2S_4general,


Operator_arm_mov_pc_lsl_imm_3pc_1cond_2S_4general_5shfimml,


Operator_arm_mov_pc_lsr_imm_3pc_1cond_2S_4general_5shfimmr,


Operator_arm_mov_pc_asr_imm_3pc_1cond_2S_4general_5shfimmr,


Operator_arm_mov_pc_ror_imm_3pc_1cond_2S_4general_5shfimml,


Operator_arm_mov_pc_rrx_3pc_1cond_2S_4general,


Operator_arm_mvns_asr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_mvns_asr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvns_imm_2noPC_1cond_3immed8,


Operator_arm_mvns_lsl_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_mvns_lsl_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvns_lsr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_mvns_lsr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvns_reg_2noPC_1cond_3general,


Operator_arm_mvns_ror_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_mvns_ror_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvns_rrx_2noPC_1cond_3general,


Operator_arm_mvn_asr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_mvn_asr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvn_imm_2noPC_1cond_3immed8,


Operator_arm_mvn_lsl_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_mvn_lsl_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvn_lsr_imm_2noPC_1cond_3general_4shfimmr,


Operator_arm_mvn_lsr_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvn_reg_2noPC_1cond_3general,


Operator_arm_mvn_ror_imm_2noPC_1cond_3general_4shfimml,


Operator_arm_mvn_ror_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_mvn_rrx_2noPC_1cond_3general,


Operator_arm_mvn_pc_imm_3pc_1cond_2S_4immed8,


Operator_arm_mvn_pc_reg_3pc_1cond_2S_4general,


Operator_arm_mvn_pc_lsl_imm_3pc_1cond_2S_4general_5shfimml,


Operator_arm_mvn_pc_lsr_imm_3pc_1cond_2S_4general_5shfimmr,


Operator_arm_mvn_pc_asr_imm_3pc_1cond_2S_4general_5shfimmr,


Operator_arm_mvn_pc_ror_imm_3pc_1cond_2S_4general_5shfimml,


Operator_arm_mvn_pc_rrx_3pc_1cond_2S_4general,


Operator_arm_cmp_asr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_cmp_asr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmp_imm_1cond_2general_3immed8,


Operator_arm_cmp_lsl_imm_1cond_2general_3general_4shfimml,


Operator_arm_cmp_lsl_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmp_lsr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_cmp_lsr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmp_reg_1cond_2general_3general,


Operator_arm_cmp_ror_imm_1cond_2general_3general_4shfimml,


Operator_arm_cmp_ror_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmp_rrx_1cond_2general_3general,


Operator_arm_cmn_asr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_cmn_asr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmn_imm_1cond_2general_3immed8,


Operator_arm_cmn_lsl_imm_1cond_2general_3general_4shfimml,


Operator_arm_cmn_lsl_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmn_lsr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_cmn_lsr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmn_reg_1cond_2general_3general,


Operator_arm_cmn_ror_imm_1cond_2general_3general_4shfimml,


Operator_arm_cmn_ror_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_cmn_rrx_1cond_2general_3general,


Operator_arm_tst_asr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_tst_asr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_tst_imm_1cond_2general_3immed8,


Operator_arm_tst_lsl_imm_1cond_2general_3general_4shfimml,


Operator_arm_tst_lsl_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_tst_lsr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_tst_lsr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_tst_reg_1cond_2general_3general,


Operator_arm_tst_ror_imm_1cond_2general_3general_4shfimml,


Operator_arm_tst_ror_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_tst_rrx_1cond_2general_3general,


Operator_arm_teq_asr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_teq_asr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_teq_imm_1cond_2general_3immed8,


Operator_arm_teq_lsl_imm_1cond_2general_3general_4shfimml,


Operator_arm_teq_lsl_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_teq_lsr_imm_1cond_2general_3general_4shfimmr,


Operator_arm_teq_lsr_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_teq_reg_1cond_2general_3general,


Operator_arm_teq_ror_imm_1cond_2general_3general_4shfimml,


Operator_arm_teq_ror_reg_1cond_2noPC_3noPC_4noPC,


Operator_arm_teq_rrx_1cond_2general_3general,


Operator_arm_adds_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_adds_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adds_imm_2noPC_1cond_3general_4immed8,


Operator_arm_adds_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_adds_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adds_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_adds_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adds_reg_2noPC_1cond_3general_4general,


Operator_arm_adds_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_adds_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adds_rrx_2noPC_1cond_3general_4general,


Operator_arm_add_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_add_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_add_imm_2noPC_1cond_3general_4immed8,


Operator_arm_add_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_add_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_add_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_add_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_add_reg_2noPC_1cond_3general_4general,


Operator_arm_add_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_add_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_add_rrx_2noPC_1cond_3general_4general,


Operator_arm_add_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_add_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_add_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_add_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_add_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_add_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_add_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_subs_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_subs_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_subs_imm_2noPC_1cond_3general_4immed8,


Operator_arm_subs_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_subs_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_subs_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_subs_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_subs_reg_2noPC_1cond_3general_4general,


Operator_arm_subs_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_subs_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_subs_rrx_2noPC_1cond_3general_4general,


Operator_arm_sub_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_sub_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sub_imm_2noPC_1cond_3general_4immed8,


Operator_arm_sub_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_sub_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sub_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_sub_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sub_reg_2noPC_1cond_3general_4general,


Operator_arm_sub_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_sub_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sub_rrx_2noPC_1cond_3general_4general,


Operator_arm_sub_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_sub_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_sub_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_sub_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_sub_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_sub_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_sub_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_rsbs_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rsbs_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsbs_imm_2noPC_1cond_3general_4immed8,


Operator_arm_rsbs_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rsbs_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsbs_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rsbs_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsbs_reg_2noPC_1cond_3general_4general,


Operator_arm_rsbs_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rsbs_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsbs_rrx_2noPC_1cond_3general_4general,


Operator_arm_rsb_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rsb_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsb_imm_2noPC_1cond_3general_4immed8,


Operator_arm_rsb_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rsb_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsb_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rsb_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsb_reg_2noPC_1cond_3general_4general,


Operator_arm_rsb_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rsb_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsb_rrx_2noPC_1cond_3general_4general,


Operator_arm_rsb_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_rsb_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_rsb_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_rsb_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_rsb_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_rsb_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_rsb_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_adcs_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_adcs_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adcs_imm_2noPC_1cond_3general_4immed8,


Operator_arm_adcs_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_adcs_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adcs_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_adcs_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adcs_reg_2noPC_1cond_3general_4general,


Operator_arm_adcs_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_adcs_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adcs_rrx_2noPC_1cond_3general_4general,


Operator_arm_adc_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_adc_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adc_imm_2noPC_1cond_3general_4immed8,


Operator_arm_adc_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_adc_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adc_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_adc_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adc_reg_2noPC_1cond_3general_4general,


Operator_arm_adc_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_adc_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_adc_rrx_2noPC_1cond_3general_4general,


Operator_arm_adc_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_adc_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_adc_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_adc_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_adc_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_adc_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_adc_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_sbcs_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_sbcs_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbcs_imm_2noPC_1cond_3general_4immed8,


Operator_arm_sbcs_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_sbcs_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbcs_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_sbcs_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbcs_reg_2noPC_1cond_3general_4general,


Operator_arm_sbcs_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_sbcs_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbcs_rrx_2noPC_1cond_3general_4general,


Operator_arm_sbc_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_sbc_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbc_imm_2noPC_1cond_3general_4immed8,


Operator_arm_sbc_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_sbc_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbc_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_sbc_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbc_reg_2noPC_1cond_3general_4general,


Operator_arm_sbc_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_sbc_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_sbc_rrx_2noPC_1cond_3general_4general,


Operator_arm_sbc_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_sbc_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_sbc_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_sbc_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_sbc_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_sbc_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_sbc_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_rscs_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rscs_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rscs_imm_2noPC_1cond_3general_4immed8,


Operator_arm_rscs_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rscs_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rscs_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rscs_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rscs_reg_2noPC_1cond_3general_4general,


Operator_arm_rscs_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rscs_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rscs_rrx_2noPC_1cond_3general_4general,


Operator_arm_rsc_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rsc_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsc_imm_2noPC_1cond_3general_4immed8,


Operator_arm_rsc_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rsc_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsc_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_rsc_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsc_reg_2noPC_1cond_3general_4general,


Operator_arm_rsc_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_rsc_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_rsc_rrx_2noPC_1cond_3general_4general,


Operator_arm_rsc_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_rsc_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_rsc_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_rsc_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_rsc_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_rsc_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_rsc_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_ands_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_ands_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_ands_imm_2noPC_1cond_3general_4immed8,


Operator_arm_ands_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_ands_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_ands_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_ands_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_ands_reg_2noPC_1cond_3general_4general,


Operator_arm_ands_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_ands_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_ands_rrx_2noPC_1cond_3general_4general,


Operator_arm_and_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_and_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_and_imm_2noPC_1cond_3general_4immed8,


Operator_arm_and_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_and_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_and_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_and_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_and_reg_2noPC_1cond_3general_4general,


Operator_arm_and_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_and_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_and_rrx_2noPC_1cond_3general_4general,


Operator_arm_and_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_and_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_and_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_and_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_and_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_and_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_and_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_bics_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_bics_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bics_imm_2noPC_1cond_3general_4immed8,


Operator_arm_bics_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_bics_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bics_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_bics_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bics_reg_2noPC_1cond_3general_4general,


Operator_arm_bics_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_bics_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bics_rrx_2noPC_1cond_3general_4general,


Operator_arm_bic_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_bic_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bic_imm_2noPC_1cond_3general_4immed8,


Operator_arm_bic_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_bic_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bic_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_bic_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bic_reg_2noPC_1cond_3general_4general,


Operator_arm_bic_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_bic_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_bic_rrx_2noPC_1cond_3general_4general,


Operator_arm_bic_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_bic_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_bic_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_bic_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_bic_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_bic_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_bic_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_eors_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_eors_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eors_imm_2noPC_1cond_3general_4immed8,


Operator_arm_eors_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_eors_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eors_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_eors_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eors_reg_2noPC_1cond_3general_4general,


Operator_arm_eors_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_eors_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eors_rrx_2noPC_1cond_3general_4general,


Operator_arm_eor_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_eor_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eor_imm_2noPC_1cond_3general_4immed8,


Operator_arm_eor_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_eor_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eor_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_eor_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eor_reg_2noPC_1cond_3general_4general,


Operator_arm_eor_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_eor_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_eor_rrx_2noPC_1cond_3general_4general,


Operator_arm_eor_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_eor_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_eor_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_eor_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_eor_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_eor_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_eor_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_orrs_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_orrs_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orrs_imm_2noPC_1cond_3general_4immed8,


Operator_arm_orrs_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_orrs_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orrs_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_orrs_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orrs_reg_2noPC_1cond_3general_4general,


Operator_arm_orrs_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_orrs_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orrs_rrx_2noPC_1cond_3general_4general,


Operator_arm_orr_asr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_orr_asr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orr_imm_2noPC_1cond_3general_4immed8,


Operator_arm_orr_lsl_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_orr_lsl_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orr_lsr_imm_2noPC_1cond_3general_4general_5shfimmr,


Operator_arm_orr_lsr_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orr_reg_2noPC_1cond_3general_4general,


Operator_arm_orr_ror_imm_2noPC_1cond_3general_4general_5shfimml,


Operator_arm_orr_ror_reg_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_orr_rrx_2noPC_1cond_3general_4general,


Operator_arm_orr_pc_imm_3pc_1cond_2S_4general_5immed8,


Operator_arm_orr_pc_reg_3pc_1cond_2S_4general_5general,


Operator_arm_orr_pc_lsl_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_orr_pc_lsr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_orr_pc_asr_imm_3pc_1cond_2S_4general_5general_6shfimmr,


Operator_arm_orr_pc_ror_imm_3pc_1cond_2S_4general_5general_6shfimml,


Operator_arm_orr_pc_rrx_3pc_1cond_2S_4general_5general,


Operator_arm_mlas_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_mla_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_muls_2noPC_1cond_3noPC_4noPC,


Operator_arm_mul_2noPC_1cond_3noPC_4noPC,


Operator_arm_smlals_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlal_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smulls_2noPC_3noPC_1cond_4noPC_5noPC,


Operator_arm_smull_2noPC_3noPC_1cond_4noPC_5noPC,


Operator_arm_umlals_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_umlal_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_umulls_2noPC_3noPC_1cond_4noPC_5noPC,


Operator_arm_umull_2noPC_3noPC_1cond_4noPC_5noPC,


Operator_arm_clz_2noPC_1cond_3noPC,


Operator_arm_mrs_cpsr_2noPC_1cond,


Operator_arm_mrs_spsr_2noPC_1cond,


Operator_arm_msr_cpsr_imm_1cond_2psrfld_3immed8,


Operator_arm_msr_cpsr_imm_f_1cond_2immed8,


Operator_arm_msr_cpsr_reg_1cond_2psrfld_3general,


Operator_arm_msr_cpsr_reg_f_1cond_2general,


Operator_arm_msr_spsr_imm_1cond_2psrfld_3immed8,


Operator_arm_msr_spsr_reg_1cond_2psrfld_3general,


Operator_arm_ldr_addi_2noPC_1cond_3general_4s13,


Operator_arm_ldr_subi_2noPC_1cond_3general_4offset12,


Operator_arm_ldr_add_2noPC_1cond_3general_4noPC,


Operator_arm_ldr_add_asr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_add_lsl_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_add_lsr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_add_ror_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_add_rrx_2noPC_1cond_3general_4noPC,


Operator_arm_ldr_addsh2_2noPC_1cond_3general_4noPC,


Operator_arm_ldr_sub_2noPC_1cond_3general_4noPC,


Operator_arm_ldr_sub_asr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_sub_lsl_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_sub_lsr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_sub_ror_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_sub_rrx_2noPC_1cond_3general_4noPC,


Operator_arm_ldr_post_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_post_add_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_post_add_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_post_add_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_post_add_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_post_add_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_post_addi_2noPC_3noPC_1cond_3noPC_4s13,


Operator_arm_ldr_post_addsh2_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_post_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_post_sub_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_post_sub_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_post_sub_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_post_sub_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_post_sub_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_post_subi_2noPC_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldr_pre_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pre_add_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pre_add_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pre_add_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pre_add_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pre_add_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pre_addi_2noPC_3noPC_1cond_3noPC_4s13,


Operator_arm_ldr_pre_addsh2_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pre_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pre_sub_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pre_sub_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pre_sub_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pre_sub_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pre_sub_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pre_subi_2noPC_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldr_addpc_2noPC_1cond_3offset12PC,


Operator_arm_ldr_subpc_2noPC_1cond_3offset12PC,


Operator_arm_ldr_pc_addi_2pc_1cond_3general_4s13,


Operator_arm_ldr_pc_subi_2pc_1cond_3general_4offset12,


Operator_arm_ldr_pc_add_2pc_1cond_3general_4noPC,


Operator_arm_ldr_pc_add_asr_2pc_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_pc_add_lsl_2pc_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_pc_add_lsr_2pc_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_pc_add_ror_2pc_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_pc_add_rrx_2pc_1cond_3general_4noPC,


Operator_arm_ldr_pc_addsh2_2pc_1cond_3general_4noPC,


Operator_arm_ldr_pc_sub_2pc_1cond_3general_4noPC,


Operator_arm_ldr_pc_sub_asr_2pc_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_pc_sub_lsl_2pc_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_pc_sub_lsr_2pc_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldr_pc_sub_ror_2pc_1cond_3general_4noPC_5shfimml,


Operator_arm_ldr_pc_sub_rrx_2pc_1cond_3general_4noPC,


Operator_arm_ldr_pc_post_add_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_post_add_asr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_post_add_lsl_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_post_add_lsr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_post_add_ror_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_post_add_rrx_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_post_addi_2pc_3noPC_1cond_3noPC_4s13,


Operator_arm_ldr_pc_post_addsh2_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_post_sub_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_post_sub_asr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_post_sub_lsl_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_post_sub_lsr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_post_sub_ror_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_post_sub_rrx_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_post_subi_2pc_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldr_pc_pre_add_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_pre_add_asr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_pre_add_lsl_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_pre_add_lsr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_pre_add_ror_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_pre_add_rrx_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_pre_addi_2pc_3noPC_1cond_3noPC_4s13,


Operator_arm_ldr_pc_pre_addsh2_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_pre_sub_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_pre_sub_asr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_pre_sub_lsl_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_pre_sub_lsr_2pc_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldr_pc_pre_sub_ror_2pc_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldr_pc_pre_sub_rrx_2pc_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldr_pc_pre_subi_2pc_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldr_pc_addpc_2pc_1cond_3offset12PC,


Operator_arm_ldr_pc_subpc_2pc_1cond_3offset12PC,


Operator_arm_ldrb_addi_2noPC_1cond_3general_4s13,


Operator_arm_ldrb_subi_2noPC_1cond_3general_4offset12,


Operator_arm_ldrb_add_2noPC_1cond_3general_4noPC,


Operator_arm_ldrb_add_asr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldrb_add_lsl_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldrb_add_lsr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldrb_add_ror_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldrb_add_rrx_2noPC_1cond_3general_4noPC,


Operator_arm_ldrb_addsh2_2noPC_1cond_3general_4noPC,


Operator_arm_ldrb_sub_2noPC_1cond_3general_4noPC,


Operator_arm_ldrb_sub_asr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldrb_sub_lsl_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldrb_sub_lsr_2noPC_1cond_3general_4noPC_5shfimmr,


Operator_arm_ldrb_sub_ror_2noPC_1cond_3general_4noPC_5shfimml,


Operator_arm_ldrb_sub_rrx_2noPC_1cond_3general_4noPC,


Operator_arm_ldrb_post_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_post_add_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_post_add_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_post_add_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_post_add_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_post_add_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_post_addi_2noPC_3noPC_1cond_3noPC_4s13,


Operator_arm_ldrb_post_addsh2_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_post_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_post_sub_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_post_sub_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_post_sub_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_post_sub_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_post_sub_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_post_subi_2noPC_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldrb_pre_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_pre_add_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_pre_add_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_pre_add_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_pre_add_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_pre_add_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_pre_addi_2noPC_3noPC_1cond_3noPC_4s13,


Operator_arm_ldrb_pre_addsh2_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_pre_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_pre_sub_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_pre_sub_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_pre_sub_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrb_pre_sub_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrb_pre_sub_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrb_pre_subi_2noPC_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldrb_addpc_2noPC_1cond_3offset12PC,


Operator_arm_ldrb_subpc_2noPC_1cond_3offset12PC,


Operator_arm_str_addi_1cond_2noPC_3general_4s13,


Operator_arm_str_subi_1cond_2noPC_3general_4offset12,


Operator_arm_str_add_1cond_2noPC_3general_4noPC,


Operator_arm_str_add_asr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_str_add_lsl_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_str_add_lsr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_str_add_ror_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_str_add_rrx_1cond_2noPC_3general_4noPC,


Operator_arm_str_addsh2_1cond_2noPC_3general_4noPC,


Operator_arm_str_sub_1cond_2noPC_3general_4noPC,


Operator_arm_str_sub_asr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_str_sub_lsl_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_str_sub_lsr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_str_sub_ror_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_str_sub_rrx_1cond_2noPC_3general_4noPC,


Operator_arm_str_post_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_post_add_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_post_add_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_post_add_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_post_add_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_post_add_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_post_addi_3noPC_1cond_2noPC_3noPC_4s13,


Operator_arm_str_post_addsh2_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_post_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_post_sub_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_post_sub_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_post_sub_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_post_sub_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_post_sub_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_post_subi_3noPC_1cond_2noPC_3noPC_4offset12,


Operator_arm_str_pre_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_pre_add_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_pre_add_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_pre_add_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_pre_add_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_pre_add_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_pre_addi_3noPC_1cond_2noPC_3noPC_4s13,


Operator_arm_str_pre_addsh2_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_pre_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_pre_sub_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_pre_sub_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_pre_sub_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_str_pre_sub_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_str_pre_sub_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_str_pre_subi_3noPC_1cond_2noPC_3noPC_4offset12,


Operator_arm_str_addpc_1cond_2noPC_3offset12PC,


Operator_arm_str_subpc_1cond_2noPC_3offset12PC,


Operator_arm_str_pc_addi_1cond_2pc_3general_4s13,


Operator_arm_str_pc_subi_1cond_2pc_3general_4offset12,


Operator_arm_str_pc_add_1cond_2pc_3general_4noPC,


Operator_arm_str_pc_add_asr_1cond_2pc_3general_4noPC_5shfimmr,


Operator_arm_str_pc_add_lsl_1cond_2pc_3general_4noPC_5shfimml,


Operator_arm_str_pc_add_lsr_1cond_2pc_3general_4noPC_5shfimmr,


Operator_arm_str_pc_add_ror_1cond_2pc_3general_4noPC_5shfimml,


Operator_arm_str_pc_add_rrx_1cond_2pc_3general_4noPC,


Operator_arm_str_pc_addsh2_1cond_2pc_3general_4noPC,


Operator_arm_str_pc_sub_1cond_2pc_3general_4noPC,


Operator_arm_str_pc_sub_asr_1cond_2pc_3general_4noPC_5shfimmr,


Operator_arm_str_pc_sub_lsl_1cond_2pc_3general_4noPC_5shfimml,


Operator_arm_str_pc_sub_lsr_1cond_2pc_3general_4noPC_5shfimmr,


Operator_arm_str_pc_sub_ror_1cond_2pc_3general_4noPC_5shfimml,


Operator_arm_str_pc_sub_rrx_1cond_2pc_3general_4noPC,


Operator_arm_str_pc_post_add_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_post_add_asr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_post_add_lsl_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_post_add_lsr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_post_add_ror_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_post_add_rrx_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_post_addi_3noPC_1cond_2pc_3noPC_4s13,


Operator_arm_str_pc_post_addsh2_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_post_sub_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_post_sub_asr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_post_sub_lsl_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_post_sub_lsr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_post_sub_ror_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_post_sub_rrx_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_post_subi_3noPC_1cond_2pc_3noPC_4offset12,


Operator_arm_str_pc_pre_add_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_pre_add_asr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_pre_add_lsl_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_pre_add_lsr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_pre_add_ror_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_pre_add_rrx_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_pre_addi_3noPC_1cond_2pc_3noPC_4s13,


Operator_arm_str_pc_pre_addsh2_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_pre_sub_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_pre_sub_asr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_pre_sub_lsl_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_pre_sub_lsr_3noPC_1cond_2pc_3noPC_4noPC_5shfimmr,


Operator_arm_str_pc_pre_sub_ror_3noPC_1cond_2pc_3noPC_4noPC_5shfimml,


Operator_arm_str_pc_pre_sub_rrx_3noPC_1cond_2pc_3noPC_4noPC,


Operator_arm_str_pc_pre_subi_3noPC_1cond_2pc_3noPC_4offset12,


Operator_arm_str_pc_addpc_1cond_2pc_3offset12PC,


Operator_arm_str_pc_subpc_1cond_2pc_3offset12PC,


Operator_arm_strb_addi_1cond_2noPC_3general_4s13,


Operator_arm_strb_subi_1cond_2noPC_3general_4offset12,


Operator_arm_strb_add_1cond_2noPC_3general_4noPC,


Operator_arm_strb_add_asr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_strb_add_lsl_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_strb_add_lsr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_strb_add_ror_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_strb_add_rrx_1cond_2noPC_3general_4noPC,


Operator_arm_strb_addsh2_1cond_2noPC_3general_4noPC,


Operator_arm_strb_sub_1cond_2noPC_3general_4noPC,


Operator_arm_strb_sub_asr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_strb_sub_lsl_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_strb_sub_lsr_1cond_2noPC_3general_4noPC_5shfimmr,


Operator_arm_strb_sub_ror_1cond_2noPC_3general_4noPC_5shfimml,


Operator_arm_strb_sub_rrx_1cond_2noPC_3general_4noPC,


Operator_arm_strb_post_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_post_add_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_post_add_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_post_add_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_post_add_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_post_add_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_post_addi_3noPC_1cond_2noPC_3noPC_4s13,


Operator_arm_strb_post_addsh2_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_post_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_post_sub_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_post_sub_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_post_sub_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_post_sub_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_post_sub_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_post_subi_3noPC_1cond_2noPC_3noPC_4offset12,


Operator_arm_strb_pre_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_pre_add_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_pre_add_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_pre_add_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_pre_add_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_pre_add_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_pre_addi_3noPC_1cond_2noPC_3noPC_4s13,


Operator_arm_strb_pre_addsh2_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_pre_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_pre_sub_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_pre_sub_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_pre_sub_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strb_pre_sub_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strb_pre_sub_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strb_pre_subi_3noPC_1cond_2noPC_3noPC_4offset12,


Operator_arm_strb_addpc_1cond_2noPC_3offset12PC,


Operator_arm_strb_subpc_1cond_2noPC_3offset12PC,


Operator_arm_ldrbt_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrbt_add_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrbt_add_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrbt_add_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrbt_add_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrbt_add_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrbt_addi_2noPC_3noPC_1cond_3noPC_4s13,


Operator_arm_ldrbt_addsh2_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrbt_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrbt_sub_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrbt_sub_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrbt_sub_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrbt_sub_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrbt_sub_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrbt_subi_2noPC_3noPC_1cond_3noPC_4offset12,


Operator_arm_ldrt_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrt_add_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrt_add_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrt_add_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrt_add_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrt_add_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrt_addi_2noPC_3noPC_1cond_3noPC_4s13,


Operator_arm_ldrt_addsh2_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrt_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrt_sub_asr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrt_sub_lsl_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrt_sub_lsr_2noPC_3noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_ldrt_sub_ror_2noPC_3noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_ldrt_sub_rrx_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrt_subi_2noPC_3noPC_1cond_3noPC_4offset12,


Operator_arm_strbt_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strbt_add_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strbt_add_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strbt_add_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strbt_add_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strbt_add_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strbt_addi_3noPC_1cond_2noPC_3noPC_4s13,


Operator_arm_strbt_addsh2_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strbt_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strbt_sub_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strbt_sub_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strbt_sub_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strbt_sub_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strbt_sub_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strbt_subi_3noPC_1cond_2noPC_3noPC_4offset12,


Operator_arm_strt_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strt_add_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strt_add_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strt_add_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strt_add_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strt_add_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strt_addi_3noPC_1cond_2noPC_3noPC_4s13,


Operator_arm_strt_addsh2_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strt_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strt_sub_asr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strt_sub_lsl_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strt_sub_lsr_3noPC_1cond_2noPC_3noPC_4noPC_5shfimmr,


Operator_arm_strt_sub_ror_3noPC_1cond_2noPC_3noPC_4noPC_5shfimml,


Operator_arm_strt_sub_rrx_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strt_subi_3noPC_1cond_2noPC_3noPC_4offset12,


Operator_arm_ldrh_addi_2noPC_1cond_3general_4s9,


Operator_arm_ldrh_subi_2noPC_1cond_3general_4offset8,


Operator_arm_ldrh_add_2noPC_1cond_3general_4noPC,


Operator_arm_ldrh_sub_2noPC_1cond_3general_4noPC,


Operator_arm_ldrh_post_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrh_post_addi_2noPC_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrh_post_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrh_post_subi_2noPC_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrh_pre_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrh_pre_addi_2noPC_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrh_pre_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrh_pre_subi_2noPC_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrh_addpc_2noPC_1cond_3offset8PC,


Operator_arm_ldrh_subpc_2noPC_1cond_3offset8PC,


Operator_arm_ldrsb_addi_2noPC_1cond_3general_4s9,


Operator_arm_ldrsb_subi_2noPC_1cond_3general_4offset8,


Operator_arm_ldrsb_add_2noPC_1cond_3general_4noPC,


Operator_arm_ldrsb_sub_2noPC_1cond_3general_4noPC,


Operator_arm_ldrsb_post_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsb_post_addi_2noPC_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrsb_post_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsb_post_subi_2noPC_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrsb_pre_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsb_pre_addi_2noPC_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrsb_pre_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsb_pre_subi_2noPC_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrsb_addpc_2noPC_1cond_3offset8PC,


Operator_arm_ldrsb_subpc_2noPC_1cond_3offset8PC,


Operator_arm_ldrsh_addi_2noPC_1cond_3general_4s9,


Operator_arm_ldrsh_subi_2noPC_1cond_3general_4offset8,


Operator_arm_ldrsh_add_2noPC_1cond_3general_4noPC,


Operator_arm_ldrsh_sub_2noPC_1cond_3general_4noPC,


Operator_arm_ldrsh_post_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsh_post_addi_2noPC_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrsh_post_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsh_post_subi_2noPC_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrsh_pre_add_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsh_pre_addi_2noPC_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrsh_pre_sub_2noPC_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrsh_pre_subi_2noPC_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrsh_addpc_2noPC_1cond_3offset8PC,


Operator_arm_ldrsh_subpc_2noPC_1cond_3offset8PC,


Operator_arm_strh_addi_1cond_2noPC_3general_4s9,


Operator_arm_strh_subi_1cond_2noPC_3general_4offset8,


Operator_arm_strh_add_1cond_2noPC_3general_4noPC,


Operator_arm_strh_sub_1cond_2noPC_3general_4noPC,


Operator_arm_strh_post_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strh_post_addi_3noPC_1cond_2noPC_3noPC_4s9,


Operator_arm_strh_post_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strh_post_subi_3noPC_1cond_2noPC_3noPC_4offset8,


Operator_arm_strh_pre_add_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strh_pre_addi_3noPC_1cond_2noPC_3noPC_4s9,


Operator_arm_strh_pre_sub_3noPC_1cond_2noPC_3noPC_4noPC,


Operator_arm_strh_pre_subi_3noPC_1cond_2noPC_3noPC_4offset8,


Operator_arm_strh_addpc_1cond_2noPC_3offset8PC,


Operator_arm_strh_subpc_1cond_2noPC_3offset8PC,


Operator_arm_ldm_1cond_2amode4L_3noPC_4reglist,


Operator_arm_ldm_w_3noPC_1cond_2amode4L_3noPC_4reglist,


Operator_arm_ldm_pc_1cond_2amode4L_3noPC_4reglist,


Operator_arm_ldm_pc_w_3noPC_1cond_2amode4L_3noPC_4reglist,


Operator_arm_ldm_s10_1cond_2amode4L_3noPC_4reglist,


Operator_arm_ldm_s11_1cond_2amode4L_3noPC_4reglist,


Operator_arm_ldm_s11_w_3noPC_1cond_2amode4L_3noPC_4reglist,


Operator_arm_stm_1cond_2amode4S_3noPC_4reglist,


Operator_arm_stm_w_3noPC_1cond_2amode4S_3noPC_4reglist,


Operator_arm_stm_s1_1cond_2amode4S_3noPC_4reglist,


Operator_arm_swp_2noPC_1cond_3noPC_4noPC,


Operator_arm_swpb_2noPC_1cond_3noPC_4noPC,


Operator_arm_bkpt_1bkptnum,


Operator_arm_swi_1cond_2swinum,


Operator_arm_cdp_1cond_2cpnum_3opcode1d_4cpreg_5cpreg_6cpreg_7opcode2,


Operator_arm_cdp2_1cpnum_2opcode1d_3cpreg_4cpreg_5cpreg_6opcode2,


Operator_arm_ldc_add_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_ldc_sub_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_ldc_post_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldc_post_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldc_pre_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldc_pre_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldc_unindexed_1cond_2cpnum_3cpreg_4general_5option,


Operator_arm_ldcl_add_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_ldcl_sub_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_ldcl_post_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldcl_post_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldcl_pre_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldcl_pre_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_ldcl_unindexed_1cond_2cpnum_3cpreg_4general_5option,


Operator_arm_stc_add_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_stc_sub_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_stc_post_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stc_post_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stc_pre_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stc_pre_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stc_unindexed_1cond_2cpnum_3cpreg_4general_5option,


Operator_arm_stcl_add_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_stcl_sub_1cond_2cpnum_3cpreg_4general_5offset8CP,


Operator_arm_stcl_post_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stcl_post_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stcl_pre_add_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stcl_pre_sub_4noPC_1cond_2cpnum_3cpreg_4noPC_5offset8CP,


Operator_arm_stcl_unindexed_1cond_2cpnum_3cpreg_4general_5option,


Operator_arm_ldc2_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldc2_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldc2_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_ldc2l_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldc2l_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldc2l_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2l_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2l_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2l_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldc2l_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_stc2_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stc2_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stc2_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_stc2l_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stc2l_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stc2l_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2l_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2l_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2l_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stc2l_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_mcr_1cond_2cpnum_3opcode1m_4noPC_5cpreg_6cpreg_7opcode2,


Operator_arm_mcr2_1cpnum_2opcode1m_3noPC_4cpreg_5cpreg_6opcode2,


Operator_arm_mrc_4noPC_1cond_2cpnum_3opcode1m_5cpreg_6cpreg_7opcode2,


Operator_arm_mrc_pc_1cond_2cpnum_3opcode1m_4cpreg_5cpreg_6opcode2,


Operator_arm_mrc2_3noPC_1cpnum_2opcode1m_4cpreg_5cpreg_6opcode2,


Operator_arm_mrc2_pc_1cpnum_2opcode1m_3cpreg_4cpreg_5opcode2,


Operator_arm_smlabb_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlabt_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlatb_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlatt_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlalbb_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlalbt_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlaltb_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlaltt_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smulbb_2noPC_1cond_3noPC_4noPC,


Operator_arm_smulbt_2noPC_1cond_3noPC_4noPC,


Operator_arm_smultb_2noPC_1cond_3noPC_4noPC,


Operator_arm_smultt_2noPC_1cond_3noPC_4noPC,


Operator_arm_smlawb_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlawt_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smulwb_2noPC_1cond_3noPC_4noPC,


Operator_arm_smulwt_2noPC_1cond_3noPC_4noPC,


Operator_arm_qadd_2noPC_1cond_3noPC_4noPC,


Operator_arm_qdadd_2noPC_1cond_3noPC_4noPC,


Operator_arm_qdsub_2noPC_1cond_3noPC_4noPC,


Operator_arm_qsub_2noPC_1cond_3noPC_4noPC,


Operator_arm_ldrd_addi_2paired_1cond_3general_4s9,


Operator_arm_MULTI_ldrd_addi_2paired_1cond_3general_4s9,


Operator_arm_ldrd_subi_2paired_1cond_3general_4offset8,


Operator_arm_MULTI_ldrd_subi_2paired_1cond_3general_4offset8,


Operator_arm_ldrd_add_2paired_1cond_3general_4noPC,


Operator_arm_MULTI_ldrd_add_2paired_1cond_3general_4noPC,


Operator_arm_ldrd_sub_2paired_1cond_3general_4noPC,


Operator_arm_MULTI_ldrd_sub_2paired_1cond_3general_4noPC,


Operator_arm_ldrd_post_add_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_MULTI_ldrd_post_add_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrd_post_addi_2paired_3noPC_1cond_3noPC_4s9,


Operator_arm_MULTI_ldrd_post_addi_2paired_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrd_post_sub_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_MULTI_ldrd_post_sub_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrd_post_subi_2paired_3noPC_1cond_3noPC_4offset8,


Operator_arm_MULTI_ldrd_post_subi_2paired_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrd_pre_add_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_MULTI_ldrd_pre_add_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrd_pre_addi_2paired_3noPC_1cond_3noPC_4s9,


Operator_arm_MULTI_ldrd_pre_addi_2paired_3noPC_1cond_3noPC_4s9,


Operator_arm_ldrd_pre_sub_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_MULTI_ldrd_pre_sub_2paired_3noPC_1cond_3noPC_4noPC,


Operator_arm_ldrd_pre_subi_2paired_3noPC_1cond_3noPC_4offset8,


Operator_arm_MULTI_ldrd_pre_subi_2paired_3noPC_1cond_3noPC_4offset8,


Operator_arm_ldrd_addpc_2paired_1cond_3offset8PC,


Operator_arm_MULTI_ldrd_addpc_2paired_1cond_3offset8PC,


Operator_arm_ldrd_subpc_2paired_1cond_3offset8PC,


Operator_arm_MULTI_ldrd_subpc_2paired_1cond_3offset8PC,


Operator_arm_strd_addi_1cond_2paired_3general_4s9,


Operator_arm_MULTI_strd_addi_1cond_2paired_3general_4s9,


Operator_arm_strd_subi_1cond_2paired_3general_4offset8,


Operator_arm_MULTI_strd_subi_1cond_2paired_3general_4offset8,


Operator_arm_strd_add_1cond_2paired_3general_4noPC,


Operator_arm_MULTI_strd_add_1cond_2paired_3general_4noPC,


Operator_arm_strd_sub_1cond_2paired_3general_4noPC,


Operator_arm_MULTI_strd_sub_1cond_2paired_3general_4noPC,


Operator_arm_strd_post_add_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_MULTI_strd_post_add_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_strd_post_addi_3noPC_1cond_2paired_3noPC_4s9,


Operator_arm_MULTI_strd_post_addi_3noPC_1cond_2paired_3noPC_4s9,


Operator_arm_strd_post_sub_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_MULTI_strd_post_sub_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_strd_post_subi_3noPC_1cond_2paired_3noPC_4offset8,


Operator_arm_MULTI_strd_post_subi_3noPC_1cond_2paired_3noPC_4offset8,


Operator_arm_strd_pre_add_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_MULTI_strd_pre_add_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_strd_pre_addi_3noPC_1cond_2paired_3noPC_4s9,


Operator_arm_MULTI_strd_pre_addi_3noPC_1cond_2paired_3noPC_4s9,


Operator_arm_strd_pre_sub_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_MULTI_strd_pre_sub_3noPC_1cond_2paired_3noPC_4noPC,


Operator_arm_strd_pre_subi_3noPC_1cond_2paired_3noPC_4offset8,


Operator_arm_MULTI_strd_pre_subi_3noPC_1cond_2paired_3noPC_4offset8,


Operator_arm_strd_addpc_1cond_2paired_3offset8PC,


Operator_arm_MULTI_strd_addpc_1cond_2paired_3offset8PC,


Operator_arm_strd_subpc_1cond_2paired_3offset8PC,


Operator_arm_MULTI_strd_subpc_1cond_2paired_3offset8PC,


Operator_arm_pld_addi_1general_2s13,


Operator_arm_pld_subi_1general_2offset12,


Operator_arm_pld_addpc_1offset12PC,


Operator_arm_pld_subpc_1offset12PC,


Operator_arm_pld_add_1general_2noPC,


Operator_arm_pld_add_asr_1general_2noPC_3shfimmr,


Operator_arm_pld_add_lsl_1general_2noPC_3shfimml,


Operator_arm_pld_add_lsr_1general_2noPC_3shfimmr,


Operator_arm_pld_add_ror_1general_2noPC_3shfimml,


Operator_arm_pld_add_rrx_1general_2noPC,


Operator_arm_pld_addsh2_1general_2noPC,


Operator_arm_pld_sub_1general_2noPC,


Operator_arm_pld_sub_asr_1general_2noPC_3shfimmr,


Operator_arm_pld_sub_lsl_1general_2noPC_3shfimml,


Operator_arm_pld_sub_lsr_1general_2noPC_3shfimmr,


Operator_arm_pld_sub_ror_1general_2noPC_3shfimml,


Operator_arm_pld_sub_rrx_1general_2noPC,


Operator_arm_mcrr_1cond_2cpnum_3opcode2_4noPC_5noPC_6cpreg,


Operator_arm_mrrc_4noPC_5noPC_1cond_2cpnum_3opcode2_6cpreg,


Operator_arm_ldr_2noPC_1cond_3general,


Operator_arm_ldr_pc_2pc_1cond_3general,


Operator_arm_ldrb_2noPC_1cond_3general,


Operator_arm_ldrbt_2noPC_3noPC_1cond_3noPC,


Operator_arm_ldrt_2noPC_3noPC_1cond_3noPC,


Operator_arm_pld_1general,


Operator_arm_str_1cond_2noPC_3general,


Operator_arm_str_pc_1cond_2pc_3general,


Operator_arm_strb_1cond_2noPC_3general,


Operator_arm_strbt_3noPC_1cond_2noPC_3noPC,


Operator_arm_strt_3noPC_1cond_2noPC_3noPC,


Operator_arm_mcr_x2_1cond_2cpnum_3opcode1m_4noPC_5cpreg_6cpreg,


Operator_arm_mcr2_x2_1cpnum_2opcode1m_3noPC_4cpreg_5cpreg,


Operator_arm_mrc_x2_4noPC_1cond_2cpnum_3opcode1m_5cpreg_6cpreg,


Operator_arm_mrc2_x2_3noPC_1cpnum_2opcode1m_4cpreg_5cpreg,


Operator_arm_ldral_1noPC_2general,


Operator_arm_ldral_pc_1pc_2general,


Operator_arm_ldralb_1noPC_2general,


Operator_arm_ldralbt_1noPC_2noPC_2noPC,


Operator_arm_ldralt_1noPC_2noPC_2noPC,


Operator_arm_stral_1noPC_2general,


Operator_arm_stral_pc_1pc_2general,


Operator_arm_stralb_1noPC_2general,


Operator_arm_stralbt_2noPC_1noPC_2noPC,


Operator_arm_stralt_2noPC_1noPC_2noPC,


Operator_arm_mcral_x2_1cpnum_2opcode1m_3noPC_4cpreg_5cpreg,


Operator_arm_mrcal_x2_3noPC_1cpnum_2opcode1m_4cpreg_5cpreg,


Operator_arm_bal_1btarg1,


Operator_arm_blal_1btarg1,


Operator_arm_blxal_1noPC,


Operator_arm_bxal_1general,


Operator_arm_movals_asr_imm_1noPC_2general_3shfimmr,


Operator_arm_movals_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_movals_imm_1noPC_2immed8,


Operator_arm_movals_lsl_imm_1noPC_2general_3shfimml,


Operator_arm_movals_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_movals_lsr_imm_1noPC_2general_3shfimmr,


Operator_arm_movals_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_movals_reg_1noPC_2general,


Operator_arm_movals_ror_imm_1noPC_2general_3shfimml,


Operator_arm_movals_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_movals_rrx_1noPC_2general,


Operator_arm_moval_asr_imm_1noPC_2general_3shfimmr,


Operator_arm_moval_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_moval_imm_1noPC_2immed8,


Operator_arm_moval_lsl_imm_1noPC_2general_3shfimml,


Operator_arm_moval_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_moval_lsr_imm_1noPC_2general_3shfimmr,


Operator_arm_moval_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_moval_reg_1noPC_2general,


Operator_arm_moval_ror_imm_1noPC_2general_3shfimml,


Operator_arm_moval_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_moval_rrx_1noPC_2general,


Operator_arm_moval_pc_imm_2pc_1S_3immed8,


Operator_arm_moval_pc_reg_2pc_1S_3general,


Operator_arm_moval_pc_lsl_imm_2pc_1S_3general_4shfimml,


Operator_arm_moval_pc_lsr_imm_2pc_1S_3general_4shfimmr,


Operator_arm_moval_pc_asr_imm_2pc_1S_3general_4shfimmr,


Operator_arm_moval_pc_ror_imm_2pc_1S_3general_4shfimml,


Operator_arm_moval_pc_rrx_2pc_1S_3general,


Operator_arm_mvnals_asr_imm_1noPC_2general_3shfimmr,


Operator_arm_mvnals_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnals_imm_1noPC_2immed8,


Operator_arm_mvnals_lsl_imm_1noPC_2general_3shfimml,


Operator_arm_mvnals_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnals_lsr_imm_1noPC_2general_3shfimmr,


Operator_arm_mvnals_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnals_reg_1noPC_2general,


Operator_arm_mvnals_ror_imm_1noPC_2general_3shfimml,


Operator_arm_mvnals_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnals_rrx_1noPC_2general,


Operator_arm_mvnal_asr_imm_1noPC_2general_3shfimmr,


Operator_arm_mvnal_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnal_imm_1noPC_2immed8,


Operator_arm_mvnal_lsl_imm_1noPC_2general_3shfimml,


Operator_arm_mvnal_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnal_lsr_imm_1noPC_2general_3shfimmr,


Operator_arm_mvnal_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnal_reg_1noPC_2general,


Operator_arm_mvnal_ror_imm_1noPC_2general_3shfimml,


Operator_arm_mvnal_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_mvnal_rrx_1noPC_2general,


Operator_arm_mvnal_pc_imm_2pc_1S_3immed8,


Operator_arm_mvnal_pc_reg_2pc_1S_3general,


Operator_arm_mvnal_pc_lsl_imm_2pc_1S_3general_4shfimml,


Operator_arm_mvnal_pc_lsr_imm_2pc_1S_3general_4shfimmr,


Operator_arm_mvnal_pc_asr_imm_2pc_1S_3general_4shfimmr,


Operator_arm_mvnal_pc_ror_imm_2pc_1S_3general_4shfimml,


Operator_arm_mvnal_pc_rrx_2pc_1S_3general,


Operator_arm_cmpal_asr_imm_1general_2general_3shfimmr,


Operator_arm_cmpal_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_cmpal_imm_1general_2immed8,


Operator_arm_cmpal_lsl_imm_1general_2general_3shfimml,


Operator_arm_cmpal_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_cmpal_lsr_imm_1general_2general_3shfimmr,


Operator_arm_cmpal_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_cmpal_reg_1general_2general,


Operator_arm_cmpal_ror_imm_1general_2general_3shfimml,


Operator_arm_cmpal_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_cmpal_rrx_1general_2general,


Operator_arm_cmnal_asr_imm_1general_2general_3shfimmr,


Operator_arm_cmnal_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_cmnal_imm_1general_2immed8,


Operator_arm_cmnal_lsl_imm_1general_2general_3shfimml,


Operator_arm_cmnal_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_cmnal_lsr_imm_1general_2general_3shfimmr,


Operator_arm_cmnal_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_cmnal_reg_1general_2general,


Operator_arm_cmnal_ror_imm_1general_2general_3shfimml,


Operator_arm_cmnal_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_cmnal_rrx_1general_2general,


Operator_arm_tstal_asr_imm_1general_2general_3shfimmr,


Operator_arm_tstal_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_tstal_imm_1general_2immed8,


Operator_arm_tstal_lsl_imm_1general_2general_3shfimml,


Operator_arm_tstal_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_tstal_lsr_imm_1general_2general_3shfimmr,


Operator_arm_tstal_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_tstal_reg_1general_2general,


Operator_arm_tstal_ror_imm_1general_2general_3shfimml,


Operator_arm_tstal_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_tstal_rrx_1general_2general,


Operator_arm_teqal_asr_imm_1general_2general_3shfimmr,


Operator_arm_teqal_asr_reg_1noPC_2noPC_3noPC,


Operator_arm_teqal_imm_1general_2immed8,


Operator_arm_teqal_lsl_imm_1general_2general_3shfimml,


Operator_arm_teqal_lsl_reg_1noPC_2noPC_3noPC,


Operator_arm_teqal_lsr_imm_1general_2general_3shfimmr,


Operator_arm_teqal_lsr_reg_1noPC_2noPC_3noPC,


Operator_arm_teqal_reg_1general_2general,


Operator_arm_teqal_ror_imm_1general_2general_3shfimml,


Operator_arm_teqal_ror_reg_1noPC_2noPC_3noPC,


Operator_arm_teqal_rrx_1general_2general,


Operator_arm_addals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_addals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addals_imm_1noPC_2general_3immed8,


Operator_arm_addals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_addals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_addals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addals_reg_1noPC_2general_3general,


Operator_arm_addals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_addals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addals_rrx_1noPC_2general_3general,


Operator_arm_addal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_addal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addal_imm_1noPC_2general_3immed8,


Operator_arm_addal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_addal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_addal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addal_reg_1noPC_2general_3general,


Operator_arm_addal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_addal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_addal_rrx_1noPC_2general_3general,


Operator_arm_addal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_addal_pc_reg_2pc_1S_3general_4general,


Operator_arm_addal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_addal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_addal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_addal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_addal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_subals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_subals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subals_imm_1noPC_2general_3immed8,


Operator_arm_subals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_subals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_subals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subals_reg_1noPC_2general_3general,


Operator_arm_subals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_subals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subals_rrx_1noPC_2general_3general,


Operator_arm_subal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_subal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subal_imm_1noPC_2general_3immed8,


Operator_arm_subal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_subal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_subal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subal_reg_1noPC_2general_3general,


Operator_arm_subal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_subal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_subal_rrx_1noPC_2general_3general,


Operator_arm_subal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_subal_pc_reg_2pc_1S_3general_4general,


Operator_arm_subal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_subal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_subal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_subal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_subal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_rsbals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rsbals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbals_imm_1noPC_2general_3immed8,


Operator_arm_rsbals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rsbals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rsbals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbals_reg_1noPC_2general_3general,


Operator_arm_rsbals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rsbals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbals_rrx_1noPC_2general_3general,


Operator_arm_rsbal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rsbal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbal_imm_1noPC_2general_3immed8,


Operator_arm_rsbal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rsbal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rsbal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbal_reg_1noPC_2general_3general,


Operator_arm_rsbal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rsbal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rsbal_rrx_1noPC_2general_3general,


Operator_arm_rsbal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_rsbal_pc_reg_2pc_1S_3general_4general,


Operator_arm_rsbal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_rsbal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_rsbal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_rsbal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_rsbal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_adcals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_adcals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcals_imm_1noPC_2general_3immed8,


Operator_arm_adcals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_adcals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_adcals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcals_reg_1noPC_2general_3general,


Operator_arm_adcals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_adcals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcals_rrx_1noPC_2general_3general,


Operator_arm_adcal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_adcal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcal_imm_1noPC_2general_3immed8,


Operator_arm_adcal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_adcal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_adcal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcal_reg_1noPC_2general_3general,


Operator_arm_adcal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_adcal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_adcal_rrx_1noPC_2general_3general,


Operator_arm_adcal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_adcal_pc_reg_2pc_1S_3general_4general,


Operator_arm_adcal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_adcal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_adcal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_adcal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_adcal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_sbcals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_sbcals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcals_imm_1noPC_2general_3immed8,


Operator_arm_sbcals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_sbcals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_sbcals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcals_reg_1noPC_2general_3general,


Operator_arm_sbcals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_sbcals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcals_rrx_1noPC_2general_3general,


Operator_arm_sbcal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_sbcal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcal_imm_1noPC_2general_3immed8,


Operator_arm_sbcal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_sbcal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_sbcal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcal_reg_1noPC_2general_3general,


Operator_arm_sbcal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_sbcal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_sbcal_rrx_1noPC_2general_3general,


Operator_arm_sbcal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_sbcal_pc_reg_2pc_1S_3general_4general,


Operator_arm_sbcal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_sbcal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_sbcal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_sbcal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_sbcal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_rscals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rscals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscals_imm_1noPC_2general_3immed8,


Operator_arm_rscals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rscals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rscals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscals_reg_1noPC_2general_3general,


Operator_arm_rscals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rscals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscals_rrx_1noPC_2general_3general,


Operator_arm_rscal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rscal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscal_imm_1noPC_2general_3immed8,


Operator_arm_rscal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rscal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_rscal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscal_reg_1noPC_2general_3general,


Operator_arm_rscal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_rscal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_rscal_rrx_1noPC_2general_3general,


Operator_arm_rscal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_rscal_pc_reg_2pc_1S_3general_4general,


Operator_arm_rscal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_rscal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_rscal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_rscal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_rscal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_andals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_andals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andals_imm_1noPC_2general_3immed8,


Operator_arm_andals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_andals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_andals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andals_reg_1noPC_2general_3general,


Operator_arm_andals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_andals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andals_rrx_1noPC_2general_3general,


Operator_arm_andal_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_andal_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andal_imm_1noPC_2general_3immed8,


Operator_arm_andal_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_andal_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andal_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_andal_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andal_reg_1noPC_2general_3general,


Operator_arm_andal_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_andal_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_andal_rrx_1noPC_2general_3general,


Operator_arm_andal_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_andal_pc_reg_2pc_1S_3general_4general,


Operator_arm_andal_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_andal_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_andal_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_andal_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_andal_pc_rrx_2pc_1S_3general_4general,


Operator_arm_bicals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_bicals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bicals_imm_1noPC_2general_3immed8,


Operator_arm_bicals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_bicals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bicals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_bicals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bicals_reg_1noPC_2general_3general,


Operator_arm_bicals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_bicals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bicals_rrx_1noPC_2general_3general,


Operator_arm_bical_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_bical_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bical_imm_1noPC_2general_3immed8,


Operator_arm_bical_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_bical_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bical_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_bical_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bical_reg_1noPC_2general_3general,


Operator_arm_bical_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_bical_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_bical_rrx_1noPC_2general_3general,


Operator_arm_bical_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_bical_pc_reg_2pc_1S_3general_4general,


Operator_arm_bical_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_bical_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_bical_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_bical_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_bical_pc_rrx_2pc_1S_3general_4general,


Operator_arm_eorals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_eorals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eorals_imm_1noPC_2general_3immed8,


Operator_arm_eorals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_eorals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eorals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_eorals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eorals_reg_1noPC_2general_3general,


Operator_arm_eorals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_eorals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eorals_rrx_1noPC_2general_3general,


Operator_arm_eoral_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_eoral_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eoral_imm_1noPC_2general_3immed8,


Operator_arm_eoral_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_eoral_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eoral_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_eoral_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eoral_reg_1noPC_2general_3general,


Operator_arm_eoral_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_eoral_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_eoral_rrx_1noPC_2general_3general,


Operator_arm_eoral_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_eoral_pc_reg_2pc_1S_3general_4general,


Operator_arm_eoral_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_eoral_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_eoral_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_eoral_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_eoral_pc_rrx_2pc_1S_3general_4general,


Operator_arm_orrals_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_orrals_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orrals_imm_1noPC_2general_3immed8,


Operator_arm_orrals_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_orrals_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orrals_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_orrals_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orrals_reg_1noPC_2general_3general,


Operator_arm_orrals_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_orrals_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orrals_rrx_1noPC_2general_3general,


Operator_arm_orral_asr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_orral_asr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orral_imm_1noPC_2general_3immed8,


Operator_arm_orral_lsl_imm_1noPC_2general_3general_4shfimml,


Operator_arm_orral_lsl_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orral_lsr_imm_1noPC_2general_3general_4shfimmr,


Operator_arm_orral_lsr_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orral_reg_1noPC_2general_3general,


Operator_arm_orral_ror_imm_1noPC_2general_3general_4shfimml,


Operator_arm_orral_ror_reg_1noPC_2noPC_3noPC_4noPC,


Operator_arm_orral_rrx_1noPC_2general_3general,


Operator_arm_orral_pc_imm_2pc_1S_3general_4immed8,


Operator_arm_orral_pc_reg_2pc_1S_3general_4general,


Operator_arm_orral_pc_lsl_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_orral_pc_lsr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_orral_pc_asr_imm_2pc_1S_3general_4general_5shfimmr,


Operator_arm_orral_pc_ror_imm_2pc_1S_3general_4general_5shfimml,


Operator_arm_orral_pc_rrx_2pc_1S_3general_4general,


Operator_arm_mlaals_1noPC_2noPC_3noPC_4noPC,


Operator_arm_mlaal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_mulals_1noPC_2noPC_3noPC,


Operator_arm_mulal_1noPC_2noPC_3noPC,


Operator_arm_smlalals_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlalal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smullals_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smullal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_umlalals_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_umlalal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_umullals_1noPC_2noPC_3noPC_4noPC,


Operator_arm_umullal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_clzal_1noPC_2noPC,


Operator_arm_mrsal_cpsr_1noPC,


Operator_arm_mrsal_spsr_1noPC,


Operator_arm_msral_cpsr_imm_1psrfld_2immed8,


Operator_arm_msral_cpsr_imm_f_1immed8,


Operator_arm_msral_cpsr_reg_1psrfld_2general,


Operator_arm_msral_cpsr_reg_f_1general,


Operator_arm_msral_spsr_imm_1psrfld_2immed8,


Operator_arm_msral_spsr_reg_1psrfld_2general,


Operator_arm_ldral_addi_1noPC_2general_3s13,


Operator_arm_ldral_subi_1noPC_2general_3offset12,


Operator_arm_ldral_add_1noPC_2general_3noPC,


Operator_arm_ldral_add_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldral_add_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldral_add_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldral_add_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldral_add_rrx_1noPC_2general_3noPC,


Operator_arm_ldral_addsh2_1noPC_2general_3noPC,


Operator_arm_ldral_sub_1noPC_2general_3noPC,


Operator_arm_ldral_sub_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldral_sub_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldral_sub_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldral_sub_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldral_sub_rrx_1noPC_2general_3noPC,


Operator_arm_ldral_post_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_post_add_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_post_add_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_post_add_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_post_add_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_post_add_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_post_addi_1noPC_2noPC_2noPC_3s13,


Operator_arm_ldral_post_addsh2_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_post_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_post_sub_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_post_sub_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_post_sub_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_post_sub_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_post_sub_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_post_subi_1noPC_2noPC_2noPC_3offset12,


Operator_arm_ldral_pre_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_pre_add_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pre_add_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pre_add_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pre_add_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pre_add_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_pre_addi_1noPC_2noPC_2noPC_3s13,


Operator_arm_ldral_pre_addsh2_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_pre_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_pre_sub_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pre_sub_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pre_sub_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pre_sub_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pre_sub_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldral_pre_subi_1noPC_2noPC_2noPC_3offset12,


Operator_arm_ldral_addpc_1noPC_2offset12PC,


Operator_arm_ldral_subpc_1noPC_2offset12PC,


Operator_arm_ldral_pc_addi_1pc_2general_3s13,


Operator_arm_ldral_pc_subi_1pc_2general_3offset12,


Operator_arm_ldral_pc_add_1pc_2general_3noPC,


Operator_arm_ldral_pc_add_asr_1pc_2general_3noPC_4shfimmr,


Operator_arm_ldral_pc_add_lsl_1pc_2general_3noPC_4shfimml,


Operator_arm_ldral_pc_add_lsr_1pc_2general_3noPC_4shfimmr,


Operator_arm_ldral_pc_add_ror_1pc_2general_3noPC_4shfimml,


Operator_arm_ldral_pc_add_rrx_1pc_2general_3noPC,


Operator_arm_ldral_pc_addsh2_1pc_2general_3noPC,


Operator_arm_ldral_pc_sub_1pc_2general_3noPC,


Operator_arm_ldral_pc_sub_asr_1pc_2general_3noPC_4shfimmr,


Operator_arm_ldral_pc_sub_lsl_1pc_2general_3noPC_4shfimml,


Operator_arm_ldral_pc_sub_lsr_1pc_2general_3noPC_4shfimmr,


Operator_arm_ldral_pc_sub_ror_1pc_2general_3noPC_4shfimml,


Operator_arm_ldral_pc_sub_rrx_1pc_2general_3noPC,


Operator_arm_ldral_pc_post_add_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_post_add_asr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_post_add_lsl_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_post_add_lsr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_post_add_ror_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_post_add_rrx_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_post_addi_1pc_2noPC_2noPC_3s13,


Operator_arm_ldral_pc_post_addsh2_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_post_sub_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_post_sub_asr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_post_sub_lsl_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_post_sub_lsr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_post_sub_ror_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_post_sub_rrx_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_post_subi_1pc_2noPC_2noPC_3offset12,


Operator_arm_ldral_pc_pre_add_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_pre_add_asr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_pre_add_lsl_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_pre_add_lsr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_pre_add_ror_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_pre_add_rrx_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_pre_addi_1pc_2noPC_2noPC_3s13,


Operator_arm_ldral_pc_pre_addsh2_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_pre_sub_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_pre_sub_asr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_pre_sub_lsl_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_pre_sub_lsr_1pc_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldral_pc_pre_sub_ror_1pc_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldral_pc_pre_sub_rrx_1pc_2noPC_2noPC_3noPC,


Operator_arm_ldral_pc_pre_subi_1pc_2noPC_2noPC_3offset12,


Operator_arm_ldral_pc_addpc_1pc_2offset12PC,


Operator_arm_ldral_pc_subpc_1pc_2offset12PC,


Operator_arm_ldralb_addi_1noPC_2general_3s13,


Operator_arm_ldralb_subi_1noPC_2general_3offset12,


Operator_arm_ldralb_add_1noPC_2general_3noPC,


Operator_arm_ldralb_add_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldralb_add_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldralb_add_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldralb_add_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldralb_add_rrx_1noPC_2general_3noPC,


Operator_arm_ldralb_addsh2_1noPC_2general_3noPC,


Operator_arm_ldralb_sub_1noPC_2general_3noPC,


Operator_arm_ldralb_sub_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldralb_sub_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldralb_sub_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_ldralb_sub_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_ldralb_sub_rrx_1noPC_2general_3noPC,


Operator_arm_ldralb_post_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_post_add_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_post_add_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_post_add_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_post_add_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_post_add_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_post_addi_1noPC_2noPC_2noPC_3s13,


Operator_arm_ldralb_post_addsh2_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_post_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_post_sub_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_post_sub_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_post_sub_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_post_sub_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_post_sub_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_post_subi_1noPC_2noPC_2noPC_3offset12,


Operator_arm_ldralb_pre_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_pre_add_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_pre_add_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_pre_add_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_pre_add_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_pre_add_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_pre_addi_1noPC_2noPC_2noPC_3s13,


Operator_arm_ldralb_pre_addsh2_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_pre_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_pre_sub_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_pre_sub_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_pre_sub_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralb_pre_sub_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralb_pre_sub_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralb_pre_subi_1noPC_2noPC_2noPC_3offset12,


Operator_arm_ldralb_addpc_1noPC_2offset12PC,


Operator_arm_ldralb_subpc_1noPC_2offset12PC,


Operator_arm_stral_addi_1noPC_2general_3s13,


Operator_arm_stral_subi_1noPC_2general_3offset12,


Operator_arm_stral_add_1noPC_2general_3noPC,


Operator_arm_stral_add_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stral_add_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_stral_add_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stral_add_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_stral_add_rrx_1noPC_2general_3noPC,


Operator_arm_stral_addsh2_1noPC_2general_3noPC,


Operator_arm_stral_sub_1noPC_2general_3noPC,


Operator_arm_stral_sub_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stral_sub_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_stral_sub_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stral_sub_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_stral_sub_rrx_1noPC_2general_3noPC,


Operator_arm_stral_post_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_post_add_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_post_add_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_post_add_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_post_add_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_post_add_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_post_addi_2noPC_1noPC_2noPC_3s13,


Operator_arm_stral_post_addsh2_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_post_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_post_sub_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_post_sub_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_post_sub_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_post_sub_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_post_sub_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_post_subi_2noPC_1noPC_2noPC_3offset12,


Operator_arm_stral_pre_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_pre_add_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pre_add_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_pre_add_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pre_add_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_pre_add_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_pre_addi_2noPC_1noPC_2noPC_3s13,


Operator_arm_stral_pre_addsh2_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_pre_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_pre_sub_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pre_sub_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_pre_sub_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pre_sub_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stral_pre_sub_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stral_pre_subi_2noPC_1noPC_2noPC_3offset12,


Operator_arm_stral_addpc_1noPC_2offset12PC,


Operator_arm_stral_subpc_1noPC_2offset12PC,


Operator_arm_stral_pc_addi_1pc_2general_3s13,


Operator_arm_stral_pc_subi_1pc_2general_3offset12,


Operator_arm_stral_pc_add_1pc_2general_3noPC,


Operator_arm_stral_pc_add_asr_1pc_2general_3noPC_4shfimmr,


Operator_arm_stral_pc_add_lsl_1pc_2general_3noPC_4shfimml,


Operator_arm_stral_pc_add_lsr_1pc_2general_3noPC_4shfimmr,


Operator_arm_stral_pc_add_ror_1pc_2general_3noPC_4shfimml,


Operator_arm_stral_pc_add_rrx_1pc_2general_3noPC,


Operator_arm_stral_pc_addsh2_1pc_2general_3noPC,


Operator_arm_stral_pc_sub_1pc_2general_3noPC,


Operator_arm_stral_pc_sub_asr_1pc_2general_3noPC_4shfimmr,


Operator_arm_stral_pc_sub_lsl_1pc_2general_3noPC_4shfimml,


Operator_arm_stral_pc_sub_lsr_1pc_2general_3noPC_4shfimmr,


Operator_arm_stral_pc_sub_ror_1pc_2general_3noPC_4shfimml,


Operator_arm_stral_pc_sub_rrx_1pc_2general_3noPC,


Operator_arm_stral_pc_post_add_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_post_add_asr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_post_add_lsl_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_post_add_lsr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_post_add_ror_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_post_add_rrx_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_post_addi_2noPC_1pc_2noPC_3s13,


Operator_arm_stral_pc_post_addsh2_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_post_sub_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_post_sub_asr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_post_sub_lsl_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_post_sub_lsr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_post_sub_ror_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_post_sub_rrx_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_post_subi_2noPC_1pc_2noPC_3offset12,


Operator_arm_stral_pc_pre_add_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_pre_add_asr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_pre_add_lsl_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_pre_add_lsr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_pre_add_ror_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_pre_add_rrx_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_pre_addi_2noPC_1pc_2noPC_3s13,


Operator_arm_stral_pc_pre_addsh2_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_pre_sub_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_pre_sub_asr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_pre_sub_lsl_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_pre_sub_lsr_2noPC_1pc_2noPC_3noPC_4shfimmr,


Operator_arm_stral_pc_pre_sub_ror_2noPC_1pc_2noPC_3noPC_4shfimml,


Operator_arm_stral_pc_pre_sub_rrx_2noPC_1pc_2noPC_3noPC,


Operator_arm_stral_pc_pre_subi_2noPC_1pc_2noPC_3offset12,


Operator_arm_stral_pc_addpc_1pc_2offset12PC,


Operator_arm_stral_pc_subpc_1pc_2offset12PC,


Operator_arm_stralb_addi_1noPC_2general_3s13,


Operator_arm_stralb_subi_1noPC_2general_3offset12,


Operator_arm_stralb_add_1noPC_2general_3noPC,


Operator_arm_stralb_add_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stralb_add_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_stralb_add_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stralb_add_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_stralb_add_rrx_1noPC_2general_3noPC,


Operator_arm_stralb_addsh2_1noPC_2general_3noPC,


Operator_arm_stralb_sub_1noPC_2general_3noPC,


Operator_arm_stralb_sub_asr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stralb_sub_lsl_1noPC_2general_3noPC_4shfimml,


Operator_arm_stralb_sub_lsr_1noPC_2general_3noPC_4shfimmr,


Operator_arm_stralb_sub_ror_1noPC_2general_3noPC_4shfimml,


Operator_arm_stralb_sub_rrx_1noPC_2general_3noPC,


Operator_arm_stralb_post_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_post_add_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_post_add_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_post_add_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_post_add_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_post_add_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_post_addi_2noPC_1noPC_2noPC_3s13,


Operator_arm_stralb_post_addsh2_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_post_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_post_sub_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_post_sub_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_post_sub_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_post_sub_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_post_sub_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_post_subi_2noPC_1noPC_2noPC_3offset12,


Operator_arm_stralb_pre_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_pre_add_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_pre_add_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_pre_add_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_pre_add_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_pre_add_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_pre_addi_2noPC_1noPC_2noPC_3s13,


Operator_arm_stralb_pre_addsh2_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_pre_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_pre_sub_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_pre_sub_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_pre_sub_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralb_pre_sub_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralb_pre_sub_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralb_pre_subi_2noPC_1noPC_2noPC_3offset12,


Operator_arm_stralb_addpc_1noPC_2offset12PC,


Operator_arm_stralb_subpc_1noPC_2offset12PC,


Operator_arm_ldralbt_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralbt_add_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralbt_add_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralbt_add_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralbt_add_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralbt_add_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralbt_addi_1noPC_2noPC_2noPC_3s13,


Operator_arm_ldralbt_addsh2_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralbt_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralbt_sub_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralbt_sub_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralbt_sub_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralbt_sub_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralbt_sub_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralbt_subi_1noPC_2noPC_2noPC_3offset12,


Operator_arm_ldralt_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralt_add_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralt_add_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralt_add_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralt_add_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralt_add_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralt_addi_1noPC_2noPC_2noPC_3s13,


Operator_arm_ldralt_addsh2_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralt_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralt_sub_asr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralt_sub_lsl_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralt_sub_lsr_1noPC_2noPC_2noPC_3noPC_4shfimmr,


Operator_arm_ldralt_sub_ror_1noPC_2noPC_2noPC_3noPC_4shfimml,


Operator_arm_ldralt_sub_rrx_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralt_subi_1noPC_2noPC_2noPC_3offset12,


Operator_arm_stralbt_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralbt_add_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralbt_add_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralbt_add_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralbt_add_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralbt_add_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralbt_addi_2noPC_1noPC_2noPC_3s13,


Operator_arm_stralbt_addsh2_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralbt_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralbt_sub_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralbt_sub_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralbt_sub_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralbt_sub_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralbt_sub_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralbt_subi_2noPC_1noPC_2noPC_3offset12,


Operator_arm_stralt_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralt_add_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralt_add_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralt_add_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralt_add_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralt_add_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralt_addi_2noPC_1noPC_2noPC_3s13,


Operator_arm_stralt_addsh2_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralt_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralt_sub_asr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralt_sub_lsl_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralt_sub_lsr_2noPC_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_stralt_sub_ror_2noPC_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_stralt_sub_rrx_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralt_subi_2noPC_1noPC_2noPC_3offset12,


Operator_arm_ldralh_addi_1noPC_2general_3s9,


Operator_arm_ldralh_subi_1noPC_2general_3offset8,


Operator_arm_ldralh_add_1noPC_2general_3noPC,


Operator_arm_ldralh_sub_1noPC_2general_3noPC,


Operator_arm_ldralh_post_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralh_post_addi_1noPC_2noPC_2noPC_3s9,


Operator_arm_ldralh_post_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralh_post_subi_1noPC_2noPC_2noPC_3offset8,


Operator_arm_ldralh_pre_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralh_pre_addi_1noPC_2noPC_2noPC_3s9,


Operator_arm_ldralh_pre_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralh_pre_subi_1noPC_2noPC_2noPC_3offset8,


Operator_arm_ldralh_addpc_1noPC_2offset8PC,


Operator_arm_ldralh_subpc_1noPC_2offset8PC,


Operator_arm_ldralsb_addi_1noPC_2general_3s9,


Operator_arm_ldralsb_subi_1noPC_2general_3offset8,


Operator_arm_ldralsb_add_1noPC_2general_3noPC,


Operator_arm_ldralsb_sub_1noPC_2general_3noPC,


Operator_arm_ldralsb_post_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsb_post_addi_1noPC_2noPC_2noPC_3s9,


Operator_arm_ldralsb_post_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsb_post_subi_1noPC_2noPC_2noPC_3offset8,


Operator_arm_ldralsb_pre_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsb_pre_addi_1noPC_2noPC_2noPC_3s9,


Operator_arm_ldralsb_pre_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsb_pre_subi_1noPC_2noPC_2noPC_3offset8,


Operator_arm_ldralsb_addpc_1noPC_2offset8PC,


Operator_arm_ldralsb_subpc_1noPC_2offset8PC,


Operator_arm_ldralsh_addi_1noPC_2general_3s9,


Operator_arm_ldralsh_subi_1noPC_2general_3offset8,


Operator_arm_ldralsh_add_1noPC_2general_3noPC,


Operator_arm_ldralsh_sub_1noPC_2general_3noPC,


Operator_arm_ldralsh_post_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsh_post_addi_1noPC_2noPC_2noPC_3s9,


Operator_arm_ldralsh_post_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsh_post_subi_1noPC_2noPC_2noPC_3offset8,


Operator_arm_ldralsh_pre_add_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsh_pre_addi_1noPC_2noPC_2noPC_3s9,


Operator_arm_ldralsh_pre_sub_1noPC_2noPC_2noPC_3noPC,


Operator_arm_ldralsh_pre_subi_1noPC_2noPC_2noPC_3offset8,


Operator_arm_ldralsh_addpc_1noPC_2offset8PC,


Operator_arm_ldralsh_subpc_1noPC_2offset8PC,


Operator_arm_stralh_addi_1noPC_2general_3s9,


Operator_arm_stralh_subi_1noPC_2general_3offset8,


Operator_arm_stralh_add_1noPC_2general_3noPC,


Operator_arm_stralh_sub_1noPC_2general_3noPC,


Operator_arm_stralh_post_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralh_post_addi_2noPC_1noPC_2noPC_3s9,


Operator_arm_stralh_post_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralh_post_subi_2noPC_1noPC_2noPC_3offset8,


Operator_arm_stralh_pre_add_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralh_pre_addi_2noPC_1noPC_2noPC_3s9,


Operator_arm_stralh_pre_sub_2noPC_1noPC_2noPC_3noPC,


Operator_arm_stralh_pre_subi_2noPC_1noPC_2noPC_3offset8,


Operator_arm_stralh_addpc_1noPC_2offset8PC,


Operator_arm_stralh_subpc_1noPC_2offset8PC,


Operator_arm_ldmal_1amode4L_2noPC_3reglist,


Operator_arm_ldmal_w_2noPC_1amode4L_2noPC_3reglist,


Operator_arm_ldmal_pc_1amode4L_2noPC_3reglist,


Operator_arm_ldmal_pc_w_2noPC_1amode4L_2noPC_3reglist,


Operator_arm_ldmal_s10_1amode4L_2noPC_3reglist,


Operator_arm_ldmal_s11_1amode4L_2noPC_3reglist,


Operator_arm_ldmal_s11_w_2noPC_1amode4L_2noPC_3reglist,


Operator_arm_stmal_1amode4S_2noPC_3reglist,


Operator_arm_stmal_w_2noPC_1amode4S_2noPC_3reglist,


Operator_arm_stmal_s1_1amode4S_2noPC_3reglist,


Operator_arm_swpal_1noPC_2noPC_3noPC,


Operator_arm_swpalb_1noPC_2noPC_3noPC,


Operator_arm_swial_1swinum,


Operator_arm_cdpal_1cpnum_2opcode1d_3cpreg_4cpreg_5cpreg_6opcode2,


Operator_arm_ldcal_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldcal_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldcal_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcal_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcal_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcal_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcal_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_ldcall_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldcall_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_ldcall_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcall_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcall_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcall_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_ldcall_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_stcal_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stcal_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stcal_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcal_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcal_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcal_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcal_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_stcall_add_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stcall_sub_1cpnum_2cpreg_3general_4offset8CP,


Operator_arm_stcall_post_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcall_post_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcall_pre_add_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcall_pre_sub_3noPC_1cpnum_2cpreg_3noPC_4offset8CP,


Operator_arm_stcall_unindexed_1cpnum_2cpreg_3general_4option,


Operator_arm_mcral_1cpnum_2opcode1m_3noPC_4cpreg_5cpreg_6opcode2,


Operator_arm_mrcal_3noPC_1cpnum_2opcode1m_4cpreg_5cpreg_6opcode2,


Operator_arm_mrcal_pc_1cpnum_2opcode1m_3cpreg_4cpreg_5opcode2,


Operator_arm_smlabbal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlabtal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlatbal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlattal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlalbbal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlalbtal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlaltbal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlalttal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smulbbal_1noPC_2noPC_3noPC,


Operator_arm_smulbtal_1noPC_2noPC_3noPC,


Operator_arm_smultbal_1noPC_2noPC_3noPC,


Operator_arm_smulttal_1noPC_2noPC_3noPC,


Operator_arm_smlawbal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlawtal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smulwbal_1noPC_2noPC_3noPC,


Operator_arm_smulwtal_1noPC_2noPC_3noPC,


Operator_arm_qaddal_1noPC_2noPC_3noPC,


Operator_arm_qdaddal_1noPC_2noPC_3noPC,


Operator_arm_qdsubal_1noPC_2noPC_3noPC,


Operator_arm_qsubal_1noPC_2noPC_3noPC,


Operator_arm_ldrald_addi_1paired_2general_3s9,


Operator_arm_MULTI_ldrald_addi_1paired_2general_3s9,


Operator_arm_ldrald_subi_1paired_2general_3offset8,


Operator_arm_MULTI_ldrald_subi_1paired_2general_3offset8,


Operator_arm_ldrald_add_1paired_2general_3noPC,


Operator_arm_MULTI_ldrald_add_1paired_2general_3noPC,


Operator_arm_ldrald_sub_1paired_2general_3noPC,


Operator_arm_MULTI_ldrald_sub_1paired_2general_3noPC,


Operator_arm_ldrald_post_add_1paired_2noPC_2noPC_3noPC,


Operator_arm_MULTI_ldrald_post_add_1paired_2noPC_2noPC_3noPC,


Operator_arm_ldrald_post_addi_1paired_2noPC_2noPC_3s9,


Operator_arm_MULTI_ldrald_post_addi_1paired_2noPC_2noPC_3s9,


Operator_arm_ldrald_post_sub_1paired_2noPC_2noPC_3noPC,


Operator_arm_MULTI_ldrald_post_sub_1paired_2noPC_2noPC_3noPC,


Operator_arm_ldrald_post_subi_1paired_2noPC_2noPC_3offset8,


Operator_arm_MULTI_ldrald_post_subi_1paired_2noPC_2noPC_3offset8,


Operator_arm_ldrald_pre_add_1paired_2noPC_2noPC_3noPC,


Operator_arm_MULTI_ldrald_pre_add_1paired_2noPC_2noPC_3noPC,


Operator_arm_ldrald_pre_addi_1paired_2noPC_2noPC_3s9,


Operator_arm_MULTI_ldrald_pre_addi_1paired_2noPC_2noPC_3s9,


Operator_arm_ldrald_pre_sub_1paired_2noPC_2noPC_3noPC,


Operator_arm_MULTI_ldrald_pre_sub_1paired_2noPC_2noPC_3noPC,


Operator_arm_ldrald_pre_subi_1paired_2noPC_2noPC_3offset8,


Operator_arm_MULTI_ldrald_pre_subi_1paired_2noPC_2noPC_3offset8,


Operator_arm_ldrald_addpc_1paired_2offset8PC,


Operator_arm_MULTI_ldrald_addpc_1paired_2offset8PC,


Operator_arm_ldrald_subpc_1paired_2offset8PC,


Operator_arm_MULTI_ldrald_subpc_1paired_2offset8PC,


Operator_arm_strald_addi_1paired_2general_3s9,


Operator_arm_MULTI_strald_addi_1paired_2general_3s9,


Operator_arm_strald_subi_1paired_2general_3offset8,


Operator_arm_MULTI_strald_subi_1paired_2general_3offset8,


Operator_arm_strald_add_1paired_2general_3noPC,


Operator_arm_MULTI_strald_add_1paired_2general_3noPC,


Operator_arm_strald_sub_1paired_2general_3noPC,


Operator_arm_MULTI_strald_sub_1paired_2general_3noPC,


Operator_arm_strald_post_add_2noPC_1paired_2noPC_3noPC,


Operator_arm_MULTI_strald_post_add_2noPC_1paired_2noPC_3noPC,


Operator_arm_strald_post_addi_2noPC_1paired_2noPC_3s9,


Operator_arm_MULTI_strald_post_addi_2noPC_1paired_2noPC_3s9,


Operator_arm_strald_post_sub_2noPC_1paired_2noPC_3noPC,


Operator_arm_MULTI_strald_post_sub_2noPC_1paired_2noPC_3noPC,


Operator_arm_strald_post_subi_2noPC_1paired_2noPC_3offset8,


Operator_arm_MULTI_strald_post_subi_2noPC_1paired_2noPC_3offset8,


Operator_arm_strald_pre_add_2noPC_1paired_2noPC_3noPC,


Operator_arm_MULTI_strald_pre_add_2noPC_1paired_2noPC_3noPC,


Operator_arm_strald_pre_addi_2noPC_1paired_2noPC_3s9,


Operator_arm_MULTI_strald_pre_addi_2noPC_1paired_2noPC_3s9,


Operator_arm_strald_pre_sub_2noPC_1paired_2noPC_3noPC,


Operator_arm_MULTI_strald_pre_sub_2noPC_1paired_2noPC_3noPC,


Operator_arm_strald_pre_subi_2noPC_1paired_2noPC_3offset8,


Operator_arm_MULTI_strald_pre_subi_2noPC_1paired_2noPC_3offset8,


Operator_arm_strald_addpc_1paired_2offset8PC,


Operator_arm_MULTI_strald_addpc_1paired_2offset8PC,


Operator_arm_strald_subpc_1paired_2offset8PC,


Operator_arm_MULTI_strald_subpc_1paired_2offset8PC,


Operator_arm_mcrral_1cpnum_2opcode2_3noPC_4noPC_5cpreg,


Operator_arm_mrrcal_3noPC_4noPC_1cpnum_2opcode2_5cpreg,


Operator_arm_bxj_1cond_2general,


Operator_arm_sadd16_2noPC_1cond_3noPC_4noPC,


Operator_arm_saddsubx_2noPC_1cond_3noPC_4noPC,


Operator_arm_ssub16_2noPC_1cond_3noPC_4noPC,


Operator_arm_ssubaddx_2noPC_1cond_3noPC_4noPC,


Operator_arm_uadd16_2noPC_1cond_3noPC_4noPC,


Operator_arm_uaddsubx_2noPC_1cond_3noPC_4noPC,


Operator_arm_usub16_2noPC_1cond_3noPC_4noPC,


Operator_arm_usubaddx_2noPC_1cond_3noPC_4noPC,


Operator_arm_qadd16_2noPC_1cond_3noPC_4noPC,


Operator_arm_qaddsubx_2noPC_1cond_3noPC_4noPC,


Operator_arm_qsub16_2noPC_1cond_3noPC_4noPC,


Operator_arm_qsubaddx_2noPC_1cond_3noPC_4noPC,


Operator_arm_uqadd16_2noPC_1cond_3noPC_4noPC,


Operator_arm_uqaddsubx_2noPC_1cond_3noPC_4noPC,


Operator_arm_uqsub16_2noPC_1cond_3noPC_4noPC,


Operator_arm_uqsubaddx_2noPC_1cond_3noPC_4noPC,


Operator_arm_shadd16_2noPC_1cond_3noPC_4noPC,


Operator_arm_shaddsubx_2noPC_1cond_3noPC_4noPC,


Operator_arm_shsub16_2noPC_1cond_3noPC_4noPC,


Operator_arm_shsubaddx_2noPC_1cond_3noPC_4noPC,


Operator_arm_uhadd16_2noPC_1cond_3noPC_4noPC,


Operator_arm_uhaddsubx_2noPC_1cond_3noPC_4noPC,


Operator_arm_uhsub16_2noPC_1cond_3noPC_4noPC,


Operator_arm_uhsubaddx_2noPC_1cond_3noPC_4noPC,


Operator_arm_sadd8_2noPC_1cond_3noPC_4noPC,


Operator_arm_ssub8_2noPC_1cond_3noPC_4noPC,


Operator_arm_uadd8_2noPC_1cond_3noPC_4noPC,


Operator_arm_usub8_2noPC_1cond_3noPC_4noPC,


Operator_arm_qadd8_2noPC_1cond_3noPC_4noPC,


Operator_arm_qsub8_2noPC_1cond_3noPC_4noPC,


Operator_arm_uqadd8_2noPC_1cond_3noPC_4noPC,


Operator_arm_uqsub8_2noPC_1cond_3noPC_4noPC,


Operator_arm_shadd8_2noPC_1cond_3noPC_4noPC,


Operator_arm_shsub8_2noPC_1cond_3noPC_4noPC,


Operator_arm_uhadd8_2noPC_1cond_3noPC_4noPC,


Operator_arm_uhsub8_2noPC_1cond_3noPC_4noPC,


Operator_arm_sel_2noPC_1cond_3noPC_4noPC,


Operator_arm_pkhbt_lsl_imm_2noPC_1cond_3noPC_4noPC_5shfimml,


Operator_arm_pkhbt_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_pkhtb_2noPC_1cond_3noPC_4noPC_5shfimmr,


Operator_arm_sxtb_reg_2noPC_1cond_3noPC,


Operator_arm_sxtb_ror_2noPC_1cond_3noPC_4rot,


Operator_arm_sxtb16_reg_2noPC_1cond_3noPC,


Operator_arm_sxtb16_ror_2noPC_1cond_3noPC_4rot,


Operator_arm_sxth_reg_2noPC_1cond_3noPC,


Operator_arm_sxth_ror_2noPC_1cond_3noPC_4rot,


Operator_arm_uxtb_reg_2noPC_1cond_3noPC,


Operator_arm_uxtb_ror_2noPC_1cond_3noPC_4rot,


Operator_arm_uxtb16_reg_2noPC_1cond_3noPC,


Operator_arm_uxtb16_ror_2noPC_1cond_3noPC_4rot,


Operator_arm_uxth_reg_2noPC_1cond_3noPC,


Operator_arm_uxth_ror_2noPC_1cond_3noPC_4rot,


Operator_arm_sxtab_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_sxtab_ror_2noPC_1cond_3noPC_4noPC_5rot,


Operator_arm_sxtab16_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_sxtab16_ror_2noPC_1cond_3noPC_4noPC_5rot,


Operator_arm_sxtah_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_sxtah_ror_2noPC_1cond_3noPC_4noPC_5rot,


Operator_arm_uxtab_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_uxtab_ror_2noPC_1cond_3noPC_4noPC_5rot,


Operator_arm_uxtab16_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_uxtab16_ror_2noPC_1cond_3noPC_4noPC_5rot,


Operator_arm_uxtah_reg_2noPC_1cond_3noPC_4noPC,


Operator_arm_uxtah_ror_2noPC_1cond_3noPC_4noPC_5rot,


Operator_arm_ssat_asr_imm_2noPC_1cond_3ssat5_4noPC_5shfimmr,


Operator_arm_ssat_lsl_imm_2noPC_1cond_3ssat5_4noPC_5shfimml,


Operator_arm_ssat_reg_2noPC_1cond_3ssat5_4noPC,


Operator_arm_usat_asr_imm_2noPC_1cond_3usat5_4noPC_5shfimmr,


Operator_arm_usat_lsl_imm_2noPC_1cond_3usat5_4noPC_5shfimml,


Operator_arm_usat_reg_2noPC_1cond_3usat5_4noPC,


Operator_arm_ssat16_2noPC_1cond_3ssat4_4noPC,


Operator_arm_usat16_2noPC_1cond_3usat4_4noPC,


Operator_arm_usad8_2noPC_1cond_3noPC_4noPC,


Operator_arm_usada8_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smuad_2noPC_1cond_3noPC_4noPC,


Operator_arm_smuadx_2noPC_1cond_3noPC_4noPC,


Operator_arm_smusd_2noPC_1cond_3noPC_4noPC,


Operator_arm_smusdx_2noPC_1cond_3noPC_4noPC,


Operator_arm_smlad_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smladx_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlsd_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlsdx_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smlald_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlaldx_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlsld_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smlsldx_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_smmla_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smmlar_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smmls_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smmlsr_2noPC_1cond_3noPC_4noPC_5noPC,


Operator_arm_smmul_2noPC_1cond_3noPC_4noPC,


Operator_arm_smmulr_2noPC_1cond_3noPC_4noPC,


Operator_arm_umaal_2noPC_3noPC_1cond_2noPC_3noPC_4noPC_5noPC,


Operator_arm_rev_2noPC_1cond_3noPC,


Operator_arm_rev16_2noPC_1cond_3noPC,


Operator_arm_revsh_2noPC_1cond_3noPC,


Operator_arm_bxjal_1general,


Operator_arm_sadd16al_1noPC_2noPC_3noPC,


Operator_arm_saddsubxal_1noPC_2noPC_3noPC,


Operator_arm_ssub16al_1noPC_2noPC_3noPC,


Operator_arm_ssubaddxal_1noPC_2noPC_3noPC,


Operator_arm_uadd16al_1noPC_2noPC_3noPC,


Operator_arm_uaddsubxal_1noPC_2noPC_3noPC,


Operator_arm_usub16al_1noPC_2noPC_3noPC,


Operator_arm_usubaddxal_1noPC_2noPC_3noPC,


Operator_arm_qadd16al_1noPC_2noPC_3noPC,


Operator_arm_qaddsubxal_1noPC_2noPC_3noPC,


Operator_arm_qsub16al_1noPC_2noPC_3noPC,


Operator_arm_qsubaddxal_1noPC_2noPC_3noPC,


Operator_arm_uqadd16al_1noPC_2noPC_3noPC,


Operator_arm_uqaddsubxal_1noPC_2noPC_3noPC,


Operator_arm_uqsub16al_1noPC_2noPC_3noPC,


Operator_arm_uqsubaddxal_1noPC_2noPC_3noPC,


Operator_arm_shadd16al_1noPC_2noPC_3noPC,


Operator_arm_shaddsubxal_1noPC_2noPC_3noPC,


Operator_arm_shsub16al_1noPC_2noPC_3noPC,


Operator_arm_shsubaddxal_1noPC_2noPC_3noPC,


Operator_arm_uhadd16al_1noPC_2noPC_3noPC,


Operator_arm_uhaddsubxal_1noPC_2noPC_3noPC,


Operator_arm_uhsub16al_1noPC_2noPC_3noPC,


Operator_arm_uhsubaddxal_1noPC_2noPC_3noPC,


Operator_arm_sadd8al_1noPC_2noPC_3noPC,


Operator_arm_ssub8al_1noPC_2noPC_3noPC,


Operator_arm_uadd8al_1noPC_2noPC_3noPC,


Operator_arm_usub8al_1noPC_2noPC_3noPC,


Operator_arm_qadd8al_1noPC_2noPC_3noPC,


Operator_arm_qsub8al_1noPC_2noPC_3noPC,


Operator_arm_uqadd8al_1noPC_2noPC_3noPC,


Operator_arm_uqsub8al_1noPC_2noPC_3noPC,


Operator_arm_shadd8al_1noPC_2noPC_3noPC,


Operator_arm_shsub8al_1noPC_2noPC_3noPC,


Operator_arm_uhadd8al_1noPC_2noPC_3noPC,


Operator_arm_uhsub8al_1noPC_2noPC_3noPC,


Operator_arm_selal_1noPC_2noPC_3noPC,


Operator_arm_pkhbtal_lsl_imm_1noPC_2noPC_3noPC_4shfimml,


Operator_arm_pkhbtal_reg_1noPC_2noPC_3noPC,


Operator_arm_pkhtbal_1noPC_2noPC_3noPC_4shfimmr,


Operator_arm_sxtbal_reg_1noPC_2noPC,


Operator_arm_sxtbal_ror_1noPC_2noPC_3rot,


Operator_arm_sxtb16al_reg_1noPC_2noPC,


Operator_arm_sxtb16al_ror_1noPC_2noPC_3rot,


Operator_arm_sxthal_reg_1noPC_2noPC,


Operator_arm_sxthal_ror_1noPC_2noPC_3rot,


Operator_arm_uxtbal_reg_1noPC_2noPC,


Operator_arm_uxtbal_ror_1noPC_2noPC_3rot,


Operator_arm_uxtb16al_reg_1noPC_2noPC,


Operator_arm_uxtb16al_ror_1noPC_2noPC_3rot,


Operator_arm_uxthal_reg_1noPC_2noPC,


Operator_arm_uxthal_ror_1noPC_2noPC_3rot,


Operator_arm_sxtabal_reg_1noPC_2noPC_3noPC,


Operator_arm_sxtabal_ror_1noPC_2noPC_3noPC_4rot,


Operator_arm_sxtab16al_reg_1noPC_2noPC_3noPC,


Operator_arm_sxtab16al_ror_1noPC_2noPC_3noPC_4rot,


Operator_arm_sxtahal_reg_1noPC_2noPC_3noPC,


Operator_arm_sxtahal_ror_1noPC_2noPC_3noPC_4rot,


Operator_arm_uxtabal_reg_1noPC_2noPC_3noPC,


Operator_arm_uxtabal_ror_1noPC_2noPC_3noPC_4rot,


Operator_arm_uxtab16al_reg_1noPC_2noPC_3noPC,


Operator_arm_uxtab16al_ror_1noPC_2noPC_3noPC_4rot,


Operator_arm_uxtahal_reg_1noPC_2noPC_3noPC,


Operator_arm_uxtahal_ror_1noPC_2noPC_3noPC_4rot,


Operator_arm_ssatal_asr_imm_1noPC_2ssat5_3noPC_4shfimmr,


Operator_arm_ssatal_lsl_imm_1noPC_2ssat5_3noPC_4shfimml,


Operator_arm_ssatal_reg_1noPC_2ssat5_3noPC,


Operator_arm_usatal_asr_imm_1noPC_2usat5_3noPC_4shfimmr,


Operator_arm_usatal_lsl_imm_1noPC_2usat5_3noPC_4shfimml,


Operator_arm_usatal_reg_1noPC_2usat5_3noPC,


Operator_arm_ssat16al_1noPC_2ssat4_3noPC,


Operator_arm_usat16al_1noPC_2usat4_3noPC,


Operator_arm_usad8al_1noPC_2noPC_3noPC,


Operator_arm_usada8al_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smuadal_1noPC_2noPC_3noPC,


Operator_arm_smuadxal_1noPC_2noPC_3noPC,


Operator_arm_smusdal_1noPC_2noPC_3noPC,


Operator_arm_smusdxal_1noPC_2noPC_3noPC,


Operator_arm_smladal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smladxal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlsdal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlsdxal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlaldal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlaldxal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlsldal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smlsldxal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smmlaal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smmlaral_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smmlsal_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smmlsral_1noPC_2noPC_3noPC_4noPC,


Operator_arm_smmulal_1noPC_2noPC_3noPC,


Operator_arm_smmulral_1noPC_2noPC_3noPC,


Operator_arm_umaalal_1noPC_2noPC_1noPC_2noPC_3noPC_4noPC,


Operator_arm_reval_1noPC_2noPC,


Operator_arm_rev16al_1noPC_2noPC,


Operator_arm_revshal_1noPC_2noPC,



#undef Operator


#define Operator_IMMEDIATE_COUNT_MAX 2




#define OperatorParameter_CONFLICTS_COUNT_MAX 4



#define OperatorParameters(INDEX,PARAMETERS) 
#undef OperatorParameters


#define OperatorParameterArray_COUNT_MAX 11


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
  arm/Bundling.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Bundling


#define Bundling_LOG2_MAX 0


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
  arm/Template.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Template


#define Template_SYLLABLES_COUNT_MAX 0



#define Template_INCREMENT_MAX 0


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
  arm/Bundle.enum --
  Automatically generated from the Machine Description System (MDS).
*/




#undef Bundle


#define Bundle_ISSUE_MAX 0



#define BundleMatch(HASH,CANONIC,BUNDLES) 


#undef BundleMatch


#define BundleMatch_HASH_MAX 0



#define BundleMatch_COUNT_MAX 0


  Bundle__
#undef Bundle
} enum_Bundle;

/*
 * Bundle_INIT --	Initialize the Bundle module.
 */




#define Bundle_INIT() 


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
  arm/Reservation.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Reservation_armv5e_ISSUE,
Reservation_armv5e_ISSUE2,
Reservation_armv5e_ISSUE3,
Reservation_armv5e_ISSUE4,
Reservation_armv5e_ISSUE5,
Reservation_armv5e_SWITCH,
Reservation_armv6_ALU,
Reservation_armv6_ALU2,
Reservation_armv6_ALU3,
Reservation_armv6_ALU4,
Reservation_armv6_ALU5,
Reservation_armv6_MAC,
Reservation_armv6_MAC2,
Reservation_armv6_MAC3,
Reservation_armv6_MAC4,
Reservation_armv6_MAC5,
Reservation_armv6_MAC6,
Reservation_armv6_MEM,
Reservation_armv6_MEM2,
Reservation_armv6_SWITCH,

#undef Reservation


#define ReservationColumn(COLUMNINDEX,REQUIREMENTS) 
#undef ReservationColumn


#define Reservation_COLUMNS_COUNT_MAX 6


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
  arm/Scheduling.enum --
  Automatically generated from the Machine Description System (MDS).
*/



Scheduling_armv5e_ALU,
Scheduling_armv5e_ALU2,
Scheduling_armv5e_ALU3,
Scheduling_armv5e_MAC,
Scheduling_armv5e_MAC2,
Scheduling_armv5e_MAC3,
Scheduling_armv5e_MAC4,
Scheduling_armv5e_MAC5,
Scheduling_armv5e_MEM,
Scheduling_armv5e_MEM2,
Scheduling_armv5e_SWITCH,
Scheduling_armv6_ALU,
Scheduling_armv6_ALU2,
Scheduling_armv6_ALU3,
Scheduling_armv6_ALU4,
Scheduling_armv6_ALU5,
Scheduling_armv6_MAC,
Scheduling_armv6_MAC2,
Scheduling_armv6_MAC3,
Scheduling_armv6_MAC4,
Scheduling_armv6_MAC5,
Scheduling_armv6_MAC6,
Scheduling_armv6_MEM,
Scheduling_armv6_MEM2,
Scheduling_armv6_SWITCH,

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
  arm/Convention.enum --
  Automatically generated from the Machine Description System (MDS).
*/







Convention_arm_PIC,
Convention_armv6_PIC,
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
  arm/Encoding.enum --
  Automatically generated from the Machine Description System (MDS).
*/



extern Instance Decode_Encoding_armv5e_ANY(const void *buffer);
extern Instance Decode_Encoding_armv6_ANY(const void *buffer);

#undef Encoding







/*
 * Decode_Modifier functions.
 */
#define DECODE(e) e
#define Modifier(NAME,VALUES,ENCODE,DECODE) static inline void Decode_Modifier_ ##NAME(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; DECODE; *(OperandDecoded *)result = VALUE; }
/*
  arm/Modifier.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline void Decode_Modifier_arm_cond(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE; }
static inline void Decode_Modifier_arm_S(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 15; *(OperandDecoded *)result = VALUE; }





static inline void Decode_Modifier_arm_cpnum(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 17; *(OperandDecoded *)result = VALUE; }
static inline void Decode_Modifier_arm_cpreg(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 33; *(OperandDecoded *)result = VALUE; }
static inline void Decode_Modifier_arm_shift(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 49; *(OperandDecoded *)result = VALUE; }







static inline void Decode_Modifier_arm_amode4(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 53; *(OperandDecoded *)result = VALUE; }







static inline void Decode_Modifier_arm_amode4L(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 57; *(OperandDecoded *)result = VALUE; }







static inline void Decode_Modifier_arm_amode4S(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 61; *(OperandDecoded *)result = VALUE; }







static inline void Decode_Modifier_arm_psrfld(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 65; *(OperandDecoded *)result = VALUE; }
#undef Modifier





#undef DECODE

/*
 * Decode_Immediate functions.
 */
#define DECODE(e) e
#define Immediate(NAME,MINVALUE,MAXVALUE,SIGNED,RELOCATIONS,ENCODE,DECODE) static inline void Decode_Immediate_ ##NAME(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; DECODE; *ImmediateConstant__VALUE(constant) = VALUE; }
/*
  arm/Immediate.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline void Decode_Immediate_arm_gen_s32(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-32); __value >>= (sizeof(__value)*8-32); VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_btarg1(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-24); __value >>= (sizeof(__value)*8-24); __value <<= 2; __value += 8; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_btarg2(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-25); __value >>= (sizeof(__value)*8-25); __value <<= 1; __value += 8; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_immed8(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_s9(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-9); __value >>= (sizeof(__value)*8-9); VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_s13(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= (sizeof(__value)*8-13); __value >>= (sizeof(__value)*8-13); VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_rotimm(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= 1; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_shfimml(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_shfimmr(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_offset12(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_offset12PC(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value += 8; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_offset8(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_offset8PC(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value += 8; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_bkptnum(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_swinum(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_offset8CP(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= 2; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_option(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_opcode1d(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_opcode1m(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_opcode2(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_rot(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value <<= 3; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_immed5(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_ssat5(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value += -1; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_usat5(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_immed4(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_ssat4(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; __value += -1; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }



static inline void Decode_Immediate_arm_usat4(OperandEncoded encoded, void *result) { ImmediateConstant constant = (ImmediateConstant)result; ImmediateValue VALUE = (ImmediateValue)encoded; int32_t __value = VALUE; VALUE = __value; *ImmediateConstant__VALUE(constant) = VALUE; }




#undef Immediate





#undef DECODE
#undef VALUETYPE

/*
 * Decode_RegClass functions.
 */
#define DECODE(e) e
#define RegClass(NAME,REGISTERS,REGFILE,ENCODE,DECODE,MINREADSTAGES) static inline void Decode_RegClass_ ##NAME(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; DECODE; *(OperandDecoded *)result = VALUE + Register__BIAS; }
/*
  arm/RegClass.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline void Decode_RegClass_arm_general(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_arm_noPC(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_arm_lr(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_arm_pc(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_arm_paired(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 16; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_arm_pairedFirst(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; /**/; *(OperandDecoded *)result = VALUE + (Register_ + 1); }





static inline void Decode_RegClass_arm_pairedSecond(OperandEncoded encoded, void *result) { OperandDecoded VALUE = encoded; VALUE += 1; *(OperandDecoded *)result = VALUE + (Register_ + 1); }






#undef RegClass

#undef DECODE

/*
 * Decode_RegMask functions.
 */
#define DECODE(e) e
#define RegMask(NAME,REGISTERS,REGFILE,ENCODE,DECODE) static inline void Decode_RegMask_ ##NAME(OperandEncoded encoded, void *result) { RegisterSet registerSet = (RegisterSet)result; RegisterSetWord WORD = (RegisterSetWord)encoded; RegisterSetWord *WORDS = RegisterSet_WORDS(registerSet); DECODE; }
/*
  arm/RegMask.enum --
  Automatically generated from the Machine Description System (MDS).
*/



static inline void Decode_RegMask_arm_reglist(OperandEncoded encoded, void *result) { RegisterSet registerSet = (RegisterSet)result; RegisterSetWord WORD = (RegisterSetWord)encoded; RegisterSetWord *WORDS = RegisterSet_WORDS(registerSet); WORDS[0] = WORD<<0; }





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
Decode_Encoding_armv5e_ANY(const void *buffer);


//

extern Instance
Decode_Encoding_armv6_ANY(const void *buffer);





/*
 * MDS_Test --	Setup and execute a MDS test.
 */
void
MDS_Test(void (*test)(void));

//
//#define MDS_FAMILY "arm"
//#define MDS_FAMILY_arm
#define MDS_CORE_armv5e 
#define MDS_CORE_armv6 
