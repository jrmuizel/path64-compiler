
#include <stdlib.h>
#include "hostsys.h"

#if defined(__SVR4)
#include <synch.h>
#include <dlfcn.h>
#include <signal.h>

void *
sys_sema_new(void) 
{
  int r;
  void *sem;
  sem = malloc(sizeof(sema_t));
  return sem;
}

void
sys_sema_del(void *sem)
{
  free(sem);
}

int sys_sema_init(void *sem)
{
  return sema_init((sema_t *)sem, USYNC_THREAD, NULL,NULL);
}

int sys_sema_destroy(void *sem)
{
  return sema_destroy((sema_t *)sem);
}


int sys_sema_post(void *sem) 
{
  return sema_post((sema_t *)sem);
}

int sys_sema_wait(void *sem)
{
  return sema_wait((sema_t *)sem);
}

int sys_sema_trywait(void *sem)
{
  return sema_trywait((sema_t *)sem);
}

void *sys_dll_open(const char *path)
{
  return dlopen(path, RTLD_NOW);
}
int sys_dll_close(void *dll)
{
  return dlclose(dll);
}

void *sys_dll_sym(void *dll, const char *name)
{
  return dlsym(dll, name);
}

char *sys_dll_error(void)
{
  return dlerror();
}

void sys_wait(int usec)
{
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = usec;
  select(0,NULL, NULL, NULL, &timeout);
}

void sys_sleep(int sec)
{
  sleep(sec);
}

int sys_select_fd(int fd)
{
  int r;
  fd_set read_fdset;
  FD_ZERO(&read_fdset);
  FD_SET(fd, &read_fdset);
  
  r = select(1, &read_fdset, NULL, NULL, NULL);
  return r;
}

int sys_set_alarm(int timeout, void *handler)
{
  signal(SIGALRM, (void (*)(int))handler);
  alarm(timeout);
  return 0;
}

#elif defined(__MINGW32__)
#include <windows.h>

void *
sys_sema_new(void) 
{
  return NULL;
}

void
sys_sema_del(void *sem)
{
}

int sys_sema_init(void *sem)
{
  return 0;
}

int sys_sema_destroy(void *sem)
{
  return 0;
}


int sys_sema_post(void *sem) 
{
  return 0;
}

int sys_sema_wait(void *sem)
{
  return 0;
}

int sys_sema_trywait(void *sem)
{
  return 0;
}

void *sys_dll_open(const char *path)
{
  return LoadLibrary(path);
}

int sys_dll_close(void *dll)
{
  return !FreeLibrary(dll);
}

void *sys_dll_sym(void *dll, const char *name)
{
  return GetProcAddress(dll,name);
}

char *sys_dll_error(void)
{
  static char error_dll_message[128];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)error_dll_message,128,NULL);
  return error_dll_message;
}

void sys_wait(int usec)
{
}

void sys_sleep(int sec)
{
}

int sys_select_fd(int fd)
{
  return 0;
}

int sys_set_alarm(int timeout, void *handler)
{
  return 0;
}
#endif
