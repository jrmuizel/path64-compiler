
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

pid_t wait(int *stat_loc);
pid_t waitpid(pid_t pid, int *stat_loc, int options);

#ifndef WIFSIGNALED
#define WIFSIGNALED(S) (((S) & 0xff) != 0 && ((S) & 0xff) != 0x7f)
#endif

#ifndef WTERMSIG
#define WTERMSIG(S) ((S) & 0x7f)
#endif

#ifndef WIFEXITED
#define WIFEXITED(S) (((S) & 0xff) == 0)
#endif

#ifndef WEXITSTATUS
#define WEXITSTATUS(S) (((S) & 0xff00) >> 8)
#endif

#ifndef WSTOPSIG
#define WSTOPSIG WEXITSTATUS
#endif

#ifndef WCOREDUMP
#define WCOREDUMP(S) ((S) & WCOREFLG)
#endif

#ifndef WCOREFLG
#define WCOREFLG 0200
#endif

#ifndef WIFSTOPPED
#define WIFSTOPPED(S) 0
#endif

#ifdef __cplusplus
}
#endif
