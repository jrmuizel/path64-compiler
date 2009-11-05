/*
 * wrap_fopen.c
 *
 * This file redefines the standard library functions 
 * open, create, fopen, fdopen, freopen, remove, rename, unlink, stat for native WIN32 build.
 * Its purpose is to preprocess argument strings in order to
 * convert CYGWIN like paths specifiers into native WIN32 paths
 * It uses the GNU ld -wrap functionality to replace
 * at link time calls to fopen into calls to __wrap_fopen.
 *
 * This file must be linked with any DLL or EXE object
 * and the linker command line must have the following  option:
 * -Wl,-wrap,open,-wrap,creat,-wrap,fopen,-wrap,freopen,-wrap,remove,-wrap,rename,-wrap,unlink,-wrap,stat
 *
 */

#ifdef __MINGW32__
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "libiberty/libiberty.h"

/*
 * Declare real versions of functions.
 */
extern int __real_open(const char *pathname, int flags, mode_t mode);
extern int __real_creat(const char *pathname, mode_t mode);
extern FILE *__real_fopen (const char *path, const char *mode);
extern FILE *__real_freopen (const char *path, const char *mode, FILE *stream);
extern int __real_unlink(const char *pathname);
extern int __real_remove(const char *pathname);
extern int __real_rename(const char *oldpath, const char *newpath);
extern int __real_stat(const char *file_name, struct stat *buf);
extern int __real_chdir(const char *path);
extern int __real_rmdir(const char *pathname); 
extern DIR *__real_opendir(const char *name);

/*
 * Following is the implementation of replacement functions.
 */
int 
__wrap_open(const char *pathname, int flags, mode_t mode)
{
  int r ;
  CYGPATH(pathname) ;
  r = __real_open(pathname, flags, mode) ;
  return r ;
}

int 
__wrap_creat(const char *pathname, mode_t mode)
{
  int r ;
  CYGPATH(pathname) ;
  r = __real_creat(pathname,  mode) ;
  return r ;
}

FILE *
__wrap_fopen (const char *path, const char *mode)
{
  FILE *f ;
  CYGPATH(path) ;
  f = __real_fopen(path, mode) ;
  return f ;
}

FILE *__wrap_freopen (const char *path, const char *mode, FILE *stream)
{
  FILE *f ;
  CYGPATH(path) ;
  f = __real_freopen(path, mode, stream) ;
  return f ;
}

int __wrap_unlink(const char *pathname) 
{
  int r ;
  CYGPATH(pathname) ;
  r = __real_unlink(pathname) ;
  return r ;
}

int __wrap_remove(const char *pathname)
{
  int r ;
  CYGPATH(pathname) ;
  r = __real_remove(pathname) ;
  return r ;
}

int __wrap_rename(const char *oldpath, const char *newpath)
{
  int r ;
  CYGPATH(oldpath) ;
  CYGPATH(newpath) ;
  r = __real_rename(oldpath, newpath) ;
  return r ;
}

int __wrap_stat(const char *file_name, struct stat *buf)
{
  int r ;
  CYGPATH(file_name) ;
  r = __real_stat(file_name, buf) ;
  return r ;
}

int __wrap_chdir(const char *path)
{
  int r ;
  CYGPATH(path) ;
  r = __real_chdir(path) ;
  return r ;
}

int __wrap_rmdir(const char *pathname)
{
  int r ;
  CYGPATH(pathname) ;
  r = __real_rmdir(pathname) ;
  return r ;
}

DIR *__wrap_opendir(const char *name)
{
  DIR *d ;
  CYGPATH(name) ;
  d = __real_opendir(name) ;
  return d ;
}

#endif /* __MINGW32__ */
