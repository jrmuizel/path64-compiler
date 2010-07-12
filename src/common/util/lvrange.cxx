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
#include "lvrange.h"

MEM_POOL LVRange::mempool_;

// new and delete overriding
void* LVRange::operator new (size_t size) {
  FmtAssert(&LVRange::mempool_ != NULL, ("LVRange mempool not initialized"));
  return TYPE_MEM_POOL_ALLOC(LVRange, &LVRange::mempool_);
}

void LVRange::operator delete (void *p) {
  FmtAssert(0, ("LVRange delete operator should never be called"));
}

LRange *LVRange::makeRangeValue(INT64 i) const { 
  LRange *r = new LVRange(Range(i));
  return r;
}

LRange *LVRange::makeRangeMinMax(ZInt min, ZInt max) const { 
  LRange *r = new LVRange(Range(min, max));
  return r;
}

LRange *LVRange::makeRangeWidth (INT Sign, INT bitwidth) const {
  LRange *r = new LVRange(Range((Range::RangeSign)Sign, bitwidth));
  return r;
}

LRange *LVRange::makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const {
  LRange *r = new LVRange(Range((Range::RangeSign)Sign, lowbit, bitwidth));
  return r;
}

LRange *LVRange::makeUniverse () const { 
  LRange *r = new LVRange(Range::Universe ());
  return r;
}

LRange *LVRange::makeTop () const { 
  LRange *r = new LVRange(Range::Empty ());
  return r;
}

LRange *LVRange::makeBottom () const { 
  LRange *r = new LVRange(Range::Universe ());
  return r;
}

LRange *LVRange::makeMeet (const LRange *a, const LRange *b) const { 
  const LVRange *vrange1 = LRANGE_CAST(const LVRange *, a);
  const LVRange *vrange2 = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Union (vrange1->range_, vrange2->range_));
  return r;
}

LRange *LVRange::makeJoin (const LRange *a, const LRange *b) const { 
  const LVRange *vrange1 = LRANGE_CAST(const LVRange *, a);
  const LVRange *vrange2 = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Intersect (vrange1->range_, vrange2->range_));
  return r;
}


// queries
LRange *LVRange::makeSignExtend (INT width) const {
  LRange *r = new LVRange(SignExtend (this->range_, width));
  return r;
}
LRange *LVRange::makeZeroExtend (INT width) const {
  LRange *r = new LVRange(ZeroExtend (this->range_, width));
  return r;
}
LRange *LVRange::makeLeftShift (INT count) const {
  LRange *r = new LVRange(LeftShift (this->range_, count));
  return r;
}
LRange *LVRange::makeLeftShiftRange (const LRange *count) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, count);
  LRange *r = new LVRange(LeftShift (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeSignedRightShift (INT count) const {
  LRange *r = new LVRange(SignedRightShift (this->range_, count));
  return r;
}
LRange *LVRange::makeSignedRightShiftRange (const LRange *count) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, count);
  LRange *r = new LVRange(SignedRightShift (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeModulo (INT width) const {
  LRange *r = new LVRange(Modulo (this->range_, width));
  return r;
}
LRange *LVRange::makeAdd (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Add (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeAddModulo (const LRange *b, INT width) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(AddModulo (this->range_, vrange->range_, width));
  return r;
}
LRange *LVRange::makeSub (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Sub (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeSubModulo (const LRange *b, INT width) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(SubModulo (this->range_, vrange->range_, width));
  return r;
}
LRange *LVRange::makeMul (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Mul (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeMulModulo (const LRange *b, INT width) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(MulModulo (this->range_, vrange->range_, width));
  return r;
}
LRange *LVRange::makeMin (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Min (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeMax (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Max (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeBitAnd (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(BitAnd (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeBitOr (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(BitOr (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeBitXor (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(BitXor (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeBitNot () const {
  LRange *r = new LVRange(BitNot (this->range_));
  return r;
}
LRange *LVRange::makeEq (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Eq (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeNe (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Ne (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeGe (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Ge (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeGt (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Gt (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeLe (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Le (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeLt (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Lt (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeAndl (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Andl (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeNandl (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Nandl (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeOrl (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Orl (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeNorl (const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Norl (this->range_, vrange->range_));
  return r;
}
LRange *LVRange::makeNotl () const {
  LRange *r = new LVRange(Notl (this->range_));
  return r;
}
LRange *LVRange::makeHighestSetBit () const {
  LRange *r = new LVRange(HighestSetBit (this->range_));
  return r;
}
/*   LRange *LVRange::makeByteSwap () const { */
/*     LRange *r = new LVRange(ByteSwap (this->range_)); */
/*     return r; */
/*   } */
LRange *LVRange::makeExtract (INT start, INT width) const {
  LRange *r = new LVRange(Extract (this->range_, start, width));
  return r;
}
LRange *LVRange::makeInsert (INT start, INT width,
				     const LRange *b) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, b);
  LRange *r = new LVRange(Insert (this->range_, start, width, vrange->range_));
  return r;
}
LRange *LVRange::makeMakeUnsigned (INT width) const {
  LRange *r = new LVRange(MakeUnsigned (this->range_, width));
  return r;
}

LRange *LVRange::makeMakeSigned (INT width) const {
  LRange *r = new LVRange(MakeSigned (this->range_, width));
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
  const LVRange *vrange = LRANGE_CAST(const LVRange *, a);
  return range_.StrictlyContains (vrange->range_);
}

BOOL LVRange::Equal (const LRange *a) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, a);
  return range_.Equal (vrange->range_);
}

BOOL LVRange::ContainsOrEqual (const LRange *a) const {
  const LVRange *vrange = LRANGE_CAST(const LVRange *, a);
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
LRange *LVRange::clone(void) const { return new LVRange(*this); }

// instance
LRange *LVRange::getInstance(void) { return new LVRange(); }

// mempool initialization
void LVRange::MEMPOOL_Initialize(MEM_POOL mempool) {LVRange::mempool_ = mempool;}
