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
