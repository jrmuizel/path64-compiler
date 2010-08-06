/*

  Copyright (C) 2001 STMicroelectronics, Inc.  All Rights Reserved.

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
 * target-specific info about elf sections.
 */

#ifndef targ_sections_INCLUDED 
#define targ_sections_INCLUDED

#include <elf.h>

/* ELF section definitions */

#define TEXT_SIZE    INT32_MAX
#define DATA_SIZE    INT32_MAX
#define SDATA_SIZE   INT32_MAX
#define LDATA_SIZE   INT32_MAX
#define RDATA_SIZE   INT32_MAX
#define SRDATA_SIZE  INT32_MAX 
#define LIT4_SIZE    INT32_MAX 
#define LIT8_SIZE    INT32_MAX 
#define LIT16_SIZE   INT32_MAX 
#define BSS_SIZE     INT32_MAX 
#define SBSS_SIZE    INT32_MAX 
#define GOT_SIZE     INT32_MAX 
#define CPLINIT_SIZE INT32_MAX 

/* special sections names */
#define SDATA_NAME   MIPS_SDATA     /* from elf header */
#define SRDATA_NAME  MIPS_SRDATA    /* from elf header */
#define LIT4_NAME    MIPS_LIT4      /* from elf header */
#define LIT8_NAME    MIPS_LIT8      /* from elf header */
#define LIT16_NAME   MIPS_LIT16     /* from elf header */
#define SBSS_NAME    MIPS_SBSS      /* from elf header */
#define CPLINIT_NAME "__cplinit"    /* is it elf ?? */


#endif /* targ_sections_INCLUDED */
