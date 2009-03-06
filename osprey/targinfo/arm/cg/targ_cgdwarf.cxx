/*

Copyright (C) 2002 ST Microelectronics, Inc.  All Rights Reserved.

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

Contact information:

For further information regarding this notice, see:

*/


#include <stdio.h>
#include <stdlib.h>
// [HK]
#if __GNUC__ >= 3
#include <list>
using std::list;
#else
#include <list.h>
#endif // __GNUC__ >= 3

#include "wn.h"
#include "tn.h"
#include "op.h"
#include "data_layout.h"
#include "cgir.h"
#include "cgdwarf.h"               /* [CL] for CGD_LABIDX */
#include "cgdwarf_debug_frame.h"
#include "tracing.h" // For TFile definition
#include "config_TARG.h"
#include "targ_em_dwarf.h" // For debug identifier macro (DBGREG_SP, etc.)

// Uncomment the following define macro and add -Wb,-tt56:64 on your comment
// line to debug the unwind operation
#ifndef DEBUG_UNWIND
#   define DEBUG_UNWIND
#   define SHOULD_UNDEF_DEBUG_UNWIND
#endif

/**
 * Output use to print all debug trace.
 */
static FILE* debugOutput = TFile;

#ifdef DEBUG_UNWIND

/**
 * Print the given argument with a conditional call to fprintf.
 *
 * @param  args The arguments to be given to fprintf. Note that they must
 *         contain the parentheses
 *
 * @pre    true
 * @post   Trace_Unwind implies fprintf has been called with args arguments
 *
 * @remarks This function is expansed in nothing unless the DEBUG_UNWIND macro
 *          is defined
 */
#    define DbgPrintUnwind(args) DbgUnwind(fprintf, args)

/**
 * Conditionally assert the given argument.
 *
 * @param  args The arguments to be given to DevAssert. Note that they must
 *         contain the parentheses
 *
 * @pre    true
 * @post   Trace_Unwind implies DevAssert has been called with args arguments
 *
 * @remarks This function is expansed in nothing unless the DEBUG_UNWIND macro
 *          is defined
 */
#    define DbgAssertUnwind(args) DbgUnwind(DevAssert, args)

/**
 * Conditionally print a development warning with the given msg, if the check
 * condition is true.
 *
 * @param  check The condition to be checked
 * @param  msg The message to be printed in case of check argument is true
 *
 * @pre    true
 * @post   Trace_Unwind and check implies DevWarn has been called with msg
 *
 * @remarks This function is expansed in nothing unless the DEBUG_UNWIND macro
 *          is defined
 */
#    define DbgWarnUnwind(check, msg) DbgUnwind(if(check), DevWarn msg)

/**
 * Generic conditionnal 'action launcher' for debug purpose.
 *
 * @param  action Action to be performed. It should be a function name, the
 *         begin of a statement, etc. What is important is that action
 *         parameter followed by args parameter made a valid C/C++ statement.
 * @param  args Arguments to be given to the action
 *
 * @pre    true
 * @post   Trace_Unwind implies action has been executed with args arguments.
 *
 * @remarks This function is expansed in nothing unless the DEBUG_UNWIND macro
 *          is defined
 */
#    define DbgUnwind(action, args)             \
    {                                           \
        if(Trace_Unwind)                        \
            {                                   \
                action args;                    \
            }                                   \
    }
#else
#    define DbgPrintUnwind(args)
#    define DbgAssertUnwind(args)
#    define DbgWarnUnwind(check, msg)
#    define DbgUnwind(action, args)
#endif


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

/**
 * Size in bytes of a core register.
 *
 * @todo It should be great if we can retrieve this value directly from the isa
 */
static const INT SIZEOF_CORE_REG = 4;

/**
 * Index of the mask value operand in a push / pop instruction
 */
static const INT PUSH_POP_MASK_VALUE_OPND = 0;

/**
 * Index of the imediate value operand in a load immediate instruction.
 * Operand properties are not set for these instructions
 */
#define LOAD_IM_IM_VALUE_OPND(op) OP_code(op) == TOP_make? 1:           \
                       (OP_code(op) == TOP_more? 2: -1)


//-----------------------------------------------------------------------------
// Functions defined in this file
//-----------------------------------------------------------------------------
static INT
Get_Value_From_TN(BB* bb, OP* op, INT index);

static BOOL
Is_Load_Immediate(OP* op);

static BOOL
Is_Pop(OP* op);

static BOOL
Is_Push(OP* op);

// static BOOL
// Is_Store(OP* op);

// static BOOL
// Is_Load(OP* op);

static BOOL
Is_Move(OP* op);

static void
Analyze_Push_Pop(OP* op, UNWIND_ELEM& ue, UINT when);

static void
Compute_Push_Pop_Regs(OP* op, INT& spOffset, UINT when);

static DebugRegId
Get_Debug_Reg_Id(OP* op, INT32 index);

static UINT32
Get_Sizeof_Reg(OP* op, INT32 index);

static void
Initialize_UE(UNWIND_ELEM& ue);

static void
Init_Drop_These();

static BOOL
Is_AM_Automod(OP* op, BOOL&  post_mod, BOOL& inc_mod, BOOL& imm_mod);


//-----------------------------------------------------------------------------
// Functions definition
//-----------------------------------------------------------------------------

/**
 * Check whether given operation is a move one.
 *
 * @param  op [in] Operation to be checked
 *
 * @pre    op <> NULL
 * @post   result implies op can be considered as a move operation
 *
 * @return TRUE if op can be considered as a move operation, FALSE otherwise
 */
static BOOL
Is_Move(OP* op)
{
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP opc = OP_code(op);
    if (opc == TOP_or_i8) {
        TN* tn = OP_opnd(op, OP_find_opnd_use(op, OU_opnd2));
        return TN_has_value(tn) && TN_value(tn) == 0;
    }
#endif
    return FALSE;
}

/**
 * Check whether given operation is a push one.
 *
 * @param  op [in] Operation to be checked
 *
 * @pre    op <> NULL
 * @post   result implies op can be considered as a push operation
 *
 * @return TRUE if op can be considered as a push operation, FALSE otherwise
 */
static BOOL
Is_Push(OP* op)
{
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
    TOP opc = OP_code(op);
    return opc == TOP_pushrl || opc == TOP_pushrh || opc == TOP_pushsh;
#endif
    return FALSE;
}

/**
 * Check whether given operation is a pop one.
 *
 * @param  op [in] Operation to be checked
 *
 * @pre    op <> NULL
 * @post   result implies op can be considered as a pop operation
 *
 * @return TRUE if op can be considered as a pop operation, FALSE otherwise
 */
static BOOL
Is_Pop(OP* op)
{
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
    TOP opc = OP_code(op);
    return opc == TOP_poprl || opc == TOP_poprh || opc == TOP_popsh;
#endif
    return FALSE;
}

/**
 * Check whether given operation is an immediate load one.
 *
 * @param  op [in] Operation to be checked
 *
 * @pre    op <> NULL
 * @post   result implies op can be considered as an immediate load operation
 *
 * @return TRUE if op can be considered as a immediate load operation, FALSE
 *         otherwise
 */
static BOOL
Is_Load_Immediate(OP* op)
{
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
    TOP opc = OP_code(op);
    return opc == TOP_make || opc == TOP_more;
#endif
    return FALSE;
}

/**
 * Retrieve the sizeof register, which is  defined by index in op's operands.
 *
 * @param  op [in] Container of the TN
 * @param  index Index of TN in op's operands
 *
 * @pre    TN_is_register(OP_opnd(op, index)) and index < number of op's
 *         operands
 * @post   true
 *
 * @return The sizeof the register in bytes.
 *
 * @todo   This information should be retrieved from the isa, since extension
 *         registers may be bigger or smaller than core one.
 *
 * @warning Current implementation always return the sizeof core register
 */
static UINT32
Get_Sizeof_Reg(OP* op, INT32 index)
{
    return SIZEOF_CORE_REG;
}

// is 'op' a move from FP to SP ? (ie restore SP from FP)
static BOOL Restore_SP_From_FP(OP *op)
{
    if (Is_Move(op) && OP_Defs_TN(op,SP_TN)
        && (OP_opnd(op, OP_find_opnd_use(op, OU_opnd1)) == FP_TN)) {
        return TRUE;
    }
    return FALSE;
}

// is 'op' a move from xx to FP ? (ie restore FP from xx)
// or a restore of FP from the stack
static BOOL Restore_FP(OP *op)
{
    if (Is_Move(op) && OP_Defs_TN(op, FP_TN)) {
        return TRUE;
    }
    else if (OP_load(op) && (OP_result(op,0) == FP_TN)) {
        if (OP_findopnd(op, OU_base) == SP_TN) {
            return TRUE;
        }
    }
    return FALSE;
}

// is 'op' a move from SP to FP ? (ie define FP as equal to SP)
static BOOL Copy_SP_To_FP(OP *op)
{
    if (Is_Move(op) && OP_Defs_TN(op,FP_TN)
        && (OP_opnd(op, OP_find_opnd_use(op, OU_opnd1)) == SP_TN)) {
        return TRUE;
    }
    return FALSE;
}

// Needed because the structure has not a default constructor
static void
Initialize_UE(UNWIND_ELEM& ue)
{
    ue.kind = UE_UNDEFINED;
#ifdef PROPAGATE_DEBUG
    ue.valid = TRUE;
    ue.propagated = UE_PROP_NONE;
    ue.top_offset = 0;
    ue.is_copy = FALSE;
    ue.after_sp_adj = FALSE;
    ue.frame_size = 0;
    ue.handler = FALSE;
#endif
    ue.label_idx = LABEL_IDX_ZERO;
    ue.offset = 0;
}

static void Record_Register_Save(OP* op, INT opndnum, BB* bb, UNWIND_ELEM* ue,
                                 BOOL opnd_is_multi)
{
    TOP opc = OP_code(op);
    if (opndnum < 0) {
        DbgUnwind(Print_OP_No_SrcLine, (op));
    }
    FmtAssert(opndnum >= 0, ("incorrect operand id %d for %s", opndnum,
                             TOP_Name(opc)));

    TN* save_tn = Get_Copied_Save_TN(OP_opnd(op,opndnum), op, bb);
    if (save_tn) {
        opndnum = OP_find_opnd_use(op, OU_base);

        if (opndnum < 0) {
            DbgUnwind(Print_OP_No_SrcLine, (op));
        }
        FmtAssert(opndnum >= 0, ("no OU_base for %s", TOP_Name(opc)));

        if (TN_is_save_reg(save_tn)) {
            ue->rc_reg = Get_Debug_Reg_Id(save_tn);
        } else {
            ue->rc_reg = Get_Debug_Reg_Id(save_tn);
        }
        TN *store_tn = OP_opnd(op,opndnum);
        if (store_tn == SP_TN || store_tn == FP_TN) {
            ue->kind = (store_tn == SP_TN) ? UE_SAVE_SP : UE_SAVE_FP;

            TN* offset_tn = OP_opnd(op, OP_find_opnd_use(op, OU_offset));

            // don't record constant offsets, they are not related to
            // restores of callee-saved (they appear in array access for
            // instance)
            if (TN_is_symbol(offset_tn)) {
                ST* st = TN_var(offset_tn);
                ST* base_st;
                INT64 base_ofst;

                Base_Symbol_And_Offset(st, &base_st, &base_ofst);
                if (base_st != SP_Sym && base_st != FP_Sym) {
                    DbgUnwind(Print_OP_No_SrcLine, (op));
                }
                FmtAssert(base_st == SP_Sym || base_st == FP_Sym,
                          ("not saving to the stack!"));

                ue->offset = CGTARG_TN_Value (offset_tn, base_ofst);

                // handle the multi-operand case.
                if (OP_multi(op)) {
                    if (!opnd_is_multi) {
                        // 1st operand
                        if (Target_Byte_Sex == BIG_ENDIAN) {
                            ue->offset += 4;
                        }
                    } else {
                        // 2nd operand (opnd_is_multi)
                        if (Target_Byte_Sex == LITTLE_ENDIAN) {
                            ue->offset += 4;
                        }
                    }
                }
                DbgPrintUnwind
                    ((debugOutput,
                      "** %s register %lld offset = %lld\n", __FUNCTION__,
                      (long long)ue->rc_reg, ue->offset));
            } else {
                // Since we consider that constant offsets are not related
                // to unwind element, set current unwind element as
                // undefined
                ue->kind = UE_UNDEFINED;
                DbgPrintUnwind((debugOutput, "** Constant offset, unwind "
                                "element set as undefined\n"));
            }
        }
    } else {
        DbgPrintUnwind((debugOutput, "** %s no save tn\n", __FUNCTION__));
    }
}

static void Record_Register_Restore(OP* op, INT opndnum, BB* bb,
                                    UNWIND_ELEM* ue, BOOL opnd_is_multi)
{
    TOP opc = OP_code(op);

    if (opndnum < 0) {
        DbgUnwind(Print_OP_No_SrcLine, (op));
    }
    FmtAssert(opndnum >= 0, ("incorrect result id %d for %s", opndnum,
                             TOP_Name(opc)));

    TN* result_tn = OP_result(op,opndnum);

    if (TN_Is_Unwind_Reg(result_tn)) {
        opndnum = OP_find_opnd_use(op, OU_base);

        if (opndnum < 0) {
            DbgUnwind(Print_OP_No_SrcLine, (op));
        }
        FmtAssert(opndnum >= 0, ("no OU_base for %s", TOP_Name(opc)));

        TN *load_tn = OP_opnd(op,opndnum);
        if (load_tn == SP_TN || load_tn == FP_TN) {
            ue->kind = (load_tn == SP_TN) ? UE_RESTORE_SP : UE_RESTORE_FP;
            ue->rc_reg = Get_Debug_Reg_Id(result_tn);

            TN* offset_tn = OP_opnd(op, OP_find_opnd_use(op, OU_offset));

            // don't record constant offsets, they are not related to
            // restores of callee-saved (they appear in array access for
            // instance)
            if (TN_is_symbol(offset_tn)) {
                ST* st = TN_var(offset_tn);
                ST* base_st;
                INT64 base_ofst;

                Base_Symbol_And_Offset(st, &base_st, &base_ofst);
                if (base_st != SP_Sym && base_st != FP_Sym) {
                    DbgUnwind(Print_OP_No_SrcLine, (op));
                }
                FmtAssert(base_st == SP_Sym || base_st == FP_Sym,
                          ("not restoring from the stack!"));

                ue->offset = CGTARG_TN_Value (offset_tn, base_ofst);

                // handle the multi-operand case. 
                if (OP_multi(op)) {
                    if (!opnd_is_multi) {
                        // 1st operand
                        if (Target_Byte_Sex == BIG_ENDIAN) {
                            ue->offset += 4;
                        }
                    } else {
                        // 2nd operand (opnd_is_multi)
                        if (Target_Byte_Sex == LITTLE_ENDIAN) {
                            ue->offset += 4;
                        }
                    }
                }

                if (ue->kind != UE_UNDEFINED) {
                    DbgPrintUnwind
                        ((debugOutput, "** %s restore register %lld from mem "
                          "offset %lld\n", __FUNCTION__,
                          (long long)ue->rc_reg, ue->offset));
                }
            } else {
                // Idem save element: Since we consider that constant
                // offsets are not related to unwind element, set current
                // unwind element as undefined
                ue->kind = UE_UNDEFINED;
                DbgPrintUnwind((debugOutput, "** Constant offset, unwind "
                                "element set as undefined\n"));
            }
        }
    }
}

static void
AnalyzeAutomod(OP* op, BOOL incMod, UINT when)
{
    DbgUnwind(Print_OP_No_SrcLine, (op));
    if(OP_Defs_TN(op, SP_TN))  {
        INT val = Get_Value_From_TN(OP_bb(op), op,
                                    OP_find_opnd_use(op, OU_offset));
        UNWIND_ELEM ue;
        Initialize_UE(ue);
        ue.offset = val;
        ue.rc_reg = Get_Debug_Reg_Id(SP_TN);

        if(incMod) {
            // An adding is a reduction of stack size
            val = -val;
            ue.kind = UE_DESTROY_FRAME;
        } else {
            ue.kind = UE_CREATE_FRAME;
        }
        DbgPrintUnwind((debugOutput, "Found CFA offset modification %d\n",
                        val));
        Record_UE(op, &ue, OP_bb(op), when);
    }
}

void Analyze_OP_For_Unwind_Info (OP *op, UINT when, BB *bb)
{
    UNWIND_ELEM ue;
    Initialize_UE(ue);
    TN *tn;
    TOP opc = OP_code(op);
    INT opnd_idx;
    BOOL isAutomod = FALSE, postMod = FALSE, incMod = FALSE, immMod = FALSE;

    if (OP_simulated(op) || opc == TOP_asm) {
        has_asm = TRUE;
    }

    DbgPrintUnwind((debugOutput, "WHEN %d BB %d\n", when, BB_id(bb)));
    DbgUnwind(Print_OP_No_SrcLine, (op));

    ue.frame_changed = FALSE;
    if((isAutomod = Is_AM_Automod(op, postMod, incMod, immMod)) && !postMod) {
        // Op is a pre automod
        DbgPrintUnwind((debugOutput, "Find a pre op\n"));
        AnalyzeAutomod(op, incMod, when);
    }

    if ( OP_has_result(op)
         && ( OP_Defs_TN(op,SP_TN) || OP_Defs_TN(op,FP_TN) )) {

        // several cases need to be handled:
        // 1. normal one: definition of SP/FP with add/sub constant or register
        // 2. if FP is defined, any insn that modifies SP is to be ignored:
        //    it only serves to reserve space on the stack but cannot be used
        //    to compute the CFA
        // 3. exception to the previous rule: when SP is restored from FP
        // 4. restore of FP: ignore here, handled below

        // case 2: if FP is already defined, ignore this redefinition of SP
        // [CL] Under STxP70, there seems to be no "case 2"
        /*        if (PU_has_FP && !PU_has_restored_FP && !Restore_SP_From_FP(op)) {
                  ue.kind = UE_UNDEFINED;

                  // case 3:
                  } else */
        if (PU_has_FP && !PU_has_restored_FP && Restore_SP_From_FP(op)) {
            DbgPrintUnwind((debugOutput, "** %s destroy fp\n", __FUNCTION__));

            ue.kind = UE_DESTROY_FP;
            ue.rc_reg =
                Get_Debug_Reg_Id(CLASS_REG_PAIR_sp);
            PU_has_restored_FP = TRUE;

            // case 4:
        } else if ( (!PU_has_FP || PU_has_restored_FP) && Restore_FP(op)
                    && !Copy_SP_To_FP(op) ) {
            // a copy of SP to FP is considered as a definition of FP,
            // to be handled as case 1 below
            ue.kind = UE_UNDEFINED;

            // case 1:
        } else {
            OP* frame_op = NULL;
            if (OP_opnds(op) > 1 && ( OP_isub(op) || OP_iadd(op) ) ) {
                ue.offset =
                    Get_Value_From_TN(bb, op, OP_find_opnd_use(op, OU_opnd2));
                goto case1_OK;
            } else {
                // FP/SP is defined as a move from another reg
                if (Is_Move(op)) {
                    ue.offset = 0;
                    goto case1_OK;
                }
            }

            DbgUnwind(Print_OP_No_SrcLine, (op));
            if (frame_op) DbgUnwind(Print_OP_No_SrcLine, (frame_op));

            if(!isAutomod && (PU_has_FP && OP_Defs_TN(op,FP_TN))) {
                DevWarn("unwind: unable to compute frame size");
            }

        case1_OK:
            if (OP_Defs_TN(op,SP_TN)) {
                ue.rc_reg =
                    Get_Debug_Reg_Id(CLASS_REG_PAIR_sp);
      
                if ( ( ue.offset > 0 && OP_isub(op) )
                     || ( ue.offset < 0 && OP_iadd(op) )
                     ) {
                    // SP is decreased
                    DbgPrintUnwind((debugOutput, "** %s create frame\n",
                                    __FUNCTION__));

                    ue.kind = UE_CREATE_FRAME;
                } else if ( ( ue.offset < 0 && OP_isub(op) )
                              || ( ue.offset > 0 && OP_iadd(op) )
                            ) {
                    // SP is increased
                    DbgPrintUnwind((debugOutput, "** %s destroy frame\n",
                                    __FUNCTION__));
                    ue.kind = UE_DESTROY_FRAME;
                } else {
                    DbgUnwind(Print_OP_No_SrcLine, (op));
                    if(!(PU_has_FP || isAutomod)) {
                        // Currently does not set an assertion failure, because
                        // dynamic frame are not proceed
                        DevWarn("unwind: unsupported operation on SP");
                    }
                }
                
                DbgPrintUnwind((debugOutput, "** %s change SP size %lld\n",
                                __FUNCTION__, ue.offset));

            } else if (OP_Defs_TN(op,FP_TN) && 0 /* SEE NEXT NOTE */) {
                // NOTE:This analysis is wrong. So, since on stxp70 dynamic
                // frames are not used, disable it.

                // FP must be defined as an offset from SP
                if (OP_opnd(op, OP_find_opnd_use(op, OU_opnd1)) == SP_TN) {
                    if(OP_iadd(op) || OP_isub(op) || Is_Move(op)) {
                        ue.rc_reg =
                            Get_Debug_Reg_Id(CLASS_REG_PAIR_fp);
                        if ( ( ue.offset >= 0 && OP_iadd(op) )
                             || ( ue.offset <= 0 && OP_isub(op) )
                             || ( ue.offset == 0 && Is_Move(op) )
                             ) {
                            // FP is higher than SP
                            DbgPrintUnwind((debugOutput, "** %s create fp\n",
                                            __FUNCTION__));

                            ue.kind = UE_CREATE_FP;
                            PU_has_FP = TRUE;
                        }
                        // FP destruction is handled above
                        DbgPrintUnwind((debugOutput,
                                        "** %s change FP size %lld\n",
                                        __FUNCTION__, ue.offset));
                    } else {
                        FmtAssert(FALSE,
                                  ("unwind: FP not defined relative to SP"));
                    }
                } else {
                    DbgUnwind(Print_OP_No_SrcLine, (op));
                    // we need a flag to  know whether current SP definition
                    // is all right for debug information or not.
                    DevWarn("unwind: If PU uses FP, debug information are "
                            "wrong");
                }
            }
            ue.offset = abs(ue.offset);
        }
    }

    if (ue.kind != UE_UNDEFINED) 
        ;	// already found
    else if (OP_store(op)) {
        DbgPrintUnwind((debugOutput, "** %s store\n", __FUNCTION__));

        // find def of storeval and see if it is copy of save reg.
        // also check if base comes from spill symbol.
        INT opndnum = OP_find_opnd_use(op, OU_storeval);
        Record_Register_Save(op, opndnum, bb, &ue, FALSE);

        if (OP_multi(op)) {
            // if there are 2 inputs, record the 1st one, and prepare the next one
            Record_UE(op, &ue, bb, when);

            opndnum = OP_find_opnd_use(op, OU_multi);
            Record_Register_Save(op, opndnum, bb, &ue, TRUE);
        }
    } else if (OP_load(op)) {
        DbgPrintUnwind((debugOutput, "** %s load\n", __FUNCTION__));

        // check if we are loading a callee saved
        // from the stack
        INT opndnum = 0;
        Record_Register_Restore(op, opndnum, bb, &ue, FALSE);

        if (OP_multi(op)) {
            // if there are 2 results, record the 1st one, and prepare the next one
            Record_UE(op, &ue, bb, when);

            opndnum = OP_find_result_with_usage(op, OU_multi);
            Record_Register_Restore(op, opndnum, bb, &ue, TRUE);
        }
    } else if ((opnd_idx = OP_Copy_Operand(op)) != -1 &&
               TN_is_register(OP_opnd(op, opnd_idx)) &&
               // Some register classes are not supported in debug information,
               // like gr classes for instance.
               ShouldGenerateInformation(TN_register_class(OP_opnd(op,
                                                                   opnd_idx)),
                                         Get_Current_PU())) {
        DbgPrintUnwind((debugOutput, "** %s copy\n", __FUNCTION__));

        TN* copy_tn = OP_opnd(op, opnd_idx);
        TN* result_tn = OP_result(op, OP_Copy_Result(op));
      
        // check that we are copying a callee saved,
        // and that the destination is not a dedicated
        // there is a special case for FP, which is removed
        // from the callee_save register set when used in a PU
        if ( (TN_Is_Unwind_Reg(copy_tn) || copy_tn==FP_TN)
             && TN_is_register(result_tn)
             && !TN_is_dedicated(result_tn)) {
            ue.kind = UE_SAVE_GR;
            ue.rc_reg = Get_Debug_Reg_Id(copy_tn);
            ue.save_rc_reg = Get_Debug_Reg_Id(result_tn);

            // or check that we are restoring a callee-saved,
            // not from a dedicated
        } else if ( (TN_Is_Unwind_Reg(result_tn) || result_tn==FP_TN)
                    && TN_is_register(copy_tn)
                    && !TN_is_dedicated(copy_tn)) {
            ue.kind = UE_RESTORE_GR;
            ue.rc_reg = Get_Debug_Reg_Id(result_tn);
            ue.save_rc_reg = Get_Debug_Reg_Id(copy_tn);
        }
    } else if(Is_Push(op) || Is_Pop(op)) {
        Analyze_Push_Pop(op, ue, when);
        // push/pop are only relative to SP, so even if FP was
        // defined, if we encountered a push or a pop, it means the
        // reference is SP
        PU_has_FP = FALSE;
    }
    Record_UE(op, &ue, bb, when);

    if(isAutomod && postMod) {
        // Op is a post automod
        DbgPrintUnwind((debugOutput, "Find a post op\n"));
        AnalyzeAutomod(op, incMod, when);
    }
}

static void
Analyze_Push_Pop(OP* op, UNWIND_ELEM& ue, UINT when)
{
    INT spOffset = 0;
    // push/pop instructions are like a bundle of store/load instructions
    // followed by a sub/add. More precisly, it is like several load/store
    // with pre decrement/increment automod addressing mode
    Compute_Push_Pop_Regs(op, spOffset, when);
    // Whatever the instruction, SP is modified
    ue.kind = Is_Push(op)? UE_CREATE_FRAME: UE_DESTROY_FRAME;
    ue.rc_reg = Get_Debug_Reg_Id(CLASS_REG_PAIR_sp);
    ue.offset = spOffset;
    DbgPrintUnwind((debugOutput, "emit push pop\n"));
}

static void
Compute_Push_Pop_Regs(OP* op, INT& spOffset, UINT when)
{
    DevAssert(Is_Pop(op) || Is_Push(op), ("%s works only on pop / push "
                                          "instructions", __FUNCTION__));
    INT mask = TN_value(OP_opnd(op, PUSH_POP_MASK_VALUE_OPND));
    INT i = 0;
    INT size_regs = 0;

    // First compute the size of the registers pushed/poped. Useful
    // when handling a pop, as the 1st register poped is at an offset
    // which is the number of registers being poped
    for(; mask; ) {
      if (mask & 1) {
          size_regs += Get_Sizeof_Reg(op, i);
      }
      mask >>= 1;
    }
    size_regs /= CodeAlignmentFactor(Get_Current_PU());

    mask = TN_value(OP_opnd(op, PUSH_POP_MASK_VALUE_OPND));
    for(; mask; ++i)
        {
            if(mask & 1)
                {
                    DebugRegId reg =
                        Get_Debug_Reg_Id(op, i);
                    UNWIND_ELEM ue;
                    Initialize_UE(ue);
                    spOffset += Get_Sizeof_Reg(op, i) /
                        CodeAlignmentFactor(Get_Current_PU());
                    if(Is_Push(op))
                        {
                            ue.kind = UE_SAVE_SP;
                            ue.offset = - spOffset;
                            DbgPrintUnwind((debugOutput,
                                            "push cfa offset %u\n", spOffset));
                        }
                    else
                        {
                            ue.kind = UE_RESTORE_SP;
                            ue.offset = size_regs - spOffset;
                            DbgPrintUnwind((debugOutput,
                                            "pop cfa offset %u\n", spOffset));
                        }
                    ue.rc_reg = reg;

                    if(CIEs[CIE_index(Get_Current_PU())].IsSaved(reg))
                        {
                            DbgPrintUnwind((debugOutput, "restore/save: %lld\n",
                                            (long long)reg));
                            Record_UE(op, &ue, OP_bb(op), when);
                        }
                }
            mask >>= 1;
        }
}

#include <elf.h>
#include <elfaccess.h>

void
Check_Dwarf_Rel(const Elf32_Rel &current_reloc)
{
  FmtAssert(REL32_type(current_reloc) == R_IA_64_DIR32MSB,
	    ("Unimplemented 32-bit relocation type %d",
	     REL32_type(current_reloc)));
}

void
Check_Dwarf_Rel(const Elf64_Rel &current_reloc)
{
  FmtAssert(REL64_type(current_reloc) == R_IA_64_DIR64MSB,
	    ("Unimplemented 64-bit relocation type %d",
	     REL64_type(current_reloc)));
}

void
Check_Dwarf_Rela(const Elf64_Rela &current_reloc)
{
  FmtAssert(FALSE,
	    ("Unimplemented 64-bit relocation type %d",
	     REL64_type(current_reloc)));
}

void
Check_Dwarf_Rela(const Elf32_Rela &current_reloc)
{
  FmtAssert(FALSE,
	    ("Unimplemented 32-bit relocation type %d",
	     REL32_type(current_reloc)));
}

/**
 * Get the constant value that is attached to TN, which is the operand of op at
 * 'index'. To get the constant value, we seek for the definition of the TN in
 * the basic block
 *
 * @param  bb [in] Basic block used for analysis 
 * @param  op [in] Start point of the definition seeking, if needed, and
 *         container of the TN
 * @param  index Index of examined TN in op's operands
 *
 * @pre    bb contains op and index < number of operand of op
 * @post   true
 *
 * @return The retrieve value
 *
 * @warning Current implementation break on a development assertion when we are
 *          not able to find the constant value. So, talk this function when
 *          you must find the constant value.
 */
static INT
Get_Value_From_TN(BB* bb, OP* op, INT index)
{
    INT result = 0;
    TN* tn = OP_opnd(op, index);
    if(TN_is_symbol(tn))
        {
            ST* st = TN_var(tn);
            INT64 base_ofst;

	    FmtAssert (Base_Offset_Is_Known (st), ("unknown offset in Get_Value_From_TN"));
	    base_ofst = Base_Offset (st);
            DbgPrintUnwind((debugOutput, "symbol\n"));
            DbgPrintUnwind((debugOutput, "offset %lld\n", base_ofst));
            result = base_ofst;
        }
    else if(TN_is_register(tn))
        {
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
            DbgPrintUnwind((debugOutput, "register\n"));
            bool redo = true;
            int nb = 0;
            while(redo)
                {
                    redo = false;
                    OP* defop = Find_Specific_OP_From_TN(bb, op, index,
                                                         Is_Load_Immediate);
                    if(defop)
                        {
                            DbgPrintUnwind((debugOutput, "found def\n"));
                            DbgUnwind(Print_OP_No_SrcLine, (defop));
                            INT tmp =
                                TN_value(OP_opnd(defop,
                                                 LOAD_IM_IM_VALUE_OPND(defop)));
                            tmp <<= (16 * nb);
                            result |= tmp;

                            if(OP_code(defop) == TOP_more)
                                {
                                    redo = true;
                                    op = defop;
                                    index = 1;
                                }
                        }
                    else
                        {
                            DevWarn("Unknow how to retrieve value from TN%d",
                                    TN_number(tn));
                        }
                    ++nb;
                }
#endif
        }
    else if(TN_is_constant(tn))
        {
            result = TN_value(tn);
            DbgPrintUnwind((debugOutput, "constant\n"));
        }
    else
        {
            DevAssert(FALSE, ("Unknow how to retrieve value from tn"));
        }
    return result;
}

/**
 * Find the defined operation of the TN defined by index in op's operands, that
 * matches the Check_Func constraints.
 *
 * @param  bb [in] Basic block used for the definition seeking
 * @param  op [in] Start point of the definition seeking, if needed, and
 *         container of the TN
 * @param  index Index of seeked TN in op's operands
 * @param  Check_Func [in] Pointer on a function used to check operation
 *         constraint
 *
 * @pre    bb contains op and index < number of op's operands
 * @post   result <> NULL implies Check_Func(result) = true
 *
 * @return The operation that defined related TN and that matches Check_Func
 *         constraints
 */
static OP*
Find_Specific_OP_From_TN(BB* bb, OP* op, INT index, BOOL (*Check_Func)(OP*))
{
    op = OP_Find_TN_Def_In_BB(op, OP_opnd(op, index));
    while(op && !Check_Func(op))
        {
            if(Is_Move(op))
                {
                  op = OP_Find_TN_Def_In_BB(op,
                                            OP_opnd(op,
                                                    OP_find_opnd_use(op,OU_opnd1)));
                }
            else
                {
                    op = NULL;
                }
        }
    return op;
}


static DebugRegId
Get_Debug_Reg_Id(OP* op, INT32 index)
{
   TOP opc = OP_code(op);
    DebugRegId debugRegId = index;
    // We assume debug identifier are a continus sequence for a given
    // instruction
    FmtAssert (FALSE, ("not yet implemented"));
#if 0

    if(opc == TOP_poprl || opc == TOP_pushrl)
        {
            if(index != 15)
                {
                    debugRegId = index + DBGREG_R0;
                }
            else
                {
                    debugRegId = DBGREG_GR;
                }
        }
    else if(opc == TOP_poprh || opc == TOP_pushrh)
        {
            debugRegId = index + DBGREG_R16;
        }
    else if(opc == TOP_popsh || opc == TOP_pushsh)
        {
            debugRegId = index + DBGREG_LCR0;
        }
#endif
    return debugRegId;
}


/**
 * Drop these list.
 * Contains the name of sections that should not be emitted in the assembler
 * file. This variable is initialized by Init_Drop_These() function.
 */
static list<const char*> drop_these;

/**
 * Debug info section name.
 */
static const char* debugInfoSection = ".debug_info";

/**
 * Debug line section name.
 */
static const char* debugLineSection = ".debug_line";

/**
 * Debug pubnames section name.
 */
static const char* debugPubnamesSection = ".debug_pubnames";

/**
 * Debug aranges section name.
 */
static const char* debugArangesSection = ".debug_aranges";

/**
 * Debug loc section name.
 */
static const char* debugLocSection = ".debug_loc";

/**
 * Debug abbrev section name.
 */
static const char* debugAbbrevSection = ".debug_abbrev";

/**
 * Debug varnames section name.
 */
static const char* debugVarnamesection = ".debug_varnames";

/**
 * Debug typenames section name.
 */
static const char* debugTypenamesSection = ".debug_typenames";

/**
 * Debug funcnames section name.
 */
static const char* debugFuncnamesSection = ".debug_funcnames";

/**
 * Initialized drop_these variable.
 * The initialization of this variable depends on the value of Full_Debug_Info
 * one. This one is set to true when all avaible debugging information should
 * be emitted. If set to false, only debug frame information should be emitted
 *
 * @pre    true
 * @post   drop_these contains the name of all sections that should not be
 *         emitted in the assembler file.         
 *
 */
static void
Init_Drop_These()
{
    if(!Full_Debug_Info)
        {
            // Note: We created variables instead of directly using the string,
            // to provide a way to resuse these strings, if needed.
            drop_these.push_back(debugInfoSection);
            drop_these.push_back(debugLineSection);
            drop_these.push_back(debugPubnamesSection);
            drop_these.push_back(debugArangesSection);
            drop_these.push_back(debugLocSection);
            drop_these.push_back(debugAbbrevSection);
            drop_these.push_back(debugVarnamesection);
            drop_these.push_back(debugTypenamesSection);
            drop_these.push_back(debugFuncnamesSection);
        }
}

/**
 * Specify whether the section that name is 'name' should be emitted in the
 * assembler. Names of sections, which should not be emitted, are stored in
 * drop_these variable.
 *
 * @param  name The name of the section to be emitted
 *
 * @pre    true
 * @post   true
 *
 * @return TRUE if section named 'name' should be emitted, FALSE otherwise
 */
extern BOOL Is_Dwarf_Section_To_Emit(const char *name)
{
    static bool isFirst = true;
    if(isFirst)
        {
            Init_Drop_These();
            isFirst = false;
        }
    list<const char*>::const_iterator it;
    BOOL result = TRUE;
	for(it = drop_these.begin(); it != drop_these.end(); ++it)
        {
            if(strcmp(name, *it) == 0)
                {
                    result = FALSE;
                    break;
                }
        }
    return result;
}

/**
 * Check whether or not offsets in dwarf sections should be generated
 * using symbolic data.
 *
 * @pre    true
 * @post   true
 * 
 * @return TRUE if it is required to generate symbolic offsets, FALSE otherwise
 */
extern BOOL Dwarf_Require_Symbolic_Offsets()
{
  return FALSE;
}

/**
 * Check whether given operation uses an automod addressing mode.
 *
 * @param  op [in] Operation to be checked
 * @param  post_mod [out] Specify if op is a post op, pre op otherwise
 * @param  inc_mod [out] Specify if op uses an increment op, decrement otherwise
 * @param  imm_mod [out] Specify if op uses an immediate as offset, register
 *         otherwise
 *
 * @pre    op->notEmpty()
 * @post   result implies post_mod, inc_mod and imm_mod have been set
 *
 * @return TRUE if op uses an automod addressing mode, FALSE otherwise
 */
static BOOL
Is_AM_Automod(OP* op, BOOL& post_mod, BOOL& inc_mod, BOOL& imm_mod)
{

#define TOP_AM_i(x)        \
        case TOP_ ## x ## _i5_post_inc: \
            inc_mod = TRUE; \
            post_mod = TRUE; \
            imm_mod = TRUE; \
            break; \
        case TOP_ ## x ## _i5_post_dec: \
            inc_mod = FALSE; \
            post_mod = TRUE; \
            imm_mod = TRUE; \
            break; \
        case TOP_ ## x ## _i5_pre_dec: \
            inc_mod = FALSE; \
            post_mod = FALSE; \
            imm_mod = TRUE; \
            break; \
        case TOP_ ## x ## _r_post_inc: \
            inc_mod = TRUE; \
            post_mod = TRUE; \
            imm_mod = FALSE; \
            break
   
    TOP top = OP_code(op);
    BOOL result = TRUE;

    FmtAssert (FALSE, ("not yet implemented"));
#if 0


    switch (top)
        {
            TOP_AM_i(fpx_flw);
            TOP_AM_i(fpx_fsw);
            TOP_AM_i(fpx_fswf2i);
            TOP_AM_i(lb);
            TOP_AM_i(lgs);
            TOP_AM_i(lh);
            TOP_AM_i(lsetub);
            TOP_AM_i(lsfr);
            TOP_AM_i(lub);
            TOP_AM_i(lw);
            TOP_AM_i(sb);
            TOP_AM_i(sh);
            TOP_AM_i(ssfr);
            TOP_AM_i(sw);
        default:
            result = FALSE;
        }

#endif
  return result;
}



#ifdef SHOULD_UNDEF_DEBUG_UNWIND
#   undef DEBUG_UNWIND
#endif
