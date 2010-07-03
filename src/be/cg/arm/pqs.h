/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


#ifndef PQS_H_INCLUDED
#define PQS_H_INCLUDED

#ifndef PQSTEST
#define PQS_USE_MEMPOOLS
#endif

#include <list>
#include <vector>
#include <map>

// Tracing variable
extern BOOL PQS_Tracing;

#ifdef PQS_USE_MEMPOOLS
// Mempool variable
extern MEM_POOL PQS_mem_pool;
extern void PQS_Init_Memory(void);
#endif
 
typedef UINT32 PQS_SYMBOL;
  
#define PQS_HASH_SIZE 16

class PQS_NODE;
class PQS_RELATION;
class PQS_SYMBOL_SET;
class PQS_TN_INFO;

typedef std::list<PQS_NODE *, mempool_allocator<PQS_NODE *> > PQS_NODE_LIST;
typedef PQS_NODE_LIST::iterator PQS_NODE_LIST_ITER;
typedef std::list<PQS_RELATION *,mempool_allocator<PQS_RELATION *> > PQS_RELATION_LIST;
typedef std::list<const PQS_RELATION *,mempool_allocator<const PQS_RELATION *> > CONST_PQS_RELATION_LIST;
typedef PQS_RELATION_LIST::iterator PQS_RELATION_LIST_ITER;
typedef PQS_RELATION_LIST::const_iterator PQS_RELATION_LIST_CONST_ITER;
typedef std::vector<PQS_NODE *,mempool_allocator<PQS_NODE *> > PQS_NODE_MAP;
typedef PQS_NODE_MAP::iterator PQS_NODE_MAP_ITER;
typedef std::pair<const TN *,PQS_TN_INFO *> tn_map_pair;
typedef std::map<TN *,PQS_TN_INFO *,std::less<TN *>,mempool_allocator<tn_map_pair> > PQS_TN_MAP;
typedef PQS_TN_MAP::iterator PQS_TN_MAP_ITER;

class PQS_MANAGER {
 private:
  PQS_SYMBOL next_pqs_symbol;
  UINT32 next_pqs_relation;
  UINT32 current_marker; // Used for graph walks.
  PQS_NODE *pqs_unknown_node;
  PQS_NODE *pqs_true_node;
  PQS_NODE *pqs_false_node;
  TN *zero_tn;
  PQS_NODE_LIST Hash_Table [PQS_HASH_SIZE];
  PQS_RELATION_LIST all_relations;
  PQS_TN_MAP tn_map;         // map from TN to PQS_NODE.
  PQS_NODE_MAP node_map;  // map from PQS_SYMBOL to PQS_NODE.
  REGISTER_SET modified_registers [ISA_REGISTER_CLASS_MAX_LIMIT+1];
  BOOL trace_entry, trace_hash;

  void Add_Node (PQS_NODE *p);
  PQS_TN_INFO *tn_map_get (TN *tn);
  PQS_RELATION *Emit_Partition_Relation (PQS_NODE *parent,
					 const PQS_SYMBOL_SET & children,
					 BOOL synthetic = FALSE);
  PQS_RELATION *Emit_Partition_Relation (PQS_NODE *parent,
					 PQS_NODE *s1, PQS_NODE *s2,
					 BOOL synthetic = FALSE);
  PQS_SYMBOL_SET Expr_Universe ();
  void Mark_Reachable (PQS_NODE *p);
  void Complete_Partition_Graph_For_Node (PQS_NODE *p);
  void Complete_Partition_Graph ();
  void Build_Levels (PQS_NODE *p, INT level);
  void Build_Disjoint ();
  BOOL Is_Disjoint (PQS_NODE *node1, PQS_NODE *node2);
  BOOL Is_Disjoint (PQS_NODE *node1, const PQS_SYMBOL_SET& expr);
  BOOL Is_Disjoint (const PQS_SYMBOL_SET& expr1, const PQS_SYMBOL_SET& expr2);
  void Sum_Reduce (PQS_NODE *p, PQS_SYMBOL_SET& e);
  BOOL Is_Subset (PQS_NODE *p, PQS_NODE *q);
  BOOL Is_Subset (PQS_NODE *p, const PQS_SYMBOL_SET& expr);
  BOOL Is_Subset (const PQS_SYMBOL_SET& expr1, const PQS_SYMBOL_SET& expr2);
  PQS_NODE *Find_Lca (const PQS_SYMBOL_SET& expr);
  PQS_SYMBOL_SET Rel_Cmpl (PQS_NODE *node1, PQS_NODE *node2);
  PQS_NODE *PQS_hash_lookup (PQS_NODE *qual_sym,
			     BOOL negate_result,
			     VARIANT v,
			     PQS_NODE *sym1, PQS_NODE *sym2);
  PQS_TN_INFO *PQS_TN_Map_To_Node (TN *, PQS_NODE *);
  PQS_NODE *PQS_Node (PQS_SYMBOL sym) { return node_map[sym]; }
  PQS_NODE *PQS_Node (TN *tn);
  PQS_NODE *PQS_Create_Symbol (PQS_NODE *qual_sym, VARIANT v,
			       BOOL negate_result, PQS_NODE *sym1,
			       PQS_NODE *sym2);
  PQS_NODE *PQS_Create_Symbol ();
  PQS_NODE *Symbol_Difference (const PQS_NODE *p1, const PQS_NODE *p2);
  PQS_SYMBOL PQS_Symbol (TN *tn);
  PQS_SYMBOL_SET PQS_Symbol_Set (PQS_TN_SET &tns);
  BOOL Is_Valid (PQS_TN_SET &tns);
  PQS_NODE *Add_Predicate_Cmp_Def (TN *pred_tn, PQS_NODE *qual_sym, 
						VARIANT v, PQS_NODE *sym1,
						PQS_NODE *sym2);
  const UINT32 Current_Marker () { return current_marker; }
  void Update_Marker () { current_marker++; }
 public:
  void Note_Use (BB *bb, TN *tn);
  void Note_Def (BB *bb, TN *tn);
  void Note_Modified_Registers (const REGISTER_SET *modified_registers);
  void Note_Synthesized_TN (TN *tn);
  PQS_NODE *Add_Predicate_Cmp_Def (TN *pred_tn, TN *qual_tn,
						VARIANT v, TN *opd1, TN *opd2);
  void Add_Copy (TN *result_tn, TN *qual_tn, TN *source_tn);
  void Set_True (TN *result_tn, TN *qual_tn);
  void Set_False (TN *result_tn, TN *qual_tn);
  BOOL PQS_is_disjoint(TN * tn1, TN * tn2);
  BOOL PQS_is_disjoint(PQS_TN_SET &tns1, PQS_TN_SET &tns2);
  BOOL PQS_is_subset_of (TN * tn1, TN * tn2);
  BOOL PQS_is_subset_of(TN * tn1, PQS_TN_SET &tns2);
  BOOL PQS_is_subset_of (PQS_TN_SET &tns1, PQS_TN_SET &tns2);
  UINT32 PQS_Rank (PQS_NODE *p);
  void Calculate_Valid_TNs ();
  void Complete_Graph ();
  PQS_MANAGER ();  
};

#define FOR_ALL_PQS_NODES(IT,P) \
  for ((IT) = node_map.begin (); \
       (IT) != node_map.end (); \
       ++(IT)) { \
    (P) = *(IT);

#define END_FOR_ALL_PQS_NODES \
  }

#define FOR_ALL_RELATIONS(IT,R) \
  for ((IT) = all_relations.begin (); \
       (IT) != all_relations.end (); \
       ++(IT)) { \
    (R) = *(IT); 

#define END_FOR_ALL_RELATIONS \
  }

#endif /* PQS_H_INCLUDED */
