/*
 * Copyright (C) 2009. PathScale, LLC. All Rights Reserved.
 */

//
// Use catopen(3), NLSPATH, and the file base name provided by argv[1] to
// read messages and generate a file containing:
//
// char *runtime_messages[] = {
//   "first message",
//   "second message",
//   ...
//   };
//
// which can then be compiled and added to libpathfortran to avoid the need
// to find the message-catalog file at runtime via NLSPATH
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nl_types.h>
#include "liberrno.h" // From clibinc/

enum { GOOD = 0, BAD = 1 };

#define SET_NUM	1

int
main(int argc, char **argv)
{
  int i, j, k;
  char *s, *s_nonl, *p1, *p2, *p3;
  size_t sz_s = 0;
  nl_catd d;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <catfilename>\n", argv[0]);
    return BAD;
  }
  d = catopen(argv[1], 0);
  if (d == (nl_catd) -1) {
    perror(argv[1]);
    return BAD;
  }
  sz_s = 100;
  s_nonl = malloc(sz_s);
  for (i = 0; i <= LAST_LIBERRNO; i += 1) {
    if (i == 0) {
      printf("char *_Ftn_lib_runtime_messages[] = {\n");
    } else {
      printf(",\n");
    }
    if (i == 4010) {
	s_nonl[1] = '\0';
    }
    s_nonl[0] = '\0';
    p1 = s_nonl;
    s = catgets(d, SET_NUM, i, 0);
    printf("  /* %d */\t\"", i);
    if (s != NULL) {
	if (strlen(s) >= sz_s) {
	    sz_s = strlen(s)+1;
	    s_nonl = realloc(s_nonl, sz_s);
	    p1 = s_nonl;
	}
	strcpy(s_nonl, s);

	p2 = strchr(p1, '\n');
	while (p2 != NULL) {
	    *p2 = '\0';
	    p3 = strchr(p1, '"');
	    while (p3 != NULL) {
		*p3 = '\0';
		printf("%s\\\"", p1);
		p1 = p3+1;
		p3 = strchr(p1, '"');
	    }
	    printf("%s", p1);
	    p1 = p2+1;
	    p2 = strchr(p1, '\n');
	}
	p3 = strchr(p1, '"');
	while (p3 != NULL) {
	    *p3 = '\0';
	    printf("%s\\\"", p1);
	    p1 = p3+1;
	    p3 = strchr(p1, '"');
	}
    }
    printf("%s\"", p1);
  }
  printf("\n  };\n");
  catclose(d);
  return GOOD;
}
