
#include_next <stdio.h>

#ifndef __STDIO_H_
#define __STDIO_H_
extern int vasprintf(char**, const char*, __VALIST);
int asprintf(char **strp, const char *fmt, ...); 
#endif /* __STDIO_H_ */
