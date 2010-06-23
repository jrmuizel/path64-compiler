
#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

//#define PRIO_MIN	-20
//#define PRIO_MAX	20
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

struct	rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
#if 0
	long	ru_maxrss;
	long	ru_ixrss;
	long	ru_idrss;
	long	ru_isrss;
	long	ru_minflt;
	long	ru_majflt;
	long	ru_nswap;
	long	ru_inblock;
	long	ru_oublock;
	long	ru_msgsnd;
	long	ru_msgrcv;
	long	ru_nsignals;
	long	ru_nvcsw;
	long	ru_nivcsw;
#define	ru_first	ru_ixrss
#define	ru_last		ru_nivcsw
#endif
};

#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
//#define RLIMIT_RSS 5
//#define RLIMIT_MEMLOCK 6
//#define RLIMIT_NPROC   7
#define RLIMIT_NOFILE  8
//#define RLIMIT_SBSIZE  9
//#define RLIM_NLIMITS  10
#define RLIMIT_AS 11

int	getpriority(int, int);
int	getrlimit(int, struct rlimit *);
int	getrusage(int, struct rusage *);
int	setpriority(int, int, int);
int	setrlimit(int, const struct rlimit *);

#endif	/* !_SYS_RESOURCE_H_ */

#if 0
struct orlimit {
    int32_t rlim_cur;		/* current (soft) limit */
    int32_t rlim_max;		/* maximum value for rlim_cur */
};


/* Load average structure. */
struct loadavg {
	fixpt_t	ldavg[3];
	long	fscale;
};
#endif
