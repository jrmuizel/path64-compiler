/*
  Copyright (C) 2002-2009, STMicroelectronics, All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.
*/

#include <stdio.h>
#include "W_alloca.h"
#include <stdarg.h>
// [HK]
#if __GNUC__ >=3
#include <vector>
#else
#include <vector.h>
#endif // __GNUC__ >=3

#include "cg.h"
#include "bb.h"
#include "wn.h"
#include "wn_core.h"
#include "wn_util.h"
#include "cg_region.h"
#include "freq.h"
#include "label_util.h"
#include "op.h"
#include "tag.h"
#include "tn_map.h"
#include "bb_map.h"
#include "gtn_universe.h"
#include "gtn_tn_set.h"
#include "gra.h"
#include "gra_live.h"
#include "cg_loop.h"
#include "cgprep.h"
#include "cflow.h"
#include "dominate.h"
#include "findloops.h"
#include "hb.h"
#include "cgtarget.h"
#include "ti_si.h"
#include "cgexp.h"
#include "data_layout.h"
#include "timing.h"
#include "config_TARG.h"

#include "annotations.h"
#include "cg_dep_graph.h"
#include "cg_spill.h"

#include "erglob.h"
#include "tracing.h"

#include "cg_flags.h"

#include "W_dlfcn.h"		    /* for dlsym() */
#include "dso.h"		    /* for load_so() */

#include "lao_stub.h"
extern "C"
{
#define this THIS
#define operator OPERATOR
#define restrict
#include "lao_interface.h"
#undef operator
#undef this
}

/* Where the results of option -ffixed-reg are stored. */
extern std::vector< std::pair< ISA_REGISTER_CLASS, REGISTER> > dont_allocate_these_registers;

/*-------------------------- LAI Interface instance -------*/
static O64_Interface O64_instance;
static Interface interface;

/*------------------------- LAO Region Map -------------*/
BB_MAP CG_LAO_Region_Map;


/*-------------------------- Interface for target dependencies -------*/
#include "lao_cgir.h"

/*--------------------------- lao_init / lao_fini ----------------------------*/
extern "C"
{
void STOP(void);
}

void STOP(void) { }

#ifdef Is_True_On
static void CGIR_print(FILE *file);
// For debug only
extern "C" char *getenv(const char *);
#define GETENV(x) getenv(x)
#else
#define GETENV(x) ((char *)0)
#endif

// The CGIR_CallBack structure and its pointer.
static CGIR_CallBack_ callback_, *callback = &callback_;
// Forward reference to the callback initialization function.
static void LIR_CGIR_callback_init(CGIR_CallBack callback);

// Variable used to skip multiple lao_init / lao_fini calls.
static int lao_initialized = 0;

// LAO so handler
#if defined(__MINGW32__) || defined(__CYGWIN__)
#define SO_EXT ".dll"
#else
#define SO_EXT ".so"
#endif
static void* lao_handler = NULL;

// Initialization of the LAO, needs to be called once per running process.
void
lao_init(void)
{
  if (lao_initialized++ == 0) {
    O64_Interface (*O64_getInstance_p)(void);
    lao_handler = load_so("lao"SO_EXT, CG_Path, 0);
    O64_getInstance_p = (O64_Interface (*)(void))dlsym(lao_handler, "O64_getInstance");
    O64_instance = (*O64_getInstance_p)();
    FmtAssert(O64_Interface_size(O64_instance) == sizeof(O64_Interface_),
             ("Open64 and LAO interfaces are not synchronized."));
    // Initialize LAI Interface
    O64_Interface_Initialize();
    interface = O64_Interface_getInstance();
    // Initialize the LIR->CGIR callback object
    LIR_CGIR_callback_init(callback);
    // Initialize the target dependent LIR<->CGIR interface
    CGIR_LAO_Init();
    // We take the first subset of the subset list which must be the base ISA subset.
    Processor processor = CGIR_IS_to_Processor(*ISA_SUBSET_LIST_Begin(ISA_SUBSET_List));
    // Patch the MaxIssue and MinTaken values.
    int maxIssue = CGTARG_Max_Issue_Width();
    int minTaken = CGTARG_Branch_Taken_Penalty();
    O64_Interface_setMaxIssue(interface, processor, maxIssue);
    O64_Interface_setMinTaken(interface, processor, minTaken);
    // Fixup in case of GP-based addressing.
    if (Gen_GP_Relative) {
      Register gp_register = CGIR_CRP_to_Register(TN_class_reg(GP_TN));
      O64_Interface_setReserved(interface, (Target_ABI)/*FIXME!*/, gp_register);
    }
    // Reserve fixed (non-allocatable) registers given in command-line using -ffixed-reg.
    std::vector< std::pair< ISA_REGISTER_CLASS, REGISTER > >::iterator iter;
    for (iter = dont_allocate_these_registers.begin(); 
         iter != dont_allocate_these_registers.end(); 
         ++iter) {
      CLASS_REG_PAIR fixed_crp;
      Set_CLASS_REG_PAIR(fixed_crp, iter->first, iter->second);
      Register fixed_register = CGIR_CRP_to_Register(fixed_crp);
      O64_Interface_setReserved(interface, (Target_ABI)/*FIXME!*/, fixed_register);
      //fprintf(stderr, "*** reserved: %s\n", REGISTER_name(iter->first, iter->second));
      DevWarn("force non-allocatable register %s", REGISTER_name(iter->first, iter->second));
    }
#if 0
    // Enumerate the allocatable registers.
    ISA_REGISTER_CLASS regclass;
    FOR_ALL_ISA_REGISTER_CLASS(regclass) {
      REGISTER registre;
      REGISTER_SET allocatable = REGISTER_CLASS_allocatable(regclass);
      FOR_ALL_REGISTER_SET_members(allocatable, registre)
      fprintf(stderr, "*** allocatable: %s\n", REGISTER_name(regclass, registre));
    }
#endif
    // Initialize the LAO register latencies with Open64 values.
    for (int op = 0; op < TOP_UNDEFINED; op++) {
      TOP top = (TOP)op;
      if (TOP_is_dummy(top) || TOP_is_var_opnds(top)) continue;
      if (!ISA_SUBSET_LIST_Member(ISA_SUBSET_List, top)) continue;
      Operator o64operator = CGIR_TOP_to_Operator(top);
      if (TSI_Operand_Access_Times_Overridden (top)) {
	int opnds = TOP_fixed_opnds(top);
	for (int opnd = 0; opnd < opnds; opnd++) {
	  int stage = TI_LATENCY_Operand_Access_Cycle(top, opnd);
	  O64_Interface_setArgStage(interface, processor, o64operator, opnd, stage);
	}
      }
      if (TSI_Result_Available_Times_Overridden (top)) {
	int results = TOP_fixed_results(top);
	for (int result = 0; result < results; result++) {
	  int stage = TI_LATENCY_Result_Available_Cycle(top, result);
	  O64_Interface_setResStage(interface, processor, o64operator, result, stage);
	}
      }
    }
  }
}

// Per PU initialization.
void
lao_init_pu(void)
{
  CG_LAO_Region_Map = NULL;
}

// Per PU finalization.
void
lao_fini_pu(void)
{
  if (CG_LAO_Region_Map) {
    BB_MAP_Delete(CG_LAO_Region_Map);
    CG_LAO_Region_Map = NULL;
  }
}

// Finalization of the LAO, needs to be called once.
void
lao_fini(void)
{
  if (--lao_initialized == 0) {
    // Finalize target dependent interface
    CGIR_LAO_Fini();
    // Finalize LAI Interface
    O64_Interface_Finalize();
    interface = NULL;
    O64_instance = NULL;
    close_so(lao_handler);
    lao_handler = NULL;
  }
}

/*-------------------------- CGIR Utility Functions ------------------*/

#define OP_gnu_asm(op)	(OP_code(op) == TOP_asm)

extern OP_MAP OP_Asm_Map;

static BOOL
OP_barrier(OP *op)
{
  if (OP_Is_Barrier(op)) return TRUE;
  return FALSE;
}


/*-------------------------- LAO Utility Functions-------------------------*/

typedef std::list<BB*> BB_List;

// Test if a BB belongs to a BB_List.
static bool
lao_in_bblist(BB_List& bb_list, BB *bb)
{
  //
  BB_List::iterator bb_iter;
  for (bb_iter = bb_list.begin(); bb_iter != bb_list.end(); bb_iter++) {
    if (*bb_iter == bb) return true;
  }
  //
  return false;
}

static void
lao_topsort_DFS(BB *bb, BB_SET *region_set, BB_MAP visited_map, BB_List &bblist)
{
  BBLIST *succs;
  //
  BB_MAP32_Set(visited_map, bb, 1);
  //
  FOR_ALL_BB_SUCCS(bb, succs) {
    BB *succ_bb = BBLIST_item(succs);
    if (!BB_MAP32_Get(visited_map, succ_bb) &&
	BB_SET_MemberP(region_set, succ_bb)) {
      lao_topsort_DFS(succ_bb, region_set, visited_map, bblist);
    }
  }
  //
  bblist.push_front(bb);
}

static void
lao_topsort(BB *entry, BB_SET *region_set, BB_List &bblist)
{
  BB_MAP visited_map = BB_MAP32_Create();
  lao_topsort_DFS(entry, region_set, visited_map, bblist);
  BB_MAP_Delete(visited_map);
}

/*-------------------- CGIR -> LIR Conversion Fonctions ----------------------*/
// These functions are the only ones to call and must only call the 
// Interface_make functions.

// Convert CGIR_LAB to LIR Label.
static inline Label
CGIR_LAB_to_Label(CGIR_LAB cgir_lab)
{
  Label label = O64_Interface_findLabel(interface, cgir_lab);
  if (label == NULL) {
    label = O64_Interface_makeLabel(interface, cgir_lab, LABEL_name(cgir_lab));
  }
  return label;
}

// Convert CGIR_ST_CLASS to LIR SClass
static inline SymbolClass
CGIR_ST_CLASS_to_SClass(ST_CLASS sclass)
{
  static SymbolClass sclasses[CLASS_COUNT] = {
    SymbolClass__UNDEF,	// CLASS_UNK
    SymbolClass_VAR,	// CLASS_VAR
    SymbolClass_FUNC,	// CLASS_FUNC
    SymbolClass_CONST,	// CLASS_CONST
    SymbolClass_PREG,	// CLASS_PREG
    SymbolClass_BLOCK,	// CLASS_BLOCK
    SymbolClass__UNDEF	// CLASS_NAME has no LAI mapping
  };
  return sclasses[sclass];
}

// Convert CGIR_ST_SCLASS to LIR SStore
static inline SymbolStore
CGIR_ST_SCLASS_to_SStore(ST_SCLASS sstore)
{
  static SymbolStore sstores[SCLASS_COUNT] = {
    SymbolStore__UNDEF,		// SCLASS_UNKNOWN
    SymbolStore_AUTO,		// SCLASS_AUTO
    SymbolStore_FORMAL,		// SCLASS_FORMAL
    SymbolStore_FORMAL_REF,		// SCLASS_FORMAL_REF
    SymbolStore_PSTATIC,		// SCLASS_PSTATIC
    SymbolStore_FSTATIC,		// SCLASS_FSTATIC
    SymbolStore_COMMON,		// SCLASS_COMMON
    SymbolStore_EXTERN,		// SCLASS_EXTERN
    SymbolStore_UGLOBAL,		// SCLASS_UGLOBAL
    SymbolStore_DGLOBAL,		// SCLASS_DGLOBAL
    SymbolStore_TEXT,		// SCLASS_TEXT
    SymbolStore_REG,		// SCLASS_REG
    SymbolStore__UNDEF,		// SCLASS_CPLINIT not mapped
    SymbolStore__UNDEF,		// SCLASS_EH_REGION not mapped
    SymbolStore__UNDEF,		// SCLASS_EH_REGION_SUPP not mapped
    SymbolStore__UNDEF,		// SCLASS_DISTR_ARRAY not mapped
    SymbolStore__UNDEF,		// SCLASS_COMMENT not mapped 
    SymbolStore__UNDEF		// SCLASS_THREAD_PRIVATE_FUNCS not mapped
  };
  return sstores[sstore];
}

// Convert CGIR_ST_EXPORT to LIR SExport
static inline SymbolExport
CGIR_ST_EXPORT_to_SExport(ST_EXPORT sexport)
{
  static SymbolExport sexports[EXPORT_COUNT] = {
    SymbolExport_LOCAL,		// EXPORT_LOCAL
    SymbolExport_LOCAL_INTERNAL,	// EXPORT_LOCAL_INTERNAL
    SymbolExport_GLOBAL_INTERNAL,	// EXPORT_INTERNAL
    SymbolExport_GLOBAL_HIDDEN,	// EXPORT_HIDDEN
    SymbolExport_GLOBAL_PROTECTED,	// EXPORT_PROTECTED
    SymbolExport_GLOBAL_PREEMPTIBLE,// EXPORT_PREEMPTIBLE
    SymbolExport__UNDEF		// EXPORT_OPTIONAL not mapped
  };
  return sexports[sexport];
}

// Convert CGIR_SYM to LIR Symbol.
static inline Symbol
CGIR_SYM_to_Symbol(CGIR_SYM cgir_sym)
{
  Symbol symbol = O64_Interface_findSymbol(interface, cgir_sym);
  if (symbol == NULL) {
    if (ST_class(cgir_sym) == CLASS_CONST) {
      char buffer[64];
      sprintf(buffer, "CONST#%llu", (uint64_t)cgir_sym);
      symbol = O64_Interface_makeSymbol(interface, cgir_sym, buffer);
    } else {
      symbol = O64_Interface_makeSymbol(interface, cgir_sym, ST_name(cgir_sym));
    }
    O64_Interface_Symbol_setClasses(interface, symbol, 
	CGIR_ST_CLASS_to_SClass(ST_sym_class(St_Table[cgir_sym])),
	CGIR_ST_SCLASS_to_SStore(ST_storage_class(St_Table[cgir_sym])),
	CGIR_ST_EXPORT_to_SExport(ST_export(St_Table[cgir_sym])));
  }
  return symbol;
}

static inline Temporary CGIR_TN_to_Temporary(CGIR_TN cgir_tn);

// Returns a TN for the rematerializable value
static Temporary
CGIR_TN_REMAT_to_Temporary(CGIR_TN cgir_tn)
{
  WN *remat = TN_remat(cgir_tn);
  Temporary temporary = NULL;
  switch (WN_operator(remat)) {
  case OPR_LDA: {
#ifdef TARG_ST200
    Immediate immediate = CGIR_LC_to_Immediate(LC_xsrc2);	// HACK ALERT
#elif defined(TARG_STxP70)
    Immediate immediate = CGIR_LC_to_Immediate(LC_imm_22);      // HACK ALERT
#elif defined(TARG_ARM)
    Immediate immediate = CGIR_LC_to_Immediate(LC_immed8);      // HACK ALERT
#else
#error TARGET
#endif

    ST *var_st = WN_st(remat);
    ST_IDX st_idx = ST_st_idx(*var_st);
    int64_t offset = WN_lda_offset(remat);
    TN * tn = Gen_Symbol_TN (var_st, offset, 0);
    temporary = CGIR_TN_to_Temporary(tn);
  } break;
  case OPR_INTCONST: {
    if (WN_rtype(remat) == MTYPE_I4 ||
	WN_rtype(remat) == MTYPE_U4) {
      TN *tn = Gen_Literal_TN ((INT32) WN_const_val(remat), 4);
      temporary = CGIR_TN_to_Temporary(tn);
    } else {
      // Currently not handled.
    }
  } break;
  case OPR_CONST: {
    // Currently not handled.
  } break;
  }
  return temporary;
}

// Returns a TN for the homeable value
static Temporary
CGIR_TN_HOME_to_Temporary(CGIR_TN cgir_tn)
{
  //CM 20091019 : CG : You can remove this devwarn as it is a well known todo for the LAO reg allocator. 
  //DevWarn("Should pass TN_is_gra_homeable to LAO for TN%d\n", TN_number(cgir_tn));
  return NULL;
}

// Convert CGIR_TN to LIR Temporary.
static inline Temporary
CGIR_TN_to_Temporary(CGIR_TN cgir_tn)
{
  Temporary temporary = O64_Interface_findTemporary(interface, cgir_tn);
  if (temporary == NULL) {
    if (TN_is_register(cgir_tn)) {
      unsigned width = TN_size(cgir_tn)*8;
      ISA_REGISTER_CLASS tn_irc = TN_register_class(cgir_tn);
      unsigned bit_size = ISA_REGISTER_CLASS_INFO_Bit_Size(ISA_REGISTER_CLASS_Info(tn_irc));
      if (TN_is_dedicated(cgir_tn)) {
	CLASS_REG_PAIR tn_crp = TN_class_reg(cgir_tn);
	// On the open64 side, the dedicated property can be set to a temporary register not
	// in the initial dedicated set. And additional information such as home location
	// may be set on such dedicated registers.
	// On the LAO side, dedicated temporaries are shared, so we create dedicated temporaries
	// only if the cgir_tn is in the initial dedicated set. Otherwise we create an
	// assigned temporary and set the dedicated flag.
	if (Build_Dedicated_TN(CLASS_REG_PAIR_rclass(tn_crp), CLASS_REG_PAIR_reg(tn_crp), 0) == cgir_tn)
	  temporary = O64_Interface_makeDedicatedTemporary(interface, cgir_tn, CGIR_CRP_to_Register(tn_crp));
	else {
	  temporary = O64_Interface_makeAssignedTemporary(interface, cgir_tn, CGIR_CRP_to_Register(tn_crp));
	  O64_Interface_Temporary_setDedicated(interface, temporary);
	}
      } else if (TN_register(cgir_tn) != REGISTER_UNDEFINED) {
	CLASS_REG_PAIR tn_crp = TN_class_reg(cgir_tn);
	temporary = O64_Interface_makeAssignedTemporary(interface, cgir_tn, CGIR_CRP_to_Register(tn_crp));
        if (width != bit_size) O64_Interface_Temporary_setWidth(interface, temporary, width);
      } else {
	temporary = O64_Interface_makeVirtualTemporary(interface, cgir_tn, CGIR_IRC_to_RegFile(tn_irc));
        if (width != bit_size) O64_Interface_Temporary_setWidth(interface, temporary, width);
      }
      // Pass special tn flags
      if (TN_is_rematerializable(cgir_tn)) {
	Temporary remat = CGIR_TN_REMAT_to_Temporary(cgir_tn);
	if (remat != NULL) {
	  O64_Interface_Temporary_setRemater(interface, temporary, remat);
	}
      } else if (TN_is_gra_homeable(cgir_tn)) {
	Temporary home = CGIR_TN_HOME_to_Temporary(cgir_tn);
	if (home != NULL) {
	  O64_Interface_Temporary_setHomeable(interface, temporary, home);
	}
      }
    } else if (TN_is_constant(cgir_tn)) {
      if (TN_has_value(cgir_tn)) {
	int64_t value = TN_value(cgir_tn);
#ifdef TARG_ST200
	Immediate immediate = CGIR_LC_to_Immediate(LC_xsrc2);	// HACK ALERT
#elif defined(TARG_STxP70)
	Immediate immediate = CGIR_LC_to_Immediate(LC_imm_22);      // HACK ALERT
#elif defined(TARG_ARM)
    Immediate immediate = CGIR_LC_to_Immediate(LC_immed8);      // HACK ALERT
#else
#error TARGET
#endif

	temporary = O64_Interface_makeAbsoluteTemporary(interface, cgir_tn, immediate, value);
      } else if (TN_is_symbol(cgir_tn)) {
	Symbol symbol = NULL;
	ST *var_st = TN_var(cgir_tn);
	ST_IDX st_idx = ST_st_idx(*var_st);
	int64_t offset = TN_offset(cgir_tn);
#ifdef TARG_ST200
	Immediate immediate = CGIR_LC_to_Immediate(LC_xsrc2);	// HACK ALERT
#elif defined(TARG_STxP70)
	Immediate immediate = CGIR_LC_to_Immediate(LC_imm_22);      // HACK ALERT
#elif defined(TARG_ARM)
        Immediate immediate = CGIR_LC_to_Immediate(LC_immed8);      // HACK ALERT
#else
#error TARGET
#endif

	symbol = CGIR_SYM_to_Symbol(st_idx);
	temporary = O64_Interface_makeSymbolTemporary(interface, cgir_tn, immediate, symbol, offset);
      } else if (TN_is_label(cgir_tn)) {
	CGIR_LAB cgir_lab = TN_label(cgir_tn);
#ifdef TARG_ST200
	Immediate immediate = CGIR_LC_to_Immediate(LC_xsrc2);	// HACK ALERT
#elif defined(TARG_STxP70)
	Immediate immediate = CGIR_LC_to_Immediate(LC_imm_22);      // HACK ALERT
#elif defined(TARG_ARM)
        Immediate immediate = CGIR_LC_to_Immediate(LC_immed8);      // HACK ALERT
#else
#error TARGET
#endif

	Label label = CGIR_LAB_to_Label(cgir_lab);
	temporary = O64_Interface_makeLabelTemporary(interface, cgir_tn, immediate, label);
	Is_True(TN_offset(cgir_tn) == 0, ("LAO requires zero offset from label."));
      } else if (TN_is_enum(cgir_tn)) {
        //ISA_ENUM_CLASS_VALUE ecv = TN_enum(cgir_tn);
        FmtAssert(0, ("CGIR_TN_to_Temporary: enum tn not implemented"));
        //O64_ModifierMember = CGIR_ECV_to_ModifierMember(ecv);
        //temporary = O64_Interface_makeModifierTemporary(interface, cgir_tn, FIXME);
      } else {
	Is_True(FALSE, ("Unknown constant TN type."));
      }
    } else {
      Is_True(FALSE, ("Unknown TN type."));
    }
  }
  return temporary;
}

static BOOL OP_Safe_Access (CGIR_OP cgir_op)
{
  WN *wn;
  TN *base;

  if (!OP_memory (cgir_op)) return FALSE;

  if (OP_Can_Be_Speculative (cgir_op)) return TRUE;

  if (OP_store (cgir_op)) {

    if ((base = OP_Base(cgir_op))
	&& TN_is_symbol(base)
	&& ! ST_is_weak_symbol(TN_var(base))) return TRUE;

    if ((wn = Get_WN_From_Memory_OP(cgir_op))
	&& Alias_Manager
	&& Safe_to_speculate(Alias_Manager, wn)) return TRUE;
  }

  return FALSE;
}

// Convert CGIR_OP to LIR Operation.
static Operation
CGIR_OP_to_Operation(CGIR_OP cgir_op)
{
  Operation operation = O64_Interface_findOperation(interface, cgir_op);
  if (operation == NULL) {
    // the Operation arguments
    int argCount = OP_opnds(cgir_op);
    Temporary *arguments = (Temporary *)(argCount? alloca(argCount*sizeof(Temporary)): NULL);
    for (int i = 0; i < argCount; i++) arguments[i] = CGIR_TN_to_Temporary(OP_opnd(cgir_op, i));
    // the Operation results
    int resCount = OP_results(cgir_op);
    Temporary *results = (Temporary *)(resCount? alloca(resCount*sizeof(Temporary)): NULL);
    for (int i = 0; i < resCount; i++) results[i] = CGIR_TN_to_Temporary(OP_result(cgir_op, i));
    // the Operation clobbers
    int clobberCount = 0;
    int clobbers[ISA_REGISTER_CLASS_COUNT*ISA_REGISTER_MAX];
    if (OP_call(cgir_op) || OP_gnu_asm(cgir_op)) {
      ISA_REGISTER_CLASS irc;
      FOR_ALL_ISA_REGISTER_CLASS(irc) {
	REGISTER_SET regset;
	if (OP_gnu_asm(cgir_op)) {
	  ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, cgir_op);
	  regset = ASM_OP_clobber_set(asm_info)[irc];
	} else if (OP_call(cgir_op)) {
          BB *bb = OP_bb(cgir_op);
          regset = BB_call_clobbered(bb, irc);
	}
	for (REGISTER reg = REGISTER_SET_Choose(regset);
	     reg != REGISTER_UNDEFINED;
	     reg = REGISTER_SET_Choose_Next(regset, reg)) {
	  TN* cgir_tn = Build_Dedicated_TN(irc, reg, 0);
	  CLASS_REG_PAIR tn_crp = TN_class_reg(cgir_tn);
	  clobbers[clobberCount++] = CGIR_CRP_to_Register(tn_crp);
	}
      }//FOR_ALL_ISA_REGISTER_CLASS
    }
    // make the Operation
    Operator OPERATOR = CGIR_TOP_to_Operator(OP_code(cgir_op));
    int iteration = OP_unrolling(cgir_op);
    operation = O64_Interface_makeOperation(interface, cgir_op, OPERATOR, iteration,
	argCount, arguments, resCount, results, clobberCount, clobbers);
    if (OP_Safe_Access (cgir_op)) {
      O64_Interface_Operation_setSafeAccess(interface, operation);
    }
    if (OP_hoisted(cgir_op)) O64_Interface_Operation_setHoisted(interface, operation);
    if (OP_volatile(cgir_op)) O64_Interface_Operation_setVolatile(interface, operation);
    if (OP_prefetch(cgir_op)) O64_Interface_Operation_setPrefetch(interface, operation);
    if (OP_preload(cgir_op)) O64_Interface_Operation_setPreload(interface, operation);
    if (OP_barrier(cgir_op)) O64_Interface_Operation_setBarrier(interface, operation);
    if (OP_Performance_Effects(cgir_op) && OP_Safe_Effects(cgir_op)) {
      O64_Interface_Operation_setSafePerfs(interface, operation);
    }
    ST *spill_st = CGSPILL_OP_Spill_Location(cgir_op);
    if (spill_st != NULL && OP_spill(cgir_op)) {
      Symbol symbol = CGIR_SYM_to_Symbol(ST_st_idx(*spill_st));
      O64_Interface_Operation_setSpillCode(interface, operation, symbol);
    }
  }
  return operation;
}

// Convert CGIR_BB to LIR BasicBlock.
static BasicBlock
CGIR_BB_to_BasicBlock(CGIR_BB cgir_bb)
{
  BasicBlock basicBlock = O64_Interface_findBasicBlock(interface, cgir_bb);
  if (basicBlock == NULL) {
    // the BasicBlock label(s)
    int labelCount = 0, MAX_LABEL_COUNT = 256;
    Label *labels = (Label *)alloca(MAX_LABEL_COUNT*sizeof(Label));
    if (BB_has_label(cgir_bb)) {
      ANNOTATION *annot;
      for (annot = ANNOT_First(BB_annotations(cgir_bb), ANNOT_LABEL);
	   annot != NULL;
	   annot = ANNOT_Next(annot, ANNOT_LABEL)) {
	Is_True(labelCount < MAX_LABEL_COUNT, ("BB has more than MAX_LABEL_COUNT labels"));
	CGIR_LAB cgir_lab = ANNOT_label(annot);
	labels[labelCount++] = CGIR_LAB_to_Label(cgir_lab);
      }
    }
    // the BasicBlock operations
    int operationCount = 0, MAX_OPERATION_COUNT = 16384;
    Operation *operations = (Operation *)alloca(MAX_OPERATION_COUNT*sizeof(Operation));
    CGIR_OP cgir_op = NULL;
    FOR_ALL_BB_OPs(cgir_bb, cgir_op) {
      Is_True(operationCount < MAX_OPERATION_COUNT,
              ("BB has more than MAX_OPERATION_COUNT operations"));
      operations[operationCount++] = CGIR_OP_to_Operation(cgir_op);
    }
    Processor processor = CGIR_IS_to_Processor(*ISA_SUBSET_LIST_Begin(ISA_SUBSET_List));
    int unrolled = BB_unrollings(cgir_bb);
    intptr_t regionId = (intptr_t)BB_rid(cgir_bb);
    float frequency = BB_freq(cgir_bb);
    // Make the BasicBlock.
    basicBlock = O64_Interface_makeBasicBlock(interface, cgir_bb, processor, unrolled,
	labelCount, labels, operationCount, operations, regionId, frequency);
    // Force the fully unrolled loop bodies to start a new scheduling region,
    // else the memory dependences will not be correct: bug pro-release-1-4-0-B/1.
    if (BB_unrolled_fully(cgir_bb)) {
      if (BB_loophead(cgir_bb))
	O64_Interface_setStart(interface, basicBlock);
      else {
	// Fix bug pro-release-1-5-0-B/1
	ANNOTATION *annot = ANNOT_Get(BB_annotations(cgir_bb), ANNOT_LOOPINFO);
	if (annot) {
	  LOOPINFO *info = ANNOT_loopinfo(annot);
	  if (info)
	    O64_Interface_setStart(interface, basicBlock);
	}
      }
    }
  }
  return basicBlock;
}

// Convert CGIR_LD to LIR LoopScope.
static LoopScope
CGIR_LD_to_LoopScope(CGIR_LD cgir_ld)
{
  LoopScope loopScope = O64_Interface_findLoopScope(interface, cgir_ld);
  if (loopScope == NULL) {
    BB *head_bb = LOOP_DESCR_loophead(cgir_ld);
    int pipelining = CG_LAO_pipelining;
    int renaming = CG_LAO_renaming;
    int boosting = CG_LAO_boosting;
    int preloading = CG_LAO_preloading;
    int l1missextra = CG_LAO_l1missextra;
    ANNOTATION *annot_remainder = ANNOT_Get(BB_annotations(head_bb), ANNOT_REMAINDERINFO);
    if (annot_remainder == NULL) {
      int unroll_times = CG_LOOP_unroll_times_max;
      ANNOTATION *annot_pipeline = NULL;
      ANNOTATION *annot_unroll = NULL;
      ANNOTATION *annot_preload = NULL;
      // Try to access the #pragma pipeline or #pragma unroll arguments if any.
      ANNOTATION *annot_pragma = ANNOT_Get(BB_annotations(head_bb), ANNOT_PRAGMA);
      while (annot_pragma != NULL) {
        WN *wn = ANNOT_pragma(annot_pragma);
	if (WN_pragma(wn) == WN_PRAGMA_PIPELINE) {
          pipelining = WN_pragma_arg1(wn);
          renaming = WN_pragma_arg2(wn);
//fprintf(stderr, "*** PIPELINE(%d,%d)\n", pipelining, renaming);
          annot_pipeline = annot_pragma;
        }
        if (WN_pragma(wn) == WN_PRAGMA_PRELOAD) {
          preloading = WN_pragma_arg1(wn);
          l1missextra = WN_pragma_arg2(wn);
//fprintf(stderr, "*** PRELOAD(%d,%d)\n", preloading, l1missextra);
          annot_preload = annot_pragma;
        }
        if (WN_pragma(wn) == WN_PRAGMA_UNROLL) {
          unroll_times = WN_pragma_arg1(wn);
//fprintf(stderr, "*** UNROLL(%d)\n", unroll_times);
	  annot_unroll = annot_pragma;
        }
	annot_pragma = ANNOT_Get(ANNOT_next(annot_pragma), ANNOT_PRAGMA);
      }
      // If #pragma unroll disable unrolling 
      // and no #pragma pipeline, prevent kernel unrolling.
      if (   annot_unroll != NULL
          && unroll_times <= 1
          && annot_pipeline == NULL) {
        renaming = 1;
      }
    } else pipelining = renaming = 0;
//fprintf(stderr, "*** LOOP_PRELOAD(%d,%d)\n", preloading, l1missextra);    
    BasicBlock head_block = CGIR_BB_to_BasicBlock(head_bb);
    LOOPINFO *cgir_li = LOOP_DESCR_loopinfo(cgir_ld);
    if (cgir_li != NULL) {
      TN *trip_count_tn = LOOPINFO_primary_trip_count_tn(cgir_li);
      Temporary tripcount = trip_count_tn != NULL?
	  CGIR_TN_to_Temporary(trip_count_tn): NULL;
      if (trip_count_tn != NULL && TN_is_constant(trip_count_tn)) {
	uint64_t trip_count = TN_value(trip_count_tn);
	//int8_t min_trip_count = trip_count <= 127? trip_count: 127;
	uint64_t trip_factor = trip_count & -trip_count;
	int8_t min_trip_factor = trip_factor <= 64? trip_factor: 64;
	loopScope = O64_Interface_makeLoopScope(interface,
	    cgir_ld, head_block, tripcount,
	    ConfigureItem_Pipelining, pipelining,
	    ConfigureItem_Renaming, renaming,
	    ConfigureItem_Boosting, boosting,
	    ConfigureItem_PreLoading, preloading,
            ConfigureItem_L1MissExtra, l1missextra,
	    //ConfigureItem_TripMinCount, min_trip_count,
	    ConfigureItem_TripModulus, min_trip_factor,
	    ConfigureItem_TripResidue, 0,
	    ConfigureItem__);
      } else {
	loopScope = O64_Interface_makeLoopScope(interface,
	    cgir_ld, head_block, tripcount,
	    ConfigureItem_Pipelining, pipelining,
	    ConfigureItem_Renaming, renaming,
	    ConfigureItem_Boosting, boosting,
	    ConfigureItem_PreLoading, preloading,
            ConfigureItem_L1MissExtra, l1missextra,
	    ConfigureItem__);
      }
    } else {
      loopScope = O64_Interface_makeLoopScope(interface,
	  cgir_ld, head_block, NULL,
	  ConfigureItem_Pipelining, pipelining,
	  ConfigureItem_Renaming, renaming,
          ConfigureItem_Boosting, boosting,
	  ConfigureItem_PreLoading, preloading,
          ConfigureItem_L1MissExtra, l1missextra,
	  ConfigureItem__);
    }
    // Fill the LoopScope dependence table.
    //
    // Make a BB_List of the loop body and compute its op_count.
    // This BB_List is reordered in topological order.
    int nest_level = BB_nest_level(head_bb), op_count = 0;
    BB_List bb_topo_list, bb_list;
    BB_SET *loop_set = LOOP_DESCR_bbset(cgir_ld);
    lao_topsort(head_bb, loop_set, bb_topo_list);
    BB_List::iterator bb_iter;
    for (bb_iter = bb_topo_list.begin(); bb_iter != bb_topo_list.end(); bb_iter++) {
      if (BB_nest_level(*bb_iter) == nest_level) {
	OP *op = NULL;
	FOR_ALL_BB_OPs(*bb_iter, op) {
	  if (OP_memory(op) || OP_barrier(op)) ++op_count;
	}
	bb_list.push_back(*bb_iter);
	if (op_count >= CG_LAO_opslimit) {
	  DevWarn("LAO_opslimit exceeded (%d memory operations)", op_count);
	  break;
	}
      }
    }
    //
    // Compute the memory dependence graph.
    if (op_count < CG_LAO_opslimit && CG_LAO_aliasing > 0) {
      bool cyclic = BB_innermost(head_bb) != 0;
      CG_DEP_Compute_Region_MEM_Arcs(bb_list, cyclic, false);
      BB_List::iterator bb_iter;
      for (bb_iter = bb_list.begin(); bb_iter != bb_list.end(); bb_iter++) {
	OP *op = NULL;
	FOR_ALL_BB_OPs(*bb_iter, op) {
	  if (_CG_DEP_op_info(op)) {
	    Operation orig_operation = CGIR_OP_to_Operation(op);
	    if (OP_memory(op) || OP_barrier(op)) {
	      WN *wn = Get_WN_From_Memory_OP(op);
	      O64_Interface_LoopScope_setDependenceNode(interface, loopScope,
                                                        orig_operation, wn);
	    }
	    for (ARC_LIST *arcs = OP_succs(op); arcs; arcs = ARC_LIST_rest(arcs)) {
	      ARC *arc = ARC_LIST_first(arcs);
	      CG_DEP_KIND kind = ARC_kind(arc);
	      if (   ARC_is_mem(arc) && kind != CG_DEP_MEMVOL
                  || kind == CG_DEP_MISC || kind == CG_DEP_PREFIN) {
		unsigned type = DependenceKind_Other;
		if (kind == CG_DEP_MEMIN) type = DependenceKind_Flow;
		if (kind == CG_DEP_MEMOUT) type = DependenceKind_Output;
		if (kind == CG_DEP_MEMANTI) type = DependenceKind_Anti;
		if (kind == CG_DEP_MEMREAD) type = DependenceKind_Input;
		if (kind == CG_DEP_SPILLIN) type = DependenceKind_Spill;
		if (kind == CG_DEP_PREFIN) type = DependenceKind_Input;
		if (ARC_is_definite(arc)) type += DependenceKind_Definite;
		int latency = ARC_latency(arc), omega = ARC_omega(arc);
		OP *pred_op = ARC_pred(arc), *succ_op = ARC_succ(arc);
		Is_True(pred_op == op, ("Error in lao_setDependences"));
		Operation dest_operation = CGIR_OP_to_Operation(succ_op);		
		O64_Interface_LoopScope_setDependenceArc(interface, loopScope,
		                                         orig_operation, dest_operation,
                                                         latency, omega,
                                                         (DependenceKind)type);
		//CG_DEP_Trace_Arc(arc, TRUE, FALSE);
	      }
	    }
	  } else {
	    DevWarn("<arc>   CG_DEP INFO is NULL\n");
	  }
	}
      }
      CG_DEP_Delete_Graph(&bb_list);
    }
  }
  return loopScope;
}

/*-------------------- LIR -> CGIR Interface Call-Backs -------------------*/

// Make a CGIR_LAB.
static CGIR_LAB
CGIR_LAB_make(CGIR_LAB cgir_lab, const char *name)
{
  if (cgir_lab == 0) {
    // Create cgir_lab.
    // code borrowed from Gen_Label_For_BB
      // [HK] have to specify the cast for Cygwin gcc-3.3.1
//     LABEL *plabel = &New_LABEL(CURRENT_SYMTAB, cgir_lab);
    LABEL *plabel = &New_LABEL(CURRENT_SYMTAB, (LABEL_IDX&)cgir_lab);
    LABEL_Init(*plabel, Save_Str(name), LKIND_DEFAULT);
  } else {
    // Update cgir_lab.
  }
  //
  return cgir_lab;
}

// Make a CGIR_SYM.
static CGIR_SYM
CGIR_SYM_make(CGIR_SYM cgir_sym, const char *name, bool isSpill, NativeType o64nativeType)
{
  if (cgir_sym == 0) {
    // Create cgir_sym.
    // Currently LAO is allowed to create:
    // - spill symbols
    // Spill symbol.
    if (isSpill) {
      // We use the CGSPILL interface to generate a CGIR spill symbol
      TY_IDX ty = MTYPE_To_TY(NativeType_to_CGIR_TYPE_ID(o64nativeType));
      ST *st = CGSPILL_Gen_Spill_Symbol(ty, name);
      cgir_sym = ST_st_idx(*st);
    }
  } else {
    // Update cgir_sym.
    // Currently LAO is allowed to update:
    // - symbol referenced by a symbol Temporary
    //   This case occurs for generation of homed/rematerialized spill by LAO
    //   where the remat symbol was passed without being allocated yet.
    //   The symbol is not modified, but should be allocated if not.
    if (!Is_Allocated(&St_Table[cgir_sym])) {
      Allocate_Object(&St_Table[cgir_sym]);
    }
  }
  return cgir_sym;
}

// Make a CGIR_TN.
static CGIR_TN
CGIR_TN_make(CGIR_TN cgir_tn, CGIRType cgir_type, ...)
{
  va_list va;
  va_start(va, cgir_type);
  if (cgir_tn == 0) {
    // Create cgir_tn.
    if (cgir_type == CGIRType_Virtual) {
      // Create Virtual cgir_tn.
      RegFile o64refFile = (RegFile)va_arg(va, int/*RegFile*/);
      Register o64register = (Register)va_arg(va, int/*Register*/);
      unsigned width = (unsigned)va_arg(va, unsigned);
      ISA_REGISTER_CLASS irc = RegFile_to_CGIR_IRC(o64refFile);
      if (!width) width = ISA_REGISTER_CLASS_INFO_Bit_Size(ISA_REGISTER_CLASS_Info(irc));
      cgir_tn = Gen_Register_TN(irc, (width + 7)/8);
    } else if (cgir_type == CGIRType_Assigned) {
      // Create Assigned cgir_tn.
      RegFile o64refFile = (RegFile)va_arg(va, int/*RegFile*/);
      Register o64register = (Register)va_arg(va, int/*Register*/);
      unsigned width = (unsigned)va_arg(va, unsigned);
      ISA_REGISTER_CLASS irc = RegFile_to_CGIR_IRC(o64refFile);
      CLASS_REG_PAIR crp = Register_to_CGIR_CRP(o64register);
      if (!width) width = ISA_REGISTER_CLASS_INFO_Bit_Size(ISA_REGISTER_CLASS_Info(irc));
      cgir_tn = Gen_Register_TN(irc, (width + 7)/8);
      Set_TN_register(cgir_tn, CLASS_REG_PAIR_reg(crp));
    } else if (cgir_type == CGIRType_Dedicated) {
      // Create Dedicated cgir_tn.
      RegFile o64refFile = (RegFile)va_arg(va, int/*RegFile*/);
      Register o64register = (Register)va_arg(va, int/*Register*/);
      CLASS_REG_PAIR crp = Register_to_CGIR_CRP(o64register);
      cgir_tn = Build_Dedicated_TN(CLASS_REG_PAIR_rclass(crp), CLASS_REG_PAIR_reg(crp), 0);
    } else if (cgir_type == CGIRType_Absolute) {
      // Create Absolute cgir_tn.
      int64_t value = (int64_t)va_arg(va, int64_t);
      int size = (value >= (int64_t)0x80000000 && value <= (int64_t)0x7FFFFFFF)? 4: 8;
      cgir_tn = Gen_Literal_TN(value, size);
    } else if (cgir_type == CGIRType_Symbol) {
      // Create Symbol cgir_tn.
      CGIR_SYM cgir_sym = (CGIR_SYM)va_arg(va, CGIR_SYM);
      int64_t offset = (int64_t)va_arg(va, int64_t);
      cgir_tn = Gen_Symbol_TN (&St_Table[cgir_sym], offset, 0);
    } else if (cgir_type == CGIRType_Label) {
      // Create Label cgir_tn.
      CGIR_LAB cgir_lab = (CGIR_LAB)va_arg(va, CGIR_LAB);
      cgir_tn = Gen_Label_TN(cgir_lab, 0);
    } else {
      Is_True (0, ("Unexpected CGIRType %d in CGIR_TN_make", cgir_type));
    }
  } else {
    // Update cgir_tn.
    // Currently LAO is only allowed to update:
    // - pseudo temporaries into assigned temporary
    // - value of absolute temporary
    // - label of label temporary
    if (cgir_type == CGIRType_Assigned) {
      // Update Assigned cgir_tn.
      RegFile o64refFile = (RegFile)va_arg(va, int/*RegFile*/);
      Register o64register = (Register)va_arg(va, int/*Register*/);
      Is_True (TN_Is_Allocatable(cgir_tn), ("Invalid TN for register allocation"));
      CLASS_REG_PAIR cgir_crp = Register_to_CGIR_CRP(o64register);
      Set_TN_register(cgir_tn, CLASS_REG_PAIR_reg(cgir_crp));
    } else if (cgir_type == CGIRType_Absolute) {
      int64_t value = (int64_t)va_arg(va, int64_t);
      if (value != TN_value(cgir_tn)) {
	// Create new Literal cgir_tn because value has changed.
	int size = (value >= (int64_t)0x80000000 && value <= (int64_t)0x7FFFFFFF)? 4: 8;
	cgir_tn = Gen_Literal_TN(value, size);
      }
    } else if (cgir_type == CGIRType_Label) {
      CGIR_LAB cgir_lab = (CGIR_LAB)va_arg(va, CGIR_LAB);
      if (cgir_lab != TN_label(cgir_tn)) {
	// Create new Label cgir_tn because cgir_lab has changed.
	cgir_tn = Gen_Label_TN(cgir_lab, 0);
      }
    }
  }
  va_end(va);
  return cgir_tn;
}

// Make a CGIR_OP.
static CGIR_OP
CGIR_OP_make(CGIR_OP cgir_op, Operator o64operator, CGIR_TN arguments[],
             CGIR_TN results[], CGIR_OP orig_op)
{
  TOP top = Operator_to_CGIR_TOP(o64operator);
  int argCount = 0, resCount = 0;
  for (argCount = 0; arguments[argCount] != NULL; argCount++);
  for (resCount = 0; results[resCount] != NULL; resCount++);

  // Special case to convert this pseudo op into a target machine op
  if (top == TOP_COPY) {
    OPS copy_ops = OPS_EMPTY;
    Is_True(argCount == 1 && resCount == 1, ("CGIR_OP: Incorrect COPY pseudo op"));
    Exp_COPY(results[0], arguments[0], &copy_ops);
    Is_True(OPS_length(&copy_ops) == 1, ("CGIR_OP: Pseudo COPY is expanded into more than 1 target instruction"));
    top = OP_code(OPS_last(&copy_ops));
  }

  if (cgir_op == 0) {
    // Create a new cgir_op.
    cgir_op = Mk_VarOP(top, resCount, argCount, results, arguments);
    CGPREP_Init_Op(cgir_op);
  } else {
    Is_True(orig_op == NULL, ("CGIR_OP_make has both cgir_op and orig_op"));
    // Update cgir_op.
    BB *bb = OP_bb(cgir_op);
    if (bb != NULL) BB_Remove_Op(bb, cgir_op);
    if (argCount != OP_opnds(cgir_op) || resCount != OP_results(cgir_op)) {
      // Duplicate cgir_op.
      orig_op = cgir_op;
      cgir_op = Mk_VarOP(top, resCount, argCount, results, arguments);
      CGPREP_Init_Op(cgir_op);
    } else {
      if (OP_code(cgir_op) != top) OP_Change_Opcode(cgir_op, top);
      for (argCount = 0; arguments[argCount] != NULL; argCount++) {
        CGIR_TN cgir_tn = arguments[argCount];
        if (OP_opnd(cgir_op, argCount) != cgir_tn) Set_OP_opnd(cgir_op, argCount, cgir_tn);
      }
      //Is_True(argCount == OP_opnds(cgir_op), ("OP_opnds mismatch in CGIR_update_OP"));
      for (resCount = 0; results[resCount] != NULL; resCount++) {
        CGIR_TN cgir_tn = results[resCount];
        if (OP_result(cgir_op, resCount) != cgir_tn) Set_OP_result(cgir_op, resCount, cgir_tn);
      }
      //Is_True(resCount == OP_results(cgir_op), ("OP_results mismatch in CGIR_update_OP"));
    }
  }
  // Copy information from orig_op if any.
  if (orig_op != NULL) {
    // Copy Annot, flags, srcpos, etc. See Dup_OP().
    OP_srcpos(cgir_op) = OP_srcpos(orig_op);
#if defined (TARG_ST) && defined (SUPPORTS_OP_EFFECTS)
    // (cbr) Support for guards on false
    Copy_Pred (cgir_op, orig_op);
#endif
    OP_variant(cgir_op) = OP_variant(orig_op);
    OP_scycle(cgir_op) = OP_scycle(orig_op);
    OP_flags(cgir_op) = OP_flags(orig_op);
#ifdef TARG_ST
    OP_flags2(cgir_op) = OP_flags2(orig_op);
#endif
    Copy_Asm_OP_Annot(cgir_op, orig_op);
    Set_OP_unroll_bb(cgir_op, OP_unroll_bb(orig_op));
    // _CG_LOOP_info_map may not be defined for multi-bb loops.
    if (Is_CG_LOOP_Op(orig_op)) CG_LOOP_Init_Op(cgir_op);
    // If a duplicate, set orig_idx and copy WN.
    Set_OP_orig_idx(cgir_op, OP_map_idx(orig_op));
    Copy_WN_For_Memory_OP(cgir_op, orig_op);
  }
  //
  return cgir_op;
}

// More of CGIR_OP.
static void
CGIR_OP_more(CGIR_OP cgir_op, int iteration, int issueDate, unsigned flags)
{
  // Set unrolling.
  Set_OP_unrolling(cgir_op, iteration);
  // Set scycle.
  OP_scycle(cgir_op) = issueDate;
  // Set spill information.
  if (flags & CGIROperationFlags_SafeAccess);	//TODO!
  if (flags & CGIROperationFlags_Hoisted) Set_OP_hoisted(cgir_op);
  if (flags & CGIROperationFlags_Volatile) Set_OP_volatile(cgir_op);
  if (flags & CGIROperationFlags_Prefetch);	//TODO!
  if (flags & CGIROperationFlags_Preload) Set_OP_preload(cgir_op);
  if (flags & CGIROperationFlags_Barrier);	//TODO!
  if (flags & CGIROperationFlags_SpillCode) {
    TN *spilled_tn;
    TN *offset_tn;
    TN *base_tn;
    if (OP_store(cgir_op)) {
      int val_idx = TOP_Find_Operand_Use(OP_code(cgir_op),OU_storeval);
      int offset_idx = TOP_Find_Operand_Use(OP_code(cgir_op),OU_offset);
      int base_idx = TOP_Find_Operand_Use(OP_code(cgir_op),OU_base);
      spilled_tn = OP_opnd(cgir_op, val_idx);
      offset_tn = OP_opnd(cgir_op, offset_idx);
      base_tn = OP_opnd(cgir_op, base_idx);
    } else if (OP_load(cgir_op)) {
      int offset_idx = TOP_Find_Operand_Use(OP_code(cgir_op),OU_offset);
      int base_idx = TOP_Find_Operand_Use(OP_code(cgir_op),OU_base);
      spilled_tn = OP_result(cgir_op, 0);
      offset_tn = OP_opnd(cgir_op, offset_idx);
      base_tn = OP_opnd(cgir_op, base_idx);
    } else {
      Is_True(0, ("Invalid LAO isSpilledOP operation"));
    }
    Is_True(base_tn == SP_TN || base_tn == FP_TN, ("Invalid base TN for LAO spill op"));
    Set_OP_spill(cgir_op);
    Set_OP_spilled_tn(cgir_op, spilled_tn);
    Set_TN_spill(spilled_tn, TN_var(offset_tn));
  }
}

// Make a CGIR_BB.
static CGIR_BB
CGIR_BB_make(CGIR_BB cgir_bb, CGIR_LAB labels[], CGIR_OP operations[], CGIR_RID cgir_rid, float frequency)
{
  if (cgir_bb == 0) {
    // Create cgir_bb.
    cgir_bb = Gen_BB();
    // Add the labels.
    for (int labelCount = 0; labels[labelCount] != 0; labelCount++) {
      CGIR_LAB cgir_lab = labels[labelCount];
      // code borrowed from Gen_Label_For_BB
      Set_Label_BB(cgir_lab, cgir_bb);
      BB_Add_Annotation(cgir_bb, ANNOT_LABEL, (void *)cgir_lab);
    }
    // Add the operations.
    OPS ops = OPS_EMPTY;
    for (int opCount = 0; operations[opCount] != NULL; opCount++) {
      OPS_Append_Op(&ops, operations[opCount]);
    }
    BB_Append_Ops(cgir_bb, &ops);
  } else {
    // Update cgir_bb.
    // Add the labels.
    for (int labelCount = 0; labels[labelCount] != 0; labelCount++) {
      CGIR_LAB cgir_lab = labels[labelCount];
      if (!Is_Label_For_BB(cgir_lab, cgir_bb)) {
	// Code borrowed from Gen_Label_For_BB.
	Set_Label_BB(cgir_lab, cgir_bb);
	BB_Add_Annotation(cgir_bb, ANNOT_LABEL, (void *)cgir_lab);
      }
    }
    // Add the operations.
    BB_Remove_All(cgir_bb);
    OPS ops = OPS_EMPTY;
    for (int opCount = 0; operations[opCount] != NULL; opCount++) {
      OPS_Append_Op(&ops, operations[opCount]);
    }
    BB_Append_Ops(cgir_bb, &ops);
    // Remove the LOOPINFO if any.
    if (!BB_unrolled_fully(cgir_bb)) {
      // Fix bug pro-release-1-5-0-B/1
      ANNOTATION *annot = ANNOT_Get(BB_annotations(cgir_bb), ANNOT_LOOPINFO);
      if (annot != NULL) {
	BB_annotations(cgir_bb) = ANNOT_Unlink(BB_annotations(cgir_bb), annot);
      }
    }
  }
  // Set the rid.
  BB_rid(cgir_bb) = cgir_rid;
  // Set the frequency.
  frequency = BB_freq(cgir_bb) = frequency;
  //
  return cgir_bb;
}

// More a CGIR_BB.
static void
CGIR_BB_more(CGIR_BB cgir_bb, CGIR_BB loop_bb, intptr_t traceId, int unrolled, unsigned flags)
{
  // Set the CG_LAO_Region_Map.
  if (CG_LAO_Region_Map != NULL)
    BB_MAP32_Set(CG_LAO_Region_Map, cgir_bb, traceId);
  // Set BB loop_head_bb.
  if (loop_bb != NULL) {
    Set_BB_loop_head_bb(cgir_bb, loop_bb);
  }
  // Set BB unrollings.
  Set_BB_unrollings(cgir_bb, unrolled);
  // Set other flags.
  if (flags & CGIRBasicBlockFlags_Allocated) Set_BB_reg_alloc(cgir_bb);
  if (flags & CGIRBasicBlockFlags_Scheduled) Set_BB_scheduled(cgir_bb);
}

// Make a CGIR_LD.
static CGIR_LD
CGIR_LD_make(CGIR_LD cgir_ld, CGIR_BB head_bb, CGIR_TN trip_count_tn, int unrolled)
{
  if (cgir_ld == 0) {
    // Create cgir_ld.
    // LOOP_DESCR are re-created by LOOP_DESCR_Detect_Loops.
    DevWarn("CGIR_LD_make: LAO loop info not in CGIR");
  } else {
    // Update cgir_ld.
    Is_True(head_bb == LOOP_DESCR_loophead(cgir_ld), ("Broken CGIR_LD in CGIR_LD_make"));
    // We only update the LOOPINFOs for use by LOOP_DESCR_Detect_Loops.
    ANNOTATION *annot = ANNOT_Get(BB_annotations(head_bb), ANNOT_LOOPINFO);
    if (annot != NULL) ANNOT_Unlink(BB_annotations(head_bb), annot);
    LOOPINFO *cgir_li = LOOP_DESCR_loopinfo(cgir_ld);
    if (cgir_li != NULL) {
      LOOPINFO *new_li = TYPE_P_ALLOC(LOOPINFO);
      *new_li = *cgir_li;
      TN *trip_count_tn = LOOPINFO_primary_trip_count_tn(new_li);
      if (trip_count_tn != NULL && TN_is_register(trip_count_tn))
	LOOPINFO_primary_trip_count_tn(new_li) = NULL;
      if (LOOPINFO_wn(cgir_li) != NULL && unrolled > 1) {
	// Code adapted from Unroll_Dowhile_Loop.
	WN *wn = WN_COPY_Tree(LOOPINFO_wn(cgir_li));
	WN_loop_trip_est(wn) /= unrolled;
	WN_loop_trip_est(wn) += 1;
	LOOPINFO_wn(new_li) = wn;
	if (LOOPINFO_kunroll(new_li) == 0)
	  LOOPINFO_kunroll(new_li) = unrolled;
	else
	  LOOPINFO_kunroll(new_li) *= unrolled;
      }
      //      LOOPINFO_srcpos(new_li) = LOOPINFO_srcpos(cgir_li);
      BB_Add_Annotation(head_bb, ANNOT_LOOPINFO, new_li);
    }
  }
  return cgir_ld;
}

// Chain two CGIR_BBs in the CGIR.
static void
CGIR_BB_chain(CGIR_BB cgir_bb, CGIR_BB succ_cgir_bb)
{
  Insert_BB(succ_cgir_bb, cgir_bb);
}

// Unchain a CGIR_BB in the CGIR.
static void
CGIR_BB_unchain(CGIR_BB cgir_bb)
{
  Remove_BB(cgir_bb);
}

// Link two CGIR_BBs in the CGIR with the given branch probability.
static void
CGIR_BB_link(CGIR_BB orig_cgir_bb, CGIR_BB dest_cgir_bb, float probability)
{
  Link_Pred_Succ_with_Prob(orig_cgir_bb, dest_cgir_bb, probability);
  // FdF 20060509: Update the basic block frequency, needed by
  // reg-alloc and post-pass scheduler.
  if (BB_freq(dest_cgir_bb) == 0.0F) {
    BB_freq(dest_cgir_bb) = BB_freq(orig_cgir_bb) * probability;
  }
}

// Unlink all the predecessors and successors of a CGIR_BB in the CGIR.
static void
CGIR_BB_unlink(CGIR_BB cgir_bb, bool preds, bool succs)
{
  BBLIST *edge;
  //
  // Remove successor edges.
  if (succs) {
    FOR_ALL_BB_SUCCS(cgir_bb, edge) {
      BB *succ = BBLIST_item(edge);
      BBlist_Delete_BB(&BB_preds(succ), cgir_bb);
    }
    BBlist_Free(&BB_succs(cgir_bb));
  }
  //
  // Remove predecessor edges.
  if (preds) {
    FOR_ALL_BB_PREDS(cgir_bb, edge) {
      BB *pred = BBLIST_item(edge);
      BBlist_Delete_BB(&BB_succs(pred), cgir_bb);
    }
    BBlist_Free(&BB_preds(cgir_bb));
  }
}

// Discard a CGIR_BB.
static void
CGIR_BB_discard(CGIR_BB cgir_bb)
{
  BB_Remove_All(cgir_bb);
}

// Initialization of the LIR->CGIR callbacks object.
static void
LIR_CGIR_callback_init(CGIR_CallBack callback)
{
  // Initialize the callback pointers.
  *CGIR_CallBack__LAB_make(callback) = CGIR_LAB_make;
  *CGIR_CallBack__SYM_make(callback) = CGIR_SYM_make;
  *CGIR_CallBack__TN_make(callback) = CGIR_TN_make;
  *CGIR_CallBack__OP_make(callback) = CGIR_OP_make;
  *CGIR_CallBack__OP_more(callback) = CGIR_OP_more;
  *CGIR_CallBack__BB_make(callback) = CGIR_BB_make;
  *CGIR_CallBack__BB_more(callback) = CGIR_BB_more;
  *CGIR_CallBack__LD_make(callback) = CGIR_LD_make;
  *CGIR_CallBack__BB_chain(callback) = CGIR_BB_chain;
  *CGIR_CallBack__BB_unchain(callback) = CGIR_BB_unchain;
  *CGIR_CallBack__BB_link(callback) = CGIR_BB_link;
  *CGIR_CallBack__BB_unlink(callback) = CGIR_BB_unlink;
  *CGIR_CallBack__BB_discard(callback) = CGIR_BB_discard;
}


/*----------------------- LAO Optimization Functions -----------------------*/

// Low-level LAO_optimize entry point.
static bool
lao_optimize(BB_List &bodyBBs, BB_List &entryBBs, BB_List &exitBBs, unsigned activation)
{
  //
#ifdef Is_True_On
#if !defined(__MINGW32__) && !defined(__CYGWIN__)
  if (GETENV("O64_PID")) {
    int dummy = -1;
    fprintf(stderr, "O64_PID=%d\n", getpid());
    scanf("%d", &dummy);
  }
#endif
  if (GETENV("CGIR_PRINT")) {
    fprintf(TFile, "*** CGIR Before LAO optimize ***\n");
    CGIR_print(TFile);
  }
#endif
  Start_Timer( T_LAO_Interface_CU );
  // 
  //
  // Get stack model
  int stackModel = 
    Current_PU_Stack_Model == SMODEL_SMALL?   0: 
    Current_PU_Stack_Model == SMODEL_LARGE?   1:
    Current_PU_Stack_Model == SMODEL_DYNAMIC? 2:
                                             -1;
  //
  // Open interface.
  const char *name = ST_name(Get_Current_PU_ST());
//fprintf(stderr, "*** FUNC_PRELOAD(%d,%d)\n", CG_LAO_preloading, CG_LAO_l1missextra);
  O64_Interface_open(interface, name,
      ConfigureItem_Compensation, CG_LAO_compensation,
      ConfigureItem_Speculation, CG_LAO_speculation,
      ConfigureItem_Relaxation, CG_LAO_relaxation,
      ConfigureItem_Pipelining, CG_LAO_pipelining,
      ConfigureItem_Renaming, CG_LAO_renaming,
      ConfigureItem_Boosting, CG_LAO_boosting,
      ConfigureItem_Aliasing, CG_LAO_aliasing,
      ConfigureItem_PreLoading, CG_LAO_preloading,
      ConfigureItem_L1MissExtra, CG_LAO_l1missextra,
      ConfigureItem__);
  //
  // Create the LAO BasicBlocks.
  BB_List::iterator bb_iter;
  // First enter the bodyBBs.
  for (bb_iter = bodyBBs.begin(); bb_iter != bodyBBs.end(); bb_iter++) {
    BasicBlock basicBlock = CGIR_BB_to_BasicBlock(*bb_iter);
    O64_Interface_setBody(interface, basicBlock);
    //fprintf(TFile, "BB_body(%d)\n", BB_id(*bb_iter));
  }
  // Then enter the entryBBs.
  for (bb_iter = entryBBs.begin(); bb_iter != entryBBs.end(); bb_iter++) {
    BasicBlock basicBlock = CGIR_BB_to_BasicBlock(*bb_iter);
    O64_Interface_setEntry(interface, basicBlock);
    //fprintf(TFile, "BB_entry(%d)\n", BB_id(*bb_iter));
  }
  // Last enter the exitBBs.
  for (bb_iter = exitBBs.begin(); bb_iter != exitBBs.end(); bb_iter++) {
    BasicBlock basicBlock = CGIR_BB_to_BasicBlock(*bb_iter);
    O64_Interface_setExit(interface, basicBlock);
    //fprintf(TFile, "BB_exit(%d)\n", BB_id(*bb_iter));
  }
  // Make the control-flow nodes and the control-flow arcs.
  for (bb_iter = bodyBBs.begin(); bb_iter != bodyBBs.end(); bb_iter++) {
    BB *orig_bb = *bb_iter;
    BasicBlock tail_block = CGIR_BB_to_BasicBlock(orig_bb);
    if (lao_in_bblist(exitBBs, orig_bb)) continue;
    BBLIST *bblist = NULL;
    FOR_ALL_BB_SUCCS(orig_bb, bblist) {
      BB *succ_bb = BBLIST_item(bblist);
      float probability = BBLIST_prob(bblist);
      if (probability < 0.0 || probability > 1.0)
	DevWarn("Inconsistent probability %f between BB %d and BB %d",
	    probability, BB_id(orig_bb), BB_id(succ_bb));
      BasicBlock head_block = CGIR_BB_to_BasicBlock(succ_bb);
      O64_Interface_linkBasicBlocks(interface, tail_block, head_block, probability);
    }
  }
  //
  // Make the LoopScopes for the bodyBBs.
  // Get loop headers and function entry that may have a pseudo loop descr
  for (bb_iter = bodyBBs.begin(); bb_iter != bodyBBs.end(); bb_iter++) {
    BB *bb = *bb_iter;
    if (BB_loophead(bb) || BB_entry(bb)) {
      LOOP_DESCR *loop = LOOP_DESCR_Find_Loop(bb);
      if (loop != NULL && LOOP_DESCR_loophead(loop) == bb) {
	LoopScope loopScope = CGIR_LD_to_LoopScope(loop);
      } else {
	// Note, the loophead is not reset by find loops, thus
	// we may have a block having it set while it's no more a loophead.
	if (BB_loophead(bb) && !BB_unrolled_fully(bb)) {
	  DevWarn("Inconsistent loop information for BB %d", BB_id(bb));
	}
      }
    }
  }
  Stop_Timer( T_LAO_Interface_CU );
  //
  if (activation & OptimizeActivation_PrePass) Start_Timer( T_LAO_PRE_CU );
  if (activation & OptimizeActivation_RegAlloc) Start_Timer( T_LAO_REG_CU );
  if (activation & OptimizeActivation_PostPass) Start_Timer( T_LAO_POST_CU );
  //
  unsigned optimizations =
      O64_Interface_optimize(interface,
                             OptimizeItem_Activation, activation,
                             OptimizeItem_Convention, (Target_ABI),	/*FIXME! Use mapping. */
                             //OptimizeItem_StackModel, stackModel,
                             OptimizeItem_RegionType, CG_LAO_regiontype,
                             OptimizeItem_Conversion, CG_LAO_conversion,
                             OptimizeItem_Coalescing, CG_LAO_coalescing,
                             OptimizeItem_Scheduling, CG_LAO_scheduling,
                             OptimizeItem_Allocation, CG_LAO_allocation,
                             OptimizeItem_RCMSSolving, CG_LAO_rcmssolving,
                             OptimizeItem_LogTimeOut, CG_LAO_logtimeout,
                             OptimizeItem_PrePadding, CG_LAO_prepadding,
                             OptimizeItem_PostPadding, CG_LAO_postpadding,
                             OptimizeItem__);
  //
  if (activation & OptimizeActivation_PrePass) Stop_Timer( T_LAO_PRE_CU );
  if (activation & OptimizeActivation_RegAlloc) Stop_Timer( T_LAO_REG_CU );
  if (activation & OptimizeActivation_PostPass) Stop_Timer( T_LAO_POST_CU );
  //
  if (optimizations != 0) {
    Start_Timer( T_LAO_Interface_CU );
    O64_Interface_updateCGIR(interface, callback);
    Stop_Timer( T_LAO_Interface_CU );
#ifdef Is_True_On
    if (GETENV("CGIR_PRINT")) {
      fprintf(TFile, "*** CGIR After LAO optimize ***\n");
      CGIR_print(TFile);
    }
#endif
  }
  //
  // Close interface.
  O64_Interface_close(interface);
  //
  return optimizations != 0;
}

//
// Makes a pesudo loop descr for a single entry region at nesting level 0.
//
static LOOP_DESCR *
make_pseudo_loopdescr(BB *entry, BB_List &bodyBBs, BB_List &exitBBs, MEM_POOL *pool)
{
  LOOP_DESCR *newloop = TYPE_L_ALLOC (LOOP_DESCR);
  BB_SET *loop_set = BB_SET_Create_Empty (PU_BB_Count + 2, pool);
  newloop->mem_pool = pool;
  LOOP_DESCR_bbset(newloop) = loop_set;
  LOOP_DESCR_loophead(newloop) = entry;
  LOOP_DESCR_nestlevel(newloop) = 0;
  LOOP_DESCR_num_exits(newloop) = 0;
  LOOP_DESCR_next(newloop) = NULL;
  //
  // Fill the loop bb set
  BB_List::iterator bb_iter;
  for (bb_iter = bodyBBs.begin(); bb_iter != bodyBBs.end(); bb_iter++) {
    BB_SET_Union1D(loop_set, *bb_iter, NULL);
    if (!BB_MAP_Get(LOOP_DESCR_map, *bb_iter)) {
      // Set the LOOP_DESCR_map to that LOOP_DESCR_Find_Loop will return newloop.
      BB_MAP_Set(LOOP_DESCR_map, *bb_iter, newloop);
    }
  }
  //
  // Count the exitBBs.
  int n_exit = 0;
  for (bb_iter = exitBBs.begin(); bb_iter != exitBBs.end(); bb_iter++) {
    n_exit++;
  }
  LOOP_DESCR_num_exits(newloop) = n_exit;
  return newloop;
}

// Optimize the complete PU through the LAO.
bool
lao_optimize_pu(unsigned activation, bool before_regalloc)
{
  bool result = false;
  MEM_POOL lao_loop_pool;
  MEM_POOL_Initialize(&lao_loop_pool, "LAO loop_descriptors", TRUE);
  //
  MEM_POOL_Push(&lao_loop_pool);
  Calculate_Dominators();
  LOOP_DESCR_Detect_Loops(&lao_loop_pool);
  //
  // List the BBs, entry BBs, exit BBs, body BBs.
  int n_entry = 0, n_exit = 0;
  BB_List entryBBs, exitBBs, bodyBBs;
  for (BB *bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    if (BB_entry(bb)) { entryBBs.push_back(bb); n_entry++; }
    if (BB_exit(bb)) { exitBBs.push_back(bb); n_exit++; }
    bodyBBs.push_back(bb);
  }
  // Create a pseudo loop descr for the function entry if not done
  if (n_entry == 1) {
    BB *entry = *entryBBs.begin();
    LOOP_DESCR *loop = LOOP_DESCR_Find_Loop(entry);
    if (loop == NULL) make_pseudo_loopdescr(entry, bodyBBs, exitBBs, &lao_loop_pool);
  }
  //
  // Create region map for postpass optimizations
  if (activation & OptimizeActivation_PostPass) {
    CG_LAO_Region_Map = BB_MAP32_Create();
  }
  // Call the lower level lao_optimize function.
  result |= lao_optimize(bodyBBs, entryBBs, exitBBs, activation);
  //
  if (result) {
    GRA_LIVE_Recalc_Liveness(NULL);
    GRA_LIVE_Rename_TNs();
#ifdef TARG_ST
    CFLOW_Optimize(CFLOW_MERGE_EMPTY, "CFLOW after LAO", before_regalloc);
#else
    CFLOW_Optimize(CFLOW_MERGE_EMPTY, "CFLOW after LAO");
#endif
  }
  //
  Free_Dominators_Memory();
  MEM_POOL_Pop(&lao_loop_pool);
  //
  MEM_POOL_Delete(&lao_loop_pool);
  return result;
}

#ifdef Is_True_On
/*-------------------------- CGIR Print Functions ----------------------------*/

typedef struct OP_list
{
  OP *op;
  struct OP_list *next;
} OP_list;

static OP_list * OP_list_new(OP_list *head)
{
  OP_list * elem;
  //
  elem = (OP_list *)malloc(sizeof(OP_list));
  elem->next = head;
  head = elem;
  return head;
}

static void
CGIR_TN_print ( const TN *tn, FILE *file )
{
  //
  if (TN_is_constant(tn)) {
    if ( TN_has_value(tn)) {
      fprintf ( file, "(0x%llx)", TN_value(tn) );
      if (TN_size(tn) == 4 && 
	  TN_value(tn) >> 32 != 0 &&
	  TN_value(tn) >> 31 != -1)
	fprintf ( file, "!!! TN_value=0x%llx is too big to fit in a word",
		  TN_value(tn));
    }
    else if (TN_is_enum(tn)) {
      fprintf ( file, "(enum:%s)", ISA_ECV_Name(TN_enum(tn)) );
    }
    else if ( TN_is_label(tn) ) {
      CGIR_LAB lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      INT64 offset = TN_offset(tn);
      BB *targetBB;
      if ( offset == 0 ) {
	fprintf ( file, "(lab:%s)", name );
      }
      else {
	fprintf ( file, "(lab:%s+%lld)", name, offset );
      }
      targetBB = Get_Label_BB(lab); // XXX
      if (targetBB != NULL)
	fprintf(file, " --> %d", BB_id(targetBB));
    } 
    else if ( TN_is_tag(tn) ) {
      CGIR_LAB lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      fprintf ( file, "(tag:%s)", name );
    }
    else if ( TN_is_symbol(tn) ) {
      ST *var = TN_var(tn);
      //
      fprintf ( file, "(sym" );
      fprintf ( file, "%s",TN_RELOCS_Name(TN_relocs(tn)) );
      //
      if (ST_class(var) == CLASS_CONST)
      	fprintf ( file, ":%s)", Targ_Print(NULL, ST_tcon_val(var)));
      else
      	fprintf ( file, ":%s%+lld)", ST_name(var), TN_offset(tn) );
    } 
    else {
      ErrMsg (EC_Unimplemented, "CGIR_TN_print: illegal constant TN");
    }
  }
  else {  /* register TN */
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
	fprintf ( file, "%s", 
		  REGISTER_name(TN_register_class(tn), TN_register(tn)));
      } else {
	fprintf ( file, "(%d,%d)", TN_register_class(tn), TN_register(tn));
      }
    }
    else if (TN_is_global_reg(tn)) {
      fprintf ( file, "G%d", TN_number(tn) );
    }
    else {
      fprintf ( file, "T%d", TN_number(tn) );
    }
    if (TN_is_save_reg(tn)) {
      fprintf ( file, "(sv:%s)", 
		REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
  }
}

static void
CGIR_OP_print ( const OP *op, bool bb_scheduled, FILE *file)
{
  int i;
  //
  //
  {
    LABEL_IDX tag = Get_OP_Tag(op);
    if(tag) fprintf (file, "<tag %s>: ", LABEL_name(tag));
  }
  //
  fprintf(file, "%s", TOP_Name(OP_code(op)));
  //
  if ( OP_variant(op) != 0 ) {
    fprintf ( file, "(%x)", OP_variant(op));
  }
  //
  if (OP_results(op) == 0)
    fprintf(file, " void");
  //
  else for (i = 0; i < OP_results(op); i++) {
    fprintf(file, "%s", (i == 0? " ": ", "));
    CGIR_TN_print(OP_result(op,i), file);
  }
  //
  fprintf(file, " =");
  //
  for (i=0; i<OP_opnds(op); i++) {
    fprintf(file, "%s", (i == 0? " ": ", "));
    TN *tn = OP_opnd(op,i);
    CGIR_TN_print(tn, file);
    if (OP_Defs_TN(op, tn)) fprintf(file, "<def>");
  }
  //if (bb_scheduled)
    fprintf(file, "\tscycle=%d", OP_scycle(op));
  WN *wn = Get_WN_From_Memory_OP(op);
  if (wn) {
    fprintf(file, " wn=0x%x", wn);
    if (Alias_Manager && Safe_to_speculate (Alias_Manager, wn)) {
      fprintf(file, "(safe_to_speculate)");
    }
  }
  // TBD: Print other attributes on operations.
  fprintf(file, " bb=%d unroll_bb=%x unrolling=%d",
          OP_bb(op)? BB_id(OP_bb(op)): 0,
          OP_unroll_bb(op)? BB_id(OP_unroll_bb(op)): 0,
          OP_unrolling(op));
  fprintf(file, " map_idx=%d orig_idx=%d",
          OP_map_idx(op), OP_orig_idx(op));
}

static void
CGIR_OPS_print ( const OPS *ops , bool bb_scheduled, FILE *file)
{
  for (OP *op = OPS_first(ops) ; op; op = OP_next(op)) {
    fprintf(file, "\t");
    CGIR_OP_print(op, bb_scheduled, file);
    fprintf(file, "\t#line[%4d]\n", Srcpos_To_Line(OP_srcpos(op)));
  }
}

static void
CGIR_BB_print_header (BB *bp, FILE *file)
{
  BBLIST *bl;
  INT16 i;
  ANNOTATION *annot = ANNOT_Get(BB_annotations(bp), ANNOT_LOOPINFO);
  BOOL freqs = FREQ_Frequencies_Computed();
  //
  if ( BB_entry(bp) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bp), ANNOT_ENTRYINFO);
    ENTRYINFO *ent = ANNOT_entryinfo (ant);
    OP *sp_adj = BB_entry_sp_adj_op(bp);
    Is_True ((sp_adj == ENTRYINFO_sp_adj(ent)),("bad sp_adj"));
    //
    fprintf ( file, "Entrypoint: %s\t Starting Line %d\n",
	      ST_name(ENTRYINFO_name(ent)),
	      Srcpos_To_Line(ENTRYINFO_srcpos(ent)));
    //
    if (sp_adj) {
      OP *op;
      BOOL found_sp_adj = FALSE;
      fprintf ( file, "SP entry adj: " );
      Print_OP_No_SrcLine (sp_adj);
      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( file, "******** ERROR ******** sp adjust not found in entry block\n");
    }
  }
  //
  if ( BB_exit(bp) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bp), ANNOT_EXITINFO);
    EXITINFO *exit = ANNOT_exitinfo (ant);
    OP *sp_adj = BB_exit_sp_adj_op(bp);
    Is_True ((sp_adj == EXITINFO_sp_adj(exit)),("bad sp_adj"));
    //
    if (sp_adj) {
      OP *op;
      BOOL found_sp_adj = FALSE;
      fprintf ( file, "SP exit adj: " );
      Print_OP_No_SrcLine (sp_adj);
      //
      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( file, "******** ERROR ******** sp adjust not found in exit block\n");
    }
  }
  //
  fprintf ( file, "    BB %d, flags 0x%04x",
	    BB_id(bp), BB_flag(bp) );
  //
  if (freqs || BB_freq_fb_based(bp))
    fprintf(file, ", freq %g (%s)", BB_freq(bp),
	    BB_freq_fb_based(bp)? "feedback": "heuristic");
  
  if (BB_scheduled(bp)) fprintf ( file, ", Scheduled");
  if (BB_reg_alloc(bp)) fprintf ( file, ", Allocated");
  if (BB_unreachable(bp)) fprintf ( file, ", Unreachable");
  if (BB_entry(bp))	fprintf ( file, ", Entry" );
  if (BB_handler(bp))	fprintf ( file, ", Handler" );
  if (BB_asm(bp)) 	fprintf ( file, ", Asm" );
  //
  if (BB_exit(bp)) {
    if (BB_call(bp))	fprintf ( file, ", Tail-call" );
    else		fprintf ( file, ", Exit" );
  } else if (BB_call(bp)) fprintf ( file, ", Call" );
  //
  if (BB_rid(bp)) {
    INT exits;
    RID *rid = BB_rid(bp);
    CGRIN *cgrin = RID_cginfo(rid);
    if (cgrin) {
      if (bp == CGRIN_entry(cgrin)) {
	fprintf ( file, ", Region-entry " );
      }
      exits = RID_num_exits(rid);
      for (i = 0; i < exits; ++i) {
	if (bp == CGRIN_exit_i(cgrin, i)) {
	  fprintf ( file, ", Region-exit[%d]", i );
	}
      }
    }
  }
  //
  fprintf ( file, "\n");
  //
  if (annot)
    Print_LOOPINFO(ANNOT_loopinfo(annot));
  //
  if (BB_loop_head_bb(bp)) {
    if (BB_loophead(bp)) {
      if (!annot) {
	fprintf(file, "\tHead of loop body line %d\n", BB_Loop_Lineno(bp));
      }
    } else {
      BB *head = BB_loop_head_bb(bp);
      fprintf(file,
	      "\tPart of loop body starting at line %d with head BB:%d\n",
	      BB_Loop_Lineno(head), BB_id(head));
    }
  }
  //
  if (BB_unrollings(bp) > 1)
    fprintf(file, "\tUnrolled %d times%s\n", BB_unrollings(bp),
	    BB_unrolled_fully(bp)? " (fully)": "");
  //
  if ( BB_rid(bp) )
    RID_Fprint( file, BB_rid(bp) );
  //
  fprintf ( file, "\tpred" );
  FOR_ALL_BB_PREDS (bp, bl) {
    fprintf ( file, " %d", BB_id(BBLIST_item(bl)));
  }
  //
  fprintf ( file, "\n\tsucc%s", freqs? " (w/probs)": "" );
  FOR_ALL_BB_SUCCS (bp, bl) {
    fprintf ( file, " %d",
	      BB_id(BBLIST_item(bl)));
    if (freqs) fprintf(file, "(%g)", BBLIST_prob(bl));
  }
  fprintf ( file, "\n" );
  //
  if (BB_has_label(bp)) {
    ANNOTATION *ant;
    fprintf(file, "\tLabel");
    for (ant = ANNOT_First(BB_annotations(bp), ANNOT_LABEL);
	 ant != NULL;
	 ant = ANNOT_Next(ant, ANNOT_LABEL))
      {
	INT eh_labs = 0;
	LABEL_IDX lab = ANNOT_label(ant);
	fprintf (file," %s", LABEL_name(lab));
	FmtAssert((Get_Label_BB(lab) == bp),
		  (" Inconsistent ST for BB:%2d label", BB_id(bp)));
	switch (LABEL_kind(Label_Table[lab])) {
	case LKIND_BEGIN_EH_RANGE:
	  fprintf (file,"%cbegin_eh_range", eh_labs++? ' ': '(');
	  break;
	case LKIND_END_EH_RANGE:
	  fprintf (file,"%cend_eh_range", eh_labs++? ' ': '(');
	  break;
	}
	if (eh_labs)
	  fprintf (file,")");
      }
    fprintf(file, "\n");
  }
  //
  return;
}

static void
CGIR_BB_print(BB *bp, FILE *file)
{
  CGIR_BB_print_header (bp, file);
  if (BB_first_op(bp))	CGIR_OPS_print (&bp->ops, BB_scheduled(bp), file);
}

static void
CGIR_Alias_print(FILE *file)
{
  OP_list *memops = NULL, *elt1, *elt2;
  BB *bp;
  OP *op;
  BOOL alias, identical;
  //
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    for (op = BB_first_op(bp); op; op = OP_next(op)) {
      if (OP_memory(op)) {
	memops = OP_list_new(memops);
	memops->op = op;
      }
    }
  }
  //
  fprintf(file, "--------------- Begin Print Alias ---------------\n");
  //
  for (elt1 = memops; elt1; elt1 = elt1->next) {
    fprintf(file, "<Alias>"); CGIR_OP_print(elt1->op, FALSE, file); fprintf(file, "\n");
    for (elt2 = memops; elt2 != elt1; elt2 = elt2->next) {
      fprintf(file, "\t<with>"); CGIR_OP_print(elt2->op, FALSE, file); fprintf(file, "\t");
      alias = CG_DEP_Mem_Ops_Alias(elt1->op, elt2->op, &identical);
      if (!alias)          fprintf(file, "NO-ALIAS");
      else if (!identical) fprintf(file, "   ALIAS");
      else                 fprintf(file, "IDENTICAL");
      fprintf(file, "</with>\n");
    }
    fprintf(file, "</Alias>\n");
  }
  //
  fprintf(file, "---------------- End Print Alias ----------------\n");
}

static void
CGIR_print(FILE *file)
{
  BB *bp;
  //
  fprintf(file, "--------CFG Begin--------\n");
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    CGIR_BB_print ( bp, file );
    fprintf ( file,"\n" );
  }
  //
  fprintf(file, "-------- CFG End --------\n");
  //CGIR_Alias_print(file);
}

#endif // Is_True_On
