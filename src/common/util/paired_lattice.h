/*
  Copyright (C) 2006, STMicroelectronics, All Rights Reserved.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

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
 * Module: paired_lattice.h
 *
 * Description:
 *
 * Implement a template derived class of LRange for range analysis 
 * using a pair of lattices
 * ====================================================================
 * ====================================================================
 */

#ifndef PAIRED_LATTICE_H_INCLUDED
#define PAIRED_LATTICE_H_INCLUDED

#include "mempool.h"
#include "lrange.h"
#include "lvrange.h"
#include "lbvalrange.h"
#include "larange.h"
#include "zint.h"
#include <assert.h>

template <class T1, class T2> class PairedLattice: public LRange {
private:
  T1 first_;
  T2 second_;
  static MEM_POOL mempool_;
  static PairedLattice *top_ ;
  static PairedLattice *bottom_ ;
  static PairedLattice *universe_ ;
public:
  // constructors
  PairedLattice(const PairedLattice<T1,T2> &that) {*this = that;}
  PairedLattice(const T1 &f, const T2 &s): first_(f), second_(s) { }
  PairedLattice(): first_(T1::Bottom ()), second_(T2::Bottom ()) { }
  virtual ~PairedLattice() { }

  static PairedLattice<T1,T2> Top(){ return PairedLattice<T1,T2>(T1::Top(), T2::Top());}
  static PairedLattice<T1,T2> Bottom(){ return PairedLattice<T1,T2>(T1::Bottom(), T2::Bottom());}
  static PairedLattice<T1,T2> Universe(){ return PairedLattice<T1,T2>(T1::Universe(), T2::Universe());}

  virtual PairedLattice<T1,T2> *makeRangeValue(INT64 i) const;

  virtual PairedLattice<T1,T2> *makeRangeMinMax(ZInt min, ZInt max) const;

  virtual PairedLattice<T1,T2> *makeRangeWidth (INT Sign, INT bitwidth) const;

  virtual PairedLattice<T1,T2> *makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const;

  virtual PairedLattice<T1,T2> *makeAlign(ZInt base, ZInt bias) const;

  virtual PairedLattice<T1,T2> *makeUniverse () const;

  virtual PairedLattice<T1,T2> *makeTop () const;

  virtual PairedLattice<T1,T2> *makeBottom () const;

  virtual PairedLattice<T1,T2> *makeMeet (const LRange *a, const LRange *b) const;

  virtual PairedLattice<T1,T2> *makeJoin (const LRange *a, const LRange *b) const;

  // queries
  virtual PairedLattice<T1,T2> *makeSignExtend (INT width) const;
  virtual PairedLattice<T1,T2> *makeZeroExtend (INT width) const;
  virtual PairedLattice<T1,T2> *makeLeftShift (INT count) const;
  virtual PairedLattice<T1,T2> *makeLeftShiftRange (const LRange *count) const;
  virtual PairedLattice<T1,T2> *makeSignedRightShift (INT count) const;
  virtual PairedLattice<T1,T2> *makeSignedRightShiftRange (const LRange *count) const;
  virtual PairedLattice<T1,T2> *makeModulo (INT width) const;
  virtual PairedLattice<T1,T2> *makeAdd (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeAddModulo (const LRange *b, INT width) const;
  virtual PairedLattice<T1,T2> *makeSub (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeSubModulo (const LRange *b, INT width) const;
  virtual PairedLattice<T1,T2> *makeMul (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeMulModulo (const LRange *b, INT width) const;
  virtual PairedLattice<T1,T2> *makeMin (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeMax (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeBitAnd (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeBitOr (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeBitXor (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeBitNot () const;
  virtual PairedLattice<T1,T2> *makeBytePermute (INT mask) const;
  virtual PairedLattice<T1,T2> *makeEq (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeNe (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeGe (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeGt (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeLe (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeLt (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeAndl (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeNandl (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeOrl (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeNorl (const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeNotl () const;
  virtual PairedLattice<T1,T2> *makeHighestSetBit () const;
/*   virtual LRange *makeByteSwap () const { */
/*     LRange *r = TYPE_MEM_POOL_ALLOC (LRange, &mempool_); r = LVRange(ByteSwap (this->range_)); */
/*     return r; */
/*   } */
  virtual PairedLattice<T1,T2> *makeExtract (INT start, INT width) const;
  virtual PairedLattice<T1,T2> *makeInsert (INT start, INT width,
			      const LRange *b) const;
  virtual PairedLattice<T1,T2> *makeMakeUnsigned (INT width) const;
  virtual PairedLattice<T1,T2> *makeMakeSigned (INT width) const;
  virtual BOOL hasValue(void) const;
  virtual INT64 getValue(void) const;
  virtual UINT64 getZeroMask(void) const;
  virtual UINT64 getOneMask(void) const;
  virtual UINT64 getBase(void) const;
  virtual UINT64 getBias(void) const;


  virtual BOOL isTop () const;
  virtual BOOL StrictlyContains (const LRange *a) const;
  virtual BOOL Equal (const LRange *a) const;
  virtual BOOL ContainsOrEqual (const LRange *a) const;
  virtual BOOL isNonZero () const;
  virtual BOOL isNonNegative () const;
  virtual BOOL isZero () const;
  virtual INT bits() const;
  virtual INT getTzcnt() const;
  virtual void Print (FILE *f) const;

  // cloning
  virtual PairedLattice<T1,T2> *clone(void) const;

  // instance
  BE_EXPORTED  static PairedLattice<T1,T2> *getInstance(void);

  // mempool initialization
  BE_EXPORTED static void MEMPOOL_Initialize(MEM_POOL mempool);

  // mempool visibility
  virtual MEM_POOL *Mem_pool()          {return &mempool_;}
};


#endif // PAIRED_LATTICE_H_INCLUDED
