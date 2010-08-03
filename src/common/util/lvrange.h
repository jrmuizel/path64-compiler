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
  static LVRange *top_ ; 
  static LVRange *bottom_ ; 
  static LVRange *universe_ ;
//   // constructors
//   LVRange(const LVRange &that) { *this = that; }
//   LVRange(const Range &that): range_(that) { }
//   LVRange(): range_(Range::Universe()) {}
public:
  // constructors
  LVRange(const LVRange &that) { *this = that; }
  LVRange(const Range &that): range_(that) { }
  LVRange(): range_(Range::Universe()) {}
  virtual ~LVRange() {} 

  static Range Top(){ return Range::Empty();}
  static Range Bottom(){ return Range::Universe();}
  static Range Universe(){ return Range::Universe();}

  virtual LVRange *makeRangeValue(INT64 i) const;

  virtual LVRange *makeRangeMinMax(ZInt min, ZInt max) const;

  virtual LVRange *makeRangeWidth (INT Sign, INT bitwidth) const;

  virtual LVRange *makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const;

  virtual LVRange *makeUniverse () const;

  virtual LVRange *makeTop () const;

  virtual LVRange *makeBottom () const;

  virtual LVRange *makeMeet (const LRange *a, const LRange *b) const;

  virtual LVRange *makeJoin (const LRange *a, const LRange *b) const;

  // queries
  virtual LVRange *makeSignExtend (INT width) const;
  virtual LVRange *makeZeroExtend (INT width) const;
  virtual LVRange *makeLeftShift (INT count) const;
  virtual LVRange *makeLeftShiftRange (const LRange *count) const;
  virtual LVRange *makeSignedRightShift (INT count) const;
  virtual LVRange *makeSignedRightShiftRange (const LRange *count) const;
  virtual LVRange *makeModulo (INT width) const;
  virtual LVRange *makeAdd (const LRange *b) const;
  virtual LVRange *makeAddModulo (const LRange *b, INT width) const;
  virtual LVRange *makeSub (const LRange *b) const;
  virtual LVRange *makeSubModulo (const LRange *b, INT width) const;
  virtual LVRange *makeMul (const LRange *b) const;
  virtual LVRange *makeMulModulo (const LRange *b, INT width) const;
  virtual LVRange *makeMin (const LRange *b) const;
  virtual LVRange *makeMax (const LRange *b) const;
  virtual LVRange *makeBitAnd (const LRange *b) const;
  virtual LVRange *makeBitOr (const LRange *b) const;
  virtual LVRange *makeBitXor (const LRange *b) const;
  virtual LVRange *makeBitNot () const;
  virtual LVRange *makeEq (const LRange *b) const;
  virtual LVRange *makeNe (const LRange *b) const;
  virtual LVRange *makeGe (const LRange *b) const;
  virtual LVRange *makeGt (const LRange *b) const;
  virtual LVRange *makeLe (const LRange *b) const;
  virtual LVRange *makeLt (const LRange *b) const;
  virtual LVRange *makeAndl (const LRange *b) const;
  virtual LVRange *makeNandl (const LRange *b) const;
  virtual LVRange *makeOrl (const LRange *b) const;
  virtual LVRange *makeNorl (const LRange *b) const;
  virtual LVRange *makeNotl () const;
  virtual LVRange *makeHighestSetBit () const;
/*   virtual LVRange *makeByteSwap () const { */
/*     LVRange *r = TYPE_MEM_POOL_ALLOC (LRange, &mempool_); r = LVRange(ByteSwap (this->range_)); */
/*     return r; */
/*   } */
  virtual LVRange *makeExtract (INT start, INT width) const;
  virtual LVRange *makeInsert (INT start, INT width,
			      const LRange *b) const;
  virtual LVRange *makeMakeUnsigned (INT width) const;
  virtual LVRange *makeMakeSigned (INT width) const;
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
  virtual LVRange *clone(void) const;

  // instance
  BE_EXPORTED  static LVRange *getInstance(void);

  // mempool initialization
  BE_EXPORTED static void MEMPOOL_Initialize(MEM_POOL mempool);

  // mempool visibility
  virtual MEM_POOL *Mem_pool() { return &mempool_; }
};

#endif /*LVRANGE_H_INCLUDED */
