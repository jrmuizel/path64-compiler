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
 * Module: lbmask.h
 *
 * Description:
 *
 * Implement a lattice bit-mask (LBitMask) class
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LBMASK_H_INCLUDED
#define LBMASK_H_INCLUDED

class LBitMask {
 private:
  enum RangeType { normal, top };
  mUINT8 rtype;
  UINT64 bmask_;
 public:
  // lattice properties
  static const LBitMask Top ();
  static const LBitMask Bottom ();
  friend const LBitMask Meet (const LBitMask &a, const LBitMask &b);
  friend const LBitMask Join (const LBitMask &a, const LBitMask &b);
  BOOL StrictlyContains (const LBitMask &a) const;  // TRUE if this < a.
  BOOL Equal (const LBitMask &a) const;      // TRUE if this == a.  
  BOOL ContainsOrEqual (const LBitMask &a) const {
    return StrictlyContains(a) || Equal(a); }
  // queries
  BOOL isTop () const { return rtype == top; }
  BOOL isBottom () const { return bmask_ == UINT64_MAX; }
  INT bits () const;
  UINT64 getbitmask () const;
  friend const LBitMask MakeUnsigned (const LBitMask &a, INT width);
  friend const LBitMask LeftShift (const LBitMask &a, INT width);
  void Print (FILE *f) const;
  // constructors
  LBitMask (INT bitwidth);
  LBitMask (INT lowbit, INT bitwidth);
  LBitMask (const LBitMask &a);
  LBitMask (INT64 a);
  LBitMask (UINT64 bitmask);
  LBitMask ();
};


#endif /* LBMASK_H_INCLUDED */
