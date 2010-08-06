//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_tail.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  14-MAR-96 - Original Version
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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

   Special thanks goes to SGI for their continued support to open source

*/
//
// ====================================================================
//
// Description:
//
// ====================================================================
// ====================================================================


#ifndef opt_tail_INCLUDED
#define opt_tail_INCLUDED "opt_tail.h"


#ifdef _KEEP_RCS_ID
static char *opt_tail_rcs_id = opt_tail_INCLUDED" $Revision$";
#endif /* _KEEP_RCS_ID */


#include "defs.h"
#include "opt_cfg.h"
#include "opt_sym.h"
#include "wn.h"
#ifdef TARG_ST
#   include <list>
#endif


//  OPT_TAIL: container for tail recursion optimization

class OPT_TAIL {
private:
  OPT_TAIL(void);
  OPT_TAIL(const OPT_TAIL&);
  OPT_TAIL& operator = (const OPT_TAIL&);

  BOOL		_do_trace;
  CFG 		*_cfg;
  OPT_STAB	*_opt_stab;
  BB_NODE	*_entry_bb;
  WN		*_entry_wn;
  WN		*_call_wn;
  WN		*_ret_ldid_wn;
  WN            *_ret_ldid_wn1;
  WN		*_ret_stid_wn;
  WN            *_ret_stid_wn1;
  BB_NODE	*_label_bb;
  WN		*_top_label;
#ifdef TARG_ST
    
    /**
     * Enumeration used to represent the status of MayBeReturnValueLoadOrStore
     * function.
     */
    enum EIsReturnVal
        {
            YES,
            NO,
            WITH_MODIFICATION
        };

    /**
     * Structure used to perform and store a simple modification on a basic
     * block.
     * A simple modification is a replacement of an instruction by an other plus
     * the modification of the basic block kind.
     * Modifications are revertable, unless related basic block state change.
     * I.e. the basic block configuration must be the same (it can change during
     * the process, but it must be the same at the undo operation), otherwise,
     * restoration may not be possible.
     *
     * @see Undo and Modification constructor for more details
     */
    struct Modification
    {
        /**
         * Basic block on which the modification applied
         */
        BB_NODE* bb;

        /**
         * Old kind of the basic block. Used to revert modification
         */
        enum BB_KIND oldKind;

        /**
         * Old operation which is now current. Used to revert modification
         */
        WN* old;

        /**
         * Replacement of old operation. This operation is currently in bb.
         * Used to revert modification
         */
        WN* current;

        /**
         * Default constructor.
         * Replace a_old by a_replacement in a_bb and set a_bb kind to
         * a_newKind. These modifications can be reverted using Undo method.
         * Some special cases are to be considered:
         * @li a_old is nil and a_replacement is not: In that case the
         *     modification is considered to be an appending of a_replacement to
         *     a_bb
         * @li a_old is not nil and a_replacement is nil: In that case the
         *     modification is considered to be the removing of the last
         *     operation of a_bb. Why the last? Simply because if it is not the
         *     case, we cannot revert modification, since we will not know where
         *     to insert the old operation.
         *
         * @param  a_bb Basic block which will be modified
         * @param  a_newKind New kind of the basic block after the application
         *         of the modification
         * @param  a_old Statement currently in a_bb to be replaced by
         *         a_replacement
         * @param  a_replacement Statement that will replace a_old
         *
         * @pre    a_bb and a_old implies a_old is in a_bb
         * @post   (a_bb implies (a_replacement implies a_replacement is in
         *         a_bb) and a_bb->Kind() = a_newKind) and bb = a_bb and
         *         old = a_old and a_replacement = current and oldKind =
         *         a_bb@pre->Kind()
         */
        Modification(BB_NODE* a_bb = NULL, enum BB_KIND a_newKind = BB_GOTO,
                     WN* a_old = NULL, WN* a_replacement = NULL);

        /**
         * Undo modification.
         * This revert operation performs the opposite operation done on bb at
         * creation phase or since last undo call.
         * To get the same bb as given at creation (before modification) or last
         * call to undo, bb state must be the same (i.e. relative position of
         * the current operation)
         *
         * @pre    bb <> NULL and current implies current is in bb
         * @post   bb->Kind() = oldKind@pre and if old@pre and current@pre then
         *         old@pre replaces current@pre in bb else if old@pre and not
         *         current@pre then old@pre is the last operation of bb else
         *         if not old@pre and current@pre then bb = bb@pre minus the
         *         last operation else no operations changes endif endif endif
         *         and oldKind = bb@pre->Kind() and old = current@pre and
         *         current = old@pre
         */
        void
        Undo();
    };

    // Shortcuts definition
    typedef std::list<Modification> Modifications;
    typedef Modifications::iterator ItModifications;
    typedef Modifications::reverse_iterator RItModifications;

    typedef std::list<BB_NODE*> BBs;
    typedef BBs::iterator ItBBs;

    /**
     * List of modifications member. This member is used to store modifications
     * done to extend tail recursion application cases.
     */
    Modifications _modifiedBbs;

    /**
     * Prepare Cfg for tail recursion optimization.
     * The preparation consists in adding return statement just after function
     * calls (recursive call), when it is possible (no usefull operations
     * between call and return statements), to allow more applicable cases.
     * Assume following case:
     * <pre>
     * void func1()
     * {
     *     ...
     *     if(...)
     *     {
     *         func2();
     *     }
     *     else
     *     {
     *         // recursive call
     *         func1();
     *     }
     * }
     * </pre>
     * Current implementation (without PrepareCfg) is not capable to apply tail
     * recursion on func1.
     * If you rewrite the previous code like this
     * <pre>
     * void func1()
     * {
     *     ...
     *     if(...)
     *     {
     *         func2();
     *     }
     *     else
     *     {
     *         // recursive call with explicit (useless) return
     *         return func1();
     *     }
     * }
     * </pre>
     * Tail recursion will work.
     * To sum up, this is what PrepareCfg does.
     *
     * @pre    true
     * @post   Function recursive calls which had a direct path to return of the
     *         function are followed by a return statement
     */
    void
    PrepareCfg();
    
    /**
     * Finalize Cfg.
     * The finalization of the Cfg consists in removing modifications done by
     * PrepareCfg, when these modifications have not led to a tail recursion
     * optimization
     *
     * @pre    true
     * @post   true
     */
    void
    FinalizeCfg();

    /**
     * Getter for exits basic blocks.
     * Retrieve the list of exit basic blocks from _cfg
     *
     * @param  exitBBs [out] Will receive the exit basic block
     *
     * @pre    true
     * @post   exitBBs = exitBBs@pre->Including(_cfg->select(bb |
     *         bb->Kind() == BB_EXIT))
     */
    void
    GetExitBlocks(BBs& exitBBs) const;

    /**
     * Find recursive calls candidates to tail recursion optimization for given
     * exit basic block.
     * Candidates are on simple path to given exit basic block and will receive
     * a return statement
     *
     * @param  exitBB Exit basic block start point for simple path finding
     * @param  returnValLoadStore Statement that represents the load store
     *         operation of the return value. NULL for void function
     *
     * @pre    exitBB <> NULL and exitBB->Kind() = BB_EXIT and
     *         returnValLoadStore = if _cfg represents a void function then NULL
     *         else valid load store statement for return value endif
     * @post   _modifiedBbs contains the modifications done on candidates
     */
    void
    FindCandidates(BB_NODE* exitBB, WN* returnValLoadStore);

    /**
     * Test if given statement is a load store of the return value.
     * Status is defined like this:
     * @li YES: stmt is a store load operation (a store operation with a load
     *     child) which either store in return value or load from return value
     *     (according to countLoad and countStore information)
     * @li NO: stmt is not a store load operation or (does not store in return
     *     value and does not load return value and does not store in local
     *     variable that is used to built return value (according to
     *     returnValLoadStore statement))
     * @li WITH_MODIFICATION: stmt is a store load operation which stores
     *     loaded value in loaded value used to build return value
     * The purpose of this function is to determine whether a store load
     * operation is in a simple path to return. When it answers YES, it is, when
     * it answers NO it does not and when it answers WITH_MODIFICATION, it is if
     * you do the proper modification, i.e. replace store temporary by return
     * value.
     *
     * @param  stmt To be tested
     * @param  countStore Count the number of store of the return value
     * @param  countLoad Count the number of load of the return value
     * @param  returnValLoadStore Return value statement
     *
     * @pre    countStore and countLoad are correctly initialized (0 for the
     *         first call, previous value for the other calls on same path)
     * @post   (result = YES implies (stmt is store load operation and
     *         (countStore = countStore@pre + 1 and countLoad = countLoad@pre
     *         and stmt stores in the return value) or
     *         (countLoad = countLoad@pre + 1 and countStore = countStore@pre
     *         and stmt loads return value)))
     *         and (result = NO implies (stmt is not a store load or is not a
     *         store load of return value and countStore = countStore@pre and
     *         countLoad = countLoad@pre)) and (result = WITH_MODIFICATION
     *         implies (stmt is store load operation and
     *         (countStore = countStore@pre + 1 and countLoad = countLoad@pre
     *         and stmt stores in variable that is loaded to build the return
     *         value)))
     *
     * @return The status of stmt regarding store load of return value
     */
    enum EIsReturnVal
    MayBeReturnValueLoadOrStore(WN* stmt, int& countStore, int& countLoad,
                                WN* returnValLoadStore = NULL);

    /**
     * Check whether given bb is a simple block.
     * A simple block is a block that do nothing considered as vital
     * (empty block, simgle goto, etc.). Moreover this function performs the
     * return statement creation for matching call.
     *
     * @param  exitBB Start point of the simple path finding
     * @param  bb Current examined basic block
     * @param  returnValLoadStore Current store load statement (see
     *         MayBeReturnValueLoadOrStore for more details)
     *
     * @pre    bb is in a simple path to exitBB (only simple block separate
     *         them)
     * @post   result = true implies all predecessor of bb are simply branched
     *         (in a simple path) to exitBB
     *
     * @return true if bb is a simple block, false otherwise
     */
    bool
    IsSimpleBlock(BB_NODE* exitBB, BB_NODE* bb, WN* returnValLoadStore);

    /**
     * Undo and remove from _modifiedBbs modifications done on given basic
     * block.
     *
     * @param  bb 
     *
     * @pre    All modifications done on bb are sorted according to the time of
     *         the modification (old to new)
     * @post   _modifiedBbs = _modifiedBbs@pre->excluding(
     *         _modifiedBbs->select(mod | mod.bb = bb))
     */
    void
    UndoAndRemoveModifications(BB_NODE* bb);

    /**
     * Strip return statement of given basic block and store modification in
     * modifiedBBs.
     * Performed modification is appended to modifiedBBs to respect the
     * constraint of UndoAndRemoveModifications method
     *
     * @param  exitBB Basic block to be stripped
     * @param  modifiedBBs [out] List of modifications
     *
     * @pre    exitBB and exitBB->Kind() == BB_EXIT
     * @post   modifiedBBs = modifiedBBs->append(strip modification)
     *
     */
    static void
    StripExitBB(BB_NODE* exitBB, Modifications& modifiedBBs);

    /**
     * Add a return statement at the end of given basic block and set kind to
     * BB_EXIT.
     * Performed modification is appended to modifiedBBs to respect the
     * constraint of UndoAndRemoveModifications method
     *
     * @param  exitBB Basic block to be transformed in exit basic block
     * @param  modifiedBBs [out] List of modifications
     *
     * @pre    exitBB
     * @post   modifiedBBs = modifiedBBs->append(return adding modification)
     */
    static void
    SetExitBB(BB_NODE* exitBB, Modifications& modifiedBBs);
#endif

protected:
  BOOL Entry_is_well_behaved();
  BOOL Exit_is_well_behaved(BB_NODE*);
  void Create_top_label();
  void Fixup_exit(BB_NODE*);

public:
  OPT_TAIL(CFG*, OPT_STAB*);
  ~OPT_TAIL();

  void Mutate();
};


#endif  // opt_tail_INCLUDED
