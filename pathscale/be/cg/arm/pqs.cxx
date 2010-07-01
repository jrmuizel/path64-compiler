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

/*================================================================

This file implements the predicate query system.
It is based on Johnson and Schlansker, "Analysis Techniques for
Predicated Code", 1996.

Key assumptions and requirements:

1) the PQS data structures are built by a scan over the BBs.
   They should be maintained as needed.

2) Any predicate TN which is defined more than once is considered
   to have unknown value.  This could be relaxed if necessary (see the
   original SGI PQS restrictions, which are more permissive).


================================================================*/

#include <list>
#include "defs.h"
#include "tn.h"
#include "pqs_defs.h"
#include "pqs_cg.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "tracing.h"

#include "pqs.h"

#ifdef PQS_USE_MEMPOOLS
// Mempool variable
MEM_POOL PQS_mem_pool;
static BOOL pqs_mempool_init=FALSE;

// Memory initialization
void PQS_Init_Memory(void)
{
  if (pqs_mempool_init) return;
  pqs_mempool_init = TRUE;
  MEM_POOL_Initialize(&PQS_mem_pool,"PQS_mem_pool",FALSE);
  MEM_POOL_Push(&PQS_mem_pool);
}
#endif

// The partition graph is made up of three types:
// PQS_NODE, PQS_RELATION, PQS_SYMBOL_SET.
// 
// PQS_NODE is a node in the partition graph.
//  Each PQS_NODE has an unique label of type PQS_SYMBOL.
// PQS_RELATION represents a partition relation,
//  it has a single lhs PQS_SYMBOL, and a set of rhs PQS_SYMBOLs
//  forming the components of the partition.  This represents
//  the arcs of the partition graph, i.e. there is an arc from
//  the lhs to each element of the rhs.
// PQS_SYMBOL_SET represents a set of PQS_SYMBOLs, in the rhs of
//  a PQS_RELATION.

// A PQS value is represented as a PQS_SYMBOL_SET, a set of
// PQS_SYMBOLs.  The value the union of these symbols,
// i.e. the 1-disjunctive normal form of Johnson and Schlanker.

// Here PQS_SYMBOL_SET is just syntatic sugar on top of the open64
// bitset type.
// Note that the open64 bitset functions lack constness on
// arguments that are unmodified, so we have to do some casts
// here to accomodate that.

#define DEFAULT_PQS_SYMBOL_SET_SIZE 64

class PQS_SYMBOL_SET {
private:
  BS *bs;
public:
  void Union (const PQS_SYMBOL_SET& e2) {
    bs = BS_UnionD (bs, e2.bs, &PQS_mem_pool);
  }
  void Insert (PQS_SYMBOL sym) {
    bs = BS_Union1D (bs, (BS_ELT)sym, &PQS_mem_pool);
  }
  void Insert (const PQS_NODE *p);
  BOOL MemberP (PQS_SYMBOL sym) const {
    return BS_MemberP (bs, (BS_ELT)sym);
  }
  BOOL MemberP (const PQS_NODE *p) const;
  BOOL EmptyP () const {
    return BS_EmptyP (bs);
  }
  BOOL ContainsP (const PQS_SYMBOL_SET& e2) const {
    return BS_ContainsP (bs, e2.bs);
  }
  BOOL IntersectsP (const PQS_SYMBOL_SET& e2) const {
    return BS_IntersectsP (bs, e2.bs);
  }
  void Difference (const PQS_SYMBOL_SET& e2) {
    bs = BS_DifferenceD (bs, e2.bs);
  }
  void Difference (PQS_SYMBOL s) {
    bs = BS_Difference1D (bs, s);
  }
  friend PQS_SYMBOL_SET Difference (const PQS_SYMBOL_SET &e1, PQS_SYMBOL s) {
    return PQS_SYMBOL_SET (BS_Difference1 (e1.bs, s, &PQS_mem_pool));
  }
  void Intersection (const PQS_SYMBOL_SET& e2) {
    bs = BS_IntersectionD (bs, e2.bs);
  }
  friend PQS_SYMBOL_SET Intersection (const PQS_SYMBOL_SET &e1, const PQS_SYMBOL_SET &e2);
  INT Count () const { return BS_Size (bs); }
  static PQS_SYMBOL_SET Universe (PQS_SYMBOL size) {
    BS *bs = BS_Universe (size, &PQS_mem_pool);
    return PQS_SYMBOL_SET (bs);
  }
  static PQS_SYMBOL_SET False_Expr () {
    return PQS_SYMBOL_SET ();
  }
  BS_ELT Choose () const {
    return BS_Choose (bs);
  }
  BS_ELT Choose_Next (BS_ELT elt) const {
    return BS_Choose_Next (bs, elt);
  } 
  void Print (FILE *f) const;
  PQS_SYMBOL_SET (BS *b) : bs (b) {}
  PQS_SYMBOL_SET () {
    bs = BS_Create_Empty (DEFAULT_PQS_SYMBOL_SET_SIZE, &PQS_mem_pool);
  }
  PQS_SYMBOL_SET (PQS_SYMBOL s) {
    BS_ELT size = DEFAULT_PQS_SYMBOL_SET_SIZE;
    if (size < s) size = s;
    bs = BS_Create_Empty (size, &PQS_mem_pool);
    bs = BS_Union1D (bs, (BS_ELT)s, &PQS_mem_pool);
  }
  PQS_SYMBOL_SET (PQS_SYMBOL s1, PQS_SYMBOL s2) {
    BS_ELT size = DEFAULT_PQS_SYMBOL_SET_SIZE;
    if (size < s1) size = s1;
    if (size < s2) size = s2;
    bs = BS_Create_Empty (size, &PQS_mem_pool);
    bs = BS_Union1D (bs, (BS_ELT)s1, &PQS_mem_pool);
    bs = BS_Union1D (bs, (BS_ELT)s2, &PQS_mem_pool);
  }
  PQS_SYMBOL_SET (PQS_TN_SET &tns);
  PQS_SYMBOL_SET (const PQS_SYMBOL_SET &e) {
    bs = BS_Copy (e.bs, &PQS_mem_pool);
  }
  ~PQS_SYMBOL_SET () {
    MEM_POOL_FREE (&PQS_mem_pool, (void *)bs);
  }
};

#define FOR_ALL_PQS_SYMBOL_SET_MEMBER(E,N) \
   { \
     BS_ELT elt; \
     for ((elt = (E).Choose (), \
	   (N) = (elt != BS_CHOOSE_FAILURE) ? PQS_Node (PQS_SYMBOL(elt)) : NULL); \
	  (N) != NULL; \
	  (elt = (E).Choose_Next (elt), \
	   (N) = (elt != BS_CHOOSE_FAILURE) ? PQS_Node (PQS_SYMBOL(elt)) : NULL))

#define END_FOR_ALL_PQS_SYMBOL_SET_MEMBER \
    }

#define FOR_ALL_PQS_SYMBOL_SET_SYMBOL_MEMBER(E,S) \
   { \
     BS_ELT elt; \
     for ((elt = (E).Choose (),					       \
	   (S) = PQS_SYMBOL(elt));				       \
	  (S) != PQS_SYMBOL(BS_CHOOSE_FAILURE);			       \
	  (elt = (E).Choose_Next (elt),				       \
	   (S) = PQS_SYMBOL(elt)))

#define END_FOR_ALL_PQS_SYMBOL_SET_SYMBOL_MEMBER \
    }


PQS_SYMBOL_SET
Intersection (const PQS_SYMBOL_SET &e1, const PQS_SYMBOL_SET &e2)
{
  return PQS_SYMBOL_SET (BS_Intersection (e1.bs, e2.bs, &PQS_mem_pool));
}

class PQS_RELATION {
private:
  INT label;
  BOOL synthetic; // TRUE if this relation synthesized to
  // complete the partition graph.
  PQS_NODE *lhs;  // lhs
  PQS_SYMBOL_SET components; // rhs components
public:
  INT Label () const { return label; }
  void Set_Synthetic (BOOL v) { synthetic = v; }
  BOOL Synthetic () const { return synthetic; }
  PQS_NODE *Lhs_Node () const { return lhs; }
  const PQS_SYMBOL_SET & Components () const { return components; }
  void Print (FILE *f) const;
  PQS_RELATION (INT l, BOOL synth, PQS_NODE *s, const PQS_SYMBOL_SET &cmps) :
    label (l), synthetic (synth), lhs (s), components (cmps) {}

 };

// A node in the partition graph
class PQS_NODE {
private:
  PQS_SYMBOL sym; // symbol for this node.
  PQS_NODE *next; // Next node in hash chain.
  PQS_NODE *negated_node; // the node that is the negation of this node,
  // since we always create the negated version of predicate expressions,
  // this is guaranteed to exist for all nodes.

  // This node represents the predicate expression
  //  [negate] (sym1 v sym2) . qual_sym
  const PQS_NODE *qual_sym;
  VARIANT v;
  BOOL negate_result;
  const PQS_NODE *sym1, *sym2; // Operands of the compare

  // If this node represents one or more program variables,
  // they are in this set.
  PQS_TN_SET tns;

  PQS_RELATION_LIST lhs_relations; // Relations for which this node is lhs.
  PQS_RELATION_LIST rhs_relations; // Relations for which this node
  // appears on rhs.
  INT level; // Level of this node in the partition graph.
  PQS_SYMBOL_SET ancestors;   // Set of all ancestors
  PQS_SYMBOL_SET descendants; // Set of all descendants
  // of this node in the partition graph.
  PQS_SYMBOL_SET disjoint; // Set of all symbols that are known to be disjoint
  // from this symbol.
  UINT32 marker; // Marker on this node for graph-walks.
  
public:
  void Set_Hash_Chain_Next (PQS_NODE *p) { next = p; }
  PQS_NODE *Hash_Chain_Next () const { return next; }
  PQS_SYMBOL Sym () const { return sym; }
  PQS_NODE *Negated_Node () const { return negated_node; }
  void Set_Negated_Node (PQS_NODE *node) { negated_node = node; }
  const PQS_NODE *Qual_Sym () const { return qual_sym; }
  const PQS_TN_SET &Tns () const { return tns; }
  void Add_Tn (TN *t) { tns.Insert (t); }
  BOOL Match (const PQS_NODE *q, VARIANT var, const PQS_NODE *s1,
	      const PQS_NODE *s2) const {
    return q == qual_sym && var == v && s1 == sym1 && s2 == sym2;
  }
  INT Level () const { return level; }
  void Set_Level (INT l) { level = l; }
  const PQS_SYMBOL_SET & Descendants () const { return descendants; }
  void Set_Descendants (const PQS_SYMBOL_SET & d) { descendants = d; }
  void Add_Descendants (const PQS_SYMBOL_SET & d) { descendants.Union (d); }
  const PQS_SYMBOL_SET & Ancestors () const { return ancestors; }
  void Set_Ancestors (const PQS_SYMBOL_SET & d) { ancestors = d; }
  void Add_Ancestors (const PQS_SYMBOL_SET &d) { ancestors.Union (d); }
  void Add_Ancestor (PQS_SYMBOL s) { ancestors.Insert (s); }
  void Add_Ancestor (const PQS_NODE *p) { Add_Ancestor (p->Sym ()); }
  const PQS_SYMBOL_SET & Disjoint () const { return disjoint; }
  void Set_Disjoint (const PQS_SYMBOL_SET & d) { disjoint = d; }
  void Add_Disjoint (const PQS_SYMBOL_SET & d) { disjoint.Union (d); }
  BOOL Symbol_Difference (const PQS_NODE *p, PQS_SYMBOL &result) const;
  UINT32 Marker () const { return marker; }
  void Set_Marker (UINT32 v) { marker = v; }
  const PQS_RELATION_LIST & Lhs_Relations () const { return lhs_relations; }
  void Add_Lhs_Relation (PQS_RELATION *relation) {
    lhs_relations.push_back (relation);
  }
  const PQS_RELATION_LIST & Rhs_Relations () const { return rhs_relations; }
  void Add_Rhs_Relation (PQS_RELATION *relation) {
    rhs_relations.push_back (relation);
  }
  void Print (FILE *f) const;
  void Print_Full (FILE *f) const;
  PQS_NODE (PQS_SYMBOL sy, const PQS_NODE *qual, VARIANT var, BOOL neg,
	    const PQS_NODE *s1, const PQS_NODE *s2) :
    qual_sym (qual), v (var), negate_result (neg), sym1 (s1), sym2 (s2),
    level (0), next (NULL), negated_node (NULL), sym (sy), marker (0) {
    lhs_relations.clear ();
    rhs_relations.clear ();
    tns.Clear ();
  }
};

#define FOR_ALL_RELATION_LIST(L,IT,R) \
  { \
    PQS_RELATION_LIST_CONST_ITER __end = (L).end (); \
    for ((IT) = (L).begin (); \
         (IT) != __end; \
       ++(IT)) { \
      (R) = *(IT);
    
#define END_FOR_ALL_RELATION_LIST \
    } \
  }

void
PQS_NODE::Print (FILE *f) const
{
  fprintf (f, "PQS S%d: ", Sym ());
  PQS_TN_SET_TYPE::iterator it;
  const PQS_TN_SET &tns = Tns ();
  BOOL empty = TRUE;
  for (it = tns.begin (); it != tns.end (); it++) {
    fPrint_TN (f, "%s ", *it);
    empty = FALSE;
  }
  if (empty) fprintf (f, "null ");
  if (v != V_NONE) {
    fprintf (f, "qual S%d %sS%d %s S%d", 
	     qual_sym->Sym (),
	     (negate_result ? "negated, " : ""),
	     sym1->Sym (), CMP_Variant_Name (v), sym2->Sym ());
  }
}

void
PQS_NODE::Print_Full (FILE *f) const
{
  PQS_RELATION_LIST_CONST_ITER it;
  PQS_RELATION *relation;

  Print (f);
  fprintf (f, " level=%d", Level ());
  if (! lhs_relations.empty ()) {
    fprintf (f, " lhs_relations = ");
    FOR_ALL_RELATION_LIST (lhs_relations, it, relation) {
      fprintf (f, " %d", relation->Label ());
    } END_FOR_ALL_RELATION_LIST
  }
  if (! rhs_relations.empty ()) {
    fprintf (f, " rhs_relations = ");
    FOR_ALL_RELATION_LIST (rhs_relations, it, relation) {
      fprintf (f, " %d", relation->Label ());
    } END_FOR_ALL_RELATION_LIST
  }
  if (! ancestors.EmptyP ()) {
    fprintf (f, " ancestors = ");
    ancestors.Print (f);
  }
  if (! descendants.EmptyP ()) {
    fprintf (f, " descendants = ");
    descendants.Print (f);
  }
  if (! disjoint.EmptyP ()) {
    fprintf (f, " disjoint = ");
    disjoint.Print (f);
  }
  fprintf (f, "\n");
}

static void
debug_node (const PQS_NODE *p)
{
  p->Print_Full (stdout);
}

class PQS_TN_INFO {
private:
  mBOOL use_before_def;
  mBOOL use_seen;
  mBOOL def_seen;
  mBOOL valid;
  BB *def_bb;
  const TN *const tn;
  // Node in the partition graph for the symbol that represents
  // this TN.
  PQS_NODE *p;
public:
  PQS_TN_INFO (const TN *t) : tn (t), use_before_def (FALSE), use_seen (FALSE),
			      def_seen (FALSE), p(NULL), def_bb(NULL) {
  }
  void Note_Use (BB *bb) {
    use_seen = TRUE;
    if (! def_seen) {
      use_before_def = TRUE;
    }
  }
  void Note_Def (BB *bb) {
    if (! def_seen) {
      def_bb = bb;
      def_seen = TRUE;
    } else {
      def_bb = NULL;
    }
  }
  void Set_PQS_Node (PQS_NODE *n) { p = n; }
  PQS_NODE *PQS_Node () const { return p; }
  const TN *Tn () const { return tn; }
  PQS_SYMBOL PQS_Symbol () const { return p->Sym(); }
  BOOL Use_Before_Def () const { return def_seen && use_before_def; }
  BOOL Defs_In_Multiple_BBs () const { return def_seen && def_bb == NULL; }
  void Set_Valid (BOOL v) { valid = v; }
  BOOL Valid () const { return valid; }
};

void
PQS_RELATION::Print (FILE *f) const
{
  fprintf (f, "PQS Relation %d: S%d <- union ", label, lhs->Sym ());
  components.Print (f);
  if (synthetic) fprintf (f, " synthetic");
  fprintf (f, "\n");
}

static void
debug_relation (PQS_RELATION *r)
{
  r->Print (stderr);
}

PQS_TN_INFO *
PQS_MANAGER::tn_map_get (TN *tn)
{
  PQS_TN_MAP_ITER it = tn_map.find (tn);
  if (it != tn_map.end ()) {
    return it->second;
  } else {
    return NULL;
  }
}

PQS_TN_INFO *
PQS_MANAGER::PQS_TN_Map_To_Node (TN *tn, PQS_NODE *p)
{
  PQS_TN_INFO *tn_info = tn_map_get (tn);
  FmtAssert (tn_info, ("Unexpected null tn_info"));
  if (tn_info->Valid ()) {
    tn_info->Set_PQS_Node (p);
    p->Add_Tn (tn);
    if (trace_entry) {
      fprintf (TFile, "PQS ");
      fprintf (TFile, "S%d is ", p->Sym ());
      Print_TN (tn, FALSE);
      fprintf (TFile, "\n");
    }
  }
  return tn_info;
}

PQS_SYMBOL_SET
PQS_MANAGER::PQS_Symbol_Set (PQS_TN_SET &tns)
{
  PQS_SYMBOL_SET result;
  PQS_TN_SET_TYPE &tnset = tns._set;
  PQS_TN_SET_TYPE::iterator it;
  for (it = tnset.begin(); it != tnset.end(); it++) {
    TN *tn = *it;
    Sum_Reduce (PQS_Node (tn), result);
  }
  return result;
}

BOOL
PQS_MANAGER::Is_Valid (PQS_TN_SET &tns)
{
  PQS_TN_SET_TYPE &tnset = tns._set;
  PQS_TN_SET_TYPE::iterator it;
  for (it = tnset.begin(); it != tnset.end(); it++) {
    TN *tn = *it;
    if (!PQS_Node (tn)) {
      return FALSE;
    }
  }
  return TRUE;
}

void
PQS_MANAGER::Calculate_Valid_TNs ()
{
  PQS_TN_MAP_ITER it;
  for (it = tn_map.begin (); it != tn_map.end (); ++it) {
    PQS_TN_INFO *info = it->second;
    const TN *tn = info->Tn ();
    BOOL valid = FALSE;
    if (TN_is_constant (tn)) {
      // Constants are always valid.
      valid = TRUE;
    } else if (TN_is_true (tn)
	       || TN_is_zero (tn)) {
      // True and zero (false) are always valid.
      valid = TRUE;
    } else if (TN_is_dedicated (tn)) {
      // Treat a dedicated TN as valid provided the registers are not
      // modified anywhere in the code (i.e. they are incoming parameters).
      valid = ! REGISTER_SET_IntersectsP (TN_registers (tn),
					  modified_registers [TN_register_class (tn)]);
    } else if (info->Use_Before_Def ()) {
      // Not valid if there is a use before a def.
      valid = FALSE;
    } else if (info->Defs_In_Multiple_BBs ()) {
      // Not valid if there are defs in multiple BBs.
      valid = FALSE;
    } else {
      valid = TRUE;
    }
    info->Set_Valid (valid);
  }
}

void
PQS_SYMBOL_SET::Insert (const PQS_NODE *p) {
  Insert (p->Sym ());
}

BOOL
PQS_SYMBOL_SET::MemberP (const PQS_NODE *p) const {
  return BS_MemberP (bs, (BS_ELT)(p->Sym ()));
}

void
PQS_SYMBOL_SET::Print (FILE *f) const {
  BS_Print (bs, f);
}

PQS_RELATION *
PQS_MANAGER::Emit_Partition_Relation (PQS_NODE *parent,
				      const PQS_SYMBOL_SET & components,
				      BOOL synthetic /* = FALSE */)
{
  PQS_RELATION *relation = CXX_NEW (PQS_RELATION (next_pqs_relation++,
						  synthetic,
						  parent, components),
				    &PQS_mem_pool);
  all_relations.push_back (relation);
  parent->Add_Lhs_Relation (relation);
  PQS_NODE *p;
  FOR_ALL_PQS_SYMBOL_SET_MEMBER (components, p) {
    p->Add_Rhs_Relation (relation);
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  if (trace_entry) {
    relation->Print (TFile);
  }

  // Update decendants and ancestors information.
  PQS_SYMBOL_SET all_ancestors = parent->Ancestors ();
  all_ancestors.Insert (parent);
  PQS_SYMBOL_SET all_descendants = components;
  FOR_ALL_PQS_SYMBOL_SET_MEMBER(components, p) {
    all_descendants.Union (p->Descendants ());
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  FOR_ALL_PQS_SYMBOL_SET_MEMBER(all_ancestors, p) {
    p->Add_Descendants (all_descendants);
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  FOR_ALL_PQS_SYMBOL_SET_MEMBER(all_descendants, p) {
    p->Add_Ancestors (all_ancestors);
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
      
  return relation;
}

PQS_RELATION *
PQS_MANAGER::Emit_Partition_Relation (PQS_NODE *parent,
				      PQS_NODE *child1, PQS_NODE *child2,
				      BOOL synthetic /* = FALSE */)
{
  PQS_SYMBOL_SET e (child1->Sym (), child2->Sym ());
  return Emit_Partition_Relation (parent, e, synthetic);
}

PQS_SYMBOL_SET 
PQS_MANAGER::Expr_Universe () {
  PQS_SYMBOL_SET u =  PQS_SYMBOL_SET::Universe (next_pqs_symbol);

  u.Difference (pqs_unknown_node->Sym ());
  u.Difference (pqs_false_node->Sym ());
  return u;
}

void
PQS_MANAGER::Mark_Reachable (PQS_NODE *p)
  // Mark sym and all its descendants in the partition graph as reachable.
{
  if (p->Marker () != Current_Marker ()) {
    p->Set_Marker (Current_Marker ());
    PQS_RELATION_LIST_CONST_ITER it;
    PQS_RELATION *relation;
    FOR_ALL_RELATION_LIST (p->Lhs_Relations (), it, relation) {
      PQS_NODE *child;
      FOR_ALL_PQS_SYMBOL_SET_MEMBER (relation->Components(), child) {
	Mark_Reachable (child);
      } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
    } END_FOR_ALL_RELATION_LIST
  }
}
    

void
PQS_MANAGER::Complete_Partition_Graph_For_Node (PQS_NODE *p)
{
  // Node p is not reachable from the root of the partition graph.
  // Add a synthetic relation to make it reachable.
  PQS_RELATION_LIST_CONST_ITER it;
  PQS_RELATION *relation;
  BOOL true_false_relation_emitted = FALSE;
  if (trace_entry) {
    fprintf (TFile, "Enter Complete_Partition_Graph_For_Node (S%d)\n", p->Sym ());
  }
  // Ensure all children are reachable.
  FOR_ALL_RELATION_LIST (p->Lhs_Relations (), it, relation) {
    PQS_NODE *child;
    FOR_ALL_PQS_SYMBOL_SET_MEMBER (relation->Components (), child) {
      if (child->Marker () != Current_Marker ()) {
	Complete_Partition_Graph_For_Node (child);
      }
    } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
    // Now complete for this relation.
    PQS_NODE *lca = Find_Lca (relation->Components ());
    if (lca == pqs_true_node) {
      // We do not have very good information for this node, just say it
      // is in a partition of true.
      if (! true_false_relation_emitted) {
	Emit_Partition_Relation (pqs_true_node, p, p->Negated_Node (), TRUE);
	true_false_relation_emitted = TRUE;
      }
    } else {
      PQS_SYMBOL_SET rel_cmpl = Rel_Cmpl (p, lca);
      rel_cmpl.Insert (p);
      PQS_RELATION *r = Emit_Partition_Relation (lca, rel_cmpl);
      r->Set_Synthetic (TRUE);
    }
  } END_FOR_ALL_RELATION_LIST
  // Now this node is reachable.
  p->Set_Marker (Current_Marker ());
  if (trace_entry) {
    fprintf (TFile, "Exit Complete_Partition_Graph_For_Node (S%d)\n", p->Sym ());
  }
}

void
PQS_MANAGER::Build_Levels (PQS_NODE *p, INT level)
{
  if (p->Level () == -1
      || p->Level () > level) {
    p->Set_Level (level);
  }
  const PQS_RELATION_LIST &relations = p->Lhs_Relations ();
  PQS_RELATION_LIST_CONST_ITER it;
  PQS_RELATION *relation;
  for (it = relations.begin ();
       it != relations.end ();
       ++it) {
    relation = *it;
    PQS_NODE *child;
    FOR_ALL_PQS_SYMBOL_SET_MEMBER (relation->Components(), child) {
      Build_Levels (child, level + 1);
    } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  }
}

void
PQS_MANAGER::Complete_Partition_Graph ()
{
  Build_Levels (pqs_true_node, 0);
  Update_Marker ();
  // Set visited for all nodes reachable from the TRUE symbol.
  Mark_Reachable (pqs_true_node);
  // Complete the graph for all unreachable (non-leaf) nodes.
  // We just ignore unreachable leafs.
  PQS_NODE_MAP_ITER it;
  PQS_NODE *p;
  FOR_ALL_PQS_NODES (it, p) {
    if (p->Marker () != Current_Marker ()
	&& ! p->Lhs_Relations ().empty ()) {
      Complete_Partition_Graph_For_Node (p);
    }
  } END_FOR_ALL_PQS_NODES
  // For tidiness, rebuild information for the completed graph.
  Build_Levels (pqs_true_node, 0);
  if (trace_entry) {
    fprintf (TFile, "Completed partition graph:\n");
    FOR_ALL_PQS_NODES (it, p) {
      PQS_RELATION_LIST_CONST_ITER rel_it;
      PQS_RELATION *relation;
      if (! p->Lhs_Relations ().empty ()) {
	fprintf (TFile, "S%d:\n", p->Sym ());
	FOR_ALL_RELATION_LIST (p->Lhs_Relations (), rel_it, relation) {
	  relation->Print (TFile);
	} END_FOR_ALL_RELATION_LIST
      }
    } END_FOR_ALL_PQS_NODES
    fprintf (TFile, "\n");
  }
}

void
PQS_MANAGER::Build_Disjoint ()
{
  PQS_RELATION_LIST_ITER it;
  PQS_RELATION *relation;
  FOR_ALL_RELATIONS (it, relation) {
    const PQS_SYMBOL_SET& components = relation->Components ();
    PQS_NODE *node1, *node2;
    FOR_ALL_PQS_SYMBOL_SET_MEMBER (components, node1) {
      FOR_ALL_PQS_SYMBOL_SET_MEMBER (components, node2) {
	if (node1 != node2) {
	  // node1 is disjoint from node2.
	  // Since all descendants of node1 are subsets of node1, so
	  // all descendants of node1 are also disjoint from node2
	  // and all its descendants,
	  // and vice versa.
	  PQS_SYMBOL_SET disjoint1 = node1->Descendants ();
	  PQS_SYMBOL_SET disjoint2 = node2->Descendants ();
	  disjoint1.Insert (node1);
	  disjoint2.Insert (node2);
	  PQS_NODE *dnode;
	  FOR_ALL_PQS_SYMBOL_SET_MEMBER (disjoint1, dnode) {
	    dnode->Add_Disjoint (disjoint2);
	  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
	  FOR_ALL_PQS_SYMBOL_SET_MEMBER (disjoint2, dnode) {
	    dnode->Add_Disjoint (disjoint1);
	  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
	}
      } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
    }  END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  } END_FOR_ALL_RELATIONS
}

void
PQS_MANAGER::Complete_Graph ()
{
  // Predicate information input is now complete, finalize
  // the predicate relation graph so it is ready for queries.
  Complete_Partition_Graph ();
  Build_Disjoint ();
  if (trace_entry) {
    fprintf (TFile, "Full node information:\n");
    PQS_NODE_MAP_ITER it;
    PQS_NODE *p;
    FOR_ALL_PQS_NODES (it, p) {
      p->Print_Full (TFile);
    } END_FOR_ALL_PQS_NODES
    fprintf (TFile, "\nTNs:");
    for (INT i = 1; i <= Last_TN; i++) {
      TN *tn = TNvec (i);
      PQS_TN_INFO *tninfo = tn_map_get (tn);
      if (tninfo) {
	fprintf (TFile, " TN%d(%svalid)", i,
		 tninfo->Valid () ? "" : "in");
      } 
    }
  }
}

BOOL
PQS_MANAGER::Is_Disjoint (PQS_NODE *node1, PQS_NODE *node2)
{
  return (node1 == pqs_false_node
	  || node2 == pqs_false_node
	  || node1->Disjoint().MemberP (node2));
}

BOOL
PQS_MANAGER::Is_Disjoint (PQS_NODE *node1, const PQS_SYMBOL_SET& expr)
{
  // All members of expr must be in the disjoint set of node1.
  return (node1 == pqs_false_node
	  || node1->Disjoint().ContainsP (expr));
}

BOOL
PQS_MANAGER::Is_Disjoint (const PQS_SYMBOL_SET& expr1,
			  const PQS_SYMBOL_SET& expr2)
{
  // All members of expr2 must be in the disjoint set of all
  // members of expr1.
  PQS_NODE *p;
  FOR_ALL_PQS_SYMBOL_SET_MEMBER (expr1, p) {
    if (! p->Disjoint().ContainsP (expr2)) {
      return FALSE;
    }
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  return TRUE;
}

void
PQS_MANAGER::Sum_Reduce (PQS_NODE *p, PQS_SYMBOL_SET & e)
{
  PQS_SYMBOL_SET expr = e;
  expr.Insert (p);
  PQS_RELATION_LIST_CONST_ITER it;
  PQS_RELATION *relation;
  FOR_ALL_RELATION_LIST (p->Rhs_Relations (), it, relation) {
    if (expr.ContainsP (relation->Components ())) {
      Sum_Reduce (relation->Lhs_Node (), expr);
      break;
    }
  } END_FOR_ALL_RELATION_LIST
  PQS_NODE *mem;
  FOR_ALL_PQS_SYMBOL_SET_MEMBER (expr, mem) {
    expr.Difference (mem->Descendants ());
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  e = expr;
}

BOOL
PQS_MANAGER::Is_Subset (PQS_NODE *p, PQS_NODE *q)
{
  return (p == pqs_false_node
	  || p == q
	  || p->Ancestors ().MemberP (q));
}

BOOL
PQS_MANAGER::Is_Subset (PQS_NODE *p, const PQS_SYMBOL_SET& expr)
{
  // Return TRUE if p is a subset of expr.
  // p is a subset of itself and all its ancestors.
  return (p == pqs_false_node
	  || expr.MemberP (p)
	  || expr.IntersectsP (p->Ancestors ()));
}

BOOL
PQS_MANAGER::Is_Subset (const PQS_SYMBOL_SET& expr1,
			const PQS_SYMBOL_SET& expr2)
{
  // Return TRUE iff
  //  if some element of expr1 is TRUE, then at least one
  //  element of expr2 is TRUE.
  // i.e. iff the union of all elements of expr1 is 
  // a subset of the union of all elements of expr2.
  PQS_NODE *p;
  FOR_ALL_PQS_SYMBOL_SET_MEMBER (expr1, p) {
    if (! Is_Subset (p, expr2)) {
      return FALSE;
    }
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  return TRUE;
}

PQS_NODE *
PQS_MANAGER::Find_Lca (const PQS_SYMBOL_SET& expr)
{
  PQS_NODE *lca = NULL;
  PQS_NODE *p;

  FOR_ALL_PQS_SYMBOL_SET_MEMBER (expr, p) {
    if (! lca) {
      lca = p;
    } else {
      PQS_SYMBOL_SET p_ancestors = p->Ancestors ();
      p_ancestors.Insert (p);
      PQS_SYMBOL_SET lca_ancestors = lca->Ancestors ();
      lca_ancestors.Insert (lca);
      PQS_SYMBOL_SET common_ancestors = Intersection (lca_ancestors, p_ancestors);
      INT largest_level = -1;
      PQS_NODE *q;
      FmtAssert (! common_ancestors.EmptyP (), ("No common ancestor found"));
      FOR_ALL_PQS_SYMBOL_SET_MEMBER (common_ancestors, q) {
	INT q_level = q->Level ();
	if (largest_level < q_level) {
	  lca = q;
	  largest_level = q_level;
	}
      } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
    }
  } END_FOR_ALL_PQS_SYMBOL_SET_MEMBER
  if (trace_entry) {
    fprintf (TFile, "Find_Lca (");
    expr.Print (TFile);
    fprintf (TFile, ") = S%d\n", lca->Sym ());
  }
  return lca;
}

PQS_SYMBOL_SET
PQS_MANAGER::Rel_Cmpl (PQS_NODE *node1, PQS_NODE *node2)
{
  if (! Is_Subset (node1, node2)) {
    return PQS_SYMBOL_SET::False_Expr ();
  }
  
  PQS_SYMBOL_SET result;
  // Walk up the partition graph from node2 to node1, adding
  // all the children not directly on the path.
  while (node2 != node1) {
    PQS_RELATION_LIST_CONST_ITER it;
    PQS_RELATION *r;
    FOR_ALL_RELATION_LIST (node2->Rhs_Relations (), it, r) {
      PQS_NODE *parent_node = r->Lhs_Node ();
      if (parent_node == node1
	  || parent_node->Ancestors().MemberP (node1)) {
	PQS_SYMBOL_SET components = Difference (r->Components (), node2->Sym ());
	result.Union (components);
	node2 = parent_node;
	break;
      }
    } END_FOR_ALL_RELATION_LIST
  }
  if (trace_entry) {
    fprintf (TFile, "Rel_Cmpl (S%d, S%d) = ", node1->Sym (), node2->Sym ());
    result.Print (TFile);
    fprintf (TFile, "\n");
  }
  return result;
}

BOOL
PQS_NODE::Symbol_Difference (const PQS_NODE *p, PQS_SYMBOL &result) const
{
  // Find a symbol representing the set difference:
  //    this - p.
  // If the symbol already exists, we will have a relation:
  //       this = ( p | sym )
  // and we can use the sym from that relation.
  PQS_RELATION_LIST_CONST_ITER it;
  PQS_RELATION *relation;
  FOR_ALL_RELATION_LIST (lhs_relations, it, relation) {
    const PQS_SYMBOL_SET& syms = relation->Components ();
    if (syms.MemberP (p) && syms.Count () == 2) {
      // Found a suitable candidate.
      PQS_SYMBOL p_sym = p->Sym ();
      PQS_SYMBOL sym;
      FOR_ALL_PQS_SYMBOL_SET_SYMBOL_MEMBER (syms, sym) {
	if (sym != p_sym) {
	  result = sym;
	  return TRUE;
	}
      } END_FOR_ALL_PQS_SYMBOL_SET_SYMBOL_MEMBER
    }
  } END_FOR_ALL_RELATION_LIST
  return FALSE;
}

PQS_NODE *
PQS_MANAGER::Symbol_Difference (const PQS_NODE *p1, const PQS_NODE *p2)
{
  // Return a node representing p1 - p2.
  PQS_SYMBOL sym;
  if (p1->Symbol_Difference (p2, sym)) {
    return PQS_Node (sym);
  } else {
    return PQS_Create_Symbol ();
  }
}

static UINT
hash (PQS_NODE *n1, VARIANT v, PQS_NODE *n2, PQS_NODE *n3)
{
  return (((UINT)n1->Sym () + (UINT)v + (UINT)n2->Sym () + (UINT)n3->Sym ())
	  % PQS_HASH_SIZE);
}
    
PQS_NODE *
PQS_MANAGER::PQS_hash_lookup (PQS_NODE *qual_sym,
			       BOOL negate_result,
			       VARIANT v,
			       PQS_NODE *sym1, PQS_NODE *sym2)
{
  // We always lookup the non-negated symbol.
  // It has a link to the negated form, if we need it.
  UINT bucket = hash(qual_sym, v, sym1, sym2);
  PQS_NODE_LIST_ITER it;
  PQS_NODE *p = NULL;
  if (trace_hash) {
    fprintf (TFile, "Look up qual:S%d %sS%d %s S%d ...",
	     qual_sym->Sym (),
	     (negate_result ? "negate " : ""),
	     sym1->Sym (), CMP_Variant_Name (v), sym2->Sym ());
  }
  for (it = Hash_Table[bucket].begin ();
       it != Hash_Table[bucket].end ();
       ++it) {
    if ((*it)->Match (qual_sym, v, sym1, sym2)) {
      p = *it;
      break;
    }
  }
  if (!p) {
    if (trace_hash) fprintf (TFile, " not found\n");
    // Create the symbol and its negation; add them to the partition graph.
    p = PQS_Create_Symbol (qual_sym, v, FALSE, sym1, sym2);
    PQS_SYMBOL sym = p->Sym ();
    PQS_NODE * neg_p = p->Negated_Node ();
    if (v == V_CMP_ANDL
	&& (qual_sym == pqs_true_node
	    || (sym1->Qual_Sym () == qual_sym
		&& sym2->Qual_Sym () == qual_sym))) {
      // We can represent this more accurately, as a partition of both
      // sym1 and sym2.
      // Given   sym  = sym1 AND sym2 :
      // We have sym1 = ( sym | sym1 - sym )
      //         sym2 = ( sym | sym2 - sym )
      // Similarly, the complements:
      //         !sym = ( !sym1 | sym1 - sym )
      //         !sym = ( !sym2 | sym2 - sym )
      //  (using the identity (!S1 - !S2) <=> (S2 - S1).
      PQS_NODE *neg_sym1 = sym1->Negated_Node ();
      PQS_NODE *neg_sym2 = sym2->Negated_Node ();
      PQS_NODE *p2 = Symbol_Difference (sym1, p);  // sym1 - sym
      PQS_NODE *p3 = Symbol_Difference (sym2, p);  // sym2 - sym
      Emit_Partition_Relation (sym1, p, p2);
      Emit_Partition_Relation (sym2, p, p3);
      Emit_Partition_Relation (neg_p, neg_sym1, p2);
      Emit_Partition_Relation (neg_p, neg_sym2, p3);
      // Could handle ORL here also, but no need, since it will have
      // been transformed to ANDL (where possible), by the caller.
    } else {
      Emit_Partition_Relation (qual_sym, p, neg_p);
    }
  } else if (trace_hash) {
    fprintf (TFile, "matched S%d\n", p->Sym ());
  }
  return negate_result ? p->Negated_Node () : p;
}

PQS_NODE *
PQS_MANAGER::PQS_Create_Symbol (PQS_NODE *qual_sym, VARIANT v,
				BOOL negate_result, PQS_NODE *sym1, PQS_NODE *sym2)
{
  PQS_NODE *p = CXX_NEW (PQS_NODE (next_pqs_symbol++, qual_sym, v, FALSE, sym1, sym2),
			 &PQS_mem_pool);
  Add_Node (p);
  if (v != V_NONE) {
    Hash_Table[hash(qual_sym, v, sym1, sym2)].push_back (p);
  }
  PQS_NODE *neg_p = CXX_NEW (PQS_NODE (next_pqs_symbol++, qual_sym, v, TRUE,
				       sym1, sym2), &PQS_mem_pool);
  Add_Node (neg_p);
  p->Set_Negated_Node (neg_p);
  neg_p->Set_Negated_Node (p);
  if (v == V_NONE && qual_sym) {
    // There is no instruction creating a relation for this symbol,
    // so make a default relation between the symbol and its negate.
    // Note that qual_sym == NULL in the case that we are creating the
    // special symbols unknown, true, false, which do not want a
    // relation here.
    Emit_Partition_Relation (qual_sym, p, neg_p);
  }
  return p;
}

PQS_NODE *
PQS_MANAGER::PQS_Create_Symbol ()
{
  return PQS_Create_Symbol (pqs_true_node, V_NONE, FALSE, NULL, NULL);
}

UINT32
PQS_MANAGER::PQS_Rank (PQS_NODE *p)
{
  return (UINT32)p->Sym ();
}

void
PQS_MANAGER::Note_Use (BB *bb, TN *tn)
{
  PQS_TN_INFO *tn_info = tn_map_get (tn);
  if (! tn_info) {
    tn_info = CXX_NEW (PQS_TN_INFO (tn), &PQS_mem_pool);
    tn_map[tn] = tn_info;
  }
  tn_info->Note_Use (bb);
}

void
PQS_MANAGER::Note_Def (BB *bb, TN *tn)
{
  PQS_TN_INFO *tn_info = tn_map_get (tn);
  if (! tn_info) {
    tn_info = CXX_NEW (PQS_TN_INFO (tn), &PQS_mem_pool);
    tn_map[tn] = tn_info;
  }
  tn_info->Note_Def (bb);
}

void
PQS_MANAGER::Note_Modified_Registers (const REGISTER_SET *mr) {
  ISA_REGISTER_CLASS rc;
  FOR_ALL_ISA_REGISTER_CLASS (rc) {
    modified_registers[rc] = mr[rc];
  }
}

void
PQS_MANAGER::Note_Synthesized_TN  (TN *tn)
{
  Note_Use (NULL, tn);
}

// Enter into the PQS a predicate which represents (OPD1 v OPD2).
// PRED_TN is the real TN that represents this predicate in the CGIR,
//   may be NULL.
// QUAL_TN is the predicate under which PRED_TN is defined, may be True_TN.
PQS_NODE *
PQS_MANAGER::Add_Predicate_Cmp_Def (TN *pred_tn, TN *qual_tn,
				    VARIANT v, TN *opd1, TN *opd2)
{
  if (qual_tn == NULL) qual_tn = True_TN;
  return Add_Predicate_Cmp_Def (pred_tn, PQS_Node (qual_tn), v,
				PQS_Node (opd1), PQS_Node (opd2));
}

PQS_NODE *
PQS_MANAGER::Add_Predicate_Cmp_Def (TN *pred_tn, PQS_NODE *qual_sym, 
				    VARIANT v, PQS_NODE *sym1,
				    PQS_NODE *sym2)
{
  BOOL negate_result = FALSE;
  PQS_NODE *p_sym;

  if (! qual_sym || ! sym1 || ! sym2)
    return NULL;

  // Normalize variant.

  // 1.Order the two operands by rank, inverting the compare as
  //   necessary.
  if (PQS_Rank (sym1) < PQS_Rank (sym2)) {
    PQS_NODE *t = sym1; sym1 = sym2; sym2 = t;
    v = Invert_CMP_Variant (v);
  }

  // 2.Bring any negate out of the compare, so that the only comparisons
  //   needed are:
  //          !=, &&, ||, >, >u, >=, >=u.
  //  
  switch (v) {
  case V_CMP_EQ:
  case V_CMP_NANDL:
  case V_CMP_NORL:
  case V_CMP_LT:
  case V_CMP_LTU:
  case V_CMP_LE:
  case V_CMP_LEU:
    v = Negate_CMP_Variant (v);
    negate_result = TRUE;
    break;
  }

  // 3. Normalize OR into AND.
  if (v == V_CMP_ORL) {
    sym1 = sym1->Negated_Node ();
    sym2 = sym2->Negated_Node ();
    negate_result = ! negate_result;
    v = V_CMP_ANDL;
  }
  
  // Recognize some idioms: andl with oneself is a copy.
  //  qual and (sym and sym)   => qual and sym
  if (sym1 == sym2
      && v == V_CMP_ANDL
      && (qual_sym == pqs_true_node || qual_sym == sym1->Qual_Sym ())) {
    PQS_NODE *sym = negate_result ? sym1->Negated_Node () : sym1;
    if (pred_tn) PQS_TN_Map_To_Node (pred_tn, sym);
    return sym;
  }
  // qual and (false and any)  => false.
  if (v == V_CMP_ANDL
      && (sym1 == pqs_false_node || sym2 == pqs_false_node)) {
    PQS_NODE *sym = negate_result ? pqs_false_node->Negated_Node () : pqs_false_node;
    if (pred_tn) PQS_TN_Map_To_Node (pred_tn, sym);
    return sym;
  }
  // compare of predicate NE zero is also a copy.
  //  qual and (sym ne zero)    => qual and sym
  if (v == V_CMP_NE) {
    PQS_NODE *zero_node = PQS_Node (zero_tn);
    PQS_NODE *sym = ((sym1 == zero_node) ? sym2 :
		     (sym2 == zero_node) ? sym1 : NULL);
    if (sym
	&& (qual_sym == pqs_true_node || qual_sym == sym->Qual_Sym ())) {
      sym = negate_result ? sym->Negated_Node () : sym;
      if (pred_tn) PQS_TN_Map_To_Node (pred_tn, sym);
      return sym;
    }
  }

  // Now lookup (qual, v, sym1, sym2).
  
  p_sym = PQS_hash_lookup (qual_sym, negate_result, v, sym1, sym2);

  // Label the predicate equation with the result TN.

  if (pred_tn) {
    PQS_TN_Map_To_Node (pred_tn, p_sym);
  }

  return p_sym;
}  

void
PQS_MANAGER::Add_Copy (TN *result_tn, TN *qual_tn, TN *source_tn)
{
  // We are copying source_tn to result_tn, if qual_tn is true.
  // If we already have a node for source_tn, label it with result_tn.
  // We can only do this if source_tn is also qualified with qual_tn.
  if (qual_tn == NULL) qual_tn = True_TN;
  PQS_NODE *source_p = PQS_Node (source_tn);
  PQS_NODE *qual_p   = PQS_Node (qual_tn);
  if (source_p && source_p->Qual_Sym () == qual_p) {
    PQS_TN_Map_To_Node (result_tn, source_p);
  }
}

void
PQS_MANAGER::Set_True (TN *result_tn, TN *qual_tn)
{
  if (qual_tn == NULL) qual_tn = True_TN;
  PQS_NODE *qual_p = PQS_Node (qual_tn);
  if (qual_p == pqs_true_node) {
    PQS_TN_Map_To_Node (result_tn, pqs_true_node);
  }
}

void
PQS_MANAGER::Set_False (TN *result_tn, TN *qual_tn)
{
  if (qual_tn == NULL) qual_tn = True_TN;
  PQS_NODE *qual_p = PQS_Node (qual_tn);
  if (qual_p == pqs_true_node) {
    PQS_TN_Map_To_Node (result_tn, pqs_false_node);
  }
}

BOOL
PQS_MANAGER::PQS_is_disjoint(TN * tn1, TN * tn2) {
  PQS_TN_INFO *tn1_info = tn_map_get(tn1);
  PQS_TN_INFO *tn2_info = tn_map_get(tn2);
  if (! tn1_info || ! tn2_info ||
      ! tn1_info->Valid () ||
      ! tn2_info->Valid ()) {
    return FALSE;
  }
  PQS_NODE *sym1 = tn1_info->PQS_Node ();
  PQS_NODE *sym2 = tn2_info->PQS_Node ();
  return sym1 && sym2 && Is_Disjoint (sym1, sym2);
}

PQS_NODE *
PQS_MANAGER::PQS_Node (TN *tn)
{
  PQS_TN_INFO *tn_info = tn_map_get (tn);
  FmtAssert (tn_info, ("Unexpected null tn info"));
  PQS_NODE *p = tn_info->PQS_Node ();
  if (! p && tn_info->Valid ()) {
    // Normalize false and true.
    if (TN_is_zero (tn)) {
      p = pqs_false_node;
    } else if (TN_is_register (tn) && TN_is_true (tn)) {
      p = pqs_true_node;
    } else {
      p = PQS_Create_Symbol ();
    }
    tn_info->Set_PQS_Node (p);
  }
  return p;
}

PQS_SYMBOL
PQS_MANAGER::PQS_Symbol (TN *tn)
{
  return PQS_Node (tn)->Sym();
}

void
PQS_MANAGER::Add_Node (PQS_NODE *p)
{
  FmtAssert (node_map.size () == p->Sym (),
	     ("node_map size (%d) out of sync with PQS_SYMBOL (%d)",
	      node_map.size (), p->Sym ()));
  node_map.push_back (p);
}

BOOL
PQS_MANAGER::PQS_is_disjoint (PQS_TN_SET &tns1, PQS_TN_SET &tns2)
{
  return (Is_Valid (tns1)
	  && Is_Valid (tns2)
	  && Is_Disjoint (PQS_Symbol_Set (tns1), PQS_Symbol_Set (tns2)));
}

BOOL
PQS_MANAGER::PQS_is_subset_of (TN * tn1, TN * tn2) {
  PQS_TN_INFO *tn1_info = tn_map_get (tn1);
  PQS_TN_INFO *tn2_info = tn_map_get (tn2);
  if (! tn1_info || ! tn2_info ||
      ! tn1_info->Valid () ||
      ! tn2_info->Valid ()) {
    return FALSE;
  }
  PQS_NODE *sym1 = tn1_info->PQS_Node ();
  PQS_NODE *sym2 = tn2_info->PQS_Node ();
  return sym1 && sym2 && Is_Subset (sym1, sym2);
}

BOOL
PQS_MANAGER::PQS_is_subset_of(TN * tn1, PQS_TN_SET &tns2)
{
  PQS_TN_INFO *tn1_info = tn_map_get (tn1);
  if (!tn1_info || !tn1_info->Valid ()) {
    return FALSE;
  }
  PQS_NODE *sym1 = tn1_info->PQS_Node ();
  return (sym1 && Is_Valid (tns2)
	  && Is_Subset (sym1, PQS_Symbol_Set (tns2)));
}

BOOL
PQS_MANAGER::PQS_is_subset_of (PQS_TN_SET &tns1, PQS_TN_SET &tns2)
{
  return (Is_Valid (tns1)
	  && Is_Valid (tns2)
	  && Is_Subset (PQS_Symbol_Set (tns1), PQS_Symbol_Set (tns2)));
}

PQS_MANAGER::PQS_MANAGER ()
#ifdef PQS_USE_MEMPOOLS
  : current_marker (0)
#endif
{
  trace_entry = Get_Trace(TP_PQS, PQS_ENTRY);
  trace_hash = Get_Trace(TP_PQS, PQS_HASH);
  all_relations.clear ();
  node_map.clear ();
  for (INT i = 0; i < PQS_HASH_SIZE; i++) {
    Hash_Table[i].clear ();
  }
  zero_tn = Gen_Literal_TN (0, 4);
  pqs_unknown_node = PQS_Create_Symbol ();
  pqs_true_node = PQS_Create_Symbol ();
  pqs_false_node = PQS_Create_Symbol ();
  pqs_true_node->Set_Negated_Node (pqs_false_node);
  pqs_false_node->Set_Negated_Node (pqs_true_node);
  tn_map[zero_tn] = CXX_NEW (PQS_TN_INFO (zero_tn), &PQS_mem_pool);
  tn_map[True_TN] = CXX_NEW (PQS_TN_INFO (True_TN), &PQS_mem_pool);
}
