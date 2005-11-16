#define FAILED -1

#include <stdio.h>
#include <stdlib.h>
#include "W_unistd.h"
#include "W_signal.h"
#include "W_dlfcn.h"
#include "W_wait.h"
#include "W_unistd.h"
#include "W_mman.h"

int _sys_nerr;



void sorry_abort (char* msg)
{
    fprintf (stderr, "%s", msg);
    fflush(stdout);
    exit(0);
}

#ifdef NEED_KILL
int kill(pid_t sig, int pro)
{
    sorry_abort(" sorry, no kill function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_SBRK
void *sbrk(ptrdiff_t inc)
{
    sorry_abort(" sorry, no sbrk function available in mingw32 ! \n");
    return NULL;
}
#endif

#ifdef NEED_GETPID
pid_t getpid(void) 
{
    sorry_abort(" sorry no getpid function available in mingw32 ! \n");
    return (pid_t)FAILED;
}
#endif

#ifdef NEED_EXECV
/* Defined in x-mingw32-gcc-3.4.2 process.h */
int  execv (const char* x, char* const* y)
{
    sorry_abort(" sorry no execv function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_FORK
pid_t fork(void)
{
    sorry_abort(" sorry no fork function available in mingw32 ! \n");
    return (pid_t)FAILED;
}
#endif

#ifdef NEED_PIPE
int pipe(int * x) 
{
    sorry_abort(" sorry no pipe function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_EXECLP
int execlp (const char* x, const char* y, ...)
{
    sorry_abort(" sorry no execlp function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_REGCOMP
int regcomp(regex_t * x, const char * y, int z)
{
    sorry_abort(" sorry no regcomp function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_REGEXEC
int regexec(const regex_t * x, const char * y, size_t z, regmatch_t k[], int l)
{
    sorry_abort(" sorry no regexec function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_WAITPID
pid_t waitpid(pid_t x, int * y, int z)
{
    sorry_abort(" sorry no waitpid function available in mingw32 ! \n");
    return (pid_t)FAILED;
}
#endif

#ifdef NEED_GETRLIMIT
int getrlimit(int x, struct rlimit * y)
{
    sorry_abort(" sorry no getrlimit function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_GETRUSAGE
int getrusage(int who, struct rusage *usage)
{
  sorry_abort(" sorry no getrusage function available in mingw32 ! \n");
  return FAILED;
}
#endif

#ifdef NEED_WAIT
pid_t wait(int *statptr)
{
    sorry_abort(" sorry no wait function available in mingw32 ! \n");
    return (pid_t)FAILED;
}
#endif

#ifdef NEED_INDEX
char *index(const char *s, int c)
{
    sorry_abort(" sorry no index function available in mingw32 ! \n");
    return NULL;
}
#endif

#ifdef NEED_SYMLINK
int symlink(const char *name1, const char *name2)
{
    sorry_abort(" sorry no symlink function available in mingw32 ! \n");
    return FAILED;
}
#endif

#ifdef NEED_LINK
int link(const char * x, const char * y)
{
    sorry_abort(" sorry no link function available in mingw32 ! \n");
    return FAILED;
}
#endif
