#define FAILED -1

#include <stdio.h>
#include "unistd.h"
#include "regex.h"
#include "sys/resource.h"


int _sys_nerr;



void sorry_abort (char* msg)
{
    fprintf (stderr, "%s", msg);
    fflush(stdout);
    exit(0);
}


int kill(unsigned long sig, int pro)
{
    sorry_abort(" sorry, no kill function available in mingw32 ! \n");
    return FAILED;
}

int mkstemp(char* x)
{
    sorry_abort(" sorry no mkstemp function available in mingw32 ! \n");
    return FAILED;
}

pid_t getpid(void) 
{
    sorry_abort(" sorry no getpid function available in mingw32 ! \n");
    return (pid_t)FAILED;
}

int  execv (const char* x, char* const* y)
{
    sorry_abort(" sorry no execv function available in mingw32 ! \n");
    return FAILED;
}

pid_t fork(void)
{
    sorry_abort(" sorry no fork function available in mingw32 ! \n");
    return (pid_t)FAILED;
}

int pipe(int * x) 
{
    sorry_abort(" sorry no pipe function available in mingw32 ! \n");
    return FAILED;
}

int     execlp (const char* x, const char* y, ...)
{
    sorry_abort(" sorry no execlp function available in mingw32 ! \n");
    return FAILED;
}

void *sbrk(intptr_t incr)
{
    sorry_abort(" sorry no execlp function available in mingw32 ! \n");
}

void *mmap(void * x, size_t y, int z, int k, int l, off_t m)
{
    sorry_abort(" sorry no mmap function available in mingw32 ! \n");
}

int munmap(void * x,  size_t y)
{
    sorry_abort(" sorry no munmap function available in mingw32 ! \n");
    return FAILED;
}

int regcomp(regex_t * x, const char * y, int z)
{
    sorry_abort(" sorry no regcomp function available in mingw32 ! \n");
    return FAILED;
}

int regexec(const regex_t * x, const char * y, size_t z, regmatch_t k[], int l)
{
    sorry_abort(" sorry no regexec function available in mingw32 ! \n");
    return FAILED;
}

pid_t waitpid(pid_t x, int * y, int z)
{
    sorry_abort(" sorry no waitpid function available in mingw32 ! \n");
    return (pid_t)FAILED;
}

int getrlimit(int x, struct rlimit * y)
{
    sorry_abort(" sorry no getrlimit function available in mingw32 ! \n");
    return FAILED;
}

void * dlopen(const char * x, int y)
{
    sorry_abort(" sorry no dlopen function available in mingw32 ! \n");
    return NULL;
}

int dlclose(void * x)
{
    sorry_abort(" sorry no dlclose function available in mingw32 ! \n");
    return FAILED;
}

char * dlerror(void)
{
    sorry_abort(" sorry no dlerror function available in mingw32 ! \n");
    return NULL;
}

pid_t wait(int *statptr)
{
    sorry_abort(" sorry no wait function available in mingw32 ! \n");
    return (pid_t)FAILED;
}

char *index(const char *s, int c)
{
    sorry_abort(" sorry no index function available in mingw32 ! \n");
    return NULL;
}

int symlink(const char *name1, const char *name2)
{
    sorry_abort(" sorry no symlink function available in mingw32 ! \n");
    return FAILED;
}

int link(const char * x, const char * y)
{
    sorry_abort(" sorry no link function available in mingw32 ! \n");
    return FAILED;
}
