
/* ====================================================================
 * ====================================================================
 *
 * Module: properties.h
 *
 * CGIR semantic properties on opcodes.
 *
 * Defines functions Fold_*: 
 * 	Each function applies the opcode property to the given constant operands.
 *
 * 	For instance:
 * 		intm_t Fold_OP_iadd(opc_t opc, opnd_t *opnds)
 * 	returns the folded constant for an opcode that have the iadd property.
 *
 * TOP_fetch_opnd
 * 	extract a constant operand from the top.
 *
 * Debugging functions:
 * 		int Check_OP_properties(opc_t opc)
 * 	checks that for a given opcode, the opcode properties and operand properties
 *	satisfy the constraints on properties.
 * ====================================================================
 * ====================================================================
 */

#ifndef properties_INCLUDED
#define properties_INCLUDED

#include "defs.h"
#include "topcode.h"
#include "tn.h"

int TOP_check_properties(TOP opc);
INT64 TOP_fetch_opnd(TOP opc, const TN * const opnds[], int opnd);
INT64 TOP_fold_iadd(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_isub(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_imax(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_imin(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_imul(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_not(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_and(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_or(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_xor(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_shl(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_shr(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_shru(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_move(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_zext(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_sext(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_select(TOP opc, const TN * const opnds[]);
INT64 TOP_fold_icmp(TOP opc, const TN * const opnds[]);

#endif
