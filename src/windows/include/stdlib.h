
#include_next <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int mkstemp(char *temp);
int unsetenv(const char *name);
int setenv(const char *envname, const char *envval, int overwrite);

#ifdef __cplusplus
}
#endif
