
#define __LAI_H__

/*!
  @file LAI.h
  
  Revision: 1.14  $Date$
  
  @author Benoit Dupont de Dinechin (Benoit.Dupont-de-Dinechin@st.com).
  @author Christophe Guillon (Christophe.Guillon@st.com).
  
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/


/*!
  Declare Opaque LAO types.
*/

typedef struct Interface_ *Interface;
typedef struct Label_ *Label;
typedef struct Symbol_ *Symbol;
typedef struct Temporary_ *Temporary;
typedef struct Operation_ *Operation;
typedef struct BasicBlock_ *BasicBlock;
typedef struct LoopInfo_ *LoopInfo;
typedef struct CodeRegion_ *CodeRegion;

#define __INTERFACE_E__

/*!
  @file Interface.e
  This file was automatically generated from Interface.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  Declare opaque host compiler CGIR types.
*/
typedef uint32_t CGIR_LAB;
typedef uint32_t CGIR_SYM;
typedef struct tn *CGIR_TN;
typedef struct op *CGIR_OP;
typedef struct bb *CGIR_BB;
typedef struct LOOP_DESCR *CGIR_LD;
typedef struct region_id *CGIR_RID;

/*!
  Interface LAO enumarations.
*/
typedef int LAI_ConfigurationItem;
typedef int LAI_DependenceType;

/*!
  Interface LAO target enumarations.
*/
typedef int LAI_Immediate;
typedef int LAI_InstrMode;
typedef int LAI_MType;
typedef int LAI_Modifier;
typedef int LAI_Operator;
typedef int LAI_RegClass;
typedef int LAI_Register;

/*!
  Import LAO enumerations definitions.
*/

#define __LAO_E__

/*!
  @file LAO.e
  This file was automatically generated from LAO.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

// Exported enumerations

#define __REGIONTYPE_E__

/*!
  @file RegionType.e
  This file was automatically generated from RegionType.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  RegionType -- enumerates the RegionType types.
*/
typedef enum {
  RegionType_BasicBlock,
  RegionType_SuperBlock,
  RegionType_TraceBlock,
  RegionType_HyperBlock,
  RegionType_BasicInner,
  RegionType_SuperInner,
  RegionType_TraceInner,
  RegionType_HyperInner,
  RegionType__
} RegionType;
extern const char *
RegionTypeNAME[];
#define RegionTypeNAME(type) RegionTypeNAME[type]


//__REGIONTYPE_E__


#define __OPTIMIZATION_E__

/*!
  @file Optimization.e
  This file was automatically generated from Optimization.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  OptimizationPass -- Enumerate the LAO optimization phases.
*/
typedef enum {
  Optimization_Linearize = 0x1,
  Optimization_PostSched = 0x2,
  Optimization_RegAlloc = 0x4,
  Optimization_PreSched = 0x8,
  Optimization_LoopUnwind = 0x10,
  Optimization_LoopUnroll = 0x20,
  Optimization_EnableSSA = 0x40,
  Optimization_StartBlock = 0x100,
  Optimization_StopBlock = 0x200,
  Optimization_Localize = 0x400,
  // Used to force passing through interface
  Optimization_Force_PostPass = 0x1000,
  Optimization_Force_RegAlloc = 0x2000,
  Optimization_Force_PrePass = 0x4000
} OptimizationPass;
const char *
OptimizationPassNAMES(unsigned phases);

/*!
  OptimizationPrePass_Mask -- Mask of optimizations allowed in prepass.
*/
#define Optimization_PrePass_Mask (\
  Optimization_PreSched |\
  Optimization_LoopUnwind |\
  Optimization_LoopUnroll |\
  Optimization_EnableSSA |\
  Optimization_StartBlock |\
  Optimization_StopBlock |\
  Optimization_Force_PrePass | \
  0)

/*!
  OptimizationRegAlloc_Mask -- Mask of optimization allowed in regalloc pass.
*/
#define Optimization_RegAlloc_Mask (\
  Optimization_RegAlloc |\
  Optimization_Localize |\
  Optimization_Force_RegAlloc | \
  0)

/*!
  OptimizationPostPass_Mask -- Mask of optimization allowed in postpass.
*/
#define Optimization_PostPass_Mask (\
  Optimization_PostSched |\
  Optimization_Force_PostPass | \
  0)
  


//__OPTIMIZATION_E__


#define __CONFIGURATION_E__

/*!
  @file Configuration.e
  This file was automatically generated from Configuration.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  ConfigurationItem -- Enumerate the Configuration items.
  @par
  Each ConfigurationItem can have a negative value, meaning that the value should be
  ignored. Each Configuration_Control can have a zero value, meaning that the
  corresponding assertion is at the safest level.
*/
typedef enum {
  Configuration_LogMaxBBOC,	//!< Log2 of maximum operation count in a BasicBlock.
  Configuration_RegionType,	//!< RegionType for instruction scheduling.
    // 0 => basic block region,
    // 1 => super block region,
    // 2 => trace block region.
  Configuration_InstrMode,	//!< Instruction Mode (ISA Subset).
  Configuration_SchedKind,	//!< Instruction scheduling kind.
    // 0 => sequential scheduling,
    // 1 => dependence scheduling,
    // 2 => insertion scheduling,
    // 3 => convergent scheduling.
  Configuration_AllocKind,	//!< Register allocation kind.
    // 0 => local register allocation,
    // 1 => global register allocation,
    // 2 => integrated register allocation.
  Configuration_Pipelining,	//!< Software pipelining level.
    // 0 => cyclic instruction schedule,
    // 1 => software pipelining with overlap 1,
    // n => software pipelining with overlap (1<<n)-1.
  Configuration_LogUnwind,	//!< Log2 of default unwind factor.
    // 0 => no unwind,
    // 1 => unwind 2,
    // n => unwind 1<<n.
  Configuration_LogUnroll,	//!< Log2 of default unroll factor.
    // 0 => no unroll,
    // 1 => unroll 2,
    // n => unroll 1<<n.
  Configuration_Speculation,	//!< Software speculation level.
    // 0 => no software speculation,
    // 1 => software speculation of non-excepting instructions.
    // 2 => software speculation of dismissable instructions (advanced LOADs).
    // 3 => software speculation of non-dismissable instructions (regular LOADs).
  Configuration_Renaming,	//!< Register renaming level.
    // 0 => no register renaming,
    // 1 => register renaming,
    // n => modulo renaming over n iterations.
  Configuration_LoopOpt,	//!< Targets of loop optimizations.
    // 0 => no loop optimizations,
    // 1 => innermost loops only,
    // n => n innermost loops.
  Configuration_LoopDep,	//!< Loop memory dependences assumed.
    // 0 => serial loop dependences,
    // 1 => scalar loop dependences,
    // 2 => vector loop dependences,
    // 3 => parallel loop dependences,
    // 4 => liberal loop dependences.
  Configuration_MinTrip,	//!< Minimum value of loop trip count.
  Configuration_Modulus,	//!< Modulus of loop trip count.
  Configuration_Residue,	//!< Residue of loop trip count.
  Configuration_StackModel,	//!< Stack model for symbol allocation.
    // 0 => stack model small,
    // 1 => stack model large,
    // 2 => stack model dynamic.
  ConfigurationItem__
} ConfigurationItem;
extern const char *
ConfigurationItemNAME[];
#define ConfigurationItemNAME(type) ConfigurationItemNAME[type]


//__CONFIGURATION_E__


#define __DEPENDENCE_E__

/*!
  @file Dependence.e
  This file was automatically generated from Dependence.xcc. DO NOT EDIT!
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/

/*!
  DependenceType -- Enumerates the Dependence types.
*/
typedef enum {
  Dependence_Flow,	// Flow memory dependence arc.
  Dependence_Anti,	// Anti memory dependence arc.
  Dependence_Input,	// Input memory dependence arc.
  Dependence_Output,	// Output memory dependence arc.
  Dependence_Spill,	// Spill (flow) memory dependence arc.
  Dependence_Other,	// Other memory dependence arc.
  Dependence_RAW,	// Register Read After Write dependence arc.
  Dependence_WAR,	// Register Write After Read dependence arc.
  Dependence_WAW,	// Register Write After Write dependence arc.
  Dependence_Volatile,	// Sequentialize the volatile operations.
  Dependence_Life,	// Register consumer node to its lifetime node.
  Dependence_Link,	// Register producer node to its lifetime node.
  Dependence_Post,	// Inserted by SchedGraph_postArc.
  Dependence_None,	// Inserted by SchedGraph_complete.
  Dependence_Span,	// Limits the span of the schedule.
  Dependence_Modulo,	// Limits the span of the modulo temporaries.
  DependenceType__,
  Dependence_Definite = 16,
  Dependence_Lifetime = 32
} DependenceType;
extern const char *
DependenceTypeNAME[];
#define DependenceTypeNAME(type) DependenceTypeNAME[(type) & (16 - 1)]
#define Dependence_isDefinite(type) (((type) & Dependence_Definite) != 0)
#define Dependence_isLifetime(type) (((type) & Dependence_Lifetime) != 0)
#define Dependence_mayAdjust(type) (((type) & (16 - 1)) <= Dependence_Volatile)


//__DEPENDENCE_E__



//__LAO_E__


/*!
  LAI_SClass -- Used for symbol class definition.
  Must match the LAO Sclass definition in LIR/Symbol.h.
*/
typedef enum {
  LAI_SClass_UNDEF,
  LAI_SClass_VAR,
  LAI_SClass_FUNC,
  LAI_SClass_CONST,
  LAI_SClass_PREG,
  LAI_SClass_BLOCK,
  LAI_SClass__
} LAI_SClass;

/*!
  LAI_SStorage -- Used for symbol storage definition.
  Must match the LAO SStorage definition in LIR/Symbol.h.
*/
typedef enum {
  LAI_SStorage_UNDEF,
  LAI_SStorage_AUTO,
  LAI_SStorage_FORMAL,
  LAI_SStorage_FORMAL_REF,
  LAI_SStorage_PSTATIC,
  LAI_SStorage_FSTATIC,
  LAI_SStorage_COMMON,
  LAI_SStorage_EXTERN,
  LAI_SStorage_UGLOBAL,
  LAI_SStorage_DGLOBAL,
  LAI_SStorage_TEXT,
  LAI_SStorage_REG,
  LAI_SStorage__
} LAI_SStorage;

/*!
  LAI_SExport -- Used for symbol export definition.
  Must match the LAO SExport definition in LIR/Symbol.h.
*/
typedef enum {
  LAI_SExport_UNDEF,
  LAI_SExport_LOCAL,
  LAI_SExport_LOCAL_INTERNAL,
  LAI_SExport_GLOBAL_INTERNAL,
  LAI_SExport_GLOBAL_HIDDEN,
  LAI_SExport_GLOBAL_PROTECTED,
  LAI_SExport_GLOBAL_PREEMPTIBLE,
  LAI_SExport__
} LAI_SExport;


//__INTERFACE_E__



/*!
  CGIR callbacks.
*/

#define __CGIR_H__

/*!
  @file CGIR.h
  
  Revision: 1.11  $Date$
  
  @author Benoit Dupont de Dinechin (Benoit.Dupont-de-Dinechin@st.com).
  @author Christophe Guillon (Christophe.Guillon@st.com).
  
  @copy Copyright 2002 - 2003 STMicroelectronics.
  @copy Copyright 1995 - 1998 Commissariat a l'Energie Atomique.
  @copy This program is free software; you can redistribute it and/or
  @copy modify it under the terms of either (at your option): the GNU
  @copy General Public License (GPL) version 2; the GNU Lesser General
  @copy Public License (LGPL) version 2.1; any later version of these
  @copy licences as published by the Free Software Foundation.
*/


/*!
  CGIR_CallBack -- Call back functions.
*/
struct CGIR_CallBack_ {
  CGIR_LAB (*LAB_create)(Label label, CGIR_LAB cgir_lab);
  void (*LAB_update)(Label label, CGIR_LAB cgir_lab);
  CGIR_SYM (*SYM_create)(Symbol symbol, CGIR_SYM cgir_sym);
  void (*SYM_update)(Symbol symbol, CGIR_SYM cgir_sym);
  CGIR_TN (*Dedicated_TN_create)(Temporary temporary, CGIR_TN cgir_tn);
  CGIR_TN (*PseudoReg_TN_create)(Temporary temporary, CGIR_TN cgir_tn);
  CGIR_TN (*AssignReg_TN_create)(Temporary temporary, CGIR_TN cgir_tn);
  CGIR_TN (*Modifier_TN_create)(Temporary temporary, CGIR_TN cgir_tn);
  CGIR_TN (*Absolute_TN_create)(Temporary temporary, CGIR_TN cgir_tn);
  CGIR_TN (*Symbol_TN_create)(Temporary temporary, CGIR_TN cgir_tn, CGIR_SYM cgir_sym);
  CGIR_TN (*Label_TN_create)(Temporary temporary, CGIR_TN cgir_tn, CGIR_LAB cgir_lab);
  void (*TN_update)(Temporary temporary, CGIR_TN cgir_tn);
  CGIR_OP (*OP_create)(Operation operation, CGIR_OP cgir_op, CGIR_TN arguments[], CGIR_TN results[], int unrolled);
  void (*OP_update)(Operation operation, CGIR_OP cgir_op, CGIR_TN arguments[], CGIR_TN results[], int unrolled);
  CGIR_BB (*BB_create)(BasicBlock basicblock, CGIR_BB cgir_bb, CGIR_LAB labels[], CGIR_OP operations[], CGIR_RID cgir_rid, unsigned optimizations);
  void (*BB_update)(BasicBlock basicblock, CGIR_BB cgir_bb, CGIR_LAB labels[], CGIR_OP operations[], CGIR_RID cgir_rid, unsigned optimizations);
  void (*BB_chain)(CGIR_BB cgir_bb, CGIR_BB succ_cgir_bb);
  void (*BB_unchain)(CGIR_BB cgir_bb);
  void (*BB_link)(CGIR_BB tail_cgir_bb, CGIR_BB head_cgir_bb, float probability);
  void (*BB_unlink)(CGIR_BB cgir_bb, int preds, int succs);
  CGIR_LD (*LD_create)(LoopInfo loopInfo, CGIR_LD cgir_ld, CGIR_BB head_bb);
  void (*LD_update)(LoopInfo loopInfo, CGIR_LD cgir_ld, CGIR_BB head_bb);
};

typedef struct CGIR_CallBack_ CGIR_CallBack_, *CGIR_CallBack;
#define CGIR_CallBack_LAB_create(this) (0, (this)->LAB_create)
#define CGIR_CallBack__LAB_create(this) (&(this)->LAB_create)
#define CGIR_CallBack_LAB_update(this) (0, (this)->LAB_update)
#define CGIR_CallBack__LAB_update(this) (&(this)->LAB_update)
#define CGIR_CallBack_SYM_create(this) (0, (this)->SYM_create)
#define CGIR_CallBack__SYM_create(this) (&(this)->SYM_create)
#define CGIR_CallBack_SYM_update(this) (0, (this)->SYM_update)
#define CGIR_CallBack__SYM_update(this) (&(this)->SYM_update)
#define CGIR_CallBack_Dedicated_TN_create(this) (0, (this)->Dedicated_TN_create)
#define CGIR_CallBack__Dedicated_TN_create(this) (&(this)->Dedicated_TN_create)
#define CGIR_CallBack_PseudoReg_TN_create(this) (0, (this)->PseudoReg_TN_create)
#define CGIR_CallBack__PseudoReg_TN_create(this) (&(this)->PseudoReg_TN_create)
#define CGIR_CallBack_AssignReg_TN_create(this) (0, (this)->AssignReg_TN_create)
#define CGIR_CallBack__AssignReg_TN_create(this) (&(this)->AssignReg_TN_create)
#define CGIR_CallBack_Modifier_TN_create(this) (0, (this)->Modifier_TN_create)
#define CGIR_CallBack__Modifier_TN_create(this) (&(this)->Modifier_TN_create)
#define CGIR_CallBack_Absolute_TN_create(this) (0, (this)->Absolute_TN_create)
#define CGIR_CallBack__Absolute_TN_create(this) (&(this)->Absolute_TN_create)
#define CGIR_CallBack_Symbol_TN_create(this) (0, (this)->Symbol_TN_create)
#define CGIR_CallBack__Symbol_TN_create(this) (&(this)->Symbol_TN_create)
#define CGIR_CallBack_Label_TN_create(this) (0, (this)->Label_TN_create)
#define CGIR_CallBack__Label_TN_create(this) (&(this)->Label_TN_create)
#define CGIR_CallBack_TN_update(this) (0, (this)->TN_update)
#define CGIR_CallBack__TN_update(this) (&(this)->TN_update)
#define CGIR_CallBack_OP_create(this) (0, (this)->OP_create)
#define CGIR_CallBack__OP_create(this) (&(this)->OP_create)
#define CGIR_CallBack_OP_update(this) (0, (this)->OP_update)
#define CGIR_CallBack__OP_update(this) (&(this)->OP_update)
#define CGIR_CallBack_BB_create(this) (0, (this)->BB_create)
#define CGIR_CallBack__BB_create(this) (&(this)->BB_create)
#define CGIR_CallBack_BB_update(this) (0, (this)->BB_update)
#define CGIR_CallBack__BB_update(this) (&(this)->BB_update)
#define CGIR_CallBack_BB_chain(this) (0, (this)->BB_chain)
#define CGIR_CallBack__BB_chain(this) (&(this)->BB_chain)
#define CGIR_CallBack_BB_unchain(this) (0, (this)->BB_unchain)
#define CGIR_CallBack__BB_unchain(this) (&(this)->BB_unchain)
#define CGIR_CallBack_BB_link(this) (0, (this)->BB_link)
#define CGIR_CallBack__BB_link(this) (&(this)->BB_link)
#define CGIR_CallBack_BB_unlink(this) (0, (this)->BB_unlink)
#define CGIR_CallBack__BB_unlink(this) (&(this)->BB_unlink)
#define CGIR_CallBack_LD_create(this) (0, (this)->LD_create)
#define CGIR_CallBack__LD_create(this) (&(this)->LD_create)
#define CGIR_CallBack_LD_update(this) (0, (this)->LD_update)
#define CGIR_CallBack__LD_update(this) (&(this)->LD_update)

//__CGIR_H__


/*!
  LAI_Interface -- LAI Interface object.
  All functions for the LAO Interface are available
  from this object.
*/
struct LAI_Interface_ {
  int size;
  void (*Interface_Initialize)(void);
  void (*Interface_Finalize)(void);
  Interface (*Interface_getInstance)(void);
  CodeRegion (*Interface_codeRegion)(Interface this);
  Label (*Interface_makeLabel)(Interface this, CGIR_LAB cgir_lab, const char *name);
  Label (*Interface_findLabel)(Interface this, CGIR_LAB cgir_lab);
  Symbol (*Interface_makeSymbol)(Interface this, CGIR_SYM cgir_sym, const char *name);
  void (*Interface_Symbol_setClasses)(Interface this, Symbol symbol, LAI_SClass sclass, LAI_SStorage sstorage, LAI_SExport sexport);
  Symbol (*Interface_findSymbol)(Interface this, CGIR_SYM cgir_sym);
  Temporary (*Interface_makeDedicatedTemporary)(Interface this, CGIR_TN cgir_tn, LAI_Register registre);
  Temporary (*Interface_makeAssignRegTemporary)(Interface this, CGIR_TN cgir_tn, LAI_Register registre);
  Temporary (*Interface_makePseudoRegTemporary)(Interface this, CGIR_TN cgir_tn, LAI_RegClass regClass);
  Temporary (*Interface_makeModifierTemporary)(Interface this, CGIR_TN cgir_tn, LAI_Modifier modifier, int64_t value);
  Temporary (*Interface_makeAbsoluteTemporary)(Interface this, CGIR_TN cgir_tn, LAI_Immediate immediate, int64_t value);
  Temporary (*Interface_makeSymbolTemporary)(Interface this, CGIR_TN cgir_tn, LAI_Immediate immediate, Symbol symbol, int64_t offset);
  Temporary (*Interface_makeLabelTemporary)(Interface this, CGIR_TN cgir_tn, LAI_Immediate immediate, Label label);
  void (*Interface_Temporary_setRematerializable)(Interface this, Temporary temporary, Temporary rematerializableValue);
  void (*Interface_Temporary_setHomeable)(Interface this, Temporary temporary, Temporary homeLocation);
  void (*Interface_Temporary_setDedicated)(Interface this, Temporary temporary);
  void (*Interface_Temporary_setGlobal)(Interface this, Temporary temporary);
  Temporary (*Interface_findTemporary)(Interface this, CGIR_TN cgir_tn);
  uint32_t (*Interface_Temporary_identity)(Temporary temporary);
  int (*Interface_Temporary_isAbsolute)(Temporary temporary);
  int64_t (*Interface_Temporary_value)(Temporary temporary);
  int (*Interface_Temporary_isSymbol)(Temporary temporary);
  Symbol (*Interface_Temporary_symbol)(Temporary temporary);
  int64_t (*Interface_Temporary_offset)(Temporary temporary);
  int (*Interface_Temporary_isModifier)(Temporary temporary);
  LAI_Modifier (*Interface_Temporary_modifier)(Temporary temporary);
  LAI_Immediate (*Interface_Temporary_immediate)(Temporary temporary);
  int (*Interface_Temporary_isPseudoReg)(Temporary temporary);
  int (*Interface_Temporary_isDedicated)(Temporary temporary);
  int (*Interface_Temporary_isAssignReg)(Temporary temporary);
  LAI_Register (*Interface_Temporary_assigned)(Temporary temporary);
  LAI_RegClass (*Interface_Temporary_regClass)(Temporary temporary);
  uint32_t (*Interface_Symbol_identity)(Symbol symbol);
  const char * (*Interface_Symbol_name)(Symbol symbol);
  int (*Interface_Symbol_isSpill)(Symbol symbol);
  LAI_MType (*Interface_Symbol_mtype)(Symbol symbol);
  uint32_t (*Interface_Operation_identity)(Operation operation);
  LAI_Operator (*Interface_Operation_operator)(Operation operation);
  int (*Interface_Operation_iteration)(Operation operation);
  int (*Interface_Operation_issueDate)(Operation operation);
  int (*Interface_Operation_isSpillCode)(Operation operation);
  int (*Interface_Operation_isVolatile)(Operation operation);
  uint32_t (*Interface_Label_identity)(Label label);
  const char * (*Interface_Label_name)(Label label);
  uint32_t (*Interface_BasicBlock_identity)(BasicBlock basicBlock);
  int (*Interface_BasicBlock_unrolled)(BasicBlock basicBlock);
  int (*Interface_BasicBlock_ordering)(BasicBlock basicBlock);
  uint32_t (*Interface_LoopInfo_identity)(LoopInfo loopInfo);
  int (*Interface_LoopInfo_unrolled)(LoopInfo loopInfo);
  Operation (*Interface_makeOperation)(Interface this, CGIR_OP cgir_op,
				       LAI_Operator operator, int argCount, Temporary arguments[], int resCount, Temporary results[],
				       int regCount, int registers[]);
  Operation (*Interface_findOperation)(Interface this, CGIR_OP cgir_op);
  void (*Interface_Operation_setVolatile)(Interface this, Operation operation);
  void (*Interface_Operation_setPrefetch)(Interface this, Operation operation);
  void (*Interface_Operation_setBarrier)(Interface this, Operation operation);
  void (*Interface_Operation_setSpillCode)(Interface this, Operation operation, Symbol symbol);
  BasicBlock (*Interface_makeBasicBlock)(Interface this, CGIR_BB cgir_bb, LAI_InstrMode instrMode,
					 int labelCount, Label labels[], int operationCount, Operation operations[]);
  BasicBlock (*Interface_findBasicBlock)(Interface this, CGIR_BB cgir_bb);
  void (*Interface_moreBasicBlock)(Interface this,
				   BasicBlock basicBlock, intptr_t regionId, float frequency,
				   int liveInCount, Temporary liveIns[], int liveOutCount, Temporary liveOuts[]);
  void (*Interface_linkBasicBlocks)(Interface this, BasicBlock tail_block, BasicBlock head_block, float probability);
  LoopInfo (*Interface_makeLoopInfo)(Interface this, CGIR_LD cgir_ld, BasicBlock basicBlock, LAI_ConfigurationItem item, ...);
  LoopInfo (*Interface_findLoopInfo)(Interface this, CGIR_LD cgir_ld);
  void (*Interface_LoopInfo_setDependenceNode)(Interface this, LoopInfo loopInfo, Operation operation);
  void (*Interface_LoopInfo_setDependenceArc)(Interface this, LoopInfo loopInfo,
					      Operation tail_operation, Operation head_operation, int latency, int omega, 
					      LAI_DependenceType type);
  void (*Interface_setBody)(Interface this, BasicBlock basicBlock);
  void (*Interface_setEntry)(Interface this, BasicBlock basicBlock);
  void (*Interface_setExit)(Interface this, BasicBlock basicBlock);
  void (*Interface_updateCGIR)(Interface this, CGIR_CallBack callback);
  CodeRegion (*Interface_open)(Interface this, const char *name, LAI_ConfigurationItem item, ...);
  unsigned (*Interface_optimize)(Interface this, unsigned optimizations);
  void (*Interface_close)(Interface this);
};

typedef struct LAI_Interface_ LAI_Interface_, *LAI_Interface;
#define LAI_Interface_size(this) (0, (this)->size)
#define LAI_Interface__size(this) (&(this)->size)
#define LAI_Interface_Interface_Initialize(this) (0, (this)->Interface_Initialize)
#define LAI_Interface__Interface_Initialize(this) (&(this)->Interface_Initialize)
#define LAI_Interface_Interface_Finalize(this) (0, (this)->Interface_Finalize)
#define LAI_Interface__Interface_Finalize(this) (&(this)->Interface_Finalize)
#define LAI_Interface_Interface_getInstance(this) (0, (this)->Interface_getInstance)
#define LAI_Interface__Interface_getInstance(this) (&(this)->Interface_getInstance)
#define LAI_Interface_Interface_codeRegion(this) (0, (this)->Interface_codeRegion)
#define LAI_Interface__Interface_codeRegion(this) (&(this)->Interface_codeRegion)
#define LAI_Interface_Interface_makeLabel(this) (0, (this)->Interface_makeLabel)
#define LAI_Interface__Interface_makeLabel(this) (&(this)->Interface_makeLabel)
#define LAI_Interface_Interface_findLabel(this) (0, (this)->Interface_findLabel)
#define LAI_Interface__Interface_findLabel(this) (&(this)->Interface_findLabel)
#define LAI_Interface_Interface_makeSymbol(this) (0, (this)->Interface_makeSymbol)
#define LAI_Interface__Interface_makeSymbol(this) (&(this)->Interface_makeSymbol)
#define LAI_Interface_Interface_Symbol_setClasses(this) (0, (this)->Interface_Symbol_setClasses)
#define LAI_Interface__Interface_Symbol_setClasses(this) (&(this)->Interface_Symbol_setClasses)
#define LAI_Interface_Interface_findSymbol(this) (0, (this)->Interface_findSymbol)
#define LAI_Interface__Interface_findSymbol(this) (&(this)->Interface_findSymbol)
#define LAI_Interface_Interface_makeDedicatedTemporary(this) (0, (this)->Interface_makeDedicatedTemporary)
#define LAI_Interface__Interface_makeDedicatedTemporary(this) (&(this)->Interface_makeDedicatedTemporary)
#define LAI_Interface_Interface_makeAssignRegTemporary(this) (0, (this)->Interface_makeAssignRegTemporary)
#define LAI_Interface__Interface_makeAssignRegTemporary(this) (&(this)->Interface_makeAssignRegTemporary)
#define LAI_Interface_Interface_makePseudoRegTemporary(this) (0, (this)->Interface_makePseudoRegTemporary)
#define LAI_Interface__Interface_makePseudoRegTemporary(this) (&(this)->Interface_makePseudoRegTemporary)
#define LAI_Interface_Interface_makeModifierTemporary(this) (0, (this)->Interface_makeModifierTemporary)
#define LAI_Interface__Interface_makeModifierTemporary(this) (&(this)->Interface_makeModifierTemporary)
#define LAI_Interface_Interface_makeAbsoluteTemporary(this) (0, (this)->Interface_makeAbsoluteTemporary)
#define LAI_Interface__Interface_makeAbsoluteTemporary(this) (&(this)->Interface_makeAbsoluteTemporary)
#define LAI_Interface_Interface_makeSymbolTemporary(this) (0, (this)->Interface_makeSymbolTemporary)
#define LAI_Interface__Interface_makeSymbolTemporary(this) (&(this)->Interface_makeSymbolTemporary)
#define LAI_Interface_Interface_makeLabelTemporary(this) (0, (this)->Interface_makeLabelTemporary)
#define LAI_Interface__Interface_makeLabelTemporary(this) (&(this)->Interface_makeLabelTemporary)
#define LAI_Interface_Interface_Temporary_setRematerializable(this) (0, (this)->Interface_Temporary_setRematerializable)
#define LAI_Interface__Interface_Temporary_setRematerializable(this) (&(this)->Interface_Temporary_setRematerializable)
#define LAI_Interface_Interface_Temporary_setHomeable(this) (0, (this)->Interface_Temporary_setHomeable)
#define LAI_Interface__Interface_Temporary_setHomeable(this) (&(this)->Interface_Temporary_setHomeable)
#define LAI_Interface_Interface_Temporary_setDedicated(this) (0, (this)->Interface_Temporary_setDedicated)
#define LAI_Interface__Interface_Temporary_setDedicated(this) (&(this)->Interface_Temporary_setDedicated)
#define LAI_Interface_Interface_Temporary_setGlobal(this) (0, (this)->Interface_Temporary_setGlobal)
#define LAI_Interface__Interface_Temporary_setGlobal(this) (&(this)->Interface_Temporary_setGlobal)
#define LAI_Interface_Interface_findTemporary(this) (0, (this)->Interface_findTemporary)
#define LAI_Interface__Interface_findTemporary(this) (&(this)->Interface_findTemporary)
#define LAI_Interface_Interface_Temporary_identity(this) (0, (this)->Interface_Temporary_identity)
#define LAI_Interface__Interface_Temporary_identity(this) (&(this)->Interface_Temporary_identity)
#define LAI_Interface_Interface_Temporary_isAbsolute(this) (0, (this)->Interface_Temporary_isAbsolute)
#define LAI_Interface__Interface_Temporary_isAbsolute(this) (&(this)->Interface_Temporary_isAbsolute)
#define LAI_Interface_Interface_Temporary_value(this) (0, (this)->Interface_Temporary_value)
#define LAI_Interface__Interface_Temporary_value(this) (&(this)->Interface_Temporary_value)
#define LAI_Interface_Interface_Temporary_isSymbol(this) (0, (this)->Interface_Temporary_isSymbol)
#define LAI_Interface__Interface_Temporary_isSymbol(this) (&(this)->Interface_Temporary_isSymbol)
#define LAI_Interface_Interface_Temporary_symbol(this) (0, (this)->Interface_Temporary_symbol)
#define LAI_Interface__Interface_Temporary_symbol(this) (&(this)->Interface_Temporary_symbol)
#define LAI_Interface_Interface_Temporary_offset(this) (0, (this)->Interface_Temporary_offset)
#define LAI_Interface__Interface_Temporary_offset(this) (&(this)->Interface_Temporary_offset)
#define LAI_Interface_Interface_Temporary_isModifier(this) (0, (this)->Interface_Temporary_isModifier)
#define LAI_Interface__Interface_Temporary_isModifier(this) (&(this)->Interface_Temporary_isModifier)
#define LAI_Interface_Interface_Temporary_modifier(this) (0, (this)->Interface_Temporary_modifier)
#define LAI_Interface__Interface_Temporary_modifier(this) (&(this)->Interface_Temporary_modifier)
#define LAI_Interface_Interface_Temporary_immediate(this) (0, (this)->Interface_Temporary_immediate)
#define LAI_Interface__Interface_Temporary_immediate(this) (&(this)->Interface_Temporary_immediate)
#define LAI_Interface_Interface_Temporary_isPseudoReg(this) (0, (this)->Interface_Temporary_isPseudoReg)
#define LAI_Interface__Interface_Temporary_isPseudoReg(this) (&(this)->Interface_Temporary_isPseudoReg)
#define LAI_Interface_Interface_Temporary_isDedicated(this) (0, (this)->Interface_Temporary_isDedicated)
#define LAI_Interface__Interface_Temporary_isDedicated(this) (&(this)->Interface_Temporary_isDedicated)
#define LAI_Interface_Interface_Temporary_isAssignReg(this) (0, (this)->Interface_Temporary_isAssignReg)
#define LAI_Interface__Interface_Temporary_isAssignReg(this) (&(this)->Interface_Temporary_isAssignReg)
#define LAI_Interface_Interface_Temporary_assigned(this) (0, (this)->Interface_Temporary_assigned)
#define LAI_Interface__Interface_Temporary_assigned(this) (&(this)->Interface_Temporary_assigned)
#define LAI_Interface_Interface_Temporary_regClass(this) (0, (this)->Interface_Temporary_regClass)
#define LAI_Interface__Interface_Temporary_regClass(this) (&(this)->Interface_Temporary_regClass)
#define LAI_Interface_Interface_Symbol_identity(this) (0, (this)->Interface_Symbol_identity)
#define LAI_Interface__Interface_Symbol_identity(this) (&(this)->Interface_Symbol_identity)
#define LAI_Interface_Interface_Symbol_name(this) (0, (this)->Interface_Symbol_name)
#define LAI_Interface__Interface_Symbol_name(this) (&(this)->Interface_Symbol_name)
#define LAI_Interface_Interface_Symbol_isSpill(this) (0, (this)->Interface_Symbol_isSpill)
#define LAI_Interface__Interface_Symbol_isSpill(this) (&(this)->Interface_Symbol_isSpill)
#define LAI_Interface_Interface_Symbol_mtype(this) (0, (this)->Interface_Symbol_mtype)
#define LAI_Interface__Interface_Symbol_mtype(this) (&(this)->Interface_Symbol_mtype)
#define LAI_Interface_Interface_Operation_identity(this) (0, (this)->Interface_Operation_identity)
#define LAI_Interface__Interface_Operation_identity(this) (&(this)->Interface_Operation_identity)
#define LAI_Interface_Interface_Operation_operator(this) (0, (this)->Interface_Operation_operator)
#define LAI_Interface__Interface_Operation_operator(this) (&(this)->Interface_Operation_operator)
#define LAI_Interface_Interface_Operation_iteration(this) (0, (this)->Interface_Operation_iteration)
#define LAI_Interface__Interface_Operation_iteration(this) (&(this)->Interface_Operation_iteration)
#define LAI_Interface_Interface_Operation_issueDate(this) (0, (this)->Interface_Operation_issueDate)
#define LAI_Interface__Interface_Operation_issueDate(this) (&(this)->Interface_Operation_issueDate)
#define LAI_Interface_Interface_Operation_isSpillCode(this) (0, (this)->Interface_Operation_isSpillCode)
#define LAI_Interface__Interface_Operation_isSpillCode(this) (&(this)->Interface_Operation_isSpillCode)
#define LAI_Interface_Interface_Operation_isVolatile(this) (0, (this)->Interface_Operation_isVolatile)
#define LAI_Interface__Interface_Operation_isVolatile(this) (&(this)->Interface_Operation_isVolatile)
#define LAI_Interface_Interface_Label_identity(this) (0, (this)->Interface_Label_identity)
#define LAI_Interface__Interface_Label_identity(this) (&(this)->Interface_Label_identity)
#define LAI_Interface_Interface_Label_name(this) (0, (this)->Interface_Label_name)
#define LAI_Interface__Interface_Label_name(this) (&(this)->Interface_Label_name)
#define LAI_Interface_Interface_BasicBlock_identity(this) (0, (this)->Interface_BasicBlock_identity)
#define LAI_Interface__Interface_BasicBlock_identity(this) (&(this)->Interface_BasicBlock_identity)
#define LAI_Interface_Interface_BasicBlock_unrolled(this) (0, (this)->Interface_BasicBlock_unrolled)
#define LAI_Interface__Interface_BasicBlock_unrolled(this) (&(this)->Interface_BasicBlock_unrolled)
#define LAI_Interface_Interface_BasicBlock_ordering(this) (0, (this)->Interface_BasicBlock_ordering)
#define LAI_Interface__Interface_BasicBlock_ordering(this) (&(this)->Interface_BasicBlock_ordering)
#define LAI_Interface_Interface_LoopInfo_identity(this) (0, (this)->Interface_LoopInfo_identity)
#define LAI_Interface__Interface_LoopInfo_identity(this) (&(this)->Interface_LoopInfo_identity)
#define LAI_Interface_Interface_LoopInfo_unrolled(this) (0, (this)->Interface_LoopInfo_unrolled)
#define LAI_Interface__Interface_LoopInfo_unrolled(this) (&(this)->Interface_LoopInfo_unrolled)
#define LAI_Interface_Interface_makeOperation(this) (0, (this)->Interface_makeOperation)
#define LAI_Interface__Interface_makeOperation(this) (&(this)->Interface_makeOperation)
#define LAI_Interface_Interface_findOperation(this) (0, (this)->Interface_findOperation)
#define LAI_Interface__Interface_findOperation(this) (&(this)->Interface_findOperation)
#define LAI_Interface_Interface_Operation_setVolatile(this) (0, (this)->Interface_Operation_setVolatile)
#define LAI_Interface__Interface_Operation_setVolatile(this) (&(this)->Interface_Operation_setVolatile)
#define LAI_Interface_Interface_Operation_setPrefetch(this) (0, (this)->Interface_Operation_setPrefetch)
#define LAI_Interface__Interface_Operation_setPrefetch(this) (&(this)->Interface_Operation_setPrefetch)
#define LAI_Interface_Interface_Operation_setBarrier(this) (0, (this)->Interface_Operation_setBarrier)
#define LAI_Interface__Interface_Operation_setBarrier(this) (&(this)->Interface_Operation_setBarrier)
#define LAI_Interface_Interface_Operation_setSpillCode(this) (0, (this)->Interface_Operation_setSpillCode)
#define LAI_Interface__Interface_Operation_setSpillCode(this) (&(this)->Interface_Operation_setSpillCode)
#define LAI_Interface_Interface_makeBasicBlock(this) (0, (this)->Interface_makeBasicBlock)
#define LAI_Interface__Interface_makeBasicBlock(this) (&(this)->Interface_makeBasicBlock)
#define LAI_Interface_Interface_findBasicBlock(this) (0, (this)->Interface_findBasicBlock)
#define LAI_Interface__Interface_findBasicBlock(this) (&(this)->Interface_findBasicBlock)
#define LAI_Interface_Interface_moreBasicBlock(this) (0, (this)->Interface_moreBasicBlock)
#define LAI_Interface__Interface_moreBasicBlock(this) (&(this)->Interface_moreBasicBlock)
#define LAI_Interface_Interface_linkBasicBlocks(this) (0, (this)->Interface_linkBasicBlocks)
#define LAI_Interface__Interface_linkBasicBlocks(this) (&(this)->Interface_linkBasicBlocks)
#define LAI_Interface_Interface_makeLoopInfo(this) (0, (this)->Interface_makeLoopInfo)
#define LAI_Interface__Interface_makeLoopInfo(this) (&(this)->Interface_makeLoopInfo)
#define LAI_Interface_Interface_findLoopInfo(this) (0, (this)->Interface_findLoopInfo)
#define LAI_Interface__Interface_findLoopInfo(this) (&(this)->Interface_findLoopInfo)
#define LAI_Interface_Interface_LoopInfo_setDependenceNode(this) (0, (this)->Interface_LoopInfo_setDependenceNode)
#define LAI_Interface__Interface_LoopInfo_setDependenceNode(this) (&(this)->Interface_LoopInfo_setDependenceNode)
#define LAI_Interface_Interface_LoopInfo_setDependenceArc(this) (0, (this)->Interface_LoopInfo_setDependenceArc)
#define LAI_Interface__Interface_LoopInfo_setDependenceArc(this) (&(this)->Interface_LoopInfo_setDependenceArc)
#define LAI_Interface_Interface_setBody(this) (0, (this)->Interface_setBody)
#define LAI_Interface__Interface_setBody(this) (&(this)->Interface_setBody)
#define LAI_Interface_Interface_setEntry(this) (0, (this)->Interface_setEntry)
#define LAI_Interface__Interface_setEntry(this) (&(this)->Interface_setEntry)
#define LAI_Interface_Interface_setExit(this) (0, (this)->Interface_setExit)
#define LAI_Interface__Interface_setExit(this) (&(this)->Interface_setExit)
#define LAI_Interface_Interface_updateCGIR(this) (0, (this)->Interface_updateCGIR)
#define LAI_Interface__Interface_updateCGIR(this) (&(this)->Interface_updateCGIR)
#define LAI_Interface_Interface_open(this) (0, (this)->Interface_open)
#define LAI_Interface__Interface_open(this) (&(this)->Interface_open)
#define LAI_Interface_Interface_optimize(this) (0, (this)->Interface_optimize)
#define LAI_Interface__Interface_optimize(this) (&(this)->Interface_optimize)
#define LAI_Interface_Interface_close(this) (0, (this)->Interface_close)
#define LAI_Interface__Interface_close(this) (&(this)->Interface_close)
/*!
  LAI_getInstance -- The singleton LAI_Interface instance.
*/
LAI_Interface
LAI_getInstance(void);

/*!
  Accessors to the LAI_Interface functions.
  LAI_instance must be an LAI_Interface object.
*/
#define LAI_Interface_Initialize (*LAI_instance->Interface_Initialize)
#define LAI_Interface_Finalize (*LAI_instance->Interface_Finalize)
#define LAI_Interface_getInstance (*LAI_instance->Interface_getInstance)
#define LAI_Interface_codeRegion (*LAI_instance->Interface_codeRegion)
#define LAI_Interface_makeLabel (*LAI_instance->Interface_makeLabel)
#define LAI_Interface_findLabel (*LAI_instance->Interface_findLabel)
#define LAI_Interface_makeSymbol (*LAI_instance->Interface_makeSymbol)
#define LAI_Interface_Symbol_setClasses (*LAI_instance->Interface_Symbol_setClasses)
#define LAI_Interface_findSymbol (*LAI_instance->Interface_findSymbol)
#define LAI_Interface_makeDedicatedTemporary (*LAI_instance->Interface_makeDedicatedTemporary)
#define LAI_Interface_makeAssignRegTemporary (*LAI_instance->Interface_makeAssignRegTemporary)
#define LAI_Interface_makePseudoRegTemporary (*LAI_instance->Interface_makePseudoRegTemporary)
#define LAI_Interface_makeModifierTemporary (*LAI_instance->Interface_makeModifierTemporary)
#define LAI_Interface_makeAbsoluteTemporary (*LAI_instance->Interface_makeAbsoluteTemporary)
#define LAI_Interface_makeSymbolTemporary (*LAI_instance->Interface_makeSymbolTemporary)
#define LAI_Interface_makeLabelTemporary (*LAI_instance->Interface_makeLabelTemporary)
#define LAI_Interface_Temporary_setRematerializable (*LAI_instance->Interface_Temporary_setRematerializable)
#define LAI_Interface_Temporary_setHomeable (*LAI_instance->Interface_Temporary_setHomeable)
#define LAI_Interface_Temporary_setDedicated (*LAI_instance->Interface_Temporary_setDedicated)
#define LAI_Interface_Temporary_setGlobal (*LAI_instance->Interface_Temporary_setGlobal)
#define LAI_Interface_findTemporary (*LAI_instance->Interface_findTemporary)
#define LAI_Interface_Temporary_identity (*LAI_instance->Interface_Temporary_identity)
#define LAI_Interface_Temporary_isAbsolute (*LAI_instance->Interface_Temporary_isAbsolute)
#define LAI_Interface_Temporary_value (*LAI_instance->Interface_Temporary_value)
#define LAI_Interface_Temporary_isSymbol (*LAI_instance->Interface_Temporary_isSymbol)
#define LAI_Interface_Temporary_symbol (*LAI_instance->Interface_Temporary_symbol)
#define LAI_Interface_Temporary_offset (*LAI_instance->Interface_Temporary_offset)
#define LAI_Interface_Temporary_isModifier (*LAI_instance->Interface_Temporary_isModifier)
#define LAI_Interface_Temporary_modifier (*LAI_instance->Interface_Temporary_modifier)
#define LAI_Interface_Temporary_immediate (*LAI_instance->Interface_Temporary_immediate)
#define LAI_Interface_Temporary_isPseudoReg (*LAI_instance->Interface_Temporary_isPseudoReg)
#define LAI_Interface_Temporary_isDedicated (*LAI_instance->Interface_Temporary_isDedicated)
#define LAI_Interface_Temporary_isAssignReg (*LAI_instance->Interface_Temporary_isAssignReg)
#define LAI_Interface_Temporary_assigned (*LAI_instance->Interface_Temporary_assigned)
#define LAI_Interface_Temporary_regClass (*LAI_instance->Interface_Temporary_regClass)
#define LAI_Interface_Symbol_identity (*LAI_instance->Interface_Symbol_identity)
#define LAI_Interface_Symbol_name (*LAI_instance->Interface_Symbol_name)
#define LAI_Interface_Symbol_isSpill (*LAI_instance->Interface_Symbol_isSpill)
#define LAI_Interface_Symbol_mtype (*LAI_instance->Interface_Symbol_mtype)
#define LAI_Interface_Operation_identity (*LAI_instance->Interface_Operation_identity)
#define LAI_Interface_Operation_operator (*LAI_instance->Interface_Operation_operator)
#define LAI_Interface_Operation_iteration (*LAI_instance->Interface_Operation_iteration)
#define LAI_Interface_Operation_issueDate (*LAI_instance->Interface_Operation_issueDate)
#define LAI_Interface_Operation_isSpillCode (*LAI_instance->Interface_Operation_isSpillCode)
#define LAI_Interface_Operation_isVolatile (*LAI_instance->Interface_Operation_isVolatile)
#define LAI_Interface_Label_identity (*LAI_instance->Interface_Label_identity)
#define LAI_Interface_Label_name (*LAI_instance->Interface_Label_name)
#define LAI_Interface_BasicBlock_identity (*LAI_instance->Interface_BasicBlock_identity)
#define LAI_Interface_BasicBlock_unrolled (*LAI_instance->Interface_BasicBlock_unrolled)
#define LAI_Interface_BasicBlock_ordering (*LAI_instance->Interface_BasicBlock_ordering)
#define LAI_Interface_LoopInfo_identity (*LAI_instance->Interface_LoopInfo_identity)
#define LAI_Interface_LoopInfo_unrolled (*LAI_instance->Interface_LoopInfo_unrolled)
#define LAI_Interface_makeOperation (*LAI_instance->Interface_makeOperation)
#define LAI_Interface_findOperation (*LAI_instance->Interface_findOperation)
#define LAI_Interface_Operation_setVolatile (*LAI_instance->Interface_Operation_setVolatile)
#define LAI_Interface_Operation_setPrefetch (*LAI_instance->Interface_Operation_setPrefetch)
#define LAI_Interface_Operation_setBarrier (*LAI_instance->Interface_Operation_setBarrier)
#define LAI_Interface_Operation_setSpillCode (*LAI_instance->Interface_Operation_setSpillCode)
#define LAI_Interface_makeBasicBlock (*LAI_instance->Interface_makeBasicBlock)
#define LAI_Interface_findBasicBlock (*LAI_instance->Interface_findBasicBlock)
#define LAI_Interface_moreBasicBlock (*LAI_instance->Interface_moreBasicBlock)
#define LAI_Interface_linkBasicBlocks (*LAI_instance->Interface_linkBasicBlocks)
#define LAI_Interface_makeLoopInfo (*LAI_instance->Interface_makeLoopInfo)
#define LAI_Interface_findLoopInfo (*LAI_instance->Interface_findLoopInfo)
#define LAI_Interface_LoopInfo_setDependenceNode (*LAI_instance->Interface_LoopInfo_setDependenceNode)
#define LAI_Interface_LoopInfo_setDependenceArc (*LAI_instance->Interface_LoopInfo_setDependenceArc)
#define LAI_Interface_setBody (*LAI_instance->Interface_setBody)
#define LAI_Interface_setEntry (*LAI_instance->Interface_setEntry)
#define LAI_Interface_setExit (*LAI_instance->Interface_setExit)
#define LAI_Interface_updateCGIR (*LAI_instance->Interface_updateCGIR)
#define LAI_Interface_open (*LAI_instance->Interface_open)
#define LAI_Interface_optimize (*LAI_instance->Interface_optimize)
#define LAI_Interface_close (*LAI_instance->Interface_close)


//__LAI_H__
