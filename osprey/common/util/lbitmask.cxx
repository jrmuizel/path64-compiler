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
 * Module: brange.cxx
 *
 * Description:
 *
 * Implementation of used-bit range type
 *
 * ====================================================================
 * ====================================================================
 */

#include "W_limits.h"
#include "defs.h"
#include "errors.h"
#include "lbitmask.h"



const LBitMask
LBitMask::Top ()
{
  LBitMask result;
  result.rtype = top;
  // Initialize for valgrind.
  result.bmask_= (UINT64)0;
  return result;
}

const LBitMask
LBitMask::Bottom ()
{
  LBitMask result;
  result.bmask_ = UINT64_MAX;
  result.rtype = normal;
  return result;
}

const LBitMask Meet (const LBitMask &a, const LBitMask &b)
{
  UINT64 bmask;
  if (b.isTop ())
    return a;
  else if (a.isTop ())
    return b;
  else {
    bmask = a.bmask_ | b.bmask_;
    return LBitMask (bmask);
  }
}


const LBitMask Join (const LBitMask &a, const LBitMask &b)
{
  if (a.isTop ())
    return a;
  else if (b.isTop ())
    return b;
  else {
    UINT64 bmask = a.bmask_ & b.bmask_;
    if (bmask == 0ULL)
      return LBitMask::Top ();
    else
      return LBitMask (bmask);
  }
}

BOOL
LBitMask::StrictlyContains (const LBitMask &a) const
{
  if (isTop ())
    return FALSE;
  else if (a.isTop ())
    return TRUE;
  else
    return ((bmask_ | a.bmask_) == bmask_
	    && ! Equal (a));
}

BOOL
LBitMask::Equal (const LBitMask &a) const
{
  if (isTop () && a.isTop ())
    return TRUE;
  return (bmask_ == a.bmask_);
}

/* ====================================================================
 *
 * Support functions.
 *
 * ====================================================================
 */

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


INT
LBitMask::bits () const
{
  if (isBottom())
    return INT_MAX;
  if (isTop ())
    return 0;
  else {
    return 64 - countLeadingZeros64(bmask_);
  }
}

UINT64
LBitMask::getbitmask () const
{
  FmtAssert (!isTop (), ("Unexpected <top> range value"));
  return bmask_;
}


const LBitMask MakeUnsigned (const LBitMask &a, INT width)
{
  if (width > 64)
    return LBitMask::Bottom();
  if (a.isTop ())
    return a;
  else {
    UINT64 bmask_width = ((UINT64)1 << width) - (UINT64)1;
    if ((a.bmask_ | bmask_width) == a.bmask_)
      return LBitMask (width);
    else
      return a;
  }
}

const LBitMask LeftShift (const LBitMask &a, INT width)
{
  // We have a range that may or may not be signed.
  // Reinterpret it as an unsigned number in width bits.
  if (a.isTop ())
    return a;
  else if (width < 0)
    return LBitMask::Bottom ();
  else {
    UINT64 mask = (width >= 64) ? 0 : a.bmask_ << width;
    return LBitMask (mask);
  }
}


void
LBitMask::Print (FILE *f) const
{
  if (isTop ())
    fputs ("<top>", f);
  else if (isBottom ())
    fputs ("<bottom>", f);
  else {
    fprintf (f, "%#llx", bmask_);
  }
}

LBitMask::LBitMask (INT lowbit, INT width)
{
  
  if (width == 0)
    rtype = top;
  else if ((width + lowbit) >= 64) {
    rtype = normal;
    bmask_ = UINT64_MAX;// bottom
  } else {
    rtype = normal;
    UINT64 mask_low  = (lowbit >= 64) ? 0 : UINT64_MAX << lowbit;
    UINT64 mask_high  = UINT64_MAX >> (64 - (width + lowbit));    
    bmask_ = mask_low & mask_high;
  }
}

LBitMask::LBitMask (INT width)
{
  LBitMask r(0, width);
  rtype = r.rtype;
  bmask_ = r.bmask_;
}


LBitMask::LBitMask (const LBitMask &a)
{
  rtype = a.rtype;
  bmask_ = a.bmask_;
}

LBitMask::LBitMask (INT64 a)
{
  rtype = normal;
  bmask_= UINT64_MAX >> countLeadingZeros64(a);
}

LBitMask::LBitMask (UINT64 bitmask)
{
  rtype = normal;
  bmask_= bitmask;
}

LBitMask::LBitMask ()
{
  rtype = normal;
  bmask_= UINT64_MAX;
}
