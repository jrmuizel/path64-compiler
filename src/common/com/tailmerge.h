//-*-c++-*-
/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/
/** 
 * @file   tailmerge.h
 * @author Quentin COLOMBET
 * @date   Tue Jan 16 08:31:30 2007
 * @brief  This file contains the implementation of a class which the tail
 *         merging optimization in a generic way (i.e. using template).
 *         This optimization has three parametrized parameters:
 *         @li CFG represents a control graph flow
 *         @li BasicBlock represents a basic basic block in that control flow
 *         @li Operation represents an operation in a basic block
 *         These parameters must match some operating contrains, which are
 *         defined in the description of the CTailmerge class
 *
 * @see    CTailmerge class
 *
 * @remarks When functions have the following todo 'Targetting to be done',
 *          this means that you must implement this function for your template
 *          arguments, otherwise the tail merging will not work/compile/link.
 */

#ifndef __TAILMERGE_H__
#define __TAILMERGE_H__

#   include <list>           // For list usage
#   include <set>            // For set usage
#   include <map>            // For map usage
#   include <stdio.h>        // For fprintf, FILE*

/**
 * Macro definition of the tailmerge namespace. This namespace is used for all
 * elements of this file
 */
#   define TAILMERGE_NAMESPACE TailmergeSpace

namespace TAILMERGE_NAMESPACE
{


/**
 * Define we want to keep the opportunity of trace generation. Keeping this
 * line reduces the effeciency of this optimization (in terms of compiler
 * speed), but allows easier debugging or verification.
 *
 * @remarks Unless DEBUG_TAILMERGE_CONDITION evaluates to true, traces are not
 *          printed out.
 */
#   ifdef Is_True_On
#      define DEBUG_TAILMERGE
#   else
#      undef DEBUG_TAILMERGE
#   endif

#   undef DEBUG_TAILMERGE_CONDITION
/**
 * Macro definition of the condition used to activate tracing
 */
#   define DEBUG_TAILMERGE_CONDITION Trace()

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
/**
 * Output use to print all debug trace.
 */
static FILE* debugOutput = TFile;

/**
 * Define whether we check or not invariants of the classes defined in this
 * file. Unless this variable is set to false, the effeciency of the compiler
 * (in terms of compiler speed) will be reduced.
 */
static const bool g_checkInvariant = true;

//-----------------------------------------------------------------------------
// Functions declaration
//-----------------------------------------------------------------------------
/**
 * Check whether given BasicBlock is empty or not.
 *
 * @param  a_bb BasicBlock to be checked
 *
 * @pre    a_bb is a valid basic block
 * @post   if result then a_bb is an empty BasicBlock else a_bb contains
 *         at least one instruction endif
 *
 * @return true if a_bb is empty, false otherwise
 *
 * @todo   Targetting to be done
 */
template<typename BasicBlock>
static bool
IsEmpty(const BasicBlock& a_bb);

/**
 * Getter for an invalid BasicBlock object.
 *
 * @pre    true
 * @post   true
 *
 * @return A BasicBlock that is considered (and known) as invalid.
 *
 * @remarks This function is used to check that a given BasicBlock is not
 *          invalid using the following pattern myBb != InvalidBasicBlock()
 *
 * @todo   Targetting to be done.
 */
template<typename Basiblock>
static Basiblock*
InvalidBasicBlock();

/**
 * Check whether given operations are equivalent.
 * Two operations are equivalent when one can be used for the other, i.e.
 * operation's parameters and behavior are the very same.
 *
 * @param  op1 [in] First operation to be checked
 * @param  op2 [in] Second operation to be checked
 *
 * @pre    true
 * @post   result implies op1 can be used as op2 and vice versa
 *
 * @return true if both operation are equivalent, false otherwise
 *
 * @remarks This function must not modified op1 and op2. However, we cannot
 *          set the const keyword because of some Open64 API!
 *
 * @todo   Targetting to be done
 */
template<typename Operation>
static bool
AreEquivalent(Operation* op1, Operation* op2);

/**
 * Check whether given operation is a jump one and optionnally on the given
 * basic block.
 * A jump operation is an instruction use to break the control flow (generally
 * to another basic block)
 *
 * @param  op Operation to be checked
 * @param  a_cfg Control graph flow
 * @param  tgt Optionnal basic block to be checked
 * 
 *
 * @pre    tgt <> NULL implies a_cfg <> NULL
 * @post   result = op is a jump operation and (tgt <> NULL implies op is a jump
 *         operation on tgt)
 *
 * @return true if op is a jump operation, false otherwise
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename Basiblock, typename Operation>
static bool
IsJump(const Operation* op, const Cfg* a_cfg = NULL,
       const Basiblock* tgt = NULL);

/**
 * Add a goto (unconditionnal jump) between a_src and a_tgt at the end of a_src.
 * The adding of this operation must update the structure of the given control
 * flow graph, so it remains coherent. For instance, the list of successors/
 * predecessors of a_src and a_tgt must be updated (if any).
 *
 * @param  a_cfg The control flow graph that contains a_src and a_tgt basic
 *         blocks
 * @param  a_src The basic block that will recieve the goto operation to a_tgt
 * @param  a_tgt The target basic block of the goto operation
 * @param  forExplicit Specify whether current goto is added to explicit an
 *         existing branch or not
 *
 * @pre    a_cfg contains a_src and a_tgt
 * @post   a_src's last operation is a jump to a_tgt
 *
 * @todo Targetting must be done
 */
template<typename Cfg, typename BasicBlock>
static void
AddGoto(Cfg& a_cfg, BasicBlock& a_src, BasicBlock& a_tgt,
         bool forExplicit = false);

/**
 * Replace a jump src to a_origBb by a jump src to tgt.
 * The aim of this jump replacement is not to replace a jump instruction to
 * another, but to update successors/predecessors lists and adding a goto
 * between src and tgt. The jump replacement is not expected because when this
 * functions is called, the list of operations of source basic block may not
 * contains the related jump, since it would be factoried. The jump src to tgt
 * can be either:
 * @li from the end of src to the beginning of tgt (jumpHere = false)
 * @li or from the end of src to the current end of tgt (jumpHere = true)
 *
 * @param  a_cfg Control flow graph that contains all given basic blocks
 * @param  src Source basic block of the jump operation to be replaced
 * @param  tgt New target basic block of the jump operation
 * @param  origBb Old target (original) basic block of the jump operation
 * @param  jumpHere Specify whether the target of the new jump is the beginning
 *         of tgt (jumpHere = false), or the end of tgt (thus, the after the
 *         last operation of tgt) (jumpHere = true) 
 *
 * @pre    a_cfg contains src, tgt and origBb
 * @post   src's last operation is a jump to tgt at the point specify by
 *         jumpHere and one reference to origBb has removed in
 *         succs list of src (if any) and one reference to tgt has been added
 *         to that list (if any) if this reference does not already exist and
 *         one reference to src has been added in preds list of tgt (if any)
 *         if this reference does not already exist
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static void
ReplaceJump(Cfg& a_cfg, BasicBlock& src, BasicBlock& tgt, BasicBlock& origBb,
            bool jumpHere);

/**
 * Return an unique identifier for given basic block.
 * The unicity of the identifier is defined by the following points:
 * @li Two calls to this function with the same parameters must return the same
 *     result.
 * @li Two calls to this function with the same cfg and a different basic block
 *     return different results.
 * @li Two calls to this function with different cfg and basic block could
 *     return the same results.
 *
 * @param  a_cfg Control flow graph that contains given bb
 * @param  bb Basic block for which we want the identifier
 *
 * @pre    a_cfg contains bb
 * @post   true
 *
 * @return The identifier of bb in a_cfg
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static int
BasicBlockId(const Cfg& a_cfg, const BasicBlock& bb);

/**
 * Dump given operation in given file.
 * This function is mainly used for debugging purpose
 *
 * @param  a_file File which will recieve the dumped operation
 * @param  op [in] Operation to be dump
 *
 * @pre    true
 * @post   op is printed in a_file
 *
 * @remarks op must not be modified. However, we have not set the const keyword
 *          because of Open64 API!
 *
 * @todo   Targetting to be done
 */
template<typename Operation>
static void
DumpOperation(FILE* a_file, Operation* op);

/**
 * Getter of last operation of a basic block.
 *
 * @param  bb [in] Considered basic block
 *
 * @pre    bb is not empty
 * @post   Operation <> NULL
 *
 * @return A pointer to last operation of bb
 *
 * @todo   Targetting to be done
 */
template<typename BasicBlock, typename Operation>
static Operation*
GetLastOp(BasicBlock& bb);

/**
 * Remove basic blocks from control graph flow.
 * The removal of the basic blocks may be complicate, since all given basic
 * blocks are empty, so the expected behavoir is: Remove that you easly can
 * from the control flow graph and reflect this removal in the given list.
 *
 * @param  a_cfg Control graph flow that contains all basic blocks of a_toRemove
 *         list
 * @param  a_toRemove [in/out] Contains the list of basic block that are no
 *         more used. After the function call, this list contains the basic
 *         blocks that have not been removed
 * @param  isEasy Specify whether the removal (if done) must update branch
 *         operations or not (isEasy == true). In the second case (isEasy is
 *         true) we expect only the removal of given basic block from the given
 *         a_cfg and from all predecessors/successors list without any checks.
 *
 * @pre    a_cfg contains all basic blocks of the a_toRemove list
 * @post   a_toRemove contains the list of the basic blocks that have not been
 *         removed
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlocks>
static void
RemoveBBs(Cfg& a_cfg, BasicBlocks& a_toRemove, bool isEasy = false);

/**
 * Remove given operation in given basic block.
 * The removal of the operation in a_bb is a simple disconnect of that operation
 * from a_bb, since op has been extracted from a_bb and must survive this
 * removal.
 *
 * @param  a_cfg Control graph flow that contains a_bb
 * @param  a_bb Basic block that contains op
 * @param  op Operation to be removed (disconnected) from a_bb
 *
 * @pre    a_cfg contains a_bb and a_bb contains op and a_bb is a valid basic
 *         block
 * @post   a_bb does not contain op anymore but op is still valid
 *
 * @todo   Targetting to be done
 *
 * @remarks op is generally the last operation of a_bb, so your targetting
 *          should traverse the operations in reverse order to be more effecient
 */
template<typename Cfg, typename BasicBlock, typename Operation>
static void
RemoveOp(Cfg& a_cfg, BasicBlock& a_bb, Operation* op);

/**
 * Append given operation to given basic block.
 * This function is expected tot update the preds/succs list if operation is a
 * jump
 *
 * @param  a_cfg Control flow graph
 * @param  a_bb Basic block that will recieve the operation
 * @param  op Operation to be added to a_bb. In fact this adding may be only a
 *         connecting in a_bb, we do not expect that this function copies the
 *         operation
 *
 * @pre    a_cfg contains a_bb and a_bb is a valid basic block
 * @post   a_bb's last operation = op and a_cfg is coherent with that
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock, typename Operation>
static void
AppendOp(Cfg& a_cfg, BasicBlock& a_bb, Operation* op);

/**
 * Get a list of all basic blocks that composed a_cfg.
 * The list must not contain duplicate
 *
 * @param  listOfBBs [out] Will contain the list of all (valid) basic blocks of
 *         a_cfg
 * @param  a_cfg Control flow graph to be considered
 *
 * @pre    true
 * @post   listOfBBs contains all (valid) basic blocks of a_cfg
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static void
GetBasicBlocksList(std::list<BasicBlock*>& listOfBBs, Cfg& a_cfg);

/**
 * Get the list of predecessors of a_bb in a_cfg.
 * The list must not contain duplicate
 *
 * @param  listOfPreds [out] Will contains the list of all (valid) predecessors
 *         basic blocks of a_bb in a_cfg
 * @param  a_cfg Control flow graph that contains a_bb
 * @param  a_bb Basic block to be considered
 *
 * @pre    a_cfg contains a_bb and a_bb is a valid basic block
 * @post   listOfPreds contains all (valid) predecessors basic blocks of a_bb
 *         in a_cfg
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static void
GetPredecessorsList(std::list<BasicBlock*>& listOfPreds, Cfg& a_cfg,
                    const BasicBlock& a_bb);

/**
 * Create a new basic block and insert it before a_bb.
 *
 * @param  a_cfg Control flow graph that contains a_bb and will contains the
 *         created basic block
 * @param  a_bb Basic block used as fixed point for the insertion
 * @param  bBefore Specify the position where created basic block must be set
 *         relatively to a_bb
 *
 * @pre    a_cfg contains a_bb and a_bb is a valid basic block
 * @post   a_cfg contains result and result is just before a_bb (bBefore = true)
 *         or after a_bb (bBefore = false)
 *
 * @return Created basic block
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static BasicBlock*
GenAndInsertBB(Cfg& a_cfg, BasicBlock& a_bb, bool bBefore);

/**
 * Get the list of exit basic blocks of given cfg.
 *
 * @param  exitBBs [out] Will contains the list of exit basic blocks of a_cfg
 * @param  a_cfg Control flow graph
 *
 * @pre    true
 * @post   exitBBs contains all exits blocks of a_cfg
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static void
GetExitBasicBlocks(std::list<BasicBlock*>& exitBBs, Cfg& a_cfg);

/**
 * Check whether given basic block is simple.
 * A basic block is simple when it does contain any useful instruction than a
 * simple goto to the next basic block. The goal of this function is to
 * determine which basic block may be useless, thus removable (after the
 * proper update of branching instructions)
 *
 * @param  a_cfg Control flow graph
 * @param  bb [in] Basic block to be checked
 *
 * @pre    true
 * @post   result = bb is simple
 *
 * @return true if bb is simple in a_cfg, false otherwise
 *
 * @remarks Due to API problem in targeting, we cannot set bb as const
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static bool
IsSimpleBB(const Cfg& a_cfg, BasicBlock& bb);

/**
 * Check whether given basic block is PU entry block.
 * Implementation can be based on specific BB kind or properties.
 *
 * @param  a_cfg Control flow graph
 * @param  bb [in] Basic block to be checked
 *
 * @pre    true
 * @post   result = bb is an entry block
 *
 * @return true if bb is entry, false otherwise
 *
 * @remarks Due to API problem in targeting, we cannot set bb as const
 *
 * @todo   Targetting to be done
 */

template<typename Cfg, typename BasicBlock>
static bool
IsEntryBB(const Cfg& a_cfg, BasicBlock& bb);

/**
 * Replace a simple jump (unconditionnal jump) from src to origBb by a jump
 * from src to tgt.
 * This function must correctly update predecessor/successor list of src, tgt
 * and origBb. Unlike ReplaceJump, this function call occurs when the cfg is in
 * a good shape (all instructions are at the right place), so the jump
 * replacement (i.e. the modification of the jump instruction of src) is
 * expected. The replacement must not occur if the jump instruction of src is
 * not simple. In that case, it must return false
 *
 * @param  a_cfg Control flow graph
 * @param  src Basic block source to be updated
 * @param  tgt Target of the new jump
 * @param  origBb Original target of the jump
 *
 * @pre    src, tgt and origBb are basic blocks of a_cfg
 * @post   src jump instruction target's is tgt and tgt.Preds contains src and
 *         src.Succs contains tgt and origBb.Preds =
 *         origBb.Preds->excluding(src)
 *
 * @return true if a simple jump replacement occured, false otherwise
 *
 * @todo   Targetting to be done
 */
template<typename Cfg, typename BasicBlock>
static bool
ReplaceSimpleJump(Cfg& a_cfg, BasicBlock& src, BasicBlock& tgt,
                  BasicBlock& origBb);

/**
 * Check whether two lists are the same.
 * Two list are considered as the same, when they have the same content in the
 * same order.
 * List typename is expected to have:
 * @li const_iterator sub classes
 * @li a operator== on each their items
 *
 * @param  l1 First list
 * @param  l2 Second list
 *
 * @pre    true
 * @post   true
 *
 * @return true if l1 and l2 are the same, false otherwise
 */
template<typename List>
static bool
CheckSame(const List& l1, const List& l2);

#ifdef DEBUG_TAILMERGE

/**
 * Print the given argument with a conditional call to fprintf.
 *
 * @param  args The arguments to be given to fprintf. Note that they must
 *         contain the parentheses
 *
 * @pre    true
 * @post   DEBUG_TAILMERGE_CONDITION implies fprintf has been called with args
 *         arguments
 *
 * @remarks This function is expansed in nothing unless the DEBUG_TAILMERGE
 *          macro is defined
 */
#    define DbgPrintTailmerge(args) DbgTailmerge(fprintf, args)

/**
 * Conditionally assert the given argument.
 *
 * @param  args The arguments to be given to DevAssert. Note that they must
 *         contain the parentheses
 *
 * @pre    true
 * @post   DEBUG_TAILMERGE_CONDITION implies DevAssert has been called with
 *         args arguments
 *
 * @remarks This function is expansed in nothing unless the DEBUG_TAILMERGE
 *          macro is defined
 */
#    define DbgAssertTailmerge(args) DbgTailmerge(DevAssert, args)

/**
 * Conditionally print a development warning with the given msg, if the check
 * condition is true.
 *
 * @param  check The condition to be checked
 * @param  msg The message to be printed in case of check argument is true
 *
 * @pre    true
 * @post   DEBUG_TAILMERGE_CONDITION and check implies DevWarn has been called
 *         with msg
 *
 * @remarks This function is expansed in nothing unless the DEBUG_TAILMERGE
 *          macro is defined
 */
#    define DbgWarmTailmerge(check, msg) DbgTailmerge(if(check), DevWarn msg)

/**
 * Generic conditionnal 'action launcher' for debug purpose.
 *
 * @param  action Action to be performed. It should be a function name, the
 *         begin of a statement, etc. What is important is that action
 *         parameter followed by args parameter made a valid C/C++ statement.
 * @param  args Arguments to be given to the action
 *
 * @pre    true
 * @post   DEBUG_TAILMERGE_CONDITION implies action has been executed with
 *         args arguments.
 *
 * @remarks This function is expansed in nothing unless the DEBUG_TAILMERGE
 *          macro is defined
 */
#    define DbgTailmerge(action, args) \
{ \
  if(DEBUG_TAILMERGE_CONDITION)                                 \
    { \
        action args; \
    } \
}
#else
#    define DbgPrintTailmerge(args)
#    define DbgAssertTailmerge(args)
#    define DbgWarmTailmerge(check, msg)
#    define DbgTailmerge(action, args)
#endif

//------------------------------------------------------------------------------
// Classes declaration
//------------------------------------------------------------------------------
/**
 * @class CNode
 * Represent a node in a tree with the following features:
 * @li A direct link to the parent link
 * @li A direct link to its direct children
 * Moreover, this class is used to represent equivalence class in tailmerging
 * optimization, so it has the following additionnal features:
 * @li The list of operations for this equivalence class
 * @li The list of sources from which the operation of the equivalence class
 *     are belonging.
 */
template<typename BasicBlock, typename Operation>
class CNode
{
 public:

    // Shortcuts used for flexibility and readability
    typedef std::list<Operation*> Operations;
    typedef typename Operations::iterator ItOperations;
    typedef typename Operations::const_iterator CItOperations;

    typedef std::list<CNode<BasicBlock, Operation>*> Nodes;
    typedef typename Nodes::iterator ItNodes;
    typedef typename Nodes::const_iterator CItNodes;

    typedef std::list<BasicBlock*> BasicBlocks;
    typedef typename BasicBlocks::iterator ItBasicBlocks;
    typedef typename BasicBlocks::const_iterator CItBasicBlocks;

    /**
     * Constructor.
     * Initialize a CNode with given argument
     *
     * @param  a_mempool Memory pool used for all dynamic allocation
     * @param  a_parent Link to the parent node
     * @param  a_equivOp Operation of the equivalence class
     *
     * @pre    a_mempool is a valid and initialized memory pool
     * @post   MemPool() = a_mempool and Parent() = a_parent and
     *         EquivOp() = a_equivOp
     */
    CNode(MEM_POOL* a_mempool, CNode* a_parent = NULL,
          Operation* a_equivOp = NULL);

    /**
     * Destructor.
     * Release all resources allocated in that object and do the same for all
     * its children
     *
     * @pre    true
     * @post   Resources released
     */
    ~CNode();

    /**
     * operator==.
     * Check the equivalence of two nodes. Currently implementation performs
     * only simple check, since it is enough.
     *
     * @param  a_node Rhs node
     *
     * @pre    true
     * @post   result = AreEquivalent(EquivOp(), a_node.EquivOp()) and
     *         CheckSame(Sources(), a_node.Sources()) and
     *         CheckSame(EmptySources(), a_node.EmptySources()) and
     *         CheckSame(Children(), a_node.Children())
     *
     * @return true if both node are the same, false otherwise
     */
    bool
    operator==(const CNode<BasicBlock, Operation>& a_node) const;

    /**
     * operator!=.
     * Check whether two nodes are different or not. (same as !operator==)
     *
     * @param  a_node Rhs node
     *
     * @pre    true
     * @post   !operator==(a_node)
     *
     * @return true if both node are different, false otherwise
     */
    bool
    operator!=(const CNode<BasicBlock, Operation>& a_node) const;

    /**
     * Add a source for this equivalence class.
     * The source is added either in Sources() list or in EmptySources() list
     * according to the result of IsEmpty(a_source)
     *
     * @param  a_source Basic block to be added
     * @param  bPushFront Specify where to insert a_source:
     *         @li true at the beginning of the related list
     *         @li false at the end of the related list
     *
     * @pre    a_source not in Sources() or EmptySources() lists
     * @post   let sources = if IsEmpty(a_source) then EmptySources() else
     *         Sources() endif in if bPushFront then sources->first() else
     *         sources->last() endif = &a_source
     *
     * @see    m_sources and m_emptySources members
     */
    void
    AddSource(BasicBlock& a_source, bool bPushFront = false);

    /**
     * Add a child to the list of children.
     * Given child is copied before being added to the list.
     *
     * @param  a_child Node to be added to the list of children
     *
     * @pre    not Children()->exists(elt | elt = a_child)
     * @post   Children() = Children()@pre->Including(new CNode(a_child))
     *
     * @return An iterator on the inserted child
     *
     * @see    m_children member
     */
    ItNodes
    AddChild(const CNode<BasicBlock, Operation>& a_child);

    /**
     * Constant getter m_sources member.
     *
     * @pre    true
     * @post   true
     *
     * @return A constant reference to m_sources member
     *
     * @see    m_sources member
     */
    const BasicBlocks&
    Sources() const;

    /**
     * Getter m_sources member.
     *
     * @pre    true
     * @post   true
     *
     * @return A reference to m_sources member
     *
     * @see    m_sources member
     */
    BasicBlocks&
    Sources();

    /**
     * Constant getter for m_emptySources member.
     *
     * @pre    true
     * @post   true
     *
     * @return A constant reference to m_emptySources
     *
     * @see    m_emptySources member
     */
    const BasicBlocks&
    EmptySources() const;

    /**
     * Getter for m_emptySources member.
     *
     * @pre    true
     * @post   true
     *
     * @return A reference to m_emptySources
     *
     * @see    m_emptySources
     */
    BasicBlocks&
    EmptySources();

    /**
     * Join empty sources and sources in m_sources member.
     *
     * @pre    true
     * @post   EmptySources()->isEmpty() and Sources() = Sources()@pre->union(
     *         EmptySources()@pre)
     */
    void
    JoinSources();

    /**
     * Constant getter for m_children member.
     *
     * @pre    true
     * @post   true
     *
     * @return A const reference to m_children
     *
     * @see    m_children member
     */
    const Nodes&
    Children() const;

    /**
     * Getter for m_children member.
     *
     * @pre    true
     * @post   true
     *
     * @return A reference to m_children
     *
     * @see    m_children member
     */
    Nodes&
    Children();

    /**
     * Getter for one of the operation of the equivalent class.
     *
     * @pre    true
     * @post   EquivClass()->isEmpty() implies result = NULL
     *
     * @return One of the operation of EquivClass() list or NULL if empty
     */
    Operation*
    EquivOp() const;

    /**
     * Add given operation to the list of equivalent operations.
     *
     * @param  op Operation to be added
     *
     * @pre    not EquivClass()->exists(elt | elt = op)
     * @post   EquivClass() = EquivClass()@pre->Including(op)
     *
     * @see    m_equivClass member
     */
    void
    AddToEquivClass(Operation* op);

    /**
     * Getter for m_equivClass member.
     *
     * @pre    true
     * @post   true
     *
     * @return A reference to m_equivClass member
     *
     * @see    m_equivClass member
     */
    Operations&
    EquivClass();

    /**
     * Constant getter for m_equivClass member.
     *
     * @pre    true
     * @post   true
     *
     * @return A constant reference to m_equivClass member
     *
     * @see    m_equivClass member
     */
    const Operations&
    EquivClass() const;

    /**
     * Getter for m_parent member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_parent member
     *
     * @see    m_parent member
     */
    CNode*
    Parent() const;

    /**
     * Setter for m_parent member.
     *
     * @param  a_parent Node that represents the parent of this one
     *
     * @pre    true
     * @post   Parent() = a_parent
     *
     * @see    m_parent member
     */
    void
    Parent(CNode* a_parent);

 private:

    /**
     * Getter for m_memPool member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_memPool member
     *
     * @see    m_memPool member
     */
    MEM_POOL*
    MemPool() const;

    /**
     * Copy constructor.
     *
     * @param  a_node Node to be copied
     *
     * @pre    a_node.Children()->isEmpty() (Currently implementation is very
     *         light but sufficient)
     * @post   result.operator==(a_node)
     */
    CNode(const CNode<BasicBlock, Operation>& a_node);

    /**
     * operator=.
     * Not implemented.
     *
     * @param  a_node Rhs node
     *
     * @pre    false
     * @post   false
     *
     * @return Lhs node
     */
    CNode&
    operator=(const CNode<BasicBlock, Operation>& a_node);

    /**
     * List of equivalent operations.
     * Equivalence class represented by this node, all operations in that list
     * are equivalent
     */
    Operations m_equivClass;

    /**
     * List of sources for this equivalence class.
     * This list has no duplicat
     */
    BasicBlocks m_sources;

    /**
     * List of sources for this equivalence class, which basic block is empty.
     * This list has no duplicat and is used for optimization purpose in
     * tailmerge algorithm (use empty basic block first instead of creating one)
     * basic block 
     */
    BasicBlocks m_emptySources;

    /**
     * Link to children node.
     * m_children->forAll(Parent() = self)
     */
    Nodes m_children;

    /**
     * Link to the parent node.
     * Parent().Children()->exists(elt | elt = self)
     */
    CNode* m_parent;

    /**
     * Memory pool member.
     * Used for all internal dynamic allocation
     */
    MEM_POOL* m_memPool;
};


/**
 * @class CTailmerge
 * Implement tailmerging mecanism.
 * Its parametrized arguments are:
 * @li A control graph flow type
 * @li A basic block type in that control graph
 * @li And an operation type in that basic block
 * Internaly, all objects are used via pointer and/or reference to avoid objects
 * copy and always work on the current control flow graph. This means that
 * reference/pointer on objects returned by functions to be targetted must
 * have at least the same liveness of this class and must be linked to the
 * related instance of control flow graph.
 * The purpose of this optimization is to merge common operation of the end of
 * several basic blocks (their tail), which have a common successor,in order to
 * reduce code size. E.g. assume following code:
 * <pre>
 * if(a > b)
 * {
 *     func1(&a, &b);
 *     ++b;
 *     a -= c;
 * }
 * else
 * {
 *     func2(&a, &b);
 *     ++b;
 *     a -= c;
 * }
 * </pre>
 * It will be transformed in:
 * <pre>
 * if(a > b)
 * {
 *     func1(&a, &b);
 * }
 * else
 * {
 *     func2(&a, &b);
 * }
 * ++b;
 * a -= c;
 * </pre>
 *
 * Current implementation applied only when all commonalizable operations are in
 * the same order. I.e. on the following example, it does nothing:
 * <pre>
 * if(a > b)
 * {
 *     func1(&a, &b);
 *     ++b;
 *     a -= c;
 * }
 * else
 * {
 *     func2(&a, &b);
 *     a -= c;
 *     ++b;
 * }
 * </pre>
 *
 * The principle of the implementation is to build a tree of equivalence classes
 * where the parent node of a node contains the previous operation of that node
 * and the related sources. E.g. with the last working example, we built:
 * <pre>
 * (root)                    a -= c        (sources bb1, bb2)
 *                             |
 * (child1)                   ++b          (sources bb1, bb2)
 *           (source bb1)    /   \     (source bb2)
 * (child2)     func1(&a, &b)     func2(&a, &b)
 * </pre>
 * Then the built graph is traversed bottom to up. For each node only one
 * operation of the equivalence class is inserted in one of the sources, the
 * other sources will point to that one. Then all parents nodes sources are
 * updated to reflect this insertion.
 *
 * @see    CNode class provides the implementation of such nodes
 *
 * @invariant In the usage with CTailmerge optimization, the following
 *            invariants for CNode class are respected for each node:
 *            @li All children are unique (no duplicat in the list
 *                of children),
 *            @li All operations in the equivalent class are unique
 *            @li All operations in the equivalent class are equivalent
 *                (AreEquivalent function return true)
 *            @li All basic blocks in sources union empty sources list are
 *                unique.
 *            @li Parent() = NULL or (Sources() union EmptySources()) is
 *                included or equal to Parent()->Sources()
 *            Moreover all nodes at the same level have strictly different
 *            equivalence class (AreEquivalent return false) and do not share
 *            any sources (empty or not).
 */
template<typename Cfg, typename BasicBlock, typename Operation>
class CTailmerge
{
 public:

    /**
     * Contructor.
     * Initialize tailmerge algorithm with given parameters
     *
     * @param  a_cfg Control flow graph to be tailmerged
     * @param  a_hasExplicitJump Define whether a_cfg has explicit jump or not.
     * @param  a_createBb Specify whether basic block creation is allow or not:
     *         @li If set to true, when several basic blocks shared the same
     *             piece of code but noone is empty, then a new basic block is
     *             created. It will contain the shared code and the other basic
     *             blocks will point to its beginning.
     *         @li If set to false, when several basic blocks shared the same
     *             piece of code but noone is empty, then one of them is chosen
     *             to store the code and the other will jump at the beginning of
     *             this piece of code (thus not the beginning of the basic
     *             block)
     * @param  a_trace Trace or not the tailmerge algorithm.
     * @param  a_before Specify where new basic block should be inserted. 'true'
     *         means before the common target, 'false' means after one of the
     *         sources
     * @param  a_memPool Memory pool used for all internal dynamic allocation
     *
     * @pre    true
     * @post   CFG() = a_cfg and HasExplicitJump() = a_hasExplicitJump and
     *         CreateBb() = a_createBb and Trace() = a_trace and MemPool() =
     *         a_memPool
     *
     * @warning When you set a_createBb to false, the algorithm may produce
     *          jump, which target is in the middle of a basic block. So, be
     *          sure that it is supported.
     */
    CTailmerge(Cfg& a_cfg, bool a_hasExplicitJump, bool a_createBb,
               bool a_trace, bool a_before, MEM_POOL* a_memPool);

    /**
     * Apply the tailmerge algorithm to CFG().
     *
     * @pre    true
     * @post   Tailmerge algorithm has been applied to CFG()
     */
    void
    Optimize();

    /**
     * Getter for m_memPool member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_memPool member
     *
     * @see    m_memPool member
     */
    MEM_POOL*
    MemPool() const;

    /**
     * Getter for m_trace member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_trace member
     *
     * @see    m_trace member
     */
    const bool
    Trace() const;

    /**
     * Getter for m_hasExplicitJump member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_hasExplicitJump member
     *
     * @see    m_hasExplicitJump member
     */
    const bool
    HasExplicitJump() const;

    /**
     * Getter for m_createBb member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_createBb member
     *
     * @see    m_createBb member
     */
    const bool
    CreateBb() const;

    /**
     * Getter for m_before member.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_before member
     *
     * @see    m_before member
     */
    const bool
    Before() const;

 protected:

    // Shortcuts used for flexibility and readability
    typedef CNode<BasicBlock, Operation> TmNode;
    typedef std::list<TmNode*> TmNodes;
    typedef typename TmNodes::iterator ItTmNodes;
    typedef typename TmNodes::const_iterator CItTmNodes;
    typedef typename TmNodes::reverse_iterator RItTmNodes;
    typedef typename TmNodes::const_reverse_iterator CRItTmNodes;

    /**
     * Getter for m_cfg member.
     *
     * @pre    true
     * @post   true
     *
     * @return m_cfg member
     *
     * @see    m_cfg member
     */
    Cfg&
    CFG();

 private:

    typedef std::set<int> Ids;

    /**
     * Apply tailmerge algorithm to all predecessors of given basic block.
     *
     * @param  bb Basic block: Try to tailmerge all its predecessors
     *
     * @pre    true
     * @post   Tailmerge algorithm has been applied to all successors of bb
     */
    void
    TailmergeBB(BasicBlock& bb);

    /**
     * Initialize tailmerge nodes for all predecessors of given basic block.
     * Each node represents an equivalence class of operations. The
     * initialization consists in creating one node for each equivalence class
     * composed by each last operation of each predecessors of bb
     *
     * @param  a_tailmergeNodes [out] List of node that will recieve computed
     *         equivalence classes
     * @param  bb Basic block: Use its predecessors to initialize equivalence
     *         classes
     * @param  a_hasNotJump [out] Will contains the identifier of all basic
     *         which last operation is not a jump
     *
     * @pre    true
     * @post   The first level of equivalence classes has been created, i.e.
     *         a_tailmergeNodes->forAll(Parent() = NULL)
     */
    void
    InitTailmergeNodes(TmNodes& a_tailmergeNodes, BasicBlock& bb,
                       Ids& a_hasNotJump);

    /**
     * Create the graph of equivlance classes of the operations.
     *
     * @param  a_tmNodes [in/out] List of equivalence nodes:
     *         @li [in] Contains the first level of the graph
     *         @li [out] Contains all nodes that composed the graph of
     *             equivalence classes.
     * @param  a_hasNotJump Set of identifiers of basic blocks which do
     *         not finish with a jump operation. Used for optimization purpose
     *         (increase usage of those basic blocks, since we will avoid one
     *         jump operation)
     *
     * @pre    true
     * @post   a_tmNodes->forAll(Children()->forAll(child | a_tmNodes->one(elt =
     *         child))) and a_tmNodes->forAll(elt | elt.Parent() = NULL or
     *         a_tmNodes->one(elt2 | elt2 = elt.Parent()))
     */
    void
    ComputeEquivalenceClasses(TmNodes& a_tmNodes, const Ids& a_hasNotJump);

    /**
     * Add the last operation of given bb in the related equivalence class.
     * Checked equivalence classes are represented by the children of given
     * node. Unless one of the class of these children is an equivalence class
     * of the last operation of bb, a new child node is created and is
     * associated to that operation and basic block. Then the operation is
     * removed from the basic block
     *
     * @param  a_node Equivalence node, which children are containing current
     *         computed equivalence classes
     * @param  bb Considered basic block
     * @param  a_hasNotJump Set of identifiers of basic blocks which do
     *         not finish with a jump operation.
     *
     * @pre    a_node.Children()->forAll(not Sources()->union(EmptySources())
     *         ->exists(elt | elt = bb))
     * @post   if IsEmpty(bb@pre) then result = NULL else
     *         (let targetNode = a_node->Children()@pre->Select(
     *         AreEquivalent(EquiOp(), GetLastOp(bb@pre))) in
     *         targetNode->isEmpty() implies result->oclIsNew() and
     *         a_node->Children() = a_node->Children()@pre->including(result))
     *         and (let sources = if IsEmpty(bb) then result.EmptySources() else
     *         result.Sources() endif in sources = sources@pre->including(bb))
     *         and result->EquivClass() = result->EquivClass()@pre->
     *         including(GetLastOp(bb@pre)) and bb = bb@pre->excluding(
     *         GetLastOp(bb@pre))
     *         endif
     *
     * @return A pointer to the node which represents the equivalence class of
     *         last operation of bb or NULL if bb is empty
     */
    TmNode*
    SetEquivalence(TmNode& a_node, BasicBlock& bb, const Ids& a_hasNotJump);

    /**
     * Build control graph flow represented by the given list of nodes.
     * This method performs the following operations for all nodes:
     * @li Choose one of the sources to recieve the operation represented
     *     by the equivalence class. The choice of the source is made according
     *     to the algorithm definied by DestinationBlock method
     * @li Update all ancestors of that node: remove the sources of that nodes
     *     and replace them by the new unique source
     * @li Branch all sources that did not recieve the operation to this new
     *     source
     * This process is applied in reverse order on the given list (see
     * pre-conditions)
     *
     * @param  a_tailmergeNodes List of equivalence class nodes
     * @param  a_origBb Origin basic block of tailmerge algorithm, i.e. the
     *         common target of all sources of all nodes
     * @param  a_hasNotJump Set of identifiers of basic blocks which do
     *         not finish with a jump operation.
     *
     * @pre    Rhe list is ordered with the level of the nodes in the
     *         equivalence graph (low (root) to high (leaf))
     * @post   For each node only one operation of the equivalence class has
     *         been inserted in one of the sources, the other sources will point
     *         to that one.
     */
    void
    BuildTailmergedCFlow(TmNodes& a_tailmergeNodes, BasicBlock& a_origBb,
                         const Ids& a_hasNotJump);

    /**
     * Decide which block should be used to insert the operation represented by
     * given node.
     * The choice is made by the following algorithm:
     * - If a_node has several children then
     *    - If a_node has empty sources then use one of them
     *    - Else take one of the sources or create a new basic block
     *      (bCreate = true)
     * - Else
     *   - If a_node has one child then use its unique source
     *   - Else use one of the source of this node. In that case, the list of
     *     sources has always a size of 1 by construction.
     * We try to use children instead of empty sources when it is possible to
     * have the longest possible sequence of operation in one basic block.
     *
     * @param  a_node [in/out] Node for which we have to decide which basic
     *         block will recieve the operation.
     * @param  bCreate Specify whehter we can create basic block or not for the
     *         choice.
     * @param  a_origBb Origin basic block of tailmerge algorithm, i.e. the
     *         common target of all sources of all nodes. Use as fixed point
     *         when creating and inserting a basic block in CFG()
     *
     * @pre    true
     * @post   result is the best canditate to recieve the operation represented
     *         by a_node.
     *
     * @return A pointer to the basic block that should recieved the operation,
     *         or NULL if all sources must recieve the operation. Regarding the
     *         algorithm defined in the description, the NULL case is not
     *         explicited. Actually, this is because it is an optimization
     *         used for the last operation of a basic block (when jump are
     *         explicit), i.e. the jump one, to avoid to merge jump operations.
     */
    BasicBlock*
    DestinationBlock(TmNode& a_node, bool bCreate, BasicBlock& a_origBb);

    /**
     * Update a_node sources connection (jump, link to successors/predecessors,
     * etc.).
     * The Updating consists in adding the right operations/connections to link
     * all sources of a_node to a_target and in removing all references to
     * a_orig.
     *
     * @param  a_node Node with sources to be updated. Note that empty sources
     *         are not updated
     * @param  a_target New target basic block that will replace a_orig basic
     *         block
     * @param  a_orig Old target basic block that will be replaced by a_target
     *
     * @pre    true
     * @post   All links of sources to a_orig has been replaced by link to
     *         a_target and all related links in a_orig (e.g. ones contains list
     *         of predecessors) have been removed
     */
    void
    UpdateSources(TmNode& a_node, BasicBlock& a_target, BasicBlock& a_orig);

    /**
     * Update the source list of all ancestors of given node.
     * The updating consists in removing all basic blocks contained in a_sources
     * from the sources of all ancestors of a_node (ancestors = parent a_node
     * union parent parent a_node union etc.) and adding a_target in that list
     * of source if not already present.
     *
     * @param  a_node Start point of the updating operation
     * @param  a_sources List of the sources to be removed
     * @param  a_target Basic block that replace that list of sources
     *
     * @pre    true
     * @post   For all ancestors of a_node, all elements in a_sources have been
     *         replaced by on occurrence of a_target in related sources list
     */
    void
    UpdateParentsSourcesList(const TmNode& a_node,
                             const typename TmNode::BasicBlocks& a_sources,
                             BasicBlock& a_target);

    /**
     * Update the source list of given node.
     * The updating of the sources is the replacement of all elements contained
     * in a_sources by a unique reference to a_target
     *
     * @param  a_node Node to be updated
     * @param  a_sources List of sources to be removed
     * @param  a_target Source to be added
     *
     * @pre    true
     * @post   a_sources->forAll(elt1 | not a_node->Sources()->exists(elt2 |
     *         elt2 = elt1)) and a_node->Sources()->one(elt | a_target = elt)
     */
    void
    UpdateNodeSourcesList(TmNode& a_node,
                          const typename TmNode::BasicBlocks& a_sources,
                          BasicBlock& a_target);

    /**
     * Has explicit jump member.
     * Specify whether control graph flow has explicit (true) or implicit
     * (false) jump.
     */
    const bool m_hasExplicitJump;

    /**
     * Create basic block member.
     * Specify whether tailmerge algorithm can create basic block or not.
     *
     * @see    CTailmerge contructor to know the constraint related to this
     *         setting
     */
    const bool m_createBb;

    /**
     * Trace member.
     * Specify whether algorithm should print debugging tracing or not
     */
    const bool m_trace;

    /**
     * Specify where generated basic blocks should be inserted:
     * 'true' means before the original basic block
     * 'false' means after one of the sources basic block
     */
    const bool m_before;

    /**
     * Memory pool member.
     * Memory pool used for all internal memory allocation
     */
    MEM_POOL* m_memPool;

    /**
     * Control flow graph member.
     * Control flow graph to be tailmerged.
     */
    Cfg& m_cfg;
};


/**
 * @class CExtendedTailmerge
 * Extend the tailmerging mecanism by providing a simplification of the control
 * flow graph before applying the optimization. Note that the simplification of
 * the control flow graph can be called separately.
 * The simplification of the control flow graph consists in suppressing indirect
 * useless jump. A jump is considered as useless when it leads to a simple basic
 * block. A basic block is simple when its execution does not modify the
 * behavior of the program. Typically, such basic blocks are empty one or
 * contain only an unconditional jump.
 *
 * E.g. Assume the two following branching:
 * BB1->BB2->BB4
 * BB3->BB4
 * Where BB2 is a simple goto to BB4.
 *
 * Without the simplification, tailmerge will apply on BB2 and BB3. Since BB2 is
 * empty (or so), it did nothing.
 * With the simplification, we get the following branching:
 * BB1->BB4
 * BB3->BB4
 * Thus, tailmerge will apply on BB1 and BB3.
 */
template<typename Cfg, typename BasicBlock, typename Operation>
class CExtendedTailmerge : public CTailmerge<Cfg, BasicBlock, Operation>
{
 public:
    // Shortcut
    typedef typename CTailmerge<Cfg, BasicBlock, Operation>::TmNode ExtTmNode;

    /**
     * SSimplifyInfo structure stores information used by the control flow
     * simplification.
     * This information will be used in a map that gives for a basic block
     * its simplification
     */
    struct SSimplifyInfo
    {
        /**
         * One of the successor
         */
        BasicBlock* succ;
        
        /**
         * Target of the basic block.
         * When the mapped basic block is simple, this field contains the first
         * non simple basic block on its trivial path.
         * When the mapped basic block is not simple, this field contains itself
         */
        BasicBlock* target;

        /**
         * Visit marker
         */
        bool visited;

        /**
         * Specify whether Mapped basic block is simple or not
         */
        bool isSimple;

        /**
         * List of the predecessors of the mapped basic block
         */
        typename ExtTmNode::BasicBlocks listOfPreds;

        /**
         * Default constructor.
         * Initialize all members with given values
         *
         * @param  a_succ Value to be set to succ
         * @param  a_target Value to be set to target
         * @param  a_visited Value to be set to visited
         * @param  a_isSimple Value to be set to isSimple
         *
         * @pre    true
         * @post   succ = a_succ and target = a_target and visited = a_visited 
         *         and isSimple = a_isSimple and listOfPreds.empty()
         */
        SSimplifyInfo(BasicBlock* a_succ = NULL,
                      BasicBlock* a_target = NULL,
                      bool a_visited = false, bool a_isSimple = false)
            :succ(a_succ), target(a_target), visited(a_visited),
             isSimple(a_isSimple), listOfPreds()
        {}
    };

    // Shortcut
    typedef std::map<BasicBlock*, SSimplifyInfo> BBsSimplifyInfo;
    typedef typename BBsSimplifyInfo::const_iterator CItBBsSimplifyInfo;
    typedef typename BBsSimplifyInfo::iterator ItBBsSimplifyInfo;

    /**
     * CExtendedTailmerge constructor.
     * @see CTailmerge::CTailmerge
     *
     * @param  a_cfg 
     * @param  a_hasExplicitJump 
     * @param  a_createBb 
     * @param  a_trace 
     * @param  a_before 
     * @param  a_memPool 
     * @param  a_simplifyFlow New parameter to specify whether applying or not
     *         the control flow simplification before tailmerge. When set to
     *         false, the behavior is exactly the same as CTailmerge::CTailmerge
     */
    CExtendedTailmerge(Cfg& a_cfg, bool a_hasExplicitJump, bool a_createBb,
                       bool a_trace, bool a_before, MEM_POOL* a_memPool,
                       bool a_simplifyFlow);

    /**
     * Apply tailmerge algorithm plus optionally control flow simplification.
     *
     * @see CTailmerge::Optimize
     */
    void
    Optimize();

    /**
     * Simplify control flow graph.
     * The simplification is split in two phases:
     * @li First we gather the simplification information (see
     *     CExtendedTailmerge::GatherSimplifyInfo)
     * @li Second we apply the simplification using collected information (see
     *     CExtendedTailmerge::DoSimplifyControlFlowGraph)
     * This is done this way to perform only one update of the CFG.
     *
     * See the description of CExtendedTailmerge class for details of on the
     * performed simplification.
     *
     * @pre    true
     * @post   CFG() has been simplified
     */
    void
    SimplifyControlFlowGraph();

    /**
     * Getter for m_simplifyFlow member.
     *
     * @pre    true
     * @post   true
     *
     * @return The value of m_simplifyFlow
     *
     * @see m_simplifyFlow for details
     */
    const bool
    SimplifyFlow() const;

    /**
     * @see CTailmerge::Trace. This function has been set to avoid compilation
     * problem with gcc. In fact, it is useless
     */
    const bool
    Trace() const;

 protected:

    /**
     * @see CTailmerge::CFG. This function has been set to avoid compilation
     * problem with gcc. In fact, it is useless
     */
    Cfg&
    CFG();

 private:

    /**
     * Initialize given simplify information for current CFG().
     * The initialization consists in setting an entry for all exit basic
     * blocks of CFG(). Each entry will have the following properties:
     * @li succ: NULL
     * @li target: itself
     * @li visited: true
     * @li isSimple: false
     *
     * @param  bbsSimplifyInfo [out] Will contain the added entries
     * @param  bbs [out] Will contain the list of exit BBs
     *
     * @pre    true
     * @post   bbsSimplifyInfo contains an entry for each exit blocks of CFG()
     */
    void
    InitSimplifyControlFlowGraph(BBsSimplifyInfo& bbsSimplifyInfo,
				 typename ExtTmNode::BasicBlocks& bbs);

    /**
     * Gather simplification information for all basic blocks of CFG().
     * The gathering will traverse all basic blocks in reverse order (exit bb
     * then predecessor of exit bb, then predecessor of predecessor of exit bb,
     * etc.) and will build for each of them an entry in bbsSimplifyInfo.
     *
     * The building of these entries is based on all information already
     * collected for successors. The idea is to propagate the farest possible
     * target accross all simple basic blocks. To do that, we compute the
     * simplification information for a basic block and we use that information
     * to create the one of all its predecessors
     *
     * @see CExtendedTailmerge::GatherSimplifyInfoBB for more details on
     *      predecessors information setting
     *
     * @param  bbsSimplifyInfo
     *         @li in: Contains the start point of the analyse, thus all basic
     *             blocks that may be exit basic blocks of CFG()
     *         @li out: Contains an entry for each basic blocks reached using 
     *             a recursive predecessors link from bbsSimplifyInfo@pre
     *             entries
     * @param  bbs [in/out] Contains the list of exit BBs, and will contain
     *             the list of all BBs of the CFG
     *
     * @pre    true
     * @post   bbsSimplifyInfo contains simplification information for all basic
     *         blocks reached using recursively the predecessors link from
     *         bbsSimplifyInfo@pre entries
     */
    void
    GatherSimplifyInfo(BBsSimplifyInfo& bbsSimplifyInfo,
		       typename ExtTmNode::BasicBlocks& bbs);

    /**
     * Gather simplification information for listOfPreds using succ information.
     * For a basic block, its simplification information is set according two
     * cases:
     * @li First: this basic block is a simple one (IsSimpleBB targeting
     *     function), then by definition it has, at most, one successor (succ).
     *     Since it is simple, this means all its predecessors can be directly
     *     branched to its successor. But its successor may be simple, so they
     *     can be branched directly to the successor of this successor, etc.
     *     Therefore, the simplification information for that basic block are:
     *     @li  succ: succ (true successor)
     *     @li target: succ's simplification information target (that is the
     *         propagation introduced in CExtendedTailmerge::GatherSimplifyInfo)
     *     @li visited: true
     *     @li isSimple: true
     * @li Second: this basic block is not simple.
     *     @li succ: succ
     *     @li target: itself
     *     @li visited: true
     *     @li isSimple: false
     *     
     *
     * @param  listOfPreds List of basic blocks to be checked. They must have
     *         succ in their successors list.
     * @param  succ Basic block successor of all elements of listOfPreds
     * @param  bbs [in/out] Contains the list of basic blocks which predecessors
     *         have to be checked
     * @param  bbsSimplifyInfo [in/out] Contains simplification information
     *
     * @pre    listOfPreds->forAll(bb | bb.succs->includes(succ)) and
     *         bbsSimplifyInfo[&succ] has already been set
     * @post   let newBbs = listOfPreds@pre->collect(visited = false) in
     *         newBbs->forAll(visited = true) and newBbs->forAll(bb |
     *         bbsSimplifyInfo[&bb] is set)
     */
    void
    GatherSimplifyInfoBB(const typename ExtTmNode::BasicBlocks& listOfPreds,
                         BasicBlock& succ,
                         typename ExtTmNode::BasicBlocks& bbs,
                         BBsSimplifyInfo& bbsSimplifyInfo);

    /**
     * Simplify control flow graph using bbsSimplifyInfo.
     * The simplification consists in applying DoSimplifyControlFlowGraphBB
     * on all basic blocks that are marked as simple
     *
     * @param  bbsSimplifyInfo Collected simplification information
     * @param  bbs [in] contains the list of all BBS of current CFG
     *
     * @pre    bbsSimplifyInfo is compatible with CFG()
     * @post   CFG() has been simplified according to bbsSimplifyInfo
     *
     */
    void
    DoSimplifyControlFlowGraph(const BBsSimplifyInfo& bbsSimplifyInfo,
			       const typename ExtTmNode::BasicBlocks& bbs);

    /**
     * Simplify control flow paths traversing simpleBb.
     * The simplification consists in setting the target of the jump of all the
     * predecessors of simpleBb to bbsSimplifyInfo[&simpleBb].target. If at
     * least one of these updating is not possible, then simpleBb is not
     * appended to the list of removable basic block.
     * The updating is considered as imposible when either:
     * @li the predecessor is simpleBb
     * @li ReplaceSimpleJump return false for predecessor as src,
     *     bbsSimplifyInfo[&simpleBb].target as new target and simpleBb as old
     *     target
     *
     * @param  simpleBb 
     * @param  bbsSimplifyInfo Gathered simplification information
     * @param  toBeRemoved [out] Contains the list of basic block that can be
     *         removed
     *
     * @pre    bbsSimplifyInfo[&simpleBb].listOfPreds = simpleBb.preds and
     *         bbsSimplifyInfo[&simpleBb].isSimple
     * @post   All updatable path using simpleBb have been updated to point on
     *         bbsSimplifyInfo[&simpleBb].target
     */
    void
    DoSimplifyControlFlowGraphBB(BasicBlock& simpleBb,
                                 const BBsSimplifyInfo& bbsSimplifyInfo,
                                 typename ExtTmNode::BasicBlocks& toBeRemoved);

    /**
     * Check whether parent_bb is already a predecessor of target_bb
     *
     * @param  pred_bb [in] the predecessor to be tested
     * @param  target_bb [in] the block whose predecessors are to be checked
     *
     * @pre    true
     * @post   result = pred_bb is a predecessor of target_bb
     *
     * @return true if pred_bb is a predecessor of target_bb
     *
     * @todo   Targetting to be done
     */

    bool
    IsInPred(BasicBlock& pred_bb, BasicBlock& target_bb);

    /**
     * Specify whether SimplifyControlFlowGraph must be called before applying
     * tailmerge algorithm or not
     */
    const bool m_simplifyFlow;
};


//------------------------------------------------------------------------------
// Classes definition
//------------------------------------------------------------------------------

//----------------------
// CNode class
//----------------------
template<typename BasicBlock, typename Operation>
CNode<BasicBlock, Operation>::CNode(MEM_POOL* a_memPool,
                                    CNode* a_parent,
                                    Operation* a_equivOp)
    :m_equivClass(), m_sources(), m_emptySources(), m_children(),
     m_parent(a_parent), m_memPool(a_memPool)
{
    AddToEquivClass(a_equivOp);
}

template<typename BasicBlock, typename Operation>
CNode<BasicBlock, Operation>::CNode(const CNode<BasicBlock, Operation>& a_node)
    :m_equivClass(a_node.EquivClass()), m_sources(a_node.Sources()),
     m_emptySources(a_node.EmptySources()), m_children(),
     m_parent(a_node.Parent()), m_memPool(a_node.MemPool())
{
    // I do not want to write code for child duplication, since it is not used
    DevAssert(a_node.Children().empty(), ("Copy construction does not support "
                                          "children duplication"));
}

template<typename BasicBlock, typename Operation>
CNode<BasicBlock, Operation>::~CNode()
{
    ItNodes it;
    for(it = Children().begin(); it != Children().end(); ++it)
        {
            CXX_DELETE(*it, MemPool());
        }
}

template<typename BasicBlock, typename Operation>
bool
CNode<BasicBlock, Operation>::operator==
    (const CNode<BasicBlock, Operation>& a_node) const
{
    bool result = &a_node == this;
    if(!result)
        {
            result = AreEquivalent(EquivOp(), a_node.EquivOp()) &&
                CheckSame(Children(), a_node.Children()) &&
                CheckSame(Sources(), a_node.Sources()) &&
                CheckSame(EmptySources(), a_node.EmptySources());
        }
    return result;
}

template<typename BasicBlock, typename Operation>
bool
CNode<BasicBlock, Operation>::operator!=
    (const CNode<BasicBlock, Operation>& a_node) const
{
    return !operator==(a_node);
}

template<typename BasicBlock, typename Operation>
void
CNode<BasicBlock, Operation>::AddSource(BasicBlock& a_source, bool bPushFront)
{
    bool result = true;
    if(g_checkInvariant)
        {
            // Ensure sources is not already in list
            CItBasicBlocks it;
            for(it = Sources().begin(); it != Sources().end() &&
                    result; ++it)
                {
                    result &= *it != &a_source;
                }
        }
    if(result)
        {
            BasicBlocks* dst;
            if(IsEmpty(a_source))
                {
                    dst = &m_emptySources;
                }
            else
                {
                    dst = &m_sources;
                }
            if(bPushFront)
                {
                    dst->push_front(&a_source);
                }
            else
                {
                    dst->push_back(&a_source);
                }
        }
    DevAssert(result, ("Invariant wrong!!!"));
}

template<typename BasicBlock, typename Operation>
typename CNode<BasicBlock, Operation>::ItNodes
CNode<BasicBlock, Operation>::AddChild
    (const CNode<BasicBlock, Operation>& a_child)
{
    bool result = true;
    ItNodes it = m_children.end();
    if(g_checkInvariant)
        {
            // Ensure child is not already in list
            CItNodes it;
            for(it = Children().begin(); it != Children().end() &&
                    result; ++it)
                {
                    result &= **it != a_child;
                }
        }
    if(result)
        {
            // We perform dynamic allocation
            // instead of storing object in list to not duplicate object
            // when pushing in the global list (children must remain
            // bound to their parents)
            CNode* child = CXX_NEW(CNode(a_child), MemPool());
            child->m_memPool = MemPool();
            it = m_children.insert(it, child);
        }
    DevAssert(it != m_children.end(), ("Invariant wrong!!!"));
    return it;
}

template<typename BasicBlock, typename Operation>
const typename CNode<BasicBlock, Operation>::BasicBlocks&
CNode<BasicBlock, Operation>::Sources() const
{
    return m_sources;
}

template<typename BasicBlock, typename Operation>
typename CNode<BasicBlock, Operation>::BasicBlocks&
CNode<BasicBlock, Operation>::Sources()
{
    return m_sources;
}

template<typename BasicBlock, typename Operation>
const typename CNode<BasicBlock, Operation>::BasicBlocks&
CNode<BasicBlock, Operation>::EmptySources() const
{
    return m_emptySources;
}

template<typename BasicBlock, typename Operation>
typename CNode<BasicBlock, Operation>::BasicBlocks&
CNode<BasicBlock, Operation>::EmptySources()
{
    return m_emptySources;
}

template<typename BasicBlock, typename Operation>
void
CNode<BasicBlock, Operation>::JoinSources()
{
    Sources().insert(Sources().end(), EmptySources().begin(),
                     EmptySources().end());
    EmptySources().clear();
}

template<typename BasicBlock, typename Operation>
const typename CNode<BasicBlock, Operation>::Nodes&
CNode<BasicBlock, Operation>::Children() const
{
    return m_children;
}

template<typename BasicBlock, typename Operation>
typename CNode<BasicBlock, Operation>::Nodes&
CNode<BasicBlock, Operation>::Children()
{
    return m_children;
}

template<typename BasicBlock, typename Operation>
Operation*
CNode<BasicBlock, Operation>::EquivOp() const
{
    Operation* result = NULL;
    if(!m_equivClass.empty())
        {
            result = m_equivClass.front();
        }
    return result;
}

template<typename BasicBlock, typename Operation>
void
CNode<BasicBlock, Operation>::AddToEquivClass(Operation* op)
{
    if(g_checkInvariant && EquivOp())
        {
            bool result = AreEquivalent(EquivOp(), op);
            CItOperations it;
            for(it = EquivClass().begin(); result && it != EquivClass().end();
                ++it)
                {
                    result &= *it != op;
                }
            DevAssert(result, ("Wrong invariant"));
        }
    m_equivClass.push_back(op);
}

template<typename BasicBlock, typename Operation>
typename CNode<BasicBlock, Operation>::Operations&
CNode<BasicBlock, Operation>::EquivClass()
{
    return m_equivClass;
}

template<typename BasicBlock, typename Operation>
const typename CNode<BasicBlock, Operation>::Operations&
CNode<BasicBlock, Operation>::EquivClass() const
{
    return m_equivClass;
}

template<typename BasicBlock, typename Operation>
CNode<BasicBlock, Operation>*
CNode<BasicBlock, Operation>::Parent() const
{
    return m_parent;
}

template<typename BasicBlock, typename Operation>
void
CNode<BasicBlock, Operation>::Parent(CNode* a_parent)
{
    m_parent = a_parent;
}

template<typename BasicBlock, typename Operation>
MEM_POOL*
CNode<BasicBlock, Operation>::MemPool() const
{
    return m_memPool;
}


//----------------------
// CTailmerge class
//----------------------
template<typename Cfg, typename BasicBlock, typename Operation>
CTailmerge<Cfg, BasicBlock, Operation>::CTailmerge(Cfg& a_cfg,
                                                   bool a_hasExplicitJump,
                                                   bool a_createBb,
                                                   bool a_trace,
                                                   bool a_before,
                                                   MEM_POOL* a_memPool)
    : m_hasExplicitJump(a_hasExplicitJump), m_createBb(a_createBb),
      m_trace(a_trace), m_before(a_before), m_memPool(a_memPool), m_cfg(a_cfg)
{
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::Optimize()
{
    typename TmNode::BasicBlocks listOfBBs;
    GetBasicBlocksList(listOfBBs, CFG());
    typename TmNode::ItBasicBlocks itBb;
    for(itBb = listOfBBs.begin(); itBb != listOfBBs.end(); ++itBb)
        {
            TailmergeBB(**itBb);
        }
}

template<typename Cfg, typename BasicBlock, typename Operation>
MEM_POOL*
CTailmerge<Cfg, BasicBlock, Operation>::MemPool() const
{
    return m_memPool;
}

template<typename Cfg, typename BasicBlock, typename Operation>
inline const bool
CTailmerge<Cfg, BasicBlock, Operation>::Trace() const
{
    return m_trace;
}

template<typename Cfg, typename BasicBlock, typename Operation>
inline const bool
CTailmerge<Cfg, BasicBlock, Operation>::HasExplicitJump() const
{
    return m_hasExplicitJump;
}

template<typename Cfg, typename BasicBlock, typename Operation>
inline const bool
CTailmerge<Cfg, BasicBlock, Operation>::CreateBb() const
{
    return m_createBb;
}

template<typename Cfg, typename BasicBlock, typename Operation>
inline const bool
CTailmerge<Cfg, BasicBlock, Operation>::Before() const
{
    return m_before;
}

template<typename Cfg, typename BasicBlock, typename Operation>
Cfg&
CTailmerge<Cfg, BasicBlock, Operation>::CFG()
{
    return m_cfg;
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::TailmergeBB(BasicBlock& bb)
{
    TmNodes tailmergeNodes;
    Ids hasNotJump;
    InitTailmergeNodes(tailmergeNodes, bb, hasNotJump);
    ComputeEquivalenceClasses(tailmergeNodes, hasNotJump);
    BuildTailmergedCFlow(tailmergeNodes, bb, hasNotJump);
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::InitTailmergeNodes(TmNodes& a_tmNodes,
                                                           BasicBlock& bb,
                                                           Ids& a_hasNotJump)
{
    DbgPrintTailmerge((debugOutput, "*** %s\n", __FUNCTION__));
    TmNode tmp(MemPool());

    typename TmNode::BasicBlocks listOfPreds;
    GetPredecessorsList(listOfPreds, CFG(), bb);
    DbgPrintTailmerge((debugOutput, "Examined BB: %d\n",
                       BasicBlockId(CFG(), bb)));
    // If size <= 1, we can optimize algorithm by just exiting this function
    DbgPrintTailmerge((debugOutput, "Nb predecessors %d\n",
                       listOfPreds.size()));
    typename TmNode::ItBasicBlocks itBb;
    Ids alreadyDone;
    for(itBb = listOfPreds.begin(); itBb != listOfPreds.end(); ++itBb)
        {
            int bbId = BasicBlockId(CFG(), **itBb);
            DbgPrintTailmerge((debugOutput, "Pred id: %d\n", bbId));
            if(*itBb == &bb || alreadyDone.find(bbId) != alreadyDone.end())
                {
                    continue;
                }
            alreadyDone.insert(bbId);
            if(HasExplicitJump() &&
               // Normally a cfg with explicit basic block do not need
               // this piece of code. But whirl representation is not regular
               // and mixes both explicit and implicit jumps!
               (IsEmpty(**itBb) ||
                !IsJump(GetLastOp<BasicBlock, Operation>(**itBb), &CFG(), &bb)))
                {
                    DbgPrintTailmerge((debugOutput, "Last operation is not a "
                                       "jump: add one\n"));
                    a_hasNotJump.insert(bbId);
                    AddGoto(CFG(), **itBb, bb, true);
                }
            TmNode* child = SetEquivalence(tmp, **itBb, a_hasNotJump);
            if(child)
                {
                    // tmp is not a valid parent
                    child->Parent(NULL);
                }
        }
    a_tmNodes.insert(a_tmNodes.end(), tmp.Children().begin(),
                     tmp.Children().end());
    // Disconnect all children to avoid their deletion, since this node is
    // dummy
    tmp.Children().clear();
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::ComputeEquivalenceClasses
    (TmNodes& a_tmNodes, const Ids& a_hasNotJump)
{
    DbgPrintTailmerge((debugOutput, "*** %s\n", __FUNCTION__));
    ItTmNodes it;
    for(it = a_tmNodes.begin(); it != a_tmNodes.end(); ++it)
        {
            if((*it)->Sources().size() > 1)
                {
                    typename TmNode::ItBasicBlocks itBb;
                    for(itBb = (*it)->Sources().begin();
                        itBb != (*it)->Sources().end(); ++itBb)
                        {
                            static_cast<void>(SetEquivalence(**it, **itBb,
                                                             a_hasNotJump));
                        }
                    a_tmNodes.insert(a_tmNodes.end(), (*it)->Children().begin(),
                                     (*it)->Children().end());
                }
        }
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::BuildTailmergedCFlow
    (TmNodes& a_tailmergeNodes, BasicBlock& a_origBb, const Ids& a_hasNotJump)
{
    DbgPrintTailmerge((debugOutput, "*** %s\n", __FUNCTION__));

    RItTmNodes it, tmp = a_tailmergeNodes.rend();

    for(it = a_tailmergeNodes.rbegin(); it != a_tailmergeNodes.rend(); ++it)
        {
            DbgPrintTailmerge((debugOutput, "-> Considered operation:\n"));
            DbgTailmerge(DumpOperation, (debugOutput,(*it)->EquivOp()));

            // Special case for jump last basic block operation
            bool isLastOp = HasExplicitJump() && !(*it)->Parent();
            BasicBlock* willReceivedOp = DestinationBlock(**it, !isLastOp,
                                                         a_origBb);
            (*it)->JoinSources();
            if(willReceivedOp != InvalidBasicBlock<BasicBlock>())
                {
                    UpdateSources(**it, *willReceivedOp, a_origBb);
                    UpdateParentsSourcesList(**it, (*it)->Sources(),
                                             *willReceivedOp);
                    // Now, set the new sources for this operation
                    (*it)->Sources().clear();
                    (*it)->Sources().push_back(willReceivedOp);
                }
            typename TmNode::BasicBlocks& sources = (*it)->Sources();
            typename TmNode::ItBasicBlocks itBb;
            typename TmNode::ItOperations itOp;

            DbgPrintTailmerge((debugOutput, "Sources size %d\n",
                               sources.size()));
            DevAssert(sources.size() <= (*it)->EquivClass().size(),
                      ("We must have at least one operation per basic block"));

            for(itBb = sources.begin(), itOp = (*it)->EquivClass().begin();
                itBb != sources.end(); ++itBb, ++itOp)
                {
                    int bbId = BasicBlockId(CFG(), **itBb);
                    if(!isLastOp ||
                       a_hasNotJump.find(bbId) == a_hasNotJump.end())
                        {
                            DbgPrintTailmerge((debugOutput,
                                               "Destination BB is:\nBB: %d\n",
                                               bbId));
                            AppendOp(CFG(), **itBb, *itOp);
                        }
                }
        }
}

template<typename Cfg, typename BasicBlock, typename Operation>
BasicBlock*
CTailmerge<Cfg, BasicBlock, Operation>::DestinationBlock(TmNode& a_node,
                                                         bool bCreate,
                                                         BasicBlock& a_origBb)
{
    BasicBlock* willReceivedOp = InvalidBasicBlock<BasicBlock>();
    typename TmNode::BasicBlocks* toBeRemoved = &a_node.EmptySources();
    if(!a_node.EmptySources().empty())
        {
            // Optimization: We do not use empty sources at
            // first level (bCreate is false) to avoid useless jump creation.
            // E.g.
            // BB1(empty) -> BB3
            // BB2 -> BB3
            // Will be factorized without this optim:
            // BB2 -> BB1(empty) -> BB3
            if(bCreate)
                {
                    DbgPrintTailmerge((debugOutput, "Sources have empty blocks."
                                       " Use it\n"));
                    willReceivedOp = toBeRemoved->front();
                    toBeRemoved->pop_front();
                }
            else
                {
                    DbgPrintTailmerge((debugOutput, "Do not change empty "
                                       "sources.\n"));
                    toBeRemoved = NULL;
                }
        }
    else if(a_node.Children().size() > 1)
        {
            if(bCreate)
                {
                    DbgPrintTailmerge((debugOutput,
                                       "Node has several children."));
                    if(CreateBb())
                        {
                            DbgPrintTailmerge((debugOutput,
                                               " Create a new basic.\n"));
                            // Since we know that the created bb will jump
                            // on a_origBb, let us place it just before.
                            BasicBlock* targetBb = &a_origBb;
                            // Unless it has been specified to do not that
                            if(!Before())
                                {
                                    targetBb = a_node.Sources().front();
                                }
                            // Create a new bb
                            willReceivedOp =
                                GenAndInsertBB(CFG(), *targetBb, Before());
                            DbgPrintTailmerge((debugOutput,
                                               "Created BB%d.\n",
                                               BasicBlockId(CFG(),
                                                            *willReceivedOp)));
                        }
                    else
                        {
                            DbgPrintTailmerge((debugOutput,
                                               " Use one of the sources.\n"));
                            willReceivedOp = a_node.Sources().front();
                            a_node.Sources().pop_front();
                        }
                }
        }
    else if(a_node.Children().size() == 1)
        {
            DbgPrintTailmerge((debugOutput, "Node has only one child. "
                               "Use it has target block.\n"));
            willReceivedOp =
                a_node.Children().front()->Sources().front();
            a_node.Sources().clear();
        }
    else
        {
            // Here we must have only one source, otherwise the 'empty
            // source' list was not empty
            DevAssert(a_node.Sources().size() == 1, ("Coherency problem"));
            DbgPrintTailmerge((debugOutput, "Node has only one source. "
                               "Use it has target block.\n"));
            willReceivedOp = a_node.Sources().front();
            a_node.Sources().clear();
        }
    if(toBeRemoved)
        {
            RemoveBBs(CFG(), *toBeRemoved);
        }
    return willReceivedOp;
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::UpdateSources(TmNode& a_node,
                                                      BasicBlock& a_target,
                                                      BasicBlock& a_origBb)
{
    typename TmNode::ItBasicBlocks it;
    typename TmNode::BasicBlocks& sources = a_node.Sources();
    for(it = sources.begin(); it != sources.end(); ++it)
        {
            DbgPrintTailmerge((debugOutput, "Add a goto from BB%d to BB%d.\n",
                               BasicBlockId(CFG(), **it),
                               BasicBlockId(CFG(), a_target)));
            ReplaceJump(CFG(), **it, a_target, a_origBb, !CreateBb());
        }
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::
 UpdateParentsSourcesList(const TmNode& a_node,
                          const typename TmNode::BasicBlocks& a_sources,
                          BasicBlock& a_target)
{
    TmNode* tmp = a_node.Parent();
    while(tmp)
        {
            UpdateNodeSourcesList(*tmp, a_sources, a_target);
            tmp = tmp->Parent();
        }
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CTailmerge<Cfg, BasicBlock, Operation>::
 UpdateNodeSourcesList(TmNode& a_node,
                       const typename TmNode::BasicBlocks& a_sources,
                       BasicBlock& a_target)
{
    DbgPrintTailmerge((debugOutput, "*** Start node processing\n"));
    typename TmNode::CItBasicBlocks it;
    for(it = a_sources.begin(); it != a_sources.end(); ++it)
        {
            typename TmNode::ItBasicBlocks nodeIt;
            for(nodeIt = a_node.Sources().begin(); nodeIt !=
                    a_node.Sources().end(); ++nodeIt)
                {
                    if(*nodeIt == *it)
                        {
                            DbgPrintTailmerge((debugOutput, "Remove BB%d from "
                                               "list of sources\n",
                                               BasicBlockId(CFG(), **nodeIt)));
                            a_node.Sources().erase(nodeIt);
                            break;
                        }
                }
        }
    typename TmNode::ItBasicBlocks nodeIt;
    bool result = true;
    for(nodeIt = a_node.Sources().begin(); result && nodeIt !=
            a_node.Sources().end(); ++nodeIt)
        {
            result &= *nodeIt != &a_target;
        }
    if(result)
        {
            DbgPrintTailmerge((debugOutput, "Add BB%d in list of sources\n",
                               BasicBlockId(CFG(), a_target)));
            a_node.Sources().push_back(&a_target);
        }
    DbgPrintTailmerge((debugOutput, "*** End node processing\n"));
}

template<typename Cfg, typename BasicBlock, typename Operation>
typename CTailmerge<Cfg, BasicBlock, Operation>::TmNode*
CTailmerge<Cfg, BasicBlock, Operation>::SetEquivalence(TmNode& a_node,
                                                       BasicBlock& bb,
                                                       const Ids& a_hasNotJump)
{
    TmNode* result = NULL;
    if(!IsEmpty(bb))
        {
            DbgPrintTailmerge((debugOutput, "Source BB is:\nBB: %d\n",
                               BasicBlockId(CFG(), bb)));
            Operation* lastBbOp = GetLastOp<BasicBlock, Operation>(bb);

            ItTmNodes it;
            for(it = a_node.Children().begin(); it != a_node.Children().end();
                ++it)
                {
                    if(AreEquivalent((*it)->EquivOp(), lastBbOp))
                        {
                            DbgPrintTailmerge((debugOutput, "Find equivalent"
                                               " operation.\n"));
                            DbgTailmerge(DumpOperation,
                                         (debugOutput, (*it)->EquivOp()));
                            break;
                        }
                }
            if(it == a_node.Children().end())
                {
                    DbgPrintTailmerge((debugOutput, "No equivalence class found"
                                       ".\nCreate a new one for OP:\n"));
                    TmNode tmp(MemPool(), &a_node, lastBbOp);
                    it = a_node.AddChild(tmp);
                }
            else
                {
                    (*it)->AddToEquivClass(lastBbOp);
                }
            DbgTailmerge(DumpOperation, (debugOutput, lastBbOp));
            RemoveOp(CFG(), bb, lastBbOp);
            (*it)->AddSource(bb, a_hasNotJump.find(BasicBlockId(CFG(), bb)) !=
                             a_hasNotJump.end());
            result = *it;
        }
    return result;
}


//----------------------
// CExtendedTailmerge class
//----------------------
template<typename Cfg, typename BasicBlock, typename Operation>
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
 CExtendedTailmerge(Cfg& a_cfg,
                    bool a_hasExplicitJump,
                    bool a_createBb,
                    bool a_trace,
                    bool a_before,
                    MEM_POOL* a_memPool,
                    bool a_simplifyFlow)
    : CTailmerge<Cfg, BasicBlock, Operation>(a_cfg, a_hasExplicitJump,
                                             a_createBb, a_trace, a_before,
                                             a_memPool),
      m_simplifyFlow(a_simplifyFlow)
{
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::Optimize()
{
    if(SimplifyFlow())
        {
            SimplifyControlFlowGraph();
        }
    CTailmerge<Cfg, BasicBlock, Operation>::Optimize();
}

template<typename Cfg, typename BasicBlock, typename Operation>
const bool
CExtendedTailmerge<Cfg, BasicBlock, Operation>::SimplifyFlow() const
{
    return m_simplifyFlow;
}

template<typename Cfg, typename BasicBlock, typename Operation>
const bool
CExtendedTailmerge<Cfg, BasicBlock, Operation>::Trace() const
{
    return CTailmerge<Cfg, BasicBlock, Operation>::Trace();
}

template<typename Cfg, typename BasicBlock, typename Operation>
Cfg&
CExtendedTailmerge<Cfg, BasicBlock, Operation>::CFG()
{
    return CTailmerge<Cfg, BasicBlock, Operation>::CFG();
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::SimplifyControlFlowGraph()
{
    typename ExtTmNode::BasicBlocks bbs;
    BBsSimplifyInfo bbsSimplifyInfo;
    InitSimplifyControlFlowGraph(bbsSimplifyInfo, bbs);
    GatherSimplifyInfo(bbsSimplifyInfo, bbs);
    DoSimplifyControlFlowGraph(bbsSimplifyInfo, bbs);
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
 InitSimplifyControlFlowGraph(BBsSimplifyInfo& bbsSimplifyInfo,
			      typename ExtTmNode::BasicBlocks& bbs)
{
    typename ExtTmNode::CItBasicBlocks it;
    DbgPrintTailmerge((debugOutput, "*** Start %s\n", __FUNCTION__));

    GetExitBasicBlocks<Cfg, BasicBlock>(bbs, CFG());

    for(it = bbs.begin(); it != bbs.end(); ++it)
        {
            bbsSimplifyInfo[*it] = SSimplifyInfo(NULL, *it, true, false);
            DbgPrintTailmerge((debugOutput, "Add exit to simplify information: "
                               "BB%d\n", BasicBlockId(CFG(), **it)));
        }
    DbgPrintTailmerge((debugOutput, "*** End %s\n", __FUNCTION__));
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
 GatherSimplifyInfo(BBsSimplifyInfo& bbsSimplifyInfo,
		    typename ExtTmNode::BasicBlocks& bbs)
{
    // We must keep a list of basic blocks even if bbsSimplifyInfo already has
    // this information, because:
    // - using a ++it traversing method does not ensure we visit all elements
    //   of the map (elements may be inserted before current iterator position)
    // - it will insure that the order of the BB traversal done in
    //   DoSimplifyControlFlowGraph() will not depends on BasicBlock addresses.
    //   (Fix for codex bug #39102)
    // Note: the list already contains the exit BBs, inserted in
    //       InitSimplifyControlFlowGraph()
    typename ExtTmNode::ItBasicBlocks it;
    for(it = bbs.begin(); it != bbs.end(); ++it)
        {
            GetPredecessorsList(bbsSimplifyInfo[*it].listOfPreds, CFG(), **it);
            GatherSimplifyInfoBB(bbsSimplifyInfo[*it].listOfPreds, **it, bbs,
                                 bbsSimplifyInfo);
        }
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
 GatherSimplifyInfoBB(const typename ExtTmNode::BasicBlocks& listOfPreds,
                      BasicBlock& succ,
                      typename ExtTmNode::BasicBlocks& bbs,
                      BBsSimplifyInfo& bbsSimplifyInfo)
{
    typename ExtTmNode::CItBasicBlocks it;
    DbgPrintTailmerge((debugOutput, "*** Start %s for BB%d\n", __FUNCTION__,
                       BasicBlockId(CFG(), succ)));

    for(it = listOfPreds.begin(); it != listOfPreds.end(); ++it)
        {
            SSimplifyInfo& simplifyInfo = bbsSimplifyInfo[*it];
            DbgPrintTailmerge((debugOutput, "Check pred: BB%d\n- Visited: %d\n",
                               BasicBlockId(CFG(), **it),
                               (int)simplifyInfo.visited));

            if(!simplifyInfo.visited)
                {
                    simplifyInfo.visited = true;
                    simplifyInfo.succ = &succ;

                    bbs.push_back(*it);

                    if((simplifyInfo.isSimple =
                        IsSimpleBB<Cfg, BasicBlock>(CFG(), **it)))
                        {
                            DevAssert(bbsSimplifyInfo[&succ].target,
                                      ("Succ has not been proceed!!!"));
                            // bb is simple, so its target is the target of its
                            // successor
                            simplifyInfo.target = bbsSimplifyInfo[&succ].target;
                        }
                    else
                        {
                            // No simplification possible, target remains
                            // current examined bb
                            simplifyInfo.target = *it;
                        }
                    DbgPrintTailmerge((debugOutput, "- Simplify info:\n"
                                       " * succ: BB%d\n"
                                       " * target: BB%d\n"
                                       " * isSimple: %d\n",
                                       BasicBlockId(CFG(),
                                                    *simplifyInfo.succ),
                                       BasicBlockId(CFG(),
                                                    *simplifyInfo.target),
                                       (int)simplifyInfo.isSimple));

                }
        }
    DbgPrintTailmerge((debugOutput, "*** End %s for BB%d\n", __FUNCTION__,
                       BasicBlockId(CFG(), succ)));
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
 DoSimplifyControlFlowGraph(const BBsSimplifyInfo& bbsSimplifyInfo,
			    const typename ExtTmNode::BasicBlocks& bbs)
{
    typename ExtTmNode::CItBasicBlocks it;
    typename ExtTmNode::BasicBlocks toBeRemoved;
    for(it = bbs.begin(); it != bbs.end(); ++it)
        {
            if((bbsSimplifyInfo.find(*it)->second).isSimple)
                {
                    DoSimplifyControlFlowGraphBB(**it, bbsSimplifyInfo,
                                                 toBeRemoved);
                }
        }
    RemoveBBs(CFG(), toBeRemoved, true);
}

template<typename Cfg, typename BasicBlock, typename Operation>
void
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
 DoSimplifyControlFlowGraphBB(BasicBlock& simpleBb,
                              const BBsSimplifyInfo& bbsSimplifyInfo,
                              typename ExtTmNode::BasicBlocks& toBeRemoved)
{
    typename ExtTmNode::CItBasicBlocks it;
    const SSimplifyInfo& simplifyInfo = bbsSimplifyInfo.find(&simpleBb)->second;
    bool canBeRemoved = true;
    bool tryUnchain = true;
    DbgPrintTailmerge((debugOutput, "*** Start %s for BB%d\n", __FUNCTION__,
                       BasicBlockId(CFG(), simpleBb)));

    // VL: this is to fix bug #40577 where entry block is wrongly seen as
    // removable due to 1/ the absence of predecessor and 2/ the absence
    // of specific analyis and filtering for this kind of blocks!
    // The underlying problem is that RemoveBBs() does not handle correc-
    // cases where a block to be removed has implicit predecessor as it
    // is the case for entry block (unlink/relink operations may be wrong
    // in this case).

    if( (IsEntryBB<Cfg, BasicBlock>(CFG(), simpleBb)) ) canBeRemoved=false;

    for(it = simplifyInfo.listOfPreds.begin();
        it != simplifyInfo.listOfPreds.end(); ++it)
        {
            const SSimplifyInfo& predInfo = bbsSimplifyInfo.find(*it)->second;
            DbgPrintTailmerge((debugOutput, "Examined BB%d\n",
                                       BasicBlockId(CFG(), **it)));

            if(*it == &simpleBb)
                {
                    DbgPrintTailmerge((debugOutput, "-> BB%d not removable\n",
                                       BasicBlockId(CFG(), simpleBb)));
                                                                                                            
                    canBeRemoved = false;
                }

            if( !predInfo.isSimple ) 
		{

                    DbgPrintTailmerge((debugOutput, "Tries to replace simple jump for src=BB%d tgt=BB%d org=BB%d\n",
                               BasicBlockId(CFG(), **it),
                               BasicBlockId(CFG(), *(simplifyInfo.target)),
                               BasicBlockId(CFG(), simpleBb )));

                    // We check if target block already has the source block as predecessor.
                    // In this case we will not remove it and rely on next pass of CFG jump to
                    // jump elimination for cleanup (#45232) - Since ReplaceSimpleJump may
                    // actually modify edges, it must be called after tryUnchain is evaluated

                    tryUnchain=!IsInPred(**it, *(simplifyInfo.target));

                    if(!tryUnchain || 
                       !ReplaceSimpleJump(CFG(), **it, *(simplifyInfo.target), simpleBb) )
                        {
                            DbgPrintTailmerge((debugOutput, "-> BB%d not removable\n",
                                              BasicBlockId(CFG(), simpleBb)));
                                                                                                            
                            canBeRemoved = false;
                        }
		    else  
                        {
                            DbgPrintTailmerge((debugOutput, "-> BB%d possibly removable\n",
                                              BasicBlockId(CFG(), simpleBb)));
                        }
                }
        }

    if(canBeRemoved)
        {
            DbgPrintTailmerge((debugOutput, "Set BB%d as removable\n",
                               BasicBlockId(CFG(), simpleBb)));
            toBeRemoved.push_back(&simpleBb);
        }
    else
        {
            DevAssert(simplifyInfo.succ, ("A basic block cannot be simple if it"
                                          " has not a successor"));

	    DbgPrintTailmerge((debugOutput, "Simple jump replaced for src=BB%d tgt=BB%d org=BB%d\n",
                               BasicBlockId(CFG(), simpleBb), 
			       BasicBlockId(CFG(), *(simplifyInfo.target)), 
			       BasicBlockId(CFG(), *(simplifyInfo.succ)) ));

            ReplaceSimpleJump(CFG(), simpleBb, *(simplifyInfo.target),
                              *(simplifyInfo.succ));
        }
    DbgPrintTailmerge((debugOutput, "*** End %s for BB%d\n", __FUNCTION__,
                       BasicBlockId(CFG(), simpleBb)));
}

//------------------------------------------------------------------------------
// Function definition
//------------------------------------------------------------------------------

template<typename Cfg, typename BasicBlock, typename Operation>
bool
CExtendedTailmerge<Cfg, BasicBlock, Operation>::
  IsInPred(BasicBlock& pred_bb, BasicBlock& target_bb)
{
    bool     isInPred = false;
    typename ExtTmNode::BasicBlocks listOfPreds;
    typename ExtTmNode::ItBasicBlocks itBb;


    GetPredecessorsList(listOfPreds, CFG(), (target_bb));

    for(itBb = listOfPreds.begin(); itBb != listOfPreds.end(); ++itBb)
        {
            DbgPrintTailmerge((debugOutput, "Check same target for src and org at BB%d\n",
                                             BasicBlockId(CFG(), **itBb) ));
            if( BasicBlockId(CFG(), **itBb) == BasicBlockId(CFG(), pred_bb ) )
                {
                    DbgPrintTailmerge((debugOutput, "Same target for src and org at BB%d\n",
                                                     BasicBlockId(CFG(), **itBb) ));
                    isInPred = true;
                }
        }

    return(isInPred);
}

template<typename List>
static bool
CheckSame(const List& l1, const List& l2)
{
    bool result = l1.size() == l2.size();
    typename List::const_iterator it, it2;
    for(it = l1.begin(), it2 = l2.begin();
        it != l1.end() && result; ++it, ++it2)
        {
            result &= *it == *it2;
        }
    return result;
}

} // End namespace TailmergeSpace

#endif
