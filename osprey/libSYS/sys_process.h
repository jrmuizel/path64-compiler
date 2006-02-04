
#ifndef SYS_PROCESS_H
#define SYS_PROCESS_H

#ifdef  __cplusplus
extern "C" {
#endif

  extern int sys_pexecute (const char *program, const char * const argv[], const char **errmsg_fmt, const char **errmsg_arg, int flags, const char *infile, const char *outfile, const char *errfile);
  extern int sys_pwait (int pid, int *status, int flags);

  extern int sys_print_ptime (int pid, FILE *file);
 
  extern int sys_print_pmem (int pid, FILE *file);
 
  /* Return code for a child that could not execute. This code
   will be returned in the waitstatus of pwait().*/
#define SYS_RC_EXEC_ERROR 120

  /* Flags to pexecute. */
#define SYS_P_STOP_ON_EXIT 1 /* The process is launched such that 
				a wait will return with 
				a stopped process status. In this case to
				effectively terminate the process, call
				pwait with the SYS_P_TERM flag.
			     */
  
  /* Flags to pwait. */
#define SYS_P_NOHANG 1		/* Do not hang, in this case returns 0 if the child
				   did not finish yet finished. */
#define SYS_P_TERM   2		/* Force termination of the process even if
				   launched with STOP_ON_EXIT. */
#ifdef  __cplusplus
}
#endif

#endif /* !defined(SYS_PROCESS_H) */
