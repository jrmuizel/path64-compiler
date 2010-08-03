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
 * Module: larange.cxx
 *
 * Description:
 *
 * Implementation of lattice alignment range type.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "cxx_memory.h"
#include "larange.h"


MEM_POOL LARange::mempool_;
LARange::LARange *LARange::top_ = new LARange(LAlign::Top ()) ; 
LARange::LARange *LARange::bottom_ = new LARange(LAlign::Bottom ()) ;
LARange::LARange *LARange::universe_ = new LARange(LAlign::Bottom ()) ;

LARange *LARange::makeRangeValue (INT64 value) const {
  LARange *r = CXX_NEW (LARange(LAlign(value)), &mempool_);
  return r;
}

LARange *LARange::makeAlign (ZInt base, ZInt bias) const {
  LARange *r = CXX_NEW (LARange(LAlign(base, bias).Normalize ()), &mempool_);
  return r;
}


LARange *LARange::makeUniverse () const { 
  Is_True(universe_, ("%s : unexpected uninitialized universe", __PRETTY_FUNCTION__));
  return universe_;
}

LARange *LARange::makeTop () const {
  Is_True(top_, ("%s : unexpected uninitialized top", __PRETTY_FUNCTION__));
  return top_;
}

LARange *LARange::makeBottom () const { 
  Is_True(bottom_, ("%s : unexpected uninitialized bottom", __PRETTY_FUNCTION__));
  return bottom_;
}

LARange *LARange::makeMeet (const LRange *a, const LRange *b) const { 
  const LARange *vrange1 = static_cast<const LARange *>(a);
  const LARange *vrange2 = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Meet (vrange1->arange_, vrange2->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeJoin (const LRange *a, const LRange *b) const { 
  const LARange *vrange1 = static_cast<const LARange *>(a);
  const LARange *vrange2 = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Join (vrange1->arange_, vrange2->arange_)), &mempool_);
  return r;
}

// queries


BOOL LARange::hasValue(void) const { 
  return arange_.hasValue (); 
}

INT64 LARange::getValue(void) const { 
  return arange_.getValue (); 
}

UINT64 LARange::getBase(void) const { 
  return arange_.getbase (); 
}

UINT64 LARange::getBias(void) const { 
  return arange_.getbias (); 
}

BOOL LARange::isTop () const {
  return arange_.isTop ();
}

BOOL LARange::StrictlyContains (const LRange *a) const {
  const LARange *vrange = static_cast<const LARange *>(a);
  return arange_.StrictlyContains (vrange->arange_);
}

BOOL LARange::Equal (const LRange *a) const {
  const LARange *vrange = static_cast<const LARange *>(a);
  return arange_.Equal (vrange->arange_);
}

BOOL LARange::ContainsOrEqual (const LRange *a) const {
  const LARange *vrange = static_cast<const LARange *>(a);
  return arange_.ContainsOrEqual (vrange->arange_);
}

LARange *LARange::makeSignExtend (INT width) const {
  LARange *r = CXX_NEW (LARange(SignExtend (this->arange_, width)), &mempool_);
  return r;
}
LARange *LARange::makeZeroExtend (INT width) const {
  LARange *r = CXX_NEW (LARange(ZeroExtend (this->arange_, width)), &mempool_);
  return r;
}

LARange *LARange::makeLeftShift (INT width) const {
  LARange *r = CXX_NEW (LARange(LeftShift (this->arange_, width)), &mempool_);
  return r;
}

LARange *LARange::makeLeftShiftRange (const LRange *count) const {
  const LARange *vrange = static_cast<const LARange *>(count);
  LARange *r = CXX_NEW (LARange(LeftShiftRange (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeSignedRightShift (INT width) const {
  LARange *r = CXX_NEW (LARange(RightShift (this->arange_, width)), &mempool_);
  return r;
}

LARange *LARange::makeSignedRightShiftRange (const LRange *count) const {
  const LARange *vrange = static_cast<const LARange *>(count);
  LARange *r = CXX_NEW (LARange(RightShiftRange (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeMakeSigned (INT width) const {
  LARange *r = CXX_NEW (LARange(MakeSigned (this->arange_, width)), &mempool_);
  return r;
}

LARange *LARange::makeMakeUnsigned (INT width) const {
  LARange *r = CXX_NEW (LARange(MakeUnsigned (this->arange_, width)), &mempool_);
  return r;
}

LARange *LARange::makeBitAnd (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(BitAnd (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeBitOr (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(BitOr (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeAdd (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Add (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeSub (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Sub (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeMul (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Mul (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeAddModulo (const LRange *b, INT width) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Add (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeSubModulo (const LRange *b, INT width) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Sub (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeMulModulo (const LRange *b, INT width) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Mul (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeDiv (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Div (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

LARange *LARange::makeMod (const LRange *b) const {
  const LARange *vrange = static_cast<const LARange *>(b);
  LARange *r = CXX_NEW (LARange(Mod (this->arange_, vrange->arange_)), &mempool_);
  return r;
}

void LARange::Print (FILE *f) const {
  arange_.Print (f);
}

// cloning
LARange *LARange::clone(void) const { return CXX_NEW (LARange(*this), &mempool_); }

// instance
LARange *LARange::getInstance(void) { return CXX_NEW (LARange(), &mempool_); }

// mempool initialization
void LARange::MEMPOOL_Initialize(MEM_POOL mempool) {LARange::mempool_ = mempool;}
