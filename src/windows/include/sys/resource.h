
#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

typedef unsigned int rlim_t;


#define RLIM_INFINITY ((rlim_t)(((unsigned __int64)1 << 63) - 1))
#define RLIM_SAVED_MAX RLIM_INFINITY
#define RLIM_SAVED_CUR RLIM_INFINITY

#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN -1

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};

struct rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
};

#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
#define RLIMIT_NOFILE  5
#define RLIMIT_AS 6

int	getpriority(int, int);
int	getrlimit(int, struct rlimit *);
int	getrusage(int, struct rusage *);
int	setpriority(int, int, int);
int	setrlimit(int, const struct rlimit *);

#ifdef __cplusplus
}
#endif

#endif	/* !_SYS_RESOURCE_H_ */
