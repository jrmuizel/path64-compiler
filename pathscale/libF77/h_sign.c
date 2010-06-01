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

#include <cmplrs/host.h>

int16_t
h_sign (int16_t *a, int16_t *b)
{
    register int16_t x;
    x = (*a >= 0 ? *a : - *a);
    return( *b >= 0 ? x : -x);
}

int16_t
__hsign (int16_t a, int16_t b)
{
    register int16_t x;
    x = (a >= 0 ? a : - a);
    return( b >= 0 ? x : -x);
}
