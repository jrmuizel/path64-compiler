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
public:
  // constructors
  LBValRange(const LBValRange &that) { *this = that; }
  LBValRange(const LBitValue &that): bvalrange_(that) { }
  LBValRange(): bvalrange_(LBitValue::Bottom()) {}

  static LBitValue Bottom(){ return LBitValue::Bottom();}

  virtual LRange *makeRangeValue (INT64 value) const;
  virtual LRange *makeBitValue (UINT64 zeromask, UINT64 onemask) const;
  virtual LRange *makeRangeWidth (INT Sign, INT bitwidth) const;
  virtual LRange *makeUniverse () const;
  virtual LRange *makeTop () const;
  virtual LRange *makeBottom () const;
  virtual LRange *makeMeet (const LRange *a, const LRange *b) const;
  virtual LRange *makeJoin (const LRange *a, const LRange *b) const;
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
  virtual LRange *makeMakeSigned (INT width) const;
  virtual LRange *makeMakeUnsigned (INT width) const;
  virtual LRange *makeSignExtend (INT width) const;
  virtual LRange *makeZeroExtend (INT width) const;
  virtual LRange *makeLeftShift (INT width) const;
  virtual LRange *makeLeftShiftRange (const LRange *count) const;
  virtual LRange *makeSignedRightShift (INT width) const;
  virtual LRange *makeSignedRightShiftRange (const LRange *count) const;
  virtual LRange *makeBitAnd (const LRange *b) const;
  virtual LRange *makeBitOr (const LRange *b) const;
  virtual LRange *makeBitXor (const LRange *b) const;
  virtual LRange *makeBitNot () const;
  virtual LRange *makeBytePermute (INT mask) const;
  virtual LRange *makeExtract (INT start, INT width) const;
  virtual LRange *makeInsert (INT start, INT width,
			      const LRange *b) const;

  virtual void Print (FILE *f) const;


  // cloning
  virtual LRange *clone(void) const;

  // instance
  BE_EXPORTED  static LRange *getInstance(void);

  static void* operator new (size_t size); 
  static void operator delete (void *p);

  // mempool initialization
  BE_EXPORTED  static void MEMPOOL_Initialize(MEM_POOL mempool);
};

#endif /*LBVALRANGE_H_INCLUDED */
