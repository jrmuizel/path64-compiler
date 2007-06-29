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
 * Module: range.h
 *
 * Description:
 *
 * Implement a value range.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef RANGE_H_INCLUDED
#define RANGE_H_INCLUDED

#include "zint.h"

class Range {
 private:
  enum RangeType { normal, empty };
  mUINT8 rtype;
  ZInt min, max;
 public:
  enum RangeSign { Signed, Unsigned, UnknownSign, NoSign };
  BE_EXPORTED static const Range Empty ();
  BE_EXPORTED static const Range Universe ();
  BE_EXPORTED friend const Range Union(const Range &a, const Range &b);
  BE_EXPORTED friend const Range Intersect (const Range &a, const Range &b);
  BE_EXPORTED friend const Range SignExtend (const Range &a, INT width);
  BE_EXPORTED friend const Range ZeroExtend (const Range &a, INT width);
  BE_EXPORTED friend const Range LeftShift (const Range &a, INT count);
  BE_EXPORTED friend const Range LeftShift (const Range &a, const Range &count);
  BE_EXPORTED friend const Range SignedRightShift (const Range &a, INT count);
  BE_EXPORTED friend const Range SignedRightShift (const Range &a, const Range &count);
  // Deliberately no UnsignedRightShift: instead rerange to
  // positive and use a SignedRightShift.
  BE_EXPORTED friend const Range Modulo (const Range &a, INT width);
  BE_EXPORTED friend const Range Add (const Range &a, const Range &b);
  BE_EXPORTED friend const Range AddModulo (const Range &a, const Range &b, INT width);
  BE_EXPORTED friend const Range Sub (const Range &a, const Range &b);
  BE_EXPORTED friend const Range SubModulo (const Range &a, const Range &b, INT width);
  BE_EXPORTED friend const Range Mul (const Range &a, const Range &b);
  BE_EXPORTED friend const Range MulModulo (const Range &a, const Range &b, INT width);
  BE_EXPORTED friend const Range Min (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Max (const Range &a, const Range &b);
  BE_EXPORTED friend const Range BitAnd (const Range &a, const Range &b);
  BE_EXPORTED friend const Range BitOr (const Range &a, const Range &b);
  BE_EXPORTED friend const Range BitXor (const Range &a, const Range &b);
  BE_EXPORTED friend const Range BitNot (const Range &a);
  BE_EXPORTED friend const Range Eq (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Ne (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Ge (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Gt (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Le (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Lt (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Andl (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Nandl (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Orl (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Norl (const Range &a, const Range &b);
  BE_EXPORTED friend const Range Notl (const Range &a);
  BE_EXPORTED friend const Range HighestSetBit (const Range &a);
  BE_EXPORTED friend const Range ByteSwap (const Range &a);
  BE_EXPORTED friend const Range Extract (const Range &a, INT start, INT width);
  BE_EXPORTED friend const Range Insert (const Range &a, INT start, INT width,
					 const Range &b);
  BE_EXPORTED friend const Range MakeUnsigned (const Range &a, INT width);
  BE_EXPORTED friend const Range MakeSigned (const Range &a, INT width);
  BE_EXPORTED BOOL StrictlyContains (const Range &a) const;  // TRUE if this < v.
  BE_EXPORTED BOOL Equal (const Range &a) const;      // TRUE if this == v.
  BOOL ContainsOrEqual (const Range &a) const {
    return StrictlyContains(a) || Equal(a); }
  BOOL isEmpty () const { return rtype == empty; }
  BOOL isUniverse () const { return min.isMinusInf () && max.isPlusInf (); }
  BE_EXPORTED BOOL isNonZero () const;  // TRUE if definitely non-zero.
  BE_EXPORTED BOOL isZero () const;     // TRUE if definitely zero.
  BE_EXPORTED BOOL isNonNegative () const;  // TRUE if definitely >= 0.
  BE_EXPORTED BOOL isLiteral () const;  // TRUE if a single finite literal value.
  BE_EXPORTED INT64 LiteralValue () const; // The value if isLiteral.
  BE_EXPORTED INT bits () const;
  BE_EXPORTED void Print (FILE *f) const;
  BE_EXPORTED Range (RangeSign, INT bitwidth);
  BE_EXPORTED Range (RangeSign, INT lowbit, INT bitwidth);
  BE_EXPORTED Range (INT64);
  BE_EXPORTED Range (ZInt min, ZInt max);
  BE_EXPORTED Range (const Range &a);
  BE_EXPORTED Range ();
};

#endif /* RANGE_H_INCLUDED */
