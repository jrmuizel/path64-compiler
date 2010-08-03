/*
  Copyright (C) 2004, STMicroelectronics, All Rights Reserved.

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
*/

/*
 * vspec.h
 *
 * Support for managing visibility specification.
 *
 * A visibility specification consists of an ordered list of pattern
 * to match against symbol names.
 * Each pattern in the list has an associated visibility and a mangling 
 * language. 
 * When a symbol is matched against a pattern, it is first demangled
 * according to the mangling language of the pattern and then matched against
 * the pattern by globing (as for shell expansion).
 *
 * vspec_pattern_list_t
 *	Type that handle a list of ordered patterns where each pattern
 *      is associated with a visibility and a mangling language.
 *
 * vspec_pattern_list_t *vspec_pattern_list_new(void)
 *	Allocates and initializes a new empty pattern list.
 *
 * void vspec_pattern_list_del(vspec_pattern_list_t *this_vpl)
 *	Deallocates a pattern list.
 *
 * void vspec_pattern_list_add(vspec_pattern_list_t *this_vpl, 
 *				  const char *vp_pattern,
 *				  vspec_visi_t vp_visi,
 *				  vspec_lang_t vp_lang)
 *	Add a new pattern specification to the pattern list.
 *	A pattern specification consists of a triplet where:
 *	vp_pattern is a symbol pattern string (with the standard 
 * 	globing wildcards) used to match symbol names,
 *	vp_visi is the visibility associated with the matched symbols,
 *	vp_lang is the language for the symbols that must be matched.
 *
 * vspec_visi_t vspec_pattern_list_match(const vspec_pattern_list_t *vpl, 
 *					 const char *name);
 *	Returns the visibility associated with the symbol given by name
 *	in the pattern list given by vpl.
 *	Returns VSPEC_VISI_UNDEF if no pattern matched the symbol name.
 * 
 * void vspec_pattern_list_print(vspec_pattern_list_t *vpl, FILE *file)
 *	Prints a visibility specification into the given file.
 *
 */

#ifndef __VSPEC_H__
#define __VSPEC_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vspec_pattern_t_ vspec_pattern_t;
typedef struct vspec_pattern_list_t_ vspec_pattern_list_t;


enum vspec_visi_t_ {
  VSPEC_VISI_UNDEF = -1,
  VSPEC_VISI_DEFAULT = 0,
  VSPEC_VISI_INTERNAL = 1,
  VSPEC_VISI_HIDDEN = 2 ,
  VSPEC_VISI_PROTECTED = 3
};
typedef enum vspec_visi_t_ vspec_visi_t;

enum vspec_lang_t_ {
  VSPEC_LANG_UNDEF = -1,
  VSPEC_LANG_ASSEMBLER = 0,
  VSPEC_LANG_C,
  VSPEC_LANG_CPLUSPLUS
};
typedef enum vspec_lang_t_ vspec_lang_t;

extern vspec_pattern_list_t *vspec_pattern_list_new(void);
extern void vspec_pattern_list_del(vspec_pattern_list_t *this_vpl);
extern void vspec_pattern_list_add(vspec_pattern_list_t *this_vpl, 
				  const char *vp_pattern,
				  vspec_visi_t vp_visi,
				  vspec_lang_t vp_lang);

extern vspec_visi_t vspec_pattern_list_match(const vspec_pattern_list_t *vpl, 
					     const char *name);

extern vspec_lang_t vspec_get_lang(const char *str_lang);
extern vspec_visi_t vspec_get_visi(const char *str_visi);
extern const char *vspec_lang_name(vspec_lang_t lang);
extern const char *vspec_visi_name(vspec_visi_t visi);

extern void vspec_pattern_list_print(vspec_pattern_list_t *vpl, FILE *);

#ifdef __cplusplus
}
#endif

#endif
