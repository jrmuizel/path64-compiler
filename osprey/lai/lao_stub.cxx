#include <stdio.h>
#include <vector.h>

#include "bb.h"
#include "cg_region.h"
#include "freq.h"
#include "label_util.h"
#include "op.h"
#include "tag.h"
#include "tn_map.h"
#include "gtn_universe.h"
#include "gtn_tn_set.h"

#include "cg_dep_graph.h"

#include "erglob.h"
#include "tracing.h"

#include "lao_stub.h"

extern "C" {
#define this THIS
#define operator OPERATOR
#define __STDBOOL_H__
#include "CCL.h"
#include "CSD.h"
#include "LAO_Interface.h"
#undef operator
#undef this
}

typedef list<BB*> BB_List;

// Test if a BB belongs to a BB_List.
static bool
CGIR_inBB_List(BB_List& bb_list, BB *bb) {
  //
  BB_List::iterator bb_iter;
  for (bb_iter = bb_list.begin(); bb_iter != bb_list.end(); bb_iter++) {
    if (*bb_iter == bb) return true;
  }
  //
  return false;
}

// Map CGIR TOP to LIR Operator.
static Operator TOP__Operator[TOP_UNDEFINED];

// Map CGIR ISA_ENUM_CLASS to LIR Modifier.
static Modifier IEC__Modifier[EC_MAX];

// Map CGIR Literal to LIR Immediate.
static Immediate LC__Immediate[LC_MAX];

// Map CGIR ISA_REGISTER_CLASS to LIR RegClass.
// WARNING! ISA_REGISTER_CLASS reaches ISA_REGISTER_CLASS_MAX
static RegClass IRC__RegClass[ISA_REGISTER_CLASS_MAX+1];

// Variable used to skip multiple lao_init / lao_fini calls.
static int lao_initialized = 0;

extern "C" {
#include <unistd.h>
}

typedef vector<BB*> BB_VECTOR;

static void CGIR_print();

/*-------------------- CGIR -> LIR Conversion Fonctions ----------------------*/

// Convert CGIR TOP to LIR Operator.
static inline Operator
CGIR_TOP_to_Operator(TOP top) {
  Operator lao_operator = TOP__Operator[top];
  Is_True(top >= 0 && top < TOP_UNDEFINED, ("TOPcode out of range"));
  Is_True(lao_operator != Operator_, ("Cannot map TOPcode to Operator"));
  return lao_operator;
}

// Convert CGIR ISA_ENUM_CLASS to LIR Modifier.
static inline Modifier
CGIR_IEC_to_Modifier(ISA_ENUM_CLASS iec) {
  Modifier lao_modifier = IEC__Modifier[iec];
  Is_True(iec >= 0 && iec < EC_MAX, ("ISA_ENUM_CLASS out of range"));
  Is_True(lao_modifier != Modifier_, ("Cannot map ISA_ENUM_CLASS to Modifier"));
  return lao_modifier;
}

// Convert CGIR ISA_LIT_CLASS to LIR Immediate.
static inline Immediate
CGIR_LC_to_Immediate(ISA_LIT_CLASS ilc) {
  Immediate lao_immediate = LC__Immediate[ilc];
  Is_True(ilc >= 0 && ilc < LC_MAX, ("ISA_LIT_CLASS out of range"));
  Is_True(lao_immediate != Immediate_, ("Cannot map ISA_LIT_CLASS to Immediate"));
  return lao_immediate;
}

// Convert CGIR ISA_REGISTER_CLASS to LIR RegClass.
static inline RegClass
CGIR_IRC_to_RegClass(ISA_REGISTER_CLASS irc) {
  RegClass lao_regclass = IRC__RegClass[irc];
  Is_True(irc >= 0 && irc <= ISA_REGISTER_CLASS_MAX, ("ISA_REGISTER_CLASS out of range"));
  Is_True(lao_regclass != RegClass_, ("Cannot map ISA_REGISTER_CLASS to RegClass"));
  return lao_regclass;
}

// Convert CGIR CLASS_REG_PAIR to LIR Register.
static inline Register
CGIR_CRP_to_Register(CLASS_REG_PAIR crp) {
  mREGISTER reg = CLASS_REG_PAIR_reg(crp);
  ISA_REGISTER_CLASS irc = CLASS_REG_PAIR_rclass(crp);
  RegClass regclass = CGIR_IRC_to_RegClass(irc);
  Register lowreg = RegClass_getLowReg(regclass);
  return (Register)(lowreg + (reg - 1));
}

// Convert CGIR_LAB to LIR Label.
static inline Label
CGIR_LAB_to_Label(CGIR_LAB cgir_lab) {
  Label label = Interface_searchLabel(interface, cgir_lab);
  if (label == NULL) {
    label = Interface_createLabel(interface, cgir_lab, LABEL_name(cgir_lab));
  }
  return label;
}

// Convert CGIR_SYM to LIR Symbol.
static inline Symbol
CGIR_SYM_to_Symbol(CGIR_SYM cgir_sym) {
  Symbol symbol = Interface_searchSymbol(interface, cgir_sym);
  if (symbol == NULL) {
    if (ST_class(cgir_sym) == CLASS_CONST) {
      char buffer[64];
      sprintf(buffer, "CONST#%llu", (uint64_t)cgir_sym);
      symbol = Interface_createSymbol(interface, cgir_sym, String_S(buffer));
    } else {
      symbol = Interface_createSymbol(interface, cgir_sym, ST_name(cgir_sym));
    }
  }
  return symbol;
}

// Convert CGIR_TN to LIR TempName.
static inline TempName
CGIR_TN_to_TempName(CGIR_TN cgir_tn) {
  TempName tempname = Interface_searchTempName(interface, cgir_tn);
  if (tempname == NULL) {
    if (TN_is_register(cgir_tn)) {
      if (TN_is_dedicated(cgir_tn)) {
	CLASS_REG_PAIR tn_crp = TN_class_reg(cgir_tn);
	tempname = Interface_createDedicatedTempName(interface, cgir_tn, CGIR_CRP_to_Register(tn_crp));
      } else {
	ISA_REGISTER_CLASS tn_irc = TN_register_class(cgir_tn);
	tempname = Interface_createPseudoRegTempName(interface, cgir_tn, CGIR_IRC_to_RegClass(tn_irc));
      }
    } else if (TN_is_constant(cgir_tn)) {
      if (TN_has_value(cgir_tn)) {
	int64_t value = TN_value(cgir_tn);
	Immediate immediate = CGIR_LC_to_Immediate((ISA_LIT_CLASS)0); // HACK ALERT
	tempname = Interface_createAbsoluteTempName(interface, cgir_tn, immediate, value);
      } else if (TN_is_symbol(cgir_tn)) {
	Symbol symbol = NULL;
	ST *var = TN_var(cgir_tn);
	ST_IDX st_idx = ST_st_idx(*var);
	int64_t offset = TN_offset(cgir_tn);
	Immediate immediate = CGIR_LC_to_Immediate((ISA_LIT_CLASS)0); // HACK ALERT
	symbol = CGIR_SYM_to_Symbol(st_idx);
	tempname = Interface_createSymbolTempName(interface, cgir_tn, immediate, symbol, offset);
      } else if (TN_is_label(cgir_tn)) {
	CGIR_LAB cgir_lab = TN_label(cgir_tn);
	Immediate immediate = CGIR_LC_to_Immediate((ISA_LIT_CLASS)0); // HACK ALERT
	Label label = CGIR_LAB_to_Label(cgir_lab);
	tempname = Interface_createLabelTempName(interface, cgir_tn, immediate, label);
	Is_True(TN_offset(cgir_tn) == 0, ("LAO requires zero offset from label."));
      } else if (TN_is_enum(cgir_tn)) {
	ISA_ENUM_CLASS_VALUE value = TN_enum(cgir_tn);
	Modifier modifier = CGIR_IEC_to_Modifier((ISA_ENUM_CLASS)0);	// HACK ALERT
	tempname = Interface_createModifierTempName(interface, cgir_tn, modifier, value);
      } else {
	Is_True(FALSE, ("Unknown constant TN type."));
      }
    } else {
      Is_True(FALSE, ("Unknown TN type."));
    }
    Is_True(tempname != NULL, ("tempname should not be NULL."));
  }
  return tempname;
}

// Convert CGIR_OP to LIR Operation.
static Operation
CGIR_OP_to_Operation(CGIR_OP cgir_op) {
  Operation operation = Interface_searchOperation(interface, cgir_op);
  if (operation == NULL) {
    int argCount = OP_opnds(cgir_op);
    int resCount = OP_results(cgir_op);
    Operator OPERATOR = CGIR_TOP_to_Operator(OP_code(cgir_op));
    operation = Interface_createOperation(interface, cgir_op, OPERATOR, argCount, resCount);
    for (int i = 0; i < argCount; i++) {
      TempName tempname = CGIR_TN_to_TempName(OP_opnd(cgir_op, i));
      Interface_Operation_appendArgument(interface, operation, tempname);
    }
    for (int i = 0; i < resCount; i++) {
      TempName tempname = CGIR_TN_to_TempName(OP_result(cgir_op, i));
      Interface_Operation_appendResult(interface, operation, tempname);
    }
    if (OP_volatile(cgir_op)) {
      // FIXME Interface_Operation_setVolatile(interface, operation);
    }
  }
  // TODO: MemInfo
  return operation;
}

// Convert CGIR_BB to LIR BasicBlock.
static BasicBlock
CGIR_BB_to_BasicBlock(CGIR_BB cgir_bb) {
  BasicBlock basicblock = Interface_searchBasicBlock(interface, cgir_bb);
  if (basicblock == NULL) {
    basicblock = Interface_createBasicBlock(interface, cgir_bb, BB_freq(cgir_bb));
    // the BasicBlock label(s)
    if (BB_has_label(cgir_bb)) {
      ANNOTATION *annot;
      for (annot = ANNOT_First(BB_annotations(cgir_bb), ANNOT_LABEL);
	   annot != NULL;
	   annot = ANNOT_Next(annot, ANNOT_LABEL)) {
	CGIR_LAB cgir_lab = ANNOT_label(annot);
	Label label = CGIR_LAB_to_Label(cgir_lab);
	Interface_BasicBlock_appendLabel(interface, basicblock, label);
      }
    }
    // the BasicBlock operations
    CGIR_OP cgir_op = NULL;
    FOR_ALL_BB_OPs(cgir_bb, cgir_op) {
      Operation operation = CGIR_OP_to_Operation(cgir_op);
      Interface_BasicBlock_appendOperation(interface, basicblock, operation);
    }
  }
  // TODO: LoopInfo
  return basicblock;
}

// Convert LIR RegClass to CGIR ISA_REGISTER_CLASS.
static inline ISA_REGISTER_CLASS
RegClass_to_CGIR_IRC(RegClass regclass) {
  for (int i = ISA_REGISTER_CLASS_MIN; i <= ISA_REGISTER_CLASS_MAX; i++)
    if (IRC__RegClass[i] == regclass) return (ISA_REGISTER_CLASS)i;
  return ISA_REGISTER_CLASS_UNDEFINED;
}

// Convert LIR Register to CGIR CLASS_REG_PAIR.
static inline CLASS_REG_PAIR
Register_to_CGIR_CRP(Register registre) {
  RegClass regclass = Register_getRegClass(registre);
  Register lowreg = RegClass_getLowReg(regclass);
  ISA_REGISTER_CLASS irc = RegClass_to_CGIR_IRC(regclass);
  REGISTER reg = (registre - lowreg) + 1;
  CLASS_REG_PAIR crp;
  Set_CLASS_REG_PAIR(crp, irc, reg);
  return crp;
}

// Convert LIR Operator to TOP.
static TOP
Operator_to_CGIR_TOP(Operator lir_operator) {
  TOP top = TOP_UNDEFINED;
  for (int i = 0; i < TOP_UNDEFINED; i++) {
    if (TOP__Operator[i] == lir_operator) {
      top = (TOP)i;
      break;
    }
  }
  return top;
}

/*-------------------- LIR Interface Call-Back Functions ---------------------*/

// Create a CGIR_LAB from a LIR Label.
static CGIR_LAB
CGIR_LAB_create(Label label) {
  CGIR_LAB cgir_lab = 0;
  String name = Interface_Label_getName(interface, label);
  // code borrowed from Gen_Label_For_BB
  LABEL *plabel = &New_LABEL(CURRENT_SYMTAB, cgir_lab);
  LABEL_Init(*plabel, Save_Str(name), LKIND_DEFAULT);
}

// Update a CGIR_LAB from a LIR Label.
static void
CGIR_LAB_update(CGIR_LAB cgir_lab, Label label) {
}

// Create a CGIR_SYM from a LIR Symbol.
static CGIR_SYM
CGIR_SYM_create(Symbol symbol) {
}

// Update a CGIR_SYM from a LIR Symbol.
static void
CGIR_SYM_update(CGIR_SYM cgir_sym, Symbol symbol) {
}

// Create a CGIR_TN from a LIR Dedicated TempName.
static CGIR_TN
CGIR_TN_Dedicated_create(Register registre) {
  int size = 0;		// not used in Build_Dedicated_TN
  CLASS_REG_PAIR crp = Register_to_CGIR_CRP(registre);
  return Build_Dedicated_TN(CLASS_REG_PAIR_rclass(crp), CLASS_REG_PAIR_reg(crp), size);
}

// Create a CGIR_TN from a LIR PseudoReg TempName.
static CGIR_TN
CGIR_TN_PseudoReg_create(RegClass regclass) {
  int size = 0;		// FIXME
  return Gen_Register_TN(RegClass_to_CGIR_IRC(regclass), size);
}

// Create a CGIR_TN from a LIR Modifier TempName.
static CGIR_TN
CGIR_TN_Modifier_create(Modifier modifier) {
  Is_True(0, ("CGIR_createModifier_TN not implemented"));
  return NULL;
}

// Create a CGIR_TN from a LIR Absolute TempName.
static CGIR_TN
CGIR_TN_Absolute_create(Immediate immediate, int64_t value) {
  int size = (value >= (int64_t)0x80000000 && value <= (int64_t)0x7FFFFFFF) ? 4 : 8;
  return Gen_Literal_TN(value, size);
}

// Create a CGIR_TN from a LIR Symbol TempName.
static CGIR_TN
CGIR_TN_Symbol_create(Immediate immediate, Symbol symbol, int64_t offset) {
  Is_True(0, ("CGIR_createSymbol_TN not implemented"));
  return NULL;
}

// Create a CGIR_TN from a LIR Label TempName.
static CGIR_TN
CGIR_TN_Label_create(Immediate immediate, Label label) {
  CGIR_LAB cgir_lab = Interface_lookupLAB(interface, label);
  Is_True(cgir_lab != 0, ("Interface_lookupLAB returned zero LABEL_IDX"));
  return Gen_Label_TN(cgir_lab, 0);
}

// Update a CGIR_TN from a LIR TempName.
static void
CGIR_TN_update(CGIR_TN cgir_tn, TempName tempname) {
}

// Create a CGIR_OP from a LIR Operation.
static CGIR_OP
CGIR_OP_create(Operation operation, int argCount, TempName arguments[], int resCount, TempName results[]) {
  TOP top = Operator_to_CGIR_TOP(Interface_Operation_getOperator(interface, operation));
  CGIR_TN *argTNs = (CGIR_TN *)alloca(sizeof(CGIR_TN)*argCount);
  for (int i = 0; i < argCount; i++) {
    argTNs[i] = Interface_lookupTN(interface, arguments[i]);
  }
  CGIR_TN *resTNs = (CGIR_TN *)alloca(sizeof(CGIR_TN)*resCount);
  for (int i = 0; i < resCount; i++) {
    resTNs[i] = Interface_lookupTN(interface, results[i]);
  }
  CGIR_OP cgir_op = Mk_VarOP(top, resCount, argCount, resTNs, argTNs);
  // TODO: issue dates, flags, etc.
  return cgir_op;
}

// Update a CGIR_OP from a LIR Operation.
static void
CGIR_OP_update(CGIR_OP cgir_op, Operation operation, int argCount, TempName arguments[], int resCount, TempName results[]) {
  TOP top = Operator_to_CGIR_TOP(Interface_Operation_getOperator(interface, operation));
  if (OP_code(cgir_op) != top) {
    OP_Change_Opcode(cgir_op, top);
  }
  Is_True(argCount == OP_opnds(cgir_op), ("OP_opnds mismatch in CGIR_update_OP"));
  for (int i = 0; i < argCount; i++) {
    CGIR_TN cgir_tn = Interface_lookupTN(interface, arguments[i]);
    if (OP_opnd(cgir_op, i) != cgir_tn) Set_OP_opnd(cgir_op, i, cgir_tn);
  }
  Is_True(resCount == OP_results(cgir_op), ("OP_results mismatch in CGIR_update_OP"));
  for (int i = 0; i < resCount; i++) {
    CGIR_TN cgir_tn = Interface_lookupTN(interface, results[i]);
    if (OP_result(cgir_op, i) != cgir_tn) Set_OP_result(cgir_op, i, cgir_tn);
  }
  // TODO: issue dates, flags, etc.
}

// Create a CGIR_BB from a LIR BasicBlock.
static CGIR_BB
CGIR_BB_create(BasicBlock basicblock, int labelCount, Label labels[], int opCount, Operation operations[]) {
  CGIR_BB cgir_bb = Gen_BB();
  for (int i = 0; i < labelCount; i++) {
    CGIR_LAB cgir_lab = Interface_lookupLAB(interface, labels[i]);
    // code borrowed from Gen_Label_For_BB
    Set_Label_BB(cgir_lab, cgir_bb);
    BB_Add_Annotation(cgir_bb, ANNOT_LABEL, (void *)cgir_lab);
  }
  OPS ops = OPS_EMPTY;
  for (int i = 0; i < opCount; i++) {
    OPS_Append_Op(&ops, Interface_lookupOP(interface, operations[i]));
  }
  BB_Append_Ops(cgir_bb, &ops);
  // TODO: loopinfo, flags, etc.
  return cgir_bb;
}

// Update a CGIR_BB from a LIR BasicBlock.
static void
CGIR_BB_update(CGIR_BB cgir_bb, BasicBlock basicblock, int labelCount, Label labels[], int opCount, Operation operations[]) {
  for (int i = 0; i < labelCount; i++) {
    CGIR_LAB label = Interface_lookupLAB(interface, labels[i]);
    if (!Is_Label_For_BB(label, cgir_bb)) {
      // code borrowed from Gen_Label_For_BB
      Set_Label_BB(label, cgir_bb);
      BB_Add_Annotation(cgir_bb, ANNOT_LABEL, (void *)label);
    }
  }
  BB_Remove_All(cgir_bb);
  OPS ops = OPS_EMPTY;
  for (int i = 0; i < opCount; i++) {
    OPS_Append_Op(&ops, Interface_lookupOP(interface, operations[i]));
  }
  BB_Append_Ops(cgir_bb, &ops);
  // TODO: loopinfo, flags, etc.
}

// Create a CGIR_LI from a LIR LoopInfo.
static CGIR_LI
CGIR_LI_create(LoopInfo loopinfo) {
  // TODO
  return NULL;
}

// Update a CGIR_LI from a LIR LoopInfo.
void
CGIR_LI_update(CGIR_LI cgir_li, LoopInfo loopinfo) {
  // TODO
}

// Create a CGIR_MI from a LIR MemInfo
CGIR_MI
CGIR_MI_create(MemInfo meminfo) {
  // TODO
  return NULL;
}

// Update a CGIR_MI from a LIR MemInfo.
void
CGIR_MI_update(CGIR_MI cgir_mi, MemInfo meminfo) {
  // TODO
}

// Chain two CGIR_BBs in the CGIR.
void
CGIR_BB_chain(CGIR_BB cgir_bb, CGIR_BB succ_cgir_bb) {
  Insert_BB(succ_cgir_bb, cgir_bb);
}

// Unchain a CGIR_BB in the CGIR.
void
CGIR_BB_unchain(CGIR_BB cgir_bb) {
  Remove_BB(cgir_bb);
}

// Link two CGIR_BBs in the CGIR with the given branch probability.
void
CGIR_BB_link(CGIR_BB orig_cgir_bb, CGIR_BB dest_cgir_bb, float probability) {
  Link_Pred_Succ_with_Prob(orig_cgir_bb, dest_cgir_bb, probability, 0);
}

// Unlink all the predecessors and successors of a CGIR_BB in the CGIR.
void
CGIR_BB_unlink(CGIR_BB cgir_bb) {
  BB_Delete_Predecessors(cgir_bb);
  BB_Delete_Successors(cgir_bb);
}

/*--------------------------- lao_init / lao_fini ----------------------------*/

// Initialization of the LAO, needs to be called once.
void
lao_init() {
  if (getenv("PID")) {
    int dummy; fprintf(stderr, "PID=%lld\n", (int64_t)getpid()); scanf("%d", &dummy);
  }
  if (lao_initialized++ == 0) {
    // initialize LIR; this constructs the interface variable
    LAO_INIT();
    // initialize the interface call-back pointers
    *Interface__CGIR_LAB_create(interface) = CGIR_LAB_create;
    *Interface__CGIR_LAB_update(interface) = CGIR_LAB_update;
    *Interface__CGIR_SYM_create(interface) = CGIR_SYM_create;
    *Interface__CGIR_SYM_update(interface) = CGIR_SYM_update;
    *Interface__CGIR_TN_Dedicated_create(interface) = CGIR_TN_Dedicated_create;
    *Interface__CGIR_TN_PseudoReg_create(interface) = CGIR_TN_PseudoReg_create;
    *Interface__CGIR_TN_Modifier_create(interface) = CGIR_TN_Modifier_create;
    *Interface__CGIR_TN_Absolute_create(interface) = CGIR_TN_Absolute_create;
    *Interface__CGIR_TN_Symbol_create(interface) = CGIR_TN_Symbol_create;
    *Interface__CGIR_TN_Label_create(interface) = CGIR_TN_Label_create;
    *Interface__CGIR_TN_update(interface) = CGIR_TN_update;
    *Interface__CGIR_OP_create(interface) = CGIR_OP_create;
    *Interface__CGIR_OP_update(interface) = CGIR_OP_update;
    *Interface__CGIR_BB_create(interface) = CGIR_BB_create;
    *Interface__CGIR_BB_update(interface) = CGIR_BB_update;
    *Interface__CGIR_LI_create(interface) = CGIR_LI_create;
    *Interface__CGIR_LI_update(interface) = CGIR_LI_update;
    *Interface__CGIR_MI_create(interface) = CGIR_MI_create;
    *Interface__CGIR_MI_update(interface) = CGIR_MI_update;
    *Interface__CGIR_BB_chain(interface) = CGIR_BB_chain;
    *Interface__CGIR_BB_unchain(interface) = CGIR_BB_unchain;
    *Interface__CGIR_BB_link(interface) = CGIR_BB_link;
    *Interface__CGIR_BB_unlink(interface) = CGIR_BB_unlink;
    // initialize TOP__Operator
    for (int i = 0; i < TOP_UNDEFINED; i++) TOP__Operator[i] = Operator_;
    TOP__Operator[TOP_add_i] = Operator_CODE_ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_add_ii] = Operator_CODE_ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_add_r] = Operator_CODE_ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_addcg] = Operator_CODE_ADDCG_DEST_BDEST_SRC1_SRC2_SCOND;
    TOP__Operator[TOP_and_i] = Operator_CODE_AND_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_and_ii] = Operator_CODE_AND_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_and_r] = Operator_CODE_AND_DEST_SRC1_SRC2;
    TOP__Operator[TOP_andc_i] = Operator_CODE_ANDC_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_andc_ii] = Operator_CODE_ANDC_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_andc_r] = Operator_CODE_ANDC_DEST_SRC1_SRC2;
    TOP__Operator[TOP_andl_i_b] = Operator_CODE_ANDL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_andl_ii_b] = Operator_CODE_ANDL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_andl_i_r] = Operator_CODE_ANDL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_andl_ii_r] = Operator_CODE_ANDL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_andl_r_b] = Operator_CODE_ANDL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_andl_r_r] = Operator_CODE_ANDL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_asm] = Operator_CODE_ASM15_DEST_SRC1_SRC2;
    TOP__Operator[TOP_begin_pregtn] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_br] = Operator_CODE_BR_BCOND_BTARG;
    TOP__Operator[TOP_break] = Operator_CODE_BREAK;
    TOP__Operator[TOP_brf] = Operator_CODE_BRF_BCOND_BTARG;
    TOP__Operator[TOP_bswap_r] = Operator_CODE_BSWAP_IDEST_SRC1;
    TOP__Operator[TOP_bwd_bar] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_call] = Operator_CODE_CALL_BTARG;
    TOP__Operator[TOP_cmpeq_i_b] = Operator_CODE_CMPEQ_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpeq_ii_b] = Operator_CODE_CMPEQ_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpeq_i_r] = Operator_CODE_CMPEQ_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpeq_ii_r] = Operator_CODE_CMPEQ_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpeq_r_b] = Operator_CODE_CMPEQ_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpeq_r_r] = Operator_CODE_CMPEQ_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpge_i_b] = Operator_CODE_CMPGE_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpge_ii_b] = Operator_CODE_CMPGE_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpge_i_r] = Operator_CODE_CMPGE_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpge_ii_r] = Operator_CODE_CMPGE_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpge_r_b] = Operator_CODE_CMPGE_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpge_r_r] = Operator_CODE_CMPGE_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgeu_i_b] = Operator_CODE_CMPGEU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgeu_ii_b] = Operator_CODE_CMPGEU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgeu_i_r] = Operator_CODE_CMPGEU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgeu_ii_r] = Operator_CODE_CMPGEU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgeu_r_b] = Operator_CODE_CMPGEU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgeu_r_r] = Operator_CODE_CMPGEU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgt_i_b] = Operator_CODE_CMPGT_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgt_ii_b] = Operator_CODE_CMPGT_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgt_i_r] = Operator_CODE_CMPGT_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgt_ii_r] = Operator_CODE_CMPGT_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgt_r_b] = Operator_CODE_CMPGT_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgt_r_r] = Operator_CODE_CMPGT_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgtu_i_b] = Operator_CODE_CMPGTU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgtu_ii_b] = Operator_CODE_CMPGTU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgtu_i_r] = Operator_CODE_CMPGTU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpgtu_ii_r] = Operator_CODE_CMPGTU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpgtu_r_b] = Operator_CODE_CMPGTU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpgtu_r_r] = Operator_CODE_CMPGTU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmple_i_b] = Operator_CODE_CMPLE_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmple_ii_b] = Operator_CODE_CMPLE_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmple_i_r] = Operator_CODE_CMPLE_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmple_ii_r] = Operator_CODE_CMPLE_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmple_r_b] = Operator_CODE_CMPLE_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmple_r_r] = Operator_CODE_CMPLE_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpleu_i_b] = Operator_CODE_CMPLEU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpleu_ii_b] = Operator_CODE_CMPLEU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpleu_i_r] = Operator_CODE_CMPLEU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpleu_ii_r] = Operator_CODE_CMPLEU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpleu_r_b] = Operator_CODE_CMPLEU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpleu_r_r] = Operator_CODE_CMPLEU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmplt_i_b] = Operator_CODE_CMPLT_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmplt_ii_b] = Operator_CODE_CMPLT_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmplt_i_r] = Operator_CODE_CMPLT_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmplt_ii_r] = Operator_CODE_CMPLT_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmplt_r_b] = Operator_CODE_CMPLT_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmplt_r_r] = Operator_CODE_CMPLT_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpltu_i_b] = Operator_CODE_CMPLTU_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpltu_ii_b] = Operator_CODE_CMPLTU_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpltu_i_r] = Operator_CODE_CMPLTU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpltu_ii_r] = Operator_CODE_CMPLTU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpltu_r_b] = Operator_CODE_CMPLTU_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpltu_r_r] = Operator_CODE_CMPLTU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpne_i_b] = Operator_CODE_CMPNE_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpne_ii_b] = Operator_CODE_CMPNE_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpne_i_r] = Operator_CODE_CMPNE_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_cmpne_ii_r] = Operator_CODE_CMPNE_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_cmpne_r_b] = Operator_CODE_CMPNE_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_cmpne_r_r] = Operator_CODE_CMPNE_DEST_SRC1_SRC2;
    TOP__Operator[TOP_copy_br] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_dfixup] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_divs] = Operator_CODE_DIVS_DEST_BDEST_SRC1_SRC2_SCOND;
    TOP__Operator[TOP_end_pregtn] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_ffixup] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_fwd_bar] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_goto] = Operator_CODE_GOTO_BTARG;
    TOP__Operator[TOP_icall] = Operator_CODE_ICALL;
    TOP__Operator[TOP_ifixup] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_igoto] = Operator_CODE_IGOTO;
    TOP__Operator[TOP_imml] = Operator_CODE_IMML_IMM;
    TOP__Operator[TOP_immr] = Operator_CODE_IMMR_IMM;
    TOP__Operator[TOP_intrncall] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_label] = Operator_PSEUDO_LABEL;
    TOP__Operator[TOP_ldb_d_i] = Operator_CODE_LDBD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldb_d_ii] = Operator_CODE_LDBD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldb_i] = Operator_CODE_LDB_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldb_ii] = Operator_CODE_LDB_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldbu_d_i] = Operator_CODE_LDBUD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldbu_d_ii] = Operator_CODE_LDBUD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldbu_i] = Operator_CODE_LDBU_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldbu_ii] = Operator_CODE_LDBU_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldh_d_i] = Operator_CODE_LDHD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldh_d_ii] = Operator_CODE_LDHD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldh_i] = Operator_CODE_LDH_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldh_ii] = Operator_CODE_LDH_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldhu_d_i] = Operator_CODE_LDHUD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldhu_d_ii] = Operator_CODE_LDHUD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldhu_i] = Operator_CODE_LDHU_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldhu_ii] = Operator_CODE_LDHU_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldw_d_i] = Operator_CODE_LDWD_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldw_d_ii] = Operator_CODE_LDWD_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_ldw_i] = Operator_CODE_LDW_IDESTL_ISRC2_SRC1;
    TOP__Operator[TOP_ldw_ii] = Operator_CODE_LDW_IDESTL_ISRCX_SRC1;
    TOP__Operator[TOP_max_i] = Operator_CODE_MAX_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_max_ii] = Operator_CODE_MAX_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_max_r] = Operator_CODE_MAX_DEST_SRC1_SRC2;
    TOP__Operator[TOP_maxu_i] = Operator_CODE_MAXU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_maxu_ii] = Operator_CODE_MAXU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_maxu_r] = Operator_CODE_MAXU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_mfb] = Operator_CODE_MFB_IDEST_SCOND;
    TOP__Operator[TOP_min_i] = Operator_CODE_MIN_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_min_ii] = Operator_CODE_MIN_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_min_r] = Operator_CODE_MIN_DEST_SRC1_SRC2;
    TOP__Operator[TOP_minu_i] = Operator_CODE_MINU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_minu_ii] = Operator_CODE_MINU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_minu_r] = Operator_CODE_MINU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_mov_i] = Operator_CODE_MOV_IDEST_ISRC2;
    TOP__Operator[TOP_mov_ii] = Operator_CODE_MOV_IDEST_ISRCX;
    TOP__Operator[TOP_mov_r] = Operator_CODE_MOV_DEST_SRC2;
    TOP__Operator[TOP_mtb] = Operator_CODE_MTB_BDEST_SRC1;
    TOP__Operator[TOP_mulh_i] = Operator_CODE_MULH_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulh_ii] = Operator_CODE_MULH_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulh_r] = Operator_CODE_MULH_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhh_i] = Operator_CODE_MULHH_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhh_ii] = Operator_CODE_MULHH_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhh_r] = Operator_CODE_MULHH_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhhu_i] = Operator_CODE_MULHHU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhhu_ii] = Operator_CODE_MULHHU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhhu_r] = Operator_CODE_MULHHU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhs_i] = Operator_CODE_MULHS_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhs_ii] = Operator_CODE_MULHS_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhs_r] = Operator_CODE_MULHS_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulhu_i] = Operator_CODE_MULHU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulhu_ii] = Operator_CODE_MULHU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulhu_r] = Operator_CODE_MULHU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mull_i] = Operator_CODE_MULL_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mull_ii] = Operator_CODE_MULL_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mull_r] = Operator_CODE_MULL_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mullh_i] = Operator_CODE_MULLH_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mullh_ii] = Operator_CODE_MULLH_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mullh_r] = Operator_CODE_MULLH_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mullhu_i] = Operator_CODE_MULLHU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mullhu_ii] = Operator_CODE_MULLHU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mullhu_r] = Operator_CODE_MULLHU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulll_i] = Operator_CODE_MULLL_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulll_ii] = Operator_CODE_MULLL_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulll_r] = Operator_CODE_MULLL_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mulllu_i] = Operator_CODE_MULLLU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mulllu_ii] = Operator_CODE_MULLLU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mulllu_r] = Operator_CODE_MULLLU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_mullu_i] = Operator_CODE_MULLU_IDESTL_SRC1_ISRC2;
    TOP__Operator[TOP_mullu_ii] = Operator_CODE_MULLU_IDESTL_SRC1_ISRCX;
    TOP__Operator[TOP_mullu_r] = Operator_CODE_MULLU_DESTL_SRC1_SRC2;
    TOP__Operator[TOP_nandl_i_b] = Operator_CODE_NANDL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_nandl_ii_b] = Operator_CODE_NANDL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_nandl_i_r] = Operator_CODE_NANDL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_nandl_ii_r] = Operator_CODE_NANDL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_nandl_r_b] = Operator_CODE_NANDL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_nandl_r_r] = Operator_CODE_NANDL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_noop] = Operator_PSEUDO_NOP;
    TOP__Operator[TOP_nop] = Operator_CODE_NOP;
    TOP__Operator[TOP_norl_i_b] = Operator_CODE_NORL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_norl_ii_b] = Operator_CODE_NORL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_norl_i_r] = Operator_CODE_NORL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_norl_ii_r] = Operator_CODE_NORL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_norl_r_b] = Operator_CODE_NORL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_norl_r_r] = Operator_CODE_NORL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_or_i] = Operator_CODE_OR_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_or_ii] = Operator_CODE_OR_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_or_r] = Operator_CODE_OR_DEST_SRC1_SRC2;
    TOP__Operator[TOP_orc_i] = Operator_CODE_ORC_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_orc_ii] = Operator_CODE_ORC_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_orc_r] = Operator_CODE_ORC_DEST_SRC1_SRC2;
    TOP__Operator[TOP_orl_i_b] = Operator_CODE_ORL_IBDEST_SRC1_ISRC2;
    TOP__Operator[TOP_orl_ii_b] = Operator_CODE_ORL_IBDEST_SRC1_ISRCX;
    TOP__Operator[TOP_orl_i_r] = Operator_CODE_ORL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_orl_ii_r] = Operator_CODE_ORL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_orl_r_b] = Operator_CODE_ORL_BDEST_SRC1_SRC2;
    TOP__Operator[TOP_orl_r_r] = Operator_CODE_ORL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_pft] = Operator_CODE_PFT_ISRC2_SRC1;
    TOP__Operator[TOP_phi] = Operator_PSEUDO_PHI;
    TOP__Operator[TOP_prgadd] = Operator_CODE_PRGADD_ISRC2_SRC1;
    TOP__Operator[TOP_prgins] = Operator_CODE_PRGINS;
    TOP__Operator[TOP_prgset] = Operator_CODE_PRGSET_ISRC2_SRC1;
    TOP__Operator[TOP_psi] = Operator_PSEUDO_PSI;
    TOP__Operator[TOP_return] = Operator_MACRO_RETURN;
    TOP__Operator[TOP_rfi] = Operator_CODE_RFI;
    TOP__Operator[TOP_sbrk] = Operator_CODE_SBRK;
    TOP__Operator[TOP_sh1add_i] = Operator_CODE_SH1ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh1add_ii] = Operator_CODE_SH1ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh1add_r] = Operator_CODE_SH1ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sh2add_i] = Operator_CODE_SH2ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh2add_ii] = Operator_CODE_SH2ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh2add_r] = Operator_CODE_SH2ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sh3add_i] = Operator_CODE_SH3ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh3add_ii] = Operator_CODE_SH3ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh3add_r] = Operator_CODE_SH3ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sh4add_i] = Operator_CODE_SH4ADD_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sh4add_ii] = Operator_CODE_SH4ADD_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sh4add_r] = Operator_CODE_SH4ADD_DEST_SRC1_SRC2;
    TOP__Operator[TOP_shl_i] = Operator_CODE_SHL_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_shl_ii] = Operator_CODE_SHL_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_shl_r] = Operator_CODE_SHL_DEST_SRC1_SRC2;
    TOP__Operator[TOP_shr_i] = Operator_CODE_SHR_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_shr_ii] = Operator_CODE_SHR_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_shr_r] = Operator_CODE_SHR_DEST_SRC1_SRC2;
    TOP__Operator[TOP_shru_i] = Operator_CODE_SHRU_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_shru_ii] = Operator_CODE_SHRU_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_shru_r] = Operator_CODE_SHRU_DEST_SRC1_SRC2;
    TOP__Operator[TOP_slct_i] = Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRC2;
    TOP__Operator[TOP_slct_ii] = Operator_CODE_SLCT_IDEST_SCOND_SRC1_ISRCX;
    TOP__Operator[TOP_slct_r] = Operator_CODE_SLCT_DEST_SCOND_SRC1_SRC2;
    TOP__Operator[TOP_slctf_i] = Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRC2;
    TOP__Operator[TOP_slctf_ii] = Operator_CODE_SLCTF_IDEST_SCOND_SRC1_ISRCX;
    TOP__Operator[TOP_slctf_r] = Operator_CODE_SLCTF_DEST_SCOND_SRC1_SRC2;
    TOP__Operator[TOP_spadjust] = Operator_PSEUDO_PRO64;
    TOP__Operator[TOP_stb_i] = Operator_CODE_STB_ISRC2_SRC1_SRC2;
    TOP__Operator[TOP_stb_ii] = Operator_CODE_STB_ISRCX_SRC1_SRC2;
    TOP__Operator[TOP_sth_i] = Operator_CODE_STH_ISRC2_SRC1_SRC2;
    TOP__Operator[TOP_sth_ii] = Operator_CODE_STH_ISRCX_SRC1_SRC2;
    TOP__Operator[TOP_stw_i] = Operator_CODE_STW_ISRC2_SRC1_SRC2;
    TOP__Operator[TOP_stw_ii] = Operator_CODE_STW_ISRCX_SRC1_SRC2;
    TOP__Operator[TOP_sub_i] = Operator_CODE_SUB_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_sub_ii] = Operator_CODE_SUB_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_sub_r] = Operator_CODE_SUB_DEST_SRC1_SRC2;
    TOP__Operator[TOP_sxtb_r] = Operator_CODE_SXTB_IDEST_SRC1;
    TOP__Operator[TOP_sxth_r] = Operator_CODE_SXTH_IDEST_SRC1;
    TOP__Operator[TOP_sync] = Operator_CODE_SYNC;
    TOP__Operator[TOP_syscall] = Operator_CODE_SYSCALL;
    TOP__Operator[TOP_xor_i] = Operator_CODE_XOR_IDEST_SRC1_ISRC2;
    TOP__Operator[TOP_xor_ii] = Operator_CODE_XOR_IDEST_SRC1_ISRCX;
    TOP__Operator[TOP_xor_r] = Operator_CODE_XOR_DEST_SRC1_SRC2;
    // initialize IEC__Modifier
    for (int i = 0; i < EC_MAX; i++) IEC__Modifier[i] = Modifier_;
    // initialize LC__Immediate
    for (int i = 0; i < LC_MAX; i++) LC__Immediate[i] = Immediate_;
    LC__Immediate[0] = Immediate_I_signed_32_overflow_dont; // HACK ALERT
    LC__Immediate[LC_s32] = Immediate_I_signed_32_overflow_dont;
    LC__Immediate[LC_u32] = Immediate_I_signed_32_overflow_dont;
    LC__Immediate[LC_s23] = Immediate_I_signed_23_overflow_signed;
    LC__Immediate[LC_u23] = Immediate_I_unsigned_23_overflow_unsigned;
    LC__Immediate[LC_s9] = Immediate_I_signed_9_overflow_signed;
    // initialize IRC__RegClass
    for (int i = 0; i <= ISA_REGISTER_CLASS_MAX; i++) IRC__RegClass[i] = RegClass_;
    IRC__RegClass[ISA_REGISTER_CLASS_integer] = RegClass_GRC;
    IRC__RegClass[ISA_REGISTER_CLASS_branch] = RegClass_BRC;
  }
}

// Finalization of the LAO, needs to be called once.
void
lao_fini() {
  if (--lao_initialized == 0) {
    // finalize LIR
    LAO_FINI();
  }
}

/*-------------------------- LAO Utility Functions----------------------------*/

// Enter the control-flow arcs in the LAO.
static void
lao_setLeaveArcs(BB *bb, BasicBlock basicblock) {
  BBLIST *bblist = NULL;
  FOR_ALL_BB_SUCCS(bb, bblist) {
    BB *succ_bb = BBLIST_item(bblist);
    BasicBlock succ_basicblock = Interface_searchBasicBlock(interface, succ_bb);
    if (succ_basicblock != NULL) {
      float probability = BBLIST_prob(bblist);
      unsigned flags = BBLIST_flags(bblist);
      Interface_createControlArc(interface, basicblock, succ_basicblock, probability, flags);
    }
  }
}

// Enter the live-in information in the LAO.
static void
lao_setLiveIn(BB *bb, BasicBlock basicblock) {
  for (TN *tn = GTN_SET_Choose(BB_live_in(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_in(bb), tn)) {
    TempName tempname = CGIR_TN_to_TempName(tn);
    Interface_BasicBlock_setLiveIn(interface, basicblock, tempname);
  }
}

// Enter the live-out information in the LAO.
static void
lao_setLiveOut(BB *bb, BasicBlock basicblock) {
  for (TN *tn = GTN_SET_Choose(BB_live_out(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_out(bb), tn)) {
    TempName tempname = CGIR_TN_to_TempName(tn);
    Interface_BasicBlock_setLiveOut(interface, basicblock, tempname);
  }
}

// Enter the BB Memory dependences into the LAO.
static void
lao_setMemoryDependences(BB* bb, LoopInfo loopinfo) {
  OP *op = NULL;
  FOR_ALL_BB_OPs(bb, op) {
    ARC_LIST *arcs = NULL;
    if (_CG_DEP_op_info(op)) {
      Operation orig_operation = Interface_searchOperation(interface, op);
      for (arcs = OP_succs(op); arcs; arcs = ARC_LIST_rest(arcs)) {
	ARC *arc = ARC_LIST_first(arcs);
	CG_DEP_KIND kind = ARC_kind(arc);
	if (ARC_is_mem(arc)) {
	  bool definite = ARC_is_definite(arc);
	  int latency = ARC_latency(arc), omega = ARC_omega(arc);
	  OP *pred_op = ARC_pred(arc), *succ_op = ARC_succ(arc);
	  Is_True(pred_op == op, ("Error in lao_setMemoryDependences"));
	  Operation dest_operation = Interface_searchOperation(interface, succ_op);
	  Interface_LoopInfo_setMemoryDependence(interface, loopinfo,
	      orig_operation, dest_operation, latency, omega, definite);
	  //CG_DEP_Trace_Arc(arc, TRUE, FALSE);
	}
      }
    } else fprintf(TFile, "<arc>   CG_DEP INFO is NULL\n");
  }
}

// Declare CG_DEP_Compute_Region_MEM_Arcs().
void 
CG_DEP_Compute_Region_MEM_Arcs(list<BB*>    bb_list, 
			    BOOL         compute_cyclic, 
			    BOOL         memread_arcs);

// Make a LAO LoopInfo from the BB_List supplied.
static LoopInfo
lao_makeLoopInfo(BB_List& bb_list, bool cyclic) {
  BB *bb = bb_list.front();
  CGIR_LAB cgir_lab = Gen_Label_For_BB(bb);
  Label label = Interface_searchLabel(interface, cgir_lab);
  LoopInfo loopinfo = Interface_createLoopInfo(interface, label);
  CG_DEP_Compute_Region_MEM_Arcs(bb_list,
      cyclic,	// compute_cyclic
      false);	// memread_arcs
  BB_List::iterator bb_iter;
  for (bb_iter = bb_list.begin(); bb_iter != bb_list.end(); bb_iter++) {
    lao_setMemoryDependences(*bb_iter, loopinfo);
  }
  CG_DEP_Delete_Graph(&bb_list);
  return loopinfo;
}

/*----------------------- LAO Optimization Functions -------------------------*/

// Low-level LAO_optimize entry point.
static bool
lao_optimize(BB_List &entryBBs, BB_List &innerBBs, BB_List &exitBBs, unsigned lao_actions) {
  bool result = false;
  BB_List::iterator bb_iter;
  //
  if (getenv("PRINT")) CGIR_print();
  Interface_open(interface);
  // Create the LAO BasicBlocks.
  for (bb_iter = entryBBs.begin(); bb_iter != entryBBs.end(); bb_iter++) {
    BasicBlock basicblock = CGIR_BB_to_BasicBlock(*bb_iter);
    Interface_setEntry(interface, basicblock);
    fprintf(TFile, "BB_entry(%d)\n", BB_id(*bb_iter));
  }
  for (bb_iter = innerBBs.begin(); bb_iter != innerBBs.end(); bb_iter++) {
    BasicBlock basicblock = CGIR_BB_to_BasicBlock(*bb_iter);
    fprintf(TFile, "BB_body(%d)\n", BB_id(*bb_iter));
  }
  for (bb_iter = exitBBs.begin(); bb_iter != exitBBs.end(); bb_iter++) {
    BasicBlock basicblock = CGIR_BB_to_BasicBlock(*bb_iter);
    Interface_setExit(interface, basicblock);
    fprintf(TFile, "BB_exit(%d)\n", BB_id(*bb_iter));
  }
  // Add the control-flow arcs, the live-in, and the live-out.
  Interface_FOREACH_BasicBlock(interface, bb, basicblock) {
    lao_setLeaveArcs(bb, basicblock);
    lao_setLiveIn(bb, basicblock);
    lao_setLiveOut(bb, basicblock);
  } Interface_ENDEACH_BasicBlock
  //
  bool cyclic = lao_actions & LAO_LoopSchedule || lao_actions & LAO_LoopPipeline;
  LoopInfo loopinfo = lao_makeLoopInfo(innerBBs, cyclic);
  //
  result = LAO_Optimize(lao_actions);
  //
  //WORK CodeRegion_updateCGIR(coderegion);
  //
  if (getenv("PRINT")) CGIR_print();
  Interface_close(interface);
  //
  return result;
}

// Optimize a LOOP_DESCR inner loop through the LAO.
bool
lao_optimize(CG_LOOP *cg_loop, unsigned lao_actions) {
  BB_List entryBBs, innerBBs, exitBBs;
  bool result = false;
  //
  LOOP_DESCR *loop = cg_loop->Loop();
  if (BB_innermost(LOOP_DESCR_loophead(loop))) {
    //
    entryBBs.push_back(CG_LOOP_prolog);
    //
    // Enter the body blocks in linear order.
    BB *loop_head = LOOP_DESCR_loophead(loop);
    BB *loop_tail = LOOP_DESCR_Find_Unique_Tail(loop);
    //
    if (loop_tail != NULL) {
      //
      for (BB *bb = loop_head;
	   bb && BB_prev(bb) != loop_tail;
	   bb = BB_next(bb)) {
	if (BB_SET_MemberP(LOOP_DESCR_bbset(loop), bb)) {
	  innerBBs.push_back(bb);
	  //
	  BBLIST *succs = NULL;
	  FOR_ALL_BB_SUCCS(bb, succs) {
	    BB *succ = BBLIST_item(succs);
	    if (!BB_SET_MemberP(LOOP_DESCR_bbset(loop), succ)) {
	      // Ensure that a bb is not put twice in the exitBBs.
	      if (!CGIR_inBB_List(exitBBs, succ)) exitBBs.push_back(succ);
	    }
	  }
	}
      }
      //
      // Call the main lao_optimize entry point.
      if (getenv("LOOP")) {
	fprintf(TFile, "LOOP_optimize\n");
	result = lao_optimize(entryBBs, innerBBs, exitBBs, lao_actions);
      }
    }
  }
  //
  return result;
}

// Optimize a HB through the LAO.
bool
lao_optimize(HB *hb, unsigned lao_actions) {
  BB_List entryBBs, innerBBs, exitBBs;
  bool result = false;
  //
  entryBBs.push_back(HB_Entry(hb));
  //
  BB *bb = NULL;
  // TODO: ensure the proper linear order of BBs
  FOR_ALL_BB_SET_members(HB_Blocks(hb), bb) {
    if (!CGIR_inBB_List(entryBBs, bb)) {
      innerBBs.push_back(bb);
    }
    //
    BBLIST *succs = NULL;
    FOR_ALL_BB_SUCCS(bb, succs) {
      BB *succ = BBLIST_item(succs);
      if (!HB_Contains_Block(hb, succ)) {
	if (!CGIR_inBB_List(exitBBs, succ))
	  exitBBs.push_back(succ);
      }
    }
  }
  //
  if (getenv("HB")) {
    fprintf(TFile, "HB_optimize\n");
    result = lao_optimize(entryBBs, innerBBs, exitBBs, lao_actions);
  }
  //
  return result;
}

// Optimize a function through the LAO.
bool
lao_optimize(unsigned lao_actions) {
fprintf(TFile, "Function_optimize\n");
  BB_List entryBBs, innerBBs, exitBBs;
  BBLIST *bl;
  BB *bp;
  int predCount, succCount;
  bool result = false;
  //
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    predCount = 0;
    FOR_ALL_BB_PREDS (bp, bl) {
      predCount ++;
    }
    succCount = 0;
    FOR_ALL_BB_SUCCS (bp, bl) {
      succCount ++;
    }
    if (predCount == 0) {
      entryBBs.push_back(bp);
    }
    else if (succCount == 0) {
      exitBBs.push_back(bp);
    }
    else
      innerBBs.push_back(bp);
  }
  //
  result = lao_optimize(entryBBs, innerBBs, exitBBs, lao_actions);
  //
  return result;
}

/*-------------------------- CGIR Print Functions ----------------------------*/

typedef struct OP_list {
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
CGIR_TN_print ( const TN *tn )
{
  //
  if (TN_is_constant(tn)) {
    if ( TN_has_value(tn)) {
      fprintf ( TFile, "(0x%llx)", TN_value(tn) );
      if (TN_size(tn) == 4 && 
	  TN_value(tn) >> 32 != 0 &&
	  TN_value(tn) >> 31 != -1)
	fprintf ( TFile, "!!! TN_value=0x%llx is too big to fit in a word",
		  TN_value(tn));
    }
    else if (TN_is_enum(tn)) {
      fprintf ( TFile, "(enum:%s)", ISA_ECV_Name(TN_enum(tn)) );
    }
    else if ( TN_is_label(tn) ) {
      CGIR_LAB lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      INT64 offset = TN_offset(tn);
      BB *targetBB;
      if ( offset == 0 ) {
	fprintf ( TFile, "(lab:%s)", name );
      }
      else {
	fprintf ( TFile, "(lab:%s+%lld)", name, offset );
      }
      targetBB = Get_Label_BB(lab);
      if (targetBB != NULL)
	fprintf(TFile, " --> %d", BB_id(targetBB));
    } 
    else if ( TN_is_tag(tn) ) {
      CGIR_LAB lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      fprintf ( TFile, "(tag:%s)", name );
    }
    else if ( TN_is_symbol(tn) ) {
      ST *var = TN_var(tn);
      //
      fprintf ( TFile, "(sym" );
      fprintf ( TFile, TN_RELOCS_Name(TN_relocs(tn)) );
      //
      if (ST_class(var) == CLASS_CONST)
      	fprintf ( TFile, ":%s)", Targ_Print(NULL, ST_tcon_val(var)));
      else
      	fprintf ( TFile, ":%s%+lld)", ST_name(var), TN_offset(tn) );
    } 
    else {
      ErrMsg (EC_Unimplemented, "sPrint_TN: illegal constant TN");
    }
  }
  else {  /* register TN */
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
	fprintf ( TFile, "%s", 
		  REGISTER_name(TN_register_class(tn), TN_register(tn)));
      } else {
	fprintf ( TFile, "(%d,%d)", TN_register_class(tn), TN_register(tn));
      }
    }
    else if (TN_is_global_reg(tn)) {
      fprintf ( TFile, "G%d", TN_number(tn) );
    }
    else {
      fprintf ( TFile, "T%d", TN_number(tn) );
    }
    if (TN_is_save_reg(tn)) {
      fprintf ( TFile, "(sv:%s)", 
		REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
  }
}

static void
CGIR_OP_print ( const OP *op )
{
  int i;
  //
  if (OP_has_tag(op)) {
    LABEL_IDX tag = Get_OP_Tag(op);
    fprintf (TFile, "<tag %s>: ", LABEL_name(tag));
  }
  //
  fprintf(TFile, "%s", TOP_Name(OP_code(op)));
  //
  if ( OP_variant(op) != 0 ) {
    fprintf ( TFile, "(%x)", OP_variant(op));
  }
  //
  if (OP_results(op) == 0)
    fprintf(TFile, " void");
  //
  else for (i = 0; i < OP_results(op); i++) {
    fprintf(TFile, "%s", (i == 0 ? " " : ", "));
    CGIR_TN_print(OP_result(op,i));
  }
  //
  fprintf(TFile, " =");
  //
  for (i=0; i<OP_opnds(op); i++) {
    fprintf(TFile, "%s", (i == 0 ? " " : ", "));
    TN *tn = OP_opnd(op,i);
    CGIR_TN_print(tn);
    if (OP_Defs_TN(op, tn)) fprintf(TFile, "<def>");
  }
}

static void
CGIR_OP_prints ( const OP *op )
{
  for ( ; op; op = OP_next(op)) {
    fprintf(TFile, "\t");
    CGIR_OP_print(op);
    fprintf(TFile, "       \t#line[%4d]", Srcpos_To_Line(OP_srcpos(op)));
    fprintf(TFile, "\n");
  }
}

static void
CGIR_BB_print_header (BB *bp)
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
    fprintf ( TFile, "Entrypoint: %s\t Starting Line %d\n",
	      ST_name(ENTRYINFO_name(ent)),
	      Srcpos_To_Line(ENTRYINFO_srcpos(ent)));
    //
    if (sp_adj) {
      OP *op;
      BOOL found_sp_adj = FALSE;
      fprintf ( TFile, "SP entry adj: " );
      Print_OP_No_SrcLine (sp_adj);
      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( TFile, "******** ERROR ******** sp adjust not found in entry block\n");
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
      fprintf ( TFile, "SP exit adj: " );
      Print_OP_No_SrcLine (sp_adj);
      //
      FOR_ALL_BB_OPs_FWD(bp,op)
	if (op == sp_adj) {
	  found_sp_adj = TRUE;
	  break;
	}
      if (found_sp_adj == FALSE)
	fprintf ( TFile, "******** ERROR ******** sp adjust not found in exit block\n");
    }
  }
  //
  fprintf ( TFile, "    BB %d, flags 0x%04x",
	    BB_id(bp), BB_flag(bp) );
  //
  if (freqs || BB_freq_fb_based(bp))
    fprintf(TFile, ", freq %g (%s)", BB_freq(bp),
	    BB_freq_fb_based(bp) ? "feedback" : "heuristic");
  
  if (BB_unreachable(bp)) fprintf ( TFile, ", Unreachable");
  if (BB_entry(bp))	fprintf ( TFile, ", Entry" );
  if (BB_handler(bp))	fprintf ( TFile, ", Handler" );
  if (BB_asm(bp)) 	fprintf ( TFile, ", Asm" );
  //
  if (BB_exit(bp)) {
    if (BB_call(bp))	fprintf ( TFile, ", Tail-call" );
    else		fprintf ( TFile, ", Exit" );
  } else if (BB_call(bp)) fprintf ( TFile, ", Call" );
  //
  if (BB_rid(bp)) {
    INT exits;
    RID *rid = BB_rid(bp);
    CGRIN *cgrin = RID_cginfo(rid);
    if (cgrin) {
      if (bp == CGRIN_entry(cgrin)) {
	fprintf ( TFile, ", Region-entry " );
      }
      exits = RID_num_exits(rid);
      for (i = 0; i < exits; ++i) {
	if (bp == CGRIN_exit_i(cgrin, i)) {
	  fprintf ( TFile, ", Region-exit[%d]", i );
	}
      }
    }
  }
  //
  fprintf ( TFile, "\n");
  //
  if (annot)
    Print_LOOPINFO(ANNOT_loopinfo(annot));
  //
  if (BB_loop_head_bb(bp)) {
    if (BB_loophead(bp)) {
      if (!annot) {
	fprintf(TFile, "\tHead of loop body line %d\n", BB_Loop_Lineno(bp));
      }
    } else {
      BB *head = BB_loop_head_bb(bp);
      fprintf(TFile,
	      "\tPart of loop body starting at line %d with head BB:%d\n",
	      BB_Loop_Lineno(head), BB_id(head));
    }
  }
  //
  if (BB_unrollings(bp) > 1)
    fprintf(TFile, "\tUnrolled %d times%s\n", BB_unrollings(bp),
	    BB_unrolled_fully(bp) ? " (fully)" : "");
  //
  if ( BB_rid(bp) )
    RID_Fprint( TFile, BB_rid(bp) );
  //
  fprintf ( TFile, "\tpred" );
  FOR_ALL_BB_PREDS (bp, bl) {
    fprintf ( TFile, " %d", BB_id(BBLIST_item(bl)));
  }
  //
  fprintf ( TFile, "\n\tsucc%s", freqs ? " (w/probs)" : "" );
  FOR_ALL_BB_SUCCS (bp, bl) {
    fprintf ( TFile, " %d",
	      BB_id(BBLIST_item(bl)));
    if (freqs) fprintf(TFile, "(%g)", BBLIST_prob(bl));
  }
  fprintf ( TFile, "\n" );
  //
  if (BB_has_label(bp)) {
    ANNOTATION *ant;
    fprintf(TFile, "\tLabel");
    for (ant = ANNOT_First(BB_annotations(bp), ANNOT_LABEL);
	 ant != NULL;
	 ant = ANNOT_Next(ant, ANNOT_LABEL))
      {
	INT eh_labs = 0;
	LABEL_IDX lab = ANNOT_label(ant);
	fprintf (TFile," %s", LABEL_name(lab));
	FmtAssert((Get_Label_BB(lab) == bp),
		  (" Inconsistent ST for BB:%2d label", BB_id(bp)));
	switch (LABEL_kind(Label_Table[lab])) {
	case LKIND_BEGIN_EH_RANGE:
	  fprintf (TFile,"%cbegin_eh_range", eh_labs++ ? ' ' : '(');
	  break;
	case LKIND_END_EH_RANGE:
	  fprintf (TFile,"%cend_eh_range", eh_labs++ ? ' ' : '(');
	  break;
	}
	if (eh_labs)
	  fprintf (TFile,")");
      }
    fprintf(TFile, "\n");
  }
  //
  return;
}

static void
CGIR_BB_print (BB *bp)
{
  CGIR_BB_print_header (bp );
  if (BB_first_op(bp))	CGIR_OP_prints (BB_first_op(bp));
}

static void
CGIR_Alias_print()
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
  fprintf(TFile, "--------------- Begin Print Alias ---------------\n");
  //
  for (elt1 = memops; elt1; elt1 = elt1->next) {
    fprintf(TFile, "<Alias>"); CGIR_OP_print(elt1->op); fprintf(TFile, "\n");
    for (elt2 = memops; elt2 != elt1; elt2 = elt2->next) {
      fprintf(TFile, "\t<with>"); CGIR_OP_print(elt2->op); fprintf(TFile, "\t");
      alias = CG_DEP_Mem_Ops_Alias(elt1->op, elt2->op, &identical);
      if (!alias)          fprintf(TFile, "NO-ALIAS");
      else if (!identical) fprintf(TFile, "   ALIAS");
      else                 fprintf(TFile, "IDENTICAL");
      fprintf(TFile, "</with>\n");
    }
    fprintf(TFile, "</Alias>\n");
  }
  //
  fprintf(TFile, "---------------- End Print Alias ----------------\n");
}

static void
CGIR_print()
{
  BB *bp;
  //
  fprintf(TFile, "--------CFG Begin--------\n");
  for (bp = REGION_First_BB; bp; bp = BB_next(bp)) {
    CGIR_BB_print ( bp );
    fprintf ( TFile,"\n" );
  }
  //
  CGIR_Alias_print();
  //
  // Print live-analysis information
  fprintf(TFile, "-------- CFG End --------\n");
}

