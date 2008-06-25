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
 * Module: lalign.h
 *
 * Description:
 *
 * Implement a lattice alignment information (LAlign) class
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LALIGN_H_INCLUDED
#define LALIGN_H_INCLUDED

#include "zint.h"

class LAlign {
 private:
  enum RangeType { normal, top };
  mUINT8 rtype;
  ZInt base_;
  ZInt bias_;
 public:
  // lattice properties
  LAlign Normalize () const;
  static const LAlign Top ();
  static const LAlign Bottom ();
  friend const LAlign Meet (const LAlign &a, const LAlign &b);
  friend const LAlign Join (const LAlign &a, const LAlign &b);
  BOOL StrictlyContains (const LAlign &a) const;  // TRUE if this < a.
  BOOL Equal (const LAlign &a) const;      // TRUE if this == a.  
  BOOL ContainsOrEqual (const LAlign &a) const {
    return StrictlyContains(a) || Equal(a); }
  // queries
  BOOL isTop () const { return rtype == top; }
  BOOL isBottom () const { return base_ == 1; }
  BOOL hasValue () const;  // TRUE if a single finite literal value.
  INT64 getValue () const; // The value if isLiteral.
  UINT64 getbase () const; // The value if isLiteral.
  UINT64 getbias () const; // The value if isLiteral.
  friend const LAlign MakeSigned (const LAlign &a, INT width);
  friend const LAlign MakeUnsigned (const LAlign &a, INT width);
  friend const LAlign LeftShift (const LAlign &a, INT width);
  friend const LAlign LeftShiftRange (const LAlign &a, const LAlign &b);
  friend const LAlign RightShift (const LAlign &a, INT width);
  friend const LAlign RightShiftRange (const LAlign &a, const LAlign &b);
  friend const LAlign BitOr (const LAlign &a, const LAlign &b);
  friend const LAlign BitAnd (const LAlign &a, const LAlign &b);
  friend const LAlign Add (const LAlign &a, const LAlign &b);
  friend const LAlign Sub (const LAlign &a, const LAlign &b);
  friend const LAlign Mul (const LAlign &a, const LAlign &b);
  friend const LAlign Div (const LAlign &a, const LAlign &b);
  friend const LAlign Mod (const LAlign &a, const LAlign &b);

  void Print (FILE *f) const;
  // constructors
  LAlign (const LAlign &a);
  LAlign (const ZInt base, const ZInt bias);
  LAlign (INT64 a);
  LAlign ();
};


#endif /* LALIGN_H_INCLUDED */
