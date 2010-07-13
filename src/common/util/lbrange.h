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
 * Module: lbrange.h
 *
 * Description:
 *
 * Implement a derived class of LRange for used-bits range type analysis, 
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LBRANGE_H_INCLUDED
#define LBRANGE_H_INCLUDED

#include "mempool.h"
#include "lrange.h"
#include "lbitmask.h"
#include <assert.h>

class LBRange: public LRange {
private:
  LBitMask brange_;
  static MEM_POOL mempool_;
public:
  // constructors
  LBRange(const LBRange &that) { *this = that; }
  LBRange(const LBitMask &that): brange_(that) { }
  LBRange(): brange_(LBitMask::Bottom()) {}

  static LBitMask Bottom(){ return LBitMask::Bottom();}

  virtual LRange *makeRangeBitWidth (INT bitwidth) const;
  virtual LRange *makeRangeValue (INT64 bmask) const;
  virtual LRange *makeRangeLowBitWidth (INT lowbit, INT bitwidth) const;
  virtual LRange *makeUniverse () const;
  virtual LRange *makeTop () const;
  virtual LRange *makeBottom () const;
  virtual LRange *makeMeet (const LRange *a, const LRange *b) const;
  virtual LRange *makeJoin (const LRange *a, const LRange *b) const;
  // queries

  virtual BOOL isTop () const;
  virtual BOOL StrictlyContains (const LRange *a) const;
  virtual BOOL Equal (const LRange *a) const;
  virtual BOOL ContainsOrEqual (const LRange *a) const;
  virtual INT bits() const;
  virtual UINT64 getBitMask() const;
  virtual LRange *makeMakeUnsigned (INT width) const;
  virtual LRange *makeLeftShift (INT width) const;

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

#endif /*LBRANGE_H_INCLUDED */
