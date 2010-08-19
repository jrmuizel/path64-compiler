#include "W_dlfcn.h"

#if defined(NEED_DLFCN)
#if defined(__MINGW32__)
/* 
 * Includes. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/*
 * Debug support
 */
#define DLFCN_DEBUG
#ifdef DLFCN_DEBUG
#define dlfcn_debug(x) do { if (getenv("DLFCN_DEBUG")) printf x; } while(0)
#else
#define dlfcn_debug(x) (void)0
#endif

void * dlopen(const char *path, int flags)
{
  void *ptr;
  dlfcn_debug(("dlopen: %s, f:%d\n", path, flags));
  ptr = LoadLibrary(path);
  dlfcn_debug(("%p = dlopen\n", ptr));
  return ptr;
}

int dlclose(void *ptr)
{
  int res;
  dlfcn_debug(("dlclose: %p\n", ptr));
  res = FreeLibrary(ptr)? 0: -1;
  dlfcn_debug(("%d = dlclose\n", res));
  return res;
}

char *dlerror(void)
{
  static char error_message[1024];
  dlfcn_debug(("dlerror\n"));
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)error_message,sizeof(error_message),NULL);
  dlfcn_debug(("'%s' = dlerror\n", error_message));
  return error_message;
}

void *dlsym(void *ptr, const char *name)
{
  void *sym;
  dlfcn_debug(("dlsym: %p, %s\n", ptr, name));
  sym = GetProcAddress(ptr, name);
  dlfcn_debug(("%p = dlsym\n", sym));
  return sym;
}

#else /* !__MINGW32__ */
#error "Unexpected system for NEED_DLFCN"
#endif /* ! __MINGW32__ */
#endif /* defined(NEED_DLFCN) */
