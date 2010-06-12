/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
  Copyright (c) 2006, 2007. QLogic Corporation.  All rights reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

/* Copyright (c) 2005, 2006. PathScale, Inc.  All Rights Reserved. */

#ifndef __pmp_affinity_h
#define __pmp_affinity_h

#if defined(BUILD_OS_DARWIN) || defined(__FreeBSD__)
#include <unistd.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <linux/unistd.h>
#endif /* defined(BUILD_OS_DARWIN) */
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#if defined(__FreeBSD__)
#include <sys/cpuset.h>
#define cpu_set_t cpuset_t
#endif
/* Reverse this so we don't have to add yet another -D on the command line */
/*#ifndef CPU_ZERO */
#if 0
/* For distributions that do not provide these in <sched.h>
 * Modern distributions do provide them, and the definitions are
 * quite different to those below. */
typedef unsigned long cpu_set_t;
#define CPU_SET_SIZE (sizeof(cpu_set_t))
#define CPU_ZERO(c) ((*c) = 0)
#define CPU_SET(i,c) ((*c) |= (1L << (i)))
#define CPU_CLR(i,c) ((*c) &= ~(1L << (i)))
#define CPU_ISSET(i,c) (((*c) & (1L << (i))) != 0)
#endif

extern int __pmp_set_affinity (int cpu);
extern int __pmp_get_affinity (cpu_set_t *cpus);
extern int __pmp_has_affinity ();

#endif
