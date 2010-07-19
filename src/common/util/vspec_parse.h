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
 * vspec_parse.h
 *
 * Support for parsing a visibility specification file into a pattern list.
 *
 * vspec_pattern_list_t *vspec_parse(const char *fname)
 *	Opens, parses and closes the file given as argument and returns a pattern list.
 *	Returns NULL if the parsing failed.
 *
 * vspec_pattern_list_t *vspec_parse_file(const char *fname, FILE *file)
 *	Parses a file already opened for reading and returns a pattern list.
 *	Returns NULL if the parsing failed.
 *
 * const char *vspec_parse_strerr(void)
 *	Returns a readable string for a parse error that occured while parsing a visibility 
 *	specification file.
 *
 */

#ifndef __VSPEC_PARSE_H__
#define __VSPEC_PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "vspec.h"
extern vspec_pattern_list_t *vspec_parse_file(const char *fname, FILE *file);
extern vspec_pattern_list_t *vspec_parse(const char *fname);
extern const char *vspec_parse_strerr(void);

#ifdef __cplusplus
}
#endif

#endif
