
#ifndef sys_types_h
#include "sys/types.h"
#define sys_types_h
#endif

#ifndef stddef_h
#include "stddef.h"
#define stddef_h
#endif

#ifndef ctype_h
#include "ctype.h"
#define ctype_h
#endif

#ifndef stdlib_h
#include "stdlib.h"
#define stdlib_h
#endif /* stdlib_h */

#ifndef unistd_h
#include "unistd.h"
#define unistd_h
#endif /* unistd_h */

#ifndef stdio_h
#include "stdio.h"
#define stdio_h
#endif /* stdio_h */

#ifndef string_h
#include "string.h"
#define string_h
#endif /* string_h */

#ifndef errno_h
#include "errno.h"
#define errno_h
#endif /* errno_h */

#ifndef pexecute_h
#include "pexecute.h"
#define pexecute_h
#endif /* pexecute_h */

typedef int bool;
#define true 1
#define false 0

#ifndef SYS_h
#include "SYS.h"
#define SYS_h
#endif /* SYS_h */

#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#define DIR_SEPARATOR2 '/'
#define HAS_DRIVE_LETTER
#define DRIVE_SEPARATOR ':'
#else
#define DIR_SEPARATOR '/'
#endif

#ifdef _WIN32
#ifdef ERROR
#undef ERROR
#endif
#include <windows.h>
#define MAX_PATH_LENGTH 1024
char win32_path[MAX_PATH_LENGTH];
#endif

const char *SYS_programname;

/* Import getenv. */
#ifndef getenv
extern char *getenv ();
#endif
void *SYS_malloc(int size)
{
  return malloc(size);
}

void *SYS_free(void *ptr)
{
  if (ptr)
    free(ptr);
  return NULL;
}

void *SYS_realloc(void *ptr, int size)
{
  return realloc(ptr, size);
}

void *SYS_calloc(int n, int size)
{
  return calloc(n, size);
}

void *SYS_revert(void *ptr, int size)
{
  int i;
  uint8_t *p = (uint8_t *)ptr;
  uint8_t swap;

  for (i = 0; i < size/2; i++)
    {
      swap = p[i];
      p[i] = p[size - i - 1];
      p[size - i - 1] = swap;
    }

  return ptr;
}

void *SYS_mempattern(void *dst, int dst_size, const void *pattern, int pattern_size)
{
  uint8_t *bptr = (uint8_t *)dst;

  if (pattern_size)
    {
      while (dst_size >= pattern_size)
	{
	  memcpy(bptr, pattern, pattern_size);
	  bptr += pattern_size;
	  dst_size -= pattern_size;
	}

      if (dst_size)
        memcpy(bptr, pattern, dst_size);
    }

  return dst;
}

char *SYS_strdup(const char *string)
{
  return strdup(string);
}

const char *SYS_strdupn(const char *string, int n)
{
  char *ptr = SYS_malloc(sizeof(char)*(n+1));
  strncpy(ptr, string, n);
  ptr[n] = '\0';
  return (const char *)ptr;
}

char *SYS_strappend(char *dest, const char *src)
{
    while (*dest++ = *src++);
    return dest-1;
}

char SYS_getDirSeparator(void)
{
  return DIR_SEPARATOR;
}

bool SYS_isDirSeparator(char c)
{
  return c == DIR_SEPARATOR
#ifdef DIR_SEPARATOR2
	|| c == DIR_SEPARATOR2
#endif
	;
}

bool SYS_isAbsolute(const char *filename)
{
#ifdef HAS_DRIVE_LETTER
  return isalpha(filename[0]) && filename[1] == DRIVE_SEPARATOR &&
	SYS_isDirSeparator(filename[2]);
#else 
  return SYS_isDirSeparator(filename[0]);
#endif
}

char *SYS_makePath(const char *dirname, const char *basename)
{
    ASSERT(basename != NULL);
    ASSERT(dirname != NULL);
    {
      char *ptr;
      int baselen = strlen(basename);
      int dirlen = strlen(dirname);
      char *path = SYS_malloc(sizeof(char)*(dirlen+baselen+2));
      ptr = SYS_strappend(path, dirname);
      if (*dirname != '\0' && !SYS_isDirSeparator(dirname[dirlen-1])) {
	*ptr++ = SYS_getDirSeparator();
      }
      SYS_strappend(ptr, basename);
      return path;
    }
}

const char *SYS_makeFileName(const char *path, const char *suffix)
{
    ASSERT(path != NULL);
    ASSERT(suffix != NULL);
    {
      int pathlen = strlen(path);
      int suffixlen = strlen(suffix);
      char *p = SYS_malloc(sizeof(char)*(pathlen+suffixlen+1));
      strcpy(p, path); 
      strcpy(p+pathlen, suffix);
      return p;
    }
}

const char *SYS_baseptr(const char *path)
{
    const char *p;
    p = strrchr(path, DIR_SEPARATOR);
#ifdef DIR_SEPARATOR2
    if (p == NULL) p = strrchr(path, DIR_SEPARATOR2);
#endif
    return p == NULL ? path : p+1;
}

char *SYS_basename(const char *path)
{
    return SYS_strdup(SYS_baseptr(path));
}

bool SYS_matchsuffix(const char *path, const char *suffix)
{
  bool match = true;
  const char *p1 = suffix + strlen(suffix);
  const char *p2 = path + strlen(path);
  while(p1 > suffix) {
    p1--; p2--;
    if (p2 < path || (*p1 != *p2)) {
      match = false;
      break;
    }
  }
  return match;
}

const char *SYS_baseprefix(const char *path, const char *suffix)
{
  bool match = true;
  const char *p = SYS_basename(path);
  const char *p1 = suffix + strlen(suffix);
  char *p2 = (char *)p + strlen(p);
  while(p1 > suffix) {
    p1--; p2--;
    if (p2 < p || (*p1 != *p2)) {
      match = false;
      break;
    }
  }
  if (match) {
    *p2 = '\0';
  }
  return (const char *)p;
}

char *SYS_dirname(const char *path)
{
    const char *p;
    p = strrchr(path, DIR_SEPARATOR);
#ifdef DIR_SEPARATOR2
    if (p == NULL) p = strrchr(path, DIR_SEPARATOR2);
#endif
    if (p == NULL) {
	return SYS_strdup(".");
    } else {
	char *dirname = SYS_malloc(sizeof(char)*(p-path+1));
	if (p-path > 0) {
	  memcpy(dirname, path, p-path);
	}
	dirname[p-path] = '\0';
	return dirname;
    }
}

int SYS_is_dir(const char *path)
{
  int ret = 0;
  struct stat buffer;
  if (stat(path, &buffer) != -1 &&
	buffer.st_mode & S_IFDIR) ret = 1;
  return ret;
}

int SYS_is_file(const char *path)
{
  struct stat buffer;
  if (stat(path, &buffer) == -1) return 0;
  if (buffer.st_mode & S_IFREG) return 1;
  return 0;
}

char *SYS_tmpdir(void)
{
  char *tmpdir;
#ifdef _WIN32
  if ((tmpdir = getenv("TMPDIR")) != NULL && SYS_is_dir(tmpdir)) return SYS_strdup(tmpdir);
  if ((tmpdir = getenv("TMP")) != NULL && SYS_is_dir(tmpdir)) return SYS_strdup(tmpdir);
  if ((tmpdir = getenv("TEMP")) != NULL && SYS_is_dir(tmpdir)) return SYS_strdup(tmpdir);
  if (SYS_is_dir("\\tmp")) return SYS_strdup("\\tmp");
  return SYS_strdup("\\");
#else
  if ((tmpdir = getenv("TMPDIR")) != NULL && SYS_is_dir(tmpdir)) return SYS_strdup(tmpdir);
  if (SYS_is_dir(P_tmpdir)) return SYS_strdup(P_tmpdir);
  return SYS_strdup("/tmp");
#endif
}

char *SYS_tmpname(void)
{

  /* tmpnam() always returns T_tmpdir. The following allows
a more flexible scheme. */
  const char *tmpfile = tmpnam(NULL);
  const char *filename = SYS_baseptr(tmpfile);
  char *tmpdir = SYS_tmpdir();
  char *path = SYS_makePath(tmpdir, filename);
  return path;
}

int SYS_unlink(const char *filename)
{
  int status;
  status = unlink(filename);
  return status;
}

int SYS_execute(const char *program, char * const *argv, int flags)
{
    char *errmsg_fmt, *errmsg_arg;
    int ret = 0;
    int status = 0;
    int pid = 0;

    if (flags & SYS_FLG_VERBOSE) {
      int i;
      for (i = 0; argv[i] != NULL; i++) {
	fprintf(stderr, "%s ", argv[i]);
      }
      fprintf (stderr, "\n");
    }

    if (!(flags & SYS_FLG_DRYRUN)) {
      pid = pexecute(program, argv,
	SYS_programname, NULL, 
	&errmsg_fmt, &errmsg_arg, flags & SYS_FLG_SEARCH ? PEXECUTE_SEARCH : 0);

      if (pid == -1) {
        fprintf (stderr, "%s: can't lauch %s: \n", SYS_programname, program);
        fprintf (stderr, errmsg_fmt, errmsg_arg);
        fprintf (stderr, "\n");
      } else {
        pid = pwait(pid, &status, 0);
        if (pid < 0) {
          ABORT();
        }
      }
    }

    if (pid == -1) {
	ret = -1;
    } else {
      if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        fprintf (stderr, "%s: sub process %s returned status %d\n", SYS_programname, program, WEXITSTATUS(status));
        ret = -1;
      } else if (WIFSIGNALED(status)) {
        fprintf (stderr, "%s: sub process %s received signal %d\n", SYS_programname, program, WTERMSIG(status));
        ret = -1;
      }
    } 
    return ret;
}

char *SYS_getpdir_w32(const char *program);
char *SYS_getpdir_w32(const char *program)
{
  char *pdir = NULL;
#ifdef _WIN32
  HMODULE mod;
  char *ignored ;  
  if((mod = GetModuleHandle (NULL)) != NULL) {
    if ((GetModuleFileName (mod, win32_path, MAX_PATH_LENGTH)) != 0) {
      pdir = SYS_dirname(win32_path);
    } else if (GetFullPathName(program, MAX_PATH_LENGTH, win32_path, &ignored)) {
	pdir = SYS_dirname(win32_path);
    } else { 
	fprintf (stderr, "%s: cannot find installation directory (GetLastError: %lu)\n", SYS_programname, GetLastError()) ;
    }
  }
#endif
  return pdir;
}

static  char *SYS_getpdir_unix(const char *pname);
static char *SYS_getpdir_unix(const char *pname)
{
  char *pdir = NULL;
#ifndef _WIN32
  const char *path;
  const char *next;
  char *path_ptr;
  int dir_len;
  int pname_len = strlen(pname);
  char *file_buffer = SYS_malloc(1024+1);
  if (SYS_isAbsolute(pname)) {
    if (pname_len + 1 > 1024+1) {}
    else { strcpy(file_buffer, pname); pdir = file_buffer; }
  } else if (strchr(pname, DIR_SEPARATOR) != NULL) {
    const char *pwd;
    pwd = getcwd(file_buffer, 1024+1);
    if (pwd != NULL) {
      dir_len = strlen(pwd);
      if (dir_len + pname_len + 1 + 1 > 1024+1) {}
      else {
        path_ptr = file_buffer + dir_len;
        *path_ptr = DIR_SEPARATOR;
        path_ptr += 1;
        strncpy(path_ptr, pname, pname_len);
        path_ptr += pname_len;
        *path_ptr = '\0';
        if (access(file_buffer, F_OK) == 0) pdir = file_buffer;
      }
    }
   } else {
    path = getenv("PATH");
    while (path != NULL) {
      next = strchr(path, ':');
      if (next == NULL) {
	dir_len = strlen(path);
      } else {
	dir_len = next - path;
	next++;
      }
      if (dir_len == 0) continue;
      if (dir_len + pname_len + 1 + 1 > 1024+1) break;
      path_ptr = file_buffer;
      strncpy(path_ptr, path, dir_len);
      path_ptr += dir_len;
      if (*(path_ptr-1) != DIR_SEPARATOR) {
	*path_ptr = DIR_SEPARATOR;
	path_ptr += 1;
      }
      strncpy(path_ptr, pname, pname_len);
      path_ptr += pname_len;
      *path_ptr = '\0';
      if (access(file_buffer, F_OK) == 0) {
	pdir = file_buffer;
	break;
      }
      path = next;
    }
  }
  if (pdir == NULL) SYS_free(file_buffer);
  else {
    char *sep = strrchr(pdir, DIR_SEPARATOR);
    if (sep != NULL) *sep = '\0';
  }
#endif
  return pdir;
}

char *SYS_getpdir(const char *program)
{
  char *path = NULL;
#ifdef _WIN32
  path = SYS_getpdir_w32(program);
#else
  path = SYS_getpdir_unix(program);
#endif
  return path;
}

const char *SYS_getcwd(void)
{
  const char *path;
  path = getcwd(NULL, 1024); 
  return path;
}

int SYS_copy(const char *filename_dst, const char *filename_src)
{
  static char buffer[4096];
  int status = 0;
  FILE *fin, *fout;
  fin = fopen(filename_src, "rb");
  if (fin != NULL) {
    fout = fopen(filename_dst, "wb");
    if (fout != NULL) {
      size_t size_r, size_w;
      while(size_r = fread(buffer, 1, sizeof(buffer), fin)) {
	if ((size_w = fwrite(buffer, 1, size_r, fout)) != size_r) {
	  status = -1;
          fprintf (stderr, "%s: error while writing %s: %s\n", SYS_programname, filename_dst, strerror(errno)); 
	  break;
	}
      }
      if (status != -1 && !feof(fin)) {
	status = -1;
        fprintf (stderr, "%s: error while reading %s: %s\n", SYS_programname, filename_src, strerror(errno)); 
      }
      fclose(fout);
    } else {
      status = -1;
      fprintf (stderr, "%s: can't write %s: %s\n", SYS_programname, filename_dst, strerror(errno)); 
    }
    fclose(fin);
  } else {
    status = -1;
    fprintf (stderr, "%s: can't read %s: %s\n", SYS_programname, filename_src, strerror(errno)); 
  }
  return status;
}

/*  void SYS_indent(const char *first_prefix, const char *prefix, int indent_size, const char *string, FILE *file) */
/*  { */
/*    int len = strlen(string); */
/*    char *buffer = SGC__ALLOCA(char, len+1); */
/*    const char *base_ptr; */
/*    char *ptr; */
/*    int nline = 0; */
/*    base_ptr = string; */
/*    for(;;) { */
/*      int count = 0, nword = 0; */
/*      if (nline == 0) */
/*        count += fprintf(file, first_prefix); */
/*      else */
/*        count += fprintf(file, prefix); */
    
/*      while ((ptr = strchr(base_ptr, ' ')) != NULL) { */
/*        if (ptr > base_ptr) { */
/*          int word_size = (ptr - base_ptr) + 1; */
/*          strncpy(buffer, base_ptr, word_size); */
/*          buffer[word_size] = '\0'; */
/*          if ((word_size + count <= indent_size) || nword == 0) { */
/*            count += fprintf(file, buffer); */
/*          } else { */
/*  	  break; */
/*  	} */
/*          nword++; */
/*        } */
/*        base_ptr = ptr + 1; */
/*      } */
/*      if (ptr == NULL) { */
/*        int word_size = string + len - base_ptr; */
/*        if ((word_size + count <= indent_size) || nword == 0) { */
/*            fprintf(file, base_ptr); */
/*            fprintf(file, "\n"); */
/*            break; */
/*        } */
/*      } */
/*      fprintf(file, "\n"); */
/*      nline++; */
/*    } */
/*  } */

int SYS_setenv(const char *var_name, const char *value)
{
  char *buffer = (char *)SYS_malloc(strlen(var_name)+strlen(value)+2);
  char *ptr;
  ptr = SYS_strappend(buffer, var_name);
  ptr = SYS_strappend(ptr, "=");
  ptr = SYS_strappend(ptr, value);
  return putenv(buffer);
}

