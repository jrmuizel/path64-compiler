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
  static LARange *top_ ; 
  static LARange *bottom_ ; 
  static LARange *universe_ ;
public:
  // constructors
  LARange(const LARange &that) { *this = that; }
  LARange(const LAlign &that): arange_(that) { }
  LARange(): arange_(LAlign::Bottom()) {}
  virtual ~LARange() {} 

  static LAlign Top(){ return LAlign::Top();}
  static LAlign Bottom(){ return LAlign::Bottom();}
  static LAlign Universe(){ return LAlign::Bottom();}

  virtual LARange *makeRangeValue (INT64 value) const;
  virtual LARange *makeAlign (ZInt base, ZInt bias) const;
  virtual LARange *makeUniverse () const;
  virtual LARange *makeTop () const;
  virtual LARange *makeBottom () const;
  virtual LARange *makeMeet (const LRange *a, const LRange *b) const;
  virtual LARange *makeJoin (const LRange *a, const LRange *b) const;
  // queries

  virtual BOOL hasValue(void) const;
  virtual INT64 getValue(void) const;
  virtual UINT64 getBase(void) const;
  virtual UINT64 getBias(void) const;
  virtual BOOL isTop () const;
  virtual BOOL StrictlyContains (const LRange *a) const;
  virtual BOOL Equal (const LRange *a) const;
  virtual BOOL ContainsOrEqual (const LRange *a) const;
  virtual LARange *makeSignExtend (INT width) const;
  virtual LARange *makeZeroExtend (INT width) const;
  virtual LARange *makeMakeSigned (INT width) const;
  virtual LARange *makeMakeUnsigned (INT width) const;
  virtual LARange *makeLeftShift (INT width) const;
  virtual LARange *makeLeftShiftRange (const LRange *count) const;
  virtual LARange *makeSignedRightShift (INT width) const;
  virtual LARange *makeSignedRightShiftRange (const LRange *count) const;
  virtual LARange *makeBitAnd (const LRange *b) const;
  virtual LARange *makeBitOr (const LRange *b) const;
  virtual LARange *makeAdd (const LRange *b) const;
  virtual LARange *makeSub (const LRange *b) const;
  virtual LARange *makeMul (const LRange *b) const;
  virtual LARange *makeAddModulo (const LRange *b, INT width) const;
  virtual LARange *makeSubModulo (const LRange *b, INT width) const;
  virtual LARange *makeMulModulo (const LRange *b, INT width) const;
  virtual LARange *makeDiv (const LRange *b) const;
  virtual LARange *makeMod (const LRange *b) const;

  virtual void Print (FILE *f) const;


  // cloning
  virtual LARange *clone(void) const;

  // instance
  BE_EXPORTED  static LARange *getInstance(void);

  // mempool initialization
  BE_EXPORTED  static void MEMPOOL_Initialize(MEM_POOL mempool);

  // mempool visibility
  virtual MEM_POOL *Mem_pool() { return &mempool_; }
};

#endif /*LARANGE_H_INCLUDED */
