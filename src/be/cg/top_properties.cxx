

#include "op.h"
#include "top_properties.h"
#include "variants.h"
#include "tracing.h"

/*
 * Definition for the largest host int type.
 */
#define INTM_BITS 64
typedef INT64 intm_t;
typedef UINT64 uintm_t;

/*
 * Definition of the open64 interface.
 */
typedef TOP opc_t;
typedef const TN *opnd_t;

static  INT64
TN_Value(const TN *tn) {
  if (TN_is_const_reg(tn)) {
    if (TN_is_zero_reg(tn)) return 0;
    if (TN_is_true_pred(tn)) return 1;
    DevAssert(0,("Unexpected"));
  } else if (TN_has_value(tn)) {
    return TN_value(tn);
  } else if (TN_is_symbol(tn)) {
    return TN_offset(tn);
  } else {
    DevAssert(0,("Unexpected"));
  }
}
static intm_t opnd_value(opnd_t opnd) { return TN_Value(opnd); }
static int opc_opnd_signed(opc_t opc, int opnd) { return TOP_opnd_use_signed(opc, opnd); }
static int opc_opnd_bits(opc_t opc, int opnd) { return TOP_opnd_use_bits(opc, opnd); }
static int opc_ou_opnd_idx(opc_t opc, int ou) { return TOP_Find_Operand_Use(opc, ou); }
static int opc_cond_variant(opc_t opc) { return TOP_cond_variant(opc); }
static int op_cmp_variant(OP *op) { return OP_cmp_variant(op); }

static int opc_opnd_negative(opc_t opc, int opnd) {
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info (opc);
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  if (ISA_OPERAND_VALTYP_Is_Literal(otype)) {
    ISA_LIT_CLASS lit_class = ISA_OPERAND_VALTYP_Literal_Class(otype);
    if (ISA_LC_Is_Negative(lit_class)) {
      return 1;
    }
  }
  return 0;
}

#define opc_is(opc, prop) TOP_is_##prop(opc)

/*
 * Utility functions.
 */

/* Zero extension. 
 * 0 <= bits <= INTM_BITS
 * Returns (value mod 2^bits)
 */
static intm_t zext(intm_t value, int bits) {
  if (bits == 0) return 0;
  return ((uintm_t)value << INTM_BITS-bits) >> INTM_BITS-bits;
}

/* Sign extension. 
 * 0 <= bits <= INTM_BITS
 * Return (value cmod 2^bits) where
 * cmod is the rest of the centered division.
 */
static intm_t sext(intm_t value, int bits) {
  if (bits == 0) return 0;
  return (value << INTM_BITS-bits) >> INTM_BITS-bits;
}

/* Sign extension for NEGATIVE. 
 * 0 <= bits <= INTM_BITS
 * Return (+/- |value| mod 2^bits) where
 * the sign is the same as initial sign.
 */
static intm_t nsext(intm_t value, int bits) {
  intm_t sign, val;
  if (bits == 0) return 0;
  sign = (value >> INTM_BITS-1) << bits;
  val = ((uintm_t)value << INTM_BITS-bits) >> INTM_BITS-bits;
  return (sign | val);
}

/* Operand fetch. */
static intm_t fetch(opc_t opc, const opnd_t *opnds, int opnd) {
  intm_t value = opnd_value(opnds[opnd]);
  if (opc_opnd_signed(opc, opnd)) {
    if (opc_opnd_negative(opc, opnd)) {
      value = nsext(value, opc_opnd_bits(opc, opnd));
    }
    else {
      value = sext(value, opc_opnd_bits(opc, opnd));
    }
  }
  else {
    value = zext(value, opc_opnd_bits(opc, opnd));
  }
  return value;
}

/* max_representable_value */
static uintm_t 
max_representable_value(opc_t opc, int opnd) {
  return (1 << opc_opnd_bits(opc, opnd)) - 1;
}

/*
 * Fetching an operand.
 */
intm_t TOP_fetch_opnd(TOP opc, const TN * const opnds[], int opnd) { return fetch(opc, opnds, opnd); }

/*
 * Constant folding of opcode properties.
 */
intm_t TOP_fold_iadd(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  result = op1 + op2;
  return result;
}

intm_t TOP_fold_isub(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  result = op1 - op2;
  return result;
}

intm_t TOP_fold_imul(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  result = op1 * op2;
  return result;
}

intm_t TOP_fold_imin(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  if (opc_opnd_signed(opc, opnd1))
      result = op1 < op2 ? op1 : op2;
  else
      result = (uintm_t)op1 < op2 ? op1 : op2;      
  return result;
}

intm_t TOP_fold_imax(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  if (opc_opnd_signed(opc, opnd1))
      result = op1 > op2 ? op1 : op2;
  else
      result = (uintm_t)op1 > op2 ? op1 : op2;      
  return result;
}

intm_t TOP_fold_not(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  intm_t op1 = fetch(opc, opnds, opnd1);
  result = ~op1;
  return result;
}

intm_t TOP_fold_and(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  result = op1 & op2;
  return result;
}

intm_t TOP_fold_or(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  result = op1 | op2;
  return result;
}

intm_t TOP_fold_xor(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  result = op1 ^ op2;
  return result;
}

intm_t TOP_fold_shl(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  if (op2 <= max_representable_value(opc,opnd2))
    result = op1 << op2;
  else
    result = 0;
  return result;
}

intm_t TOP_fold_shr(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  if (op2 <= max_representable_value(opc,opnd2))
    result = op1 >> op2;
  else
    result = op1 < 0 ? -1: 0;
  return result;
}

intm_t TOP_fold_shru(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  if (op2 <= max_representable_value(opc,opnd2)) {
    result = (uintm_t)op1 >> op2;
  }
  else {
    result = op1 < 0 ? -1: 0;
  }
  return result;
}

intm_t TOP_fold_move(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  result = fetch(opc, opnds, opnd1);
  return result;
}


intm_t TOP_fold_zext(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  result = fetch(opc, opnds, opnd1);
  return result;
}

intm_t TOP_fold_sext(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  result = fetch(opc, opnds, opnd1);
  return result;
}

intm_t TOP_fold_select(opc_t opc, const opnd_t *opnds) {
  intm_t result;
  int condition = opc_ou_opnd_idx(opc, OU_condition);
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t cond = fetch(opc, opnds, condition);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);
  VARIANT variant = opc_cond_variant(opc);
  if ((variant == V_COND_TRUE && cond != 0) ||
      (variant == V_COND_FALSE && cond == 0))
    result = op1;
  else
    result = op2;
  return result;
}

  intm_t OP_fold_icmp(OP *op, const opnd_t *opnds) {
  intm_t result;
  opc_t opc = OP_code(op);  
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);
  intm_t op1 = fetch(opc, opnds, opnd1);
  intm_t op2 = fetch(opc, opnds, opnd2);

  VARIANT variant = op_cmp_variant(op);
  switch (variant) {
  case V_CMP_NE: result = (op1 != op2); break;
  case V_CMP_EQ: result = (op1 == op2); break;
  case V_CMP_LE: result = (op1 <= op2); break;
  case V_CMP_LEU: result = ((uintm_t)op1 <= op2); break;
  case V_CMP_LT: result = (op1 < op2); break;
  case V_CMP_LTU: result = ((uintm_t)op1 < op2); break;
  case V_CMP_GT: result = (op1 > op2); break;
  case V_CMP_GTU: result = ((uintm_t)op1 > op2); break;
  case V_CMP_GE: result = (op1 >= op2); break;
  case V_CMP_GEU: result = ((uintm_t)op1 >= op2); break;
  case V_CMP_ANDL: result = (op1 != 0) & (op2 != 0); break;
  case V_CMP_NANDL: result = (op1 == 0) | (op2 == 0); break;
  case V_CMP_ORL: result = (op1 != 0) | (op2 != 0); break;
  case V_CMP_NORL: result = (op1 == 0) & (op2 == 0); break;
  }
  return result;
}


/*
 * Constraints on properties.
 */
#define CHECK(x) FmtAssert((x), ("Inconsistent property: %s", #x))
#define TOP_CHECK(x) FmtAssert((x), ("TOP %s has inconsistent property: %s", TOP_Name((TOP)opc), #x))


int TOP_check_properties(opc_t opc)
{
  int ok = 1;
  int opnd1 = opc_ou_opnd_idx(opc, OU_opnd1);
  int opnd2 = opc_ou_opnd_idx(opc, OU_opnd2);

  /* Check that host sign extends. */
  CHECK((intm_t)-1 >> 1 == -1);
  /* Check that host is 2 complement. */
  CHECK((intm_t)-1 == ~(uintm_t)1 + 1);

  if (opc_is(opc, intop) &&
      (opc_is(opc, add) ||
      opc_is(opc, sub) ||
      opc_is(opc, and) ||
      opc_is(opc, or) ||
      opc_is(opc, xor) ||
      opc_is(opc, select) ||
      opc_is(opc, cmp) ||
      opc_is(opc, mul))) {
    TOP_CHECK(opnd1 >= 0);
    TOP_CHECK(opnd2 >= 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd1) > 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd2) > 0);
  }
  //if (opc_is(opc, not)) {
  //if (opnd1 < 0) fail;
  //if (opc_opnd_bits(opc,opnd1) <= 0) fail;
  //}
  if (opc_is(opc, move)) {
    TOP_CHECK(opnd1 >= 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd1) > 0);
  }
  if (opc_is(opc, sext)) {
    TOP_CHECK(opnd1 >= 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd1) > 0);
    TOP_CHECK(opc_opnd_signed(opc,opnd1));
  }
  if (opc_is(opc, zext)) {
    TOP_CHECK(opnd1 >= 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd1) > 0);
    TOP_CHECK(!opc_opnd_signed(opc,opnd1));
  }
  if (opc_is(opc, select)) {
    int condition = opc_ou_opnd_idx(opc, OU_condition);
    TOP_CHECK(condition >= 0);
    TOP_CHECK(opnd1 >= 0);
    TOP_CHECK(opnd2 >= 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd1) > 0);
    TOP_CHECK(opc_opnd_bits(opc,opnd2) > 0);
  }
  if (opc_is(opc, load)) {
    int base = opc_ou_opnd_idx(opc, OU_base);
    int offset = opc_ou_opnd_idx(opc, OU_offset);
    TOP_CHECK(base >= 0);
    TOP_CHECK(offset >= 0);
    TOP_CHECK(opc_opnd_bits(opc,base) > 0);
    TOP_CHECK(opc_opnd_bits(opc,offset) > 0);
  }
  if (opc_is(opc, store)) {
    int base = opc_ou_opnd_idx(opc, OU_base);
    int offset = opc_ou_opnd_idx(opc, OU_offset);
    int storeval = opc_ou_opnd_idx(opc, OU_storeval);
    TOP_CHECK(base >= 0);
    TOP_CHECK(offset >= 0);
    TOP_CHECK(storeval >= 0);
    TOP_CHECK(opc_opnd_bits(opc,base) > 0);
    TOP_CHECK(opc_opnd_bits(opc,offset) > 0);
    TOP_CHECK(opc_opnd_bits(opc,storeval) > 0);
    if (!opc_is(opc, multi))
      TOP_CHECK(opc_opnd_bits(opc,storeval) == TOP_Mem_Bytes(opc)*8);
  }

  return 1;
}

