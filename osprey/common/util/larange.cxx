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
#include "larange.h"


MEM_POOL LARange::mempool_;

// new and delete overriding
void* LARange::operator new (size_t size) {
  FmtAssert(&LARange::mempool_ != NULL, ("LARange mempool not initialized"));
  return TYPE_MEM_POOL_ALLOC(LARange, &LARange::mempool_);
}

void LARange::operator delete (void *p) {
  FmtAssert(0, ("LARange delete operator should never be called"));
}

LRange *LARange::makeRangeValue (INT64 value) const {
  LRange *r = new LARange(LAlign(value));
  return r;
}

LRange *LARange::makeAlign (ZInt base, ZInt bias) const {
  LRange *r = new LARange(LAlign(base, bias).Normalize ());
  return r;
}


LRange *LARange::makeUniverse () const { 
  LRange *r = new LARange(arange_.Bottom ());
  return r;
}

LRange *LARange::makeTop () const { 
  LRange *r = new LARange(arange_.Top ());
  return r;
}

LRange *LARange::makeBottom () const { 
  LRange *r = new LARange(arange_.Bottom ());
  return r;
}

LRange *LARange::makeMeet (const LRange *a, const LRange *b) const { 
  const LARange *vrange1 = LRANGE_CAST(const LARange *, a);
  const LARange *vrange2 = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Meet (vrange1->arange_, vrange2->arange_));
  return r;
}

LRange *LARange::makeJoin (const LRange *a, const LRange *b) const { 
  const LARange *vrange1 = LRANGE_CAST(const LARange *, a);
  const LARange *vrange2 = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Join (vrange1->arange_, vrange2->arange_));
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
  const LARange *vrange = LRANGE_CAST(const LARange *, a);
  return arange_.StrictlyContains (vrange->arange_);
}

BOOL LARange::Equal (const LRange *a) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, a);
  return arange_.Equal (vrange->arange_);
}

BOOL LARange::ContainsOrEqual (const LRange *a) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, a);
  return arange_.ContainsOrEqual (vrange->arange_);
}

LRange *LARange::makeLeftShift (INT width) const {
  LRange *r = new LARange(LeftShift (this->arange_, width));
  return r;
}

LRange *LARange::makeLeftShiftRange (const LRange *count) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, count);
  LRange *r = new LARange(LeftShiftRange (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeSignedRightShift (INT width) const {
  LRange *r = new LARange(RightShift (this->arange_, width));
  return r;
}

LRange *LARange::makeSignedRightShiftRange (const LRange *count) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, count);
  LRange *r = new LARange(RightShiftRange (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeMakeSigned (INT width) const {
  LRange *r = new LARange(MakeSigned (this->arange_, width));
  return r;
}

LRange *LARange::makeMakeUnsigned (INT width) const {
  LRange *r = new LARange(MakeUnsigned (this->arange_, width));
  return r;
}

LRange *LARange::makeBitAnd (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(BitAnd (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeBitOr (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(BitOr (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeAdd (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Add (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeSub (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Sub (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeMul (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Mul (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeAddModulo (const LRange *b, INT width) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Add (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeSubModulo (const LRange *b, INT width) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Sub (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeMulModulo (const LRange *b, INT width) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Mul (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeDiv (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Div (this->arange_, vrange->arange_));
  return r;
}

LRange *LARange::makeMod (const LRange *b) const {
  const LARange *vrange = LRANGE_CAST(const LARange *, b);
  LRange *r = new LARange(Mod (this->arange_, vrange->arange_));
  return r;
}

void LARange::Print (FILE *f) const {
  arange_.Print (f);
}

// cloning
LRange *LARange::clone(void) const { return new LARange(*this); }

// instance
LRange *LARange::getInstance(void) { return new LARange(); }

// mempool initialization
void LARange::MEMPOOL_Initialize(MEM_POOL mempool) {LARange::mempool_ = mempool;}
