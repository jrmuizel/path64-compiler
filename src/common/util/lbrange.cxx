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
 * Module: lbrange.cxx
 *
 * Description:
 *
 * Implementation of used-bit range type.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "cxx_memory.h"
#include "lbrange.h"


MEM_POOL LBRange::mempool_;
LBRange::LBRange *LBRange::top_ = new LBRange(LBitMask::Top ()) ; 
LBRange::LBRange *LBRange::bottom_ = new LBRange(LBitMask::Bottom ()) ;
LBRange::LBRange *LBRange::universe_ = new LBRange(LBitMask::Bottom ()) ;


LBRange *LBRange::makeRangeBitWidth (INT bitwidth) const {
  LBRange *r = CXX_NEW (LBRange(LBitMask(bitwidth)), &mempool_);
  return r;
}

LBRange *LBRange::makeRangeValue (INT64 value) const {
  LBRange *r = CXX_NEW (LBRange(LBitMask(value)), &mempool_);
  return r;
}

LBRange *LBRange::makeRangeLowBitWidth (INT lowbit, INT bitwidth) const {
  LBRange *r = CXX_NEW (LBRange(LBitMask(lowbit, bitwidth)), &mempool_);
  return r;
}


LBRange *LBRange::makeUniverse () const { 
  Is_True(universe_, ("%s : unexpected uninitialized universe", __PRETTY_FUNCTION__));
  return universe_;
}

LBRange *LBRange::makeTop () const { 
  Is_True(top_, ("%s : unexpected uninitialized top", __PRETTY_FUNCTION__));
  return top_;
}

LBRange *LBRange::makeBottom () const { 
  Is_True(bottom_, ("%s : unexpected uninitialized bottom", __PRETTY_FUNCTION__));
  return bottom_;
}

LBRange *LBRange::makeMeet (const LRange *a, const LRange *b) const { 
  const LBRange *vrange1 = static_cast<const LBRange *>(a);
  const LBRange *vrange2 = static_cast<const LBRange *>(b);
  LBRange *r = CXX_NEW (LBRange(Meet (vrange1->brange_, vrange2->brange_)), &mempool_);
  return r;
}

LBRange *LBRange::makeJoin (const LRange *a, const LRange *b) const { 
  const LBRange *vrange1 = static_cast<const LBRange *>(a);
  const LBRange *vrange2 = static_cast<const LBRange *>(b);
  LBRange *r = CXX_NEW (LBRange(Join (vrange1->brange_, vrange2->brange_)), &mempool_);
  return r;
}

// queries

BOOL LBRange::isTop () const {
  return brange_.isTop ();
}

BOOL LBRange::StrictlyContains (const LRange *a) const {
  const LBRange *vrange = static_cast<const LBRange *>(a);
  return brange_.StrictlyContains (vrange->brange_);
}

BOOL LBRange::Equal (const LRange *a) const {
  const LBRange *vrange = static_cast<const LBRange *>(a);
  return brange_.Equal (vrange->brange_);
}

BOOL LBRange::ContainsOrEqual (const LRange *a) const {
  const LBRange *vrange = static_cast<const LBRange *>(a);
  return brange_.ContainsOrEqual (vrange->brange_);
}

INT LBRange::bits() const {
  return brange_.bits();
}

UINT64 LBRange::getBitMask() const {
  return brange_.getbitmask();
}

LBRange *LBRange::makeMakeUnsigned (INT width) const {
  LBRange *r = CXX_NEW (LBRange(MakeUnsigned (this->brange_, width)), &mempool_);
  return r;
}

LBRange *LBRange::makeLeftShift (INT width) const {
  LBRange *r = CXX_NEW (LBRange(LeftShift (this->brange_, width)), &mempool_);
  return r;
}

void LBRange::Print (FILE *f) const {
  brange_.Print (f);
}

// cloning
LBRange *LBRange::clone(void) const { return CXX_NEW (LBRange(*this), &mempool_); }

// instance
LBRange *LBRange::getInstance(void) { return CXX_NEW (LBRange(), &mempool_); }

// mempool initialization
void LBRange::MEMPOOL_Initialize(MEM_POOL mempool) {LBRange::mempool_ = mempool;}
