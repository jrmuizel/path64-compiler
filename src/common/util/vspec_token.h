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

#ifndef __VSPEC_TOKEN_H__
#define __VSPEC_TOKEN_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ll_token_t_ ll_token_t;

extern void ll_token_ctor(ll_token_t *token, 
			  int id, const char *ptr, int len);
extern void ll_token_dtor(ll_token_t *token);

extern int ll_token_id(const ll_token_t *token);
extern const char *ll_token_str(const ll_token_t *token);
extern int ll_token_len(const ll_token_t *token);

/*
 * Private definitions.
 */
struct ll_token_t_ {
  int t_id;
  int t_len;
  union {
    char *t_ptr;
    char t_buf[4];
  } u;
};

#ifdef __cplusplus
}
#endif

#endif
