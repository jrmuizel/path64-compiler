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
