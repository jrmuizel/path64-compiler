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
 * Module: lbvalue.h
 *
 * Description:
 *
 * Implement a lattice bit-value (LBitValue) class
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LBVALUE_H_INCLUDED
#define LBVALUE_H_INCLUDED

class LBitValue {
 private:
  enum RangeType { normal, top };
  mUINT8 rtype;
  UINT64 zeromask_;
  UINT64 onemask_;
 public:
  enum RangeSign { Signed, Unsigned};
  // lattice properties
  static const LBitValue Top ();
  static const LBitValue Bottom ();
  friend const LBitValue Meet (const LBitValue &a, const LBitValue &b);
  friend const LBitValue Join (const LBitValue &a, const LBitValue &b);
  BOOL StrictlyContains (const LBitValue &a) const;  // TRUE if this < a.
  BOOL Equal (const LBitValue &a) const;      // TRUE if this == a.  
  BOOL ContainsOrEqual (const LBitValue &a) const {
    return StrictlyContains(a) || Equal(a); }
  // queries
  BOOL isTop () const { return rtype == top; }
  BOOL isBottom () const { return (zeromask_ | onemask_) == 0; }
  BOOL hasValue () const;  // TRUE if a single finite literal value.
  INT64 getValue () const; // The value if isLiteral.
  INT bits () const;
  INT getlzcnt () const;
  INT getlocnt () const;
  INT gettzcnt () const;
  INT gettocnt () const;
  UINT64 getzeromask () const;
  UINT64 getonemask () const;
  UINT64 getvalmask () const;
  friend const LBitValue MakeSigned (const LBitValue &a, INT width);
  friend const LBitValue MakeUnsigned (const LBitValue &a, INT width);
  friend const LBitValue SignExtend (const LBitValue &a, INT width);
  friend const LBitValue ZeroExtend (const LBitValue &a, INT width);
  friend const LBitValue LeftShift (const LBitValue &a, INT width);
  friend const LBitValue LeftShiftRange (const LBitValue &a, const LBitValue &b);
  friend const LBitValue RightShift (const LBitValue &a, INT width);
  friend const LBitValue RightShiftRange (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitOr (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitXor (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitAnd (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitNot (const LBitValue &a);
  friend const LBitValue BytePermute (const LBitValue &a, INT mask);
  friend const LBitValue Extract (const LBitValue &a, INT start, INT width);
  friend const LBitValue Insert (const LBitValue &a, INT start, INT width,
					 const LBitValue &b);
  void Print (FILE *f) const;
  // constructors
  LBitValue (const LBitValue &a);
  LBitValue (INT64 a);
  LBitValue (const UINT64 bmask, const UINT64 valmask);
  LBitValue (RangeSign sign, int bitwidth);
  LBitValue ();
};


#endif /* LBVALUE_H_INCLUDED */
