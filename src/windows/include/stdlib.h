
#include_next <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int mkstemps (char *pattern, int suffix_len);
int unsetenv(const char *name);
int setenv(const char *envname, const char *envval, int overwrite);

#define mkstemp(temp) mkstemps((temp), 0)

#ifdef __cplusplus
}
#endif
