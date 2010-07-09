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
 * Module: lbvalrange.h
 *
 * Description:
 *
 * Implement a derived class of LRange for bit-value range type analysis, 
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LBVALRANGE_H_INCLUDED
#define LBVALRANGE_H_INCLUDED

#include "mempool.h"
#include "lrange.h"
#include "lbitvalue.h"
#include <assert.h>

class LBValRange: public LRange {
private:
  LBitValue bvalrange_;
  static MEM_POOL mempool_;
  static LBValRange *top_ ; 
  static LBValRange *bottom_ ; 
  static LBValRange *universe_ ;
public:
  // constructors
  LBValRange(const LBValRange &that) { *this = that; }
  LBValRange(const LBitValue &that): bvalrange_(that) { }
  LBValRange(): bvalrange_(LBitValue::Bottom()) {}
  virtual ~LBValRange() {} 

  static LBitValue Top(){ return LBitValue::Top();}
  static LBitValue Bottom(){ return LBitValue::Bottom();}
  static LBitValue Universe(){ return LBitValue::Bottom();}

  virtual LBValRange *makeRangeValue (INT64 value) const;
  virtual LBValRange *makeBitValue (UINT64 zeromask, UINT64 onemask) const;
  virtual LBValRange *makeRangeWidth (INT Sign, INT bitwidth) const;
  virtual LBValRange *makeUniverse () const;
  virtual LBValRange *makeTop () const;
  virtual LBValRange *makeBottom () const;
  virtual LBValRange *makeMeet (const LRange *a, const LRange *b) const;
  virtual LBValRange *makeJoin (const LRange *a, const LRange *b) const;
  // queries

  virtual BOOL hasValue(void) const;
  virtual INT64 getValue(void) const;
  virtual BOOL isTop () const;
  virtual BOOL StrictlyContains (const LRange *a) const;
  virtual BOOL Equal (const LRange *a) const;
  virtual BOOL ContainsOrEqual (const LRange *a) const;
  virtual INT bits() const;
  virtual UINT64 getZeroMask() const;
  virtual UINT64 getOneMask() const;
  virtual UINT64 getValMask() const;
  virtual INT getTzcnt() const;
  virtual LBValRange *makeMakeSigned (INT width) const;
  virtual LBValRange *makeMakeUnsigned (INT width) const;
  virtual LBValRange *makeSignExtend (INT width) const;
  virtual LBValRange *makeZeroExtend (INT width) const;
  virtual LBValRange *makeLeftShift (INT width) const;
  virtual LBValRange *makeLeftShiftRange (const LRange *count) const;
  virtual LBValRange *makeSignedRightShift (INT width) const;
  virtual LBValRange *makeSignedRightShiftRange (const LRange *count) const;
  virtual LBValRange *makeBitAnd (const LRange *b) const;
  virtual LBValRange *makeBitOr (const LRange *b) const;
  virtual LBValRange *makeBitXor (const LRange *b) const;
  virtual LBValRange *makeBitNot () const;
  virtual LBValRange *makeBytePermute (INT mask) const;
  virtual LBValRange *makeExtract (INT start, INT width) const;
  virtual LBValRange *makeInsert (INT start, INT width,
			      const LRange *b) const;

  virtual void Print (FILE *f) const;


  // cloning
  virtual LBValRange *clone(void) const;

  // instance
  BE_EXPORTED  static LBValRange *getInstance(void);

  // mempool initialization
  BE_EXPORTED  static void MEMPOOL_Initialize(MEM_POOL mempool);

  // mempool visibility
  virtual MEM_POOL *Mem_pool() { return &mempool_; }
};

#endif /*LBVALRANGE_H_INCLUDED */
