#ifndef __SYS_H__
#define __SYS_H__

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define SYS_FLG_VERBOSE	1
#define SYS_FLG_DRYRUN	2
#define SYS_FLG_SEARCH	4

#define ABORT() abort()
#define ASSERT(x) if(!(x)) abort()


extern const char *SYS_programname;

  /*************************************************************
   * Memory managment functions.
   * This set of functions are simple wrappers to malloc/free 
   * heap based allocation.
   *************************************************************/
#ifdef SYS_new
#undef SYS_new
#endif
#define SYS_new(type) \
(type *)SYS_malloc(sizeof(type)) \
/*SYS_new*/
#ifdef SYS_delete
#undef SYS_delete
#endif
#define SYS_delete(this) \
SYS_free(this) \
/*SYS_delete*/
#ifdef SYS_reset
#undef SYS_reset
#endif
#define SYS_reset(this, type) \
(type *)memset(this, 0, sizeof(type)) \
/*SYS_reset*/
extern void *SYS_malloc(int size);
extern void *SYS_free(void *ptr);
extern void *SYS_realloc(void *ptr, int size);
extern void *SYS_calloc(int n, int size);

  /*************************************************************
   * String management functions.
   * This set of functions are helper function for string 
   * manipulation.
   *************************************************************/
extern void *SYS_revert(void *ptr, int size);
extern void *SYS_mempattern(void *dst, int dst_size, const void *pattern, int pattern_size);
extern char *SYS_strdup(const char *string);
extern char *SYS_strdupn(const char *string, int n);
extern char *SYS_strappend(char *dest, const char *src);

  /*************************************************************
   * File management functions.
   * This set of functions are helper function for host independent
   * file/path management.
   *************************************************************/

  /*
   * SYS_getDirSeparator()
   * Get commonly used dir separator.
   */
extern char SYS_getDirSeparator(void);

  /*
   * SYS_isDirSeparator()
   * Returns true if the given char is a directory separator.
   */
extern int SYS_isDirSeparator(char c);

   /*
    * SYS_isAbsolute()
    * Returns true if the given path is an absolute path.
    */
extern int SYS_isAbsolute(const char *filename);

  /* 
   * SYS_makePath()
   * Returns a newly allocated and canonicalized pathname 
   * from a dirname and a basename. basename may itself be a relative path.
   * For instance under unix: SYS_makePath("bar", "foo") -> "bar/foo" .
   */
extern char *SYS_makePath(const char *dirname, const char *basename);

  /* 
   * SYS_makefileName()
   * Returns a newly allocated and canonicalized pathname from
   * a path and a suffix. 
   * For instance under unix: SYS_makeFileName("foo", ".exe") -> "foo.exe".
   */
extern char *SYS_makeFileName(const char *path, const char *suffix);

  
  /*
   * SYS_baseptr()
   * Returns the base part of path, i.e. a pointer into path to the basename.
   * It does not modify the content of path.
   */
extern char *SYS_baseptr(const char *path);

  /* 
   * SYS_ibasename()
   * The same as SYS_baseptr. ibasename is for "inline basename".
   * It is allowed to modify the content of path (though it actually doesn't).
   */
extern char *SYS_ibasename(char *path);

  /* 
   * SYS_abasename()
   * Returns a newly allocated basename for path. abasename is for 
   * "allocated basename". It does the same as SYS_ibasename() but in
   * a newly allocated buffer. It does not modify the content of path.
   */
extern char *SYS_abasename(const char *path);

  /* 
   * SYS_idirname()
   * Returns the directory part for path, i.e. a pointer into path that
   * points to the directory part. idirname is for
   * "inlined dirname". It may modify the content of path for removing the
   * basename part or the trailing dir separators.
   */
extern char *SYS_idirname(char *path);

  /* 
   * SYS_adirname()
   * Returns a newly allocated directory part for path. adirname is for
   * "allocated dirname". It does the same as SYS_idirname but in a newly
   * allocated buffer. It does not modify the content of path.
   */
extern char *SYS_adirname(const char *path);

  /* 
   * SYS_is_file()
   * Returns true if path is an existing readable plain file. 
   * It stats the path. 
   */
extern int SYS_is_file(const char *path);

  /* 
   * SYS_is_dir()
   * Returns true if path is an existing readable directory. 
   * It stats the path. 
   */
extern int SYS_is_dir(const char *path);

  /* 
   * SYS_is_wdir()
   * Returns true if path is an existing visible/readable/writable directory. 
   * It stats the path. 
   */
extern int SYS_is_wdir(const char *path);

  /* 
   * SYS_full_path()
   * Returns the newly allocated and canonicalized absolute path 
   * for the current running program given by pname (should be argv[0]).
   * On unix systems it relies on the content of the PATH environment
   * variable. On win32 systems it relies on the current module handle.
   * The symbolic links if any in the absolute path are not resolved.
   */
extern char *SYS_full_path(const char *pname);
  
  /*
   * SYS_real_path()
   * Returns a newly allocated and resolved path where symbolic links
   * are replaced by real directories. 
   * On unix it relies on realpath().
   * On win32 it simply returns a copy of path.
   */
extern char *SYS_real_path(const char *pname);

  /*
   * SYS_matchsuffix()
   * Returns true if the path suffix matches the given suffix.
   * For instance: SYS_matchsuffix("foo.exe", ".exe") -> true.
   */
extern int SYS_matchsuffix(const char *path, const char *suffix);

  /*
   * SYS_baseprefix()
   * Returns the inlined basename for the path where the suffix as
   * been dropped. It modifies the content of path.
   * For instance: SYS_matchsuffix("bar/foo.exe", ".exe") -> "foo".
   */
extern char *SYS_baseprefix(char *path, const char *suffix);
  
  /* 
   * SYS_tmpdir()
   * Returns a temporary directory available for read/write.
   * It checks in this order:
   * 'TMPDIR' env. value, 'TMP' env. value, 'TEMP' env. value, 
   * P_tmpdir macro if defined, /var/tmp, /usr/tmp, /tmp.
   * It falls back to the current directory "." if none of the above
   * is suitable.
   */
extern char *SYS_tmpdir(void);

  /* 
   * SYS_tmpname()
   * Returns a temporary filename.
   * It first determines a temporary directory as with SYS_tmpdir()
   * and then it creates an unique file name.
   * the filename can be used for creating an unique file.
   */
extern char *SYS_tmpname(void);
  
  /*
   * SYS_unlink()
   * Unlink a file.
   * Returns 0 if ok, -1 otherwise.
   */
extern int SYS_unlink(const char *filename);
  
  /*
   * SYS_copy()
   * Copy the filename_src file to filename_dst.
   * Returns 0 if ok, -1 otherwise.
   */
extern int SYS_copy(const char *filename_dst, const char *filename_src);

  /*
   * SYS_getcwd()
   * Returns a newly allocated path for the
   * current directory.
   */
extern char *SYS_getcwd(void);

  /*************************************************************
   * Environment management functions.
   * This set of functions are helper setting reading the
   * environment.
   *************************************************************/

  /*
   * SYS_setenv()
   * Sets the var_name value to value.
   * The value can be read with SYS_getenv().
   */
extern int SYS_setenv(const char *var_name, const char *value);

  /*
   * SYS_getenv()
   * Same as getenv().
   * Gets the value of var_name or NULL if not defined.
   */
char *SYS_getenv(const char *var_name);


  /*************************************************************
   * Process  management functions.
   * This set of functions are helper functions for 
   * process launching.
   *************************************************************/

  /*
   * SYS_execute()
   * Execute the given program and associated argv arguments.
   * See implementation for flags.
   * Returns the exit code of the program.
   */
extern int SYS_execute(const char *program, char * const *argv, int flags);

#ifdef  __cplusplus
}
#endif


#endif /*__SYS_H__*/
