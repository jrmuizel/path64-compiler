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


/* $Header$ */

#ifndef __READ_H__
#define __READ_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <ar.h>

#if 0
extern void unread_sections (an_object_file_ptr);

extern char read_headers (an_object_file_ptr, int, int);

extern void unread_obj (an_object_file_ptr, int);

extern void read_one_section (int, an_object_file_ptr);

extern UNSIGNED objs_mapped_total_size (void);

extern char *copy_section(an_object_file_ptr, int, an_elf_section_header *);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __READ_H__ */
