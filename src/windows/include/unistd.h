
#include_next <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readlink(const char * path, char * buf, size_t bufsize);
void *sbrk(intptr_t increment);
int symlink(const char *src, const char *dest);

#ifdef __cplusplus
}
#endif

