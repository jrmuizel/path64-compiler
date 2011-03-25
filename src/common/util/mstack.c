/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

   Special thanks goes to SGI for their continued support to open source

*/


static char *Version = "$Source: /home/bos/bk/kpro64-pending/common/util/SCCS/s.mstack.c $ $Revision: 1.5 $";
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "mstack.h"

static int getsp ( int a );
static int fra ( int a );
static char *savestr ( char *str );
static struct frec *search_in_ftab ( int adr );


#if A_UX

struct x {
  struct x *next;
};

stack_lev(b)
  int b;
{
  struct x *l = (struct x *) (((int)(&b)) - 8);
  int a = 0;
  while (l) {
    a++;
    l = l->next;
  }
  return a;
}

trace_stack(a, b)
{
  return stack_lev()-1;
}

#else	/* not A_UX */

char **__Argv;

/*ARGSUSED*/
int trace_stack(a, b)
  int a;
  int b;
{
  return 1;	/* not implemented */
}

#endif	/* not A_UX */

