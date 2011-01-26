/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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
 * Module: em_elf.c
 *
 * Description:
 *
 * Generate the elf headers and sections for the object file.
 *
 * ====================================================================
 * ====================================================================
 */


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "elf_defines.h"
#include <elfaccess.h>
#include <stamp.h>
#include <cmplrs/leb128.h>
#include <cmplrs/elf_interfaces.h>
/* #include <sys/unwindP.h> */

#define	USE_STANDARD_TYPES 1
#include "defs.h"
#include "erlib.h"
#include "erglob.h"
#include "config.h"
#include "targ_const.h"
#include "glob.h"
#include "config.h"
#include "config_elf_targ.h"
#include "em_elf.h"

INT GP_DISP = 0;

pSCNINFO Interface_Scn;

