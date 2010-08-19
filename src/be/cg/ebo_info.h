/*
 * Copyright (C) 2008 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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


/* =======================================================================
 * =======================================================================
 *
 *  Module: ebo_info.h
 *  $Revision: 1.10 $
 *  $Date: 05/04/19 14:28:25-07:00 $
 *  $Author: kannann@iridot.keyresearch $
 *  $Source: be/cg/SCCS/s.ebo_info.h $
 *
 *  Revision comments:
 *
 *  29-May-1998 - Initial version
 *
 *  Description:
 *  ============
 *
 *  	EBO_TN_INFO
 *		This data structure is used to keep track of information related
 *		to a particular TN.  The TN_MAP utilities are used to associate
 *		one of these entries with a TN.  The fields in the entry are:
 *
 *	     INT sequence_num
 *		A number indicating the order in which the entry was created.
 *	     TN *local_tn
 *		A pointer back to the original TN.
 *	     TN *replacement_tn
 *		Another TN that is equivalent to the original TN.  It can be used
 *		to forward-propagate an assignment to the original TN or to track
 *		a constant that could be used in place of the TN.
 *	     EBO_TN_INFO *replacement_tninfo
 *		This is a pointer to the EBO_TN_INFO entry that is associated
 *		with the replacement_tn entry.
 *	     mUINT8 omega
 *		If EBO is called during loop optimization, the omega for the
 *		operand is copied, otherwise this field is set to zero which
 *		is intended to mean: "the current iteration of the loop".
 *	     EBO_TN_INFO *predicate_tninfo
 *		This is a pointer to the EBO_TN_INFO entry that is associated
 *		with the predicate undef which this TN is defined.
 *	     BB *in_bb
 *		A pointer to the BB that caused the EBO_TN_INFO to be created.
 *	     OP *in_op
 *		A pointer to the OP that caused the EBO_TN_INFO to be created.
 *	     EBO_OP_INFO *in_opinfo
 *		A pointer to the opinfo entry that caused this EBO_TN_INFO 
 *		entry to be created.  This means that the TN associated with
 *		this EBO_TN_INFO entry is a result TN of the OP associated
 *		with the EBO_OP_INFO that is pointed to.
 *	     INT reference_count
 *		A count of the number of references to this TN in the EB.
 *	     EBO_TN_INFO *same
 *		A pointer to other occurances of the same TN name.  This field
 *		can be used to track and resolve references to TN's when
 *		predication is involved.
 *	     EBO_TN_INFO *prior
 *	     EBO_TN_INFO *next
 *		Used to support an ordering of EBO_TN_INFO entries so they can be
 *		reclaimed after a block is processed.
 *
 *	EBO_OP_INFO;
 *		This data structure is used to keep track of information related
 *		to a particular OP.  A hash table is used to track down identical
 *		OP entries.  The fields in the entry are:
 *
 *	     INT hash_index
 *		The original hash value generated from the OP.
 *	     BOOL in_delay_slot
 *		If this instruction is in a delay slot, it must be turned into
 *		a noop instead of being removed. 
 *	     BOOL op_must_not_be_removed
 *		If a condition is encountered that requires keeping this op,
 *		set this flag.  The most common case is to mark a store because
 *		a potentially aliased load is encountered.
 *	     BOOL op_must_not_be_moved
 *		Some transformations require deleting an OP and re-generating it
 *		in a different place in the block.  This may not always be safe
 *		and this flag will be set when the condition arises.
 *		For example: this flag is set when a load is followed by a
 *		store to the same location because moving the load to follow the
 *		store must not be allowed.
 *	     BB *in_bb
 *		A pointer to the block that contains the OP entry.
 *	     OP *in_op
 *		A pointer to the original OP that caused this EBO_OP_INFO entry
 *		to be created.
 *	     EBO_OP_INFO *same
 *		A pointer to other EBO_OP_INFO entries with the same hash value.
 *	     EBO_OP_INFO *prior
 *	     EBO_OP_INFO *next
 *		Used to support an ordering of EBO_OP_INFO entries so they can be
 *		reclaimed after a block is processed.
 *	     EBO_TN_INFO **actual_rslt
 *		Pointers to the associated EBO_TN_INFO entries for each (non
 *		constant) result. 
 *	     EBO_TN_INFO **actual_opnd
 *		Pointers to the associated EBO_TN_INFO entries for each (non
 *		constant) operand.  These EBO_TN_INFO pointers are associated
 *		with the actual TNs in the expression. They are the entries
 *		that must be decremented when the OP is deleted.
 *	     EBO_TN_INFO **optimal_opnd
 *		Pointers to the associated EBO_TN_INFO entries for each (non
 *		constant) operand.  If the original operand could have been
 *		replaced by another TN, this is a pointer to the replacement
 *		EBO_TN_INFO entry.  That way, the most current value is
 *		available, even if the semantics of the OP does not allow the
 *		operand field to contain the replacement TN.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef EBO_INFO_INCLUDED
#define EBO_INFO_INCLUDED

#ifdef TARG_ST
#include "pqs_cg.h"
#endif

/* forward decls */
typedef struct local_tn_info EBO_TN_INFO;
typedef struct local_op_info EBO_OP_INFO;

/* =====================================================================
 *
 * Define the key data structure that will be used to track information
 * associated with each definition of a TN.
 *
 * ===================================================================== */
typedef struct local_tn_info {
	INT sequence_num;		/* Creation order number. */
        TN *local_tn;			/* My name. */
	TN *replacement_tn;		/* Rename TN with this new name. */
	EBO_TN_INFO *replacement_tninfo;/* Rename TN with this tninfo.   */
#ifdef TARG_ST
	TN *copy_tn;			/* Alternative physical replacement TN. */
	EBO_TN_INFO *copy_tninfo;	/* Alternative physical replacement tninfo.   */
	mBOOL false_predicate_tn; 	/* To manage ! True_TN */
#endif

	EBO_TN_INFO *predicate_tninfo;	/* link to predicate tninfo. */
        BB *in_bb;			/* The defining BB. */
        OP *in_op;			/* The defining OP. */
        EBO_OP_INFO *in_opinfo;		/* The defining EBO_OP_INFO entry. */
        EBO_TN_INFO *same;		/* Other definitions of the same TN. */
        EBO_TN_INFO *prior;		/* Used previous info node. */
        EBO_TN_INFO *next;		/* Used next info node. */
        INT32 reference_count;		/* Number of references to this TN. */
        mUINT8 omega;			/* The associated omega. */
	mBOOL redefined_before_block_end; /* A following definition exists. */
} EBO_TN_INFO;

/* Keep track of lists of used and free EBO_TN_INFO entries. */
extern INT EBO_tninfo_number;
extern EBO_TN_INFO *EBO_free_tninfo;
extern EBO_TN_INFO *EBO_first_tninfo;
extern EBO_TN_INFO *EBO_last_tninfo;

/* =====================================================================
 *
 * Define the key data structure that will be used to track information
 * associated with each defining OP.
 *
 * ===================================================================== */
typedef struct local_op_info {
	INT hash_index;			/* Hash value of defining op. */
	mBOOL in_delay_slot;         	/* Special handling required. */
	mBOOL op_must_not_be_removed;	/* Some condition requires this op. */
	mBOOL op_must_not_be_moved;	/* This OP can not be moved or recreated. */
        BB *in_bb;			/* The defining BB. */
        OP *in_op;			/* The defining OP. */
#ifdef KEY
        int op_num;			/* OP's number in BB; first OP is 1 */
#endif
        EBO_OP_INFO *same;		/* Other OPs with the same hash. */
        EBO_OP_INFO *prior;		/* Previous info node. */
        EBO_OP_INFO *next;		/* Next info node. */
	EBO_TN_INFO **optimal_opnd;	/* Operand array (ptr into tn_info) */
#ifdef TARG_ST
        TN *optimal_tn[OP_MAX_FIXED_OPNDS];  /* FdF 20070402: A copy of
					       opnd_tn when calling
					       add_to_hash_table, so
					       as to keep a TN on
					       immediate values. */
#endif
        EBO_TN_INFO **actual_opnd;	/* Operand array (ptr into tn_info) */
	EBO_TN_INFO **actual_rslt;	/* Result  array (ptr into tn_info) */
	EBO_TN_INFO *tn_info[		/* Variable size array for EBO_TN_INFOs */
		OP_MAX_FIXED_RESULTS +	/* for above result and opnd arrays */
		OP_MAX_FIXED_OPNDS +	/* (see EBO_OP_INFO_sizeof). */
		OP_MAX_FIXED_OPNDS];	/* !!!! tn_info MUST BE LAST!!!! */
} EBO_OP_INFO;


#ifdef TARG_ST
// FdF 20070402
inline TN *
Opinfo_optimal_tn(EBO_OP_INFO *opinfo, INT idx) {
  if (idx < OP_MAX_FIXED_OPNDS) {
    return opinfo->optimal_tn[idx];
  }
  else {
    OP *op = opinfo->in_op;
    return OP_opnd(op, idx);
  }
}
#endif

/* Given an op, return the 'sizeof' the EBO_OP_INFO struct needed
 * for that OP. For the sake of simplicity, the result of the calculation 
 * is often slightly larger than necessary and not rounded up to
 * the alignment of the struct. The size of the struct allocated,
 * is always large enough to represent all OPs with fixed numbers of
 * operands/results (so that the structure can be reused efficiently).
 */
inline size_t EBO_OP_INFO_sizeof(OP *op)
{
  size_t sizeof_info = sizeof(EBO_OP_INFO);
  INT extra_tns =   OP_results(op) - OP_MAX_FIXED_RESULTS
		  + OP_opnds(op) - OP_MAX_FIXED_OPNDS
		  + OP_opnds(op) - OP_MAX_FIXED_OPNDS;
  if (extra_tns > 0) {
    sizeof_info += sizeof(EBO_TN_INFO *) * extra_tns;
  }
  return sizeof_info;
}

#define EBO_DEFAULT_MEM_HASH 0
#define EBO_NO_ALIAS_MEM_HASH 1
#define EBO_SPILL_MEM_HASH 2
#define EBO_MAX_MEM_OP_HASH 3
#define EBO_COPY_OP_HASH 3
#define EBO_RESERVED_OP_HASH 4
#define EBO_MAX_EXP_OP_HASH 1024
#define EBO_MAX_OP_HASH (EBO_RESERVED_OP_HASH + EBO_MAX_EXP_OP_HASH)
#define EBO_EXP_OP_HASH(val) ((EBO_MAX_EXP_OP_HASH-1) & (val>>6))

/* Keep track of lists of used and free EBO_OP_INFO entries. */
extern EBO_OP_INFO *EBO_free_opinfo;
extern EBO_OP_INFO *EBO_first_opinfo;
extern EBO_OP_INFO *EBO_last_opinfo;
extern EBO_OP_INFO *EBO_opinfo_table[EBO_MAX_OP_HASH];

extern BOOL EBO_in_pre;
extern BOOL EBO_in_loop;
extern BOOL EBO_in_peep;
extern BOOL EBO_flow_safe;

extern TN_MAP EBO_tninfo_table;
extern MEM_POOL EBO_pool;

extern INT EBO_num_tninfo_entries;
extern INT EBO_tninfo_entries_reused;
extern INT EBO_num_opinfo_entries;
extern INT EBO_opinfo_entries_reused;

extern const char *EBO_trace_pfx;
extern BOOL EBO_Trace_Execution;
extern BOOL EBO_Trace_Optimization;
extern BOOL EBO_Trace_Block_Flow;
extern BOOL EBO_Trace_Data_Flow;
extern BOOL EBO_Trace_Hash_Search;

/* ===================================================================== */
/* Define macros that will allow us to treat Zero_TN as having a value of 0. */
#define TN_Is_Constant(tn) (TN_is_const_reg(tn) ? TRUE : TN_is_constant(tn))
#define TN_Has_Value(tn) (TN_is_const_reg(tn) ? TRUE : TN_has_value(tn))
#define TN_Value(tn) ((tn == Zero_TN) ? 0 : TN_value(tn))
#define has_assigned_reg(tn) (TN_is_register(tn) &&			\
				(TN_is_dedicated(tn) ||			\
				 TN_register(tn) != REGISTER_UNDEFINED))

/* ===================================================================== */
/*
 * These macros  map each  TN to the TN_INFO entry that is
 * used to track it.  If registers have been assigned, all
 * TNs are mapped to the dedicated TNs that were initially
 * created to define the entire available register set.
 *
 * The basic assumption is that if a TN has an assigned
 * register, any dominating definition of that TN has the
 * same register assigned.
 */
#define use_tn_or_reg(tn) (has_assigned_reg(tn)            \
             ? Build_Dedicated_TN ( TN_register_class(tn), \
                                    TN_register(tn),       \
                                    0)                     \
             : tn)
#define get_tn_info(tn) ((EBO_TN_INFO *)TN_MAP_Get(EBO_tninfo_table, use_tn_or_reg(tn)))
#define set_tn_info(tn, tninfo) (TN_MAP_Set(EBO_tninfo_table, use_tn_or_reg(tn), tninfo))
#define inc_ref_count(tninfo) ((void)tninfo->reference_count++)
#define dec_ref_count(tninfo) ((void)tninfo->reference_count--)

/* ===================================================================== */
/* The functions that are associated with processing EBO_TN_INFO and
 * EBO_OP_INFO entries.
 */
void tn_info_entry_dump (EBO_TN_INFO *tninfo);
void tn_info_table_dump ();
#ifdef KEY
#if 0
void delete_useless_store_op (EBO_OP_INFO *opinfo);
#endif
#endif

inline EBO_TN_INFO *
get_new_tninfo (BB *current_bb, OP *current_op, TN *local_tn)
{
  EBO_TN_INFO *tninfo;

 /* Get space for a new entry. */
  if (EBO_free_tninfo) {
    tninfo = EBO_free_tninfo;
    EBO_free_tninfo = tninfo->next;
    tninfo->next = NULL;
    EBO_tninfo_entries_reused++;
  } else {
    tninfo = TYPE_MEM_POOL_ALLOC (EBO_TN_INFO, &EBO_pool);
    EBO_num_tninfo_entries++;
  }

 /* Initialize the entry. */
  tninfo->sequence_num = ++EBO_tninfo_number;
  tninfo->local_tn = local_tn;
  tninfo->replacement_tn = NULL;
  tninfo->replacement_tninfo = NULL;
#ifdef TARG_ST
  tninfo->copy_tn = NULL;
  tninfo->copy_tninfo = NULL;
  tninfo->false_predicate_tn = (current_op)?OP_PredOnFalse(current_op):false; ;
#endif
  tninfo->predicate_tninfo = NULL;
  tninfo->omega = 0;
  tninfo->in_bb = current_bb;
  tninfo->in_op = current_op;
  tninfo->in_opinfo = NULL;
  tninfo->reference_count = 0;
  tninfo->redefined_before_block_end = FALSE;
  tninfo->same = NULL;

 /* Build a list that is ordered by when we first saw the TN.       */
 /* This list is used to "backup" after a block has been processed. */
  tninfo->prior = EBO_last_tninfo;
  if (EBO_first_tninfo == NULL) {
      EBO_first_tninfo = tninfo;
  } else {
      EBO_last_tninfo->next = tninfo;
  }
  EBO_last_tninfo = tninfo;
  tninfo->next = NULL;

 /* Link into the MAP for future references. */
  set_tn_info (local_tn, tninfo);

  return tninfo;
} 

#ifdef TARG_ST
/*
 * Check that the 2 provided predicate TNs are equivalent.
 * TNs_Are_Equivalent() cannot be called directly because
 * it must be checked first that we are dealing with register TNs.
 */
inline BOOL
EBO_Predicate_TNs_Are_Equivalent(TN *pred1, TN *pred2) {
  return (pred1 == pred2 ||
          (TN_is_register(pred1) &&
           TN_is_register(pred2) &&
           TNs_Are_Equivalent(pred1, pred2)));
}
#endif


/*
 * EBO_predicate_dominates
 *
 * Return TRUE if the first predicate is TRUE everytime
 * that the second predicate is TRUE.
 *
 */
#ifdef TARG_ST
// (cbr) Support for guards on false
inline BOOL
EBO_predicate_dominates (TN *pred1, bool false1, EBO_TN_INFO *info1,
                         TN *pred2, bool false2, EBO_TN_INFO *info2)
{
  if (PQSCG_pqs_valid() && !false1 && !false2) {
    if (PQSCG_is_subset_of (pred2, pred1)) {
      return TRUE;
    }
  }

  if (pred1 == True_TN && !false1) {
   /* A TRUE predicate dominates everything. */
    return TRUE;
  }

  if (pred1 == Zero_TN && false1) {
   /* A TRUE predicate dominates everything. */
    return TRUE;
  }

  if (EBO_Predicate_TNs_Are_Equivalent(pred1, pred2) && false1 == false2) {
   /* Equal predicates are fine if the values are current. */
    return (info1 == info2);
  }
 /* Until we can resolve subsets, assume a problem. */
  return FALSE;
}
#else
inline BOOL
EBO_predicate_dominates (TN *pred1, EBO_TN_INFO *info1,
                         TN *pred2, EBO_TN_INFO *info2)
{
  if (pred1 == True_TN) {
   /* A TRUE predicate dominates everything. */
    return TRUE;
  }
  if (pred1 == pred2) {
   /* Equal predicates are fine if the values are current. */
    return (info1 == info2);
  }
 /* Until we can resolve subsets, assume a problem. */
  return FALSE;
}
#endif

#ifdef TARG_ST
/*
 * EBO_predicate_equivalent
 *
 * Return TRUE if the first predicate is TRUE if and only if
 * the second predicate is TRUE.
 *
 */
inline BOOL
EBO_predicate_equivalent (TN *pred1, bool false1, EBO_TN_INFO *info1,
			  TN *pred2, bool false2, EBO_TN_INFO *info2)
{
  return EBO_predicate_dominates(pred1, false1, info1, pred2, false2, info2) &&
    EBO_predicate_dominates(pred2, false2, info2, pred1, false1, info1);
}
#endif



/*
 * EBO_predicate_complements
 *
 * Return TRUE if the first predicate is TRUE everytime
 * that the second predicate is FALSE.
 *
 */
#ifdef TARG_ST
// (cbr) Support for guards on false
inline BOOL
EBO_predicate_complements (TN *pred1, bool false1, EBO_TN_INFO *info1,
                           TN *pred2, bool false2, EBO_TN_INFO *info2)
{
  if (PQSCG_pqs_valid () && !false1 && !false2) {
    PQS_TN_SET pred;
    pred.Insert (pred1); pred.Insert (pred2);
    if (PQSCG_is_disjoint (pred1, pred2)
	&& PQSCG_is_subset_of (True_TN, pred)) {
      return TRUE;
    }
  }
  if ((pred1 == True_TN && !false1) && (pred2 == Zero_TN && !false2)) {
    return TRUE;
  }
  if ((pred1 == Zero_TN && !false1) && (pred2 == True_TN && !false2)) {
    return TRUE;
  }
  if (EBO_Predicate_TNs_Are_Equivalent(pred1, pred2) && false1 != false2) {
    return (info1 == info2);
  }

  if ((info1 == NULL) || (info2 == NULL) ||
      (info1->in_op == NULL) || (info2->in_op == NULL)) {
    return FALSE;
  }
  if (!EBO_Predicate_TNs_Are_Equivalent(pred1, pred2) && (false1 == false2) && (info1->in_op == info2->in_op)
      && info1->in_op && OP_results(info1->in_op) == 1) {
   /* If defined by the same instruction but not equal, they must be complements. */
    return TRUE;
  }
 /* Until we can resolve subsets, assume a problem. */
  return FALSE;
}
#else
inline BOOL
EBO_predicate_complements (TN *pred1, EBO_TN_INFO *info1,
                           TN *pred2, EBO_TN_INFO *info2)
{
  if ((pred1 == True_TN) && (pred2 == Zero_TN)) {
    return TRUE;
  }
  if ((pred1 == Zero_TN) && (pred2 == True_TN)) {
    return TRUE;
  }
  if ((info1 == NULL) || (info2 == NULL) ||
      (info1->in_op == NULL) || (info2->in_op == NULL)) {
    return FALSE;
  }
  if ((pred1 != pred2) && (info1->in_op == info2->in_op)) {
   /* If defined by the same instruction but not equal, they must be complements. */
    return TRUE;
  }
 /* Until we can resolve subsets, assume a problem. */
  return FALSE;
}
#endif

#ifdef TARG_ST
/*
 * EBO_predicate_disjoint
 *
 * Return TRUE if the first predicate and the second predicate
 * are never true at the same time.
 * This is slightly weaker than the complements test, since
 * both may be false at the same time.
 *
 */
inline BOOL
EBO_predicate_disjoint (TN *pred1, bool false1, EBO_TN_INFO *info1,
			TN *pred2, bool false2, EBO_TN_INFO *info2)
{
  if (PQSCG_pqs_valid () && PQSCG_is_disjoint (pred1, pred2) && !false1 && !false2) {
      return TRUE;
  }
  return EBO_predicate_complements (pred1, false1, info1, pred2, false2, info2);
}
#endif



inline EBO_TN_INFO *
tn_info_def (BB *current_bb, OP *current_op, TN *local_tn,
             TN *predicate_tn, EBO_TN_INFO *predicate_info)
{
  EBO_TN_INFO *tninfo;
  EBO_TN_INFO *tninfo_prev;

 /* Define this TN and see if there were any previous ones. */
  tninfo_prev = get_tn_info (local_tn);
  tninfo = get_new_tninfo (current_bb, current_op, local_tn);
  tninfo->predicate_tninfo = predicate_info;
  tninfo->same = tninfo_prev;

  if ((tninfo_prev != NULL)  &&
      (tninfo_prev->in_bb == current_bb) &&
      ((predicate_tn == NULL) ||
#ifdef TARG_ST
       /* (cbr) there is a redefinition only if predicate have the same logical. */
       (EBO_predicate_dominates(predicate_tn,
				(predicate_tn != NULL)?
				OP_Pred_False(current_op,  OP_find_opnd_use(current_op, OU_predicate)) : false,
                                predicate_info,
                                (tninfo_prev->predicate_tninfo != NULL)?
				tninfo_prev->predicate_tninfo->local_tn:True_TN,
                                (tninfo_prev->predicate_tninfo != NULL && tninfo_prev->in_op)?
				OP_Pred_False(tninfo_prev->in_op,
					      OP_find_opnd_use(tninfo_prev->in_op, OU_predicate)) : false,
                                tninfo_prev->predicate_tninfo)))) {
#else
       (EBO_predicate_dominates(predicate_tn,
                                predicate_info,
                                (tninfo_prev->predicate_tninfo != NULL)?
                                 tninfo_prev->predicate_tninfo->local_tn:True_TN,
                                tninfo_prev->predicate_tninfo)))) {
#endif
   /* The new definition completely redefines the previous. */
#ifdef TARG_X8664
    // In rare situations, if tninfo and tninfo_prev correspond to the same
    // TN and are defined in the same OP, then don't consider the TN as
    // being redefined in order to prevent EBO from deleting the OP.  For
    // example:
    //   rdx TN100 = idiv ...	; TN100 not used
    // idiv requires "rax rdx" results in that order, but the WHIRL wants to
    // return the first result via rdx (the second return value register).
    // add_to_hash_table calls tn_info_def to add the TNs and their subclass in
    // this order:
    //   rax	; result 0's subclass
    //   rdx	; result 0
    //   rdx	; result 1's subclass
    //   TN100	; result 1
    // The second add of rdx would have set the first rdx's
    // tninfo->redefined_before_block_end to TRUE.  Since result 1 is never
    // used, EBO would think both results are not used, and the OP can be
    // deleted.  To avoid this, don't set redefined_before_block_end.
    // Bug 12744.
    if (tninfo_prev->in_op != current_op)
#endif
    tninfo_prev->redefined_before_block_end = TRUE;
  }

  return tninfo;
}

#ifdef TARG_ST
inline EBO_TN_INFO *
tn_info_use (BB *current_bb, OP *current_op, TN *local_tn,
             TN *predicate_tn, EBO_TN_INFO *predicate_info,
             mUINT8 associated_omega)
{
  EBO_TN_INFO *tninfo;
  EBO_TN_INFO *tninfo_prev;
  EBO_TN_INFO *tninfo_best_so_far;
  BOOL false_predicate_tn;

  tninfo = get_tn_info (local_tn);
  tninfo_prev = tninfo;
  tninfo_best_so_far = NULL;

  // [SC] To simplify the following code, treat an unpredicated op
  // as predicated by 1.
  if (! predicate_tn) {
    predicate_tn = True_TN;
    false_predicate_tn = false;
  }
  else {
    false_predicate_tn = OP_Pred_False(current_op, OP_find_opnd_use(current_op, OU_predicate));
  }

  // [SC] Scan backwards through the TNINFO entries created for this TN, finding
  // one that is valid whenever predicate_tn is true.
  // If predicate_tn is NULL, we search for one that is valid always (i.e. one that
  // is unpredicated, or that has a true predicate).
  // If predicate_tn is non-NULL, we search for one that is unpredicated, or that
  // has a predicate which dominates predicate_tn.
  // Note in this backwards scan, if we encounter a TNINFO for which tninfo->in_op
  // is NULL, then this is a dummy entry marking a place where we could not find
  // better information, for example a place where we could not find a TNINFO whose predicate
  // dominates that predicate_tn.  It is always safe to use these (if they dominate our
  // predicate_tn), and it is also always safe to ignore these, and continue searching for
  // a TNINFO that dominates predicate_tn.  We prefer to continue searching
  // to find a real TNINFO where that is possible.  However, if predicate_tn is True, there
  // is no point to do this, since no better TNINFO will be found.
  //
  // Interesting cases:
  // A.
  //     TN283 :- p  ? ld       Create real TNINFO for TN283
  //     TN283 :- !p ? ld       Create real TNINFO for TN283
  //     TN216 :- mov TN283     here MUST create a dummy TNINFO for TN283 since we have
  //                            no dominating defn for TN283
  //
  // B.
  //     TN195 :- mov 2         Create real TNINFO for TN195
  //     TN195 :- p ? ld A      Create real TNINFO for TN195
  //           :- mov TN195     here MUST create a dummy TNINFO for TN195, since neither
  //                            real TNINFO may reach this use.
  //     TN163 :- p2 ? ld A     where p DOM p2
  //   We want to transform this second load into  TN163 :- p2? mov TN195, but we
  //   can only do this if TN195 is available at the current point.  To determine this,
  //   we look up TN195 under predicate p2, and we want to see that it is the defn at the
  //   first load.
  //
  while (tninfo != NULL) {

    if (tninfo->omega == associated_omega) {
     /* First the omegas must be equal. */

      /* Compare the predicate on the previous definition (tninfo) with the
	 current predicate.  To see if the previous definition is a suitable
	 reaching definition of this use.
      */

      TN *tninfo_predicate_tn = ((tninfo->predicate_tninfo != NULL)
				 ? tninfo->predicate_tninfo->local_tn
				 : True_TN);

     /* [TTh] Retrieve Pred_False value */
      BOOL false_tninfo_predicate_tn = false;
	  if (tninfo->predicate_tninfo != NULL) {
		if(tninfo->in_op) 
			false_tninfo_predicate_tn = OP_Pred_False(tninfo->in_op,   OP_find_opnd_use(tninfo->in_op, OU_predicate));
		  else {
			false_tninfo_predicate_tn = tninfo->false_predicate_tn;
		  }
	  }
      if (EBO_predicate_complements(tninfo_predicate_tn,
				    false_tninfo_predicate_tn,
				    tninfo->predicate_tninfo,
				    predicate_tn,
				    false_predicate_tn,
				    predicate_info)) {
	/* The predicates are completely independent. Keep looking for a definition. */
      } else {
	BOOL tninfo_dom_current = EBO_predicate_dominates(tninfo_predicate_tn,
							  false_tninfo_predicate_tn,
							  tninfo->predicate_tninfo,
							  predicate_tn,
							  false_predicate_tn,
							  predicate_info);
	BOOL current_dom_tninfo = EBO_predicate_dominates(predicate_tn,
							  false_predicate_tn,
							  predicate_info,
							  tninfo_predicate_tn,
							  false_tninfo_predicate_tn,
							  tninfo->predicate_tninfo);
	if (tninfo_dom_current && current_dom_tninfo) {
	  // Predicates are equivalent, so use this tninfo.
	  // No point to continue searching if tninfo is dummy, since we
	  // will not find better.
	  break;
	} else if (current_dom_tninfo) {
	  // We have an instruction that only partially defines local_tn.
	  // No point in ignoring a dummy, since we will not find better.
	  tninfo = NULL;
	  break;
	} else if (tninfo_dom_current) {
	  // We have an instruction that defines local_tn, but if it is
	  // a dummy, we could find a better match if we continue searching.
	  if (tninfo->in_op != NULL) {
	    break;
	  } else {
	    // else its a dummy, so continue search.
	    // But remember this one, since we can use it if we do not find
	    // anything better.
	    if (tninfo_best_so_far == NULL) tninfo_best_so_far = tninfo;
	  }
	} else {
	  // We have a partial overlap.
	  // Force a dummy if its a real tninfo, otherwise continue searching.
	  if (tninfo->in_op != NULL) {
	    tninfo = NULL;
	    break;
	  }
	}
      }
    }
      
    /* Look at the next entry for a possible predicate match.       */
    tninfo = tninfo->same;
  }

  if (tninfo == NULL) tninfo = tninfo_best_so_far;

  if (tninfo == NULL) {
    tninfo = get_new_tninfo (current_bb, NULL, local_tn);
    tninfo->predicate_tninfo = predicate_info;
    tninfo->false_predicate_tn = false_predicate_tn;
    tninfo->same = tninfo_prev;
    tninfo->omega = associated_omega;
  }
  inc_ref_count(tninfo);
  return tninfo;
}
#else
inline EBO_TN_INFO *
tn_info_use (BB *current_bb, OP *current_op, TN *local_tn,
             TN *predicate_tn, EBO_TN_INFO *predicate_info,
             mUINT8 associated_omega)
{
  EBO_TN_INFO *tninfo;
  EBO_TN_INFO *tninfo_prev;

  tninfo = get_tn_info (local_tn);
  tninfo_prev = tninfo;

  while (tninfo != NULL) {

    if (tninfo->omega == associated_omega) {
     /* First the omegas must be equal. */

      if (predicate_tn != NULL) {
       /* Then, if the predicates have the right relationship, we have
          found the matching input to this use. */
        if (EBO_predicate_dominates((tninfo->predicate_tninfo != NULL)?
                                             tninfo->predicate_tninfo->local_tn:True_TN,
                                    tninfo->predicate_tninfo,
                                    predicate_tn,
                                    predicate_info)) {
          break;
        } else if (EBO_predicate_complements((tninfo->predicate_tninfo != NULL)?
                                                      tninfo->predicate_tninfo->local_tn:True_TN,
                                    tninfo->predicate_tninfo,
                                    predicate_tn,
                                    predicate_info)) {
         /* The predicates are completely independant. Keep looking for a definition. */
        } else if (tninfo->in_op != NULL) {
         /* A store into an unresolved predicate is a potential problem. */

         /* Stop searching and create a new tninfo entry for this TN.    */
          tninfo = NULL;
          break;
        }
      } else {
       /* But if there are no predicates, equal omegas indicate a match. */
        break;
      }
    }

   /* Look at the next entry for a possible predicate match.       */
    tninfo = tninfo->same;
  }

  if (tninfo == NULL) {
    tninfo = get_new_tninfo (current_bb, NULL, local_tn);
    tninfo->predicate_tninfo = predicate_info;
    tninfo->same = tninfo_prev;
    tninfo->omega = associated_omega;
  }
  inc_ref_count(tninfo);
  return tninfo;
}
#endif

inline EBO_OP_INFO *
get_new_opinfo (OP *op)
{
  EBO_OP_INFO *opinfo;
  size_t sizeof_info = EBO_OP_INFO_sizeof(op);

 /* Get space for a new entry if there are any discarded structs and
  * the current OP can use a "standard sized" struct. */
  if (EBO_free_opinfo && sizeof_info <= sizeof(EBO_OP_INFO)) {
    opinfo = EBO_free_opinfo;
    EBO_free_opinfo = opinfo->next;
    EBO_opinfo_entries_reused++;
  } else {
    opinfo = (EBO_OP_INFO *)MEM_POOL_Alloc(&EBO_pool, sizeof_info);
    EBO_num_opinfo_entries++;
  }

 /* Initialize fields in the entry. */
  const INT nopnds = OP_opnds(op);
  EBO_TN_INFO ** const tn_info = &opinfo->tn_info[0];
  opinfo->optimal_opnd = &tn_info[0];
  opinfo->actual_opnd = &tn_info[nopnds];
  opinfo->actual_rslt = &tn_info[nopnds + nopnds];
  opinfo->op_must_not_be_removed = FALSE;
  opinfo->op_must_not_be_moved = FALSE;
  opinfo->in_delay_slot = FALSE;

 /* Build a list that is ordered by when we create these entries.  */
 /* This list is used to "backup" after a block has been processed. */
  opinfo->prior = EBO_last_opinfo;
  if (EBO_first_opinfo == NULL) {
      EBO_first_opinfo = opinfo;
  } else {
      EBO_last_opinfo->next = opinfo;
  }
  EBO_last_opinfo = opinfo;
  opinfo->next = NULL;

  return opinfo;
} 


inline void backup_tninfo_list (EBO_TN_INFO *previous_last)
/* -----------------------------------------------------------------------
 *
 * Remove information about a block by backing
 * EBO_TN_INFO entires out of the map table
 * and putting them on the free chain.
 * -----------------------------------------------------------------------
 */
{
  EBO_TN_INFO *tninfo = EBO_last_tninfo;

  if (EBO_last_tninfo != previous_last) {
   /* Update the map table entry with any previous ptr. */
    while (tninfo != previous_last) {
      set_tn_info (tninfo->local_tn, tninfo->same);
      tninfo = tninfo->prior;
    }

    if (previous_last != NULL) {
     /* Link the inactive list onto the head of the free chain. */
      EBO_TN_INFO *new_free = previous_last->next;
      previous_last->next = NULL;
      EBO_last_tninfo->next = EBO_free_tninfo;
      EBO_free_tninfo = new_free;

     /* Adjust the active list. */
      EBO_last_tninfo = previous_last;
      EBO_tninfo_number = previous_last->sequence_num;
    } else {
      EBO_last_tninfo->next = EBO_free_tninfo;
      EBO_free_tninfo = EBO_first_tninfo;
      EBO_first_tninfo = NULL;
      EBO_last_tninfo = NULL;
      EBO_tninfo_number = 0;
    }
  }
}


inline void backup_opinfo_list (EBO_OP_INFO *previous_last)
/* -----------------------------------------------------------------------
 *
 * Remove information about a block by backing
 * EBO_OP_INFO entires out of the hash table
 * and putting them on the free chain.
 * -----------------------------------------------------------------------
 */
{
  EBO_OP_INFO *opinfo = EBO_last_opinfo;

  if (EBO_last_opinfo != previous_last) {
   /* Update the hash table entry with any previous ptr. */
    while (opinfo != previous_last) {
#ifdef KEY      
#if 0
      if (opinfo->in_op &&
         OP_store(opinfo->in_op))    
        delete_useless_store_op(opinfo);
#endif
#endif
      EBO_opinfo_table[opinfo->hash_index] = opinfo->same;
      opinfo = opinfo->prior;
    }

    if (previous_last != NULL) {
     /* Link the inactive list onto the head of the free chain. */
      EBO_OP_INFO *new_free = previous_last->next;
      previous_last->next = NULL;
      EBO_last_opinfo->next = EBO_free_opinfo;
      EBO_free_opinfo = new_free;

     /* Adjust the active list. */
      EBO_last_opinfo = previous_last;
    } else {
      EBO_last_opinfo->next = EBO_free_opinfo;
      EBO_free_opinfo = EBO_first_opinfo;
      EBO_first_opinfo = NULL;
      EBO_last_opinfo = NULL;
    }
  }
}



inline void remove_uses (INT num_opnds,
                         EBO_TN_INFO **actual_tninfo)
/* -----------------------------------------------------------------------
 * Decrement the use counts for the actual operands of an expression.
 * -----------------------------------------------------------------------
 */
{
  INT opndnum;
  EBO_TN_INFO *tninfo;

 /* Process all the operand TN_INFOs. */
  for (opndnum = 0; opndnum < num_opnds; opndnum++) {
    tninfo = actual_tninfo[opndnum];
    if (tninfo != NULL) {
      dec_ref_count(tninfo);
    }
  }

}



inline void remove_op (EBO_OP_INFO *opinfo)
/* -----------------------------------------------------------------------
 * Decrement the use counts for the actual operands of an EBO_OP_INFO entry.
 * -----------------------------------------------------------------------
 */
{
  OP *op = opinfo->in_op;
  INT num_opnds = OP_opnds(op);
  INT opndnum;
  EBO_TN_INFO *tninfo;

 /* Process all the operand TN_INFOs. */
  for (opndnum = 0; opndnum < num_opnds; opndnum++) {
    tninfo = opinfo->actual_opnd[opndnum];
    if (tninfo != NULL) {
      dec_ref_count(tninfo);
    }
  }

}

#endif /* EBO_INFO_INCLUDED */
