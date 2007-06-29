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
 * Module: range.cxx
 *
 * Description:
 *
 * Implementation of value range type.
 *
 * ====================================================================
 * ====================================================================
 */

#include "W_limits.h"
#include "defs.h"
#include "errors.h"
#include "range.h"



const Range
Range::Empty ()
{
  Range result;
  result.rtype = empty;
  // Initialize for valgrind.
  result.min = 1; result.max = 0;
  return result;
}

const Range
Range::Universe ()
{
  Range result;
  result.min = ZInt::MinusInf ();
  result.max = ZInt::PlusInf ();
  result.rtype = normal;
  return result;
}

const Range Union (const Range &a, const Range &b)
{
  if (b.isEmpty ())
    return a;
  else if (a.isEmpty ())
    return b;
  else
    return Range (MIN (a.min, b.min),
		  MAX (a.max, b.max));
}

const Range Intersect (const Range &a, const Range &b)
{
  if (a.isEmpty ())
    return a;
  else if (b.isEmpty ())
    return b;
  else {
    ZInt new_min = MAX (a.min, b.min);
    ZInt new_max = MIN (a.max, b.max);
    if (new_min > new_max)
      return Range::Empty ();
    else
      return Range (new_min, new_max);
  }
}

const Range SignExtend (const Range &a, INT width)
{
  return MakeSigned (a, width);
}

const Range ZeroExtend (const Range &a, INT width)
{
  return MakeUnsigned (a, width);
}

const Range LeftShift (const Range &a, INT count)
{
  if (a.isEmpty ())
    return a;
  else if (count < 0)
    return Range::Universe ();
  else
    return Range (a.min << count,
		  a.max << count);
}

const Range LeftShift (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (b.min < 0)
    return Range::Universe ();
  else {
    ZInt v1 = a.min << b.min;
    ZInt v2 = a.min << b.max;
    ZInt v3 = a.max << b.min;
    ZInt v4 = a.max << b.max;
    return Range (MIN(MIN(MIN(v1, v2), v3), v4),
                  MAX(MAX(MAX(v1, v2), v3), v4));
  }
}

const Range SignedRightShift (const Range &a, INT count)
{
  if (a.isEmpty ())
    return a;
  else if (count < 0)
    return Range::Universe ();
  else
    return Range (a.min >> count,
		  a.max >> count);
}
		  
const Range SignedRightShift (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (b.min < 0)
    return Range::Universe ();
  else {
    ZInt v1 = a.min >> b.min;
    ZInt v2 = a.min >> b.max;
    ZInt v3 = a.max >> b.min;
    ZInt v4 = a.max >> b.max;
    return Range (MIN(MIN(MIN(v1, v2), v3), v4),
                  MAX(MAX(MAX(v1, v2), v3), v4));
  }
}

const Range Add (const Range &a, const Range &b)
{
  if (a.isEmpty ())
    return a;
  else if (b.isEmpty ())
    return b;
  else
    return Range (a.min + b.min, a.max + b.max);
}

const Range Modulo (const Range &a, INT width)
{
  if (a.isEmpty ())
    return a;
  else {
    ZInt low_limit = (ZInt)-1 << width;
    ZInt high_limit = ((ZInt)1 << width) - 1;
    ZInt max = (a.min < low_limit) ? high_limit : a.max;
    ZInt min = (a.max > high_limit) ? low_limit : a.min;
    return Range (min, max);
  }
}

const Range AddModulo (const Range &a, const Range &b, INT width)
{
  return Modulo (Add (a, b), width);
}

const Range Sub (const Range &a, const Range &b)
{
  if (a.isEmpty ())
    return a;
  else if (b.isEmpty ())
    return b;
  else
    return Range (a.min - b.max, a.max - b.min);
}

const Range SubModulo (const Range &a, const Range &b, INT width)
{
  return Modulo (Sub (a, b), width);
}

const Range Mul (const Range &a, const Range &b)
{
  if (a.isEmpty ())
    return a;
  else if (b.isEmpty ())
    return b;
  else {
    ZInt v1 = a.min * b.min;
    ZInt v2 = a.min * b.max;
    ZInt v3 = a.max * b.min;
    ZInt v4 = a.max * b.max;
    return Range (MIN(MIN(MIN(v1, v2), v3), v4),
                  MAX(MAX(MAX(v1, v2), v3), v4));
  }
}

const Range MulModulo (const Range &a, const Range &b, INT width)
{
  return Modulo (Mul (a, b), width);
}    

const Range Min (const Range &a, const Range &b)
{
  if (a.isEmpty ())
    return a;
  else if (b.isEmpty ())
    return b;
  else
    return Range (MIN(a.min, b.min),
		  MIN(a.max, b.max));
}

const Range Max (const Range &a, const Range &b)
{
  if (a.isEmpty ())
    return a;
  else if (b.isEmpty ())
    return b;
  else
    return Range (MAX(a.min, b.min),
		  MAX(a.max, b.max));
}

const Range
BitAnd (const Range &a, const Range &b)
{
  /*
   * AND(lhs,rhs)
   *	((POS, s1), (POS, s2)) ->	(POS, MIN(s1,s2))
   *	((NEG, s1), (POS, s2)) ->	(POS, s2)
   *	((POS, s1), (NEG, s2)) ->	(POS, s1)
   *	((NEG, s1), (NEG, s2)) ->	(NEG, MAX(s1,s2))
   */
  Range new_value;
  INT bits;
  Range::RangeSign sign;

  if (a.isEmpty() || a.isZero())
    new_value = a;
  else if (b.isEmpty() || b.isZero())
    new_value = b;
  else {
    INT bits1 = a.bits();
    INT bits2 = b.bits();
    BOOL pos1 = a.isNonNegative ();
    BOOL pos2 = b.isNonNegative ();
    bits = ((pos1 && pos2) ? MIN(bits1, bits2)
	    : pos1 ? bits1
	    : pos2 ? bits2
	    : MAX(bits1, bits2));
    if (pos1 || pos2) {
      sign = Range::Unsigned;
    } else {
      sign = Range::Signed;
    }
    new_value = Range (sign, bits);
  }
  return new_value;
}

const Range BitOr (const Range &a, const Range &b)
{
  /*
   * OR(lhs,rhs)
   *	((POS, s1), (POS, s2)) ->	(POS, MAX(s1,s2))
   *	((NEG, s1), (POS, s2)) ->	(NEG, MAX(s1,s2+1))
   *	((POS, s1), (NEG, s2)) ->	(NEG, MAX(s1+1,s2))
   *	((NEG, s1), (NEG, s2)) ->	(NEG, MAX(s1,s2))
   */
  Range new_value;
  INT bits;
  Range::RangeSign sign;

  if (a.isEmpty () || a.isZero ())
    new_value = b;
  else if (b.isEmpty () || b.isZero ())
    new_value = a;
  else {
    BOOL pos1 = a.isNonNegative ();
    BOOL pos2 = b.isNonNegative ();
    INT bits1 = a.bits();
    INT bits2 = b.bits();
    // When combining values of different signedness,
    // the unsigned value becomes signed and requires
    // an extra bit for the sign.
    bits = (((pos1 && pos2)
	    || (!pos1 && !pos2)) ? MAX (bits1, bits2)
	    : pos1 ? MAX (bits1 + 1, bits2)
	    : MAX (bits1, bits2 + 1));
    sign = (pos1 && pos2) ? Range::Unsigned : Range::Signed;
    new_value = Range (sign, bits);
  }
  return new_value;
}

const Range BitXor (const Range &a, const Range &b)
{
  /*
   * XOR(lhs, rhs)
   *	((POS, s1), (POS, s2)) ->	(POS, MAX(s1,s2))
   *	((NEG, s1), (POS, s2)) ->	(NEG, MAX(s1,s2+1))
   *	((POS, s1), (NEG, s2)) ->	(NEG, MAX(s1+1,s2))
   *	((NEG, s1), (NEG, s2)) ->	(NEG, MAX(s1,s2))
   */
  Range new_value;
  INT bits;
  Range::RangeSign sign;

  if (a.isEmpty() || a.isZero())
    new_value = b;
  else if (b.isEmpty() || b.isZero())
    new_value = a;
  else {
    BOOL pos1 = a.isNonNegative ();
    BOOL pos2 = b.isNonNegative ();
    INT bits1 = a.bits();
    INT bits2 = b.bits();
    bits = (((pos1 && pos2)
	    || (!pos1 && !pos2)) ? MAX (bits1, bits2)
	    : pos1 ? MAX (bits1 + 1, bits2)
	    : MAX (bits1, bits2 + 1));
    sign = (pos1 && pos2) ? Range::Unsigned : Range::Signed;
    new_value = Range (sign, bits);
  }
  return new_value;
} 

const Range BitNot (const Range &a)
{
  /*
   * NOT(range)
   * (POS, s)		(NEG, s+1)
   * (NEG, s)		(NEG, s)
   */
  if (a.isEmpty ())
    return a;
  else
    return Range (Range::Signed, a.bits() + INT (a.isNonNegative ()));
}

const Range Eq (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (a.min == a.max
	   && b.min == b.max
	   && a.min == b.min)
    return Range (1);
  else if (Intersect (a, b).isEmpty ())
    return Range (0);
  else
    return Range (0, 1);
}

const Range Ne (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (a.min == a.max
	   && b.min == b.max
	   && a.min == b.min)
    return Range (0);
  else if (Intersect (a, b).isEmpty ())
    return Range (1);
  else
    return Range (0, 1);
}

const Range Ge (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (a.min >= b.max)
    return Range (1);
  else if (a.max < b.min)
    return Range (0);
  else
    return Range (0, 1);
}

const Range Gt (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (a.min > b.max)
    return Range (1);
  else if (a.max <= b.min)
    return Range (0);
  else
    return Range (0, 1);
}

const Range Le (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (a.max <= b.min)
    return Range (1);
  else if (a.min > b.max)
    return Range (0);
  else
    return Range (0, 1);
}

const Range Lt (const Range &a, const Range &b)
{
  if (a.isEmpty () || b.isEmpty ())
    return Range::Empty ();
  else if (a.max < b.min)
    return Range (1);
  else if (a.min >= b.max)
    return Range (0);
  else
    return Range (0, 1);
}

const Range Andl (const Range &a, const Range &b)
{
  if (a.isNonZero () && b.isNonZero ())
    return Range (1);
  else if (a.isZero () || b.isZero ())
    return Range (0);
  else
    return Range (0, 1);
}

const Range Nandl (const Range &a, const Range &b)
{
  return Notl (Andl (a, b));
}

const Range Orl (const Range &a, const Range &b)
{
  if (a.isNonZero () || b.isNonZero ())
    return Range (1);
  else if (a.isZero () && b.isZero ())
    return Range (0);
  else
    return Range (0, 1);
}

const Range Norl (const Range &a, const Range &b)
{
  return Notl (Orl (a, b));
}

const Range Notl (const Range &a)
{
  if (a.isEmpty ())
    return a;
  else if (a.isZero ())
    return Range (1);
  else if (a.isNonZero ())
    return Range (0);
  else
    return Range (0, 1);
}

const Range HighestSetBit (const Range &a)
{
  ZInt min, max;

  if (a.min < 0) {
    max = ZInt::PlusInf ();
    if (a.max > 0)
      min = a.max.highest_set_bit ();
    else
      min = ZInt::PlusInf ();
  } else {
    min = a.min.highest_set_bit ();
    max = a.max.highest_set_bit ();
  }
  return Range (min, max);
}

const Range Extract (const Range &a, INT start, INT width)
{
  if (start+width > 64) return Range::Universe();
  return BitAnd (SignedRightShift (a, start),
		    Range (Range::Unsigned, width));
}      

const Range Insert (const Range &a, INT start, INT width,
		    const Range &b)
{
  if (start+width > 64) return Range::Universe();
  ZInt mask = ((ZInt)1 << width) - 1;
  return BitOr (LeftShift (BitAnd (b, Range (mask, mask)),
			   start),
		a);
}

const Range MakeUnsigned (const Range &a, INT width)
{
  // We have a range that may or may not be signed.
  // Reinterpret it as an unsigned number in width bits.
  if (width > 64)
    return Range::Universe();
  if (a.isEmpty ())
    return a;
  else {
    ZInt mostneg = 0;
    ZInt mostpos = (ZInt(1) << width) - ZInt(1);
    if (a.max > mostpos || a.min < mostneg)
      return Range (Range::Unsigned, width);
    else
      return a;
  }
}

const Range MakeSigned (const Range &a, INT width)
{
  if (width > 64)
    return Range::Universe();
  // We have a range that may or may not be signed.
  // Reinterpret it as a signed number in width bits.
  if (a.isEmpty ())
    return a;
  else {
    ZInt mostpos = (ZInt(1) << (width - 1)) - ZInt(1);
    ZInt mostneg = ZInt(-1) << (width - 1);
    if (a.max > mostpos || a.min < mostneg)
      return Range (Range::Signed, width);
    else
      return a;
  }
}


BOOL
Range::StrictlyContains (const Range &a) const
{
  if (isEmpty ())
    return FALSE;
  else if (a.isEmpty ())
    return TRUE;
  else
    return (min <= a.min
	    && max >= a.max
	    && ! Equal (a));
}

BOOL
Range::Equal (const Range &a) const
{
  if (isEmpty () && a.isEmpty ())
    return TRUE;
  return min == a.min && max == a.max;
}

BOOL
Range::isNonZero () const
{
  // TRUE if every member of range is non-zero.
  return (isEmpty () || min > 0);
}

BOOL
Range::isZero () const
{
  return (! isEmpty () && min == 0 && max == 0);
}

BOOL
Range::isNonNegative () const
{
  // TRUE if every member of range is non-negative.
  return (isEmpty () || min >= 0);
}

BOOL
Range::isLiteral () const
{
  // Recall: comparison returns false if either is non-finite.
  return ! isEmpty () && min == max;
}

INT64
Range::LiteralValue () const
{
  FmtAssert (isLiteral (), ("Called LiteralValue on non-literal Range"));
  return min.to_INT64 ();
}

INT
Range::bits () const
{
  if (isUniverse())
    return INT_MAX;
  else if (isEmpty ())
    return 0;
  else {
    INT min_msb = min.bits ();
    INT max_msb = max.bits ();
    if (min < 0 && max > 0)
      return MAX (min_msb, max_msb + 1);
    else
      return MAX (min_msb, max_msb);
  }
}


void
Range::Print (FILE *f) const
{
  if (isEmpty ())
    fputs ("<empty>", f);
  else if (isUniverse ())
    fputs ("<universe>", f);
  else {
    putc ('[', f);
    min.Print (f);
    putc (':', f);
    max.Print (f);
    putc (']', f);
  }
}

Range::Range (RangeSign Sign, INT lowbit, INT width)
{
  if (Sign == NoSign) Sign = Unsigned;
  if (width == 0)
    rtype = empty;
  else if ((width + lowbit) >= 64) {
    rtype = normal;
    min = ZInt::MinusInf ();
    max = ZInt::PlusInf ();
  } else {
    rtype = normal;
    INT64 wanted_mask  = (lowbit >= 64) ? 0 : (INT64)-1 << lowbit;
    ZInt mostneg_signed = ZInt(-1) << (lowbit + width - 1);
    ZInt mostpos_signed = ZInt((((INT64)1 << (lowbit + width - 1)) - 1)
			       & wanted_mask);
    ZInt mostneg_unsigned = 0;
    ZInt mostpos_unsigned = ZInt ((((INT64)1 << (lowbit + width)) - 1)
				  & wanted_mask);
    min = (Sign == Unsigned) ? mostneg_unsigned : mostneg_signed;
    max = (Sign == Signed)   ? mostpos_signed   : mostpos_unsigned;
  }
}

Range::Range (RangeSign Sign, INT width)
{
  if (Sign == NoSign) Sign = Unsigned;
  Range r(Sign, 0, width);
  rtype = r.rtype;
  min = r.min;
  max = r.max;
}

Range::Range (INT64 a)
{
  rtype = normal;
  min = ZInt (a);
  max = ZInt (a);
}

Range::Range (ZInt minval, ZInt maxval) : min (minval), max (maxval)
{
  rtype = normal;
  FmtAssert (minval <= maxval, ("Range min and max error"));
}

Range::Range (const Range &a)
{
  rtype = a.rtype;
  min = a.min;
  max = a.max;
}

Range::Range ()
{
  rtype = normal;
  min = ZInt::MinusInf ();
  max = ZInt::PlusInf ();
}
