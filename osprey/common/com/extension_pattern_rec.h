#ifndef EXTENSION_PATTERN_REC_H
#define EXTENSION_PATTERN_REC_H

#include "wintrinsic.h"
#include "opcode_gen_core.h"


/*
 * Whirl pattern recursive data structure
 * There are 5 kinds of pattern_desc elements
 *   1) NULL element:
 *         terminal value for the pattern.
 *   2) type==pattern_desc_imm_value : 
 *         immediate value of a whirl node (like the imm parameter in
 *         node OPC_INTCONST or node OPC_I4CVTL)
 *   3) type==pattern_desc_opc :
 *         a whirl node of opcode OPC which kids defined in kids[]
 *         array.
 *   4) type==pattern_desc_intrinsic_by_name :
 *         an intrinsic call defined by its name
 *   5) type==pattern_desc_intrinsic :
 *         an intrinsic call defined by its intrinsic ID.
 */

/*
 * Note that current code pattern recognition code limits use of
 * intrinsic patterns as terminal nodes.
 */

typedef enum {
  pattern_desc_imm_value,
  pattern_desc_opc,
  pattern_desc_intrinsic_by_name,
  pattern_desc_intrinsic,
} pattern_desc_type;
  
/* forward declaration */
struct pdesc;
typedef struct pdesc {
  pattern_desc_type type;
  union {
    int value;
    OPCODE opc;
    const char* builtin_name;
    INTRINSIC intrn;
  } u;
  struct pdesc** kids; // effective length is given
                       // by OPCODE/INTRINSIC semantic
} pattern_desc;


/* operand_desc structure specifies the content of an intrinsic
 * operand.
 * There are 3 kinds of operand_desc selected via type value:
 *   1) operand_desc_path : operand is a register whose value is
 * defined by the whirl node pointed by 'path' array in a pattern_desc
 *   2) operand_desc_sfr  : operand is an sfr register.
 *   3) operand_desc_imm  : operand is an immediate value.
 */

typedef enum {
  operand_desc_path,
  operand_desc_sfr,
  operand_desc_imm
} operand_desc_type;

typedef struct {
  operand_desc_type type;
  union {
    int sfr_rank;    // Rank in SFR register file
    int imm_value;   // Immediate value
    int length;          // effective length of path[] array
  } u;
  int* path; // operand access path thru pattern_desc
                       // empty if type!=operand_desc_path
} operand_desc;


/*
 * Main structure that store a full pattern description and its
 * corresponding intrinsic.
 */
typedef struct {
  INTRINSIC intrn;          // Intrn idx (dynamically extracted 
                            //  during extension loading)
  const char* builtin_name; // Used to initialize intrn idx
  long long flags;          // Flags used to selectively activate
                            //  recog rules depending on end-user options
  pattern_desc* pattern;    // Whirl pattern definition
  operand_desc** arg;       // Intrinsic operand definitions
} recog_rule;


#define RULE_FLAG_NONE       0
#define RULE_FLAG_FRAC     0x1
#define RULE_FLAG_DISABLED 0x2

#define Is_Rule_Flag_Set(rule, flg) (rule->flags & (flg))
#define Set_Rule_Flag(rule, flg)    (rule->flags |= (flg))
#define Clear_Rule_Flag(rule, flg)  (rule->flags &= (~(flg)))

#endif
