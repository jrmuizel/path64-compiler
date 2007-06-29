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
  BE_EXPORTED friend const ZInt operator<<(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const ZInt operator>>(const ZInt& a, const ZInt& b);
  BE_EXPORTED friend const BOOL operator>(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator>=(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator<(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator<=(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator==(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const BOOL operator!=(const ZInt &a, const ZInt &b);
  BE_EXPORTED friend const ZInt abs(const ZInt& a);
  BE_EXPORTED INT64  const to_INT64 () const;
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
