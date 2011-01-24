/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/*
 * These are the header files that should be included
 * in most if not all the linker source file. Putting 
 * them in a single location will ensure that they will
 * be included in the correct order.
 */

#ifndef __LINKER_H__
#define __LINKER_H__

#include <sys/types.h>
#include <cmplrs/host.h>
#include <string.h>
#include <sys/inst.h>
#include "elf_defines.h"
#if !defined(__NetBSD__)
#include <sys/procfs.h>
#endif
#include <sys/elf_whirl.h>

#include "ld_elf.h"

#include "fake.h"
#include "ld_ipa_option.h"

#endif
