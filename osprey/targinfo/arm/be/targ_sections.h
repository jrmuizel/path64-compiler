/*

  Copyright (C) 2001 STMicroelectronics, Inc.  All Rights Reserved.

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

/* for now I keep MIPS and IA_64 stuff. */
#define SHF_IA_64_SHORT	SHF_MIPS_GPREL

#endif /* targ_sections_INCLUDED */
