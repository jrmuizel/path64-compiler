/*
 * Stack back tracing utility functions.
 *
 * Returns a multi line string containing backtrace information.
 * Arguments are:
 * uap : frame context to analyse
 * pathInTrace: print source file in trace if not 0
 * printLongStack: print whole stack is not 0
 * numLibs/libsPP: number and list of libraries to exclude from back trace
 * debug: debugging of this facility if not 0
 * max_level: max trace level or 0 for whole stack
 */


#ifndef __STACKTRACE_H__
#define __STACKTRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

extern const char * C_StackTrace(int pathInTrace,
				 int printLongStack,
				 int numLibs,
				 const char **libsPP,
				 int debug,
				 int max_level);

extern const char * C_ContextStackTrace(void *uap,
					int pathInTrace,
					int printLongStack,
					int numLibs,
					const char **libsPP,
					int debug,
					int max_level);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

inline 
const char * ContextStackTrace(void *uap,
			       bool pathInTrace = 0,
			       bool printLongStack = 1,
			       int numLibs = 0,
			       const char **libsPP = 0,
			       int debug = 0,
			       int max_level = 0) 
{
  return C_ContextStackTrace(uap, pathInTrace, printLongStack, numLibs, libsPP, debug, max_level);
}

inline
const char * StackTrace(bool pathInTrace = 0,
			bool printLongStack = 1,
			int numLibs = 0,
			const char **libsPP = 0,
			int debug = 0,
			int max_level = 0)
{
  return C_StackTrace(pathInTrace, printLongStack, numLibs, libsPP, debug, max_level);
}


#endif

#endif /* __STACKTRACE_H__ */
