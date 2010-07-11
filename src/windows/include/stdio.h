
#include_next <stdio.h>

#ifndef __STDIO_PROXY_H_
#define __STDIO_PROXY_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int vasprintf(char**, const char*, __VALIST);
int asprintf(char **strp, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __STDIO_PROXY_H_ */
