
/*
 * wrap_printf.c
 *
 * This file redefines the standard library functions 
 * [v|vf|s|sn|vs|sn]printf for native MINGW32 build.
 * Its purpose is to preprocess format strings in order to
 * convert GNU like %lld printf format specifiers into 
 * native (msvcrt like) %I64d specifiers.
 * It uses the GNU ld -wrap functionality to replace
 * at link time calls to printf into calls to __wrap_printf.
 *
 * This file must be linked with any DLL or EXE object
 * and the linker command line must have the following (quite long) option:
 * -Wl,-wrap,printf,-wrap,fprintf,-wrap,sprintf,-wrap,snprintf,-wrap,vprintf,-wrap,vfprintf,-wrap,vsprintf,-wrap,vsnprintf -wrap,_vsnprintf,-wrap,_snprintf 
 *
 */

#ifdef __MINGW32__
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>	/* For alloca(). */
#include <inttypes.h>	/* For PRIxx format specifiers. */

/*
 * Declare real versions of functions.
 */
extern int __real_vprintf(const char *format, va_list ap);
extern int __real_vfprintf(FILE *stream, const char *format, va_list ap);
extern int __real_vsprintf(char *str, const char *format, va_list ap);
/* 
   Special case for vsnprintf under MINGW32. It is defined as _[v]snprintf by
   default, and also as [v]snprintf if  __NO_ISOCEXT is not defined.
   For the real symbol reference we always use _[v]snprintf.
   In addition this means that we must redefined _snprintf and _vsnprintf
   in addition to snprintf and vsnprintf if __NO_ISOCEXT is not defined.
*/
extern int __real__vsnprintf(char *str, size_t size, const char *format, va_list ap);

/*
 * fixup_format()
 *
 * Build the fixed format string from the GNU like format string 'fmt'
 * into the buffer 'buf'.
 * If 'len_p' is non-NULL the new format string len is returned in '*len_p'.
 * If 'buf' is NULL, the format string is not converted but '*len_p'
 * is updated. Thus calling fixup_format(fmt, NULL, &len) will return
 * the string len required into 'len' that can be used for allocating a
 * sufficiently large buffer.
 * If buf is non-NULL the format string 'fmt' is converted into 'buf'. No
 * length overflow is done, thus ensure that the buffer is long enough by using
 * the above mentionned method.
 * This function returns the number of replacement that occured.
 */
static int
fixup_format(const char *fmt, char *buf, int *len_p)
{
  int n = 0;
  int len;
  len = 0;
  /* Copy until end of fmt or end of buffer. */
  while(fmt[0] != '\0') {
    if (fmt[0] == '%' && fmt[1] == '%') /* Skip printable '%'. */ {
      if (buf != NULL) {*buf++ = fmt[0]; *buf++ = fmt[1];}
      fmt += 2;
      len += 2;
    } else if (fmt[0] == '\%' && fmt[1] == 'l' && fmt[2] == 'l') {
      const char *repl = NULL;
      switch (fmt[3]) {
      case 'd':
	repl = PRId64;
	break;
      case 'i':
	repl = PRIi64;
	break;
      case 'o':
	repl = PRIo64;
	break;
      case 'u':
	repl = PRIu64;
	break;
      case 'x':
	repl = PRIx64;
	break;
      case 'X':
	repl = PRIX64;
	break;
      default:
	break;
      }
      if (repl != NULL) {
	/* Replacement found, copy the '%', skip the 'llx' and copy replacement
	   format. */
	if (buf != NULL) *buf++ = fmt[0];
	len++;
	fmt += 4;
	while(*repl) { 
	  if (buf != NULL) *buf++ = *repl;
	  repl++;
	  len++;
	}
	n++;
      } else {
	/* No replacement, continue copy. */
	if (buf != NULL) *buf++ = *fmt;
	fmt++;
	len++;
      }
    } else {
      if (buf != NULL) *buf++ = *fmt;
      fmt++;
      len++;
    }
  }
  if (buf != NULL) *buf = '\0';
  if (len_p != NULL) *len_p = len;
  return n;
}

/*
 * Following is the implementation of replacement printf-like functions.
 */

int
__wrap_printf(const char *fmt, ...)
{
  int new_len, n, r;
  va_list list;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  va_start(list, fmt);
  r = __real_vprintf(fmt, list);
  va_end(list);
  return r;
}

int
__wrap_fprintf(FILE *file, const char *fmt, ...)
{
  int new_len, n, r;
  va_list list;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  va_start(list, fmt);
  r = __real_vfprintf(file, fmt, list);
  va_end(list);
  return r;
}

int
__wrap_sprintf(char *str, const char *fmt, ...)
{
  int new_len, n, r;
  va_list list;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  va_start(list, fmt);
  r = __real_vsprintf(str, fmt, list);
  va_end(list);
  return r;
}

int
__wrap__snprintf(char *str, size_t size, const char *fmt, ...)
{
  int new_len, n, r;
  va_list list;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  va_start(list, fmt);
  r = __real__vsnprintf(str, size, fmt, list);
  va_end(list);
  return r;
}

int
__wrap_vprintf(const char *fmt, va_list list)
{
  int new_len, n;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  return __real_vprintf(fmt, list);
}

int
__wrap_vfprintf(FILE *file, const char *fmt, va_list list)
{
  int new_len, n;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  return __real_vfprintf(file, fmt, list);
}

int
__wrap_vsprintf(char *str, const char *fmt, va_list list)
{
  int new_len, n;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  return __real_vsprintf(str, fmt, list);
}

int
__wrap__vsnprintf(char *str, size_t size, const char *fmt, va_list list)
{
  int new_len, n;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  return __real__vsnprintf(str, size, fmt, list);
}


#ifndef __NO_ISOCEXT
int
__wrap_snprintf(char *str, size_t size, const char *fmt, ...)
{
  int new_len, n, r;
  va_list list;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  va_start(list, fmt);
  r = __real__vsnprintf(str, size, fmt, list);
  va_end(list);
  return r;
}


int
__wrap_vsnprintf(char *str, size_t size, const char *fmt, va_list list)
{
  int new_len, n;
  
  n = fixup_format(fmt, NULL, &new_len);
  if (n > 0) {
    char *buf;
    buf = (char *)alloca(new_len+1);
    fixup_format(fmt, buf, &new_len);
    fmt = buf;
  }
  return __real__vsnprintf(str, size, fmt, list);
}


#endif

#endif /* __MINGW32__ */
