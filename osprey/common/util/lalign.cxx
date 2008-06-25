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
  LAlign a1, a2;
  ZInt base;
  if (b.isTop ())
    return a;
  else if (a.isTop ())
    return b;
  else {
    if ((a.base_ | b.base_) == 0) { // case of 2 constant values
      return LAlign(abs(a.bias_ - b.bias_), Min(a.bias_, b.bias_)).Normalize ();
    }
    else if ( a.base_ == 0) { 
      if (a.bias_ % b.base_ == b.bias_)
	return b;
      else
	return LAlign::Bottom ();
    }
    else if ( b.base_ == 0) { 
      if (b.bias_ % a.base_ == a.bias_)
	return a;
      else
	return LAlign::Bottom ();
    }
    else {
      UINT64 basint = gcd(a.base_.to_INT64 (), b.base_.to_INT64 ());
      base = (ZInt)basint;
      a1 = LAlign(base, a.bias_).Normalize ();
      a2 =  LAlign(base, b.bias_).Normalize ();
      if (a1.Equal (a2))
	return a1;
      else {
	int i;
	for (i = Max(a1.bias_, a2.bias_).to_INT (); i > 1; i--)
	  if ((a1.base_ % i) == 0 && ((a1.bias_ % i) == (a2.bias_ % i)))
	    return LAlign(i, a1.bias_ % i);
	return LAlign::Bottom ();
      }
    } 
  }
}

const LAlign Join (const LAlign &a, const LAlign &b)
{
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
  else if ( b.base_ == 0) {
    if (b.bias_ % a.base_ == a.bias_)
	return b;
      else
	return LAlign::Top ();
  }
  else if ( a.base_ == 0) {
    if (a.bias_ % b.base_ == b.bias_)
	return a;
      else
	return LAlign::Top ();
  }
  else {
    LAlign result;
    int i;
    UINT64 igcd = gcd(a.base_.to_INT64 (), b.base_.to_INT64 ());
    ZInt ilcm = a.base_ * b.base_ / (ZInt)igcd;
    for (i=0; i <= (ilcm - b.bias_)/b.base_; i++)
      if ((((b.base_ * (ZInt)i) + b.bias_ - a.bias_) % a.base_) == 0){
	result.base_ = ilcm;
	result.bias_ = (b.base_ * (ZInt)i) + b.bias_;
	return result;
      }
    return LAlign::Top ();
  }
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

const LAlign MakeUnsigned(const LAlign &a, INT width)
{
  ZInt mostneg = 0;
  ZInt mostpos = (ZInt(1) << width) - ZInt(1);
  if (a.isTop ())
    return a;
  else if (width >= 64)
    return  LAlign::Bottom ();
  else if ( !a.hasValue ()){
    if (a.base_ > mostpos || a.base_ < mostneg) 
      return  LAlign::Bottom ();
    else
      return a;
  }
  else { // a.hasValue ()
    ZInt val = a.getValue ();
    if (val > mostpos || val < mostneg){
      INT64 res = (UINT64)((val.to_INT64 ()) << (64 - width)) >> (64 - width);
      return LAlign (res);
    }
    else
      return a;
  }
}



const LAlign MakeSigned (const LAlign &a, INT width)
{
  ZInt mostpos = (ZInt(1) << (width - 1)) - ZInt(1);
  ZInt mostneg = ZInt(-1) << (width - 1);
  if (a.isTop ())
    return a;
  else if (width >= 64)
    return  LAlign::Bottom ();
  else if ( !a.hasValue ()){
    if (a.base_ > mostpos || a.base_ < mostneg) 
      return  LAlign::Bottom ();
    else
      return a;
  }
  else { // a.hasValue ()
    ZInt val = a.getValue ();
    if (val > mostpos || val < mostneg){
      INT64 res = ((val.to_INT64 ()) << (64 - width)) >> (64 - width);
      return LAlign (res);
    }
    else
      return a;
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
    return LAlign (base, bias);
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
    return LAlign (base, bias);
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
