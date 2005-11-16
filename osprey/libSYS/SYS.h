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
extern void *SYS_revert(void *ptr, int size);
extern void *SYS_mempattern(void *dst, int dst_size, const void *pattern, int pattern_size);
extern char *SYS_strdup(const char *string);
extern const char *SYS_strdupn(const char *string, int n);
extern char *SYS_strappend(char *dest, const char *src);
extern char SYS_getDirSeparator(void);
extern int SYS_isDirSeparator(char c);
extern int SYS_isAbsolute(const char *filename);
extern char *SYS_makePath(const char *dirname, const char *basename);
extern const char *SYS_makeFileName(const char *path, const char *suffix);
extern const char *SYS_baseptr(const char *path);
extern char *SYS_abasename(const char *path);
extern const char *SYS_ibasename(char *path);
extern int SYS_matchsuffix(const char *path, const char *suffix);
extern const char *SYS_baseprefix(const char *path, const char *suffix);
extern char *SYS_adirname(const char *path);
extern char *SYS_idirname(char *path);
extern int SYS_is_dir(const char *path);
extern int SYS_is_file(const char *path);
extern char *SYS_tmpdir(void);
extern char *SYS_tmpname(void);
extern int SYS_unlink(const char *filename);
extern int SYS_execute(const char *program, char * const *argv, int flags);
extern char *SYS_full_path(const char *pname);
extern char *SYS_real_path(const char *pname);
extern const char *SYS_getcwd(void);
extern int SYS_copy(const char *filename_dst, const char *filename_src);
extern void SYS_indent(const char *first_prefix, const char *prefix, int indent_size, const char *string, FILE *file);
extern int SYS_setenv(const char *var_name, const char *value);
#ifdef  __cplusplus
}
#endif


#endif /*__SYS_H__*/
