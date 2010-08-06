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
 * Module: larange.h
 *
 * Description:
 *
 * Implement a derived class of LRange for alignment range type analysis, 
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LARANGE_H_INCLUDED
#define LARANGE_H_INCLUDED

#include "mempool.h"
#include "lrange.h"
#include "zint.h"
#include "lalign.h"
#include <assert.h>

class LARange: public LRange {
private:
  LAlign arange_;
  static MEM_POOL mempool_;
public:
  // constructors
  LARange(const LARange &that) { *this = that; }
  LARange(const LAlign &that): arange_(that) { }
  LARange(): arange_(LAlign::Bottom()) {}

  static LAlign Bottom(){ return LAlign::Bottom();}

  virtual LRange *makeRangeValue (INT64 value) const;
  virtual LRange *makeAlign (ZInt base, ZInt bias) const;
  virtual LRange *makeUniverse () const;
  virtual LRange *makeTop () const;
  virtual LRange *makeBottom () const;
  virtual LRange *makeMeet (const LRange *a, const LRange *b) const;
  virtual LRange *makeJoin (const LRange *a, const LRange *b) const;
  // queries

  virtual BOOL hasValue(void) const;
  virtual INT64 getValue(void) const;
  virtual UINT64 getBase(void) const;
  virtual UINT64 getBias(void) const;
  virtual BOOL isTop () const;
  virtual BOOL StrictlyContains (const LRange *a) const;
  virtual BOOL Equal (const LRange *a) const;
  virtual BOOL ContainsOrEqual (const LRange *a) const;
  virtual LRange *makeSignExtend (INT width) const;
  virtual LRange *makeZeroExtend (INT width) const;
  virtual LRange *makeMakeSigned (INT width) const;
  virtual LRange *makeMakeUnsigned (INT width) const;
  virtual LRange *makeLeftShift (INT width) const;
  virtual LRange *makeLeftShiftRange (const LRange *count) const;
  virtual LRange *makeSignedRightShift (INT width) const;
  virtual LRange *makeSignedRightShiftRange (const LRange *count) const;
  virtual LRange *makeBitAnd (const LRange *b) const;
  virtual LRange *makeBitOr (const LRange *b) const;
  virtual LRange *makeAdd (const LRange *b) const;
  virtual LRange *makeSub (const LRange *b) const;
  virtual LRange *makeMul (const LRange *b) const;
  virtual LRange *makeAddModulo (const LRange *b, INT width) const;
  virtual LRange *makeSubModulo (const LRange *b, INT width) const;
  virtual LRange *makeMulModulo (const LRange *b, INT width) const;
  virtual LRange *makeDiv (const LRange *b) const;
  virtual LRange *makeMod (const LRange *b) const;

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

#endif /*LARANGE_H_INCLUDED */
