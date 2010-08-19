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
