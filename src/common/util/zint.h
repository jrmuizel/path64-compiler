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
 * Module: zint.h
 *
 * Description:
 * ============
 *
 * Provides a clamped integer class.
 *
 *
 * Interface Description:
 *
 *    Exported type:
 *
 *      ZInt - a clamped integer.
 *
 *      Constructors:
 *
 *        ZInt()
 *          Create an uninitialized ZInt.
 *        ZInt(INT64)
 *          Create a ZInt from an INT64, out of range values are clamped.
 *        ZInt(UINT64)
 *          Create a ZInt from a UINT64, out of range values are clamped.
 *
 *      Operators:
 *        + - *     Clamped addition, subtraction, multiplication
 *        += -=     Clamped accumulation
 *        - (unary) Clamped negation
 *        <<        Clamped left shift by integer shiftcount.
 *
 *      Type conversions:
 *        INT64     A ZInt can be type cast to INT64.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ZINT_H_INCLUDED
#define ZINT_H_INCLUDED


#define ZINT_MAX INT64_MAX
#define ZINT_MIN -ZINT_MAX

class ZInt {
 private:
  INT64 value;
 public:
  BE_EXPORTED ZInt ();
  BE_EXPORTED ZInt (INT);
  BE_EXPORTED ZInt (INT64);
  BE_EXPORTED ZInt (UINT64);
  BE_EXPORTED ZInt (const ZInt&);
  BE_EXPORTED friend const ZInt operator+(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend ZInt& operator+=(ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator-(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend ZInt& operator-=(ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator-(const ZInt& a);
  BE_EXPORTED friend const ZInt operator*(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator/(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator%(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator|(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator&(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator~(const ZInt& a);
  BE_EXPORTED friend const ZInt operator<<(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator>>(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const BOOL operator>(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator>=(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator<(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator<=(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator==(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator!=(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const ZInt Max(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const ZInt Min(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const ZInt abs(const ZInt& a);
  BE_EXPORTED INT64  const to_INT64 () const;
  BE_EXPORTED INT  const to_INT () const;
  BE_EXPORTED static const ZInt MinusInf ();
  BE_EXPORTED static const ZInt PlusInf ();
  const BOOL isPlusInf () const { return value == ZINT_MAX; }
  const BOOL isMinusInf () const { return value == ZINT_MIN; }
  const BOOL isFinite () const {
    return ! isPlusInf () && ! isMinusInf ();
  }
  BE_EXPORTED const INT bits () const;
  BE_EXPORTED const INT highest_set_bit () const;
  BE_EXPORTED void Print (FILE *f) const;
};

#endif /* ZINT_H_INCLUDED */
