/*
 * sys_process_unix.c
 *
 * This file defines the unix version of sys_pexecute()/sys_pwait().
 * It has been tested with solaris/linux/cygwin hosts.
 */

#if !defined(__MINGW32__)
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "sys_process.h"

/*
 * Defines.
 */
#define STDIN_FILE_NO fileno(stdin)
#define STDOUT_FILE_NO fileno(stdout)
#define STDERR_FILE_NO fileno(stderr)

/*
 * Debug.
 */
#ifdef Is_True_On
#define EXEC_DEBUG
#endif
#ifdef EXEC_DEBUG
#include <stdio.h>
#include <stdlib.h>
#define exec_debug(args...) do { if (getenv("EXEC_DEBUG")) fprintf(stderr, args); } while (0)
#else
#define exec_debug(args...) (void)0
#endif

/*
 * Processes management.
 * Local process id is defined as lpid and is used only locally
 * to this file. lpid is used to maintain a lpid->lpid_pinfo_t map and the
 * list ofcurrently active child processes in lpid_info_list.
 * Valid lpid values are in the range [1..MAX_LPIDS]
 */
/* Maximum number of active child processes for the calling process. */
#define MAX_LPIDS 32

/* Process information for a lpid. */
typedef struct {
  int li_flags;
  int li_pid;
  struct timeval li_start_tv;
  struct timeval li_end_tv;
  struct rusage li_rusage;
} lpid_info_t;

/* Flags. */
#define LPID_STOP_ON_EXIT 1
#define LPID_STOPPED 2

/* List of lpid processes. */
static lpid_info_t lpid_info_list[MAX_LPIDS];

/* Accessors for lpid. */
#define lpid_info(lpid) (&lpid_info_list[(lpid)-1])
#define lpid_flags(lpid) (lpid_info(lpid)->li_flags)
#define lpid_rusage(lpid) (&lpid_info(lpid)->li_rusage)
#define lpid_pid(lpid) (lpid_info(lpid)->li_pid)
#define lpid_start_tv(lpid) (&lpid_info(lpid)->li_start_tv)
#define lpid_end_tv(lpid) (&lpid_info(lpid)->li_end_tv)

/*
 * Allocates and returns a new lpid.
 */
static int
lpid_alloc(void)
{
  int lpid;
  for (lpid = 1; lpid <= MAX_LPIDS; lpid++) {
    if (lpid_pid(lpid) == 0) return lpid;
  }
  return 0;
}

/*
 * Free a lpid. Must be called after all system specific process
 * information is released.
 */
static void
lpid_free(int lpid)
{
  lpid_pid(lpid) = 0;
}

/*
 * Returns the lpid for a process pid.
 */
static int
lpid_from_pid(int pid)
{
  int lpid;
  for (lpid = 1; lpid <= MAX_LPIDS; lpid++) {
    if (lpid_pid(lpid) == pid) return lpid;
  }
  return 0;
}


/*
 * Error messages.
 */
#define ERR_NOERR 0
#define ERR_MAX_LPID 1
#define ERR_FORK 2
#define ERR_FILE_OUT 3
#define ERR_FILE_ERR 4
#define ERR_FILE_IN 5

static char *error_msgs[] = {
  "no error",
  "maximum child process limit exceeded, cannot exec `%s'",
  "unable to fork process when launching program '%s'",
  "cannot open output file for writing `%s'",
  "cannot open error file for writing `%s'",
  "cannot open input file for reading `%s'"
};

#define set_error(err, arg) { \
  *errmsg_fmt = error_msgs[err]; \
  *errmsg_arg = (arg); \
}

/*
 * Executes the given program.
 * Returns the pid for the new process or -1 if an error occured.
 * program: program name.
 * argv: argument vector.
 * errmsg_fmt: pointer to a string for the error message.
 * errmsg_arg: pointer to a string for the error argument.
 * flags: flags (always 0 currently).
 * infile,outfile,errfile: optional inpiut, outpur, error filenames for the
 * created process.
 */
int
sys_pexecute (const char *program, const char * const argv[], const char **errmsg_fmt, const char **errmsg_arg, int flags, const char *infile, const char *outfile, const char *errfile)
{
  int fdin = STDIN_FILE_NO;
  int fdout = STDOUT_FILE_NO;
  int fderr = STDERR_FILE_NO;
  int pid;
  int lpid = 0;

  exec_debug("pexecute: LAUNCHING: %s\n", program);
  
  /* First allocate a lpid for the new process. */
  if ((lpid = lpid_alloc()) == 0) {
    exec_debug("pexecute: active processes limit exceeded (limit is %d)\n",
	       MAX_LPIDS);
    set_error(ERR_MAX_LPID, program);
    goto failed;
  }
  
  /* First redirect streams if requested. */
  if (outfile != 0) {
    if ((fdout = creat (outfile, 0666)) == -1) {
      exec_debug ("cannot create output file %s while running %s\n", outfile, program);
      set_error(ERR_FILE_OUT, outfile);
      goto failed;
    }
    exec_debug ("created output file %s while running %s\n", outfile, program);
  } 
  if (errfile != 0) {
    if ((fderr = creat (errfile, 0666)) == -1) {
      exec_debug ("cannot create output error file %s while running %s\n", errfile, program);
      set_error(ERR_FILE_ERR, outfile);
      goto failed;
    }
    exec_debug ("created error file %s while running %s\n", errfile, program);
  } 
  if (infile != 0) {
    if ((fdin = open (infile, O_RDONLY)) == -1) {
      exec_debug ("cannot open input file %s while running %s", infile, program);
      set_error(ERR_FILE_IN, outfile);
      goto failed;
    }
    exec_debug ("created input file %s while running %s\n", errfile, program);
  } 
  gettimeofday(lpid_start_tv(lpid), NULL);
  pid = fork ();
  if (pid == -1) {
    set_error(ERR_FORK, program);
    goto failed;
  }
    
  if (pid == 0) {
    /* In child, first redirect input/output/error. */
    if (fdin != STDIN_FILENO) {
      close(STDIN_FILE_NO);
      dup2(fdin, STDIN_FILE_NO);
      close(fdin);
    }
    if (fdout != STDOUT_FILENO) {
      close(STDOUT_FILE_NO);
      dup2(fdout, STDOUT_FILE_NO);
      close(fdout);
    }
    if (fderr != STDERR_FILENO) {
      close(STDERR_FILE_NO);
      dup2(fderr, STDERR_FILE_NO);
      close(fderr);
    }

    /* Execute the program. */
    execv(program, (char **)argv);
    exec_debug ("execv failed for %s\n", program);
    fprintf(stderr,"execv failed for %s\n", program);

    /* If failed we return 127. This special error code should be check
       in parent on sys_pwait() result. to detect problem on exec. */
    exit(SYS_RC_EXEC_ERROR);
    /* NOT REACHED. */
  }

  /* In parent. */
  if (fdin != STDIN_FILE_NO) close(fdin);
  if (fdout != STDOUT_FILE_NO) close(fdout);
  if (fderr != STDERR_FILE_NO) close(fderr);

  lpid_flags(lpid) = 0;
  lpid_pid(lpid) = pid;
  if (flags & SYS_P_STOP_ON_EXIT) {
    lpid_flags(lpid) |= LPID_STOP_ON_EXIT;
  }
  
  exec_debug("pexecute: LAUNCHED: pid %ld\n", pid);

  return pid;

 failed:
  if (fdin != -1 && fdin != STDIN_FILE_NO) close(fdin);
  if (fdout != -1 && fdout != STDOUT_FILE_NO) close(fdout);
  if (fderr != -1 && fderr != STDERR_FILE_NO) close(fderr);
  if (lpid != 0) lpid_free(lpid);
  return -1;
}

/*
 * Waits for completion of a process launched by sys_pexecute.
 * Returns the pid for the completed process or -1 if an error occured.
 * pid: process to wait for.
 * status: pointer to the wait status.
 * flags: flags (0 currently).
 */
int
sys_pwait (int pid, int *status, int flags)
{
  int lpid;
  if (pid == -1) {
    exec_debug("pwait: process pid invalid: pid %d\n", pid);
    return -1;
  }
  lpid = lpid_from_pid(pid);
  if (lpid == 0) {
    exec_debug("pwait: process already terminated: pid %d\n", pid);
    return -1;
  }
  if (lpid_flags(lpid) & LPID_STOPPED) {
    exec_debug("pwait: ALREADY STOPPED: lpid %d, pid %d\n", lpid, pid);
  } else {
    exec_debug("pwait: WAITING FOR: lpid %d, pid %d\n", lpid, pid);
    if (flags & SYS_P_NOHANG) {
      pid = wait4 (pid, status, WNOHANG, lpid_rusage(lpid));
      if (pid == 0) {
	/* Not finished, return 0. */
	return 0;
      }
    } else {
      pid = wait4 (pid, status, 0, lpid_rusage(lpid));
    }
    /* The child has finished, update time information. */
    gettimeofday(lpid_end_tv(lpid), NULL);
  }

  if (lpid_flags(lpid) & LPID_STOP_ON_EXIT) {
    /* The process is passed into STOPPED status. */
    lpid_flags(lpid) |= LPID_STOPPED;
  }
  if (!(lpid_flags(lpid) & LPID_STOPPED) || flags == SYS_P_TERM) {
    /* Free local lpid. */
    lpid_free(lpid);
  }
  return pid;
}

double tv_to_sec(struct timeval *tv)
{
  return tv->tv_sec + (double)tv->tv_usec / 1000000.0;
}
int
sys_print_ptime (int pid, FILE *file)
{
  int lpid;
  double utime, stime, etime;
  if (pid == -1) {
    exec_debug("print_ptime: process pid invalid: pid %d\n", pid);
    return -1;
  }
  lpid = lpid_from_pid(pid);
  if (lpid == 0) {
    exec_debug("print_ptime: process already terminated: pid %d\n", pid);
    return -1;
  }
  utime = tv_to_sec(&lpid_rusage(lpid)->ru_utime);
  stime = tv_to_sec(&lpid_rusage(lpid)->ru_stime);
  etime = tv_to_sec(lpid_end_tv(lpid)) - tv_to_sec(lpid_start_tv(lpid));
  
  fprintf(file, "%.2fu %.2fs %.2fe %.2f%%\n", utime, stime, etime, (utime+stime)/etime * 100.0);
  return 0;
}

int
sys_print_pmem (int pid, FILE *file)
{
  int lpid;
  long maxrss;
  long minflt;
  long majflt;
  if (pid == -1) {
    exec_debug("print_pmem: process pid invalid: pid %d\n", pid);
    return -1;
  }
  lpid = lpid_from_pid(pid);
  if (lpid == 0) {
    exec_debug("print_pmem: process already terminated: pid %d\n", pid);
    return -1;
  }
  maxrss = lpid_rusage(lpid)->ru_maxrss * getpagesize();
  minflt = lpid_rusage(lpid)->ru_minflt * getpagesize();
  majflt = lpid_rusage(lpid)->ru_majflt * getpagesize();
  fprintf(file, "max memory: %ldkb, min flt: %ldkb, maj_flt: %ldkb\n", maxrss/1024, minflt/1024, majflt/1024);
  return 0;
}

#else /* if !(!defined(__MINGW32__)) */
#error "unsupported host, use sys_process_win32.c instead"
#endif

#ifdef TEST_EXEC
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

int
main(int argc, char *argv[])
{
  int pid_list[100];
  char name[1024];
  char *new_argv[4];
  const char *errmsg, *errarg;
  int retval = 0;
  int id = 0;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <out_file>\n", argv[0]);
    exit(1);
  }

  if (argc < 3) {
    /* Must be the parent process. */
    int pid;
    int i = 0;
    int j;
    const char *output;
    do {
      sprintf(name, "%d", i);
      new_argv[0] = argv[0];
      new_argv[1] = argv[1];
      new_argv[2] = name;
      new_argv[3] = NULL;
      if (strcmp(argv[1], "-") == 0) output = NULL;
      else output = argv[1];
      printf("Launching child%d\n", i);
      pid = sys_pexecute(new_argv[0], (const char* const *)new_argv, 
			   &errmsg, &errarg, SYS_P_STOP_ON_EXIT, 
			   NULL, output, NULL);
      if (pid == -1) {
	printf(errmsg, errarg);
	break;
      }
      
      pid_list[i] = pid;
      i++;
    } while(i <= 5);
    for (j = 0; j < i; j++) {
      int wstatus;
      printf("Wait for child%d, pid %d\n", j, pid_list[j]);
      pid = sys_pwait(pid_list[j], &wstatus, 0);
      if (pid != -1)
	printf("Wait child%d returned: %d\n", j, (wstatus>>8));
      else 
	printf("Wait child%d FAILED\n", j); /* Should not happen. */
      printf("Time for child%d: ", j); sys_print_ptime(pid, stdout);
      printf("Mem for child%d: ", j); sys_print_pmem(pid, stdout);
      pid = sys_pwait(pid_list[j], &wstatus, SYS_P_TERM);
    }
  } else {
    /* Must be a child process. */
    int i;
    static int sum;
    char *a;
    id = atoi(argv[2]);
    fprintf(stderr, "<stderr>Child %s running, outputing into %s.\n", argv[2], argv[1]);
    fprintf(stdout, "<stdout>Child %s running.\n", argv[2]);
#define MB (1024*1024)
    /* Allocate an amount of memory and clean. */
    a = (char *)malloc(id*10*MB);
    /* Work some amount of time. */
    for (i = 0; i < id * 10*MB; i++) {
      sum += a[i];
    }
    /* Sleep some amount of time. */
    sleep(id);

    fprintf(stdout, "<stdout>Child %s end run.\n", argv[2]);
    /* Exits with number passed as first argument. */
    retval = id;
  }
  {
    struct rusage ru;
    getrusage(RUSAGE_CHILDREN, &ru);
    printf("All child max memory: %ldkb, min flt: %ldkb, maj flt: %ldkb\n", 
	   ru.ru_maxrss*getpagesize()/1024, ru.ru_minflt*getpagesize()/1024, ru.ru_majflt*getpagesize()/1024);
  }
  return retval;
}

#endif



