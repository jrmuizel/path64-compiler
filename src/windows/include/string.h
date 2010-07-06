
#include_next <string.h>

#ifdef __cplusplus
extern "C" {
#endif

char *__strtok_r(char *s1, const char *s2, char **s3);
#define strtok_r __strtok_r

char * __rawmemchr(const char * s, int c_in);

char *strsignal(int sig);

#ifdef __cplusplus
}
#endif
