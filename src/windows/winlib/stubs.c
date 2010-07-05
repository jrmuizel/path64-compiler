
#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdio.h>

pid_t wait(int *stat_loc)
{
    fprintf(stderr, "wait() is unimplemented\n");
    return 0;
}

const char * Anl_File_Path()
{
    fprintf(stderr, "Anl_File_Path() is unimplemented\n");
    return 0;
}

struct rusage;
int getrusage(int who, struct rusage *r_usage)
{
    fprintf(stderr, "getrusage() is unimplemented\n");
    return 0;
}

char *sbrk(int increment)
{
    fprintf(stderr, "sbrk() is unimplemented\n");
    return 0;
}

__int64 
Get_Next_Construct_Id()
{
    fprintf(stderr, "Get_Next_Construct_Id() is unimplemented\n");
    return 0;
}

__int64 
New_Construct_Id()
{
    fprintf(stderr, "New_Construct_Id() is unimplemented\n");
    return 0;
}

int kill(pid_t pid, int sig)
{
    fprintf(stderr, "kill() is unimplemented\n");
    return 0;
}

int fork()
{
    fprintf(stderr, "fork() is unimplemented\n");
    return -1;
}

int getdomainname(char *name, size_t len)
{
    fprintf(stderr, "getdomainname() is unimplemented\n");
    snprintf(name, len, "fakedomain");
    return 0;
}

int gethostname(char *name, size_t namelen)
{
    fprintf(stderr, "gethostname() is unimplemented\n");
    snprintf(name, namelen, "fakehostname");
    return 0;
}

void
Handle_Signals(void)
{
    fprintf(stderr, "Handle_Signals() is unimplemented\n");
}

