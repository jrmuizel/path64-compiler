/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* $Header: fake_ld/common/ld_elf.h 1.4 05/05/06 08:20:23-07:00 bos@eng-25.pathscale.com $ */

#ifndef __LD_ELF_H__
#define __LD_ELF_H__

#include <elf.h>
#include "elfaccess.h"
#include "sys/elf_whirl.h"
#include "libelf/libelf.h"

#ifdef _64BIT_OBJECTS

#define an_elf_e_header Elf64_Ehdr			    /* Elf header */
#define an_elf_program_header Elf64_Phdr		    /* Program header */
#define an_elf_section_header Elf64_Shdr		    /* Section header */
#define an_elf_sym_record  Elf64_Sym			    /* Symbol table entry */
#define an_elf_rel_record  Elf64_Rel			    /* relocation records */
#define an_elf_rela_record Elf64_Rela			    /* relocation with addend */
#define an_elf_reginfo_record Elf64_RegInfo		    /* register info */
#define an_elf_gp_reginfo_record Elf64_GP_RegInfo
#define an_elf_dyn_record Elf64_Dyn			    /* .dynamic section */
#define an_elf_lib_record Elf64_Lib			    /* .liblist */
#define an_elf_conflict_record Elf64_Conflict		    /* .conflict list */
#define an_elf_interface_descriptor Elf_Interface_Descriptor /* .MIPS.interfaces */

#define ELF_ST_BIND ELF64_ST_BIND
#define ELF_ST_TYPE ELF64_ST_TYPE
#define ELF_ST_INFO ELF64_ST_INFO

#define ELF_MS_INFO ELF64_MS_INFO   /* set msym ms_info */

#undef ELF_AR_SYMTAB_NAME
#define ELF_AR_SYMTAB_NAME      "/SYM64/         "
#define ELF_OTHER_AR_NAME       "/               "

#define ELF_WORD Elf64_Word	    /* general type */

#else

#define an_elf_e_header Elf32_Ehdr		    /* Elf header */
#define an_elf_program_header Elf32_Phdr	    /* Program header */
#define an_elf_section_header Elf32_Shdr	    /* Section header */
#define an_elf_sym_record  Elf32_Sym		    /* Symbol table entry */
#define an_elf_rel_record  Elf32_Rel		    /* relocation records */
#define an_elf_rela_record Elf32_Rela		    /* relocation with addend */
#define an_elf_reginfo_record Elf32_RegInfo	    /* register info */
#define an_elf_gp_reginfo_record Elf32_GP_RegInfo
#define an_elf_dyn_record Elf32_Dyn		    /* .dynamic section */
#define an_elf_lib_record Elf32_Lib		    /* liblist */
#define an_elf_conflict_record Elf32_Conflict	    /* conflict list */
#define an_elf_interface_descriptor Elf_Interface_Descriptor /* .MIPS.interfaces */

#define ELF_ST_BIND ELF32_ST_BIND
#define ELF_ST_TYPE ELF32_ST_TYPE
#define ELF_ST_INFO ELF32_ST_INFO

#undef ELF_AR_SYMTAB_NAME
#define ELF_AR_SYMTAB_NAME      "/               "
#define ELF_OTHER_AR_NAME       "/SYM64/         "

#define ELF_MS_INFO ELF32_MS_INFO   /* set msym ms_info */

#define ELF_WORD Elf32_Word	    /* general type */

#endif /*_64BIT_OBJECTS */

#define ELF_AR_STRING_NAME      "//              "
#undef ELF_MSYM
#define ELF_MSYM Elf32_Msym         /* Msym structure */

#endif /* __LD_ELF_H__ */
