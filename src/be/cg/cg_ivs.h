/*
  Copyright (C) 2001, STMicroelectronics, All Rights Reserved.

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


//-*-c++-*-
// =======================================================================
// =======================================================================
//
//  Module: cg_ivs.h
//
//  Description:
//  ============

//  This module consists of Loop Induction Variables analysis and
//  optimization
//
// 
//  Exported Functions:
//  ===================
//
// LOOP_IVS::LOOP_IVS(MEM_POOL *mem_pool)
//    Constructor for a new LOOP_IVS instance
//
//  LOOP_IVS::~LOOP_IVS(void)
//    Destructor for a LOOP_IVS instance
//
//  void LOOP_IVS::Init( LOOP_DESCR *loop )
//    Initializes a LOOP_IVS instance for a loop with no internal
//    control, multiple exits and call instructions are allowded.
//
//  DefID_t OPND_IV_cycle(INT op_idx, INT opnd_idx);
//    Returns an ID for an Induction Variable for an operand that
//    belongs to an IV cycle. This ID is the last operation of the
//    loop that defines a TN that belongs to this IV cycle
//
//  DefID_t RES_IV_cycle(INT op_idx, INT res_idx);
//    Returns an ID for an Induction Variable for the defintion of a
//    TN that belongs to an IV cycle
//
//  INT OPND_IV_offset(INT op_idx, INT opnd_idx);
//    Returns the constant value from this use to the value of the IV
//    at the beginnning of a loop iteration.
//
//  INT IV_step(DefID_t iv_cycle);
//    Get the step of an IV
//
//  OP *IV_init(DefID_t iv_cycle);
//    Get the init operation outside of the loop of an IV
//
//  void Trace_IVs_Entries(const char *message);
//    Dump some traces for an LOOP_IVS instance.
//
// =======================================================================
// ======================================================================= */

#ifdef TARG_ST
#include "bb.h"
#include "cg_loop.h"
#include "findloops.h"

#ifndef CG_IVS_INCLUDED
#define CG_IVS_INCLUDED

extern BOOL CG_IV_offset_space;

// A use-def link is represented as an integer composed with:
//  24 bits for the index of the op in the IVS entries
//   8 bits for the index of the def in the op
typedef int DefID_t;
static inline DefID_t DEFID_make(UINT op_idx, UINT res_idx) { return (op_idx << 8 | res_idx); };
static inline UINT DEFID_idx(DefID_t defid) { return (defid >> 8); }
static inline UINT DEFID_res(DefID_t defid) { return (defid & 0xff); }

// A def-use link is represented as an integer composed with:
//  24 bits for the index of the op in the IVS entries
//   8 bits for the index of the use in the op
typedef unsigned int UseID_t;
static inline UseID_t USEID_make(UINT op_idx, UINT opnd_idx) { return (op_idx << 8 | opnd_idx); };
static inline UINT USEID_idx(UseID_t useid) { return (useid >> 8); }
static inline UINT USEID_opnd(UseID_t useid) { return (useid & 0xff); }

class OP_IV {
  friend class LOOP_IVS;

 private:

  OP    *op;
  /* For each result */
  DefID_t *IV_cycle;
  INT     *IV_offset;
  short   *UC;
  /* For each argument */
  DefID_t *UDlink;

  OP_IV(void) :
    op(NULL),
    IV_cycle(NULL),
    IV_offset(NULL),
    UC(NULL),
    UDlink(NULL)
  {}

  ~OP_IV() {}

  void alloc_OP_IV(INT results, INT opnds, MEM_POOL *mempool) {
    INT size = (sizeof(*IV_cycle) + sizeof(*IV_offset) + sizeof(*UC)) * results + sizeof(*UDlink) * opnds;
    // Not already allocated
    if (IV_cycle == NULL) {
      void *ptr = MEM_POOL_Alloc(mempool, size);
      memset(ptr, 0, size);
    
      IV_cycle = (DefID_t *)ptr;
      IV_offset = (INT *)((char *)IV_cycle + sizeof(*IV_cycle) * results);
      UDlink = (DefID_t *)((char *)IV_offset + sizeof(*IV_offset) * results);
      UC = (short *)((char *)UDlink + sizeof(*UDlink) * opnds);
    }
    Is_True((char*)UC + sizeof(*UC) * results - (char *)IV_cycle == size, ("OP_IV: Allocation error"));
  }

  void init_OP_IV(OP *cur_op, MEM_POOL *mempool) {
    INT results = (cur_op != NULL) ? OP_results(cur_op) : 1;
    INT opnds = (cur_op != NULL) ? OP_opnds(cur_op) : 0;
    alloc_OP_IV(results, opnds, mempool);
    op = cur_op;
  }

  void del_OP_IV(MEM_POOL *mempool) {
    MEM_POOL_FREE(mempool, IV_cycle);
  }

  void setIV(INT res_idx, DefID_t iv_cycle, INT iv_offset)  {
    IV_cycle[res_idx] = iv_cycle; IV_offset[res_idx] = iv_offset;
  }
  void setNotIV(INT res_idx)  {
    IV_cycle[res_idx] = 0; IV_offset[res_idx] = 1;
  }
  BOOL isIV(INT res_idx) {
    return (IV_cycle[res_idx] != 0);
  }
  BOOL notIV(INT res_idx) {
    return (IV_cycle[res_idx] == 0) && (IV_offset[res_idx] != 0);
  }
};

class LOOP_IVS {
 private:

  // For an operation op, attach IV information and def-use links on
  // results, and use-def links on operands.
  MEM_POOL *_loc_mem_pool;

  // For each operation in the loop there is an entry in the table,
  // starting at 1.
  OP_IV *ivs_table;
  INT ivs_count;

  // Information on the loop
  BB *head;
  BB *tail;
  BB *prolog;
  BB *epilog;

  OP_IV *new_IVS_Table(INT size) {
    OP_IV * op_iv = CXX_NEW_ARRAY( OP_IV, size, _loc_mem_pool );
    // TBD: Check this is needed
    memset(op_iv, 0, sizeof(OP_IV)*size);
    return op_iv;
  }
  void del_IVS_Table() {
    for (INT i = 0; i < ivs_count; i++)
      ivs_table[i].del_OP_IV(_loc_mem_pool);
    CXX_DELETE_ARRAY( ivs_table, _loc_mem_pool );
  }
  void Init_IVs_Table(LOOP_DESCR *loop, BB **sorted_bbs, UINT32 num_bbs);

  // Return an OP entry from a use-def link
  OP_IV& RES_entry(DefID_t defid) { return ivs_table[DEFID_idx(defid)]; }

  // Functions related to compuation of IV cycles
  void RES_setIV(DefID_t defid, DefID_t iv_cycle, INT iv_offset) {
    RES_entry(defid).setIV(DEFID_res(defid), iv_cycle, iv_offset);
  }
  void RES_setNotIV(DefID_t defid) { RES_entry(defid).setNotIV(DEFID_res(defid)); }
  BOOL RES_isIV(DefID_t defid) { return RES_entry(defid).isIV(DEFID_res(defid)); }
  BOOL RES_notIV(DefID_t defid) { return RES_entry(defid).notIV(DEFID_res(defid)); }
  DefID_t Find_IV( DefID_t defid, DefID_t in_iv_cycle );
  DefID_t IV_cycle(DefID_t defid) { INT x=RES_entry(defid).IV_cycle[DEFID_res(defid)]; return x<0?-x:x;}
  INT IV_offset(DefID_t defid) { return RES_entry(defid).IV_offset[DEFID_res(defid)]; }
  // Return whether the use-def link crosses an iteration or not.
  BOOL OPND_hasOmega(INT op_idx, INT opnd_idx) { return DEFID_idx(OPND_deflink(op_idx, opnd_idx)) >= op_idx; }

 public:

 LOOP_IVS(MEM_POOL *mem_pool)
   : _loc_mem_pool(mem_pool) ,
    ivs_table( NULL ),
    ivs_count( 0 ),
    head (NULL),
    tail (NULL),
    prolog (NULL),
    epilog (NULL) {}

  ~LOOP_IVS(void) {}

  BB *Head() { return head; }
  BB *Tail() { return tail; }
  BB *Prolog() { return prolog; }
  BB *Epilog() { return epilog; }
  INT First_opidx() { return 1; }
  INT Last_opidx() { return ivs_count-1; }
  INT Size() { return ivs_count; }

  MEM_POOL *Mem_pool() { return _loc_mem_pool; }

  // For a loop, build a table that provides access to Use-Def links
  // and induction variable information
  void Init( LOOP_DESCR *loop, BB **sorted_bbs, UINT32 num_bbs );

  // Replace one OP in the loop by another one, but they they must
  // have identical number of results and arguments, and have similar
  // semantics.
  void Replace_Op(INT index, OP *new_op);

  // Returns the OP associated to an index into the table
  OP *Op(INT op_idx) {
    return ((op_idx > 0) && (op_idx < ivs_count)) ? ivs_table[op_idx].op : NULL;
  }

  // Returns a (op_idx, res_idx) that links to the definition for an OPND
  DefID_t OPND_deflink(INT op_idx, INT opnd_idx) { return ivs_table[op_idx].UDlink[opnd_idx]; }
  // Set a use-def link for an OPND
  void Set_OPND_deflink(INT op_idx, INT opnd_idx, DefID_t defid) { ivs_table[op_idx].UDlink[opnd_idx] = defid; }

  // Returns an (op_idx, res_idx) that represents the Induction
  // Variable this OPND belongs to
  DefID_t OPND_IV_cycle(INT op_idx, INT opnd_idx);

  // Returns the offset such that the value of the IV at loop entry +
  // offset is the value of this OPND
  INT OPND_IV_offset(INT op_idx, INT opndx_idx);

  // Returns an (op_idx, res_idx) that represents the Induction
  // Variable this result belongs to
  DefID_t RES_IV_cycle(DefID_t defid);

  // Returns the step of IV
  INT IV_step(DefID_t iv_cycle);

  // Give the first use of the loop that belongs to this IV cycle. In
  // SSA mode, this will be the loop operand in a PHI operation.
  void IV_first(DefID_t iv_cycle, INT &op_idx, INT &opnd_idx);

  // Returns the operation outside of the loop that defines the value
  // of the IV at loop entry for the first iteration
  OP *IV_init(DefID_t iv_cycle);

  // Functions related to Usage Count of defintions
  INT RES_UC(DefID_t defid) { return ivs_table[DEFID_idx(defid)].UC[DEFID_res(defid)]; }
  void Set_RES_UC(DefID_t defid, INT uc) { ivs_table[DEFID_idx(defid)].UC[DEFID_res(defid)] = uc; }
  void Inc_RES_UC(DefID_t defid) { ivs_table[DEFID_idx(defid)].UC[DEFID_res(defid)] ++; }
  void Dec_RES_UC(DefID_t defid) { ivs_table[DEFID_idx(defid)].UC[DEFID_res(defid)] --; }

  void Trace_IVs_Entries(const char *message);
};

#define FOR_ALL_LOOP_IVS_OPs_FWD(loop_ivs, opidx, op) \
  for (op = (loop_ivs)->Op(opidx=((loop_ivs)->First_opidx())); opidx <= (loop_ivs)->Last_opidx(); op = (loop_ivs)->Op(++opidx))

#define FOR_ALL_LOOP_IVS_OPs_REV(loop_ivs, opidx, op) \
  for (op = (loop_ivs)->Op(opidx=((loop_ivs)->Last_opidx())); opidx >= (loop_ivs)->First_opidx(); op = (loop_ivs)->Op(--opidx))

BOOL Optimize_Loop_Induction_Offsets( LOOP_DESCR *loop );

/* CG_LOOP load_store_packing:
   0x1 : Enable load packing
   0x2 : Enable store packing
   0x4 : Enable load packing also if alignment is unknown
   0x8 : Enable loop peeling
   0x10: Enable loop specialization
   0x20: Enable loop peeling also when loop trip count is a compile time literal value
   0x40: Force unroll factor to 2 if required for packing and the
         loop would not be unrolled otherwise.
   0x80: Enable packing only on memory bounded loops, but keep other
         loop transformations related to packing.
   0x100:Perform packing analysis and loop transformation, but do not
         actually pack
   0x200:Perform load packing in EBO
*/
enum LOAD_STORE_PACKING {
  PACKING_LOAD				= 0x1,
  PACKING_STORE				= 0x2,
  PACKING_LOAD_DYN_ALIGN 		= 0x4,
  PACKING_LOOP_PEELING			= 0x8,
  PACKING_LOOP_SPECIALIZATION		= 0x10,
  PACKING_LOOP_PEELING_CONST_TRIP_COUNT = 0x20,
  PACKING_FORCE_LOOP_UNROLL		= 0x40,
  PACKING_ONLY_MEMORY_BOUNDED		= 0x80,
  PACKING_NO_ACTUAL_PACKING		= 0x100,
  PACKING_EBO_LOAD			= 0x200
};
extern INT32 Loop_packing_flags;
extern INT32 CG_LOOP_stream_align;
extern BOOL CG_LOOP_opt_iv_offset;

extern BOOL IVS_Analyze_Load_Store_Packing( CG_LOOP &cg_loop);
extern BOOL IVS_Perform_Load_Store_Packing( CG_LOOP &cg_loop);
extern INT  Loop_Packing_Options(CG_LOOP &cg_loop);
#endif /* CG_IVS_INCLUDED */

#endif /* TARG_ST */
