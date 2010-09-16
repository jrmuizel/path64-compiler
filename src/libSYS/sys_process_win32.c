/*
 * sys_process_win32.c
 *
 * This file defines MINGW32 version of sys_pexecute()/sys_pwait().
 * It has been tested with x-mingw32-gcc-3.4.2.
 * Note that sys_process_win32.c depends upon times.c for the
 * call to the function win32_update_child_times().
 */

#ifdef __MINGW32__

/*
 * Declarations
 */
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "sys_process.h"

/* Defined in win32 version of times.c. */
extern void sys_update_child_times(HANDLE child);

/*
 * Defines.
 */
#define STDIN_FILE_NO fileno(stdin)
#define STDOUT_FILE_NO fileno(stdout)
#define STDERR_FILE_NO fileno(stdout)

/*
 * Debug.
 */
#ifdef Is_True_On
#define EXEC_DEBUG 1
#endif
#ifdef EXEC_DEBUG
#include <stdio.h>
#include <stdlib.h>
#define exec_debug(args...) do { if (getenv("EXEC_DEBUG")) fprintf(stderr, args); } while (0)
#else
#define exec_debug(args...) (void)0
#endif

/* 
 * Fix argv for win32 process launching (extracted from libiberty pexecute.c).
 */
static const char * const *
fix_argv (argvec)
     const char * const *argvec;
{
  int i;
  int argc;
  char **new_argvec;
  
  for (argc = 0; argvec[argc] != 0; argc++);
  new_argvec = (char **)malloc(sizeof(char *)*(argc+1));

  new_argvec[0] = strdup(argvec[0]);
  new_argvec[argc] = NULL;
  for (i = 1; argvec[i] != 0; i++)
    {
      int len, j;
      char *temp, *newtemp;

      temp = strdup(argvec[i]);
      len = strlen (temp);
      for (j = 0; j < len; j++)
        {
          if (temp[j] == '"')
            {
              newtemp = malloc (len + 2);
              strncpy (newtemp, temp, j);
              newtemp [j] = '\\';
              strncpy (&newtemp [j+1], &temp [j], len-j);
              newtemp [len+1] = 0;
              temp = newtemp;
              len++;
              j++;
            }
        }

        new_argvec[i] = temp;
      }

  for (i = 0; new_argvec[i] != 0; i++)
    {
      if (strpbrk (new_argvec[i], " \t"))
        {
	  int len, trailing_backslash;
	  char *temp;

	  len = strlen (new_argvec[i]);
	  trailing_backslash = 0;

	  /* There is an added complication when an arg with embedded white
	     space ends in a backslash (such as in the case of -iprefix arg
	     passed to cpp). The resulting quoted strings gets misinterpreted
	     by the command interpreter -- it thinks that the ending quote
	     is escaped by the trailing backslash and things get confused. 
	     We handle this case by escaping the trailing backslash, provided
	     it was not escaped in the first place.  */
	  if (len > 1 
	      && new_argvec[i][len-1] == '\\' 
	      && new_argvec[i][len-2] != '\\')
	    {
	      trailing_backslash = 1;
	      ++len;			/* to escape the final backslash. */
	    }

	  len += 2;			/* and for the enclosing quotes. */

	  temp = malloc (len + 1);
	  temp[0] = '"';
	  strcpy (temp + 1, new_argvec[i]);
	  if (trailing_backslash)
	    temp[len-2] = '\\';
	  temp[len-1] = '"';
	  temp[len] = '\0';

	  new_argvec[i] = temp;
	}
    }

  return (const char * const *) new_argvec;
}

/*
 * Fixes argument vector for win32 and build a single string command line.
 */
#define MAX_CMDLINE 32000
static char *
get_cmd_line(const char * const *argv)
{
  static char buf[MAX_CMDLINE];
  const char *p;
  int i;
  int len = 0;
  argv = fix_argv(argv);
  for(i = 0, p = argv[i]; p != NULL; i++, p = argv[i]) {
    if (len > 0) len++;
    len += strlen(p);
  }
  if (len + 1 > MAX_CMDLINE) {
    exec_debug("pexecute: command_line too long %d (max is %d)\n", len, MAX_CMDLINE);
    return NULL;
  }
  
  len = 0;
  for(i = 0, p = argv[i]; p != NULL; i++, p = argv[i]) {
    if (len > 0) buf[len++] = ' ';
    strcpy(&buf[len], p);
    len += strlen(p);
  }
  buf[len] = '\0';
  return buf;
}


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
  PROCESS_INFORMATION li_winpi;
} lpid_info_t;

/* Flags. */
#define LPID_STOP_ON_EXIT 1
#define LPID_STOPPED 2

/* List of lpid processes. */
static lpid_info_t lpid_info_list[MAX_LPIDS];

/* Accessors for lpid. */
#define lpid_info(lpid) (&lpid_info_list[(lpid)-1])
#define lpid_flags(lpid) (lpid_info(lpid)->li_flags)
#define lpid_winpi(lpid) (&lpid_info(lpid)->li_winpi)
#define lpid_pid(lpid) (lpid_winpi(lpid)->dwProcessId)
#define lpid_handle(lpid) (lpid_winpi(lpid)->hProcess)

/*
 * Allocates and returns a new lpid.
 */
static int
lpid_alloc(void)
{
  int lpid;
  for (lpid = 1; lpid <= MAX_LPIDS; lpid++) {
    if (lpid_handle(lpid) == NULL) return lpid;
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
  lpid_handle(lpid) = NULL;
}

/*
 * Returns the lpid for an opened win32 process handle.
 */
static int
lpid_from_handle(HANDLE hHandle)
{
  int lpid;
  for (lpid = 1; lpid <= MAX_LPIDS; lpid++) {
    if (lpid_handle(lpid) == hHandle) return lpid;
  }
  return 0;
}

/*
 * Returns the lpid for an opened win32 process id.
 */
static int
lpid_from_pid(DWORD dwPid)
{
  int lpid;
  for (lpid = 1; lpid <= MAX_LPIDS; lpid++) {
    if (lpid_pid(lpid) == dwPid) return lpid;
  }
  return 0;
}

/*
 * Creates a new process and attach it to the given lpid.
 * lpid: allocated lpid for the new process.
 * program: program name.
 * cmd_line: full quoted command line.
 * fdin: input stream for the process or -1 (stdin in this case).
 * fdout: outpout stream for the process or -1 (stdout in this case).
 * fderr: error stream for the process or -1 (stderr in this case).
 * fdout and fderr can refer to the same stream for common redirection.
 */
static int
lpid_create_process(int lpid, int flags, const char *program, char *cmd_line, int fdin, int fdout, int fderr)
{
  BOOL res;
  STARTUPINFO si;

  exec_debug("creating process for lpid %d: %s\n", lpid, cmd_line);

  /* Initialize win32 startup information. */
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  if (fdin != -1)  si.hStdInput = (HANDLE)_get_osfhandle(fdin);
  else si.hStdInput = (HANDLE)_get_osfhandle(STDIN_FILE_NO);
  if (fdout != -1)  si.hStdOutput = (HANDLE)_get_osfhandle(fdout);
  else si.hStdOutput = (HANDLE)_get_osfhandle(STDOUT_FILE_NO);
  if (fderr != -1)  si.hStdError = (HANDLE)_get_osfhandle(fderr);
  else si.hStdError = (HANDLE)_get_osfhandle(STDERR_FILE_NO);
  si.dwFlags |= STARTF_USESTDHANDLES;

  /* Initialize win32 process information. */
  memset(lpid_winpi(lpid), 0, sizeof(*lpid_winpi(lpid)));
  res = CreateProcess(program, cmd_line, 
		      NULL, /* No process security attributes */
		      NULL, /* No thread security attributes */
		      TRUE, /* Inherit handles. */
		      0, /* No flags. */
		      NULL, /* Use caller environment. */
		      NULL, /* Use caller directory. */
		      &si, /* Startup info. */
		      lpid_winpi(lpid) /* Process info to be filled. */);

  lpid_flags(lpid) = 0;
  if (flags & SYS_P_STOP_ON_EXIT) {
    lpid_flags(lpid) |= LPID_STOP_ON_EXIT;
  }

#ifdef EXEC_DEBUG
  if (!res) {

    LPVOID msg;
    DWORD err = GetLastError();
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		  FORMAT_MESSAGE_FROM_SYSTEM,
		  NULL,
		  err,
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &msg,
		  0, NULL );
    exec_debug("error launching process for lpid %d:%s\n", lpid, (char *)msg);
    LocalFree(msg);
  }
#endif
  return res;
}

/*
 * Error messages.
 */
#define ERR_NOERR 0
#define ERR_MAX_LPID 1
#define ERR_MAX_CMD 2
#define ERR_FILE_OUT 3
#define ERR_FILE_ERR 4
#define ERR_FILE_IN 5
#define ERR_EXEC 6

static char *error_msgs[] = {
  "no error",
  "maximum child process limit exceeded, cannot exec `%s'",
  "maximum process command line length exceeded, cannot exec `%s'",
  "cannot open output file for writing `%s'",
  "cannot open error file for writing `%s'",
  "cannot open input file for reading `%s'",
  "error executing program `%s'"

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
  char *cmd_line;
  int fdin = STDIN_FILE_NO;
  int fdout = STDOUT_FILE_NO;
  int fderr = STDERR_FILE_NO;
  int lpid = 0;
  int is_ok;
  
  exec_debug("pexecute: LAUNCHING: %s\n", program);
  
  /* First allocate a lpid for the new process. */
  if ((lpid = lpid_alloc()) == 0) {
    exec_debug("pexecute: active processes limit exceeded (limit is %d)\n",
	       MAX_LPIDS);
    set_error(ERR_MAX_LPID, program);
    goto failed;
  }
  
  /* Process argv vector and build single string command line. */
  cmd_line = get_cmd_line(argv);
  if (cmd_line == NULL) {
    set_error(ERR_MAX_CMD, program);
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

  is_ok = lpid_create_process(lpid, flags, program, cmd_line, fdin, fdout, fderr);

  if (!is_ok) {
    set_error(ERR_EXEC, program);
    goto failed;
  }
  
  if (fdin != STDIN_FILE_NO) close(fdin);
  if (fdout != STDOUT_FILE_NO) close(fdout);
  if (fderr != STDERR_FILE_NO) close(fderr);

  exec_debug("pexecute: LAUNCHED: lpid %d, pid %ld\n", lpid, lpid_pid(lpid));

  return lpid_pid(lpid);

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
 * Note that -1 is returned if the process pid is invalid or if the process has
 * been already completed by a previous sys_pwait() call.
 * For this implementation, signal exception SIGABRT is assumed if
 * the process return value is >= 3. There is no way on win32 to guess
 * process interruption signal.
 */
int
sys_pwait (int pid, int *status, int flags)
{
  int termstat;
  int lpid;
  DWORD exitcode;
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
      if (WaitForSingleObject(lpid_handle(lpid), 0) == WAIT_TIMEOUT) {
	/* Not finished, return 0. */
	return 0;
      }
    } else {
      WaitForSingleObject(lpid_handle(lpid), INFINITE);
    }
  }
    
  GetExitCodeProcess(lpid_handle(lpid), &exitcode);
  if (lpid_flags(lpid) & LPID_STOP_ON_EXIT) {
    /* The process is passed into STOPPED status. */
    lpid_flags(lpid) |= LPID_STOPPED;
  }
  if (!(lpid_flags(lpid) & LPID_STOPPED) || flags == SYS_P_TERM) {
    /* Update children time information for the calling process. */
    sys_update_child_times(lpid_winpi(lpid)->hProcess);
    /* Terminate process. */
    CloseHandle(lpid_winpi(lpid)->hProcess);
    CloseHandle(lpid_winpi(lpid)->hThread);
    lpid_free(lpid);
  }
  /* We do not have mean to retrieve signals, so always return the error
     code. */
  /*  if (exitcode >= 3)  *status = SIGABRT; */
  *status = (exitcode & 0xff)<<8;
  return pid;
}

/*
 * Extracted from cygwin sources.
 */
#define NSPERSEC 10000000LL
static unsigned long long 
ft_to_ull (FILETIME * src)
{
  unsigned long long total = ((unsigned long long) src->dwHighDateTime << 32) +
    ((unsigned)src->dwLowDateTime);
  return total;
}


int
sys_print_ptime (int pid, FILE *file)
{
  FILETIME creation_time, exit_time, kernel_time, user_time;
  int lpid;
  if (pid == -1) {
    exec_debug("print_ptime: process pid invalid: pid %d\n", pid);
    return -1;
  }
  lpid = lpid_from_pid(pid);
  if (lpid == 0) {
    exec_debug("print_ptime: process already terminated: pid %d\n", pid);
    return -1;
  }
  if (GetProcessTimes (lpid_handle(lpid), &creation_time, &exit_time,
		       &kernel_time, &user_time) != 0) {
    unsigned long long e_time, s_time, u_time;
    e_time = (ft_to_ull(&exit_time) - 
	      ft_to_ull(&creation_time));
    s_time = ft_to_ull(&kernel_time);
    u_time = ft_to_ull(&user_time);
    /* It may happen that elapsed time is lower than user+kernel time due
       to timer granularity. Fix it if it is the case. */
    if (s_time+u_time > e_time) e_time = s_time+u_time;
    if (e_time == 0) e_time = 1;
    fprintf(file, "%.2fu %.2fs %.2fe %.2f%%\n",
	    (double)u_time/NSPERSEC,
	    (double)s_time/NSPERSEC,
	    (double)e_time/NSPERSEC,
	    (double)(u_time + s_time)/e_time * 100.0);
  } else {
#ifdef EXEC_DEBUG
    exec_debug(("print_ptime: *** Failed times\n"));
#endif
    return -1;
  }
  return 0;
}

int
sys_print_pmem (int pid, FILE *file)
{
  PROCESS_MEMORY_COUNTERS stat;
  int lpid;
  if (pid == -1) {
    exec_debug("print_pmem: process pid invalid: pid %d\n", pid);
    return -1;
  }
  lpid = lpid_from_pid(pid);
  if (lpid == 0) {
    exec_debug("print_pmem: process already terminated: pid %d\n", pid);
    return -1;
  }
  if (GetProcessMemoryInfo (lpid_handle(lpid), &stat, sizeof(stat))) {
    fprintf(file, "max memory: %lukb, max pagefile: %lukb, page flt: %lu\n", stat.WorkingSetSize/1024, stat.PeakPagefileUsage/1024, stat.PageFaultCount);
    ;
  } else {
#ifdef EXEC_DEBUG
    exec_debug(("print_pmem: *** Failed memory info\n"));
#endif
    return -1;
  }
  return 0;
}

#else /* if !defined(__MINGW32__) */
#error "unsupported host, use sys_process_unix.c instead"
#endif

#ifdef TEST_EXEC
#include "W_times.h"

int
main(int argc, char *argv[])
{
  int pid_list[100];
  char name[1024];
  char *new_argv[4];
  const char *errmsg, *errarg;
  struct tms t0, t1;
  clock_t c0, c1;
  double utime, stime, etime;
  int retval = 0;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <out_file>\n", argv[0]);
    exit(1);
  }
  c0 = times(&t0);
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
			   &errmsg, &errarg, 0, 
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
    }
  } else {
    /* Must be a child process. */
    int id = atoi(argv[2]);
    int i;
    static int sum;
    fprintf(stderr, "<stderr>%.02fu %.02fs\n", (double)t0.tms_utime/CLOCKS_PER_SEC, (double)t0.tms_stime/CLOCKS_PER_SEC);
    fprintf(stderr, "<stderr>Child %s running, outputing into %s.\n", argv[2], argv[1]);
    fprintf(stdout, "<stdout>Child %s running.\n", argv[2]);
    /* Work some amount of time. */
    for (i = 0; i < id * 10000000; i++) {
      sum += i;
    }
    /* Sleep some amount of time. */
    Sleep(id*1000);
    /* Exits with number passed as first argument. */
    retval = id;
  }

  c1 = times(&t1);
  utime = (double)(t1.tms_utime + t1.tms_cutime -
		   t0.tms_utime - t0.tms_cutime) / CLOCKS_PER_SEC;
  stime = (double)(t1.tms_stime + t1.tms_cstime -
		   t0.tms_stime - t0.tms_cstime) / CLOCKS_PER_SEC;
  etime =  (double)(c1 - c0)/ CLOCKS_PER_SEC;
  printf("%.02fu %.02fs %.02fe\n", utime, stime, etime);

  return retval;
}
#endif /* TEST_EXEC */

#ifdef TEST_CMDLINE

int
main(int argc, char *argv[])
{
  int i, n;
  char *new_argv[4];

  if (argc < 2) {
    fprintf(stderr, "usage: %s n [<arg>]\n", argv[0]);
    exit(1);
  }
  n = atoi(argv[1]);

  if (argc == 3) {
    i = strlen(argv[2]);
    printf("TEST %d \n", n);
    if (i != n) {
      printf("FAILED %d expected (%d)\n", i, n);
      exit(1);
    } else {
      printf("SUCCESS %d\n", i);
    }
  }
  if (argc == 2) {
    char *new_argv[4];
    const char *errmsg, *errarg;
    int pid, wstatus;
    char *buf;
    printf("Launch myself with an arg size of %d\n", n);
    fflush(stdout);
    buf = (char *)alloca(n+1);
    memset(buf, 'X', n);
    buf[n] = 0;
    new_argv[0] = argv[0];
    new_argv[1] = argv[1];
    new_argv[2] = buf;
    new_argv[3] = NULL;
    pid = sys_pexecute(argv[0], (const char* const *)new_argv, 
		       &errmsg, &errarg, 0, 
		       NULL, NULL, NULL);
    if (pid == -1) {
      printf("error: ");
      printf(errmsg, errarg);
      printf("\n");
      exit(1);
    }
    
    pid = sys_pwait(pid, &wstatus, 0);
    if (pid != -1) {
      if (wstatus & 0xF) {
	printf("Child signaled by: %d\n", wstatus&0xF);
	exit(1);
      } else if (wstatus>>8) {
	printf("Child failed, returned: %d\n", wstatus>>8);
	exit(1);
      }
    } else {
      printf("Wait child FAILED\n"); /* Should not happen. */
      exit(1);
    }
  }
  return 0;
}
      
#endif /* TEST_CMDLINE */
