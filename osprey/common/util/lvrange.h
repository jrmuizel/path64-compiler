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
 * Module: lvrange.h
 *
 * Description:
 *
 * Implement a derived class of LRange for value range type analysis, 
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LVRANGE_H_INCLUDED
#define LVRANGE_H_INCLUDED

#include "mempool.h"
#include "lrange.h"
#include "zint.h"
#include "range.h"
#include <assert.h>

class LVRange: public LRange {
private:
  Range range_;
  static MEM_POOL mempool_;
//   // constructors
//   LVRange(const LVRange &that) { *this = that; }
//   LVRange(const Range &that): range_(that) { }
//   LVRange(): range_(Range::Universe()) {}
public:
  // constructors
  LVRange(const LVRange &that) { *this = that; }
  LVRange(const Range &that): range_(that) { }
  LVRange(): range_(Range::Universe()) {}

  static Range Bottom(){ return Range::Universe();}

  virtual LRange *makeRangeValue(INT64 i) const;

  virtual LRange *makeRangeMinMax(ZInt min, ZInt max) const;

  virtual LRange *makeRangeWidth (INT Sign, INT bitwidth) const;

  virtual LRange *makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const;

  virtual LRange *makeUniverse () const;

  virtual LRange *makeTop () const;

  virtual LRange *makeBottom () const;

  virtual LRange *makeMeet (const LRange *a, const LRange *b) const;

  virtual LRange *makeJoin (const LRange *a, const LRange *b) const;

  // queries
  virtual LRange *makeSignExtend (INT width) const;
  virtual LRange *makeZeroExtend (INT width) const;
  virtual LRange *makeLeftShift (INT count) const;
  virtual LRange *makeLeftShiftRange (const LRange *count) const;
  virtual LRange *makeSignedRightShift (INT count) const;
  virtual LRange *makeSignedRightShiftRange (const LRange *count) const;
  virtual LRange *makeModulo (INT width) const;
  virtual LRange *makeAdd (const LRange *b) const;
  virtual LRange *makeAddModulo (const LRange *b, INT width) const;
  virtual LRange *makeSub (const LRange *b) const;
  virtual LRange *makeSubModulo (const LRange *b, INT width) const;
  virtual LRange *makeMul (const LRange *b) const;
  virtual LRange *makeMulModulo (const LRange *b, INT width) const;
  virtual LRange *makeMin (const LRange *b) const;
  virtual LRange *makeMax (const LRange *b) const;
  virtual LRange *makeBitAnd (const LRange *b) const;
  virtual LRange *makeBitOr (const LRange *b) const;
  virtual LRange *makeBitXor (const LRange *b) const;
  virtual LRange *makeBitNot () const;
  virtual LRange *makeEq (const LRange *b) const;
  virtual LRange *makeNe (const LRange *b) const;
  virtual LRange *makeGe (const LRange *b) const;
  virtual LRange *makeGt (const LRange *b) const;
  virtual LRange *makeLe (const LRange *b) const;
  virtual LRange *makeLt (const LRange *b) const;
  virtual LRange *makeAndl (const LRange *b) const;
  virtual LRange *makeNandl (const LRange *b) const;
  virtual LRange *makeOrl (const LRange *b) const;
  virtual LRange *makeNorl (const LRange *b) const;
  virtual LRange *makeNotl () const;
  virtual LRange *makeHighestSetBit () const;
/*   virtual LRange *makeByteSwap () const { */
/*     LRange *r = TYPE_MEM_POOL_ALLOC (LRange, &mempool_); r = LVRange(ByteSwap (this->range_)); */
/*     return r; */
/*   } */
  virtual LRange *makeExtract (INT start, INT width) const;
  virtual LRange *makeInsert (INT start, INT width,
			      const LRange *b) const;
  virtual LRange *makeMakeUnsigned (INT width) const;
  virtual LRange *makeMakeSigned (INT width) const;
  virtual BOOL hasValue(void) const;
  virtual INT64 getValue(void) const;


  virtual BOOL isTop () const;
  virtual BOOL StrictlyContains (const LRange *a) const;
  virtual BOOL Equal (const LRange *a) const;
  virtual BOOL ContainsOrEqual (const LRange *a) const;
  virtual BOOL isNonZero () const;
  virtual BOOL isNonNegative () const;
  virtual BOOL isZero () const;
  virtual INT bits() const;
  virtual void Print (FILE *f) const;

  // cloning
  virtual LRange *clone(void) const;

  // instance
  BE_EXPORTED  static LRange *getInstance(void);

  static void* operator new (size_t size); 
  static void operator delete (void *p);

  // mempool initialization
  BE_EXPORTED static void MEMPOOL_Initialize(MEM_POOL mempool);
};

#endif /*LVRANGE_H_INCLUDED */
