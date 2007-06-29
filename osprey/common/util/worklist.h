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
 * Module: worklist.h
 *
 * Description:
 *
 * Provides a worklist template class.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef WORKLIST_H_INCLUDED
#define WORKLIST_H_INCLUDED

#ifndef defs_INCLUDED
#include "defs.h"
#endif /* defs_INCLUDED */

#include <queue>

template<class T>
class Worklist {
 private:
  typedef std::queue<T> Members;
  Members members;
 public:
  void clear ()         { while (! members.empty()) members.pop(); }
  void push (T v)       { members.push (v); }
  void initialize (T v) { clear(); push (v); }
  BOOL empty ()         { return members.empty (); }
  T choose () {
    FmtAssert (! members.empty(),
	       ("Worklist choose() applied to empty worklist"));
    T v = members.front ();
    members.pop ();
    return v;
  }
};

#endif /* WORKLIST_H_INCLUDED */
