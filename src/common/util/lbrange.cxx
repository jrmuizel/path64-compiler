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
 * Module: lbrange.cxx
 *
 * Description:
 *
 * Implementation of used-bit range type.
 *
 * ====================================================================
 * ====================================================================
 */
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "lbrange.h"


MEM_POOL LBRange::mempool_;

// new and delete overriding
void* LBRange::operator new (size_t size) {
  FmtAssert(&LBRange::mempool_ != NULL, ("LBRange mempool not initialized"));
  return TYPE_MEM_POOL_ALLOC(LBRange, &LBRange::mempool_);
}

void LBRange::operator delete (void *p) {
  FmtAssert(0, ("LBRange delete operator should never be called"));
}

LRange *LBRange::makeRangeBitWidth (INT bitwidth) const {
  LRange *r = new LBRange(LBitMask(bitwidth));
  return r;
}

LRange *LBRange::makeRangeValue (INT64 value) const {
  LRange *r = new LBRange(LBitMask(value));
  return r;
}

LRange *LBRange::makeRangeLowBitWidth (INT lowbit, INT bitwidth) const {
  LRange *r = new LBRange(LBitMask(lowbit, bitwidth));
  return r;
}


LRange *LBRange::makeUniverse () const { 
  LRange *r = new LBRange(brange_.Bottom ());
  return r;
}

LRange *LBRange::makeTop () const { 
  LRange *r = new LBRange(brange_.Top ());
  return r;
}

LRange *LBRange::makeBottom () const { 
  LRange *r = new LBRange(brange_.Bottom ());
  return r;
}

LRange *LBRange::makeMeet (const LRange *a, const LRange *b) const { 
  const LBRange *vrange1 = LRANGE_CAST(const LBRange *, a);
  const LBRange *vrange2 = LRANGE_CAST(const LBRange *, b);
  LRange *r = new LBRange(Meet (vrange1->brange_, vrange2->brange_));
  return r;
}

LRange *LBRange::makeJoin (const LRange *a, const LRange *b) const { 
  const LBRange *vrange1 = LRANGE_CAST(const LBRange *, a);
  const LBRange *vrange2 = LRANGE_CAST(const LBRange *, b);
  LRange *r = new LBRange(Join (vrange1->brange_, vrange2->brange_));
  return r;
}

// queries

BOOL LBRange::isTop () const {
  return brange_.isTop ();
}

BOOL LBRange::StrictlyContains (const LRange *a) const {
  const LBRange *vrange = LRANGE_CAST(const LBRange *, a);
  return brange_.StrictlyContains (vrange->brange_);
}

BOOL LBRange::Equal (const LRange *a) const {
  const LBRange *vrange = LRANGE_CAST(const LBRange *, a);
  return brange_.Equal (vrange->brange_);
}

BOOL LBRange::ContainsOrEqual (const LRange *a) const {
  const LBRange *vrange = LRANGE_CAST(const LBRange *, a);
  return brange_.ContainsOrEqual (vrange->brange_);
}

INT LBRange::bits() const {
  return brange_.bits();
}

UINT64 LBRange::getBitMask() const {
  return brange_.getbitmask();
}

LRange *LBRange::makeMakeUnsigned (INT width) const {
  LRange *r = new LBRange(MakeUnsigned (this->brange_, width));
  return r;
}

LRange *LBRange::makeLeftShift (INT width) const {
  LRange *r = new LBRange(LeftShift (this->brange_, width));
  return r;
}

void LBRange::Print (FILE *f) const {
  brange_.Print (f);
}

// cloning
LRange *LBRange::clone(void) const { return new LBRange(*this); }

// instance
LRange *LBRange::getInstance(void) { return new LBRange(); }

// mempool initialization
void LBRange::MEMPOOL_Initialize(MEM_POOL mempool) {LBRange::mempool_ = mempool;}
