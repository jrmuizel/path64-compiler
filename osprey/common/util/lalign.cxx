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
 * Module: lalign.cxx
 *
 * Description:
 *
 * Implementation of alignment range type
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "errors.h"
#include "lalign.h"


/* ====================================================================
 *
 * Support functions.
 *
 * ====================================================================
 */
/*
-------------------------------------------------------------------------------
Returns the gcd of 2 unsigned 64-bit numbers a and b
-------------------------------------------------------------------------------
*/
static UINT64 gcd(UINT64 u, UINT64 v)
{
    int shift;

    /* GCD(0,x) := x */
    if (u == 0 || v == 0)
      return u | v;

    /* Let shift := lg K, where K is the greatest power of 2
       dividing both u and v. */
    for (shift = 0; ((u | v) & 1) == 0; ++shift) {
        u >>= 1;
        v >>= 1;
    }

    while ((u & 1) == 0)
      u >>= 1;

    /* From here on, u is always odd. */
    do {
        while ((v & 1) == 0)  /* Loop X */
          v >>= 1;

        /* Now u and v are both odd, so diff(u, v) is even.
           Let u = min(u, v), v = diff(u, v)/2. */
        if (u <= v) {
            v -= u;
        } else {
            INT64 diff = u - v;
            u = v;
            v = diff;
        }
        v >>= 1;
    } while (v != 0);

    return u << shift;
}

/*
-------------------------------------------------------------------------------
Extended Euclidean algorithm
reference: http://fr.wikipedia.org/wiki/Algorithme_d'Euclide_étendu
Computes the gcd of 2 unsigned 64-bit numbers a and b, as well as 2 signed 64-bit
numbers u and v, such as gcd(a, b) = u*a + v*b 
(u,v) are called the Bezout coeficients
-------------------------------------------------------------------------------
*/
static void extended_euclid(UINT64 a, UINT64 b, UINT64 *gcd, INT64 *u, INT64 *v)
{
  UINT64 rr, q, rs;
  INT64 uu, vv, us, vs;

  *gcd = a;
  rr = b;
  *u = 1;
  *v = 0;
  uu = 0;
  vv = 1;

  while ( rr != 0) {
    q = *gcd / rr;
    rs = *gcd; us = *u; vs = *v;
    *gcd = rr; *u = uu; *v = vv;
    rr = rs - q * rr; uu = us - q * uu; vv = vs - q * vv;
  }
  
  FmtAssert ((*gcd == (*u * a + *v * b)), ("incompatible output for extended_euclid"));
  return;
}
  
/*
-------------------------------------------------------------------------------
Returns the number of leading 0 bits before the most-significant 1 bit of
`a'.  If `a' is zero, 32 is returned.
-------------------------------------------------------------------------------
*/
static int countLeadingZeros32( UINT a )
{
    static const int countLeadingZerosHigh[] = {
        8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int shiftCount;

    shiftCount = 0;
    if ( a < 0x10000 ) {
        shiftCount += 16;
        a <<= 16;
    }
    if ( a < 0x1000000 ) {
        shiftCount += 8;
        a <<= 8;
    }
    shiftCount += countLeadingZerosHigh[ a>>24 ];
    return shiftCount;
}


/*
-------------------------------------------------------------------------------
Returns the number of leading 0 bits before the most-significant 1 bit of
`a'.  If `a' is zero, 64 is returned.
-------------------------------------------------------------------------------
*/
static int countLeadingZeros64( UINT64 a )
{
    int shiftCount;

    shiftCount = 0;
    if ( a < ( (UINT64) 1 )<<32 ) {
        shiftCount += 32;
    }
    else {
        a >>= 32;
    }
    shiftCount += countLeadingZeros32( a );
    return shiftCount;
}

static int countTrailingZeros64( UINT64 a )
{
  return 64 - countLeadingZeros64(~a & (a - 1));
}

// Latice operators

const LAlign
LAlign::Top ()
{
  LAlign result;
  result.rtype = top;
  // Initialize for valgrind.
  result.base_ = (ZInt)0;
  result.bias_ = (ZInt)0;
  return result;
}

const LAlign
LAlign::Bottom ()
{
  LAlign result;
  result.base_ = (ZInt)1;
  result.bias_ = (ZInt)0;
  result.rtype = normal;
  return result;
}


// Base and bias Normalization
// such as :
// + base is >= 0
// + 0 <= bias < base
//
// Fix for codex bug #72153
// + normalize base to the biggest power of two, which divides the input base
//  (this is a conservative normalization such that the alignment operators remains valid 
//   in case of wrap-around)
//
LAlign
LAlign::Normalize () const
{
  LAlign result;

  if (! base_.isFinite () || ! bias_.isFinite ())
    return LAlign::Bottom ();

  if ( base_ == 0 )
    return LAlign(base_, bias_);

  if ( base_ < 0 )
    result.base_ = -base_;
  else
    result.base_ = base_;

  result.base_ = (ZInt(1) << countTrailingZeros64( result.base_.to_INT64 ()));
  
  result.bias_ = bias_ % result.base_;

  if ( result.bias_ < 0 )
    result.bias_ += result.base_;

  
  FmtAssert (result.base_ >= 0, ("Negative base value encountered"));
  FmtAssert (result.bias_ >= 0 && result.bias_ < result.base_, ("Non-normalized bias value encountered"));

 
  result.rtype = normal;
  return result;
}

const LAlign Meet (const LAlign &a, const LAlign &b)
{
  LAlign result;
  ZInt base, bias_diff;
  if (b.isTop ())
    return a;
  else if (a.isTop ())
    return b;
  else {
    bias_diff = abs(a.bias_-b.bias_);
    if (! bias_diff.isFinite ()) // case of a negative constant incurring an overflow in a.bias_-b.bias_
      return LAlign::Bottom ();  
    result.base_ = (ZInt)gcd(gcd(a.base_.to_INT64 (), b.base_.to_INT64 ()), bias_diff.to_INT64 ());
    result.bias_ = a.bias_;
    return result.Normalize ();
  }
}

/*
Implement the Join operator as the result of the linear congruence system:
   x = C1 [M1]  
   x = C2 [M2]
which is equivalent to:
   M2*k = C1 - C2 [M1] (for C1 >= C2)
or M1*k = C2 - C1 [M2] (for C2 >= C1)

The condition of existence of a set of solution for k is the following:
   abs(C1 - C2) % gcd(M1, M2) = 0

If this condition is fullfiled, then the solution of the system is the set :
   (M, C) = (lcm(M1, M2), M2 * (C1 - C2) * s / gcd(M1, M2) + C2) for C1 >= C2
          = (lcm(M1, M2), M1 * (C2 - C1) * r / gcd(M1, M2) + C1) for C2 >= C1

where (r,s) are the Bezout coefficients of the numbers (M1, M2), i.e the integers such as
  r * M1 + s * M2 = gcd(M1,M2)
and 
  lcm(M1, M2) is the least common multiple of (M1, M2), i.e: lcm(M1, M2) * gcd(M1, M2) = M1 * M2
*/
const LAlign Join (const LAlign &a, const LAlign &b)
{
  ZInt bias_diff = abs(a.bias_-b.bias_);
  UINT64 gcd;
  INT64 r, s;

  extended_euclid(a.base_.to_INT64 (), b.base_.to_INT64 (), &gcd, &r, &s);

  if (a.isTop ())
    return a;
  else if (b.isTop ())
    return b;
  else if (a.isBottom ())
    return b;
  else if (b.isBottom ())
    return a;
  else if ((a.base_ | b.base_) == 0) { // case of 2 constant values
    if (a.bias_ == b.bias_)
      return a;
    else
      return LAlign::Top ();
  }
  else if (! bias_diff.isFinite ()){
      return LAlign::Top ();
  }
  else if (bias_diff % gcd == 0){ // condition of existence of a non-null intersection
    LAlign result;
    result.base_ = a.base_ * b.base_ / (ZInt)gcd;
    result.bias_ = (a.bias_ >= b.bias_) ? bias_diff * b.base_ * (ZInt)s / (ZInt)gcd + b.bias_ : 
      bias_diff * a.base_ * (ZInt)r / (ZInt)gcd + a.bias_;
    return result.Normalize ();
  }
  return LAlign::Top ();
  
}


BOOL
LAlign::StrictlyContains (const LAlign &a) const
{
  if (isTop ())
    return FALSE;
  else if (a.isTop ())
    return TRUE;
  else if (base_ == 0)
    return FALSE;
  else if (a.base_ == 0)
    return TRUE;
  else 
    return (base_ < a.base_);
}

BOOL
LAlign::Equal (const LAlign &a) const
{
  if (isTop () && a.isTop ())
    return TRUE;
  else
    return (base_ == a.base_ && bias_ == a.bias_);
}


// Queries


BOOL
LAlign::hasValue () const
{
  return (!isTop () && (base_  == 0));
}

INT64
LAlign::getValue () const
{
  FmtAssert (hasValue (), ("Called LiteralValue on non-literal Range"));
  return bias_.to_INT64 ();
}


UINT64
LAlign::getbase () const
{
    return base_.to_INT64 ();
}

UINT64
LAlign::getbias () const
{
    return bias_.to_INT64 ();
}

// Operators

static ZInt ZeroExtend (ZInt a, INT width)
{
  return a & (((INT64)1 << width) - 1);
}

static ZInt SignExtend (ZInt a, INT width)
{
  if ((a & ((INT64)1 << (width - 1))) != ZInt(0)) {
    return a | ((INT64)-1 << width);
  } else {
    return a & (((INT64)1 << width) - 1);
  }
}

const LAlign MakeUnsigned(const LAlign &a, INT width)
{
  if (a.isTop () || width > 63)
    return a;
  else {
    ZInt base = ZeroExtend (a.base_, width);
    ZInt bias = ZeroExtend (a.bias_, width);
    return LAlign(base, bias).Normalize();
  }
}

const LAlign MakeSigned (const LAlign &a, INT width)
{
  if (a.isTop () || width > 63) {
    return a;
  } else {
    ZInt base = SignExtend (a.base_, width);
    ZInt bias = SignExtend (a.bias_, width);
    return LAlign(base, bias).Normalize();
  }
}

const LAlign LeftShift (const LAlign &a, INT width)
{
  if (a.isTop ())
    return a;
  else if (width < 0 || width >= 64)
    return LAlign::Bottom ();
  else {
    ZInt base =  a.base_ << width;
    ZInt bias =  a.bias_ << width;
    return LAlign (base, bias).Normalize ();
  }
}


const LAlign LeftShiftRange (const LAlign &a, const LAlign &b)
{
  if (a.isTop () || b.isTop ())
    return LAlign::Top ();
  else if (!b.hasValue ()) 
    return LAlign::Bottom ();
  else { // b is a literal
    return LeftShift (a, b.getValue ());
  }
}

const LAlign RightShift (const LAlign &a, INT width)
{
  if (a.isTop ())
    return a;
  else if (width < 0 || width >= 64 || 
	   (!a.hasValue () && (a.base_ <= (ZInt(1)<<width))))
    return LAlign::Bottom ();
  else {
    ZInt base =  a.base_ >> width;
    ZInt bias =  a.bias_ >> width ;
    return LAlign (base, bias).Normalize ();
  }
}


const LAlign RightShiftRange (const LAlign &a, const LAlign &b)
{
  if (a.isTop () || b.isTop ())
    return LAlign::Top ();
  else if (!b.hasValue ()) 
    return LAlign::Bottom ();
  else { // b is a literal
    return RightShift (a, b.getValue ());
  }
}


const LAlign BitOr (const LAlign &a, const LAlign &b)
{
  ZInt bias = a.bias_ | b.bias_;
  if (a.isTop ())
    return b;
  else  if (b.isTop ())
    return a;
  else if (b.hasValue () && a.hasValue ()) 
    return LAlign(0,a.bias_ | b.bias_);
  else if ( b.hasValue ()) {
    return LAlign (a.base_, bias).Normalize ();
  }
  else if ( a.hasValue ()) {
    return LAlign (b.base_, bias).Normalize ();
  }
  else 
    return LAlign::Bottom ();
}

const LAlign BitAnd (const LAlign &a, const LAlign &b)
{
  if (a.isTop ())
    return a;
  else  if (b.isTop ())
    return b;
  else if (a.hasValue () && b.hasValue ()) 
    return LAlign(0,a.bias_ & b.bias_);
  else if (b.hasValue ()) { 
    INT64 val =  b.getValue ();
    int n = countTrailingZeros64(val);
    ZInt bias = a.bias_ & b.bias_;
    if ((UINT64)b.bias_.to_INT64 () >= (UINT64)a.base_.to_INT64 () ) {
      ZInt base = Max(a.base_, ZInt(1)<<n);
      return LAlign (base, bias).Normalize ();
    }
    else {
      return LAlign (ZInt(0), bias);
    }
  }
  else if (a.hasValue ()) { 
    INT64 val =  a.getValue ();
    int n = countTrailingZeros64(val);
    ZInt bias = a.bias_ & b.bias_;
    if ((UINT64)a.bias_.to_INT64 () >= (UINT64)b.base_.to_INT64 ()) {
      ZInt base = Max(b.base_, ZInt(1)<<n);
      return LAlign (base, bias).Normalize ();
    }
    else {
      return LAlign (ZInt(0), bias);
    }
  }
  else
    return LAlign::Bottom ();
}


const LAlign Add (const LAlign &a, const LAlign &b)
{
  if (a.isTop ())
    return a;
  else  if (b.isTop ())
    return b;
  else if (a.hasValue () && b.hasValue ()) 
    return LAlign(0,a.bias_ + b.bias_);
  else if (b.hasValue ()) { 
    return LAlign (a.base_, a.bias_ + b.bias_).Normalize ();    
  }
  else if (a.hasValue ()) { 
    return LAlign (b.base_, a.bias_ + b.bias_).Normalize ();    
 }
  else {
    UINT64 basint = gcd(a.base_.to_INT64 (), b.base_.to_INT64 ());
    ZInt base = (ZInt)basint;
    ZInt bias = a.bias_ + b.bias_;
    return LAlign(base, bias).Normalize ();
  }
}

const LAlign Sub (const LAlign &a, const LAlign &b)
{
  if (a.isTop ())
    return a;
  else  if (b.isTop ())
    return b;
  else if (a.hasValue () && b.hasValue ()) 
    return LAlign(0,a.bias_ - b.bias_);
  else if (b.hasValue ()) { 
    return LAlign (a.base_, a.bias_ - b.bias_).Normalize ();    
  }
  else if (a.hasValue ()) { 
    return LAlign (b.base_, a.bias_ - b.bias_).Normalize ();    
 }
  else {
    UINT64 basint = gcd(a.base_.to_INT64 (), b.base_.to_INT64 ());
    ZInt base = (ZInt)basint;
    ZInt bias = a.bias_ - b.bias_;
    return LAlign(base, bias).Normalize ();
  }
}

const LAlign Mul (const LAlign &a, const LAlign &b)
{
  if (a.isTop ())
    return a;
  else  if (b.isTop ())
    return b;
  else if (a.hasValue () && b.hasValue ()) 
    return LAlign(0,a.bias_ * b.bias_);
  else if (b.hasValue ()) { 
    return LAlign (a.base_ * b.bias_, a.bias_ * b.bias_).Normalize ();    
  }
  else if (a.hasValue ()) { 
    return LAlign (b.base_ * a.bias_, a.bias_ * b.bias_).Normalize ();    
  }
  else if ((a.bias_ | b.bias_) == 0) { 
    return LAlign (a.base_ * b.base_, 0).Normalize ();    
  }
  else if (a.base_ == b.base_) { 
    return LAlign (a.base_, a.bias_ * b.bias_).Normalize ();    
  }  
  else {
    return LAlign::Bottom ();
  }
}

const LAlign Div (const LAlign &a, const LAlign &b)
{
  if (a.isTop ())
    return a;
  else  if (b.isTop ())
    return b;
  else if (a.hasValue () && b.hasValue ()) {
    FmtAssert (b.bias_ != 0, ("attempt to divide by zero"));
    return LAlign(0,a.bias_ / b.bias_);
  }
  else if (b.hasValue ()) { 
    FmtAssert (b.bias_ != 0, ("attempt to divide by zero"));
    if (a.base_ % b.bias_ == 0)
      return LAlign (a.base_ / b.bias_, a.bias_ / b.bias_);  
    else
      return LAlign::Bottom ();
  }
  else {
    return LAlign::Bottom ();
  }
}

const LAlign Mod (const LAlign &a, const LAlign &b)
{
  if (a.isTop ())
    return a;
  else  if (b.isTop ())
    return b;
  else if (a.hasValue () && b.hasValue ()) {
    FmtAssert (b.bias_ != 0, ("attempt to divide by zero"));
    return LAlign(0,a.bias_ % b.bias_);
  }
  else if (b.hasValue ()) { 
    FmtAssert (b.bias_ != 0, ("attempt to divide by zero"));
    if (b.bias_ > a.base_)
      return a;
    else if (a.base_ % b.bias_ == 0)
      return LAlign (b.base_, a.bias_ % b.bias_);  
    else
      return LAlign::Bottom ();
  }
  else {
    return LAlign::Bottom ();
  }
}


// Tracing helper

void
LAlign::Print (FILE *f) const
{
  if (isTop ())
    fputs ("<top>", f);
  else if (isBottom ())
    fputs ("<bottom>", f);
  else {
    fprintf (f, "\t base: %lld \t bias: %lld", base_.to_INT64 (), bias_.to_INT64 ());
  }
}

// Constructors

LAlign::LAlign (const LAlign &a)
{
  rtype = a.rtype;
  base_ = a.base_;
  bias_ = a.bias_;
}

LAlign::LAlign (ZInt base, ZInt bias)
{
  rtype = normal;
  base_= base;
  bias_= bias;
}

LAlign::LAlign (INT64 value)
{
  rtype = normal;
  base_= 0;
  bias_= value;
}

LAlign::LAlign ()
{
  rtype = normal;
  base_= (ZInt)0;
  bias_= (ZInt)0;
}
