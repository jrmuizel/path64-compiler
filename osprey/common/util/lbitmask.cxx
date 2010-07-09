/*
  Copyright (C) 2006, STMicroelectronics, All Rights Reserved.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

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
#include "clz.h"
#include "errors.h"
#include "lbitmask.h"



const LBitMask
LBitMask::Top ()
{
  LBitMask result;
  // Top is represented by a Zero mask
  // We don't use the ctor since it is built to assert when creating such a value
  result.bmask_= (UINT64)0;
  return result;
}

const LBitMask
LBitMask::Bottom ()
{
  return LBitMask(UINT64_MAX);
}

const LBitMask Meet (const LBitMask &a, const LBitMask &b)
{
  if (b.isTop ())
    return a;
  else if (a.isTop ())
    return b;
  else {
    UINT64 bmask = a.bmask_ | b.bmask_;
    Is_True(bmask,("%s : unexpected zero non-top bmask created from a=%#llx b=%#llx",__PRETTY_FUNCTION__, a.bmask_, b.bmask_));
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
  // If both are Top, they are equal
  // Else if both masks are equal, they are equal
  // Meaning we need to test masks only
  return (bmask_ == a.bmask_);
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

// Constructors
LBitMask::LBitMask (INT lowbit, INT width)
{
  
  if (width == 0) {
    // This represents Top
    bmask_ = 0;
  } else if ((width + lowbit) >= 64) {
    bmask_ = UINT64_MAX;// bottom
  } else {
    UINT64 mask_low  = (lowbit >= 64) ? 0 : UINT64_MAX << lowbit;
    UINT64 mask_high  = UINT64_MAX >> (64 - (width + lowbit));    
    bmask_ = mask_low & mask_high;
    Is_True(bmask_,("%s : unexpected zero non-top bmask created from lowbit=%#x width=%d", __PRETTY_FUNCTION__, lowbit, width));
  }
}

LBitMask::LBitMask (INT width)
{
  LBitMask r(0, width);
  bmask_ = r.bmask_;
}

LBitMask::LBitMask (INT64 a)
{
  bmask_= UINT64_MAX >> countLeadingZeros64(a);
  Is_True(bmask_,("%s : unexpected zero non-top bmask created from a=%#llx",__PRETTY_FUNCTION__, a));
}

