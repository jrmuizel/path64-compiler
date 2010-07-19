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
 * vspec_parse.c
 *
 * Implements support for parsing a visibility specification file
 * into a pattern list.
 *
 * The implementation uses an handcoded recursive descent parser.
 * The parser uses a tokenizer with lookahead support.
 *
 * The implemented grammar (which is LL(2)) for parsing the visibility
 * specification file is:
 *
 * top : version_decls | file_decls 
 * 						[TERM: TOK_EOF ]
 * version_decls : 'VERSION' '{' file_decls '}' ;
 * 						[FIRST: 'VERSION'  TERM: TOK_EOF ]
 * file_decls : file_decl* ; 
 *						[FIRST: 'ANY', '{' TERM: TOK_EOF, '}'  ]
 * file_decl : ANY? '{' visibility_decls '}' ANY? ';'* ;
 * 						[TERM: TOK_EOF, '{', '}', ANY ]
 * visibility_decls : default_visibility_decl visibility_decl* ;
 *						[TERM: '}']
 * default_visibility_decl : ( IDENT ':' )? pattern_decls ; 
 *						[TERM: '}', IDENT ':' ]
 * visibility_decl : IDENT ':' pattern_decls ; 
 *						[TERM: '}', IDENT ':' ]
 * pattern_decls : pattern_decl* ; 
 *						[TERM: '}', IDENT ':' ]
 * pattern_decl : lang_decl | symbol_decl ; 
 *			    [TERM: '}', IDENT ':', 'extern' STRING, ANY ';' ]
 * lang_decl : 'extern' STRING '{' symbol_decls '}' ; 
 *						[TERM: '}', IDENT ':' ]
 * symbol_decls : symbol_decl* ; 
 *						[TERM: '}' ]
 * symbol_decl : ANY ';' 
 *						[TERM: '}', IDENT ':' ]
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "vspec.h"
#include "vspec_token.h"
#include "vspec_parse.h"

#define MALLOC(x) malloc(x)
#define FREE(x) free(x)
#define ASSERT(x) assert(x)

#define TOK_UNDEF -1
#define TOK_EOF 256
#define TOK_ANY 257
#define TOK_STRING 258
#define TOK_ERROR 259
#define TOK_EOF_STR "end of file"

/*
 * type for the tokenizer state.
 * The tokenizer supports up to LL_MAX_AHEAD look ahead tokens.
 * It reads input line by line and fills its token buffer.
 * A token can not be split at line end.
 */
#define LL_MAX_AHEAD	2
#define LL_BUFFER_SIZE	1024
struct ll_state_t_ {
  int s_line;
  const char *s_fname;
  FILE *s_file;
  int s_eof;
  int s_err;
  int s_ntoks;
  struct ll_token_t_ s_toks[LL_MAX_AHEAD];
  int s_state;
  int s_bsize;
  int s_bpos;
  char *s_buffer;
};
typedef struct ll_state_t_ ll_state_t;

/* States can be or'ed together, with the following rules:
   - DEFAULT/EXTERN are exclusive
   - COMMENT can be or'ed with any other.
*/
#define STATE_DEFAULT 0
#define STATE_IN_EXTERN 1
#define STATE_IN_COMMENT 2 

/*
 * Constructor for a tokenizer state.
 */
static void
ll_state_ctor(ll_state_t *state)
{
  state->s_line = 0;
  state->s_fname = NULL;
  state->s_file = NULL;
  state->s_eof = 0;
  state->s_err = 0;
  state->s_ntoks = 0;
  state->s_state = STATE_DEFAULT;
  state->s_bsize = LL_BUFFER_SIZE;
  state->s_bpos = 0;
  state->s_buffer = (char *)MALLOC(state->s_bsize);
  state->s_buffer[0] = '\0';
}

/*
 * Destructor for a tokenizer state.
 */
static void
ll_state_dtor(ll_state_t *state)
{
  FREE(state->s_buffer);
}

/*
 * Shift one token.
 */
static void
ll_state_shift(ll_state_t *state)
{
  int i = 0;
  if (state->s_ntoks == 0) return;
  
  ll_token_dtor(&state->s_toks[0]);
  for (i = 0; i < state->s_ntoks-1; i++) {
    state->s_toks[i] = state->s_toks[i+1];
  }
  state->s_ntoks--;
}

/*
 * Refill the line buffer.
 */
static void
ll_state_refill(ll_state_t *state)
{
  if (state->s_eof) return;
  if (state->s_buffer[state->s_bpos] != '\0') return;
  
  state->s_line++;
  state->s_bpos = 0;
  if (fgets(state->s_buffer, state->s_bsize, state->s_file) == NULL) {
    state->s_eof = 1;
    if (!feof(state->s_file)) { 
      state->s_err = 1;
    }
  }
}

/*
 * ll_state_tokenize
 * Returns a token from the tokenizer state.
 */
#define PP_is_punct(state,c) ((c) == '{' || \
			(c) == '}' || \
			(c) == ';' || \
			((state)->s_state == STATE_DEFAULT && (c) == ':') || \
			0)

#define PP_is_any(state,c) (isalnum(c) || c == '_' || c == '*' || \
		      c == '[' || c == ']' || c == '?' || c == '.' || \
		      ((state)->s_state == STATE_IN_EXTERN && c == ':') || \
		      0)

static ll_token_t
ll_state_tokenize(ll_state_t *state)
{
  ll_token_t tok;
  int c, c2;

  while (1) {
    ll_state_refill(state);
    if (state->s_eof) {
      ll_token_ctor(&tok, TOK_EOF, TOK_EOF_STR, strlen(TOK_EOF_STR));
      goto found;
    }

    c = state->s_buffer[state->s_bpos];
    c2 = state->s_buffer[state->s_bpos+1];
    if (isspace(c)) goto next;	/* skip spaces and return. */
    else if ((state->s_state & STATE_IN_COMMENT)) {
      if (c == '*' && c2 == '/') {
	state->s_bpos+=2;
	state->s_state &= ~STATE_IN_COMMENT;
	continue;
      } else goto next;
    } else if (c == '/' && c2 == '*') {
      /* Start comment. */
      state->s_bpos+=2;
      state->s_state |= STATE_IN_COMMENT;
      continue;
    } else if (c == '#' ||
	       (c == '/' && c2 == '/')) {
      /* One line comment, skip to end of line. */
      state->s_buffer[state->s_bpos] = '\0';
      continue;
    } 
    else if (PP_is_punct(state, c)) {
      ll_token_ctor(&tok, c, &state->s_buffer[state->s_bpos], 1);
      state->s_bpos++;
      goto found;
    } else if (PP_is_any(state, c)) {
      int len;
      len = 1;
      c = state->s_buffer[state->s_bpos+len];
      while (PP_is_any(state, c)) {
	len++;
	c = state->s_buffer[state->s_bpos+len];
      }
      ll_token_ctor(&tok, TOK_ANY, &state->s_buffer[state->s_bpos], len);
      state->s_bpos += len;
      goto found;
    } else if (c == '\"') {
      int len;
      len = 0;
      state->s_bpos++;
      c = state->s_buffer[state->s_bpos];
      while (c != '\0' && c != '\n' && c != '\"') {
	len++;
	c = state->s_buffer[state->s_bpos+len];
      }
      if (c == '\"') {
	ll_token_ctor(&tok, TOK_STRING, &state->s_buffer[state->s_bpos], len);
	state->s_bpos += len+1;
      } else {
	ll_token_ctor(&tok, TOK_ERROR, &state->s_buffer[state->s_bpos-1], len+1);
	state->s_bpos+= len;
      }
      goto found;
    }
      
    /* Unexpected characters. */
    ll_token_ctor(&tok, TOK_ERROR, &state->s_buffer[state->s_bpos], 1);
    state->s_bpos++;
    goto found;
  next:
    state->s_bpos++;
  }
 found:
  return tok;
}

/*
 * Read up to n tokens, n must be in [1...LL_MAX_AHEAD]
 */
static void
ll_state_fetch(ll_state_t *state, int n)
{
  ASSERT(n >= 1);
  ASSERT(n <= LL_MAX_AHEAD);
  while(state->s_ntoks < n) {
    state->s_toks[state->s_ntoks] = ll_state_tokenize(state);
    state->s_ntoks++;
  }
}

/*
 * Get the look ahead token <la> in the range [0..LL_MAX_AHEAD[
 */
static ll_token_t *
ll_state_la_token(ll_state_t *state, int la)
{
  int n;
  ASSERT(la >= 0);
  ASSERT(la < LL_MAX_AHEAD);
  n = la  + 1;
  ll_state_fetch(state,n);
  return &state->s_toks[la];
}

/*
 * Helper macros.
 */
#define ll_state_token(s) ll_state_la_token(s, 0)
#define ll_state_token_id(s) ll_token_id(ll_state_token(s))
#define ll_state_token_str(s) ll_token_str(ll_state_token(s))
#define ll_state_token_match(s, id) (ll_state_token_id(s) == (id))
#define ll_state_la_token_id(s, i) ll_token_id(ll_state_la_token(s, i))
#define ll_state_la_token_str(s, i) ll_token_str(ll_state_la_token(s, i))
#define ll_state_la_token_match(s, i, id) (ll_state_la_token_id(s,i) == (id))

static int
ll_state_la_token_match2(ll_state_t *s, int la, int id, const char *str)
{
  ll_token_t *tok = ll_state_la_token(s, la);
  return ll_token_id(tok) == id && 
    strcmp(ll_token_str(tok), str) == 0;
}

#define ll_state_token_match2(s, id, str) ll_state_la_token_match2(s, 0, id, str)


/*
 * This type defines the parsing state.
 * It contains the tokenizer state and fields for
 * the parsing semantic into the <x> union.
 */
struct PP_state_t_ {
  struct ll_state_t_ ll_state_;
  struct {
    vspec_pattern_list_t *vpl;
    vspec_visi_t visi;
    vspec_lang_t lang;
  } x;
};
typedef struct PP_state_t_ PP_state_t;

/*
 * Constructs a parsing state.
 */
static void
PP_state_ctor(PP_state_t *state, vspec_pattern_list_t *vpl, const char *fname, FILE *file)
{
  ll_state_ctor(&state->ll_state_);
  state->ll_state_.s_file = file;
  state->ll_state_.s_fname = fname;
  state->x.vpl = vpl;
}

/*
 * Destructs a parsing state.
 */
static void
PP_state_dtor(PP_state_t *state)
{
  ll_state_dtor(&state->ll_state_);
  state->x.vpl = NULL;
}

/*
 * Helper macros.
 */
#define PP_fname(s) ((s)->ll_state_.s_fname)
#define PP_line(s) ((s)->ll_state_.s_line)

/*
 * Error reporting support.
 */
static char PP_err_buffer[1024];

static void
PP_set_strerr(PP_state_t *state, const char *fmt, ...)
{
  int n = 0;
  va_list v;
  va_start(v, fmt);
  n += sprintf(PP_err_buffer, "%s:%d: ", PP_fname(state), PP_line(state));
  n += vsprintf(PP_err_buffer + n, fmt, v);
  va_end(v);
}

static const char *
PP_strerr(void)
{
  return PP_err_buffer;
}
		      
/*
 * Shortcut macros for matching and getting tokens.
 */
#define PP_MATCH(s, id) ll_state_token_match(&(s)->ll_state_, id)
#define PP_MATCH_LA(s, la, id) ll_state_la_token_match(&(s)->ll_state_, la, id)
#define PP_MATCH2(s, id, str) ll_state_token_match2(&(s)->ll_state_, id, str)
#define PP_MATCH2_LA(s, la, id, str) ll_state_la_token_match2(&(s)->ll_state_, la, id, str)
#define PP_SHIFT(s) ll_state_shift(&(s)->ll_state_)
#define PP_TOK_ID(s) ll_state_token_id(&(s)->ll_state_)
#define PP_TOK_STR(s) ll_state_token_str(&(s)->ll_state_)

/*
 * Recursive descent parser.
 * This set of functions (prefixed by PP_) 
 * implement the grammar for the visibility
 * specification file. See above for the complete grammar.
 */
static int
PP_parse_token(PP_state_t *state, int id)
{
  if (!PP_MATCH(state, id)) {
    PP_set_strerr(state, "unexpected character or string: %s", PP_TOK_STR(state));
    return -1;
  }
  PP_SHIFT(state);
  return 0;
}

static int
PP_parse_symbol_decl(PP_state_t *state)
{
  int status = 0;
  if (!PP_MATCH(state, TOK_ANY)) {
    PP_set_strerr(state, "missing symbol declaration");
    status = -1;
  }
  if (status == 0) {
    vspec_pattern_list_add(state->x.vpl,
			  PP_TOK_STR(state),
			  state->x.visi,
			  state->x.lang);
    PP_SHIFT(state);
  }
  if (status == 0) status = PP_parse_token(state, ';');
  return status;
}

static int
PP_parse_symbol_decls(PP_state_t *state)
{
  int status = 0;
  while(status == 0 && !PP_MATCH(state, '}')) {
    status = PP_parse_symbol_decl(state);
  }
  return status;
}

static int
PP_parse_lang_decl(PP_state_t *state)
{
  int status = 0;
  int old_lang = state->x.lang;
  if (PP_MATCH2(state, TOK_ANY, "extern")) PP_SHIFT(state);
  else {
    PP_set_strerr(state, "missing extern declaration");
    status = -1;
  }
  if (status == 0 && !PP_MATCH(state, TOK_STRING)) {
    PP_set_strerr(state, "missing language in extern declaration");
    status = -1;
  }
  if (status == 0) {
    state->x.lang = vspec_get_lang(PP_TOK_STR(state));
    PP_SHIFT(state);
  }
  if (status == 0) status = PP_parse_token(state, '{');
  state->ll_state_.s_state = STATE_IN_EXTERN;
  if (status == 0) status = PP_parse_symbol_decls(state);
  state->ll_state_.s_state = STATE_DEFAULT;
  if (status == 0) status = PP_parse_token(state, '}');
  state->x.lang = old_lang;
  return status;
}

static int
PP_parse_pattern_decl(PP_state_t *state)
{
  int status = 0;
  if (PP_MATCH2(state, TOK_ANY, "extern") &&
      PP_MATCH_LA(state, 1, TOK_STRING)) {
    status = PP_parse_lang_decl(state);
  } else {
    status = PP_parse_symbol_decl(state);
  }
  return status;
}

static int
PP_parse_pattern_decls(PP_state_t *state)
{
  int status = 0;
  while(status == 0 && 
	!PP_MATCH(state, '}') &&
	!(PP_MATCH(state, TOK_ANY) && PP_MATCH_LA(state, 1, ':'))
	) {
    status = PP_parse_pattern_decl(state);
  }
  return status;
}

static int
PP_parse_visibility_decl(PP_state_t *state)
{
  int status = 0;
  
  if (!(PP_MATCH(state, TOK_ANY) &&
	PP_MATCH_LA(state, 1, ':'))) {
    PP_set_strerr(state, "missing visibility declaration");
    status = -1;
  }
  if (status == 0) {
    state->x.visi = vspec_get_visi(PP_TOK_STR(state));
    if (state->x.visi == VSPEC_VISI_UNDEF) {
      PP_set_strerr(state, "unexpected visibility: %s", PP_TOK_STR(state));
      status = -1;
    }
    PP_SHIFT(state);
  }
  if (status == 0) status = PP_parse_token(state, ':');
  if (status == 0) status = PP_parse_pattern_decls(state);
  return status; 
}

static int
PP_parse_default_visibility_decl(PP_state_t *state)
{
  int status = 0;
  
  if (PP_MATCH(state, TOK_ANY) &&
      PP_MATCH_LA(state, 1, ':')) {
    state->x.visi = vspec_get_visi(PP_TOK_STR(state));
    if (state->x.visi == VSPEC_VISI_UNDEF) {
      PP_set_strerr(state, "unexpected visibility: %s", PP_TOK_STR(state));
      status = -1;
    }
    PP_SHIFT(state);
    PP_SHIFT(state);
  } else {
    state->x.visi = VSPEC_VISI_DEFAULT;
  }
  if (status == 0) status = PP_parse_pattern_decls(state);
  return status; 
}

static int
PP_parse_visibility_decls(PP_state_t *state)
{
  int status = 0;

  state->x.lang = VSPEC_LANG_ASSEMBLER;
  state->x.visi = VSPEC_VISI_UNDEF;

  if (!PP_MATCH(state, '}')) {
    status = PP_parse_default_visibility_decl(state);
  }
  while(status == 0 && !PP_MATCH(state, '}')) {
    status = PP_parse_visibility_decl(state);
  }
  return status;
}

static int
PP_parse_file_decl(PP_state_t *state)
{
  int status = 0;
  if (PP_MATCH(state, TOK_ANY)) PP_SHIFT(state);
  status = PP_parse_token(state, '{');
  if (status == 0) status = PP_parse_visibility_decls(state);
  if (status == 0) status = PP_parse_token(state, '}');
  if (status == 0 && PP_MATCH(state, TOK_ANY)) PP_SHIFT(state);
  while (status == 0 && PP_MATCH(state, ';')) {
    status = PP_parse_token(state, ';');
  }
  return status;
}

static int
PP_parse_file_decls(PP_state_t *state)
{
  int status = 0;
  while(status == 0 && !(PP_MATCH(state, TOK_EOF) || PP_MATCH(state, '}'))) {
    status = PP_parse_file_decl(state);
  }
  return status;
}


static int
PP_parse_version_decls(PP_state_t *state)
{
  int status = 0;
  if (PP_MATCH2(state, TOK_ANY, "VERSION")) PP_SHIFT(state);
  else {
    PP_set_strerr(state, "missing VERSION declaration");
    status = -1;
  }
  if (status == 0) status = PP_parse_token(state, '{');
  if (status == 0) status = PP_parse_file_decls(state);
  if (status == 0) status = PP_parse_token(state, '}');
  return status;
}

static int
PP_parse_file(PP_state_t *state)
{
  int status = 0;
  if (PP_MATCH2(state, TOK_ANY, "VERSION")) {
    status = PP_parse_version_decls(state);
  } else {
    status = PP_parse_file_decls(state);
  }
  if (status == 0 && !PP_MATCH(state, TOK_EOF)) {
    PP_set_strerr(state, "trailing character found, expected end of specification");
    status = -1;
  }
  return status;
}


/*
 * Error reporting support for the vspec parser.
 */
static char vspec_parse_err_buffer[1024];

/*
 * Sets error string.
 */
static void
vspec_parse_set_strerr(const char *fmt, ...)
{
  va_list v;
  va_start(v, fmt);
  vsprintf(vspec_parse_err_buffer, fmt, v);
  va_end(v);
}

/*
 * Get error string if the parser failed.
 */
const char *
vspec_parse_strerr(void)
{
  return vspec_parse_err_buffer;
}
		      
/*
 * Parse an input file (given filename and file descriptor)
 * an return a visibility pattern list.
 * Returns NULL on failure.
 */
vspec_pattern_list_t *
vspec_parse_file(const char *fname, FILE *file)
{
  vspec_pattern_list_t *vpl;
  int status;
  PP_state_t state;
  
  vpl = vspec_pattern_list_new();
  PP_state_ctor(&state, vpl, fname, file);
  status = PP_parse_file(&state);
  PP_state_dtor(&state);
  if (status != 0) {
    vspec_pattern_list_del(vpl);
    vpl = NULL;
    vspec_parse_set_strerr("error parsing visibility file:\n%s", 
			  PP_strerr());
  }
  return vpl;
}

/*
 * Parse the given file from name <fname> and
 * return the corresponding visibility patter list.
 * Returns NULL on failure.
 */
vspec_pattern_list_t *
vspec_parse(const char *fname)
{
  vspec_pattern_list_t *vpl;
  FILE *file;
  file = fopen(fname, "r");
  if (file == NULL) {
    vspec_parse_set_strerr("unable to open file %s: %s", 
			  fname, strerror(errno));
    return NULL;
  }

  vpl = vspec_parse_file(fname, file);
  fclose(file);
  return vpl;
}

#ifdef TEST
/*
 * Compile with cc -DTEST -o t.out vspec_token.c vspec_parse.c vspec.c -liberty
 * The executable is a simple test for the parsing of visibility
 * specification files.
 * usage: ./t.out <visibility_spec> [symbols...]
 * Reads the specification file and dump it.
 * Output for each given symbol its visibility.
 */
#include <stdio.h>
#include <stdlib.h>
#include "vspec.h"
#include "vspec_parse.h"

int 
main(int argc, char *argv[])
{
  vspec_pattern_list_t *vpl;
  int i;
  if (argc < 2) {
    fprintf(stderr, "missing argument\n");
    exit(1);
  }
  vpl = vspec_parse(argv[1]);
  if (vpl == NULL) {
    fprintf(stderr, "error in visilbility file:\n  %s\n", 
	    vspec_parse_strerr());
    exit(1);
  }
  vspec_pattern_list_print(vpl, stdout);
  for (i = 2; i < argc; i++) {
    vspec_visi_t visibility = 
      vspec_pattern_list_match(vpl, argv[i]);
    if (visibility == VSPEC_VISI_UNDEF) {
      printf("%s: undefined\n", argv[i]);
    } else {
      printf("%s: %s\n", argv[i], vspec_visi_name(visibility));
    }
  }
  vspec_pattern_list_del(vpl);
  return 0;
}

#endif
