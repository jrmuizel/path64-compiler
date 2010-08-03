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
 * vspec.c
 *
 * Implements support for manipulating a visibility specification as
 * an ordered pattern list.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vspec.h"

/* 
 * Configuration.
 */
/* Define if you have -liberty. */
#define HAVE_LIBIBERTY

/* Define if you have -ldemangle. */
/*#define HAVE_LIBDEMANGLE*/
/* Define if you have fnmatch.h */
/*#define HAVE_FNMATCH_H*/


/*
 * Redefinitions.
 */
#define ASSERT(x) assert(x)
#define MALLOC(x) malloc(x)
#define FREE(x) free(x)
#define STRDUP(x) strdup(x)

/*
 * Declaration of demangle function.
 */
#ifdef HAVE_LIBDEMANGLE
#include <demangle.h>
#else
/* For libiberty. */
extern char * cplus_demangle(const char *, int);
#endif

/*
 * Declaration of fnmatch.
 */
#ifdef HAVE_FNMATCH_H
#include <fnmatch.h>
#else
/* For libiberty. */
extern int fnmatch(const char *, const char *, int);
#endif


/*
 * Type for a visibility specification pattern.
 */
struct vspec_pattern_t_ {
  struct vspec_pattern_t_ *vp_next;	/* Link to next for lists. */
  const char *vp_pattern;		/* The pattern to match. */
  vspec_visi_t vp_visi;			/* The visibility associated. */
  vspec_lang_t vp_lang;			/* The mangling language. */
};

/*
 * Type for a list of patterns.
 * This type olds the full ordered list of visibility patterns to match.
 */
struct vspec_pattern_list_t_ {
  struct vspec_pattern_t_ *vpl_first;
  struct vspec_pattern_t_ *vpl_last;
};


/*
 * Get the language name from the enumeration.
 */
const char *
vspec_lang_name(vspec_lang_t lang)
{
  static const char * const tab [] = {
    NULL, "C", "C++"};
  ASSERT(lang >= 0);
  ASSERT(lang < 3);
  return tab[lang];
}

/*
 * Get the visibility name from the enumeration.
 */
const char *
vspec_visi_name(vspec_visi_t visi)
{
  static const char * const tab []  = {
    "default", "internal", "hidden", "protected"};
  ASSERT(visi >= 0);
  ASSERT(visi < 4);
  return tab[visi];
}

/*
 * Allocate and initialize a new pattern.
 */
static vspec_pattern_t *
vspec_pattern_new(const char *vp_pattern,
		 vspec_visi_t vp_visi,
		 vspec_lang_t vp_lang)
{
  vspec_pattern_t *new_vp;
  ASSERT(vp_pattern != NULL);
  ASSERT(vp_visi != VSPEC_VISI_UNDEF);
  
  new_vp =  (vspec_pattern_t *)MALLOC(sizeof(*new_vp));
  new_vp->vp_next = NULL;
  new_vp->vp_pattern = STRDUP(vp_pattern);
  new_vp->vp_visi = vp_visi;
  new_vp->vp_lang = vp_lang;
  return new_vp;
}

/*
 * Destruct and deallocate a pattern.
 */
static void
vspec_pattern_del(vspec_pattern_t *this_vp)
{
  FREE((char *)this_vp->vp_pattern);
  FREE(this_vp);
}

/*
 * Allocate and initialize a new pattern list,
 */
vspec_pattern_list_t *
vspec_pattern_list_new(void)
{
  vspec_pattern_list_t *vlist = (vspec_pattern_list_t *)
    MALLOC(sizeof(*vlist));
  vlist->vpl_first = vlist->vpl_last = NULL;
  return vlist;
}

/*
 * Destruct and deallocate a pattern list,
 */
void
vspec_pattern_list_del(vspec_pattern_list_t *this_vpl)
{
  vspec_pattern_t *next_vp, *this_vp = this_vpl->vpl_first;
  while(this_vp != NULL) {
    next_vp = this_vp->vp_next;
    vspec_pattern_del(this_vp);
    this_vp = next_vp;
  }
  FREE(this_vpl);
}

/*
 * Add a pattern (order preserved) into the pattern list.
 */
void
vspec_pattern_list_add(vspec_pattern_list_t *this_vpl, 
		      const char *vp_pattern,
		      vspec_visi_t vp_visi,
		      vspec_lang_t vp_lang)
{
  vspec_pattern_t *new_vp;
  ASSERT(this_vpl != NULL);
  new_vp = vspec_pattern_new(vp_pattern, vp_visi, vp_lang);
  if (this_vpl->vpl_last == NULL)
    this_vpl->vpl_first = new_vp;
  else
    this_vpl->vpl_last->vp_next = new_vp;
  this_vpl->vpl_last = new_vp;
}


/*
 * Get language id from string.
 * If lang == NULL the language is assumed to be VSPEC_LANG_ASSEMBLER
 */
vspec_lang_t
vspec_get_lang(const char *lang)
{
  if (lang == NULL) 
    return VSPEC_LANG_ASSEMBLER;
  else if (strcmp(lang, "C") == 0 ||
	   strcmp(lang, "c") == 0)
    return VSPEC_LANG_C;
  else if (strcmp(lang, "C++") == 0 ||
	   strcmp(lang, "c++") == 0)
    return VSPEC_LANG_CPLUSPLUS;
  else 
    return VSPEC_LANG_UNDEF;
  
}

/*
 * Get visibility id from string.
 */
vspec_visi_t
vspec_get_visi(const char *str_visi)
{
  if (str_visi == NULL || 
      strcmp(str_visi, "default") == 0 ||
      strcmp(str_visi, "global") == 0)
    return VSPEC_VISI_DEFAULT;
  else if (strcmp(str_visi, "protected") == 0)
    return VSPEC_VISI_PROTECTED;
  else if (strcmp(str_visi, "hidden") == 0 ||
	   strcmp(str_visi, "local") == 0)
    return VSPEC_VISI_HIDDEN;
  else if (strcmp(str_visi, "internal") == 0)
    return VSPEC_VISI_INTERNAL;
  else return VSPEC_VISI_UNDEF;
}

/*
 * Match a pattern and a demangled name.
 */
static vspec_visi_t
vspec_pattern_match_demangled(const vspec_pattern_t *this_vp, const char *name)
{
  if (fnmatch (this_vp->vp_pattern, name, 0) == 0) 
    return this_vp->vp_visi;
  return VSPEC_VISI_UNDEF;
}

/*
 * Demangle a C name.
 */
static const char *
vspec_c_demangle_name(const char *name)
{
  return name;
}

/*
 * Free a demangled C name.
 */
static void
vspec_c_demangle_free(const char *name)
{
}

/*
 * Demangle a C++ name.
 */
static const char *
vspec_cplus_demangle_name(const char *name)
{
  char *alt_name;
#ifdef HAVE_LIBIBERTY
  alt_name = cplus_demangle (name, 1<<14);
#elif defined(HAVE_LIBDEMANGLE)
  alt_name = (char *)malloc(1024);
  if (cplus_demangle(name, alt_name, 1024) != 0) {
    free(alt_name);
    alt_name = NULL;
  }
#else
#error "No support for demangling"
#endif
  return alt_name;
}

/*
 * Free a C++ demangled name.
 */
static void
vspec_cplus_demangle_free(const char *name)
{
  free((char *)name);
}


/*
 * Match a symbol against the pattern list and
 * return the corresponding visibility is found or
 * VSPEC_VISI_UNDEF if not found.
 */
vspec_visi_t
vspec_pattern_list_match(const vspec_pattern_list_t *this_vpl, 
			const char *name)
{
  const vspec_pattern_t *this_vp;
  vspec_visi_t result = VSPEC_VISI_UNDEF;
  const char *cplus_name = NULL;
  const char *c_name = NULL;
  const char *demangled_name;
  char invalid_name;

  for (this_vp = this_vpl->vpl_first; this_vp != NULL; this_vp = this_vp->vp_next) {
    if (this_vp->vp_lang == VSPEC_LANG_ASSEMBLER) {
      demangled_name = name;
    } else if (this_vp->vp_lang == VSPEC_LANG_C) {
      if (c_name == NULL) {
	c_name = vspec_c_demangle_name(name);
	if (c_name == NULL) c_name = &invalid_name;
      }
      demangled_name = c_name;
    } else if (this_vp->vp_lang == VSPEC_LANG_CPLUSPLUS) {
      if (cplus_name == NULL) {
	cplus_name = vspec_cplus_demangle_name(name);
	if (cplus_name == NULL) cplus_name = &invalid_name;
      }
      demangled_name = cplus_name;
    } else {
      /* Skip unsupported language, */
      demangled_name = &invalid_name;
    }
    if (demangled_name != &invalid_name) {
      result = vspec_pattern_match_demangled(this_vp, demangled_name);
      if (result != VSPEC_VISI_UNDEF) break; /* Matched. */
    }
  }

  if (cplus_name != NULL && cplus_name != &invalid_name)
    vspec_cplus_demangle_free(cplus_name);
  if (c_name != NULL && c_name != &invalid_name)
    vspec_c_demangle_free(c_name);
  
  return result;
}

/*
 * Print a pattern list into a file.
 * The output is in the format of the visibility specification files.
 * Though it may be more verbose than needed.
 */
void
vspec_pattern_list_print(vspec_pattern_list_t *vpl, FILE *file)
{
  vspec_pattern_t *this_vp;
  vspec_visi_t prev_visi = VSPEC_VISI_UNDEF;
  vspec_lang_t prev_lang = VSPEC_LANG_ASSEMBLER;

  fprintf(file, "{\n");
  
  for (this_vp = vpl->vpl_first; this_vp != NULL; this_vp = this_vp->vp_next) {
    int visi_changed = prev_visi != this_vp->vp_visi;
    int lang_changed = prev_lang != this_vp->vp_lang;
    /* Skip unknown  language. */
    if (this_vp->vp_lang == VSPEC_LANG_UNDEF) continue;
    if (visi_changed || lang_changed) {
      if (prev_lang != VSPEC_LANG_ASSEMBLER) {
	fprintf(file, "  }\n");
	prev_lang = VSPEC_LANG_ASSEMBLER;
	prev_visi = VSPEC_VISI_UNDEF;
	visi_changed = 1;
	lang_changed = 1;
      }
    }
    if (visi_changed) {
      fprintf(file, "  %s:\n", vspec_visi_name(this_vp->vp_visi));
      prev_visi = this_vp->vp_visi;
    }
    if (lang_changed && this_vp->vp_lang != VSPEC_LANG_ASSEMBLER) {
      fprintf(file, "  extern \"%s\" {\n", vspec_lang_name(this_vp->vp_lang));
      prev_lang = this_vp->vp_lang;
    }
    fprintf(file, "    %s ;\n", this_vp->vp_pattern);
  }
  if (prev_lang != VSPEC_LANG_ASSEMBLER) {
    fprintf(file, "  }\n");
  }
  fprintf(file, "}\n");

}

