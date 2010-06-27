
#include_next <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readlink(const char * path, char * buf, size_t bufsize);

#ifdef __cplusplus
}
#endif

