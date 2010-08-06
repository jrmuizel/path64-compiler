/* Basic error reporting routines.
   Copyright (C) 1999, 2000, 2001, 2003, 2004, 2005
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */
#include <stdio.h>
#include <stdarg.h>
#include "gccfe_errors.h"

void
error (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: ", "libfereconf.a");
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc('\n', stderr);
}
