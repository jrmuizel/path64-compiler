
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

pid_t wait(int *stat_loc);
pid_t waitpid(pid_t pid, int *stat_loc, int options);

#define WIFEXITED(s) ((s) != 259) /* STILL_ACTIVE */
#define WEXITSTATUS(s) (s)

#ifdef __cplusplus
}
#endif
