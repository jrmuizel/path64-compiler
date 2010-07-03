/*
  Copyright (C) 2000-2003, Institute of Computing Technology, Chinese Academy of Sciences
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:
  
  Redistributions of source code must retain the above copyright notice, this list
  of conditions and the following disclaimer. 

  Redistributions in binary form must reproduce the above copyright notice, this list
  of conditions and the following disclaimer in the documentation and/or other materials
  provided with the distribution. 

  Neither the name of the owner nor the names of its contributors may be used to endorse or
  promote products derived from this software without specific prior written permission. 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
//-*-c++-*-

 /* =========================================================================
  * =========================================================================
  * 
  * Module: loop_invar_hoist.cxx
  *
  *
  * Revision comments:
  * 
  *    14-April-2003 - Initial version
  *
  *    FdF: 29-August-2003 - Import from ORC 2.1
  *
  * 
  * Description:
  * ============
  *  
  *    This module is designed to implement loop invariant code motion as 
  *    files' name suggest. It is appllied upon CGIR level and right before
  *    Perform_Loop_Optimization () is invoked. 
  *    
  *    The motivation of this moddule is to pick the chances that can not 
  *    be caught by the earlier loop-invar-code-motion-er which is a 
  *    by-product of PRE (Roy told me so if I have not misconstrued what
  *    he said. If things turn out to be a totally different story, do not
  *    make fuss, since I kown nothing about WOPT and I likely misunderstand
  *    what other people told me).
  *     
  *    The "biggest chances" that earlier LICMer cannt caught is the 
  *    address calculation, e.g "addl r11=@ltoff(seedi#),gp". Those address
  *    calc instructions do not exist before lowering to CGIR, hence 
  *    earlier LICMer has no idea of them.   
  *     
  *    One week ago, I was unfortunately asked by Roy to implement this 
  *    LICMer.  Life is so hard!
  * 
  *    Our algorithm is based on "Steven S.Muchnick, Advanced Compiler 
  *    Design and Implementation,p397-407". For the purpose of clarity, this
  *    algorithm understate alias issues -- it treats all instructions 
  *    as simple ALU calculation. However, a pragmatic LICMer should take 
  *    alias into account. What is more, it should also take an eye on 
  *    architecture specific dependences beteen instructions which could 
  *    not easily observed by bare eye via comparing the TNs explicitly 
  *    defined and used between instructions. 
  *
  *    FdF: 14/10/2003
  *    ---------------
  *
  *    I have added store sinking to this module. This has also an
  *    impact on load hoisting. Here is a description of the
  *    implementation.
  *    
  *    I introduced a new class LI_MEMORY_INFO, to group together
  *    memory operations (load and store) that refer to the same
  *    invariant memory location. These operations are identified by
  *    Identify_Memory_Groups(), called after the first pass of loop
  *    invariant code motion. Then, the algorithm checks if
  *    scalarizations of all load and store in a MEMORY group is valid
  *    and profitable. If it is, load hoisting is performed and store
  *    sinking is performed.
  *  
  * =========================================================================
  * =========================================================================
  */
// [HK]
#if __GNUC__ >= 3
#include <vector>  /* for STL vector and list */
#include <list>
// using std::vector;
// using std::list;
#else
#include <vector.h>  /* for STL vector and list */
#include <list.h>
#endif // __GNUC__ >= 3

#include "tracing.h"

#include "mempool_allocator.h" /* for class mempool_allocator */
#include "cxx_memory.h"        /* for CXX_NEW() */


#include "op.h"
#include "bb.h"
#include "bb_set.h"
#include "freq.h"

#include "gtn_universe.h"    /* for GTN_SET_xxx() */
#include "gtn_set.h"
#include "register.h"
#include "gra_live.h"

#include "dominate.h"   /* for BB_dom_set() */
#include "cgtarget.h"   /* for CGTARG_Can_Be_Speculative() */  
#include "cg_dep_graph.h" /* for dependence info */
#ifndef TARG_ST
#include "error.h"
#endif
#include "cg_loop.h"    /* for LOOP_DESCR */

#ifdef TARG_ST
#include "cgexp.h"
#include "config_TARG.h"
#include "cg_select.h"
#include "cg_affirm.h"

INT32 IPFEC_Enable_LICM = 0;
INT32 IPFEC_Enable_LICM_passes = 0;
#endif

/* Tracing flags. */
static BOOL LICM_Trace_Non_Profitable;
static BOOL LICM_Trace_Profitable;
static BOOL LICM_Trace_Candidates;
static char diagnostic[1024];

/* Parameters to the LICM. */
//#define OLD_PARAMETERS
#ifdef OLD_PARAMETERS
#define TINY_LOOP_SIZE_MAX 8
#define SMALL_LOOP_SIZE_MAX 15
#define MEDIUM_LOOP_SIZE_MAX 31
#define LARGE_LOOP_SIZE_MAX 127
#define HOT_LOOP_FREQ_MIN 0
#define VERY_HOT_LOOP_FREQ_MIN (1000000-1)
#define LOOP_MAX_BB 8
#define HOT_LOOP_MAX_BB 10
#else
#define TINY_LOOP_SIZE_MAX	8
#define SMALL_LOOP_SIZE_MAX	64
#define MEDIUM_LOOP_SIZE_MAX 256
#define LARGE_LOOP_SIZE_MAX 1024
#define HOT_LOOP_FREQ_MIN 10
#define VERY_HOT_LOOP_FREQ_MIN 1000 
#define LOOP_MAX_BB 8
#define HOT_LOOP_MAX_BB 10
#endif


    /* STL OP vector */
typedef mempool_allocator<OP*> OP_ALLOC;
typedef std::vector<OP*,OP_ALLOC>   OP_Vector;
typedef OP_Vector::iterator    OP_Vector_Iter;

    /* STL BB list */
typedef mempool_allocator<BB*> BB_ALLOC;
#ifdef TARG_ST
/* (cbr) use bb* allocator */ 
typedef std::list<BB*,BB_ALLOC>     BB_Lst;
#else
typedef std::list<BB*,OP_ALLOC>     BB_Lst;
#endif
typedef BB_Lst::iterator       BB_Lst_Iter;

     /* class LI_TN_INFO :
      *     keep track of definition and use of TNs.
      */
class LI_TN_INFO {
private:
    OP_Vector _def_list;    /* {op|op define the corresponding TN } */
    OP_Vector _use_list;    /* {op|op use the corresponding TN}     */
    INT32     _inloop_def_num; /* # of in-loop-OPs that def this TN */    
    INT32     _inloop_use_num; /* # of in-loop-OPs that use this TN */  
    mBOOL     _loop_invar;  /* TRUE iff corrsponding TN is loop invar */

public:
    
    LI_TN_INFO (MEM_POOL* mp): 
        _def_list(mp), _use_list(mp), _loop_invar(FALSE), 
        _inloop_def_num(0), _inloop_use_num(0) {};
    ~LI_TN_INFO (void) {} ;

    void Add_Def_OP (OP* o) { _def_list.push_back (o); }
    void Add_Use_OP (OP* o) { _use_list.push_back (o); }

    const OP_Vector* Defs (void) const { return &_def_list; }   
    const OP_Vector* Uses (void) const { return &_use_list; }

    BOOL Is_Loop_Invar  (void) const { return _loop_invar; } 
    void Set_Loop_Invar (void)       { _loop_invar = TRUE; }
    void Reset_Loop_Invar (void)     { _loop_invar = FALSE;}

    INT32 Inloop_Def_Num (void) const { return _inloop_def_num;   }
    INT32 Dec_Inloop_Def_Num (void)   { return --_inloop_def_num; }
    INT32 Inc_Inloop_Def_Num (void)   { return ++_inloop_def_num; }

    INT32 Inloop_Use_Num (void) const { return _inloop_use_num;   }
    INT32 Dec_Inloop_Use_Num (void)   { return --_inloop_use_num; }
    INT32 Inc_Inloop_Use_Num (void)   { return ++_inloop_use_num; }
};

    /* =============================================================
     * =============================================================
     * 
     *  class LOOP_TOPO_ITER
     * 
     *    iterator used to topologically traverse loop. 
     *
     * =============================================================
     * =============================================================
     */ 
class LOOP_TOPO_ITER {
private:
    MEM_POOL*   _mp;
    BB_SET*     _visit; /* _visit: BB -> boolean */
    LOOP_DESCR* _loop;  /* the loop in question */  
    BB*         _cur;   /* currently visited node */
    BB_Lst      _seq;   /* visit sequence */

public:
    LOOP_TOPO_ITER (LOOP_DESCR* l, MEM_POOL* mp);
    ~LOOP_TOPO_ITER (void) {};
    BOOL end   (void) const { return _seq.empty (); }
    
    LOOP_TOPO_ITER& operator++ (void);  // prefix inc 
    // LOOP_TOPO_ITER& operator++ (int);   // postfix inc

    BB*  operator*(void) const { return _cur; }
};

    /* =============================================================
     * =============================================================
     *
     * class LI_OP_INFO and LI_OP_INFO_MGR 
     *
     *  Bind OP within loop with some essential info as described by 
     *  LI_OP_INFO which is managed under LI_OP_INFO_MGR.
     *
     * =============================================================
     * =============================================================
     */
#ifdef TARG_ST
class LI_MEMORY_INFO;
#endif
class LI_OP_INFO_MGR;
class LI_OP_INFO {
friend class LI_OP_INFO_MGR;
private:
    mBOOL  _def_loop_invar;
    INT16  _level;  /* level(op) = +-- 0 : op is loop entry point
                     *             |-- max{level(pred)} + 1:otherwise.
                     *             +-- -1: invalid
                     */                
#ifdef TARG_ST
    LI_MEMORY_INFO * _memory; /* Memory info for this operation. */
#endif

    void Set_Level (INT32 l) { _level = (INT16)l; }

public:
#ifdef TARG_ST
    LI_OP_INFO  (void): _def_loop_invar(FALSE), _level(0), _memory(NULL) {};
#else
    LI_OP_INFO  (void): _def_loop_invar(FALSE), _level(0) {};
#endif
    ~LI_OP_INFO (void) {};
    BOOL Def_Loop_Invar (void) const { return _def_loop_invar; }
    void Set_Def_Loop_Invar   (void) { _def_loop_invar = TRUE; }
    void Reset_Def_Loop_Invar (void) { _def_loop_invar = FALSE;}

    INT32 Level         (void) const { return (INT32)_level; }
    BOOL  Invalid_Level (void) const { return _level == -1; } 
#ifdef TARG_ST
    void Set_Memory_Group (LI_MEMORY_INFO *memory) { _memory = memory; }
    LI_MEMORY_INFO *Get_Memory_Group (void) { return _memory; }
#endif
};

class LI_OP_INFO_MGR {
private:
    MEM_POOL*   _mp;
    LOOP_DESCR* _loop;

    INT16       _bb_num;    // this many blocks in loop  
    INT16       _op_num;    // this many ops in loop.  
    INT16       _max_level; //

    typedef struct bb_map_bb_pair{
        BB_OP_MAP op_info; 
        BB* bb; 
    } BB_MAP_BB_PAIR;
    BB_MAP_BB_PAIR* _info;     // (_info[i]) : OP -> LI_OP_INFO 

            /* get BB_MAP_BB_PAIR associated with <b> */
    inline const BB_MAP_BB_PAIR* Idx (BB* b) const; 

    void Init_OP_Level (void);

public:
    LI_OP_INFO_MGR (LOOP_DESCR* l, MEM_POOL* mp);
    ~LI_OP_INFO_MGR (void) {} ;

    LI_OP_INFO* Get (OP*) const;
    void        Set (OP*, LI_OP_INFO*);

    INT32 BB_Num    (void) const { return (INT32)_bb_num;  } 
    INT32 OP_Num    (void) const { return (INT32)_op_num;  }
    INT32 Max_Level (void) const { return (INT32)_max_level;}
    LOOP_DESCR *LoopDescr (void) const { return _loop;}

    BOOL It_is_Tiny_Loop  (void) const {return _op_num <= TINY_LOOP_SIZE_MAX;}
    BOOL It_is_Small_Loop (void) const {return _op_num <= SMALL_LOOP_SIZE_MAX && _op_num > TINY_LOOP_SIZE_MAX;}
    BOOL It_is_Mediate_Sized_Loop (void) const 
                    { return _op_num <= MEDIUM_LOOP_SIZE_MAX && _op_num > SMALL_LOOP_SIZE_MAX; }
    BOOL It_is_Large_Loop (void) const { return _op_num <= LARGE_LOOP_SIZE_MAX && _op_num > MEDIUM_LOOP_SIZE_MAX; }
    BOOL It_is_Huge_Loop (void) const  { return _op_num > LARGE_LOOP_SIZE_MAX; }
    BOOL Hot_Loop (void) const { 
                    BB* b = LOOP_DESCR_loophead(_loop); 
                    return BB_freq(b) >= HOT_LOOP_FREQ_MIN;
         }
    BOOL Very_Hot_Loop (void) const { 
                    BB* b = LOOP_DESCR_loophead(_loop); 
                    return BB_freq(b) >= VERY_HOT_LOOP_FREQ_MIN;
         }
};

    /* ==============================================================
     *
     * Init_OP_Level()
     *   level-func is such termed:
     *     level(op) = +-- 0 : op is loop entry point
     *                 |-- max{level(pred)} + 1:otherwise.
     *                 +-- -1: invalid
     *   we use level() to "guesstimate" register pressure.
     * 
     * =============================================================
     */
void
LI_OP_INFO_MGR :: Init_OP_Level (void) {

        /* Loop each BB in topolocal order 
         */
    INT32 start_level = 0;
    BB* header = LOOP_DESCR_loophead (_loop);

    for (LOOP_TOPO_ITER iter(_loop, _mp); !iter.end (); ++iter) {
        BB* cur_bb = *iter;

            /* determine level(BB_first_op(cur_bb)) 
             */
        if (cur_bb != header) {
            BBLIST* P;
            FOR_ALL_BB_PREDS (cur_bb,P) {
                BB* pred = BBLIST_item(P); 
                if (BB_loop_head_bb (pred) != header) {
                    /* it is not in this loop */
                    continue; 
                }

                OP* last = BB_last_op (pred);
                if (last) {
                    INT32 l = Get(last)->Level ();
                    start_level = max(start_level, l);
                }
            }
        }

        OP* op;
        FOR_ALL_BB_OPs (cur_bb, op) {
            LI_OP_INFO* oi = Get(op);
            oi->Set_Level (start_level++);
        }

    }
}

LI_OP_INFO_MGR :: LI_OP_INFO_MGR (LOOP_DESCR* l, MEM_POOL* mp): 
    _loop (l), _mp(mp) {

    BB_SET* bbs = LOOP_DESCR_bbset (l);
    BB* bb; 
    _bb_num = 0 ; 
    _op_num = 0;

        /* 1. get the loop body size 
         */
    FOR_ALL_BB_SET_members (bbs, bb) {
        _bb_num ++;  _op_num += BB_length(bb); 
    }
    
        /* 2. Initialize mapping data structure. 
         */
    _info = TYPE_MEM_POOL_ALLOC_N (BB_MAP_BB_PAIR, _mp, _bb_num); 
    INT i = 0;
    FOR_ALL_BB_SET_members (bbs, bb) {
        _info[i].op_info = BB_OP_MAP_Create (bb, _mp);
        _info[i].bb = bb; 

        OP* o;
        FOR_ALL_BB_OPs (bb, o) {
            LI_OP_INFO* oi = CXX_NEW (LI_OP_INFO, _mp);
            BB_OP_MAP_Set (_info[i].op_info, o, oi); 
        }

        i ++;
    }

        /* bubble sort BB_OP_MAPs so that those smaller corresponding
         * BB-id is permuted before the larger one. This sort is 
         * actually leads into NOP because "FOR_ALL_BB_SET_members"
         * visit BB of smaller ID before that of larger one. 
         */
    for (INT i1 = _bb_num - 2; i1 >= 0; i1 --) {
        for (INT i2 = i1; i2 >= 0 ; i2 --) {
            if (BB_id(_info[i2+1].bb) < BB_id(_info[i2].bb)) {
                    /* swap them */
                BB_MAP_BB_PAIR t = _info[i2];
                _info[i2] = _info[i2+1]; _info[i2] = t;
            }
        }
    }

    Init_OP_Level ();
}
      
    /* ========================================================
     *
     * Idx (BB* b):
     *      return  BB_MAP_BB_PAIR associated with <b>
     *
     * ========================================================
     */
inline const LI_OP_INFO_MGR::BB_MAP_BB_PAIR* 
LI_OP_INFO_MGR :: Idx (BB* b) const {
    
    mBB_NUM id = BB_id (b);
    INT32 low=1, high=_bb_num, mid;

    do {
        mid = (low + high)/2;
        mBB_NUM t = BB_id (_info[mid-1].bb);
        
        if (t == id) return _info+mid-1;
        if (t < id)  { low = mid + 1; continue; }
        high = mid - 1;
    } while (low <= high);

    Is_True (FALSE, ("No BB_MAP_BB_PAIR associated with BB:%d", id));
    return NULL; // make compiler happy. 
}

    /* ==========================================================
     * 
     * Return <op> corresponding LI_OP_INFO
     *
     * =========================================================
     */
LI_OP_INFO* 
LI_OP_INFO_MGR :: Get (OP* op) const {

    const BB_MAP_BB_PAIR* t = Idx (OP_bb(op));
    return (LI_OP_INFO*) BB_OP_MAP_Get (t->op_info, op);
}

    /* =========================================================
     * 
     * Bind <info> with <op>  
     * 
     * =========================================================
     */
void
LI_OP_INFO_MGR :: Set (OP* op, LI_OP_INFO* info) {
    const BB_MAP_BB_PAIR* t = Idx (OP_bb(op));
    BB_OP_MAP_Set (t->op_info, op, info);
}

LOOP_TOPO_ITER :: LOOP_TOPO_ITER (LOOP_DESCR* l, MEM_POOL* mp) : _mp(mp) {
    _loop = l;

    _cur = LOOP_DESCR_loophead(l);
    _seq.push_back (_cur);
    _visit = BB_SET_Create_Empty (252, _mp);
}

    /* advance */
LOOP_TOPO_ITER&
LOOP_TOPO_ITER :: operator++ (void) {

    if (end ()) { _cur = NULL; return *this; }

    Is_True (_cur == *_seq.begin (), 
("currently visited (BB:%d) is not the first element of visit-sequence", 
             BB_id(_cur)));

    BB_SET* body = LOOP_DESCR_bbset(_loop);
    BB* head =  LOOP_DESCR_loophead(_loop);

    _visit = BB_SET_Union1D (_visit, _cur, _mp);
    _seq.pop_front (); 

    BBLIST *s,*p;
    FOR_ALL_BB_SUCCS(_cur, s) {

        BB* succ = BBLIST_item(s);
        if (!BB_SET_MemberP (body,succ)) { continue; }
        if (BB_SET_MemberP (_visit,succ)){ continue; }

        BOOL all_pred_visited = TRUE;
        if (succ != head) {

            FOR_ALL_BB_PREDS (succ, p) {
                BB* pred = BBLIST_item (p);
                if (!BB_SET_MemberP (_visit, pred)) {
                    all_pred_visited = FALSE; break;
                }
            }
        }
        
        if (all_pred_visited) { _seq.push_back (succ); }
    }

    _cur = *_seq.begin ();
    
    return *this;
}


    /* ===========================================================
     * ===========================================================
     * 
     * class LOOP_INVAR_CODE_MOTION  
     *   
     *    Loop invariant identification and code motion.
     *
     * ===========================================================
     * ===========================================================
     */
class LOOP_INVAR_CODE_MOTION : public CXX_MEM_POOL {
    /* 
     * Hint: LI and LICM is acronym for "loop invariant" and 
     *       "Loop invariant code motion" respectively. 
     */
private:
    LOOP_DESCR* _loop;  /* currently handled loop */
#ifdef TARG_ST
    BB_Lst _prolog;      /* corresponding prologue */
#else
    BB* _prolog;      /* corresponding prologue */
#endif
    MEM_POOL* _mp;    /* underlying MEM_POOL*/

    TN_SET*  _def_within_loop; /* all TNs that are defined within loop */
    GTN_SET* _liveout_defs;    /* TNs that live out of loop. */
    
    hTN_MAP _tn_info;          /* f: tn -> class LI_TN_INFO */
    LI_OP_INFO_MGR _op_info;  
    OP_Vector _loop_invar_defs;/* all loop invariants definitions in 
                                * in topological order. */

    BB_Lst  _loop_exits;
    
    OP_Vector _ld_ops,   /* all loads in this loop */ 
              _store_ops,/* all stores in this loop*/ 
              _call_ops; /* all calls in this loop */
#ifdef TARG_ST
    BB* _epilog;	/* corresponding epilogue */
    LI_MEMORY_INFO *_memory_groups; /* List of memory groups. */
    
#endif

    BOOL It_Is_Constant_TN (TN* t) const {
        return TN_is_const_reg(t) || TN_is_constant (t) || t == GP_TN; }

    BOOL It_Is_Loop_Invar_TN (TN* t) const {
            LI_TN_INFO* i = (LI_TN_INFO*)hTN_MAP_Get (_tn_info,t);
            return i->Is_Loop_Invar ();
        }

    LI_OP_INFO* Get_OP_Info (OP* o) const { return _op_info.Get (o); }

    void Sanity_Check (void); /* Abort if this routine does not happy */

    void Calc_Defs_Within_Loop_Body (void);
    void Calc_Liveout_Defs (void);
    void Collect_TN_Def_Use_Info (void);
    void Collect_Ld_St_Call (void);
    void Find_Out_All_Exits (void);
    void Misc_Init (void);
    BOOL Init (void);

    void Mark_Dep_OPs_As_Non_Loop_Invar (OP* op);

    void Identify_Loop_Invariants (void);

        /* Functions that are used to identify loop invariants:
         *
         *  o. TN_is_Loop_Constant (TN* tn)
         *     determine whether given <tn> is loop constant
         *  
         *  o. All_Reaching_Def_are_Outside_Of_Loop(op, opnd_idx)
         *     return TRUE iff all definitions of OP_opnd(op,opnd_idx) that 
         *     reach op is defined outside of loop. 
         * 
         *  o. Unique_Reaching_Def_Inside_Loop (op, opnd_idx)
         *     return TRUE Iff 
         *      - there is exactly one definition of OP_opnd(op,opnd_idx) 
         *        reach <op> 
         *      - the definition in quesion reside within loop body. 
         *
         *  o. Register_Para_Passing(OP*)
         *     determine whether <op> define a register which serve as 
         *     passing parameter to call.
         */ 
    BOOL TN_is_Loop_Constant (TN* tn);
    BOOL All_Reaching_Def_are_Outside_Of_Loop (OP *, INT opnd_idx);
    BOOL Unique_Reaching_Def_Inside_Loop (OP*, INT opdn_idx);
    BOOL Register_Para_Passing (OP* op);
    BOOL Alias_With_Call(OP*, OP* call);
    BOOL It_is_Fake_Loop_Invar (OP*);
    
        /* Functions that are used to perform L.I code motion. 
         *
         * o. Def_Dominate_All_Use (OP* op) 
         *      return TRUE iff loop invariants defined by <op> dominate 
         *      all use of these invariants. 
         *
         * o. Live_Out_Of_Loop (OP* op)
         *      determine whether left-hand-side variables of <op> live out
         *      loop. This function is useful when:
         *            !Def_Dominate_All_Use () && !Dom_All_Loop_Exits ()
         *      at that circumstance, moving <op>, which satisfy 
         *      Live_Out_Of_Loop(), to loop-prologue will produce 
         *      incorrect results. 
         * 
         * o. Code_Motion_Is_Profitable (OP* op)
         *      Determine whether perform L.I.C.M of <op> is profitable or not.   
         */
    BOOL Def_Dominate_All_Use    (OP*);
    BOOL Dom_All_Loop_Exits      (BB* );
    inline BOOL Live_Out_Of_Loop (OP* ) const;
    BOOL Illegal_Or_Nonprofitable (OP* );
    void Perform_Code_Motion     (OP* );

        /* Heuristic
         * 
         *  o Ignore_Loop_With_Few_Interation 
         *    discard some non-interesting loop at very early stage.
         * 
         *  o.Code_Motion_Is_Profitable 
         *    called rigth refore performing code motion.
         */
    BOOL Ignore_Loop_With_Few_Interation (void);
    BOOL Code_Motion_Is_Profitable (OP*);

#ifdef TARG_ST
    void Identify_Memory_Groups (void);
    INT32 Perform_Scalarization (LI_MEMORY_INFO *);
    BOOL Scalarization_Is_Profitable (LI_MEMORY_INFO *, BOOL);
    void Set_Memory_Group (OP *);
    LI_MEMORY_INFO *Get_Memory_Group (OP *);
    BOOL Invariant_Address (OP *);
    BOOL Ambiguous_Alias (OP *);
    LI_MEMORY_INFO *Lookup_Memory (OP *);
    BOOL Dom_All_Loop_Exits (LI_MEMORY_INFO* );
    BOOL is_in_prolog_p (BB *bb) {
      for (BB_Lst_Iter it = _prolog.begin (); it != _prolog.end (); ++it) {
        if (*it == bb) return TRUE;
      }
      return FALSE;
    }
#endif

public:
        /* Construction & Destruction
         */
#ifdef TARG_ST
    LOOP_INVAR_CODE_MOTION  (LOOP_DESCR* l, BB* prolog, BB *epilog);
#else
    LOOP_INVAR_CODE_MOTION  (LOOP_DESCR* l, BB* prolog);
#endif
    ~LOOP_INVAR_CODE_MOTION (void) {} 

    INT32 Perform_Code_Motion (void);

        /* Auxillary functions
         */
    BOOL Loop_Has_Call (void) const  { return Is_Call_Loop(_loop);}
    const LOOP_DESCR* Loop (void) const { return _loop;}
    GTN_SET* Get_liveout_defs(void) { return _liveout_defs; }

    void Dump (FILE* f=stderr);
    #ifdef Is_True_On    
        /* gdb (v5.2) is clumsy at calling a func with parameter being 
         * assigned a default value. Hence "gdb_dump()".
         */
        void gdb_dump (void);  
    #endif 
};


    /* ================================================================= 
     * Calc_Defs_Within_Loop_Body 
     * 
     * Collect all TNs that are defined within loop body. We think all 
     * TNs on the right-hand-side of OP are "defined" are by that OP
     * no matter it is constant or not.
     * =================================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Calc_Defs_Within_Loop_Body (void) {

    BB_SET* bbs = LOOP_DESCR_bbset (_loop);
    BB* bb; 

    FOR_ALL_BB_SET_members (bbs, bb) {
        OP* op;

        FOR_ALL_BB_OPs (bb, op) {
            for (INT i = OP_results (op) - 1; i >= 0; i--) {
                TN* res = OP_result(op, i);
                _def_within_loop = TN_SET_Union1D (_def_within_loop,res, _mp);

            }
        }
    }
}

    /* =================================================================
     * Calc_Liveout_Defs 
     * 
     *  Collect all TNs that are live out of loop. 
     *
     *  CONSTRAINTS:
     *    Loop exits should be identified prior to calling this function. 
     * 
     * =================================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Calc_Liveout_Defs (void) {

    BB_SET* body = LOOP_DESCR_bbset (_loop);

    for (BB_Lst_Iter iter = _loop_exits.begin (); 
         iter != _loop_exits.end (); iter++) {

        BB* exit_blk = *iter;
        BBLIST* s;

        FOR_ALL_BB_SUCCS (exit_blk, s) {
            BB* succ = BBLIST_item (s);
            if (BB_SET_MemberP (body, succ)) { continue ; }
        
            _liveout_defs = 
                GTN_SET_UnionD (_liveout_defs, BB_live_in (succ), _mp);

        } // end of FOR...(exit_blk,s)
    } // end of for(BB_Lst_Iter...)
}


    /* =================================================================
     * Collect_TN_Def_Use_Info 
     *
     *   Find out all def and use of each TN. these info are kept track 
     *   of by LOOP_INVAR_CODE_MOTION::_tn_info which is indiced by 
     *   TN. 
     * =================================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Collect_TN_Def_Use_Info (void) {

    BB_SET* bbs = LOOP_DESCR_bbset(_loop);
    BB* bb; 

    FOR_ALL_BB_SET_members (bbs, bb) {

        OP* op;
        FOR_ALL_BB_OPs (bb, op) {
                /* Acquire left-hand-side TN's information
                 */
            for (INT i = 0 ; i < OP_results(op); i++) {

                TN* res = OP_result(op,i);
                if (It_Is_Constant_TN (res)) { continue; }

                LI_TN_INFO* info;
                info = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, res);

                if (!info) { 
                    info = CXX_NEW(LI_TN_INFO(_mp), _mp); 
                    hTN_MAP_Set (_tn_info, res, (void*)info);
                }

                info->Add_Def_OP (op);
                info->Inc_Inloop_Def_Num ();

            } // end of for(...OP_Results...)

                /* Acquire right-hand-side TN's information
                 */
            for (INT i = 0; i < OP_opnds(op); i++) {
                
                TN* opnd = OP_opnd(op,i);
                if (It_Is_Constant_TN (opnd)) { continue; }

                LI_TN_INFO* info;
                info = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, opnd);

                if (!info) {
                    info = CXX_NEW (LI_TN_INFO(_mp), _mp); 
                    hTN_MAP_Set (_tn_info, opnd, (void*)info);
                }

                info->Add_Use_OP (op);
                info->Inc_Inloop_Use_Num ();

            } // end of for(...OP_opnd...)
        } // end of for ALL_BB_OPs
    } // end of FOR_ALL_BB_SET_members
}

    /* =============================================================
     *
     * Collect_Ld_St_Call 
     * 
     *   use 3 separated list to collect all loads stores and calls.
     *
     * =============================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Collect_Ld_St_Call (void) {

    BB_SET* bbs = LOOP_DESCR_bbset(_loop);
    BB* bb; 

    FOR_ALL_BB_SET_members (bbs, bb) {
        OP* op;
        FOR_ALL_BB_OPs (bb,op) {

	    // [HK] useless ??
 //            extern BOOL OP_like_store(OP *op);

            if (OP_load (op)) { _ld_ops.push_back (op) ; continue; }
            if (OP_like_store (op)) { _store_ops.push_back(op); continue; }
            if (OP_call (op)) {
                _call_ops.push_back (op); 
                    /* findloops.cxx does not set this flag, so we set it 
                     * each time we encount a loop.
                     */
                Set_Call_Loop (_loop);     
            }
        }
    } 
}

    /* ==============================================================
     * Find_Out_All_Exits 
     * 
     *   Find out loop exits and store them <_loop_exits>. function  
     *   create great difficulty in identifing a loop-exit which 
     *   will be ampled later. 
     *   
     *   A sub-routine may break a loop (such as longjmp()) or terminate
     *   program's execution. We conservatively treat any call as a 
     *   loop-exit point. 
     *
     * ==============================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Find_Out_All_Exits (void) {

    BB_SET* bbs = LOOP_DESCR_bbset(_loop);
    BB* bb;

    FOR_ALL_BB_SET_members (bbs, bb) {

        BBLIST *succs;
        FOR_ALL_BB_SUCCS(bb, succs) {
            BB *succ = BBLIST_item(succs);
            if (!BB_SET_MemberP(bbs, succ) && 
                find (_loop_exits.begin(), 
                      _loop_exits.end(), bb) == _loop_exits.end()) {
                _loop_exits.push_back (bb);
            }
        }
    }
}
    
void
LOOP_INVAR_CODE_MOTION :: Misc_Init (void) {
}

    /* ==============================================================
     *
     * Sanity_Check 
     *
     *     this function will abort execution via Is_True () if 
     *     he does not feel happy. 
     *
     * ==============================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Sanity_Check (void) {

#ifdef Is_True_On
        /* 1. Since some code in this package cound upon OP->order field,
         * we need to double check this field are managed well. 
         */
    BB_SET* bbs = LOOP_DESCR_bbset (_loop);
    BB* bb; 

    FOR_ALL_BB_SET_members (bbs, bb) {
        BB_Verify_OP_Order (bb); 
    }
#endif /*Is_True_On*/
}

BOOL
LOOP_INVAR_CODE_MOTION :: Init (void) {

    _tn_info       = hTN_MAP_Create (_mp);

    _def_within_loop = TN_SET_Create_Empty (Last_TN + 1, _mp);
    _liveout_defs    = TN_SET_Create_Empty (Last_TN + 1, _mp);

    Find_Out_All_Exits (); /*should be prior to Calc_Liveout_Defs()*/
    Calc_Defs_Within_Loop_Body ();
    Calc_Liveout_Defs  (); 
    Collect_TN_Def_Use_Info ();
    Collect_Ld_St_Call ();

    Misc_Init ();

    return TRUE;
}

#ifdef TARG_ST
LOOP_INVAR_CODE_MOTION::LOOP_INVAR_CODE_MOTION (LOOP_DESCR* l, BB* prolog, BB* epilog):
#else
LOOP_INVAR_CODE_MOTION::LOOP_INVAR_CODE_MOTION (LOOP_DESCR* l, BB* prolog):
#endif
    CXX_MEM_POOL("Loop Invariant Code Motion", FALSE),
    _mp((*this)()), _loop_exits (_mp), _loop_invar_defs(_mp), 
    _ld_ops (_mp), _store_ops(_mp), _call_ops(_mp),
    _loop(l), _op_info(l, _mp) {

#ifdef TARG_ST
    _prolog.push_back (prolog); 
    _memory_groups = NULL;
    _epilog   = epilog; 
#else
    _prolog = prolog;
#endif

    Init ();
}

    /* ================================================================ 
     * TN_is_Loop_Constant (TN*)
     *
     *  return TRUE if <tn> is loop constant. 
     * 
     * NOTE: 
     *   for simplicity, only constant is identified as "loop constant". 
     * =================================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: TN_is_Loop_Constant (TN* t) {

    if (It_Is_Constant_TN (t)) { return TRUE; }

    LI_TN_INFO* info;
    info = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, t);
    return info->Is_Loop_Invar();
}


    /* ===============================================================
     * All_Reaching_Def_are_Outside_Of_Loop (OP* op, INT opnd_idx)
     *
     *     return TRUE iff all definitions of OP_opnd(op,opnd_idx) that 
     *     reach op is defined outside of loop. 
     * ===============================================================
     */ 
BOOL
LOOP_INVAR_CODE_MOTION :: All_Reaching_Def_are_Outside_Of_Loop 
    (OP* op, INT opnd_idx) {

    TN* opnd = OP_opnd (op, opnd_idx); 
    
    Is_True (opnd, 
        ("OP[%d] (of BB:%d)'s No.%d operand does not exist!", 
          OP_map_idx(op), BB_id(OP_bb(op)), opnd_idx));

        /* Constant literal/reg is deemed as "defined" outside of loop 
         */
    if (It_Is_Constant_TN (opnd)) { return TRUE; }

    return !TN_SET_MemberP (_def_within_loop, opnd);
}

    /* =================================================================
     * Unique_Reaching_Def_Inside_Loop 
     *
     * return TRUE Iff 
     *   o there is exactly one definition of OP_opnd(op,opnd_idx) reach <op> 
     *   o the definition in quesion reside within loop body. 
     * 
     * NOTE: 
     *   Without immediate-dominance information, it is quite hard to 
     *   kown whether a def is acutally reach given point. Hence, in 
     *   this function think any def of variable v can reach any other 
     *   point inside the loop(either through a path within one iteration
     *   of a path across ajacent iternations).
     *
     * TODO:
     *   Current identification of "Uniqueness" is too conservative. 
     * 
     * =================================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: Unique_Reaching_Def_Inside_Loop 
    (OP* op, INT opnd_idx) {

    BB* home = OP_bb (op);

    TN* opnd = OP_opnd (op, opnd_idx);
    LI_TN_INFO* ti = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, opnd);
    OP_Vector* ops = (OP_Vector*)ti->Defs ();

    if (ops->size () != 1) {
        /* TODO: some def may be killed and hence could not reach 
         *    <op>, we should take this case into account.
         */
        return FALSE; 
    }

    OP* def = *ops->begin (); 

        /* def should dominate <opnd>
         */
    if (OP_bb(op) != OP_bb(def)) {
      // FdF: Fixed to !BB_SET_MemberP
        if (!BB_SET_MemberP (BB_dom_set(home), OP_bb(def))) { 
            return FALSE; 
        }
    } else {
        if (OP_Precedes (op,def) || op == def) { return FALSE; }
    }

#ifdef TARG_ST
    // (cbr) Support for guards on false
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    if (OP_has_predicate(op) && OP_opnd(op, pred_index) != True_TN) {
#else
    if (OP_has_predicate(op) && OP_opnd(op, OP_PREDICATE_OPND) != True_TN) {
#endif
        /* TODO: utilize PQS/PRDB to determine whether this statement 
         *   is true or not:
         *     <use>'s guarding predicate being 1 imply that <def>'s
         *     guarding predicate is 1.
         */
        return FALSE;
    }

    return TRUE;
}


    /* =========================================================
     * 
     * Register_Para_Passing 
     * 
     *   return true iff <op> define a regiter which serve as  
     *   passing parameter to sub-call.
     *
     * ===========================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: Register_Para_Passing (OP* op) {

    if (!BB_call(OP_bb(op))) { return FALSE; }

    for (INT i = OP_results(op) - 1; i >= 0; i--) {
        TN* res = OP_result(op,i); 
        if (!TN_register(res)) { continue; }
#ifdef HAS_STACKED_REGISTERS
        if (REGISTER_Is_Stacked_Output (TN_register_class(res), 
                                        TN_register(res))) {
            return TRUE;
        }
#endif
    }

    return FALSE;
}

    /* ===============================================================
     *
     *  Mark_Dep_OPs_As_Non_Loop_Invar 
     *    mark <op> does not compute loop invariants
     *
     * ===============================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Mark_Dep_OPs_As_Non_Loop_Invar (OP* op) {

    for (INT i = OP_results(op) - 1; i >= 0; i--) {

        TN* res = OP_result(op, i);
        if (It_Is_Constant_TN (res)) { continue; }

        LI_TN_INFO* tninfo = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, res);
        OP_Vector* ops = (OP_Vector*)tninfo->Uses ();

#ifdef TARG_ST
	// FdF: Mark the TN as non invariant also.
	tninfo->Reset_Loop_Invar ();
#endif

        for (OP_Vector_Iter iter = ops->begin (); 
             iter != ops->end (); iter++) {

            OP* use = *iter;
            LI_OP_INFO* opinfo = Get_OP_Info (use);
            opinfo->Reset_Def_Loop_Invar ();
        }
    }
}

    /* ===============================================================
     *
     * Identify_Loop_Invariants 
     * 
     *   This routine traverse loop body in topological order. It mark
     *   currently encount OP as "loop invariants definition" if it 
     *   satisfy all following conditions:
     *
     *    1. any src operand V must fall into one of the following category
     *          - V is Loop constant
     *          - all reaching definition of V reside out of loop. 
     *          - There is exactly one definiton of V reach this OP and 
     *            this def locates within loop.
     *    2. it is not a control-transfer (e.g call) instruction. 
     *
     * *IMPORTANT NOTE*:
     *
     *   We omit the issues of alias and the architecture specific dep. 
     *   Attacking these issues is postponed until right before the time 
     *   when we try to peform actual code motion by calling 
     *   It_is_Fake_Loop_Invar(). 
     * 
     *   The motivation of this two phase identifcation of loop-invar is 
     *   that: many fake "loop invariant"s found by this routines are 
     *   either illegal or non-profitable to perform code motion, hence 
     *   we save the time of determining the alias and arch-spec-dep between
     *   these fake loop-invar and other instructions. it seem to be 
     *   a big saving! 
     *
     * ===============================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Identify_Loop_Invariants (void) {
  for (LOOP_TOPO_ITER iter(_loop, _mp); !iter.end (); ++iter) {

        BB* b = *iter;
        OP* op;

        FOR_ALL_BB_OPs (b, op) {
            BOOL it_is; // indidate whether <op> define loop-invar
            it_is = TRUE;

            for (INT i = OP_opnds (op) - 1; i >= 0 && it_is; i--) {

                TN* opnd = OP_opnd(op,i);
		if (!(TN_is_Loop_Constant (opnd)  ||
		      All_Reaching_Def_are_Outside_Of_Loop (op, i) || 
		      (It_Is_Loop_Invar_TN (opnd) && 
		       Unique_Reaching_Def_Inside_Loop (op, i)))) {
		  it_is = FALSE;
		  break;
		}
            }


            if (it_is) {
              if (CGTARG_OP_Has_Loop_Sideeffects(b, op)) {
                  it_is = false;
              }
            }
    
            if (OP_xfer (op) || OP_call (op)) {
                /* branch, obviously, do not define any loop invar
                 */
                it_is = TRUE;
                continue; 
            }

                /* Do not move those instructions that serves as paramater
                 * passing.
                 */
            if (it_is && Register_Para_Passing (op)) {
                it_is = FALSE;
            }

	    // FdF 20060407: This was not checked ! (ddts 25090)
	    if (OP_has_implicit_interactions(op))
	      it_is = FALSE;

	    // FdF 20080724: Do not mov loop invariant AFFIRM
	    // operation out of the loop
	    if (OP_Is_Affirm(op))
	      it_is = FALSE;

#ifdef TARG_ST
	    /*
	     * [CG]: We must first iterate over all results to determine
	     * if the operation is loop invariant or not.
	     * Then only we will mark each results are loop invariant or not.
	     * The original loop did both in the same time which is wrong.
	     * For instance:
	     * R1 , R2 = op ...
	     * If R2 is determined to be loop invariant (Inloop_Def_Num() > 1),
	     * we must mark both R2 _and_ R1 as not loop invariant, even
	     * if R1 is actually loop invariant. 
	     */
            for (INT i = OP_results (op) - 1; i >= 0; i--) {
	      TN* res = OP_result (op, i);
	      
	      if (It_Is_Constant_TN (res)) continue; 
	      
	      LI_TN_INFO* tninfo;
	      tninfo = (LI_TN_INFO*) hTN_MAP_Get (_tn_info, res);
	      if (tninfo->Inloop_Def_Num () != 1) {
		it_is = FALSE; 
	      }
	    }
	    /*
	     * Mark all results TN as non loop invariants if the 
	     * op is non loop invariant.
	     */
            for (INT i = OP_results (op) - 1; i >= 0; i--) {
	      TN* res = OP_result (op, i);
	      
	      if (It_Is_Constant_TN (res)) continue; 

	      LI_TN_INFO* tninfo;
	      tninfo = (LI_TN_INFO*) hTN_MAP_Get (_tn_info, res);
	      if (it_is) {
		tninfo->Set_Loop_Invar ();
	      } else {
		tninfo->Reset_Loop_Invar ();
	      }
	    }

            /* [vcdv] mark register operands which are defined outside
               loop as being loop invariants */
            for (INT i = OP_opnds (op) - 1; i >= 0; i--) {
              TN* opnd = OP_opnd (op, i);
              
              LI_TN_INFO* tninfo = NULL;
              if (TN_is_register(opnd))
                tninfo = (LI_TN_INFO*) hTN_MAP_Get (_tn_info, opnd);
              
              if (tninfo && tninfo->Inloop_Def_Num ()==0) {
                tninfo->Set_Loop_Invar ();
              }
            }
	    
#else /* !TARG_ST */
	    /* [CG] : This loop is wrong, see the replacement code above. */

                /* mark all non-constant results as loop-invariant or 
                 * non-loop-invariant accordingly
                 */
            for (INT i = OP_results (op) - 1; i >= 0; i--) {
                TN* res = OP_result (op, i);

                if (It_Is_Constant_TN (res)) continue; 

                LI_TN_INFO* tninfo;
                tninfo = (LI_TN_INFO*) hTN_MAP_Get (_tn_info, res);
                if (tninfo->Inloop_Def_Num () != 1) { it_is = FALSE; }

                if (it_is) {
                    tninfo->Set_Loop_Invar ();
                } else {
                    tninfo->Reset_Loop_Invar ();
		    /* [CG] : Why is this break for? All results shgould be
		     * reset, not the first one. It seems wrong to me. */
                    break;
                }

             }/* end of for(INT...) */
#endif /* !TARG_ST */
	    
             if (it_is) {
                /* Memorize this loop-invar definition. Since we traverse
                 * the CFG in topolocal order, and each loop-invar-def we 
                 * encount is append to _loop_invar_defs, hence the OPs 
                 * in _loop_invar_defs are also topologically permuted. 
                 */
                _loop_invar_defs.push_back (op);
                _op_info.Get(op)->Set_Def_Loop_Invar ();
             } /* end of if(it_is) */

        } /* end of FOR_ALL_BB_OPs(...) */

    } /* end of for (LOOP_TOPO_ITER...) */
}

BOOL
LOOP_INVAR_CODE_MOTION :: Alias_With_Call (OP* op, OP* call) {

    BOOL alias =  
            !CG_DEP_Can_OP_Move_Across_Call (op, call, TRUE, TRUE) || 
            !CG_DEP_Can_OP_Move_Across_Call (op, call, FALSE, TRUE);

    if (!alias) { return FALSE; } 
    
        /* Sign! CG_DEP_Can_OP_Move_Across_Call() is too conservative to 
         * be of little useful.
         * 
         * Address calculation, such as "addl r11=@ltoff(seedi#),gp" 
         * frequently occurs in loop and it is 
         * !CG_DEP_Can_OP_Move_Across_Call with any call. So we handle
         * this special case at the remainding of this function. 
         */

#ifdef TARG_ST
    // [vcdv] check whether op may alias with a call.
    // Note that here the register have not been allocated therefore
    // this api has a strong impact on performances. We must only
    // allow instructions that will not increment the register pressure.
    if (CGTARG_op_may_alias_with_call(op))
      return alias;
#else
    if (OP_code(op) != TOP_addl) { return TRUE; }
#endif

        /* If one of the source operands is not constant, we 
         * conservatively think <op> and <call> alias. 
         */
    for (INT i = 0; i < OP_opnds(op); i++) {
        TN* opnd = OP_opnd(op,i);
        //opnd should be a loop constant
        if (!TN_is_Loop_Constant (opnd)) return TRUE;
    }

    for (INT i = 0; i < OP_results(op); i++) {

        TN* result = OP_result(op, i);
#ifndef TARG_ST
        // [vcdv] i don't see the point in discarding dedicated registers ?
        if (TN_is_dedicated(result)) { return TRUE; }
#endif

        REGISTER reg = TN_register(result);
        if (reg == REGISTER_UNDEFINED) continue;
        ISA_REGISTER_CLASS rclass = TN_register_class (result);

            /* prune out regs which have implicit meaning.
             */
        if(REGISTER_SET_MemberP(REGISTER_CLASS_function_value(rclass), reg) ||
           REGISTER_SET_MemberP(REGISTER_CLASS_function_argument(rclass), 
                                reg) ||
           REGISTER_SET_MemberP(REGISTER_CLASS_caller_saves(rclass), reg)
#ifdef TARG_ST
           // [vcdv] hw loop are non allocatable and thus do not
           // appear in the REGISTER_SET REGISTER_CLASS_caller_saves
           || ABI_PROPERTY_Is_caller(rclass, REGISTER_machine_id(rclass, reg))
#endif
           ) {
          return TRUE;
        }

        #ifdef HAS_STACKED_REGISTERS
        if (REGISTER_Is_Stacked_Output(rclass, reg)) {
            return TRUE;
        }
        #endif
    }

    return FALSE;
}

BOOL
LOOP_INVAR_CODE_MOTION :: It_is_Fake_Loop_Invar (OP* op) {

    UINT8 omega = 0;
    BOOL  definite=FALSE;

    if (OP_store (op)) { return TRUE; }

    if (Loop_Has_Call ()) {
        for (OP_Vector_Iter iter = _call_ops.begin (); 
             iter != _call_ops.end (); iter++) {

            if (Alias_With_Call (op, *iter)) { return TRUE; }
        }
    }

    if (OP_load (op)) {
        for (OP_Vector_Iter st_iter = _store_ops.begin ();
             st_iter != _store_ops.end (); st_iter ++) {
            
            OP* op_like_st = *st_iter;
            if (!OP_store (op_like_st)) { 
                    /* e.g CGTARG_Is_OP_Intrinsic(). ref OP_like_store() 
                     * for details 
                     */
                return TRUE; 
            }

	    // FdF 20061127: Fix bug #19532. Need to check for
	    // dependencies in the two directions, because
	    // cross-iteration dependencies are ignored (store in one
	    // iteration in dependence with a load in a successive
	    // iteration).
            if (CG_DEP_Mem_Ops_Alias (op, op_like_st, &definite) ||
		CG_DEP_Mem_Ops_Alias (op_like_st, op, &definite)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

   /* ===============================================================
    * Def_Dominate_All_Use 
    * 
    *   return TRUE iff all uses of <linvar>'s left-hand-side might 
    *   be reached only be <linvar>. Put in other word <linvar> 
    *   dominate all use the its left-hand-side variables. 
    *
    * ==============================================================
    */
BOOL
LOOP_INVAR_CODE_MOTION :: Def_Dominate_All_Use (OP* linvar) {
    
    BB* home = OP_bb (linvar);
    
    for (INT i=0 ; i < OP_results(linvar); i++) {

        TN* res = OP_result(linvar, i);
        if (It_Is_Constant_TN (res)) { continue; }

        LI_TN_INFO* info;
        info = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, res);
        Is_True (info, 
            ("LI_TN_INFO for OP:%d of BB:%d does not exist", 
              OP_map_idx(linvar), BB_id (OP_bb(linvar))));
       
        OP_Vector* ops = (OP_Vector*)info->Uses ();

            /* loop over each use of OP_result(linvar, i)
             */
        for (OP_Vector_Iter iter = ops->begin (); 
             iter != ops->end (); iter++) {

            OP* o = *iter;
            BB* t = OP_bb (o);

                /* If def and use locate in diff blocks, we can count on 
                 * BB level dorminance info, otherwise we resort to comparing
                 * the "order" fields of these 2 OPs.
                 */
            if (!BB_SET_MemberP (BB_dom_set(t), home)) {
                return FALSE; 
            } else if (t == home) {
                if (!OP_Precedes (linvar, o)) { return FALSE; }
            }
        }
    }

    return TRUE;
}

    /* ============================================================
     *  
     * Dom_All_Exits 
     *   Determine whether <b> dominate all loop exits. 
     * 
     * ============================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: Dom_All_Loop_Exits (BB* b) {

    BOOL no_exit = TRUE;

    for (BB_Lst_Iter iter = _loop_exits.begin (); 
         iter != _loop_exits.end (); iter++) {

        BB* exit_bb = *iter;
        if (!BB_SET_MemberP (BB_dom_set(exit_bb), b)) { return FALSE; }

        no_exit = FALSE;
    }

        /* If this loop SHOULD had no loop exit, we conservatively 
         * think b does not dominate all exits.  
         */
    return  !no_exit;
}

    /* =============================================================
     *
     * Live_Out_Of_Loop 
     *
     *   Determine whether <op>'s left-hand-side variables are live
     *   out of loop.
     *
     * =============================================================
     */
inline BOOL
LOOP_INVAR_CODE_MOTION :: Live_Out_Of_Loop (OP* op) const {

    for (INT i = OP_results (op) - 1; i >= 0; i--) {
        TN* res = OP_result(op, i);
        
        if (!TN_is_constant(res) && TN_is_global_reg(res) &&
            GTN_SET_MemberP (_liveout_defs, res)) {
            return TRUE;
        }
    }
    return FALSE;
}

    /* ================================================================
     * 
     * Code_Motion_Is_Profitable 
     * 
     *   Determine whether move loop-invar def <op> to loop-header
     *   is profitable or not. Our concern are roughly two folds:
     *      -  register pressure
     *      -  penalty inherient in speculation: some L.I.C.Ms are 
     *         speculation (i.e OP is not always executed in every 
     *         interation)
     *
     * =================================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: Code_Motion_Is_Profitable (OP* op) {
    
    BB* loop_ent = LOOP_DESCR_loophead(_loop);
    BB* home = OP_bb (op);
    
        /* rule 1: rule out those candidate of low reach probability 
         *    from loop entry. 
         */
      /* the larger prob-thresold the small latency
       */ 
#ifdef TARG_ST
    #define PROG_SCALE          (100)
    INT32 LOW_PROB_FOR_ALU_OP = (70);
    INT32 LOW_PROB_FOR_LD_OP  = (40);
    INT32 LOW_PROB_FOR_LONG_LATENCY = (40);

#ifdef Is_True_On
    if (getenv("PROB_ALU"))
      LOW_PROB_FOR_ALU_OP = atoi(getenv("PROB_ALU"));

    if (getenv("PROB_LD"))
      LOW_PROB_FOR_LD_OP = atoi(getenv("PROB_LD"));

    if (getenv("PROB_LONG"))
      LOW_PROB_FOR_LONG_LATENCY = atoi(getenv("PROB_LONG"));
#endif
#else
    #define PROG_SCALE          (100)
    #define LOW_PROB_FOR_ALU_OP (70)
    #define LOW_PROB_FOR_LD_OP  (40)
    #define LOW_PROB_FOR_LONG_LATENCY (40)
#endif

    float freq1 = BB_freq (loop_ent), freq2 = BB_freq (home);
    freq1 = (freq1 < 1.0f) ? 1.0f : freq1;
    INT32 div = (INT32)(freq2 * PROG_SCALE/ freq1);

    if (OP_Is_Long_Latency (OP_code (op))) {
        if (div < LOW_PROB_FOR_LONG_LATENCY) { 
	    sprintf(diagnostic, "long latency operation, probability too low: %d/100 < %d/100",
		    div, LOW_PROB_FOR_LONG_LATENCY);
	    goto not_profitable;
	}
    } else if (OP_load (op)) {
        if (div < LOW_PROB_FOR_LD_OP) { 
	    sprintf(diagnostic, "load operation, probability too low: %d/100 < %d/100",
		    div, LOW_PROB_FOR_LD_OP);
	    goto not_profitable;
	}
    } else {
        if (div < LOW_PROB_FOR_ALU_OP) { 
	    sprintf(diagnostic, "alu operation, probability too low: %d/100 < %d/100",
		    div, LOW_PROB_FOR_ALU_OP);
	    goto not_profitable;
	}
    }

        /* rule 2: prevent those code motion that deteriorate reg pressure.
         */
    if (Live_Out_Of_Loop (op)) {
        /* it does not change the reg-pressure */  
	sprintf(diagnostic, "profitable because live out of loop");
        goto profitable;
    }

    if (_op_info.It_is_Tiny_Loop () ||
	_op_info.It_is_Small_Loop ())  { 
	return TRUE; 
    }
    if (_op_info.It_is_Mediate_Sized_Loop () && !_op_info.Hot_Loop ()) {
	sprintf(diagnostic, "medium loop is not hot: op num %d, freq %.2f",
		_op_info.OP_Num(), BB_freq(LOOP_DESCR_loophead(_op_info.LoopDescr())));
	goto not_profitable;
    }
    if (_op_info.It_is_Large_Loop () && !_op_info.Very_Hot_Loop ()) {
	sprintf(diagnostic, "large loop is not very hot: op num %d, freq %.2f",
		_op_info.OP_Num(), BB_freq(LOOP_DESCR_loophead(_op_info.LoopDescr())));
	goto not_profitable;
    }
    if (_op_info.It_is_Huge_Loop ())  { 
	sprintf(diagnostic, "huge loop: op num %d",
		_op_info.OP_Num());
	goto not_profitable;
    }


        /* for mediate sized loop 
         */
    /*
    INT32 max_use_level = 0;
    for (INT i = OP_results (op) - 1; i >= 0; i--) {
        TN* res = OP_result (op, i);
        
        LI_TN_INFO* ti = (LI_TN_INFO*)hTN_MAP_Get (_tn_info, res);
        OP_Vector* ops = (OP_Vector*)ti->Uses ();

        for (OP_Vector_Iter iter = ops->begin (); iter != ops->end (); iter++) {
            OP* use = *iter; 
            LI_OP_INFO* oi = _op_info.Get (use);  
            if (max_use_level > oi->Level ()) {
                max_use_level = oi->Level (); 
            }
        }
    } 

    LI_OP_INFO* oi = _op_info.Get (op);
    INT32 potion = (oi->Level() + 1 - max_use_level) * 100 / 
                        (_op_info.Max_Level () + 1);
    return potion >= 20;
    */
    sprintf(diagnostic, "profitable for all rules");
 profitable:
    if (LICM_Trace_Profitable) {
	fprintf(TFile, "<licm> operation hoisted: %s\n<licm>  ", diagnostic);
	Print_OP_No_SrcLine(op);
    }
    return TRUE;

 not_profitable:
    if (LICM_Trace_Non_Profitable) {
	fprintf(TFile, "<licm> operation not hoisted: %s\n<licm>  ", diagnostic);
	Print_OP_No_SrcLine(op);
    }
    return FALSE;
}

    /* ================================================================
     *
     * Ignore_Loop_With_Few_Interation 
     * 
     *    Detemine whether the current loop has few interations. If so, 
     *    performing loop invariant code motion will degrade performance.
     *
     *    This function should be called at very early stage to rule 
     *    out some non-interesting loop.
     *
     * ==================================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: Ignore_Loop_With_Few_Interation (void) {

    BB* loop_ent = LOOP_DESCR_loophead(_loop);
    float f = 0.0f;
    BOOL ignore_loop;

    BBLIST* pred;
    FOR_ALL_BB_PREDS (loop_ent,pred) {
        BB* b = BBLIST_item(pred);
        f += BB_freq (b);
    }

    f = (f < 1.0f) ? 1.0f : f;
    ignore_loop = (BB_freq (loop_ent) - f) < 10^5;
    if (ignore_loop && LICM_Trace_Non_Profitable) {
	fprintf(TFile, "<licm> loop ignored BB:%d :to few interations\n", BB_id(loop_ent));
    }
    return ignore_loop;
}

    /* =================================================================
     *
     * Can_be_Moved_to_Prologue 
     * 
     *   Determine whether loop invariant definition,<loopinvar>, can be 
     *   moved the end of prologue. 
     *
     *   NOTE: this routine assume all the left-hand-side variables of 
     *         <op> are loop invariants. 
     *
     * =================================================================
     */
BOOL
LOOP_INVAR_CODE_MOTION :: Illegal_Or_Nonprofitable (OP* loopinvar) {

    BB* home = OP_bb (loopinvar);

    if (LICM_Trace_Candidates) {
	fprintf(TFile, "<licm> loop invariant candidate: BB:%d, OP:%d\n<licm>  ", BB_id(home), OP_map_idx(loopinvar));
	Print_OP_No_SrcLine(loopinvar);
    }

    if (!Def_Dominate_All_Use (loopinvar)) {
        return TRUE;
    }

        /* "<spec> == TURE" indicate the code motion is speculation rather 
         *  than useful code motion.
         */
    BOOL spec = !Dom_All_Loop_Exits (home);

    if (spec) {
            /* It will raise exception and hence erase your disk and 
             * hence spread epidemic such as severe acute respiratory
             * syndrome...!!! */
#ifdef TARG_ST
        if (!OP_Can_Be_Speculative (loopinvar)) return TRUE;
#else
        if (!CGTARG_Can_Be_Speculative (loopinvar)) return TRUE;
#endif
            /* To make life easier, we do not perform code motion for 
             * speculated loads(ld.s, ld.a and ld.sa).
             */
#ifdef TARG_ST
	if (OP_Is_Speculative (loopinvar)) return TRUE;
#else
	if (CGTARG_Is_OP_Speculative (loopinvar)) return TRUE; 
#endif
            /* It will kill variables that are live extend the loop. 
             */
        if (Live_Out_Of_Loop (loopinvar)) { 
            return TRUE; 
        }
    }

    if (!Code_Motion_Is_Profitable (loopinvar)) { 
        return TRUE; 
    }

        /* Rule out the *FAKE* loop invariant definition, ref details 
         * of the comment right before the the definition of  
         * LOOP_INVAR_CODE_MOTION::Identify_Loop_Invariants ().
         */
    if (It_is_Fake_Loop_Invar (loopinvar)) {
        return TRUE;
    }

#ifdef TARG_ST
    if (! CGTARG_Code_Motion_To_BB_Is_Legal( loopinvar,  LOOP_DESCR_loophead(_loop)) ) {
      return TRUE;
    }
#endif

    return FALSE; 
}

#ifdef TARG_ST
    /* ===========================================================
     * ===========================================================
     * 
     * class LI_MEMORY_INFO
     *   
     *    Memory groups information for loop invariant addresses
     *
     * ===========================================================
     * ===========================================================
     */
class LI_MEMORY_INFO {
private:

    TN *_baseTN;          /* The base and offset of this invariant loop address. */
    TN *_offsetTN;
    BOOL _hoisted;	  /* Whether operations in this memory group */
    BOOL _sunk;		  /* have already been hoisted or sunk. */
    OP_Vector _mem_ops;   /* List of memory operations that access
                             this loop invariant memory location. */
    TN *_scalarTN;        /* TN used for the scalarization of the
                             memory accesses. */
    TN *_addrTN;	  /* TN to be used at a store location to set
                             the speculated store address. */
    LI_MEMORY_INFO *_next; /* Linked list of LI_MEMORY_INFO. Head is
			     pointed to by
			     LOOP_INVAR_CODE_MOTION::_memory_groups. */

public:
    LI_MEMORY_INFO (MEM_POOL* mp, TN *baseTN, TN *offsetTN);
    ~LI_MEMORY_INFO (void) {}

    void Add_Memory_OP(OP *memop) { _mem_ops.push_back (memop); }
    OP_Vector Get_Memory_OPS(void) { return _mem_ops; }
    BOOL Same_Address (TN *baseTN, TN *offsetTN) { return (baseTN == _baseTN && offsetTN == _offsetTN); }
    TN *base(void) { return _baseTN; }
    TN *offset(void) { return _offsetTN; }
    BOOL Hoisted(void) { return _hoisted; }
    void Set_Hoisted(void) { _hoisted = TRUE; }
    BOOL Sunk(void) { return _sunk; }
    void Set_Sunk(void) { _sunk = TRUE; }
    TN *Get_Scalar(void) { return _scalarTN; }
    void Set_Scalar(TN *v) { _scalarTN = v; }
    TN *Get_Addr(void) { return _addrTN; }
    void Set_Addr(TN *tn) { _addrTN = tn; }
    LI_MEMORY_INFO *Next(void) { return _next; }
    void Link(LI_MEMORY_INFO *next) { _next = next; }
};

LI_MEMORY_INFO :: LI_MEMORY_INFO (MEM_POOL* mp, TN *baseTN, TN *offsetTN) :
    _mem_ops(mp), _scalarTN(NULL), _addrTN(NULL), _hoisted(FALSE), _sunk(FALSE) {
    _baseTN = baseTN;
    _offsetTN = offsetTN;
}

void
LOOP_INVAR_CODE_MOTION :: Set_Memory_Group ( OP * memop ) {

    /* If there is already a MEMORY for this address, use it and add
       memop to the list of operations for this MEMORY. Otherwise, create
       a new MEMORY. */
    LI_MEMORY_INFO *memory;

    memory = Lookup_Memory(memop);
    memory->Add_Memory_OP(memop);
    LI_OP_INFO *meminfo = Get_OP_Info(memop);
    meminfo->Set_Memory_Group(memory);
}

LI_MEMORY_INFO *
LOOP_INVAR_CODE_MOTION :: Get_Memory_Group ( OP * memop ) {
    LI_OP_INFO *meminfo = Get_OP_Info(memop);
    return (meminfo != NULL) ? meminfo->Get_Memory_Group() : NULL;
}

LI_MEMORY_INFO *
LOOP_INVAR_CODE_MOTION :: Lookup_Memory ( OP * memop ) {
    TN *baseTN, *offsetTN;

    baseTN = OP_opnd(memop, OP_find_opnd_use(memop, OU_base));
    offsetTN = OP_opnd(memop, OP_find_opnd_use(memop, OU_offset));

    for (LI_MEMORY_INFO *memory = _memory_groups; memory; memory = memory->Next()) {
	if (memory->Same_Address(baseTN, offsetTN))
	    return memory;
    }

    /* Create a new MEMORY_INFO */
    LI_MEMORY_INFO *memory = CXX_NEW(LI_MEMORY_INFO(_mp, baseTN, offsetTN), _mp);
    memory->Link(_memory_groups);
    _memory_groups = memory;

    return memory;
}

/* Returns TRUE if this BB, or all predecessors of this BB, contains
   at least one load or store operation. Returns FALSE otherwise. */

static BOOL
checkMemop(BB *bb, OP_Vector mem_ops, BB *loophead, BB_SET *visited) {

    if (BB_SET_MemberP(visited, bb))
	return FALSE;

    for (OP_Vector_Iter st_iter = mem_ops.begin();
	 st_iter != mem_ops.end (); st_iter ++) {
	OP *memop = *st_iter;
	/* Dismissible loads must be ignored. */
	if (((OP_load(memop) && !OP_Is_Speculative(memop)) ||
	     OP_store(memop)) &&
	    (OP_bb(memop) == bb))
	    return TRUE;
    }

    if (bb == loophead)
	return FALSE;

    BB_SET *bbset = BB_SET_Union1D(visited, bb, NULL);
    Is_True(bbset == visited, ("Union1D must not create a new BB_SET"));

    BBLIST *P;
    FOR_ALL_BB_PREDS (bb, P) {
	BB *pred = BBLIST_item(P);
	if (!checkMemop(pred, mem_ops, loophead, visited)) {
	    BB_SET_Difference1D(visited, bb);
	    return FALSE;
	}
    }
    
    BB_SET_Difference1D(visited, bb);
    return TRUE;
}

/* Check that every path from a loop exit to the head of the loop
   contains a load or store operation. */
BOOL
LOOP_INVAR_CODE_MOTION :: Dom_All_Loop_Exits (LI_MEMORY_INFO *memory) {

    Is_True(memory, ("memory must no be NULL"));
    BB_SET *visited = BB_SET_Create_Empty(PU_BB_Count+2, _mp);

    for (BB_Lst_Iter iter = _loop_exits.begin (); 
         iter != _loop_exits.end (); iter++) {

        BB* exit_bb = *iter;
	Is_True(BB_SET_EmptyP(visited), ("visited must be empty"));
	if (!checkMemop(exit_bb, memory->Get_Memory_OPS(), LOOP_DESCR_loophead(_loop), visited))
	    return FALSE;
    }

    return TRUE;
}

/* Check if the address for a memory operation is loop
   invariant. Volatile operations must not be considered as
   invariant. */
BOOL
LOOP_INVAR_CODE_MOTION :: Invariant_Address (OP* op) {

    int offset_opnd, base_opnd;
    TN *offset_tn, *base_tn;
    BOOL invariant;

    Is_True(OP_load(op) || OP_store(op), ("Must be load or store"));

    if (OP_volatile(op) || OP_has_implicit_interactions(op))
      return FALSE;
    
    if (!OP_plain_load(op) && !OP_plain_store(op))
      return FALSE;

    offset_opnd = OP_find_opnd_use(op, OU_offset);
    base_opnd = OP_find_opnd_use(op, OU_base);
    Is_True(offset_opnd >= 0 && base_opnd >= 0, ("Missing base or offset on memory operation"));
    offset_tn = OP_opnd(op, offset_opnd);
    base_tn = OP_opnd(op, base_opnd);

    invariant = ((It_Is_Constant_TN (offset_tn) ||
		  All_Reaching_Def_are_Outside_Of_Loop (op, offset_opnd) ||
		  (It_Is_Loop_Invar_TN (offset_tn) &&
		  Unique_Reaching_Def_Inside_Loop (op, offset_opnd))) &&
		 (It_Is_Constant_TN (base_tn) ||
		  All_Reaching_Def_are_Outside_Of_Loop (op, base_opnd) ||
		  (It_Is_Loop_Invar_TN (base_tn) &&
		  Unique_Reaching_Def_Inside_Loop (op, base_opnd))));

    return invariant;
}

/* Check if op is aliased with operations that are not explicitely at
   the same loop invariant address. */

BOOL
LOOP_INVAR_CODE_MOTION :: Ambiguous_Alias (OP *op) {

    BOOL  definite=FALSE;

    if (Loop_Has_Call ()) {
        for (OP_Vector_Iter iter = _call_ops.begin (); 
             iter != _call_ops.end (); iter++) {

            if (Alias_With_Call (op, *iter)) { return TRUE; }
        }
    }

    if (OP_store (op)) {
        for (OP_Vector_Iter ld_iter = _ld_ops.begin ();
             ld_iter != _ld_ops.end (); ld_iter ++) {
            
            OP* op_load = *ld_iter;

	    /* Operation was already moved out of loop or removed. */
#ifdef TARG_ST
	    if (is_in_prolog_p (OP_bb(op_load)) || OP_bb(op_load) == NULL)
#else
	    if (OP_bb(op_load) == _prolog || OP_bb(op_load) == NULL)
#endif
	      continue;

	    /* Ignore aliasing with load operations in the same MEMORY
               group, since load operations will be hoisted. */

	    if ((Get_Memory_Group(op) == NULL) ||
		(Get_Memory_Group(op) != Get_Memory_Group(op_load)))
	      // FdF 20061127: Fix bug #19532. Need to check for
	      // dependencies in the two directions, because
	      // cross-iteration dependencies are ignored (store in
	      // one iteration in dependence with a load in a
	      // successive iteration).
		if (CG_DEP_Mem_Ops_Alias (op, op_load, &definite) ||
		    CG_DEP_Mem_Ops_Alias (op_load, op, &definite)) {
		    return TRUE;
            }
        }
    }

    if (OP_load (op) || OP_store (op)) {
        for (OP_Vector_Iter st_iter = _store_ops.begin ();
             st_iter != _store_ops.end (); st_iter ++) {

            OP* op_like_st = *st_iter;
            if (!OP_store (op_like_st)) { 
                    /* e.g CGTARG_Is_OP_Intrinsic(). ref OP_like_store() 
                     * for details 
                     */
                return TRUE; 
            }

	    /* Operation was already moved out of loop or removed. */
	    if (OP_bb(op_like_st) == _epilog || OP_bb(op_like_st) == NULL)
	      continue;

	    /* Ignore aliasing with store operations in the same MEMORY
               group, since store operations will be sunk. */

	    if ((Get_Memory_Group(op) == NULL) ||
		(Get_Memory_Group(op) != Get_Memory_Group(op_like_st)))
	      // FdF 20061127: Fix bug #19532. Need to check for
	      // dependencies in the two directions, because
	      // cross-iteration dependencies are ignored (store in
	      // one iteration in dependence with a load in a
	      // successive iteration).
		if (CG_DEP_Mem_Ops_Alias (op, op_like_st, &definite) ||
		    CG_DEP_Mem_Ops_Alias (op_like_st, op, &definite)) {
		    return TRUE;
            }
        }
    }

    return FALSE;
}

/* For memory operation based on loop invariant addresses, associate a
   LI_MEMORY_INFO, so as to group together memory operations that
   unambiguously access to the same memory location. */

void
LOOP_INVAR_CODE_MOTION :: Identify_Memory_Groups (void) {

    for (OP_Vector_Iter st_iter = _ld_ops.begin ();
	 st_iter != _ld_ops.end (); st_iter ++) {
	OP* op_load = *st_iter;
	/* op_load may have already been hoisted. */
#ifdef TARG_ST
	if (is_in_prolog_p (OP_bb(op_load))) continue;
#else
	if (OP_bb(op_load) == _prolog) continue;
#endif
	if (Invariant_Address (op_load)) {
	    Set_Memory_Group (op_load);
	}
    }

    for (OP_Vector_Iter st_iter = _store_ops.begin ();
	 st_iter != _store_ops.end (); st_iter ++) {
	OP* op_like_st = *st_iter;
	if (!OP_store (op_like_st))
	    continue;
	/* op_store have not yet been sunk. */
	if (Invariant_Address (op_like_st)) {
	    Set_Memory_Group (op_like_st);
	}
    }
}

BOOL
LOOP_INVAR_CODE_MOTION :: Scalarization_Is_Profitable (LI_MEMORY_INFO *memory, BOOL speculate) {
    int countLoad, countStore;
    OP_Vector mem_ops;

    /* In case of speculation, check if dismissible loads are
       allowed. */
    if (speculate && !(Enable_Dismissible_Load && PROC_has_dismissible_load()))
	return FALSE;

    /* Count the number of load and store operations. */

    countLoad = 0;
    countStore = 0;
    mem_ops = memory->Get_Memory_OPS();
    for (OP_Vector_Iter st_iter = mem_ops.begin();
	 st_iter != mem_ops.end (); st_iter ++) {
	OP *mem_op = *st_iter;

	if (OP_load(mem_op)) countLoad ++;

	else if (OP_store(mem_op)) countStore ++;
    }

    /* If there are only store operations and an epilog could not be
       created, do not perform scalarization since it will only add a
       new variable in the loop. */
    if (countLoad == 0 && _epilog == NULL)
	return FALSE;

    return TRUE;
}

#endif

    /* =============================================================
     *
     *  Perform_Loop_Invar_Code_Motion 
     * 
     *    This routine topogically visit each loop-invar definition.
     *    When progress, it will move some loop-invar def to prologue
     *    if it think this moving is both benefial and legal.
     *
     *    the return value is the # of code motion that is actully 
     *    performed. 
     * 
     * ==============================================================
     */
INT32 
LOOP_INVAR_CODE_MOTION :: Perform_Code_Motion (void) {

    Identify_Loop_Invariants ();

        /* Looping over loop-invarants in topological order, NOTE:
         * it should be in topological order.
         */
    INT32 code_motion_num = 0;
    for (OP_Vector_Iter iter = _loop_invar_defs.begin ();
         iter != _loop_invar_defs.end (); iter++) {

        OP* linvar = *iter;
        LI_OP_INFO* opinfo = Get_OP_Info (linvar);

        BOOL perform = TRUE;

        if (!opinfo->Def_Loop_Invar ()) {
                /* This test is _*NOT*_ NOP, since some source operands 
                 * of <linvar> is defined by 
                 *   - some "fake" loop-invar identified by 
                 *     Identify_Loop_Invariants (), or 
                 *   - genuine loop-invar defs which is illegal or unprofitabl     
                 *     to be moved to the prologue. 
                 */ 
            perform = FALSE;
        }

#ifdef TARG_ST
	if (perform && (! _prolog.back () || BB_asm (_prolog.back ()))) {
	  perform = FALSE;
	}
#endif
        if (perform && Illegal_Or_Nonprofitable (linvar)) {
            perform = FALSE;
        }

        if (!perform) {
                /* mark those OPs that immediately flow dependents upon 
                 * this OPs as non-loop-invariant definition.
                 */
            Mark_Dep_OPs_As_Non_Loop_Invar (linvar);
        } else {
            Perform_Code_Motion (linvar);
            code_motion_num ++;
        }
    }

    /* At this point, invariant loop hoisting has been performed on
       memory operations not in alias with other operations in the
       loop. We will now consider memory operations that refer to the
       same loop invariant memory location and see if we can scalarize
       them by hoisting the loads and sinking the stores. */

#ifdef TARG_ST
    if (IPFEC_Enable_LICM < 2
	|| BB_asm (_prolog.back ()))
      return code_motion_num;

#ifdef TARG_ST200
    /* Add tags on explicit memory references, so as to group together
       memory operations to a same location. */
    Identify_Memory_Groups ();

    /* Check and perform hoisting and sinking on memory groups. */
    for (LI_MEMORY_INFO *memory = _memory_groups; memory; memory = memory->Next()) {
	code_motion_num += Perform_Scalarization (memory);
    }
#elif defined(TARG_STxP70)
    /* Not activated for STxP70. */
#endif
#endif

    return code_motion_num;
}

#ifdef TARG_ST
/* This function will generate a copy or sign extension operation
   between the original TN in the memory operation and the new scalar
   variable. */

static void
Scalarize_OP(OP *memop, TN *scalar) {
    OPS New_OPs = OPS_EMPTY;

    if (OP_load(memop)) {
	/* generate a sign extension operation if needed. */
	INT size = OP_Mem_Ref_Bytes(memop);
	if (TN_size(scalar) > size) {
	    TN *to_length_tn = Gen_Literal_TN (TN_size(scalar) * 8, 4);
	    TYPE_ID from_mtype;
	    BOOL is_signed = ! TOP_is_unsign(OP_code(memop));
	    if (size == 1) {
	      from_mtype = is_signed ? MTYPE_I1 : MTYPE_U1;
	    } else if (size == 2) {
	      from_mtype = is_signed ? MTYPE_I2 : MTYPE_U2;
	    }
	    Expand_Convert_Length(OP_result(memop, 0), scalar,
				  to_length_tn,
				  from_mtype,
				  is_signed, &New_OPs);
	}
	else
	    Exp_COPY(OP_result(memop, 0), scalar, &New_OPs);
    }

    else if (OP_store(memop))
	Exp_COPY(scalar, OP_opnd(memop, OP_find_opnd_use(memop, OU_storeval)), &New_OPs);

    else Is_True(0, ("Only load and store can be scalarized"));

 #ifdef TARG_ST
    // (cbr) Support for guards on false
    OPS_Copy_Predicate (&New_OPs, memop);
#endif

   BB_Insert_Ops(OP_bb(memop), memop, &New_OPs, TRUE);	
}

/* Load and store operations in a same MEMORY group are in alias. To
   check if these operations can be hoisted or sunk, we ignore aliases
   between operations in a same MEMORY group.

   We first check if all paths from loophead to any loop exit go
   through a non speculative load or store memory operation. If this
   is not the case, this means that hoisting or sinking will be
   speculative.

   Then we compute the cost of replacing memory accesses by a scalar
   variable. Scalarization introduces a cost of one new live-range for
   the whole loop. This means that there is no reason to move only
   parts of the load or store operations in a memory group, all
   operations of a MEMORY group will be moved at the cost of one extra
   live-range. This cost can be avoided if the def of one load, or the
   use of one store, can be used as the scalar variable.

   Hoisting load operations may have significant benefits due to the
   long latency of these operations. Sinking store operations will
   introduce less benefits. The benefit will only be greater to the
   cost of one extra live range in loops where the register pressure
   is small.

   Speculation do not introduce extra cost for load operations. For
   store operations however, speculation adds another live-range for a
   TN to keep the adress at which a speculative store must be
   peformed. This address is initialized to a black-hole location in
   the prolog, and then changed to the actual store address if a store
   would have been performed in the loop.

*/

INT32
LOOP_INVAR_CODE_MOTION :: Perform_Scalarization (LI_MEMORY_INFO *memory) {
    INT32 code_motion_num = 0;
    BOOL perform = TRUE;
    BOOL speculate = FALSE;
    

    /* Check if other operations in the loop are in alias with
       operations in this MEMORY group. Aliasing must be checked
       against call and store operations, and also against load
       operations if the group contains store operations. */

    /* memoryOP will be a store operation if there is one, a load
       operation otherwise. */
    OP *memoryOP = NULL;
    OP_Vector mem_ops = memory->Get_Memory_OPS();
    //    memoryOP = mem_ops.back();
    for (OP_Vector_Iter st_iter = mem_ops.begin();
	 st_iter != mem_ops.end (); st_iter ++) {
	OP *mem_op = *st_iter;

	memoryOP = mem_op;
	if (OP_store(mem_op))
	    break;
    }
    Is_True(memoryOP != NULL, ("At least one load or store operation in a MEMORY group."));

    /* Check that all memory ops access the memory location with the
       same size. */
    
    for (OP_Vector_Iter st_iter = mem_ops.begin();
	 st_iter != mem_ops.end (); st_iter ++) {
	OP *mem_op = *st_iter;

	if (OP_Mem_Ref_Bytes(mem_op) != OP_Mem_Ref_Bytes(memoryOP)) {
	    perform = FALSE;
	    break;
	}
    }

    /* Check there is no alias with other operations in the loop. */
    perform = perform && !Ambiguous_Alias(memoryOP);

    /* Check if scalarization will require speculation. */
    speculate = perform && !Dom_All_Loop_Exits (memory);

    /* Check if load hoisting will be beneficial. If there is no load,
       check if store sinking will be beneficial. */

    if (perform && !Scalarization_Is_Profitable (memory, speculate))
        perform = FALSE;

    if (!perform)
	return code_motion_num;

    if (memory->Get_Scalar() == NULL) {
	TN *scalar;
	if (OP_load(memoryOP))
	    scalar = Dup_TN(OP_result(memoryOP, 0));
	else
	    scalar = Dup_TN_Even_If_Dedicated(OP_opnd(memoryOP, OP_find_opnd_use(memoryOP, OU_storeval)));
	// FdF 20071113: The new TN should not inherit the
	// rematerializable from memoryOP.
	if (TN_is_rematerializable(scalar)) {
	  Reset_TN_is_rematerializable(scalar);
	  Set_TN_remat(scalar, NULL);
	}
	memory->Set_Scalar(scalar);
    }

    for (OP_Vector_Iter st_iter = mem_ops.begin();
	 st_iter != mem_ops.end (); st_iter ++) {
	OP *mem_op = *st_iter;

	if (OP_load(mem_op)) {

	    /* Insert loaddef = scalar */
	    Scalarize_OP(mem_op, memory->Get_Scalar());

	    /* Change the load into a speculative load. */
	    if (speculate && !memory->Hoisted()) {
		if (!OP_Is_Speculative(mem_op)) {
		    /* Create a dismissible load */
		    TOP ld_top = CGTARG_Speculative_Load (mem_op);
		    DevAssert(ld_top != TOP_UNDEFINED, ("couldnt find a speculative load"));

		    OP_Change_Opcode(mem_op, ld_top); 
		    Set_OP_speculative(mem_op);  
		}
	    }

	    if (!memory->Hoisted()) {
		Set_OP_result(mem_op, 0, memory->Get_Scalar());
		Perform_Code_Motion(mem_op);
		memory->Set_Hoisted();
	    }
	    else BB_Remove_Op(OP_bb(mem_op), mem_op);
	    code_motion_num ++;
	}
	else {

	    /* Insert scalar = storeval */
	    Scalarize_OP(mem_op, memory->Get_Scalar());

	    /* If _epilog is not defined, store operations cannot be
               moved out of the loop. */
	    if (_epilog == NULL)
	      continue;

	    if (speculate && !memory->Sunk()) {
		OPS New_OPs = OPS_EMPTY;

		/* Generate a black-hole location and set
                   memory->Addr() to it in the prolog. */

#ifdef TARG_ST
		TN *addrTN = Gen_Register_TN (Pointer_Register_Class,
                                              Pointer_Size);
#else
		TN *addrTN = NULL;
		FmtAssert(FALSE,("do not know how to generate address TN"));
#endif
		memory->Set_Addr(addrTN);
		Expand_BlackHole(mem_op, addrTN, &New_OPs);
#ifdef TARG_ST
		BB_Append_Ops(_prolog.back (), &New_OPs);
#else
		BB_Append_Ops(_prolog, &New_OPs);
#endif

		/* Generate a store operation in the epilog. */
		OPS_Remove_All(&New_OPs);
		Set_OP_opnd(mem_op, OP_find_opnd_use(mem_op, OU_storeval), memory->Get_Scalar());
		Expand_CondStoreOP(mem_op, addrTN, &New_OPs);
		BB_Append_Ops(_epilog, &New_OPs);
		memory->Set_Sunk();
	    }

	    /* Insert addr = storeaddr in case of speculation. */
	    if (speculate) {
		OPS New_OPs = OPS_EMPTY;
		Expand_CondStoreAddr(mem_op, memory->Get_Addr(), &New_OPs);
		BB_Insert_Ops_After(OP_bb(mem_op), mem_op, &New_OPs);
	    }

	    if (!memory->Sunk()) {
		Set_OP_opnd(mem_op, OP_find_opnd_use(mem_op, OU_storeval), memory->Get_Scalar());
		Perform_Code_Motion(mem_op);
		memory->Set_Sunk();
	    }
	    else BB_Remove_Op(OP_bb(mem_op), mem_op);
	    code_motion_num ++;
	}

    }

    /* Initialize the scalar in the prolog, if not yet, to avoid any
       uninitialized use. */		
    if (speculate && !memory->Hoisted()) {
        OPS New_OPs = OPS_EMPTY;
	Expand_Immediate (memory->Get_Scalar(), Gen_Literal_TN(0, 4), FALSE, &New_OPs);
        BB_Append_Ops(_prolog.back(), &New_OPs);
    }

    return code_motion_num;
}

static BB* Create_Loop_Prologue (LOOP_DESCR* l); // forward
static void Maintain_Dominator_Info (LOOP_DESCR* l, BB* prolog, BB* epilog,
				     MEM_POOL* mp); // forward

#endif

    /* ==============================================================
     * 
     * Perform_Code_Motion 
     *
     *   move <linvar> to the end of prologue.
     * 
     * =============================================================
     */
void
LOOP_INVAR_CODE_MOTION :: Perform_Code_Motion (OP* linvar) {

    if (LICM_Trace_Candidates) {
      fprintf (TFile, "<licm> candidate hoisted: BB:%d, OP:%d\n",
	       BB_id(OP_bb(linvar)), OP_map_idx(linvar));
    }

#ifdef TARG_ST
  BB *tgt = NULL;
  BB *src = OP_bb(linvar);
  if (OP_store(linvar)) {
    tgt = _epilog;
    BB_Move_Op_To_Start (tgt, OP_bb(linvar), linvar);
  }
  else {
    tgt = _prolog.back();
    BB_Move_Op_To_End (tgt, OP_bb(linvar), linvar);
  }
  // Fix for bug #20894: Assertion failure "TOP_asm not in BB_asm" in cgemit
  if(OP_code(linvar) == TOP_asm) {
    Set_BB_asm(tgt);
    // A basic block can have at most one asm instruction, so we have just to
    // transfer the information
    BB_Transfer_Asminfo(src, tgt);
    // Now create a new prolog BB.
    BB *new_prolog = Create_Loop_Prologue (_loop);
    if (new_prolog) {
      Reset_BB_gra_spill (new_prolog);
      _prolog.push_back (new_prolog);
      Maintain_Dominator_Info (_loop, new_prolog, NULL, _mp);
    }
  }
#else
    BB_Move_Op_To_End (_prolog, OP_bb(linvar), linvar);
#endif


    /* we need to change the result TNs to be global version since 
     * later on we will recaculate liveness from scratch.
     */
}
        
    /* =============================================================
     *
     * Count_Loop_Interation 
     * 
     *  return the number of itertions loop will go. if loop has 
     *  constant trip-count, use that value, otherwise, derive 
     *  from frequency from backedge.
     *
     * ============================================================
     */
static float
Count_Loop_Interation (LOOP_DESCR* l) {

    LOOPINFO* li LOOP_DESCR_loopinfo(l);

    TN* trip_count = NULL; 
    if (li) {
#ifdef TARG_ST
        trip_count = LOOPINFO_exact_trip_count_tn(li); 
#else
        trip_count = LOOPINFO_trip_count_tn(li); 
#endif
    }
    
    if (trip_count && TN_is_constant(trip_count) && 
        TN_has_value(trip_count)) {
        return (float)TN_value(trip_count);
    }

    BB* head = LOOP_DESCR_loophead(l); 
    float f = 0.0f;
#ifdef TARG_ST
    f = BB_freq(head);
#else
    BBLIST* p;
    BB_SET* body = LOOP_DESCR_bbset(l);
    FOR_ALL_BB_PREDS (head, p) {
        BB* succ = BBLIST_item(p);
        if (!BB_SET_MemberP (body, succ)) {
            continue;
        }

        BBLIST* bl = BB_Find_Succ (succ,head);
        f += BBLIST_freq(bl);
    }
#endif
    return f;
}


    /* ==============================================================
     * 
     * Skip_Loop_Invar_Code_Motion 
     *
     *   This function detect those loops that are not approciate for 
     *   loop invariant code motion. It is pre-cursor to actual Loop 
     *   Invariant Code Motion. 
     * 
     * ===============================================================
     */
static BOOL
Skip_Loop_Invar_Code_Motion (LOOP_DESCR* loop) {
    
    INT bb_num = 0 , op_num = 0;
    BB* bb;
    BB_SET* body = LOOP_DESCR_bbset(loop);

    INT32 MAX_BB = LOOP_MAX_BB, MAX_OP = LARGE_LOOP_SIZE_MAX;  
    
        /* relax the constraints for very hot loop 
         */
    bb = LOOP_DESCR_loophead(loop);
    if (BB_freq(bb) > HOT_LOOP_FREQ_MIN) {
        MAX_BB = HOT_LOOP_MAX_BB; MAX_OP = 2*LARGE_LOOP_SIZE_MAX;
    }

    FOR_ALL_BB_SET_members (body, bb) {

        if (BB_rotating_kernel (bb)) {
               /* rule 1: It is hard for us to determine whether variable
                *   is defined or used within rotating-kernel-ed block. so
                *   we give up.
                */
            sprintf(diagnostic, "rotaing kernel loop");
	    goto skip_loop;
        }

            /* rule 2: Do not perform LICM for very large loop 
             */
        if (BB_length(bb)) { ++ bb_num; }

        if (bb_num > MAX_BB) { 
	    sprintf(diagnostic, "max number of BB reached %d", MAX_BB);
	    goto skip_loop;
	}
        op_num += BB_length(bb);
        if (op_num > MAX_OP) { 
	    sprintf(diagnostic, "max number of OP reached %d", MAX_BB);
	    goto skip_loop;
	}
    }

        /* rule 3: ignore unimportant loops.
         */
    {
      LOOPINFO *info = LOOP_DESCR_loopinfo(loop);
      if (info && WN_Loop_Unimportant_Misc(LOOPINFO_wn(info))) {
	  sprintf(diagnostic, "unimportant loop in loopinfo");
	  goto skip_loop;
      }
    }
#ifdef TARG_ST
    return FALSE;
#else
    BB *head = LOOP_DESCR_loophead(loop);
    float f1 = Count_Loop_Interation (loop);
    float f2 = BB_freq(head);

    return !(f1 > 1000 || f2 > 10000);
#endif
 skip_loop:
    if (LICM_Trace_Non_Profitable) {
	fprintf(TFile, "<licm> loop skiped BB:%d : %s\n", BB_id(bb), diagnostic);
    }
    return TRUE;
}


    /* =================================================================
     *
     * Maintain_Dominator_Info
     * 
     *   Maintain the dominator-info in incremental manner  
     *   when prologue/epilogue is inserted
     *
     * =================================================================
     */
static void
#ifdef TARG_ST
Maintain_Dominator_Info (LOOP_DESCR* l, BB* prolog, BB* epilog, MEM_POOL* mp) {
#else
Maintain_Dominator_Info (LOOP_DESCR* l, BB* prolog, MEM_POOL* mp) {
#endif
        /* set initial value of BB_pdom_set(prolog)
         */
    BB* head = LOOP_DESCR_loophead(l);

    if (prolog) {
        BS* dom_pro = BS_Copy (BB_dom_set(head), mp);
        dom_pro = BS_Union1D (dom_pro, (BS_ELT)BB_id(prolog),mp);
        dom_pro = BS_Difference1D (dom_pro, (BS_ELT)BB_id(head));
        Set_BB_dom_set (prolog, dom_pro);
#ifdef TARG_ST
	// FdF 20050629. Also, update pdom_set. This is needed in case
	// this prolog will be the unique successor of another loop
	// epilog (ddts 22256).
        BS* pdom_pro = BS_Copy (BB_pdom_set(head), mp);
        pdom_pro = BS_Union1D (pdom_pro, (BS_ELT)BB_id(prolog), mp);
        Set_BB_pdom_set (prolog, pdom_pro);
#endif
    }
     
    if (epilog) {
	BBLIST *p;
	BS* dom_epi = NULL;
	FOR_ALL_BB_PREDS (epilog, p) {
	    BB* pred = BBLIST_item (p);
	    dom_epi = (dom_epi == NULL)
		? BS_Copy(BB_dom_set(pred), mp)
		: BS_IntersectionD(dom_epi, BB_dom_set(pred));
	}
	dom_epi = BS_Union1D (dom_epi, (BS_ELT)BB_id(epilog), mp);
	Set_BB_dom_set (epilog, dom_epi);

	/* Post dominators are only partially updated. */
        BB* uniq = BB_Unique_Successor(epilog);
        if (uniq == NULL) {
          epilog = NULL; //cancel
        } else {
          BS* pdom_epi = BS_Copy(BB_pdom_set(uniq), mp);
          pdom_epi = BS_Union1D(pdom_epi, BB_id(epilog), mp);
          Set_BB_pdom_set (epilog, pdom_epi);
        }
    }

        /* propagate the changes
         */
    for (BB* b = REGION_First_BB; b; b = BB_next(b)) {

            /* <prolog> dominate <b> iff (1)<prolog> == <b> 
             *    or (2)- <head> dominate <b>.
             */
        BS* dom_set = BB_dom_set (b);
        if (BS_MemberP (BB_dom_set(b), BB_id(head))) {
            dom_set = BS_Union1D (dom_set, BB_id(prolog), mp);
            Set_BB_dom_set (b, dom_set);
        }

	/* <epilog> dominates <b> iff (1)<epilog> == <b>
	 *    or (2)-<head> dominate <b> and <b> is not in body(l).
	 */
	if (epilog &&
	    BS_MemberP(BB_dom_set(b), BB_id(head)) &&
	    !BS_MemberP(LOOP_DESCR_bbset(l), BB_id(b))) {
	    dom_set = BS_Union1D (dom_set, BB_id(epilog), mp);
	    Set_BB_dom_set (b, dom_set);
	}
    }
}

static BB*
Create_Loop_Prologue (LOOP_DESCR* l) {
    
    BB* lhead = LOOP_DESCR_loophead(l);
    BB_SET* body = LOOP_DESCR_bbset(l);
    BB* like_prolog = NULL;

    BBLIST* p;
    FOR_ALL_BB_PREDS (lhead, p) {
        BB* pred = BBLIST_item (p);
        if (BB_SET_MemberP (body, pred)) { continue; }
        
        if (like_prolog) {
            like_prolog = NULL; break; 
        } else {
            like_prolog = pred;
        }
    }

    if (like_prolog) {
        if (BB_kind (like_prolog) != BBKIND_GOTO || 
#ifdef TARG_ST
	    BB_asm (like_prolog) ||
#endif
            BB_branch_op (like_prolog)) {
            like_prolog = NULL;
        }
    }

    if (like_prolog) {
        Set_BB_gra_spill(like_prolog);
    }

    Is_True (!like_prolog || BB_prev(lhead) == like_prolog, 
             ("BB:%d's does not fall through to BB:%d", 
               BB_id(like_prolog), BB_id(lhead)));

    if (!like_prolog) {
#ifdef TARG_ST
      if (LOOP_DESCR_Can_Retarget_Loop_Entrances(l)) {
	// FdF 20070604: Also check that the loop prolog can be
	// created to fall-thru into lhead, without inserting
	// auxiliiary node, because otherwise update of dominators is
	// less obvious.
	BB *ftp = BB_Fall_Thru_Predecessor(lhead);
	if (!ftp || !BB_SET_MemberP(LOOP_DESCR_bbset(l), ftp) || (BB_branch_op(ftp) == NULL)) {
	  like_prolog = CG_LOOP_Gen_And_Prepend_To_Prolog (lhead,l);
	  GRA_LIVE_Compute_Liveness_For_BB(like_prolog);
	}
      }
#else
      like_prolog = CG_LOOP_Gen_And_Prepend_To_Prolog (lhead,l);
#endif
    }
              
    return like_prolog;
}

#ifdef TARG_ST

/* Most of the following code for creation of loop epilog is taken
   from cg_loop.cxx. */

static void retarget_loop_exits(LOOP_DESCR *loop, BB *from, BB *to)
/* -----------------------------------------------------------------------
 * Requires: <to> is inserted where intended in BB chain
 *
 * Change all exits from <loop> to <from> so they exit to <to> instead.
 * Updates pred/succ lists and frequency info.
 * -----------------------------------------------------------------------
 */
{
  BBLIST *preds = BB_preds(from);
  while (preds) {
    BB *pred = BBLIST_item(preds);
    preds = BBLIST_next(preds);
    if (BB_SET_MemberP(LOOP_DESCR_bbset(loop), pred))
      if (!BB_Retarget_Branch(pred, from, to))
	/* must fall-through to <from> */
	Change_Succ(pred, from, to);
  }
}

static BOOL all_bbs_in(BBLIST *list, BB_SET *bbs)
/* -----------------------------------------------------------------------
 * Return TRUE iff all BBs in <list> are members of <bbs>.
 * -----------------------------------------------------------------------
 */
{
  BBLIST *item;
  FOR_ALL_BBLIST_ITEMS(list, item)
    if (!BB_SET_MemberP(bbs, BBLIST_item(item))) return FALSE;
  return TRUE;
}

/* We can create an epilog node if the loop has one single exit, or
   all the exits have the same unique successor. */
static BB*
Create_Loop_Epilogue (LOOP_DESCR* l) {
    BB* like_epilog = NULL;
    BB_SET* body = LOOP_DESCR_bbset (l);
    BOOL freqs = FREQ_Frequencies_Computed();

    BB *bb;
    FOR_ALL_BB_SET_members(body, bb) {
        BBLIST* s;
        FOR_ALL_BB_SUCCS (bb, s) {
            BB* succ = BBLIST_item (s);
            if (!BB_SET_MemberP (body, succ)) {
		if (like_epilog && succ != like_epilog)
		    return NULL;
		like_epilog = succ;
	    }
	}
    }

    /* This is an infinite loop. */
    if (like_epilog == NULL)
	return NULL;

    /* Code from cg_loop.cxx */

    BB *next = like_epilog;
    BB *ftp = BB_Fall_Thru_Predecessor(next);
    BBKIND ftp_kind = ftp ? BB_kind(ftp) : BBKIND_UNKNOWN;

#ifdef TARG_ST
    // filters out hwloops. the epilog is correct by construction
    // therefore we must not create a new epilog BB which breaks the
    // hwloop contraints.
    if (ftp) {
      OP* bop = BB_branch_op(ftp);
      if (bop) {
        if (bop && OP_Is_Counted_Loop(bop)) {
          return like_epilog;
        }
      }
    }
#endif

    if (ftp && !BB_SET_MemberP(LOOP_DESCR_bbset(l), ftp)) {
	/*
	 * Since <ftp> isn't in loop, we don't want it to fall through
	 * to like_epilog.  It was falling through to <next>, so we'll
	 * make it either branch to <next> or fall through to a new BB
	 * that branches to <next>.  */
	if (ftp_kind == BBKIND_GOTO) {
	    BBLIST *bbl = BB_Find_Succ(ftp, next);
	    Add_Goto(ftp, next);
	    BBLIST_prob(bbl) = 1.0F;	/* otherwise is incremented to 2.0 */
	} else {
	  // FdF: Code from cg_loop.cxx inserts an empty node as the
	  // fall through from ftp and inserts a goto to
	  // next. However, here we need to maintain the dominance
	  // information (see Maintain_Dominator_Info), which is just
	  // too complicated here.
	    return NULL; 
	}
    }

    LOOP_DESCR *enclosing = LOOP_DESCR_Next_Enclosing_Loop(l);
    like_epilog = Gen_And_Insert_BB_Before(next);
    BB_rid(like_epilog) = BB_rid(LOOP_DESCR_loophead(l));
    Set_BB_gra_spill(like_epilog);
    if (BB_freq_fb_based(LOOP_DESCR_loophead(l))) Set_BB_freq_fb_based(like_epilog);
    retarget_loop_exits(l, next, like_epilog);
    Link_Pred_Succ_with_Prob(like_epilog, next, 1.0);
    if (freqs || BB_freq_fb_based(next))
	BB_freq(next) += BB_freq(like_epilog);
    GRA_LIVE_Compute_Liveness_For_BB(like_epilog);

    /* Add like_epilog to appropriate LOOP_DESCRs, if any.
     * It can belong only to loops enclosing this one, so
     * we don't bother checking any others.  */
    if (enclosing &&
	all_bbs_in(BB_preds(like_epilog), LOOP_DESCR_bbset(enclosing)))
	LOOP_DESCR_Add_BB(enclosing, like_epilog);

    return like_epilog;
}
#endif

    /* ==================================================================
     * 
     * Perform_Loop_Invariant_Code_Motion 
     *      The wrapper of LOOP_INVAR_CODE_MOTION::Perform_Loop_Motion (). 
     *      This function is exported to outside.
     *
     * ==================================================================
     */
void
Perform_Loop_Invariant_Code_Motion (void) {

    INT32 code_motion_num;

    CXX_MEM_POOL local_mp("LICM", FALSE);

    MEM_POOL loop_descr_pool;
    MEM_POOL_Initialize(&loop_descr_pool, "loop_descriptors", TRUE);
    MEM_POOL_Push (&loop_descr_pool);

    LICM_Trace_Non_Profitable = Get_Trace(TP_LICM, 0x001);
    LICM_Trace_Profitable = Get_Trace(TP_LICM, 0x002);
    LICM_Trace_Candidates = Get_Trace(TP_LICM, 0x004);

        /* make sure liveness is accurate 
         */
    GRA_LIVE_Init (NULL);
    Calculate_Dominators ();

    code_motion_num = 0;

    for (LOOP_DESCR *l = LOOP_DESCR_Detect_Loops(&loop_descr_pool);
         l; l = LOOP_DESCR_next(l)) {

        if (Skip_Loop_Invar_Code_Motion (l)) { continue; }

        BB* prolog;
        prolog = Create_Loop_Prologue (l);
        if (!prolog) { continue; }
        Reset_BB_gra_spill (prolog);

#ifdef TARG_ST
        BB* epilog;
        epilog = Create_Loop_Epilogue (l);
        if (epilog) Reset_BB_gra_spill (epilog);
#endif

#ifdef TARG_ST
        Maintain_Dominator_Info (l, prolog, epilog, local_mp());
        LOOP_INVAR_CODE_MOTION licm (l, prolog, epilog);
#else
        Maintain_Dominator_Info (l, prolog, local_mp());
        LOOP_INVAR_CODE_MOTION licm (l, prolog);
#endif
        code_motion_num = licm.Perform_Code_Motion ();
    }

    GRA_LIVE_Init (NULL);
    Free_Dominators_Memory ();

    MEM_POOL_Pop (&loop_descr_pool);
    MEM_POOL_Delete(&loop_descr_pool);

}
