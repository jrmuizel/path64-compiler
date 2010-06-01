/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


/* $Header$ */

/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	actual or intended publication of such source code.	*/

#include "cmplrs/host.h"
#include "bit.h"

/* Move len bits from position i through i+len-1 of argument m to
 *		      position j through j+len-1 of argument n.
 * The right most bit is bit 0.
 */

void
mvbits_long_long(int64_t *m,int64_t *i,int64_t *len,int64_t *n,int64_t *j)
{
        uint64_t b;

	if ( (*i + *len > NBLL) || (*j + *len > NBLL) ||
	     (*len <= 0ll) || (*i < 0ll) || (*j < 0ll) ) return;

        b = (*m >> *i) & F77llmask[*len];         /* extract bits from src */
        *n &= ~(F77llmask[*j + *len] ^ F77llmask[*j]);      /* clear dest field */
	*n |= (b << *j);                        /* position bits and insert */
}
void
mvbits_long(int32_t *m,int32_t *i,int32_t *len,int32_t *n,int32_t *j)
{
	uint32_t b;

	if ( (*i + *len > NBI) || (*j + *len > NBI) ||
	     (*len <= 0) || (*i < 0) || (*j < 0) ) return;

	b = (*m >> *i) & F77mask[*len];		/* extract bits from src */
	*n &= ~(F77mask[*j + *len] ^ F77mask[*j]);	/* clear dest field */
	*n |= (b << *j);			/* position bits and insert */
}
void
mvbits_short(int16_t *m,int16_t *i,int16_t *len,int16_t *n,int16_t *j)
{
	uint16_t b;

	/* The following test is correct (NBI versus NBSI) for VAX compatibility PV130932 */
	if ( (*i + *len > NBI) || (*j + *len > NBI) ||
	     (*len <= 0) || (*i < 0) || (*j < 0) ) return;

	b = (*m >> *i) & F77mask[*len];		/* extract bits from src */
	*n &= ~(F77mask[*j + *len] ^ F77mask[*j]);	/* clear dest field */
	*n |= (b << *j);			/* position bits and insert */
}
void
mvbits_byte(int8_t *m,int8_t *i,int8_t *len,int8_t *n,int8_t *j)
{
	uint8_t b;

	/* The following test is correct (NBI versus NBB) for VAX compatibility PV130932 */
	if ( (*i + *len > NBI) || (*j + *len > NBI) ||
	     (*len <= 0) || (*i < 0) || (*j < 0) ) return;

	b = (*m >> *i) & F77mask[*len];		/* extract bits from src */
	*n &= ~(F77mask[*j + *len] ^ F77mask[*j]);	/* clear dest field */
	*n |= (b << *j);			/* position bits and insert */
}
