/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

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
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "lbvalrange.h"


MEM_POOL LBValRange::mempool_;

// new and delete overriding
void* LBValRange::operator new (size_t size) {
  FmtAssert(&LBValRange::mempool_ != NULL, ("LBValRange mempool not initialized"));
  return TYPE_MEM_POOL_ALLOC(LBValRange, &LBValRange::mempool_);
}

void LBValRange::operator delete (void *p) {
  FmtAssert(0, ("LBValRange delete operator should never be called"));
}

LRange *LBValRange::makeRangeValue (INT64 value) const {
  LRange *r = new LBValRange(LBitValue(value));
  return r;
}

LRange *LBValRange::makeBitValue (UINT64 zeromask, UINT64 onemask) const {
  LRange *r = new LBValRange(LBitValue(zeromask, onemask));
  return r;
}

LRange *LBValRange::makeRangeWidth (INT sign, INT bitwidth) const {
  LRange *r = new LBValRange(LBitValue((LBitValue::RangeSign)sign, bitwidth));
  return r;
}

LRange *LBValRange::makeUniverse () const { 
  LRange *r = new LBValRange(bvalrange_.Bottom ());
  return r;
}

LRange *LBValRange::makeTop () const { 
  LRange *r = new LBValRange(bvalrange_.Top ());
  return r;
}

LRange *LBValRange::makeBottom () const { 
  LRange *r = new LBValRange(bvalrange_.Bottom ());
  return r;
}

LRange *LBValRange::makeMeet (const LRange *a, const LRange *b) const { 
  const LBValRange *vrange1 = LRANGE_CAST(const LBValRange *, a);
  const LBValRange *vrange2 = LRANGE_CAST(const LBValRange *, b);
  LRange *r = new LBValRange(Meet (vrange1->bvalrange_, vrange2->bvalrange_));
  return r;
}

LRange *LBValRange::makeJoin (const LRange *a, const LRange *b) const { 
  const LBValRange *vrange1 = LRANGE_CAST(const LBValRange *, a);
  const LBValRange *vrange2 = LRANGE_CAST(const LBValRange *, b);
  LRange *r = new LBValRange(Join (vrange1->bvalrange_, vrange2->bvalrange_));
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
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, a);
  return bvalrange_.StrictlyContains (vrange->bvalrange_);
}

BOOL LBValRange::Equal (const LRange *a) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, a);
  return bvalrange_.Equal (vrange->bvalrange_);
}

BOOL LBValRange::ContainsOrEqual (const LRange *a) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, a);
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

LRange *LBValRange::makeLeftShift (INT width) const {
  LRange *r = new LBValRange(LeftShift (this->bvalrange_, width));
  return r;
}

LRange *LBValRange::makeLeftShiftRange (const LRange *count) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, count);
  LRange *r = new LBValRange(LeftShiftRange (this->bvalrange_, vrange->bvalrange_));
  return r;
}

LRange *LBValRange::makeSignedRightShift (INT width) const {
  LRange *r = new LBValRange(RightShift (this->bvalrange_, width));
  return r;
}

LRange *LBValRange::makeSignedRightShiftRange (const LRange *count) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, count);
  LRange *r = new LBValRange(RightShiftRange (this->bvalrange_, vrange->bvalrange_));
  return r;
}

LRange *LBValRange::makeMakeSigned (INT width) const {
  LRange *r = new LBValRange(MakeSigned (this->bvalrange_, width));
  return r;
}

LRange *LBValRange::makeMakeUnsigned (INT width) const {
  LRange *r = new LBValRange(MakeUnsigned (this->bvalrange_, width));
  return r;
}

LRange *LBValRange::makeSignExtend (INT width) const {
  LRange *r = new LBValRange(SignExtend (this->bvalrange_, width));
  return r;
}

LRange *LBValRange::makeZeroExtend (INT width) const {
  LRange *r = new LBValRange(ZeroExtend (this->bvalrange_, width));
  return r;
}

LRange *LBValRange::makeBitAnd (const LRange *b) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, b);
  LRange *r = new LBValRange(BitAnd (this->bvalrange_, vrange->bvalrange_));
  return r;
}

LRange *LBValRange::makeBitOr (const LRange *b) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, b);
  LRange *r = new LBValRange(BitOr (this->bvalrange_, vrange->bvalrange_));
  return r;
}

LRange *LBValRange::makeBitXor (const LRange *b) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, b);
  LRange *r = new LBValRange(BitXor (this->bvalrange_, vrange->bvalrange_));
  return r;
}

LRange *LBValRange::makeBitNot () const {
  LRange *r = new LBValRange(BitNot (this->bvalrange_));
  return r;
}

LRange *LBValRange::makeBytePermute (INT mask) const {
  LRange *r = new LBValRange(BytePermute (this->bvalrange_, mask));
  return r;
}

LRange *LBValRange::makeExtract (INT start, INT width) const {
  LRange *r = new LBValRange(Extract (this->bvalrange_, start, width));
  return r;
}

LRange *LBValRange::makeInsert (INT start, INT width,
				     const LRange *b) const {
  const LBValRange *vrange = LRANGE_CAST(const LBValRange *, b);
  LRange *r = new LBValRange(Insert (this->bvalrange_, start, width, vrange->bvalrange_));
  return r;
}

void LBValRange::Print (FILE *f) const {
  bvalrange_.Print (f);
}

// cloning
LRange *LBValRange::clone(void) const { return new LBValRange(*this); }

// instance
LRange *LBValRange::getInstance(void) { return new LBValRange(); }

// mempool initialization
void LBValRange::MEMPOOL_Initialize(MEM_POOL mempool) {LBValRange::mempool_ = mempool;}
