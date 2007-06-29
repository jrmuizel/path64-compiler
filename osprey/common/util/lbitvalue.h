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
  friend const LBitValue LeftShift (const LBitValue &a, INT width);
  friend const LBitValue LeftShiftRange (const LBitValue &a, const LBitValue &b);
  friend const LBitValue RightShift (const LBitValue &a, INT width);
  friend const LBitValue RightShiftRange (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitOr (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitXor (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitAnd (const LBitValue &a, const LBitValue &b);
  friend const LBitValue BitNot (const LBitValue &a);
  friend const LBitValue Extract (const LBitValue &a, INT start, INT width);
  friend const LBitValue Insert (const LBitValue &a, INT start, INT width,
					 const LBitValue &b);
  void Print (FILE *f) const;
  // constructors
  LBitValue (const LBitValue &a);
  LBitValue (INT64 a);
  LBitValue (const UINT64 bmask, const UINT64 valmask);
  LBitValue ();
};


#endif /* LBVALUE_H_INCLUDED */
