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


#if 1
/* #ifdef _TARG_MIPS */

#ifndef __IPA_INTERFACE_H__
#define __IPA_INTERFACE_H__

/* Do NOT put any inlined function definitions in this header file.  We
   need to maintain a pure functional interface between ld and ipa.
 */

#ifdef __cplusplus
extern "C" {
#endif

extern void *
ld_slookup_mext(char *, boolean);

extern UINT64
ld_get_section_size (an_object_file_ptr pobj, int index);
#pragma weak ld_get_section_size

extern char *
ld_get_section_name (an_object_file_ptr pobj, int index);
#pragma weak ld_get_section_name

extern char *
ld_get_section_base (an_object_file_ptr pobj, int index);
#pragma weak ld_get_section_base

extern void *
ld_get_mmap_addr (an_object_file_ptr pobj);
#pragma weak ld_get_mmap_addr

extern unsigned int
ld_get_sym_attr (void* pext);
#pragma weak ld_get_sym_attr

extern int
ld_is_weak_symbol (void* pext);
#pragma weak ld_is_weak_symbol

extern ST_EXPORT
ld_get_export (void* pext);
#pragma weak ld_get_export

extern void
ld_set_st_idx (void* pext, ST_IDX st_idx);
#pragma weak ld_set_st_idx

extern ST_IDX
ld_get_st_idx (void* pext);
#pragma weak ld_get_st_idx


extern BOOL
ld_resolved_to_obj (void* pext, void* pobj);
#pragma weak ld_resolved_to_obj

extern void
cleanup_symtab_for_ipa (void);

extern INT 
Count_elf_external_gots (void);
#pragma weak Count_elf_external_gots

#ifdef __cplusplus
}
#endif

#endif /*  __IPA_INTERFACE_H__ */

#endif /* _TARG_MIPS */
