/*
  Copyright (C) 2006, STMicroelectronics, All Rights Reserved.

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

/* ====================================================================
 * ====================================================================
 *
 * Module: paired_lattice.cxx
 *
 * Description:
 *
 * Implementation of a template paired lattice type.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "paired_lattice.h"


template<class T1, class T2> MEM_POOL PairedLattice<T1,T2>::mempool_;

// new and delete overriding
template<class T1, class T2> void* PairedLattice<T1,T2>::operator new (size_t size) {
  FmtAssert(&(PairedLattice<T1,T2>::mempool_) != NULL, ("PairedLattice<T1,T2> mempool not initialized"));
  return TYPE_MEM_POOL_ALLOC(PairedLattice, &PairedLattice::mempool_);
}

template<class T1, class T2> void PairedLattice<T1,T2>::operator delete (void *p) {
  FmtAssert(0, ("PairedLattice<T1,T2> delete operator should never be called"));
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeRangeValue(INT64 i) const { 
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeRangeValue(i)),*LRANGE_CAST(T2 *, second_.makeRangeValue(i)));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeRangeMinMax(ZInt min, ZInt max) const { 
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeRangeMinMax(min, max)),*LRANGE_CAST(T2 *, second_.makeRangeMinMax(min, max)));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeRangeWidth (INT Sign, INT bitwidth) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeRangeWidth(Sign, bitwidth)),*LRANGE_CAST(T2 *, second_.makeRangeWidth(Sign, bitwidth)));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeRangeLowbit(Sign, lowbit, bitwidth)),*LRANGE_CAST(T2 *, second_.makeRangeLowbit(Sign, lowbit, bitwidth)));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeUniverse () const { 
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeUniverse()),*LRANGE_CAST(T2 *, second_.makeUniverse()));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeTop () const { 
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeTop()),*LRANGE_CAST(T2 *, second_.makeTop()));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeBottom () const { 
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeBottom()),*LRANGE_CAST(T2 *, second_.makeBottom()));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMeet (const LRange *a, const LRange *b) const { 
  const PairedLattice *pla = LRANGE_CAST(const PairedLattice *, a);
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMeet(&pla->first_, &plb->first_)),*LRANGE_CAST(T2 *, second_.makeMeet(&pla->second_, &plb->second_)));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeJoin (const LRange *a, const LRange *b) const { 
  const PairedLattice *pla = LRANGE_CAST(const PairedLattice *, a);
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice(*LRANGE_CAST(T1 *, first_.makeJoin(&pla->first_, &plb->first_)),*LRANGE_CAST(T2 *, second_.makeJoin(&pla->second_, &plb->second_)));
  return r;
}


// queries
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeSignExtend (INT width) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeSignExtend(width)),*LRANGE_CAST(T2 *, second_.makeSignExtend(width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeZeroExtend (INT width) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeZeroExtend(width)),*LRANGE_CAST(T2 *, second_.makeZeroExtend(width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeLeftShift (INT count) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeLeftShift(count)),*LRANGE_CAST(T2 *, second_.makeLeftShift(count)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeLeftShiftRange (const LRange *count) const {
  const PairedLattice *plcount = LRANGE_CAST(const PairedLattice *, count);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeLeftShiftRange(&plcount->first_)),*LRANGE_CAST(T2 *, second_.makeLeftShiftRange(&plcount->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeSignedRightShift (INT count) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeSignedRightShift(count)),*LRANGE_CAST(T2 *, second_.makeSignedRightShift(count)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeSignedRightShiftRange (const LRange *count) const {
  const PairedLattice *plcount = LRANGE_CAST(const PairedLattice *, count);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeSignedRightShiftRange(&plcount->first_)),*LRANGE_CAST(T2 *, second_.makeSignedRightShiftRange(&plcount->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeModulo (INT width) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeModulo(width)),*LRANGE_CAST(T2 *, second_.makeModulo(width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeAdd (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeAdd(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeAdd(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeAddModulo (const LRange *b, INT width) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeAddModulo(&plb->first_, width)),*LRANGE_CAST(T2 *, second_.makeAddModulo(&plb->second_, width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeSub (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeSub(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeSub(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeSubModulo (const LRange *b, INT width) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeSubModulo(&plb->first_, width)),*LRANGE_CAST(T2 *, second_.makeSubModulo(&plb->second_, width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMul (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMul(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeMul(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMulModulo (const LRange *b, INT width) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMulModulo(&plb->first_, width)),*LRANGE_CAST(T2 *, second_.makeMulModulo(&plb->second_, width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMin (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMin(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeMin(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMax (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMax(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeMax(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeBitAnd (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeBitAnd(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeBitAnd(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeBitOr (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeBitOr(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeBitOr(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeBitXor (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeBitXor(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeBitXor(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeBitNot () const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeBitNot()),*LRANGE_CAST(T2 *, second_.makeBitNot()));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeEq (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeEq(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeEq(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeNe (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeNe(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeNe(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeGe (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeGe(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeGe(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeGt (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeGt(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeGt(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeLe (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeLe(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeLe(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeLt (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeLt(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeLt(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeAndl (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeAndl(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeAndl(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeNandl (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeNandl(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeNandl(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeOrl (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeOrl(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeOrl(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeNorl (const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeNorl(&plb->first_)),*LRANGE_CAST(T2 *, second_.makeNorl(&plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeNotl () const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeNotl()),*LRANGE_CAST(T2 *, second_.makeNotl()));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeHighestSetBit () const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeHighestSetBit()),*LRANGE_CAST(T2 *, second_.makeHighestSetBit()));
  return r;
}
/*   template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeByteSwap () const { */
/*     LRange *r = new PairedLattice<T1,T2>(ByteSwap (this->range_)); */
/*     return r; */
/*   } */
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeExtract (INT start, INT width) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeExtract(start, width)),*LRANGE_CAST(T2 *, second_.makeExtract(start, width)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeInsert (INT start, INT width,
				     const LRange *b) const {
  const PairedLattice *plb = LRANGE_CAST(const PairedLattice *, b);
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeInsert(start, width, &plb->first_)),*LRANGE_CAST(T2 *, second_.makeInsert(start, width, &plb->second_)));
  return r;
}
template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMakeUnsigned (INT width) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMakeUnsigned(width)),*LRANGE_CAST(T2 *, second_.makeMakeUnsigned(width)));
  return r;
}

template<class T1, class T2> LRange *PairedLattice<T1,T2>::makeMakeSigned (INT width) const {
  LRange *r = new PairedLattice<T1,T2>(*LRANGE_CAST(T1 *, first_.makeMakeSigned(width)),*LRANGE_CAST(T2 *, second_.makeMakeSigned(width)));
  return r;
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::hasValue(void) const { 
  return first_.hasValue () || second_.hasValue (); 
}

template<class T1, class T2> INT64 PairedLattice<T1,T2>::getValue(void) const { 
  if (first_.hasValue ())
    return first_.getValue ();
  return second_.getValue (); 
}

template<class T1, class T2> UINT64 PairedLattice<T1,T2>::getZeroMask(void) const { 
  return first_.getZeroMask () | second_.getZeroMask (); 
}

template<class T1, class T2> UINT64 PairedLattice<T1,T2>::getOneMask(void) const { 
  return first_.getOneMask () | second_.getOneMask (); 
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::isTop () const {
  return first_.isTop () || second_.isTop ();
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::StrictlyContains (const LRange *a) const {
  const PairedLattice *pla = LRANGE_CAST(const PairedLattice *, a);
  return first_.StrictlyContains (&pla->first_) || second_.StrictlyContains (&pla->second_);
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::Equal (const LRange *a) const {
  const PairedLattice *pla = LRANGE_CAST(const PairedLattice *, a);
  return first_.Equal (&pla->first_) && second_.Equal (&pla->second_);
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::ContainsOrEqual (const LRange *a) const {
  const PairedLattice *pla = LRANGE_CAST(const PairedLattice *, a);
  return first_.ContainsOrEqual (&pla->first_) && second_.ContainsOrEqual (&pla->second_);
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::isNonZero () const {
  return first_.isNonZero () || second_.isNonZero ();
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::isNonNegative () const {
  return first_.isNonNegative () || second_.isNonNegative ();
}

template<class T1, class T2> BOOL PairedLattice<T1,T2>::isZero () const {
  return first_.isZero () || second_.isZero ();
}

template<class T1, class T2> INT PairedLattice<T1,T2>::bits() const {
  return Min(first_.bits(), second_.bits()) ;
}

template<class T1, class T2> INT PairedLattice<T1,T2>::getTzcnt() const {
  return Max(first_.getTzcnt(), second_.getTzcnt()) ;
}

template<class T1, class T2> void PairedLattice<T1,T2>::Print (FILE *f) const {
  first_.Print (f);
  second_.Print (f);
}

// cloning
template<class T1, class T2> LRange *PairedLattice<T1,T2>::clone(void) const { return new PairedLattice<T1,T2>(*this); }

// instance
template<class T1, class T2> LRange *PairedLattice<T1,T2>::getInstance(void) { return new PairedLattice<T1,T2>(); }

// mempool initialization
template<class T1, class T2> void PairedLattice<T1,T2>::MEMPOOL_Initialize(MEM_POOL mempool) {
  PairedLattice<T1,T2>::mempool_ = mempool;
  T1::MEMPOOL_Initialize(mempool);
  T2::MEMPOOL_Initialize(mempool);
}

// explicit instanciation of template specialization
template class PairedLattice <LVRange, LBValRange>;
