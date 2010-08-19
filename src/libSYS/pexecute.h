/*
 * file : pexecute.h
 *
 * Header for the pexecute GCC function.
 *
 */
#ifndef __PEXECUTE_H__
#define __PEXECUTE_H__

/* execute function. */
int pexecute (const char *program,
	      char * const *argv,
	      const char *this_pname,
	      const char *temp_base,
	      char **errmsg_fmt, char **errmsg_arg,
	      int flags);

/* wait function. */
int pwait (int pid, int *status, int flags);

/* Flag arguments to pexecute.  */
#define PEXECUTE_FIRST   1
#define PEXECUTE_LAST    2
#define PEXECUTE_SEARCH  4
#define PEXECUTE_VERBOSE 8

/* signal functions .*/
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

#endif
