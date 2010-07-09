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
 * Module: lvrange.cxx
 *
 * Description:
 *
 * Implementation of lattice value range type.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "cxx_memory.h"
#include "lvrange.h"

MEM_POOL LVRange::mempool_;
LVRange::LVRange *LVRange::top_ = new LVRange(Range::Empty ()) ; 
LVRange::LVRange *LVRange::bottom_ = new LVRange(Range::Universe ()) ;
LVRange::LVRange *LVRange::universe_ = new LVRange(Range::Universe ()) ;

LVRange *LVRange::makeRangeValue(INT64 i) const { 
  LVRange *r = CXX_NEW (LVRange(Range(i)), &mempool_);
  return r;
}

LVRange *LVRange::makeRangeMinMax(ZInt min, ZInt max) const { 
  LVRange *r = CXX_NEW (LVRange(Range(min, max)), &mempool_);
  return r;
}

LVRange *LVRange::makeRangeWidth (INT Sign, INT bitwidth) const {
  LVRange *r = CXX_NEW (LVRange(Range((Range::RangeSign)Sign, bitwidth)), &mempool_);
  return r;
}

LVRange *LVRange::makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const {
  LVRange *r = CXX_NEW (LVRange(Range((Range::RangeSign)Sign, lowbit, bitwidth)), &mempool_);
  return r;
}

LVRange *LVRange::makeUniverse () const { 
  Is_True(universe_, ("%s : unexpected uninitialized universe", __PRETTY_FUNCTION__));
  return universe_;
}

LVRange *LVRange::makeTop () const { 
  Is_True(top_, ("%s : unexpected uninitialized top", __PRETTY_FUNCTION__));
  return top_;
}

LVRange *LVRange::makeBottom () const { 
  Is_True(bottom_, ("%s : unexpected uninitialized bottom", __PRETTY_FUNCTION__));
  return bottom_;
}

LVRange *LVRange::makeMeet (const LRange *a, const LRange *b) const { 
  const LVRange *vrange1 = static_cast<const LVRange *>(a);
  const LVRange *vrange2 = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Union (vrange1->range_, vrange2->range_)), &mempool_);
  return r;
}

LVRange *LVRange::makeJoin (const LRange *a, const LRange *b) const { 
  const LVRange *vrange1 = static_cast<const LVRange *>(a);
  const LVRange *vrange2 = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Intersect (vrange1->range_, vrange2->range_)), &mempool_);
  return r;
}


// queries
LVRange *LVRange::makeSignExtend (INT width) const {
  LVRange *r = CXX_NEW (LVRange(SignExtend (this->range_, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeZeroExtend (INT width) const {
  LVRange *r = CXX_NEW (LVRange(ZeroExtend (this->range_, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeLeftShift (INT count) const {
  LVRange *r = CXX_NEW (LVRange(LeftShift (this->range_, count)), &mempool_);
  return r;
}
LVRange *LVRange::makeLeftShiftRange (const LRange *count) const {
  const LVRange *vrange = static_cast<const LVRange *>(count);
  LVRange *r = CXX_NEW (LVRange(LeftShift (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeSignedRightShift (INT count) const {
  LVRange *r = CXX_NEW (LVRange(SignedRightShift (this->range_, count)), &mempool_);
  return r;
}
LVRange *LVRange::makeSignedRightShiftRange (const LRange *count) const {
  const LVRange *vrange = static_cast<const LVRange *>(count);
  LVRange *r = CXX_NEW (LVRange(SignedRightShift (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeModulo (INT width) const {
  LVRange *r = CXX_NEW (LVRange(Modulo (this->range_, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeAdd (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Add (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeAddModulo (const LRange *b, INT width) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(AddModulo (this->range_, vrange->range_, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeSub (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Sub (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeSubModulo (const LRange *b, INT width) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(SubModulo (this->range_, vrange->range_, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeMul (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Mul (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeMulModulo (const LRange *b, INT width) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(MulModulo (this->range_, vrange->range_, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeMin (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Min (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeMax (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Max (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeBitAnd (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(BitAnd (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeBitOr (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(BitOr (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeBitXor (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(BitXor (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeBitNot () const {
  LVRange *r = CXX_NEW (LVRange(BitNot (this->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeEq (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Eq (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeNe (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Ne (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeGe (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Ge (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeGt (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Gt (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeLe (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Le (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeLt (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Lt (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeAndl (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Andl (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeNandl (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Nandl (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeOrl (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Orl (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeNorl (const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Norl (this->range_, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeNotl () const {
  LVRange *r = CXX_NEW (LVRange(Notl (this->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeHighestSetBit () const {
  LVRange *r = CXX_NEW (LVRange(HighestSetBit (this->range_)), &mempool_);
  return r;
}
/*   LVRange *LVRange::makeByteSwap () const { */
/*     LVRange *r = CXX_NEW (LVRange(ByteSwap (this->range_)), &mempool_); */
/*     return r; */
/*   } */
LVRange *LVRange::makeExtract (INT start, INT width) const {
  LVRange *r = CXX_NEW (LVRange(Extract (this->range_, start, width)), &mempool_);
  return r;
}
LVRange *LVRange::makeInsert (INT start, INT width,
				     const LRange *b) const {
  const LVRange *vrange = static_cast<const LVRange *>(b);
  LVRange *r = CXX_NEW (LVRange(Insert (this->range_, start, width, vrange->range_)), &mempool_);
  return r;
}
LVRange *LVRange::makeMakeUnsigned (INT width) const {
  LVRange *r = CXX_NEW (LVRange(MakeUnsigned (this->range_, width)), &mempool_);
  return r;
}

LVRange *LVRange::makeMakeSigned (INT width) const {
  LVRange *r = CXX_NEW (LVRange(MakeSigned (this->range_, width)), &mempool_);
  return r;
}
BOOL LVRange::hasValue(void) const { 
  return range_.isLiteral (); 
}

INT64 LVRange::getValue(void) const { 
  return range_.LiteralValue (); 
}


BOOL LVRange::isTop () const {
  return range_.isEmpty ();
}

BOOL LVRange::StrictlyContains (const LRange *a) const {
  const LVRange *vrange = static_cast<const LVRange *>(a);
  return range_.StrictlyContains (vrange->range_);
}

BOOL LVRange::Equal (const LRange *a) const {
  const LVRange *vrange = static_cast<const LVRange *>(a);
  return range_.Equal (vrange->range_);
}

BOOL LVRange::ContainsOrEqual (const LRange *a) const {
  const LVRange *vrange = static_cast<const LVRange *>(a);
  return range_.ContainsOrEqual (vrange->range_);
}

BOOL LVRange::isNonZero () const {
  return range_.isNonZero ();
}

BOOL LVRange::isNonNegative () const {
  return range_.isNonNegative ();
}

BOOL LVRange::isZero () const {
  return range_.isZero ();
}

INT LVRange::bits() const {
  return range_.bits();
}

void LVRange::Print (FILE *f) const {
  range_.Print (f);
}

// cloning
LVRange *LVRange::clone(void) const { return CXX_NEW (LVRange(*this), &mempool_); }

// instance
LVRange *LVRange::getInstance(void) { return CXX_NEW (LVRange(), &mempool_); }

// mempool initialization
void LVRange::MEMPOOL_Initialize(MEM_POOL mempool) {LVRange::mempool_ = mempool;}
