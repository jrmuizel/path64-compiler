
#include <sys/types.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "libiberty/libiberty.h"
#include "SYS.h"

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


#ifdef __MINGW32__
#define DIR_SEPARATOR '\\'
#define DIR_SEPARATOR2 '/'
#define HAS_DRIVE_LETTER
#define DRIVE_SEPARATOR ':'
#else
#define DIR_SEPARATOR '/'
#endif

#ifdef __MINGW32__
#ifdef ERROR
#undef ERROR
#endif
#include <windows.h>
#endif

/* Set max path len to some common value. */
#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 1024
#endif

const char *SYS_programname = "<undefined>";

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
  char *p = (char *)ptr;
  char swap;

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
  char *bptr = (char *)dst;

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

char *SYS_strdupn(const char *string, int n)
{
  char *ptr = SYS_malloc(sizeof(char)*(n+1));
  strncpy(ptr, string, n);
  ptr[n] = '\0';
  return ptr;
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

int SYS_isDirSeparator(char c)
{
  return c == DIR_SEPARATOR
#ifdef DIR_SEPARATOR2
	|| c == DIR_SEPARATOR2
#endif
	;
}

int SYS_isAbsolute(const char *filename)
{
#ifdef HAS_DRIVE_LETTER
  return isalpha(filename[0]) && filename[1] == DRIVE_SEPARATOR &&
	SYS_isDirSeparator(filename[2]);
#else 
  return SYS_isDirSeparator(filename[0]);
#endif
}

char *SYS_inormalize(char *path)
{
  char *p = path;
  char *ret = p;
  /* Normalize all dir separators to the canonical one. */
  while(*path != '\0') {
    if (SYS_isDirSeparator(*path)) {
      *p++ = SYS_getDirSeparator();
      path++;
      while(SYS_isDirSeparator(*path)) path++;
    }
    else {
      *p++ = *path++;
    }
  }
  if (p > ret && SYS_isDirSeparator(*(p-1))) p--;
  *p = '\0';
  return ret;
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
      if (dirlen > 0 && !SYS_isDirSeparator(dirname[dirlen-1])) {
	*ptr++ = SYS_getDirSeparator();
      }
      SYS_strappend(ptr, basename);
      path = SYS_inormalize(path);
      return path;
    }
}

char *SYS_makeFileName(const char *path, const char *suffix)
{
    ASSERT(path != NULL);
    ASSERT(suffix != NULL);
    {
      int pathlen = strlen(path);
      int suffixlen = strlen(suffix);
      char *p = SYS_malloc(sizeof(char)*(pathlen+suffixlen+1));
      strcpy(p, path); 
      strcpy(p+pathlen, suffix);
      p = SYS_inormalize(p);
      return p;
    }
}

char *SYS_baseptr(const char *path)
{
    char *p;
    p = strrchr(path, DIR_SEPARATOR);
#ifdef DIR_SEPARATOR2
    {
      char *p2;
      p2 = strrchr(path, DIR_SEPARATOR2);
      if (p2 > p) p = p2;
    }
#endif
    return p == NULL ? (char *)path : p+1;
}

char *SYS_abasename(const char *path)
{
    return SYS_strdup(SYS_baseptr(path));
}

char *SYS_ibasename(char *path)
{
    return SYS_baseptr(path);
}


int SYS_matchsuffix(const char *path, const char *suffix)
{
  int match = 1;
  const char *p1 = suffix + strlen(suffix);
  const char *p2 = path + strlen(path);
  while(p1 > suffix) {
    p1--; p2--;
    if (p2 < path || (*p1 != *p2)) {
      match = 0;
      break;
    }
  }
  return match;
}

char *SYS_baseprefix(char *path, const char *suffix)
{
  int match = 1;
  char *p = SYS_baseptr(path);
  const char *p1 = suffix + strlen(suffix);
  char *p2 = p + strlen(p);
  while(p1 > suffix) {
    p1--; p2--;
    if (p2 < p || (*p1 != *p2)) {
      match = 0;
      break;
    }
  }
  if (match) {
    *p2 = '\0';
  }
  return p;
}

char *SYS_adirname(const char *path)
{
    const char *p;
    p = strrchr(path, DIR_SEPARATOR);
#ifdef DIR_SEPARATOR2
    {
      const char *p2;
      p2 = strrchr(path, DIR_SEPARATOR2);
      if (p2 > p) p = p2;
    }
#endif
    if (p == NULL) {
	return SYS_strdup(".");
    } else {
	char *dirname = SYS_malloc(sizeof(char)*(p-path+1));
	if (p-path > 0) {
	  memcpy(dirname, path, p-path);
	} else {
	  dirname[0] = DIR_SEPARATOR;
	  p++;
	}
	dirname[p-path] = '\0';
	return dirname;
    }
}

char *SYS_idirname(char *path)
{
    const char *p;
    p = strrchr(path, DIR_SEPARATOR);
#ifdef DIR_SEPARATOR2
    {
      const char *p2;
      p2 = strrchr(path, DIR_SEPARATOR2);
      if (p2 > p) p = p2;
    }
#endif
    if (p == NULL) {
      return strcpy(path, ".");
    } else {
      if (p-path == 0) {
	path[0] = DIR_SEPARATOR;
	p++;
      }
      path[p-path] = '\0';
      return path;
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

int SYS_is_wdir(const char *path)
{
  return SYS_is_dir(path) && access(path, R_OK | W_OK | X_OK) == 0;
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
  static const char d_tmp[] = 
    { DIR_SEPARATOR, 't', 'm', 'p', '\0' };
  static const char d_usrtmp[] =
    { DIR_SEPARATOR, 'u', 's', 'r', DIR_SEPARATOR, 't', 'm', 'p', '\0' };
  static const char d_vartmp[] =
    { DIR_SEPARATOR, 'v', 'a', 'r', DIR_SEPARATOR, 't', 'm', 'p', '\0' };
  static char *cached_tmpdir = NULL;
  const char *tmpdir = NULL;
  
  if (cached_tmpdir != NULL) return SYS_strdup(cached_tmpdir);
  
  if ((tmpdir = getenv("TMPDIR")) != NULL && SYS_is_wdir(tmpdir)) goto found;
  if ((tmpdir = getenv("TMP")) != NULL && SYS_is_wdir(tmpdir)) goto found;
  if ((tmpdir = getenv("TEMP")) != NULL && SYS_is_wdir(tmpdir)) goto found;
#ifdef P_tmpdir
  if ((tmpdir = P_tmpdir) != NULL && SYS_is_wdir(tmpdir)) goto found;
#endif
  if ((tmpdir = d_vartmp) != NULL && SYS_is_wdir(tmpdir)) goto found;
  if ((tmpdir = d_usrtmp) != NULL && SYS_is_wdir(tmpdir)) goto found;
  if ((tmpdir = d_tmp) != NULL && SYS_is_wdir(tmpdir))  goto found;
  tmpdir = ".";
 found:
  cached_tmpdir = SYS_strdup(tmpdir);
  return SYS_strdup(cached_tmpdir);
}

char *SYS_tmpname(void)
{
  
  return make_temp_file("");
}

int SYS_unlink(const char *filename)
{
  int status;
  status = unlink(filename);
  return status;
}

#ifdef __MINGW32__
char *SYS_full_path_w32(const char *pname)
{
  static char win32_path[MAX_PATH_LENGTH];
  char *pdir = NULL;
  char *ignored ;  
  if (SearchPath(NULL, pname, ".exe", MAX_PATH_LENGTH, win32_path, &ignored)) {
    pdir = SYS_strdup(win32_path);
  }
  return pdir;
}
#endif

#ifndef __MINGW32__
static char *
SYS_full_path_unix(const char *pname)
{
  int pname_len = strlen(pname);
  char *file_buffer = SYS_malloc(MAX_PATH_LENGTH+1);
  
  if (SYS_isAbsolute(pname)) {
    if (pname_len + 1 > MAX_PATH_LENGTH+1) goto failed;
    strcpy(file_buffer, pname); 
    if (access(file_buffer, F_OK) == -1) goto failed;
  } else if (strchr(pname, DIR_SEPARATOR) != NULL) {
    const char *pwd;
    int dir_len;
    pwd = getcwd(file_buffer, MAX_PATH_LENGTH+1);
    if (pwd == NULL || file_buffer[0] == '\0' ) goto failed;
    dir_len = strlen(file_buffer);
    if (dir_len + 1 + pname_len + 1 > MAX_PATH_LENGTH+1) goto failed;
    if (file_buffer[dir_len-1] != DIR_SEPARATOR) {
      file_buffer[dir_len] = DIR_SEPARATOR;
      strcpy(&file_buffer[dir_len+1], pname);
    } else {
      strcpy(&file_buffer[dir_len], pname);
    }
    if (access(file_buffer, F_OK) == -1) goto failed;
  } else {
    
    const char *path, *next;
    int dir_len, found = 0;
    path = getenv("PATH");
    while (path != NULL) {
      next = strchr(path, ':');
      if (next == NULL) {
	dir_len = strlen(path);
      } else {
	dir_len = next - path;
	next++;
      }
      if (dir_len == 0) {
	path = next;
	continue;
      }
      if (dir_len + 1 + pname_len + 1 > MAX_PATH_LENGTH+1) goto failed;
      memcpy(file_buffer, path, dir_len); /* Not yet '\0' terminated. */
      if (file_buffer[dir_len-1] != DIR_SEPARATOR) {
	file_buffer[dir_len] = DIR_SEPARATOR;
	strcpy(&file_buffer[dir_len+1], pname);
      } else {
	strcpy(&file_buffer[dir_len], pname);
      }
      if (access(file_buffer, F_OK) == 0) {
	found = 1;
	break;
      }
      path = next;
    }
    if (!found) goto failed;
  }
  return file_buffer;
 failed:
  SYS_free(file_buffer);
  return NULL;
}
#endif

char *SYS_full_path(const char *pname)
{
  char *path = NULL;
#ifdef __MINGW32__
  path = SYS_full_path_w32(pname);
#else
  path = SYS_full_path_unix(pname);
#endif
  return path;
}

char *SYS_real_path(const char *pname)
{
  char *path = NULL;
#ifdef __MINGW32__
  path = SYS_strdup(pname);
#else
    /* Get real path. */
  path = lrealpath (pname);
#endif
  return path;
}

char *SYS_getcwd(void)
{
  char *path;
  path = getcwd(NULL, MAX_PATH_LENGTH+1); 
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


int SYS_setenv(const char *var_name, const char *value)
{
  char *buffer = (char *)SYS_malloc(strlen(var_name)+strlen(value)+2);
  char *ptr;
  ptr = SYS_strappend(buffer, var_name);
  ptr = SYS_strappend(ptr, "=");
  ptr = SYS_strappend(ptr, value);
  return putenv(buffer);
}

char *SYS_getenv(const char *var_name)
{
  return getenv(var_name);
}

