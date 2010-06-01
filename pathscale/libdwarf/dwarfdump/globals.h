/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/* 
  Copyright (C) 2000,2004 Silicon Graphics, Inc.  All Rights Reserved.

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



$Header: libdwarf/dwarfdump/globals.h 1.3 05/05/25 12:44:27-07:00 bos@serpentine.pathscale.com $ */
#ifndef globals_INCLUDED
#define globals_INCLUDED

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef HAVE_ELF_H
#include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
#include <libelf.h>
#else
#ifdef HAVE_LIBELF_LIBELF_H
#include <libelf/libelf.h>
#endif
#endif
#include <dwarf.h>
#include <libdwarf.h>

typedef char * string;
typedef int boolean;
#ifndef FALSE
#define FALSE 0
#endif 
#ifndef TRUE
#define TRUE 1
#endif

/* size of attrib_buffer, defined in print_die.c */
#define ATTRIB_BUFSIZ 999

typedef struct {
    int checks;
    int errors;
} Dwarf_Check_Result;

extern int verbose;
extern boolean dense;
extern boolean ellipsis;
extern boolean dst_format;

extern boolean check_pubname_attr;
extern boolean check_attr_tag;
extern boolean check_tag_tree;
extern boolean check_type_offset;

extern Dwarf_Check_Result abbrev_code_result;
extern Dwarf_Check_Result pubname_attr_result;
extern Dwarf_Check_Result reloc_offset_result;
extern Dwarf_Check_Result attr_tag_result;
extern Dwarf_Check_Result tag_tree_result;
extern Dwarf_Check_Result type_offset_result;

extern boolean info_flag;
extern boolean use_old_dwarf_loclist;

extern char cu_name[ ];
extern boolean cu_name_flag;
extern Dwarf_Unsigned cu_offset;
extern Dwarf_Off fde_offset_for_cu_low;
extern Dwarf_Off fde_offset_for_cu_high;


extern int check_error;
extern Dwarf_Error err;
extern void print_error (Dwarf_Debug dbg, string msg,int res, Dwarf_Error err);

extern void print_line_numbers_this_cu (Dwarf_Debug dbg, Dwarf_Die in_die);
extern void print_frames (Dwarf_Debug dbg);
extern void print_pubnames (Dwarf_Debug dbg);
extern void print_macinfo (Dwarf_Debug dbg);
extern void print_locs (Dwarf_Debug dbg);
extern void print_abbrevs (Dwarf_Debug dbg);
extern void print_strings (Dwarf_Debug dbg);
extern void print_aranges (Dwarf_Debug dbg);
extern void print_relocinfo (Dwarf_Debug dbg);
extern void print_static_funcs(Dwarf_Debug dbg);
extern void print_static_vars(Dwarf_Debug dbg);
extern void print_types(Dwarf_Debug dbg);
extern void print_weaknames(Dwarf_Debug dbg);
extern void print_exception_tables(Dwarf_Debug dbg);
extern string get_fde_proc_name(Dwarf_Debug dbg, Dwarf_Addr low_pc);
extern void print_die_and_children(
	Dwarf_Debug dbg, 
	Dwarf_Die in_die,
	char **srcfiles,
	Dwarf_Signed cnt);
extern void print_one_die(
	Dwarf_Debug dbg, 
	Dwarf_Die die, 
	boolean print_information,
	char **srcfiles,
	Dwarf_Signed cnt);

#define DWARF_CHECK_ERROR(str) {\
	printf("*** DWARF CHECK: %s ***\n", str);\
	check_error ++; \
}

#define DWARF_CHECK_ERROR2(str1, str2) {\
	printf("*** DWARF CHECK: %s: %s ***\n", str1, str2);\
	check_error ++; \
}

#endif /* globals_INCLUDED */
