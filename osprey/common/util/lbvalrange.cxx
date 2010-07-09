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
 * Module: lbvalrange.cxx
 *
 * Description:
 *
 * Implementation of lattice bit-value range type.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "cxx_memory.h"
#include "lbvalrange.h"


MEM_POOL LBValRange::mempool_;
LBValRange::LBValRange *LBValRange::top_ = new LBValRange(LBitValue::Top ()); 
LBValRange::LBValRange *LBValRange::bottom_ = new LBValRange(LBitValue::Bottom ()); 
LBValRange::LBValRange *LBValRange::universe_ = new LBValRange(LBitValue::Bottom ());

LBValRange *LBValRange::makeRangeValue (INT64 value) const {
  LBValRange *r = CXX_NEW (LBValRange(LBitValue(value)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeBitValue (UINT64 zeromask, UINT64 onemask) const {
  LBValRange *r = CXX_NEW (LBValRange(LBitValue(zeromask, onemask)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeRangeWidth (INT sign, INT bitwidth) const {
  LBValRange *r = CXX_NEW (LBValRange(LBitValue((LBitValue::RangeSign)sign, bitwidth)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeUniverse () const {
  Is_True(universe_, ("%s : unexpected uninitialized universe", __PRETTY_FUNCTION__));
  return universe_;
}

LBValRange *LBValRange::makeTop () const { 
  Is_True(top_, ("%s : unexpected uninitialized top", __PRETTY_FUNCTION__));
  return top_;
}

LBValRange *LBValRange::makeBottom () const { 
  Is_True(bottom_, ("%s : unexpected uninitialized bottom", __PRETTY_FUNCTION__));
  return bottom_;
}

LBValRange *LBValRange::makeMeet (const LRange *a, const LRange *b) const { 
  const LBValRange *vrange1 = static_cast<const LBValRange *>(a);
  const LBValRange *vrange2 = static_cast<const LBValRange *>(b);
  LBValRange *r = CXX_NEW (LBValRange(Meet (vrange1->bvalrange_, vrange2->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeJoin (const LRange *a, const LRange *b) const { 
  const LBValRange *vrange1 = static_cast<const LBValRange *>(a);
  const LBValRange *vrange2 = static_cast<const LBValRange *>(b);
  LBValRange *r = CXX_NEW (LBValRange(Join (vrange1->bvalrange_, vrange2->bvalrange_)), &mempool_);
  return r;
}

// queries


BOOL LBValRange::hasValue(void) const { 
  return bvalrange_.hasValue (); 
}

INT64 LBValRange::getValue(void) const { 
  return bvalrange_.getValue (); 
}

BOOL LBValRange::isTop () const {
  return bvalrange_.isTop ();
}

BOOL LBValRange::StrictlyContains (const LRange *a) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(a);
  return bvalrange_.StrictlyContains (vrange->bvalrange_);
}

BOOL LBValRange::Equal (const LRange *a) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(a);
  return bvalrange_.Equal (vrange->bvalrange_);
}

BOOL LBValRange::ContainsOrEqual (const LRange *a) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(a);
  return bvalrange_.ContainsOrEqual (vrange->bvalrange_);
}

INT LBValRange::bits() const {
  return bvalrange_.bits();
}

UINT64 LBValRange::getZeroMask() const {
  return bvalrange_.getzeromask();
}

UINT64 LBValRange::getOneMask() const {
  return bvalrange_.getonemask();
}

UINT64 LBValRange::getValMask() const {
  return bvalrange_.getvalmask();
}

INT LBValRange::getTzcnt() const {
  return bvalrange_.gettzcnt();
}

LBValRange *LBValRange::makeLeftShift (INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(LeftShift (this->bvalrange_, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeLeftShiftRange (const LRange *count) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(count);
  LBValRange *r = CXX_NEW (LBValRange(LeftShiftRange (this->bvalrange_, vrange->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeSignedRightShift (INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(RightShift (this->bvalrange_, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeSignedRightShiftRange (const LRange *count) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(count);
  LBValRange *r = CXX_NEW (LBValRange(RightShiftRange (this->bvalrange_, vrange->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeMakeSigned (INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(MakeSigned (this->bvalrange_, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeMakeUnsigned (INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(MakeUnsigned (this->bvalrange_, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeSignExtend (INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(SignExtend (this->bvalrange_, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeZeroExtend (INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(ZeroExtend (this->bvalrange_, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeBitAnd (const LRange *b) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(b);
  LBValRange *r = CXX_NEW (LBValRange(BitAnd (this->bvalrange_, vrange->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeBitOr (const LRange *b) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(b);
  LBValRange *r = CXX_NEW (LBValRange(BitOr (this->bvalrange_, vrange->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeBitXor (const LRange *b) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(b);
  LBValRange *r = CXX_NEW (LBValRange(BitXor (this->bvalrange_, vrange->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeBitNot () const {
  LBValRange *r = CXX_NEW (LBValRange(BitNot (this->bvalrange_)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeBytePermute (INT mask) const {
  LBValRange *r = CXX_NEW (LBValRange(BytePermute (this->bvalrange_, mask)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeExtract (INT start, INT width) const {
  LBValRange *r = CXX_NEW (LBValRange(Extract (this->bvalrange_, start, width)), &mempool_);
  return r;
}

LBValRange *LBValRange::makeInsert (INT start, INT width,
				     const LRange *b) const {
  const LBValRange *vrange = static_cast<const LBValRange *>(b);
  LBValRange *r = CXX_NEW (LBValRange(Insert (this->bvalrange_, start, width, vrange->bvalrange_)), &mempool_);
  return r;
}

void LBValRange::Print (FILE *f) const {
  bvalrange_.Print (f);
}

// cloning
LBValRange *LBValRange::clone(void) const { return CXX_NEW (LBValRange(*this), &mempool_); }

// instance
LBValRange *LBValRange::getInstance(void) { return CXX_NEW (LBValRange(), &mempool_); }

// mempool initialization
void LBValRange::MEMPOOL_Initialize(MEM_POOL mempool) {LBValRange::mempool_ = mempool;}
