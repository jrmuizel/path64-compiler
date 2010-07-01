//-*-c++-*-
/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/
/** 
 * @file   cg_tailmerge.cxx
 * @author Quentin COLOMBET
 * @date   Thu Feb  1 08:41:30 2007
 * @brief  Contains the targetting of functions required for tailmerge algorithm
 *         as defined in commom/com/tailmerge.h file. This targetting supplies
 *         the facilities to apply this algorithm to the CGIR representation.
 */

#include <map>                 // For map usage
#include <set>                 // For set usage

#include "defs_exported.h"     // For common type definition (needed by op.h)
#include "op.h"                // For operation usage
#include "bb.h"                // For basicblock usage
#include "errors.h"            // For DevAssert
#include "defs.h"              // Needed by mempool.h for MEM_PTR definition
#include "dwarf.h"             // For dwarf operand definition
#include "mempool.h"           // For MEM_POOL definition
#include "cxx_memory.h"        // For call to CXX_NEW/DELETE_ARRAY
#include "tracing.h"           // For tracing
#include "cg_flags.h"          // For CG_tailmerge
#include "cg.h"                // For Check_for_Dump
#include "label_util.h"        // For Get_Label_BB
#include "tn.h"                // For TN usage
#include "cg_tailmerge.h"
#include "tailmerge.h"         // For tailmerge algorithm
#include "cgtarget.h"          // For target specific function
#include "gra_live.h"          // For GRA_LIVE_Recalc_Liveness
#include "hb_sched.h"          // For scheduler usage

static bool Trace_Tailmerge = false;

typedef std::map<int, BBKIND> KindOfBB;
typedef KindOfBB::const_iterator CItKindOfBB;

typedef std::set<BB*> SetOfBBs;
typedef SetOfBBs::const_iterator CItSetOfBBs;

/**
 * Map a basic block to its kind. This mapping is used during tailmerge
 * algorithm to access true kind of a basic block. We store this information
 * because BB_kind determines dynamically the kind of a basic block, which may
 * not be possible during tailmerge.
 */
static KindOfBB g_saveOfBBKind;

//------------------------------------------------------------------------------
// Tailmerge functions to be targeted declaration
//------------------------------------------------------------------------------
namespace TAILMERGE_NAMESPACE
{

/**
 * @see tailmerge.h
 */
template<>
bool
IsEmpty<BB>(const BB& a_bb);

/**
 * @see tailmerge.h
 */
template<>
BB*
InvalidBasicBlock<BB>();

/**
 * @see tailmerge.h
 */
template<>
bool
AreEquivalent<OP>(OP* op1, OP* op2);

/**
 * @see tailmerge.h
 */
template<>
OP*
GetLastOp<BB, OP>(BB& bb);

/**
 * @see tailmerge.h
 */
template<>
void
DumpOperation<OP>(FILE* a_file, OP* op);

/**
 * @see tailmerge.h
 */
template<>
int
BasicBlockId<PU, BB>(const PU& pu, const BB& bb);

/**
 * @see tailmerge.h
 */
template<>
void
ReplaceJump<PU, BB>(PU& pu, BB& src, BB& tgt, BB& origBb,
                      bool jumpHere);

/**
 * @see tailmerge.h
 */
template<>
void
AddGoto<PU, BB>(PU& pu, BB& src, BB& tgt, bool forExplicit);

/**
 * @see tailmerge.h
 */
template<>
bool
IsJump<PU, BB, OP>(const OP* op, const PU*, const BB*);

/**
 * @see tailmerge.h
 */
template<>
void
RemoveBBs<PU, CNode<BB, OP>::BasicBlocks>(PU& a_pu, CNode<BB, OP>::BasicBlocks& a_toRemove, bool isEasy);

/**
 * @see tailmerge.h
 */
template<>
void
RemoveOp<PU, BB, OP>(PU& a_pu, BB& a_bb, OP* op);

/**
 * @see tailmerge.h
 */
template<>
void
AppendOp<PU, BB, OP>(PU& a_pu, BB& a_bb, OP* op);

/**
 * @see tailmerge.h
 */
template<>
void
GetBasicBlocksList<PU, BB>(std::list<BB*>& listOfBBs, PU& a_pu);

/**
 * @see tailmerge.h
 */
template<>
void
GetPredecessorsList<PU, BB>(std::list<BB*>& listOfPreds, PU& a_cfg,
                              const BB& a_bb);

/**
 * @see tailmerge.h
 */
template<>
BB*
GenAndInsertBB<PU, BB>(PU& a_cfg, BB& a_bb, bool bBefore);

/**
 * @see tailmerge.h
 */
template<>
void
GetExitBasicBlocks<PU, BB>(std::list<BB*>& exitBBs, PU& a_cfg);

/**
 * @see tailmerge.h
 */
template<>
bool
IsSimpleBB<PU, BB>(const PU& a_cfg, BB& bb);

/**
 * @see tailmerge.h
 */
template<>
bool
IsEntryBB<PU, BB>(const PU& a_cfg, BB& bb);

/**
 * @see tailmerge.h
 */
template<>
bool
ReplaceSimpleJump<PU, BB>(PU& a_cfg, BB& src, BB& tgt, BB& origBb);
} // End TAILMERGE_NAMESPACE

//------------------------------------------------------------------------------
// Functions declared in this files
//------------------------------------------------------------------------------
static void
InitializeTailmerge(PU& pu);

static void
FinalizeTailmerge(PU& pu);

static bool
IsUnconditionalJump(OP* op);

//------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------
/**
 * Get basic block from jump operation.
 *
 * @param  a_jump Jump operation
 *
 * @pre    a_jump is a jump operation
 * @post   result is the target basic block of a_jump operation
 *
 * @return The target of the jump operation
 */
static BB*
GetBBFromJump(const OP* a_jump)
{
    DevAssert((TAILMERGE_NAMESPACE::IsJump<PU, BB, OP>(a_jump)),
              ("Given operation is not a jump one"));
    TN* tgt = OP_opnd(a_jump, OP_find_opnd_use(a_jump, OU_target));
    DevAssert(TN_is_label(tgt), ("Target is not a label!"));
    return Get_Label_BB(TN_label(tgt));
}

void
Tailmerge(INT phase)
{
    if(CG_tailmerge & phase)
        {
            MEM_POOL mempool;
            MEM_POOL_Initialize(&mempool, "Tailmerge optimization", TRUE);
            MEM_POOL_Push (&mempool);
            Check_for_Dump(TP_TAIL, NULL);
            InitializeTailmerge(Get_Current_PU());

            Trace_Tailmerge = Get_Trace(TP_TAIL, 1) == TRUE;
            
            {
                TAILMERGE_NAMESPACE::CExtendedTailmerge<PU, BB, OP>
                    tailmergeOpt(Get_Current_PU(), true, true, Trace_Tailmerge,
                                 true, &mempool, false);

                if(CG_simp_flow_in_tailmerge & phase)
                    tailmergeOpt.SimplifyControlFlowGraph();
                tailmergeOpt.Optimize();
            }

            FinalizeTailmerge(Get_Current_PU());
            
            MEM_POOL_Pop(&mempool);
            MEM_POOL_Delete(&mempool);
            Check_for_Dump(TP_TAIL, NULL);
        }
    // Perform a second call only to simplify generated control flow graph
    if(CG_tailmerge & phase)
        {
            MEM_POOL mempool;
            MEM_POOL_Initialize(&mempool, "Tailmerge optimization", TRUE);
            MEM_POOL_Push (&mempool);
            Check_for_Dump(TP_TAIL, NULL);
            InitializeTailmerge(Get_Current_PU());

            Trace_Tailmerge = Get_Trace(TP_TAIL, 1) == TRUE;
            
            {
                TAILMERGE_NAMESPACE::CExtendedTailmerge<PU, BB, OP>
                    tailmergeOpt(Get_Current_PU(), true, true, Trace_Tailmerge,
                                 true, &mempool, false);

                if(CG_simp_flow_in_tailmerge & phase)
                    tailmergeOpt.SimplifyControlFlowGraph();
            }

            FinalizeTailmerge(Get_Current_PU());
            
            MEM_POOL_Pop(&mempool);
            MEM_POOL_Delete(&mempool);
            Check_for_Dump(TP_TAIL, NULL);
        }
}

/**
 * Initialize structures of current tailmerge targeting.
 *
 * @param  pu Program unit to be tailmerged
 *
 * @pre    true
 * @post   g_saveOfBBKind is properly initialized
 *
 */
static void
InitializeTailmerge(PU& pu)
{
    g_saveOfBBKind.clear();

    BB* bb;
    for(bb = REGION_First_BB; bb; bb = BB_next(bb))
        {
            using namespace TAILMERGE_NAMESPACE;
            g_saveOfBBKind[BasicBlockId<PU, BB>(pu, *bb)] = BB_kind(bb);
        }
}

/**
 * Finalize PU after tailmerge processing.
 *
 * @param  pu Program unit tailmerged
 *
 * @pre    true
 * @post   pu can be in next compiler optimizations
 */
static void
FinalizeTailmerge(PU& pu)
{
    BB* bb;
    for(bb = REGION_First_BB; bb; bb = BB_next(bb))
        {
            if(BB_kind(bb) == BBKIND_GOTO && BB_succs(bb))
                {
                    using namespace TAILMERGE_NAMESPACE;
                    OP* branchOp = BB_branch_op(bb);
                    BB *target = BBLIST_item(BB_succs(bb));
                    if(!branchOp)
                        {
                            if(target && target != BB_next(bb))
                                {
                                    AddGoto<PU, BB>(pu, *bb, *target, false);
                                }
                        }
                    else if(IsUnconditionalJump(branchOp) && target && 
                            target == BB_next(bb))
                        {
                            RemoveOp<PU, BB, OP>(pu, *bb, branchOp);
                            BB_succs(bb) = NULL;
                            Target_Simple_Fall_Through_BB(bb, target);
                        }
                }
        }
    GRA_LIVE_Recalc_Liveness(NULL);
    g_saveOfBBKind.clear();
}


/**
 * Check whether given operation is an unconditional jump or not.
 *
 * @remarks If OP_bb(op) is not in a good shape (eg during tailmerge algorithm),
 * it must be set to NULL to avoid development warnings.
 *
 * @param  op Operation to be checked
 *
 * @pre    op <> NULL
 * @post   true
 *
 * @return true is op is an unconditional jump, false otherwise
 */
static bool
IsUnconditionalJump(OP* op)
{
    int opndIdx;
    return TAILMERGE_NAMESPACE::IsJump<PU, BB, OP>(op) &&
        ((OP_bb(op) && BB_kind(OP_bb(op)) == BBKIND_GOTO) ||
         (((opndIdx = OP_find_opnd_use(op,OU_predicate)) < 0 ||
           OP_opnd(op, opndIdx) == True_TN) &&
          ((opndIdx = OP_find_opnd_use(op,OU_condition)) < 0 ||
           OP_opnd(op, opndIdx) == True_TN)));
}


//------------------------------------------------------------------------------
// Tailmerge functions to be targeted definition
//------------------------------------------------------------------------------
namespace TAILMERGE_NAMESPACE
{

template<>
bool
IsEmpty<BB>(const BB& a_bb)
{
    return BB_length(&a_bb) == 0;
}

template<>
BB*
InvalidBasicBlock<BB>()
{
    return (BB*)NULL;
}

template<>
bool
AreEquivalent<OP>(OP* op1, OP* op2)
{
    bool result = op1 == op2;
    if(!result && op1 && op2 && OP_code(op1) == OP_code(op2)
       && OP_code(op1) != TOP_asm)
        {
            if(IsJump<PU, BB, OP>(op1))
                {
                    TN *tgt1, *tgt2;
                    DevAssert(OP_find_opnd_use(op1, OU_target) != -1,
                              ("IsJump specification may be wrong"));
                    tgt1 = OP_opnd(op1, OP_find_opnd_use(op1, OU_target));
                    tgt2 = OP_opnd(op2, OP_find_opnd_use(op2, OU_target));
                    if(!TN_is_label(tgt1) || !TN_is_label(tgt2)) return false;
                    result = Get_Label_BB(TN_label(tgt1)) == 
                        Get_Label_BB(TN_label(tgt2)) &&
                        TN_offset(tgt1) == TN_offset(tgt2);
                    int i;
                    for(i = 0; i < OP_results(op1) && result; ++i)
                        {
                            result &= TN_equiv(OP_result(op1, i),
                                              OP_result(op2, i));
                        }
                    for(i = 0; i < OP_opnds(op1) && result; ++i)
                        {
                            if(OP_opnd(op1, i) == tgt1 ||
                               OP_opnd(op2, i) == tgt2)
                                {
                                    continue;
                                }
                            result &= TN_equiv(OP_opnd(op1, i), OP_opnd(op2, i));
                        }
                    // BB_kind may be invalid, so temporary removes the
                    // reference for the test
                    BB* saved = OP_bb(op1);
                    op1->bb = NULL;
                    result &= IsUnconditionalJump(op1);
                    op1->bb = saved;
                }
            else
                {
                  int i;
                  result = true;

                  /* tailmerge must not be activated on conditional
                   * transfert operations (like jrgtudec on xp70)
                   */
                  int opndIdx;
                  if (
                      (OP_xfer(op1) &&
                      (((opndIdx = OP_find_opnd_use(op1,OU_predicate)) >= 0 &&
                        OP_opnd(op1, opndIdx) != True_TN) ||
                       ((opndIdx = OP_find_opnd_use(op1,OU_condition)) >= 0 &&
                        OP_opnd(op1, opndIdx) != True_TN))) ||
                      (OP_xfer(op2) &&
                      (((opndIdx = OP_find_opnd_use(op2,OU_predicate)) >= 0 &&
                        OP_opnd(op2, opndIdx) != True_TN) ||
                       ((opndIdx = OP_find_opnd_use(op2,OU_condition)) >= 0 &&
                        OP_opnd(op2, opndIdx) != True_TN)))
                      ) {
                    result=false;
                  }
                  for(i = 0; i < OP_results(op1) && result; ++i)
                    {
                      result &= TN_equiv(OP_result(op1, i),
                                         OP_result(op2, i));
                    }
                  for(i = 0; i < OP_opnds(op1) && result; ++i)
                    {
                      result &= TN_equiv(OP_opnd(op1, i), OP_opnd(op2, i));
                    }
                  // VL 2008-09-16: we must check that instructions guarded by same guard pair
                  // have the same predicate too - fix for codex bug #51929
                  if(result && OP_has_predicate(op1))
                    {
                            result &= (OP_Pred_False(op1, OP_find_opnd_use(op1, OU_predicate)) ==
                                       OP_Pred_False(op2, OP_find_opnd_use(op2, OU_predicate)));
                    }
                }
        }
    return result;
}

template<>
void
RemoveBBs<PU, CNode<BB, OP>::BasicBlocks>
 (PU& a_pu, CNode<BB, OP>::BasicBlocks& a_toRemove, bool isEasy)
{
    if(isEasy)
        {
            CNode<BB, WN>::ItBasicBlocks it;
            for(it = a_toRemove.begin(); it != a_toRemove.end(); ++it)
                {
                    while(BB_preds(*it))
                        {
                            BB* tmp = BBLIST_item(BB_preds(*it));
                            Unlink_Pred_Succ(tmp, *it);
                        }

                    while(BB_succs(*it))
                        {
                            BB* tmp = BBLIST_item(BB_succs(*it));
                            Unlink_Pred_Succ(*it, tmp);
                        }
                    Remove_BB(*it);
                }
        }
    // Too complicate
}

template<>
bool
IsJump<PU, BB, OP>(const OP* op, const PU* a_pu, const BB* tgt)
{
    // Warning, not the same meaning as OP_xfer && OP_cond
    // Simple jump are mergeable
    return CGTARG_Is_Simple_Jump(op);
}

template<>
void
AddGoto<PU, BB>(PU& pu, BB& src, BB& tgt, bool forExplicit)
{
    Add_Goto(&src, &tgt);
    if(BB_Find_Succ(&src, &tgt))
        {
            if(!forExplicit)
                {
                    // src to tgt edge is unconditional
                    BBLIST_prob(BB_Find_Succ(&src, &tgt)) = 1.0F;
                }
            else
                {
                    // remove goto frequency
                    BBLIST_prob(BB_Find_Succ(&src, &tgt)) -= 1.0F;
                }
        }
    OP* opGoto = BB_last_op(&src);
    if(OP_prev(opGoto))
        {
            // Schedule unconditional jump instruction
            OP_scycle(opGoto) = OP_scycle(OP_prev(opGoto)) + 1;
        }
    else
        {
            OP_scycle(opGoto) = 1;
        }
}

template<>
void
ReplaceJump<PU, BB>(PU& pu, BB& src, BB& tgt, BB& origBb,
                      bool jumpHere)
{
    DevAssert(!jumpHere, ("Not yet implemented"));
    if(BB_in_succs(&src, &origBb))
        {
            if(BB_in_succs(&src, &tgt))
                {
                    Unlink_Pred_Succ(&src, &origBb);
                }
            else
                {
                    Change_Succ(&src, &origBb, &tgt);
                }
        }
    // else link will be done by add goto
    AddGoto<PU, BB>(pu, src, tgt);
}

template<>
int
BasicBlockId<PU, BB>(const PU& pu, const BB& bb)
{
    return BB_id(&bb);
}

template<>
void
DumpOperation<OP>(FILE* a_file, OP* op)
{
    Print_OP_No_SrcLine(op);
}

template<>
OP*
GetLastOp<BB, OP>(BB& bb)
{
    return BB_last_op(&bb);
}

template<>
void
RemoveOp<PU, BB, OP>(PU& a_pu, BB& a_bb, OP* op)
{
    BB_Remove_Op(&a_bb, op);
    // keep original location block to avoid useless scheduling
    op->bb = &a_bb;
}

template<>
void
AppendOp<PU, BB, OP>(PU& a_pu, BB& a_bb, OP* op)
{
    // Here OP_bb(op) contains the original basic block of op
    if(OP_bb(op) != &a_bb)
        {
            // Content of a_bb will change so invalidate scheduling
            Reset_BB_scheduled(&a_bb);
            Reset_BB_scheduled_hbs(&a_bb);

            // Set new arcs
            if(IsJump<PU, BB, OP>(op))
                {
                    BB* tgt = GetBBFromJump(op);
                    Link_Pred_Succ(&a_bb, tgt);
                    if(BB_Find_Succ(&a_bb, tgt))
                        {
                            // remove reference to origin bb
                            op->bb = NULL;
                            if(IsUnconditionalJump(op) &&
                               !BBLIST_prob(BB_Find_Succ(&a_bb, tgt)))
                                {
                                    // src to tgt edge is unconditional
                                    BBLIST_prob(BB_Find_Succ(&a_bb, tgt)) =
                                        1.0F;
                                }
                        }
                }
        }
    BB_Append_Op(&a_bb, op);
}

template<>
void
GetBasicBlocksList<PU, BB>(std::list<BB*>& listOfBBs, PU& a_pu)
{
    BB* bb;
    for(bb = REGION_First_BB; bb; bb = BB_next(bb))
        {
            listOfBBs.push_back(bb);
        }
}

template<>
void
GetPredecessorsList<PU, BB>(std::list<BB*>& listOfPreds, PU& a_cfg,
                              const BB& a_bb)
{
    BBLIST* bbList;
    FOR_ALL_BB_PREDS(&a_bb, bbList)
    {
        listOfPreds.push_back(BBLIST_item(bbList));
    }
}

template<>
BB*
GenAndInsertBB<PU, BB>(PU& a_cfg, BB& a_bb, bool bBefore)
{
    BB* fixedBb = &a_bb;
    CItKindOfBB it;
    if(bBefore && BB_prev(fixedBb) &&
       // Kind may be unknown since branch op may have been removed (temporary)
       // so, use "save" map
       ((it = g_saveOfBBKind.find(BasicBlockId<>(a_cfg, *BB_prev(fixedBb)))) ==
        g_saveOfBBKind.end() || it->second == BBKIND_LOGIF ||
        it->second == BBKIND_UNKNOWN))
        {
            // This should not happen, otherwise the g_saveOfBBKind has not
            // been updated for all created bb
            DevAssert(it != g_saveOfBBKind.end(), ("Problem during build of"
                                                   " information"));
            bBefore = false;
            while(BB_next(fixedBb))
                {
                    fixedBb = BB_next(fixedBb);
                }
        }
    BB* result = bBefore? Gen_And_Insert_BB_Before(fixedBb):
        Gen_And_Insert_BB_After(fixedBb);
    // By construction we create only goto BB.
    g_saveOfBBKind[BasicBlockId<PU, BB>(a_cfg, *result)] = BBKIND_GOTO;
    return result;
}


template<>
void
GetExitBasicBlocks<PU, BB>(std::list<BB*>& exitBBs, PU& a_cfg)
{
    BB_LIST* bbList;
    for(bbList = Exit_BB_Head; bbList; bbList = BB_LIST_rest(bbList))
        {
            exitBBs.push_back(BB_LIST_first(bbList));
        }
}

template<>
bool
IsSimpleBB<PU, BB>(const PU& a_cfg, BB& bb)
{
    // bb must not be part of a hwloop or something else
    bool result = BB_kind(&bb) == BBKIND_GOTO && !BB_Has_Addr_Taken_Label(&bb)
        && !BB_Has_Exc_Label(&bb);
    if(result)
        {
            OP* curStmt = BB_first_op(&bb);
            while(curStmt && result)
                {
                    result &= IsUnconditionalJump(curStmt);
                    curStmt = OP_next(curStmt);
                }
        }
    return result;
}

template<>
bool
IsEntryBB<PU, BB>(const PU& a_cfg, BB& bb)
{
    bool result = BB_entry(&bb);
    return result;
}

template<>
bool
ReplaceSimpleJump<PU, BB>(PU& a_cfg, BB& src, BB& tgt, BB& origBb)
{
    bool result = true;
    if(&tgt != &origBb)
        {
            OP* lastop = !IsEmpty<BB>(src)? GetLastOp<BB, OP>(src):
                NULL;
            if(BB_kind(&src) == BBKIND_GOTO)
                {
                    if(lastop && IsUnconditionalJump(lastop))
                        {
                            RemoveOp<PU, BB, OP>(a_cfg, src, lastop);
                        }
                    ReplaceJump<PU, BB>(a_cfg, src, tgt, origBb, false);
                }
            else if(lastop && BB_kind(&src) == BBKIND_LOGIF &&
                    // In this case, we are sure we are not on the not
                    // updatable fall through path
                    IsJump<PU, BB, OP>(lastop) &&
                    GetBBFromJump(lastop) == &origBb)
                {
                    TN *tgtLbl = Gen_Label_TN(Gen_Label_For_BB(&tgt),0);
                    Set_OP_opnd(lastop, OP_find_opnd_use(lastop, OU_target),
                                tgtLbl);
                    Change_Succ(&src, &origBb, &tgt);                    
                }
            else
                {
                    result = false;
                }
        }
    return result;
}
   
} // End namespace TAILMERGE_NAMESPACE

