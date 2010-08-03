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


#include <stdlib.h>
#include "vspec_token.h"

#define MALLOC(x) malloc(x)
#define FREE(x) free(x)

void
ll_token_ctor(ll_token_t *token, int id, const char *ptr, int len)
{
  int i;
  char *t_ptr;
  token->t_id = id;
  token->t_len = len;
  if (len <= 3) {
    t_ptr = &token->u.t_buf[0];
  } else {
    t_ptr = token->u.t_ptr = (char *)MALLOC(len+1);
  }
  
  for (i = 0; i < len; i++)
    t_ptr[i] = ptr[i];
  t_ptr[i] = '\0';
}

void
ll_token_dtor(ll_token_t *token)
{
  if (token->t_len > 3) {
    FREE(token->u.t_ptr);
  }
}

int
ll_token_id(const ll_token_t *token)
{
  return token->t_id;
}

const char *
ll_token_str(const ll_token_t *token)
{
  if (token->t_len <= 3) return token->u.t_buf;
  else return token->u.t_ptr;
}

int
ll_token_len(const ll_token_t *token)
{
  return token->t_len;
}

