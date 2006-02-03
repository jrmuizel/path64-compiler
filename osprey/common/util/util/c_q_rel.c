/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* =======================================================================
 * =======================================================================
 *
 *  Module: c_q_rel.c
 *
 * =======================================================================
 * =======================================================================
 */

static char *source_file = __FILE__;


#include "defs.h"
#include "quad.h"

extern INT c_q_lt(QUAD, QUAD, INT *);
#ifndef _NO_WEAK_SUPPORT_
#pragma weak c_q_lt = __c_q_lt
#else
extern INT c_q_lt(QUAD x, QUAD y, INT *p_err ) __attribute__((alias("__c_q_lt")));
#endif
#define	c_q_lt __c_q_lt

	/* implements the relational operator < for long doubles */

INT
c_q_lt(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi < y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo < y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_le(QUAD, QUAD, INT *);
#ifndef _NO_WEAK_SUPPORT_
#pragma weak c_q_le = __c_q_le
#else
extern INT c_q_le(QUAD x, QUAD y, INT *p_err ) __attribute__((alias("__c_q_le")));
#endif
#define	c_q_le __c_q_le

	/* implements the relational operator <= for long doubles */

INT
c_q_le(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi < y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo <= y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_eq(QUAD, QUAD, INT *);
#ifndef _NO_WEAK_SUPPORT_
#pragma weak c_q_eq = __c_q_eq
#else
extern INT c_q_eq(QUAD x, QUAD y, INT *p_err ) __attribute__((alias("__c_q_eq")));
#endif
#define	c_q_eq __c_q_eq

	/* implements the relational operator == for long doubles */

INT
c_q_eq(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( (x.hi == y.hi) && (x.lo == y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_ne(QUAD, QUAD, INT *);
#ifndef _NO_WEAK_SUPPORT_
#pragma weak c_q_ne = __c_q_ne
#else
extern INT c_q_ne(QUAD x, QUAD y, INT *p_err ) __attribute__((alias("__c_q_ne")));
#endif
#define	c_q_ne __c_q_ne

	/* implements the relational operator != for long doubles */

INT
c_q_ne(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( (x.hi != y.hi) || (x.lo != y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_gt(QUAD, QUAD, INT *);
#ifndef _NO_WEAK_SUPPORT_
#pragma weak c_q_gt = __c_q_gt
#else
extern INT c_q_gt(QUAD x, QUAD y, INT *p_err ) __attribute__((alias("__c_q_gt")));
#endif
#define	c_q_gt __c_q_gt

	/* implements the relational operator > for long doubles */

INT
c_q_gt(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi > y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo > y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_ge(QUAD, QUAD, INT *);
#ifndef _NO_WEAK_SUPPORT_
#pragma weak c_q_ge = __c_q_ge
#else
extern INT c_q_ge(QUAD x, QUAD y, INT *p_err ) __attribute__((alias("__c_q_ge")));
#endif
#define	c_q_ge __c_q_ge

	/* implements the relational operator >= for long doubles */

INT
c_q_ge(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi > y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo >= y.lo) )
		return ( 1 );

	return ( 0 );
}

