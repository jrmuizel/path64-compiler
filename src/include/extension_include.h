#ifndef EXTENSION_INCLUDE_H
#define EXTENSION_INCLUDE_H

#define BITS_PER_UNIT 8
#define HOST_WIDE_INT int
/* mtypes.h: This file automatically generated. */





/**
***		Intrinsics, Intrinsic Types, and Intrinsic Flags
***		------------------------------------------------
***
*** Description:
***
***	This interface describes all the intrinsic names, operators,
***	types associated with intrinsics, and properties associated with
***	intrinsics.
***
*** Reserved Prefixes:
***
***	INTRN		for INTRINSIC members only.
***
*** Exported types:
***
***	INTRINSIC
***
***	    An enumerated type.  The members are a partial set of all language
***	    defined intrinsics.  Those language intrinsics not included in this
***	    enumerated type are already present in WHIRL as OPC nodes.  There
***	    are usually two separate flavors of each fortran intrinsic - one
***	    named INTRN_XXX and one named INTRN_XXXe.  The former name
***	    represents the version called directly by generated code and usually
***	    has call by value semantics.  These intrinsics might eventually be
***	    turned into calls, be inlined or have direct code generated for
***	    them.  The INTRN_XXXe version is always an external routine with
***	    call by reference semantics.  It is needed to support passing an
***	    intrinsic function itself to another subprogram.
***
***	    All INTRINSICs are prefixed with INTRN.
***
*** Exported data:
***
***	    none
***
**/


#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define CONCAT2(a,b)	 a##b
#else
#define CONCAT2(a,b)	 a/**/b
#endif
#ifndef PARAMS
#define PARAMS(x) x
#endif
/* since we do not use the regular gcc configure way, we need to
   distinguish here between the different host platforms... */

#ifdef linux
/* auto-host.h.  Generated automatically by configure.  */
/* config.in.  Generated automatically from configure.in by autoheader 2.13.  */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
#define GETGROUPS_T gid_t

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have the ANSI # stringizing operator in cpp. */
#define HAVE_STRINGIZE 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define if your C compiler doesn't accept -c and -o together.  */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* Define to 1 if NLS is requested.  */
#define ENABLE_NLS 1

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
/* #undef HAVE_CATGETS */

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
#define HAVE_GETTEXT 1

/* Define if your locale.h file contains LC_MESSAGES.  */
#define HAVE_LC_MESSAGES 1

/* Define as 1 if you have the stpcpy function.  */
#define HAVE_STPCPY 1

/* Define if your assembler supports specifying the maximum number
   of bytes to skip when using the GAS .p2align command. */
/* #undef HAVE_GAS_MAX_SKIP_P2ALIGN */

/* Define if your assembler supports .balign and .p2align.  */
/* #undef HAVE_GAS_BALIGN_AND_P2ALIGN */

/* Define if your assembler uses the old HImode fild and fist notation.  */
/* #undef HAVE_GAS_FILDS_FISTS */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef ssize_t */

/* Define if cpp should also search $prefix/include. */
/* #undef PREFIX_INCLUDE_DIR */

/* Define if you have the __argz_count function.  */
#define HAVE___ARGZ_COUNT 1

/* Define if you have the __argz_next function.  */
#define HAVE___ARGZ_NEXT 1

/* Define if you have the __argz_stringify function.  */
#define HAVE___ARGZ_STRINGIFY 1

/* Define if you have the atoll function.  */
#define HAVE_ATOLL 1

/* Define if you have the atoq function.  */
/* #undef HAVE_ATOQ */

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the bsearch function.  */
#define HAVE_BSEARCH 1

/* Define if you have the dcgettext function.  */
#define HAVE_DCGETTEXT 1

/* Define if you have the fputc_unlocked function.  */
#undef HAVE_FPUTC_UNLOCKED

/* Define if you have the fputs_unlocked function.  */
#undef HAVE_FPUTS_UNLOCKED

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the getrusage function.  */
#define HAVE_GETRUSAGE 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the isascii function.  */
#define HAVE_ISASCII 1

/* Define if you have the kill function.  */
#define HAVE_KILL 1

/* Define if you have the lstat function.  */
#define HAVE_LSTAT 1

/* Define if you have the munmap function.  */
#define HAVE_MUNMAP 1

/* Define if you have the nl_langinfo function.  */
#define HAVE_NL_LANGINFO 1

/* Define if you have the popen function.  */
#define HAVE_POPEN 1

/* Define if you have the putc_unlocked function.  */
#define HAVE_PUTC_UNLOCKED 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the setenv function.  */
#define HAVE_SETENV 1

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setrlimit function.  */
#define HAVE_SETRLIMIT 1

/* Define if you have the stpcpy function.  */
#define HAVE_STPCPY 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the sysconf function.  */
#define HAVE_SYSCONF 1

/* Define if you have the <argz.h> header file.  */
#define HAVE_ARGZ_H 1

/* Define if you have the <direct.h> header file.  */
/* #undef HAVE_DIRECT_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <langinfo.h> header file.  */
#define HAVE_LANGINFO_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <nl_types.h> header file.  */
#define HAVE_NL_TYPES_H 1

/* Define if you have the <stab.h> header file.  */
#define HAVE_STAB_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/times.h> header file.  */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the i library (-li).  */
/* #undef HAVE_LIBI */

/* Define to enable the use of a default linker. */
/* #undef DEFAULT_LINKER */

/* Define to enable the use of a default assembler. */
/* #undef DEFAULT_ASSEMBLER */

/* Define if you want more run-time sanity checks.  This one gets a grab
   bag of miscellaneous but relatively cheap checks. */
/* #undef ENABLE_CHECKING */

/* Define if you want all operations on trees (the basic data
   structure of the front ends) to be checked for dynamic type safety
   at runtime.  This is moderately expensive. */
/* #undef ENABLE_TREE_CHECKING */

/* Define if you want all operations on RTL (the basic data structure
   of the optimizer and back end) to be checked for dynamic type safety
   at runtime.  This is quite expensive. */
/* #undef ENABLE_RTL_CHECKING */

/* Define if you want the garbage collector to do object poisoning and
   other memory allocation checks.  This is quite expensive. */
/* #undef ENABLE_GC_CHECKING */

/* Define if you want the garbage collector to operate in maximally
   paranoid mode, validating the entire heap and collecting garbage at
   every opportunity.  This is extremely expensive. */
/* #undef ENABLE_GC_ALWAYS_COLLECT */

/* Define if you want the C and C++ compilers to support multibyte
   character sets for source code. */
/* #undef MULTIBYTE_CHARS */

/* Define if your compiler supports the `long double' type. */
#define HAVE_LONG_DOUBLE 1

/* Define if the `_Bool' type is built-in. */
/* #undef HAVE__BOOL */

/* Always define this when using the GNU C Library */
#define _GNU_SOURCE 1

/* Define if your compiler understands volatile. */
#define HAVE_VOLATILE 1

/* Define if you have a working <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define if you can safely include both <string.h> and <strings.h>. */
#define STRING_WITH_STRINGS 1

/* Define if you have a working <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if enumerated bitfields are treated as unsigned values. */
#define ENUM_BITFIELDS_ARE_UNSIGNED 1

/* Define if printf supports %p. */
#define HAVE_PRINTF_PTR 1

/* Define if mmap can get us zeroed pages from /dev/zero. */
#define HAVE_MMAP_DEV_ZERO 1

/* Define if mmap can get us zeroed pages using MAP_ANON(YMOUS). */
#define HAVE_MMAP_ANON 1

/* Define if read-only mmap of a plain file works. */
#define HAVE_MMAP_FILE 1

/* Define if you have the iconv() function. */
#define HAVE_ICONV 1

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST const

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_BCOPY 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_GETENV 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_ATOL 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_SBRK 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_ABORT 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_ATOF 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_GETCWD 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_GETWD 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_STRSIGNAL 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_PUTC_UNLOCKED 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_FPUTS_UNLOCKED 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_STRSTR 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_ENVIRON 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_MALLOC 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_REALLOC 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_CALLOC 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_FREE 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_BASENAME 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_GETOPT 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_GETRLIMIT 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_SETRLIMIT 1

/* Define to 1 if we found this declaration otherwise define to 0. */
#define HAVE_DECL_GETRUSAGE 1

/* Define if host mkdir takes a single argument. */
/* #undef MKDIR_TAKES_ONE_ARG */

/* Define to the name of the distribution. */
#define PACKAGE "gcc"

/* Define to the version of the distribution. */
#define VERSION "3.0.3"

/* Define to 1 if installation paths should be looked up in Windows32
   Registry. Ignored on non windows32 hosts. */
#define ENABLE_WIN32_REGISTRY 1

/* Define to be the last portion of registry key on windows hosts. */
#define WIN32_REGISTRY_KEY "3.0.3"

/* Define if your assembler supports .subsection and .subsection -1 starts
   emitting at the beginning of your section. */
/* #undef HAVE_GAS_SUBSECTION_ORDERING */

/* Define if your assembler supports .weak. */
/* #undef HAVE_GAS_WEAK */

/* Define if your assembler supports .hidden. */
/* #undef HAVE_GAS_HIDDEN */

/* Define if your assembler supports .uleb128. */
/* #undef HAVE_AS_LEB128 */

/* Define if your assembler mis-optimizes .eh_frame data. */
/* #undef USE_AS_TRADITIONAL_FORMAT */

/* Define if your assembler supports .register. */
/* #undef HAVE_AS_REGISTER_PSEUDO_OP */

/* Define if your assembler supports -relax option. */
/* #undef HAVE_AS_RELAX_OPTION */

/* Define if the assembler supports 64bit sparc. */
/* #undef AS_SPARC64_FLAG */

/* Define if your assembler supports offsetable %lo(). */
/* #undef HAVE_AS_OFFSETABLE_LO10 */

/* Define if your assembler supports dwarf2 .file/.loc directives. */
/* #undef HAVE_AS_DWARF2_DEBUG_LINE */

/* Define to 1 if you want to enable namespaces (-fhonor-std) by default. */
#define ENABLE_STD_NAMESPACE 1

/* Define 0/1 to force the choice for exception handling model. */
/* #undef CONFIG_SJLJ_EXCEPTIONS */


/* Bison unconditionally undefines `const' if neither `__STDC__' nor
   __cplusplus are defined.  That's a problem since we use `const' in
   the GCC headers, and the resulting bison code is therefore type
   unsafe.  Thus, we must match the bison behavior here.  */

#ifndef __STDC__
#ifndef __cplusplus
/* #undef const */
#define const
#endif
#endif

/**** gcc 3.3 upgrade need following defs *****/

/* Define if your compiler supports the `long double' type. */
#define HAVE_LONG_DOUBLE 1

/* Define if your compiler supports the `long long' type. */
#define HAVE_LONG_LONG 1

/* Define if your compiler supports the `__int64' type. */
/* #undef HAVE___INT64 */

/* Define if the `_Bool' type is built-in. */
/* #undef HAVE__BOOL */

/* The number of bytes in type short */
#define SIZEOF_SHORT 2

/* The number of bytes in type int */
#define SIZEOF_INT 4

/* The number of bytes in type long */
#define SIZEOF_LONG 4

/* The number of bytes in type long long */
#define SIZEOF_LONG_LONG 8

/* Define if you have a working <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to the name of a file containing a list of extra machine modes
   for this architecture. */
#define EXTRA_MODES_FILE "arm/arm-modes.def"

/* Define if the target architecture needs extra machine modes to represent
   the results of comparisons. */
#define EXTRA_CC_MODES 1

/* Define if <sys/times.h> defines struct tms. */
#define HAVE_STRUCT_TMS 1

/* Define if <time.h> defines clock_t. */
#define HAVE_CLOCK_T 1

/* host dependent, where should it be defined ? */
#define HOST_FLOAT_WORDS_BIG_ENDIAN 0


#endif
#ifdef sun
/* auto-host.h.  Generated automatically by configure.  */
/* config.in.  Generated automatically from configure.in by autoheader.  */
/* Define if you can safely include both <string.h> and <strings.h>.  */
#define STRING_WITH_STRINGS 1

/* Define if printf supports "%p".  */
#define HAVE_PRINTF_PTR 1

/* Define if you want expensive run-time checks. */
/* #undef ENABLE_CHECKING */

/* Define to 1 if NLS is requested.  */
/* #undef ENABLE_NLS */

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
/* #undef HAVE_CATGETS */

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
/* #undef HAVE_GETTEXT */

/* Define if your cpp understands the stringify operator.  */
#define HAVE_CPP_STRINGIFY 1

/* Define if your compiler understands volatile.  */
#define HAVE_VOLATILE 1

/* Define if your assembler supports specifying the maximum number
   of bytes to skip when using the GAS .p2align command. */
/* #undef HAVE_GAS_MAX_SKIP_P2ALIGN */

/* Define if your assembler supports .balign and .p2align.  */
/* #undef HAVE_GAS_BALIGN_AND_P2ALIGN */

/* Define if your assembler supports .subsection and .subsection -1 starts
   emitting at the beginning of your section */
/* #undef HAVE_GAS_SUBSECTION_ORDERING */

/* Define if your assembler uses the old HImode fild and fist notation.  */
/* #undef HAVE_GAS_FILDS_FISTS */

/* Define if you have a working <inttypes.h> header file.  */
#define HAVE_INTTYPES_H 1

/* Define if your locale.h file contains LC_MESSAGES.  */
#define HAVE_LC_MESSAGES 1

/* Define as 1 if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Whether malloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_MALLOC */

/* Whether realloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_REALLOC */

/* Whether calloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_CALLOC */

/* Whether free must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_FREE */

/* Whether bcopy must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BCOPY */

/* Whether bcmp must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BCMP */

/* Whether bzero must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BZERO */

/* Whether index must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_INDEX */

/* Whether rindex must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_RINDEX */

/* Whether getenv must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_GETENV */

/* Whether atol must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ATOL */

/* Whether atof must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ATOF */

/* Whether sbrk must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_SBRK */

/* Whether abort must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ABORT */

/* Whether strerror must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRERROR */

/* Whether strsignal must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRSIGNAL */

/* Whether strstr must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRSTR */

/* Whether getcwd must be declared even if <unistd.h> is included.  */
/* #undef NEED_DECLARATION_GETCWD */

/* Whether getwd must be declared even if <unistd.h> is included.  */
/* #undef NEED_DECLARATION_GETWD */

/* Whether getrlimit must be declared even if <sys/resource.h> is included.  */
/* #undef NEED_DECLARATION_GETRLIMIT */

/* Whether setrlimit must be declared even if <sys/resource.h> is included.  */
/* #undef NEED_DECLARATION_SETRLIMIT */

/* Whether putc_unlocked must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_PUTC_UNLOCKED 1

/* Whether fputs_unlocked must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_FPUTS_UNLOCKED 1

/* Define to enable the use of a default assembler. */
/* #undef DEFAULT_ASSEMBLER */

/* Define to enable the use of a default linker. */
/* #undef DEFAULT_LINKER */

/* Define if host mkdir takes a single argument. */
/* #undef MKDIR_TAKES_ONE_ARG */

/* Define to the name of the distribution.  */
#define PACKAGE "gcc"

/* Define to the version of the distribution.  */
#define VERSION "2.95.3"

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if `sys_siglist' is declared by <signal.h>.  */
/* #undef SYS_SIGLIST_DECLARED */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define vfork as fork if vfork does not work.  */
#define vfork fork

/* Define if you have the __argz_count function.  */
/* #undef HAVE___ARGZ_COUNT */

/* Define if you have the __argz_next function.  */
/* #undef HAVE___ARGZ_NEXT */

/* Define if you have the __argz_stringify function.  */
/* #undef HAVE___ARGZ_STRINGIFY */

/* Define if you have the atoll function.  */
#define HAVE_ATOLL 1

/* Define if you have the atoq function.  */
/* #undef HAVE_ATOQ */

/* Define if you have the bcmp function.  */
#define HAVE_BCMP 1

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the bsearch function.  */
#define HAVE_BSEARCH 1

/* Define if you have the bzero function.  */
#define HAVE_BZERO 1

/* Define if you have the dcgettext function.  */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the fputc_unlocked function.  */
/* #undef HAVE_FPUTC_UNLOCKED */

/* Define if you have the fputs_unlocked function.  */
/* #undef HAVE_FPUTS_UNLOCKED */

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the index function.  */
#define HAVE_INDEX 1

/* Define if you have the isascii function.  */
#define HAVE_ISASCII 1

/* Define if you have the kill function.  */
#define HAVE_KILL 1

/* Define if you have the lstat function.  */
#define HAVE_LSTAT 1

/* Define if you have the munmap function.  */
#define HAVE_MUNMAP 1

/* Define if you have the popen function.  */
#define HAVE_POPEN 1

/* Define if you have the putc_unlocked function.  */
#define HAVE_PUTC_UNLOCKED 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the rindex function.  */
#define HAVE_RINDEX 1

/* Define if you have the setenv function.  */
/* #undef HAVE_SETENV */

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setrlimit function.  */
#define HAVE_SETRLIMIT 1

/* Define if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the sysconf function.  */
#define HAVE_SYSCONF 1

/* Define if you have the <argz.h> header file.  */
/* #undef HAVE_ARGZ_H */

/* Define if you have the <direct.h> header file.  */
/* #undef HAVE_DIRECT_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <nl_types.h> header file.  */
#define HAVE_NL_TYPES_H 1

/* Define if you have the <stab.h> header file.  */
/* #undef HAVE_STAB_H */

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/times.h> header file.  */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the i library (-li).  */
/* #undef HAVE_LIBI */

/**** gcc 3.3 upgrade need following defs *****/

/* Define if your compiler supports the `long double' type. */
#define HAVE_LONG_DOUBLE 1

/* Define if your compiler supports the `long long' type. */
#define HAVE_LONG_LONG 1

/* Define if your compiler supports the `__int64' type. */
/* #undef HAVE___INT64 */

/* Define if the `_Bool' type is built-in. */
/* #undef HAVE__BOOL */

/* The number of bytes in type short */
#define SIZEOF_SHORT 2

/* The number of bytes in type int */
#define SIZEOF_INT 4

/* The number of bytes in type long */
#define SIZEOF_LONG 4

/* The number of bytes in type long long */
#define SIZEOF_LONG_LONG 8

/* Define if you have a working <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to the name of a file containing a list of extra machine modes
   for this architecture. */
#define EXTRA_MODES_FILE "arm/arm-modes.def"

/* Define if the target architecture needs extra machine modes to represent
   the results of comparisons. */
#define EXTRA_CC_MODES 1

/* Define if <sys/times.h> defines struct tms. */
#define HAVE_STRUCT_TMS 1

/* Define if <time.h> defines clock_t. */
#define HAVE_CLOCK_T 1

/* Define if mmap can get us zeroed pages from /dev/zero. */
#define HAVE_MMAP_DEV_ZERO 1

/* Define if read-only mmap of a plain file works. */
#define HAVE_MMAP_FILE 1

/* host dependent, where should it be defined ? */
#define HOST_FLOAT_WORDS_BIG_ENDIAN 1

#endif
#ifdef __CYGWIN__
/* auto-host.h.  Generated automatically by configure.  */
/* config.in.  Generated automatically from configure.in by autoheader.  */
/* Define if you can safely include both <string.h> and <strings.h>.  */
#define STRING_WITH_STRINGS 1

/* Define if printf supports "%p".  */
#define HAVE_PRINTF_PTR 1

/* Define if you want expensive run-time checks. */
/* #undef ENABLE_CHECKING */

/* Define to 1 if NLS is requested.  */
/* #undef ENABLE_NLS */

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
/* #undef HAVE_CATGETS */

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
/* #undef HAVE_GETTEXT */

/* Define if your cpp understands the stringify operator.  */
#define HAVE_CPP_STRINGIFY 1

/* Define if your compiler understands volatile.  */
#define HAVE_VOLATILE 1

/* Define if your assembler supports specifying the maximum number
   of bytes to skip when using the GAS .p2align command. */
#define HAVE_GAS_MAX_SKIP_P2ALIGN 1

/* Define if your assembler supports .balign and .p2align.  */
#define HAVE_GAS_BALIGN_AND_P2ALIGN 1

/* Define if your assembler supports .subsection and .subsection -1 starts
   emitting at the beginning of your section */
/* #undef HAVE_GAS_SUBSECTION_ORDERING */

/* Define if your assembler uses the old HImode fild and fist notation.  */
#define HAVE_GAS_FILDS_FISTS 1

/* Define if you have a working <inttypes.h> header file.  */
/* #undef HAVE_INTTYPES_H */

/* Define if your locale.h file contains LC_MESSAGES.  */
#ifndef CROSS_COMPILE
#define HAVE_LC_MESSAGES 1
#else
#undef HAVE_LC_MESSAGES 
#endif

/* Define as 1 if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Whether malloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_MALLOC */

/* Whether realloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_REALLOC */

/* Whether calloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_CALLOC */

/* Whether free must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_FREE */

/* Whether bcopy must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BCOPY */

/* Whether bcmp must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BCMP */

/* Whether bzero must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BZERO */

/* Whether index must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_INDEX */

/* Whether rindex must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_RINDEX */

/* Whether getenv must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_GETENV */

/* Whether atol must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ATOL */

/* Whether atof must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ATOF */

/* Whether sbrk must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_SBRK */

/* Whether abort must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ABORT */

/* Whether strerror must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRERROR */

/* Whether strsignal must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRSIGNAL */

/* Whether strstr must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRSTR */

/* Whether getcwd must be declared even if <unistd.h> is included.  */
/* #undef NEED_DECLARATION_GETCWD */

/* Whether getwd must be declared even if <unistd.h> is included.  */
/* #undef NEED_DECLARATION_GETWD */

/* Whether getrlimit must be declared even if <sys/resource.h> is included.  */
/* #undef NEED_DECLARATION_GETRLIMIT */

/* Whether setrlimit must be declared even if <sys/resource.h> is included.  */
/* #undef NEED_DECLARATION_SETRLIMIT */

/* Whether putc_unlocked must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_PUTC_UNLOCKED 1

/* Whether fputs_unlocked must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_FPUTS_UNLOCKED 1

/* Define to enable the use of a default assembler. */
/* #undef DEFAULT_ASSEMBLER */

/* Define to enable the use of a default linker. */
/* #undef DEFAULT_LINKER */

/* Define if host mkdir takes a single argument. */
/* #undef MKDIR_TAKES_ONE_ARG */

/* Define to the name of the distribution.  */
#define PACKAGE "gcc"

/* Define to the version of the distribution.  */
#define VERSION "2.95.2"

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have a working `mmap' system call.  */
/* #undef HAVE_MMAP */

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if `sys_siglist' is declared by <signal.h>.  */
/* #undef SYS_SIGLIST_DECLARED */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* Define if you have the __argz_count function.  */
/* #undef HAVE___ARGZ_COUNT */

/* Define if you have the __argz_next function.  */
/* #undef HAVE___ARGZ_NEXT */

/* Define if you have the __argz_stringify function.  */
/* #undef HAVE___ARGZ_STRINGIFY */

/* Define if you have the atoll function.  */
/* #undef HAVE_ATOLL */

/* Define if you have the atoq function.  */
/* #undef HAVE_ATOQ */

/* Define if you have the bcmp function.  */
#define HAVE_BCMP 1

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the bsearch function.  */
#define HAVE_BSEARCH 1

/* Define if you have the bzero function.  */
#define HAVE_BZERO 1

/* Define if you have the dcgettext function.  */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the fputc_unlocked function.  */
/* #undef HAVE_FPUTC_UNLOCKED */

/* Define if you have the fputs_unlocked function.  */
/* #undef HAVE_FPUTS_UNLOCKED */

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the index function.  */
#define HAVE_INDEX 1

/* Define if you have the isascii function.  */
#define HAVE_ISASCII 1

/* Define if you have the kill function.  */
#define HAVE_KILL 1

/* Define if you have the lstat function.  */
#define HAVE_LSTAT 1

/* Define if you have the munmap function.  */
#define HAVE_MUNMAP 1

/* Define if you have the popen function.  */
#define HAVE_POPEN 1

/* Define if you have the putc_unlocked function.  */
/* #undef HAVE_PUTC_UNLOCKED */

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the rindex function.  */
#define HAVE_RINDEX 1

/* Define if you have the setenv function.  */
#define HAVE_SETENV 1

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setrlimit function.  */
#define HAVE_SETRLIMIT 1

/* Define if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the sysconf function.  */
#define HAVE_SYSCONF 1

/* Define if you have the <argz.h> header file.  */
/* #undef HAVE_ARGZ_H */

/* Define if you have the <direct.h> header file.  */
/* #undef HAVE_DIRECT_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <nl_types.h> header file.  */
/* #undef HAVE_NL_TYPES_H */

/* Define if you have the <stab.h> header file.  */
/* #undef HAVE_STAB_H */

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/times.h> header file.  */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the i library (-li).  */
/* #undef HAVE_LIBI */

/* Define to 1 if you want to enable namespaces (-fhonor-std) by default. */
#define ENABLE_STD_NAMESPACE 1

/* The number of bytes in type short */
#define SIZEOF_SHORT 2

/* The number of bytes in type int */
#define SIZEOF_INT 4

/* The number of bytes in type long */
#define SIZEOF_LONG 4

/* Define if you have a working <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to the name of a file containing a list of extra machine modes
   for this architecture. */
#define EXTRA_MODES_FILE "arm/arm-modes.def"

/* Define if the target architecture needs extra machine modes to represent
   the results of comparisons. */
#define EXTRA_CC_MODES 1

/* Define if <sys/times.h> defines struct tms. */
#define HAVE_STRUCT_TMS 1

/* Define if <time.h> defines clock_t. */
#define HAVE_CLOCK_T 1

/* host dependent, where should it be defined ? */
#define HOST_FLOAT_WORDS_BIG_ENDIAN 0
#endif
#ifdef __MINGW32__
/* auto-host.h.  Generated automatically by configure.  */
/* config.in.  Generated automatically from configure.in by autoheader.  */
/* Define if you can safely include both <string.h> and <strings.h>.  */
#undef STRING_WITH_STRINGS

/* Define if printf supports "%p".  */
#define HAVE_PRINTF_PTR 1

/* Define if you want expensive run-time checks. */
/* #undef ENABLE_CHECKING */

/* Define to 1 if NLS is requested.  */
/* #undef ENABLE_NLS */

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
/* #undef HAVE_CATGETS */

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
/* #undef HAVE_GETTEXT */

/* Define if your cpp understands the stringify operator.  */
#define HAVE_CPP_STRINGIFY 1

/* Define if your compiler understands volatile.  */
#define HAVE_VOLATILE 1

/* Define if your assembler supports specifying the maximum number
   of bytes to skip when using the GAS .p2align command. */
#define HAVE_GAS_MAX_SKIP_P2ALIGN 1

/* Define if your assembler supports .balign and .p2align.  */
#define HAVE_GAS_BALIGN_AND_P2ALIGN 1

/* Define if your assembler supports .subsection and .subsection -1 starts
   emitting at the beginning of your section */
/* #undef HAVE_GAS_SUBSECTION_ORDERING */

/* Define if your assembler uses the old HImode fild and fist notation.  */
#define HAVE_GAS_FILDS_FISTS 1

/* Define if you have a working <inttypes.h> header file.  */
/* #undef HAVE_INTTYPES_H */

/* Define if your locale.h file contains LC_MESSAGES.  */
#undef HAVE_LC_MESSAGES

/* Define as 1 if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Whether malloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_MALLOC */

/* Whether realloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_REALLOC */

/* Whether calloc must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_CALLOC */

/* Whether free must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_FREE */

/* Whether bcopy must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BCOPY */

/* Whether bcmp must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BCMP */

/* Whether bzero must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_BZERO */

/* Whether index must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_INDEX */

/* Whether rindex must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_RINDEX */

/* Whether getenv must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_GETENV */

/* Whether atol must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ATOL */

/* Whether atof must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ATOF */

/* Whether sbrk must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_SBRK */

/* Whether abort must be declared even if <stdlib.h> is included.  */
/* #undef NEED_DECLARATION_ABORT */

/* Whether strerror must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRERROR */

/* Whether strsignal must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRSIGNAL */

/* Whether strstr must be declared even if <string.h> is included.  */
/* #undef NEED_DECLARATION_STRSTR */

/* Whether getcwd must be declared even if <unistd.h> is included.  */
/* #undef NEED_DECLARATION_GETCWD */

/* Whether getwd must be declared even if <unistd.h> is included.  */
/* #undef NEED_DECLARATION_GETWD */

/* Whether getrlimit must be declared even if <sys/resource.h> is included.  */
/* #undef NEED_DECLARATION_GETRLIMIT */

/* Whether setrlimit must be declared even if <sys/resource.h> is included.  */
/* #undef NEED_DECLARATION_SETRLIMIT */

/* Whether putc_unlocked must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_PUTC_UNLOCKED 1

/* Whether fputs_unlocked must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_FPUTS_UNLOCKED 1

/* Define to enable the use of a default assembler. */
/* #undef DEFAULT_ASSEMBLER */

/* Define to enable the use of a default linker. */
/* #undef DEFAULT_LINKER */

/* Define if host mkdir takes a single argument. */
/* #undef MKDIR_TAKES_ONE_ARG */

/* Define to the name of the distribution.  */
#define PACKAGE "gcc"

/* Define to the version of the distribution.  */
#define VERSION "2.95.2"

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have a working `mmap' system call.  */
/* #undef HAVE_MMAP */

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#undef HAVE_SYS_WAIT_H

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if `sys_siglist' is declared by <signal.h>.  */
/* #undef SYS_SIGLIST_DECLARED */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* Define if you have the __argz_count function.  */
/* #undef HAVE___ARGZ_COUNT */

/* Define if you have the __argz_next function.  */
/* #undef HAVE___ARGZ_NEXT */

/* Define if you have the __argz_stringify function.  */
/* #undef HAVE___ARGZ_STRINGIFY */

/* Define if you have the atoll function.  */
/* #undef HAVE_ATOLL */

/* Define if you have the atoq function.  */
/* #undef HAVE_ATOQ */

/* Define if you have the bcmp function.  */
#define HAVE_BCMP 1

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the bsearch function.  */
#define HAVE_BSEARCH 1

/* Define if you have the bzero function.  */
#define HAVE_BZERO 1

/* Define if you have the dcgettext function.  */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the fputc_unlocked function.  */
/* #undef HAVE_FPUTC_UNLOCKED */

/* Define if you have the fputs_unlocked function.  */
/* #undef HAVE_FPUTS_UNLOCKED */

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getrlimit function.  */
/*#define HAVE_GETRLIMIT 1*/

/* Define if you have the gettimeofday function.  */
#undef HAVE_GETTIMEOFDAY

/* Define if you have the index function.  */
#define HAVE_INDEX 1

/* Define if you have the isascii function.  */
#define HAVE_ISASCII 1

/* Define if you have the kill function.  */
#define HAVE_KILL 1

/* Define if you have the munmap function.  */
#define HAVE_MUNMAP 1

/* Define if you have the popen function.  */
#define HAVE_POPEN 1

/* Define if you have the putc_unlocked function.  */
/* #undef HAVE_PUTC_UNLOCKED */

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the rindex function.  */
#define HAVE_RINDEX 1

/* Define if you have the setenv function.  */
#define HAVE_SETENV 1

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setrlimit function.  */
#define HAVE_SETRLIMIT 1

/* Define if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the sysconf function.  */
#define HAVE_SYSCONF 1

/* Define if you have the <argz.h> header file.  */
/* #undef HAVE_ARGZ_H */

/* Define if you have the <direct.h> header file.  */
/* #undef HAVE_DIRECT_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <nl_types.h> header file.  */
/* #undef HAVE_NL_TYPES_H */

/* Define if you have the <stab.h> header file.  */
/* #undef HAVE_STAB_H */

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/param.h> header file.  */
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/resource.h> header file.  */
#undef HAVE_SYS_RESOURCE_H

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/times.h> header file.  */
#undef HAVE_SYS_TIMES_H

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the i library (-li).  */
/* #undef HAVE_LIBI */

/* Define to 1 if you want to enable namespaces (-fhonor-std) by default. */
#define ENABLE_STD_NAMESPACE 1

/* The number of bytes in type short */
#define SIZEOF_SHORT 2

/* The number of bytes in type int */
#define SIZEOF_INT 4

/* The number of bytes in type long */
#define SIZEOF_LONG 4

/* Define if you have a working <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to the name of a file containing a list of extra machine modes
   for this architecture. */
#define EXTRA_MODES_FILE "arm/arm-modes.def"

/* Define if the target architecture needs extra machine modes to represent
   the results of comparisons. */
#define EXTRA_CC_MODES 1

/* Define if <sys/times.h> defines struct tms. */
#define HAVE_STRUCT_TMS 1

/* Define if <time.h> defines clock_t. */
#define HAVE_CLOCK_T 1

/* host dependent, where should it be defined ? */
#define HOST_FLOAT_WORDS_BIG_ENDIAN 0
#endif

/* Machine mode definitions for GNU C-Compiler; included by rtl.h and tree.h.
   Copyright (C) 1991, 1993, 1994, 1996, 1998, 1999, 2000, 2001
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef HAVE_MACHINE_MODES
#define HAVE_MACHINE_MODES

/* Make an enum class that gives all the machine modes.  */

#define DEF_MACHMODE(SYM, NAME, TYPE, BITSIZE, SIZE, UNIT, WIDER, INNER)  SYM,

#ifndef TARG_ST
//enum machine_mode {
//#include "machmode.def"
//MAX_MACHINE_MODE };
#else
enum machine_mode {
/* This file contains the definitions and documentation for the
   machine modes used in the GNU compiler.
   Copyright (C) 1987, 1992, 1994, 1997, 1998, 2000 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */


/* This file defines all the MACHINE MODES used by GCC.

   A machine mode specifies a size and format of data
   at the machine level.

   Each RTL expression has a machine mode.

   At the syntax tree level, each ..._TYPE and each ..._DECL node
   has a machine mode which describes data of that type or the
   data of the variable declared.  */

/* The first argument is the internal name of the machine mode
   used in the C source.
   By convention these are in UPPER_CASE, except for the word  "mode".

   The second argument  is the name of the machine mode in the
   external ASCII format used for reading and printing RTL and trees.
   By convention these names in UPPER_CASE.

   Third argument states the kind of representation:
   MODE_INT - integer
   MODE_FLOAT - floating
   MODE_PARTIAL_INT - PQImode, PHImode, PSImode and PDImode
   MODE_CC - modes used for representing the condition code in a register
   MODE_COMPLEX_INT, MODE_COMPLEX_FLOAT - complex number
   MODE_VECTOR_INT, MODE_VECTOR_FLOAT - vector
   MODE_RANDOM - anything else

   Fourth argument is the relative size of the object, in bits,
   so we can have modes smaller than 1 byte.

   Fifth argument is the relative size of the object, in bytes.
   It is zero when the size is meaningless or not determined.
   A byte's size is determined by BITS_PER_UNIT in tm.h. 

   Sixth arg is the relative size of subunits of the object.
   It is same as the fifth argument except for complexes and vectors,
   since they are really made of many equal size subunits.

   Seventh arg is next wider natural mode of the same class.  0 if
   there is none.  Vector modes use this field to point to the next
   vector size, so we can iterate through the different vectors modes.
   The ordering is by increasing byte size, with QI coming before HI,
   HI before SI, etc.

   Eigth arg is the mode of the internal elements in a vector or
   complex, and VOIDmode if not applicable.
*/

/* VOIDmode is used when no mode needs to be specified,
   as for example on CONST_INT RTL expressions.  */
DEF_MACHMODE (VOIDmode, "VOID", MODE_RANDOM, 0, 0, 0, VOIDmode, VOIDmode)

DEF_MACHMODE (BImode, "BI", MODE_INT, 1, 1, 1, QImode, VOIDmode)
DEF_MACHMODE (QImode, "QI", MODE_INT, BITS_PER_UNIT, 1, 1, HImode, VOIDmode)
DEF_MACHMODE (HImode, "HI", MODE_INT, BITS_PER_UNIT*2, 2, 2, SImode, VOIDmode)
DEF_MACHMODE (SImode, "SI", MODE_INT, BITS_PER_UNIT*4, 4, 4, DImode, VOIDmode)
DEF_MACHMODE (DImode, "DI", MODE_INT, BITS_PER_UNIT*8, 8, 8, TImode, VOIDmode)
DEF_MACHMODE (TImode, "TI", MODE_INT, BITS_PER_UNIT*16, 16, 16, OImode, VOIDmode)
DEF_MACHMODE (OImode, "OI", MODE_INT, BITS_PER_UNIT*32, 32, 32, VOIDmode, VOIDmode)

/* Pointers on some machines use these types to distinguish them from
   ints.  Useful if a pointer is 4 bytes but has some bits that are
   not significant, so it is really not quite as wide as an integer.  */
DEF_MACHMODE (PQImode, "PQI", MODE_PARTIAL_INT, BITS_PER_UNIT, 1, 1, PHImode, VOIDmode)
DEF_MACHMODE (PHImode, "PHI", MODE_PARTIAL_INT, BITS_PER_UNIT*2, 2, 2, PSImode, VOIDmode)
DEF_MACHMODE (PSImode, "PSI", MODE_PARTIAL_INT, BITS_PER_UNIT*4, 4, 4, PDImode, VOIDmode)
DEF_MACHMODE (PDImode, "PDI", MODE_PARTIAL_INT, BITS_PER_UNIT*8, 8, 8, VOIDmode, VOIDmode)

DEF_MACHMODE (QFmode, "QF", MODE_FLOAT, BITS_PER_UNIT, 1, 1, HFmode, VOIDmode)
DEF_MACHMODE (HFmode, "HF", MODE_FLOAT, BITS_PER_UNIT*2, 2, 2, TQFmode, VOIDmode)
DEF_MACHMODE (TQFmode, "TQF", MODE_FLOAT, BITS_PER_UNIT*3, 3, 3, SFmode, VOIDmode) /* MIL-STD-1750A */
DEF_MACHMODE (SFmode, "SF", MODE_FLOAT, BITS_PER_UNIT*4, 4, 4, DFmode, VOIDmode)
DEF_MACHMODE (DFmode, "DF", MODE_FLOAT, BITS_PER_UNIT*8, 8, 8, XFmode, VOIDmode)
DEF_MACHMODE (XFmode, "XF", MODE_FLOAT, BITS_PER_UNIT*12, 12, 12, TFmode, VOIDmode) /* IEEE extended */
DEF_MACHMODE (TFmode, "TF", MODE_FLOAT, BITS_PER_UNIT*16, 16, 16, VOIDmode, VOIDmode)

/* Complex modes.  */
DEF_MACHMODE (QCmode, "QC", MODE_COMPLEX_FLOAT, BITS_PER_UNIT*2, 2, 1, HCmode, QFmode)
DEF_MACHMODE (HCmode, "HC", MODE_COMPLEX_FLOAT, BITS_PER_UNIT*4, 4, 2, SCmode, HFmode)
DEF_MACHMODE (SCmode, "SC", MODE_COMPLEX_FLOAT, BITS_PER_UNIT*8, 8, 4, DCmode, SFmode)
DEF_MACHMODE (DCmode, "DC", MODE_COMPLEX_FLOAT, BITS_PER_UNIT*16, 16, 8, XCmode, DFmode)
DEF_MACHMODE (XCmode, "XC", MODE_COMPLEX_FLOAT, BITS_PER_UNIT*24, 24, 12, TCmode, XFmode)
DEF_MACHMODE (TCmode, "TC", MODE_COMPLEX_FLOAT, BITS_PER_UNIT*32, 32, 16, VOIDmode, TFmode)

DEF_MACHMODE (CQImode, "CQI", MODE_COMPLEX_INT, BITS_PER_UNIT*2, 2, 1, CHImode, QImode)
DEF_MACHMODE (CHImode, "CHI", MODE_COMPLEX_INT, BITS_PER_UNIT*4, 4, 2, CSImode, HImode)
DEF_MACHMODE (CSImode, "CSI", MODE_COMPLEX_INT, BITS_PER_UNIT*8, 8, 4, CDImode, SImode)
DEF_MACHMODE (CDImode, "CDI", MODE_COMPLEX_INT, BITS_PER_UNIT*16, 16, 8, CTImode, DImode)
DEF_MACHMODE (CTImode, "CTI", MODE_COMPLEX_INT, BITS_PER_UNIT*32, 32, 16, COImode, TImode)
DEF_MACHMODE (COImode, "COI", MODE_COMPLEX_INT, BITS_PER_UNIT*64, 64, 32, VOIDmode, OImode)

/* Vector modes.  */
/* The wider mode field for vectors follows in order of increasing bit
   size with QI coming before HI, HI before SI, and SI before DI
   within same bit sizes.  */
DEF_MACHMODE (V1DImode, "V1DI", MODE_VECTOR_INT, BITS_PER_UNIT*8, 8, 8, V2QImode, DImode)
DEF_MACHMODE (V2QImode, "V2QI", MODE_VECTOR_INT, BITS_PER_UNIT*2, 2, 1, V4QImode, QImode)
DEF_MACHMODE (V2HImode, "V2HI", MODE_VECTOR_INT, BITS_PER_UNIT*4, 4, 2, V8QImode, HImode)
DEF_MACHMODE (V2SImode, "V2SI", MODE_VECTOR_INT, BITS_PER_UNIT*8, 8, 4, V16QImode, SImode)
DEF_MACHMODE (V2DImode, "V2DI", MODE_VECTOR_INT, BITS_PER_UNIT*16, 16, 8, V8SImode, DImode)

DEF_MACHMODE (V4QImode, "V4QI", MODE_VECTOR_INT, BITS_PER_UNIT*4, 4, 1, V2HImode, QImode)
DEF_MACHMODE (V4HImode, "V4HI", MODE_VECTOR_INT, BITS_PER_UNIT*8, 8, 2, V2SImode, HImode)
DEF_MACHMODE (V4SImode, "V4SI", MODE_VECTOR_INT, BITS_PER_UNIT*16, 16, 4, V2DImode, SImode)
DEF_MACHMODE (V4DImode, "V4DI", MODE_VECTOR_INT, BITS_PER_UNIT*32, 32, 8, V8DImode, DImode)

DEF_MACHMODE (V8QImode, "V8QI", MODE_VECTOR_INT, BITS_PER_UNIT*8, 8, 1, V4HImode, QImode)
DEF_MACHMODE (V8HImode, "V8HI", MODE_VECTOR_INT, BITS_PER_UNIT*16, 16, 2, V4SImode, HImode)
DEF_MACHMODE (V8SImode, "V8SI", MODE_VECTOR_INT, BITS_PER_UNIT*32, 32, 4, V4DImode, SImode)
DEF_MACHMODE (V8DImode, "V8DI", MODE_VECTOR_INT, BITS_PER_UNIT*64, 64, 8, VOIDmode, DImode)

DEF_MACHMODE (V16QImode, "V16QI", MODE_VECTOR_INT, BITS_PER_UNIT*16, 16, 1, V8HImode, QImode)

DEF_MACHMODE (V2SFmode, "V2SF", MODE_VECTOR_FLOAT, BITS_PER_UNIT*8, 8, 4, V4SFmode, SFmode)
DEF_MACHMODE (V2DFmode, "V2DF", MODE_VECTOR_FLOAT, BITS_PER_UNIT*16, 16, 8, V8SFmode, DFmode)

DEF_MACHMODE (V4SFmode, "V4SF", MODE_VECTOR_FLOAT, BITS_PER_UNIT*16, 16, 4, V2DFmode, SFmode)
DEF_MACHMODE (V4DFmode, "V4DF", MODE_VECTOR_FLOAT, BITS_PER_UNIT*32, 32, 8, V8DFmode, DFmode)

DEF_MACHMODE (V8SFmode, "V8SF", MODE_VECTOR_FLOAT, BITS_PER_UNIT*32, 32, 4,V4DFmode, SFmode)
DEF_MACHMODE (V8DFmode, "V8DF", MODE_VECTOR_FLOAT, BITS_PER_UNIT*64, 64, 8, VOIDmode, DFmode)
#ifndef TARG_ST
/* [TB] Put below */
DEF_MACHMODE (V16SFmode, "V16SF", MODE_VECTOR_FLOAT, 512, 64, 4, VOIDmode, SFmode)
#endif
/* BLKmode is used for structures, arrays, etc.
   that fit no more specific mode.  */
DEF_MACHMODE (BLKmode, "BLK", MODE_RANDOM, 0, 0, 0, VOIDmode, VOIDmode)

/* The modes for representing the condition codes come last.  CCmode
   is always defined.  Additional modes for the condition code can be
   specified in the EXTRA_CC_MODES header.  All MODE_CC modes are the
   same width as SImode and have VOIDmode as their next wider mode.  */

/* We do not use CC() for CCmode to avoid a warning about use of
   function-like macros with no arguments.  */
DEF_MACHMODE (CCmode, "CC", MODE_CC, BITS_PER_UNIT*4, 4, 4, VOIDmode, VOIDmode)

#ifdef EXTRA_MODES_FILE
#define CC(N)  \
  DEF_MACHMODE (CONCAT2 (N,mode), STRINGX (N), \
                MODE_CC, BITS_PER_UNIT*4, 4, 4, VOIDmode, VOIDmode)
/* Definitions of target machine for GNU compiler, for ARM.
   Copyright (C) 2002 Free Software Foundation, Inc.
   Contributed by Pieter `Tiggr' Schoenmakers (rcpieter@win.tue.nl)
   and Martin Simmons (@harleqn.co.uk).
   More major hacks by Richard Earnshaw (rearnsha@arm.com)
   Minor hacks by Nick Clifton (nickc@cygnus.com)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* CCFPEmode should be used with floating inequalities,
   CCFPmode should be used with floating equalities.
   CC_NOOVmode should be used with SImode integer equalities.
   CC_Zmode should be used if only the Z flag is set correctly
   CCmode should be used otherwise. */

CC (CC_NOOV)
CC (CC_Z)
CC (CC_SWP)
CC (CCFP)
CC (CCFPE)
CC (CC_DNE)
CC (CC_DEQ)
CC (CC_DLE)
CC (CC_DLT)
CC (CC_DGE)
CC (CC_DGT)
CC (CC_DLEU)
CC (CC_DLTU)
CC (CC_DGEU)
CC (CC_DGTU)
CC (CC_C)
#undef CC
#endif

#ifdef TARG_ST
     /* TB To put in place dynamic mode, I need to know the last enum
	value; So move V16SFmode after target dependant mode*/
DEF_MACHMODE (V16SFmode, "V16SF", MODE_VECTOR_FLOAT, 512, 64, 4, VOIDmode, SFmode)

#endif

/* The symbol Pmode stands for one of the above machine modes (usually SImode).
   The tm file specifies which one.  It is not a distinct mode.  */

/*
Local variables:
mode:c
version-control: t
End:
*/
// STATIC_COUNT_MACHINE_MODE is the last machine mode staticaly known
STATIC_COUNT_MACHINE_MODE,
// MAX_LIMIT_MACHINE_MODE is the static limit of nb of machine mode
// (constraint must be on 7 bit because of tree bit field
// ENUM_BITFIELD(machine_mode) mode : 7;
MAX_LIMIT_MACHINE_MODE = 127
 };
// COUNT_MACHINE_MODE = dynamic count of the last machine mode
// COUNT_MACHINE_MODE cannot be > to MAX_LIMIT_MACHINE_MODE
typedef int machine_mode_t;
extern machine_mode_t COUNT_MACHINE_MODE;
/* We define the gcc MAX_MACHINE_MODE to be the counter.*/
#define MAX_MACHINE_MODE (COUNT_MACHINE_MODE)
#endif
#undef DEF_MACHMODE

#ifdef TARG_ST
// Last machine mode statically defined 
#define MACHINE_MODE_STATIC_LAST (STATIC_COUNT_MACHINE_MODE-1)

/* Nonzero if MODE has been dynamically defined */
#define IS_DYNAMIC_MACHINE_MODE(MODE) (MODE > MACHINE_MODE_STATIC_LAST)
#endif

#ifndef NUM_MACHINE_MODES
#ifdef TARG_ST
#define NUM_MACHINE_MODES (int) COUNT_MACHINE_MODE
#else
#define NUM_MACHINE_MODES (int) MAX_MACHINE_MODE
#endif
#endif

/* Get the name of mode MODE as a string.  */

extern const char *mode_name[MAX_LIMIT_MACHINE_MODE];
#define GET_MODE_NAME(MODE)		(mode_name[(int) (MODE)])

enum mode_class { MODE_RANDOM, MODE_INT, MODE_FLOAT, MODE_PARTIAL_INT, MODE_CC,
		  MODE_COMPLEX_INT, MODE_COMPLEX_FLOAT,
		  MODE_VECTOR_INT, MODE_VECTOR_FLOAT,
		  MAX_MODE_CLASS};

/* Get the general kind of object that mode MODE represents
   (integer, floating, complex, etc.)  */

extern  enum mode_class mode_class[MAX_LIMIT_MACHINE_MODE];
#define GET_MODE_CLASS(MODE)		(mode_class[(int) (MODE)])

/* Nonzero if MODE is an integral mode.  */
#define INTEGRAL_MODE_P(MODE)			\
  (GET_MODE_CLASS (MODE) == MODE_INT		\
   || GET_MODE_CLASS (MODE) == MODE_PARTIAL_INT \
   || GET_MODE_CLASS (MODE) == MODE_COMPLEX_INT \
   || GET_MODE_CLASS (MODE) == MODE_VECTOR_INT)

/* Nonzero if MODE is a floating-point mode.  */
#define FLOAT_MODE_P(MODE)		\
  (GET_MODE_CLASS (MODE) == MODE_FLOAT	\
   || GET_MODE_CLASS (MODE) == MODE_COMPLEX_FLOAT \
   || GET_MODE_CLASS (MODE) == MODE_VECTOR_FLOAT)

/* Nonzero if MODE is a complex mode.  */
#define COMPLEX_MODE_P(MODE)			\
  (GET_MODE_CLASS (MODE) == MODE_COMPLEX_INT	\
   || GET_MODE_CLASS (MODE) == MODE_COMPLEX_FLOAT)

/* Nonzero if MODE is a vector mode.  */
#define VECTOR_MODE_P(MODE)			\
  (GET_MODE_CLASS (MODE) == MODE_VECTOR_INT	\
   || GET_MODE_CLASS (MODE) == MODE_VECTOR_FLOAT)

/* Nonzero if MODE is a scalar integral mode.  */
#define SCALAR_INT_MODE_P(MODE)			\
  (GET_MODE_CLASS (MODE) == MODE_INT		\
   || GET_MODE_CLASS (MODE) == MODE_PARTIAL_INT)

/* Nonzero if MODE is a scalar floating point mode.  */
#define SCALAR_FLOAT_MODE_P(MODE)		\
  (GET_MODE_CLASS (MODE) == MODE_FLOAT)

/* Get the size in bytes of an object of mode MODE.  */

#ifdef TARG_ST
// [TTh] Support types wider than 255 bytes
extern  unsigned short mode_size[MAX_LIMIT_MACHINE_MODE];
#else
extern  unsigned char mode_size[MAX_LIMIT_MACHINE_MODE];
#endif
#define GET_MODE_SIZE(MODE)		(mode_size[(int) (MODE)])

/* Get the size in bytes of the basic parts of an object of mode MODE.  */

#ifdef TARG_ST
// [TTh] Support types wider than 255 bytes
extern  unsigned short mode_unit_size[MAX_LIMIT_MACHINE_MODE];
#else
extern  unsigned char mode_unit_size[MAX_LIMIT_MACHINE_MODE];
#endif
#define GET_MODE_UNIT_SIZE(MODE)	(mode_unit_size[(int) (MODE)])

/* Get the number of units in the object.  */

#define GET_MODE_NUNITS(MODE)  \
  ((GET_MODE_UNIT_SIZE ((MODE)) == 0) ? 0 \
   : (GET_MODE_SIZE ((MODE)) / GET_MODE_UNIT_SIZE ((MODE))))

/* Get the size in bits of an object of mode MODE.  */

extern  unsigned short mode_bitsize[MAX_LIMIT_MACHINE_MODE];
#define GET_MODE_BITSIZE(MODE)  (mode_bitsize[(int) (MODE)])

#endif /* not HAVE_MACHINE_MODES */

#if defined HOST_WIDE_INT && ! defined GET_MODE_MASK

/* Get a bitmask containing 1 for all bits in a word
   that fit within mode MODE.  */

extern  unsigned HOST_WIDE_INT mode_mask_array[MAX_LIMIT_MACHINE_MODE];

#define GET_MODE_MASK(MODE) mode_mask_array[(int) (MODE)]

extern  machine_mode_t inner_mode_array[MAX_LIMIT_MACHINE_MODE];

/* Return the mode of the inner elements in a vector.  */

#define GET_MODE_INNER(MODE) inner_mode_array[(int) (MODE)]

#endif /* defined (HOST_WIDE_INT) && ! defined GET_MODE_MASK */

#if ! defined GET_MODE_WIDER_MODE || ! defined GET_MODE_ALIGNMENT \
    || ! defined GET_CLASS_NARROWEST_MODE

/* Get the next wider natural mode (eg, QI -> HI -> SI -> DI -> TI).  */

extern  unsigned char mode_wider_mode[MAX_LIMIT_MACHINE_MODE];
#define GET_MODE_WIDER_MODE(MODE)	((machine_mode_t)mode_wider_mode[(int) (MODE)])

/* Return the mode for data of a given size SIZE and mode class CLASS.
   If LIMIT is nonzero, then don't use modes bigger than MAX_FIXED_MODE_SIZE.
   The value is BLKmode if no other mode is found.  */
extern machine_mode_t mode_for_size PARAMS ((unsigned int,
						enum mode_class, int));

/* Similar, but find the smallest mode for a given width.  */

extern machine_mode_t smallest_mode_for_size 
				PARAMS ((unsigned int, enum mode_class));


/* Return an integer mode of the exact same size as the input mode,
   or BLKmode on failure.  */

extern machine_mode_t int_mode_for_mode PARAMS ((machine_mode_t));

/* Find the best mode to use to access a bit field.  */

extern machine_mode_t get_best_mode PARAMS ((int, int, unsigned int,
						machine_mode_t, int));

/* Determine alignment, 1<=result<=BIGGEST_ALIGNMENT.  */

extern unsigned get_mode_alignment PARAMS ((machine_mode_t));

#define GET_MODE_ALIGNMENT(MODE) get_mode_alignment (MODE)

/* For each class, get the narrowest mode in that class.  */

extern const machine_mode_t class_narrowest_mode[(int) MAX_MODE_CLASS];
#define GET_CLASS_NARROWEST_MODE(CLASS) class_narrowest_mode[(int) (CLASS)]

/* Define the integer modes whose sizes are BITS_PER_UNIT and BITS_PER_WORD
   and the mode whose class is Pmode and whose size is POINTER_SIZE.  */

extern machine_mode_t byte_mode;
extern machine_mode_t word_mode;
extern machine_mode_t ptr_mode;

#endif /* ! defined GET_MODE_WIDER_MODE || ! defined GET_MODE_ALIGNMENT
	  || ! defined GET_CLASS_NARROWEST_MODE */
#ifndef BUILTINS_H
#define BUILTINS_H


/* Codes that identify the various built in functions
   so that expand_call can identify them quickly.  */

#define DEF_BUILTIN(ENUM, N, C, T, LT, B, F, NA, AT) ENUM,
enum built_in_function
{
/* This file contains the definitions and documentation for the
   builtins used in the GNU compiler.
   Copyright (C) 2000, 2001 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

/* Before including this file, you should define a macro:

     DEF_BUILTIN (ENUM, NAME, CLASS, TYPE, LIBTYPE, BOTH_P,
                  FALLBACK_P, NONANSI_P, ATTRS)

   This macro will be called once for each builtin function.  The
   ENUM will be of type `enum built_in_function', and will indicate
   which builtin function is being processed.  The NAME of the builtin
   function (which will always start with `__builtin_') is a string
   literal.  The CLASS is of type `enum built_in_class' and indicates
   what kind of builtin is being processed.

   Some builtins are actually two separate functions.  For example,
   for `strcmp' there are two builtin functions; `__builtin_strcmp'
   and `strcmp' itself.  Both behave identically.  Other builtins
   define only the `__builtin' variant.  If BOTH_P is TRUE, then this
   builtin has both variants; otherwise, it is has only the first
   variant.

   TYPE indicates the type of the function.  The symbols correspond to
   enumerals from builtin-types.def.  If BOTH_P is true, then LIBTYPE
   is the type of the non-`__builtin_' variant.  Otherwise, LIBTYPE
   should be ignored.

   If FALLBACK_P is true then, if for some reason, the compiler cannot
   expand the builtin function directly, it will call the
   corresponding library function (which does not have the
   `__builtin_' prefix.

   If NONANSI_P is true, then the non-`__builtin_' variant is not an
   ANSI/ISO library function, and so we should pretend it does not
   exist when compiling in ANSI conformant mode.

   ATTRs is an attribute list as defined in builtin-attrs.def that
   describes the attributes of this builtin function.  */
   
/* A GCC builtin (like __builtin_saveregs) is provided by the
   compiler, but does not correspond to a function in the standard
   library.  */
#undef DEF_GCC_BUILTIN
#define DEF_GCC_BUILTIN(ENUM, NAME, TYPE, ATTRS)		\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, BT_LAST,	\
               false, false, false, ATTRS)


/* A fallback builtin is a builtin (like __builtin_puts) that falls
   back to the corresopnding library function if necessary -- but
   for which we should not introduce the non-`__builtin' variant of
   the name.  */
#undef DEF_FALLBACK_BUILTIN				
#define DEF_FALLBACK_BUILTIN(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
	       false, true, false, ATTRS)

/* Like DEF_FALLBACK_BUILTIN, except that the function is not one that
   is specified by ANSI/ISO C.  So, when we're being fully conformant
   we ignore the version of these builtins that does not begin with
   __builtin.  */
#undef DEF_EXT_FALLBACK_BUILTIN
#define DEF_EXT_FALLBACK_BUILTIN(ENUM, NAME, TYPE)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
	       false, true, true, ATTR_NOTHROW_LIST)

/* A library builtin (like __builtin_strchr) is a builtin equivalent
   of an ANSI/ISO standard library function.  In addition to the
   `__builtin' version, we will create an ordinary version (e.g,
   `strchr') as well.  If we cannot compute the answer using the
   builtin function, we will fall back to the standard library
   version.  */
#undef DEF_LIB_BUILTIN					
#define DEF_LIB_BUILTIN(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
	       true, true, false, ATTRS)

/* Like DEF_LIB_BUILTIN, except that a call to the builtin should
   never fall back to the library version.  */
#undef DEF_LIB_ALWAYS_BUILTIN				
#define DEF_LIB_ALWAYS_BUILTIN(ENUM, NAME, TYPE)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
    	       true, false, true, ATTR_CONST_NOTHROW_LIST)

/* Like DEF_LIB_BUILTIN, except that the function is not one that is
   specified by ANSI/ISO C.  So, when we're being fully conformant we
   ignore the version of these builtins that does not begin with
   __builtin.  */
#undef DEF_EXT_LIB_BUILTIN				
#define DEF_EXT_LIB_BUILTIN(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
   	       true, true, true, ATTRS)
#ifdef TARG_ST
/* Like DEF_EXT_LIB_BUILTIN but the version without "__builtin_"
   prefix is effectively not considered as a builtin.
 */
#undef DEF_EXT_LIB_BUILTIN_SINGLEDEF				
#define DEF_EXT_LIB_BUILTIN_SINGLEDEF(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
   	       false, true, true, ATTRS)
#endif

/* Like DEF_LIB_BUILTIN, except that the function is only a part of
   the standard in C99 or above.  */
#undef DEF_C99_BUILTIN					
#define DEF_C99_BUILTIN(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_NORMAL, TYPE, TYPE,	\
   	       true, true, !flag_isoc99, ATTRS)

/* Like DEF_LIB_BUILTIN, except that the function is expanded in the
   front-end.  */
#undef DEF_FRONT_END_LIB_BUILTIN			
#define DEF_FRONT_END_LIB_BUILTIN(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_FRONTEND, TYPE, TYPE,	\
	       true, true, false, ATTRS)

/* Like DEF_FRONT_END_LIB_BUILTIN, except that the function is not one
   that is specified by ANSI/ISO C.  So, when we're being fully
   conformant we ignore the version of these builtins that does not
   begin with __builtin.  */
#undef DEF_EXT_FRONT_END_LIB_BUILTIN			
#define DEF_EXT_FRONT_END_LIB_BUILTIN(ENUM, NAME, TYPE, ATTRS)	\
  DEF_BUILTIN (ENUM, NAME, BUILT_IN_FRONTEND, TYPE, TYPE,	\
	       true, true, true, ATTRS)

/* A built-in that is not currently used.  */
#undef DEF_UNUSED_BUILTIN					
#define DEF_UNUSED_BUILTIN(X)					\
  DEF_BUILTIN (X, (const char *) NULL, NOT_BUILT_IN, BT_LAST,	\
	       BT_LAST, false, false, false, ATTR_NOTHROW_LIST)

/* If SMALL_STACK is defined, then `alloca' is only defined in its
   `__builtin' form.  */
#if SMALL_STACK  
DEF_FALLBACK_BUILTIN(BUILT_IN_ALLOCA,
		     "__builtin_alloca",
		     BT_FN_PTR_SIZE,
		     ATTR_MALLOC_NOTHROW_LIST)
#else
DEF_EXT_LIB_BUILTIN(BUILT_IN_ALLOCA,
		    "__builtin_alloca",
		    BT_FN_PTR_SIZE,
		    ATTR_MALLOC_NOTHROW_LIST)
#endif

DEF_LIB_ALWAYS_BUILTIN(BUILT_IN_ABS,
		       "__builtin_abs",
		       BT_FN_INT_INT)
DEF_LIB_ALWAYS_BUILTIN(BUILT_IN_LABS,
		       "__builtin_labs",
		       BT_FN_LONG_LONG)

DEF_LIB_ALWAYS_BUILTIN(BUILT_IN_FABS,
		       "__builtin_fabs",
		       BT_FN_DOUBLE_DOUBLE)
DEF_LIB_ALWAYS_BUILTIN(BUILT_IN_FABSF,
		       "__builtin_fabsf",
		       BT_FN_FLOAT_FLOAT)
DEF_LIB_ALWAYS_BUILTIN(BUILT_IN_FABSL,
		       "__builtin_fabsl",
		       BT_FN_LONG_DOUBLE_LONG_DOUBLE)

DEF_C99_BUILTIN(BUILT_IN_LLABS,
		"__builtin_llabs",
		BT_FN_LONGLONG_LONGLONG,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_IMAXABS,
		"__builtin_imaxabs",
		BT_FN_INTMAX_INTMAX,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CONJ,
		"__builtin_conj",
		BT_FN_COMPLEX_DOUBLE_COMPLEX_DOUBLE,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CONJF,
		"__builtin_conjf",
		BT_FN_COMPLEX_FLOAT_COMPLEX_FLOAT,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CONJL,
		"__builtin_conjl",
		BT_FN_COMPLEX_LONG_DOUBLE_COMPLEX_LONG_DOUBLE,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CREAL,
		"__builtin_creal",
		BT_FN_DOUBLE_COMPLEX_DOUBLE,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CREALF,
		"__builtin_crealf",
		BT_FN_FLOAT_COMPLEX_FLOAT,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CREALL,
		"__builtin_creall",
		BT_FN_LONG_DOUBLE_COMPLEX_LONG_DOUBLE,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CIMAG,
		"__builtin_cimag",
		BT_FN_DOUBLE_COMPLEX_DOUBLE,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CIMAGF,
		"__builtin_cimagf",
		BT_FN_FLOAT_COMPLEX_FLOAT,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_CIMAGL,
		"__builtin_cimagl",
		BT_FN_LONG_DOUBLE_COMPLEX_LONG_DOUBLE,
		ATTR_NOTHROW_LIST)
#ifdef BUILT_IN_DIV
DEF_UNUSED_BUILTIN(BUILT_IN_DIV)
#endif
#ifdef BUILT_IN_LDIV
DEF_UNUSED_BUILTIN(BUILT_IN_LDIV)
#endif
#ifdef BUILT_IN_FFLOOR
DEF_UNUSED_BUILTIN(BUILT_IN_FFLOOR)
#endif
#ifdef BUILT_IN_FCEIL
DEF_UNUSED_BUILTIN(BUILT_IN_FCEIL)
#endif
#ifdef BUILT_IN_FMOD
DEF_UNUSED_BUILTIN(BUILT_IN_FMOD)
#endif
#ifdef BUILT_IN_FREM
DEF_UNUSED_BUILTIN(BUILT_IN_FREM)
#endif
/* The system prototypes for `bzero' and `bcmp' functions have many
   variations, so don't specify parameters to avoid conflicts.  The
   expand_* functions check the argument types anyway.  */
DEF_BUILTIN (BUILT_IN_BZERO,
	     "__builtin_bzero",
	     BUILT_IN_NORMAL,
	     BT_FN_VOID_PTR_SIZE, 
	     BT_FN_VOID_VAR,
	     true, true, true,
	     ATTR_NOTHROW_LIST)
DEF_BUILTIN (BUILT_IN_BCMP,
	     "__builtin_bcmp",
	     BUILT_IN_NORMAL,
	     BT_FN_INT_CONST_PTR_CONST_PTR_SIZE,
	     BT_FN_INT_VAR,
	     true, true, true,
	     ATTR_PURE_NOTHROW_LIST)

DEF_EXT_LIB_BUILTIN(BUILT_IN_FFS,
		    "__builtin_ffs",
		    BT_FN_INT_INT,
		    ATTR_CONST_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN(BUILT_IN_INDEX,
		    "__builtin_index",
		    BT_FN_STRING_CONST_STRING_INT,
		    ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN(BUILT_IN_RINDEX,
		    "__builtin_rindex",
		    BT_FN_STRING_CONST_STRING_INT,
		    ATTR_PURE_NOTHROW_LIST)

DEF_LIB_BUILTIN(BUILT_IN_MEMCPY,
		"__builtin_memcpy",
		BT_FN_PTR_PTR_CONST_PTR_SIZE,
		ATTR_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_MEMCMP,
		"__builtin_memcmp",
		BT_FN_INT_CONST_PTR_CONST_PTR_SIZE,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_MEMSET,
		"__builtin_memset",
		BT_FN_PTR_PTR_INT_SIZE,
		ATTR_NOTHROW_LIST)

DEF_LIB_BUILTIN(BUILT_IN_STRCAT,
		"__builtin_strcat",
		BT_FN_STRING_STRING_CONST_STRING,
		ATTR_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRNCAT,
		"__builtin_strncat",
		BT_FN_STRING_STRING_CONST_STRING_SIZE,
		ATTR_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRCPY,
		"__builtin_strcpy",
		BT_FN_STRING_STRING_CONST_STRING,
		ATTR_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRNCPY,
		"__builtin_strncpy",
		BT_FN_STRING_STRING_CONST_STRING_SIZE,
		ATTR_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRCMP,
		"__builtin_strcmp",
		BT_FN_INT_CONST_STRING_CONST_STRING,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRNCMP,
		"__builtin_strncmp",
		BT_FN_INT_CONST_STRING_CONST_STRING_SIZE,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRLEN,
		"__builtin_strlen",
		BT_FN_SIZE_CONST_STRING,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRSTR,
		"__builtin_strstr",
		BT_FN_STRING_CONST_STRING_CONST_STRING,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRPBRK,
		"__builtin_strpbrk",
		BT_FN_STRING_CONST_STRING_CONST_STRING,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRSPN,
		"__builtin_strspn",
		BT_FN_SIZE_CONST_STRING_CONST_STRING,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRCSPN,
		"__builtin_strcspn",
		BT_FN_SIZE_CONST_STRING_CONST_STRING,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRCHR,
		"__builtin_strchr",
		BT_FN_STRING_CONST_STRING_INT,
		ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_STRRCHR,
		"__builtin_strrchr",
		BT_FN_STRING_CONST_STRING_INT,
		ATTR_PURE_NOTHROW_LIST)

DEF_LIB_BUILTIN(BUILT_IN_SQRT,
		"__builtin_sqrt",
		BT_FN_DOUBLE_DOUBLE,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_SIN,
		"__builtin_sin",
		BT_FN_DOUBLE_DOUBLE,
		flag_unsafe_math_optimizations ? ATTR_CONST_NOTHROW_LIST
					       : ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_COS,
		"__builtin_cos",
		BT_FN_DOUBLE_DOUBLE,
		flag_unsafe_math_optimizations ? ATTR_CONST_NOTHROW_LIST
					       : ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_EXP,
		"__builtin_exp",
		BT_FN_DOUBLE_DOUBLE,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_LOG,
		"__builtin_log",
		BT_FN_DOUBLE_DOUBLE,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_SQRTF,
		"__builtin_sqrtf",
		BT_FN_FLOAT_FLOAT,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_SINF,
		"__builtin_sinf",
		BT_FN_FLOAT_FLOAT,
		flag_unsafe_math_optimizations ? ATTR_CONST_NOTHROW_LIST
					       : ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_COSF,
		"__builtin_cosf",
		BT_FN_FLOAT_FLOAT,
		flag_unsafe_math_optimizations ? ATTR_CONST_NOTHROW_LIST
					       : ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_EXPF,
		"__builtin_expf",
		BT_FN_FLOAT_FLOAT,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_LOGF,
		"__builtin_logf",
		BT_FN_FLOAT_FLOAT,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_SQRTL,
		"__builtin_sqrtl",
		BT_FN_LONG_DOUBLE_LONG_DOUBLE,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_SINL,
		"__builtin_sinl",
		BT_FN_LONG_DOUBLE_LONG_DOUBLE,
		flag_unsafe_math_optimizations ? ATTR_CONST_NOTHROW_LIST
					       : ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_COSL,
		"__builtin_cosl",
		BT_FN_LONG_DOUBLE_LONG_DOUBLE,
		flag_unsafe_math_optimizations ? ATTR_CONST_NOTHROW_LIST
					       : ATTR_PURE_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_EXPL,
		"__builtin_expl",
		BT_FN_LONG_DOUBLE_LONG_DOUBLE,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))
DEF_LIB_BUILTIN(BUILT_IN_LOGL,
		"__builtin_logl",
		BT_FN_LONG_DOUBLE_LONG_DOUBLE,
		flag_errno_math ? ATTR_NOTHROW_LIST
				: (flag_unsafe_math_optimizations
				   ? ATTR_CONST_NOTHROW_LIST
				   : ATTR_PURE_NOTHROW_LIST))

DEF_GCC_BUILTIN(BUILT_IN_INF,
		"__builtin_inf",
		BT_FN_DOUBLE,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_INFF,
		"__builtin_inff",
		BT_FN_FLOAT,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_INFL,
		"__builtin_infl",
		BT_FN_LONG_DOUBLE,
		ATTR_CONST_NOTHROW_LIST)

DEF_GCC_BUILTIN(BUILT_IN_HUGE_VAL,
		"__builtin_huge_val",
		BT_FN_DOUBLE,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_HUGE_VALF,
		"__builtin_huge_valf",
		BT_FN_FLOAT,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_HUGE_VALL,
		"__builtin_huge_vall",
		BT_FN_LONG_DOUBLE,
		ATTR_CONST_NOTHROW_LIST)

DEF_LIB_BUILTIN(BUILT_IN_NAN,
		"__builtin_nan",
		BT_FN_DOUBLE_CONST_STRING,
		ATTR_CONST_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_NANF,
		"__builtin_nanf",
		BT_FN_FLOAT_CONST_STRING,
		ATTR_CONST_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_NANL,
		"__builtin_nanl",
		BT_FN_LONG_DOUBLE_CONST_STRING,
		ATTR_CONST_NOTHROW_LIST)

DEF_LIB_BUILTIN(BUILT_IN_NANS,
		"__builtin_nans",
		BT_FN_DOUBLE_CONST_STRING,
		ATTR_CONST_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_NANSF,
		"__builtin_nansf",
		BT_FN_FLOAT_CONST_STRING,
		ATTR_CONST_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_NANSL,
		"__builtin_nansl",
		BT_FN_LONG_DOUBLE_CONST_STRING,
		ATTR_CONST_NOTHROW_LIST)

DEF_GCC_BUILTIN(BUILT_IN_SAVEREGS,
		"__builtin_saveregs",
		BT_FN_PTR_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_CLASSIFY_TYPE,
		"__builtin_classify_type",
		BT_FN_INT_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_NEXT_ARG,
		"__builtin_next_arg",
		BT_FN_PTR_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_ARGS_INFO,
		"__builtin_args_info",
		BT_FN_INT_INT,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_CONSTANT_P, 
		"__builtin_constant_p", 
		BT_FN_INT_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_FRAME_ADDRESS,
		"__builtin_frame_address",
		BT_FN_PTR_UNSIGNED,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_RETURN_ADDRESS, 
		"__builtin_return_address",
		BT_FN_PTR_UNSIGNED,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_AGGREGATE_INCOMING_ADDRESS,
		"__builtin_aggregate_incoming_address",
		BT_FN_PTR_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_APPLY_ARGS,
		"__builtin_apply_args",
		BT_FN_PTR_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_APPLY,
		"__builtin_apply",
		BT_FN_PTR_PTR_FN_VOID_VAR_PTR_SIZE,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_RETURN,
		"__builtin_return",
		BT_FN_VOID_PTR,
		ATTR_NORETURN_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_SETJMP,
		"__builtin_setjmp",
		BT_FN_INT_PTR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_LONGJMP,
		"__builtin_longjmp",
		BT_FN_VOID_PTR_INT,
		ATTR_NORETURN_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_TRAP,
		"__builtin_trap",
		BT_FN_VOID,
		ATTR_NORETURN_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_PREFETCH,
		"__builtin_prefetch",
		BT_FN_VOID_CONST_PTR_VAR,
		ATTR_NULL)

DEF_GCC_BUILTIN(BUILT_IN_ASSUME,
		"__builtin_assume",
		BT_FN_VOID_INT,
		ATTR_NULL)

/* stdio.h builtins (without FILE *).  */

DEF_FRONT_END_LIB_BUILTIN(BUILT_IN_PRINTF,
			  "__builtin_printf",
			  BT_FN_INT_CONST_STRING_VAR,
			  ATTR_FORMAT_PRINTF_1_2)
DEF_LIB_BUILTIN(BUILT_IN_PUTCHAR,
		"__builtin_putchar",
		BT_FN_INT_INT,
		ATTR_NOTHROW_LIST)
DEF_LIB_BUILTIN(BUILT_IN_PUTS,
		"__builtin_puts",
		BT_FN_INT_CONST_STRING,
		ATTR_NOTHROW_LIST)
DEF_C99_BUILTIN(BUILT_IN_SNPRINTF,
		"__builtin_snprintf",
		BT_FN_INT_STRING_SIZE_CONST_STRING_VAR,
		ATTR_FORMAT_PRINTF_3_4)
DEF_LIB_BUILTIN(BUILT_IN_SPRINTF,
		"__builtin_sprintf",
		BT_FN_INT_STRING_CONST_STRING_VAR,
		ATTR_FORMAT_PRINTF_2_3)
DEF_LIB_BUILTIN(BUILT_IN_SCANF,
		"__builtin_scanf",
		BT_FN_INT_CONST_STRING_VAR,
		ATTR_FORMAT_SCANF_1_2)
DEF_LIB_BUILTIN(BUILT_IN_SSCANF,
		"__builtin_sscanf",
		BT_FN_INT_CONST_STRING_CONST_STRING_VAR,
		ATTR_FORMAT_SCANF_2_3)
DEF_LIB_BUILTIN(BUILT_IN_VPRINTF,
		"__builtin_vprintf",
		BT_FN_INT_CONST_STRING_VALIST_ARG,
		ATTR_FORMAT_PRINTF_1_0)
DEF_C99_BUILTIN(BUILT_IN_VSCANF,
		"__builtin_vscanf",
		BT_FN_INT_CONST_STRING_VALIST_ARG,
		ATTR_FORMAT_SCANF_1_0)
DEF_C99_BUILTIN(BUILT_IN_VSSCANF,
		"__builtin_vsscanf",
		BT_FN_INT_CONST_STRING_CONST_STRING_VALIST_ARG,
		ATTR_FORMAT_SCANF_2_0)
DEF_C99_BUILTIN(BUILT_IN_VSNPRINTF,
		"__builtin_vsnprintf",
		BT_FN_INT_STRING_SIZE_CONST_STRING_VALIST_ARG,
		ATTR_FORMAT_PRINTF_3_0)
DEF_LIB_BUILTIN(BUILT_IN_VSPRINTF,
		"__builtin_vsprintf",
		BT_FN_INT_STRING_CONST_STRING_VALIST_ARG,
		ATTR_FORMAT_PRINTF_2_0)


/* stdio.h builtins (with FILE *).  */

/* Declare the __builtin_ style with arguments and the regular style
   without them.  We rely on stdio.h to supply the arguments for the
   regular style declaration since we had to use void* instead of
   FILE* in the __builtin_ prototype supplied here.  */

DEF_FALLBACK_BUILTIN(BUILT_IN_FPUTC,
		     "__builtin_fputc",
		     BT_FN_INT_INT_PTR,
		     ATTR_NOTHROW_LIST)
DEF_BUILTIN (BUILT_IN_FPUTS,
	     "__builtin_fputs",
	     BUILT_IN_NORMAL,
	     BT_FN_INT_CONST_STRING_PTR,
	     BT_FN_INT_VAR,
	     true, true, false, ATTR_NOTHROW_LIST)
DEF_FALLBACK_BUILTIN(BUILT_IN_FWRITE,
		     "__builtin_fwrite",
		     BT_FN_SIZE_CONST_PTR_SIZE_SIZE_PTR,
		      ATTR_NOTHROW_LIST)
DEF_FRONT_END_LIB_BUILTIN(BUILT_IN_FPRINTF,
			 "__builtin_fprintf",
			 BT_FN_INT_PTR_CONST_STRING_VAR,
			 ATTR_FORMAT_PRINTF_2_3)

/* stdio unlocked builtins (without FILE *).  */

DEF_EXT_FALLBACK_BUILTIN(BUILT_IN_PUTCHAR_UNLOCKED,
			 "__builtin_putchar_unlocked",
			 BT_FN_INT_INT)
DEF_EXT_FALLBACK_BUILTIN(BUILT_IN_PUTS_UNLOCKED,
			 "__builtin_puts_unlocked",
			 BT_FN_INT_CONST_STRING)
DEF_EXT_FRONT_END_LIB_BUILTIN(BUILT_IN_PRINTF_UNLOCKED,
			      "__builtin_printf_unlocked",
			      BT_FN_INT_CONST_STRING_VAR,
			      ATTR_FORMAT_PRINTF_1_2)

/* stdio unlocked builtins (with FILE *).  */

/* Declare the __builtin_ style with arguments and the regular style
   without them.  We rely on stdio.h to supply the arguments for the
   regular style declaration since we had to use void* instead of
   FILE* in the __builtin_ prototype supplied here.  */

DEF_EXT_FALLBACK_BUILTIN(BUILT_IN_FPUTC_UNLOCKED,
			 "__builtin_fputc_unlocked",
			 BT_FN_INT_INT_PTR)
DEF_BUILTIN (BUILT_IN_FPUTS_UNLOCKED,
	     "__builtin_fputs_unlocked",
	     BUILT_IN_NORMAL,
	     BT_FN_INT_CONST_STRING_PTR,
	     BT_FN_INT_VAR,
	     true, true, true, ATTR_NOTHROW_LIST)
DEF_EXT_FALLBACK_BUILTIN(BUILT_IN_FWRITE_UNLOCKED,
			 "__builtin_fwrite_unlocked",
			 BT_FN_SIZE_CONST_PTR_SIZE_SIZE_PTR)
DEF_EXT_FRONT_END_LIB_BUILTIN(BUILT_IN_FPRINTF_UNLOCKED,
			      "__builtin_fprintf_unlocked",
			      BT_FN_INT_PTR_CONST_STRING_VAR,
			      ATTR_FORMAT_PRINTF_2_3)

  /* ISO C99 floating point unordered comparisons.  */
DEF_GCC_BUILTIN(BUILT_IN_ISGREATER, 
		"__builtin_isgreater",
		BT_FN_INT_VAR,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_ISGREATEREQUAL,
		"__builtin_isgreaterequal",
		BT_FN_INT_VAR,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_ISLESS,
		"__builtin_isless",
		BT_FN_INT_VAR,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_ISLESSEQUAL,
		"__builtin_islessequal",
		BT_FN_INT_VAR,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_ISLESSGREATER,
		"__builtin_islessgreater",
		BT_FN_INT_VAR,
		ATTR_CONST_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_ISUNORDERED,
		"__builtin_isunordered",
		BT_FN_INT_VAR,
		ATTR_CONST_NOTHROW_LIST)

/* Various hooks for the DWARF 2 __throw routine.  */
DEF_GCC_BUILTIN(BUILT_IN_UNWIND_INIT,
		"__builtin_unwind_init",
		BT_FN_VOID,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_DWARF_CFA,
		"__builtin_dwarf_cfa",
		BT_FN_PTR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_DWARF_SP_COLUMN,
		"__builtin_dwarf_sp_column",
		BT_FN_UNSIGNED,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_INIT_DWARF_REG_SIZES,
		"__builtin_init_dwarf_reg_size_table",
		BT_FN_VOID_PTR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_FROB_RETURN_ADDR,
		"__builtin_frob_return_addr",
		BT_FN_PTR_PTR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_EXTRACT_RETURN_ADDR,
		"__builtin_extract_return_addr",
		BT_FN_PTR_PTR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_EH_RETURN,
		"__builtin_eh_return",
		BT_FN_VOID_PTRMODE_PTR,
		ATTR_NORETURN_NOTHROW_LIST)
DEF_GCC_BUILTIN(BUILT_IN_EH_RETURN_DATA_REGNO,
		"__builtin_eh_return_data_regno",
		BT_FN_INT_INT,
		ATTR_NULL)

/* Variable argument list (stdarg.h) support */
DEF_GCC_BUILTIN(BUILT_IN_VA_START,
		"__builtin_va_start",
		BT_FN_VOID_VALIST_REF_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_STDARG_START,		/* backward compat */
		"__builtin_stdarg_start",
		BT_FN_VOID_VALIST_REF_VAR,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_VA_END,
		"__builtin_va_end",
		BT_FN_VOID_VALIST_REF,
		ATTR_NULL)
DEF_GCC_BUILTIN(BUILT_IN_VA_COPY,
		"__builtin_va_copy",
		BT_FN_VOID_VALIST_REF_VALIST_ARG,
		ATTR_NULL)

DEF_GCC_BUILTIN(BUILT_IN_EXPECT,
		"__builtin_expect",
		BT_FN_LONG_LONG_LONG,
		ATTR_NULL)

/* C++ extensions */
#ifdef BUILT_IN_NEW
DEF_UNUSED_BUILTIN(BUILT_IN_NEW)
#endif
#ifdef BUILT_IN_VEC_NEW
DEF_UNUSED_BUILTIN(BUILT_IN_VEC_NEW)
#endif
#ifdef BUILT_IN_DELETE
DEF_UNUSED_BUILTIN(BUILT_IN_DELETE)
#endif
#ifdef BUILT_IN_VEC_DELETE
DEF_UNUSED_BUILTIN(BUILT_IN_VEC_DELETE)
#endif

/* Declare abort, exit, _exit and _Exit */
DEF_BUILTIN (BUILT_IN_ABORT,
	     "__builtin_abort",
	     NOT_BUILT_IN,
	     BT_FN_VOID,
	     BT_FN_VOID,
	     1, 0, 0,
	     ATTR_NORETURN_NOTHROW_LIST)

DEF_BUILTIN (BUILT_IN_EXIT,
	     "__builtin_exit",
	     NOT_BUILT_IN,
	     BT_FN_VOID_INT,
	     BT_FN_VOID_INT,
	     1, 0, 0,
	     ATTR_NORETURN_NOTHROW_LIST)

DEF_BUILTIN (BUILT_IN__EXIT,
	     "__builtin__exit",
	     NOT_BUILT_IN,
	     BT_FN_VOID_INT,
	     BT_FN_VOID_INT,
	     1, 0, 1,
	     ATTR_NORETURN_NOTHROW_LIST)

DEF_BUILTIN (BUILT_IN__EXIT2,
	     "__builtin__Exit",
	     NOT_BUILT_IN,
	     BT_FN_VOID_INT,
	     BT_FN_VOID_INT,
	     1, 0, !flag_isoc99,
	     ATTR_NORETURN_NOTHROW_LIST)

#if defined (TARG_ST)
#if defined (TARG_ST) && (GNU_FRONT_END==33)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ABSCH,
             "__builtin___absch",
             BT_FN_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ABSCL,
             "__builtin___abscl",
             BT_FN_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ABSCW,
             "__builtin___abscw",
             BT_FN_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ABSH,
             "__builtin___absh",
             BT_FN_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ABSL,
             "__builtin___absl",
             BT_FN_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ABSW,
             "__builtin___absw",
             BT_FN_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDCH,
             "__builtin___addch",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDCL,
             "__builtin___addcl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDCW,
             "__builtin___addcw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDD,
             "__builtin___addd",
             BT_FN_DOUBLE_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDL,
             "__builtin___addl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDS,
             "__builtin___adds",
             BT_FN_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ADDUL,
             "__builtin___addul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITCLRH,
             "__builtin___bitclrh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITCLRW,
             "__builtin___bitclrw",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITCNTH,
             "__builtin___bitcnth",
             BT_FN_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITCNTW,
             "__builtin___bitcntw",
             BT_FN_USHORT_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITNOTH,
             "__builtin___bitnoth",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITNOTW,
             "__builtin___bitnotw",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITREVW,
             "__builtin___bitrevw",
             BT_FN_UNSIGNED_USHORT_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITSETH,
             "__builtin___bitseth",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITSETW,
             "__builtin___bitsetw",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITVALH,
             "__builtin___bitvalh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BITVALW,
             "__builtin___bitvalw",
             BT_FN_USHORT_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__DIVUW,
             "__builtin__divuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__DIVW,
             "__builtin__divw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__MODUW,
             "__builtin__moduw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__MODW,
             "__builtin__modw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_CLAMPLW,
             "__builtin___clamplw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_CLAMPWH,
             "__builtin___clampwh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DISTH,
             "__builtin___disth",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DISTUH,
             "__builtin___distuh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DISTUW,
             "__builtin___distuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DISTW,
             "__builtin___distw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVD,
             "__builtin___divd",
             BT_FN_DOUBLE_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVFCH,
             "__builtin___divfch",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVFCM,
             "__builtin___divfcm",
             BT_FN_INT_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVFCW,
             "__builtin___divfcw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVH,
             "__builtin___divh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVL,
             "__builtin___divl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVM,
             "__builtin___divm",
             BT_FN_INT_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVS,
             "__builtin___divs",
             BT_FN_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVUH,
             "__builtin___divuh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVUL,
             "__builtin___divul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVUM,
             "__builtin___divum",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVUW,
             "__builtin___divuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DIVW,
             "__builtin___divw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DTOL,
             "__builtin___dtol",
             BT_FN_LONGLONG_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DTOS,
             "__builtin___dtos",
             BT_FN_FLOAT_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DTOUL,
             "__builtin___dtoul",
             BT_FN_ULONGLONG_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DTOUW,
             "__builtin___dtouw",
             BT_FN_UNSIGNED_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_DTOW,
             "__builtin___dtow",
             BT_FN_INT_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_EDGESH,
             "__builtin___edgesh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_EDGESW,
             "__builtin___edgesw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_EQD,
             "__builtin___eqd",
             BT_FN_INT_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_EQL,
             "__builtin___eql",
             BT_FN_INT_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_EQS,
             "__builtin___eqs",
             BT_FN_INT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_EQUL,
             "__builtin___equl",
             BT_FN_INT_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GED,
             "__builtin___ged",
             BT_FN_INT_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GEL,
             "__builtin___gel",
             BT_FN_INT_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GES,
             "__builtin___ges",
             BT_FN_INT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GETHH,
             "__builtin___gethh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GETHW,
             "__builtin___gethw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GETLH,
             "__builtin___getlh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GETLW,
             "__builtin___getlw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GEUL,
             "__builtin___geul",
             BT_FN_INT_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GTD,
             "__builtin___gtd",
             BT_FN_INT_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GTL,
             "__builtin___gtl",
             BT_FN_INT_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GTS,
             "__builtin___gts",
             BT_FN_INT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_GTUL,
             "__builtin___gtul",
             BT_FN_INT_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSEQUW,
             "__builtin___insequw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSEQW,
             "__builtin___inseqw",
             BT_FN_INT_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSGEUW,
             "__builtin___insgeuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSGEW,
             "__builtin___insgew",
             BT_FN_INT_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSGTUW,
             "__builtin___insgtuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSGTW,
             "__builtin___insgtw",
             BT_FN_INT_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSLEUW,
             "__builtin___insleuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSLEW,
             "__builtin___inslew",
             BT_FN_INT_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSLTUW,
             "__builtin___insltuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSLTW,
             "__builtin___insltw",
             BT_FN_INT_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSNEUW,
             "__builtin___insneuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_INSNEW,
             "__builtin___insnew",
             BT_FN_INT_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LED,
             "__builtin___led",
             BT_FN_INT_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LEL,
             "__builtin___lel",
             BT_FN_INT_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LES,
             "__builtin___les",
             BT_FN_INT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LEUL,
             "__builtin___leul",
             BT_FN_INT_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LTD,
             "__builtin___ltd",
             BT_FN_INT_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LTL,
             "__builtin___ltl",
             BT_FN_INT_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LTOD,
             "__builtin___ltod",
             BT_FN_DOUBLE_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LTOS,
             "__builtin___ltos",
             BT_FN_FLOAT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LTS,
             "__builtin___lts",
             BT_FN_INT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LTUL,
             "__builtin___ltul",
             BT_FN_INT_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LZCNTH,
             "__builtin___lzcnth",
             BT_FN_USHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LZCNTL,
             "__builtin___lzcntl",
             BT_FN_USHORT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_LZCNTW,
             "__builtin___lzcntw",
             BT_FN_USHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MADDS,
             "__builtin___madds",
             BT_FN_FLOAT_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAFCW,
             "__builtin___mafcw",
             BT_FN_INT_INT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXD,
             "__builtin___maxd",
             BT_FN_DOUBLE_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXH,
             "__builtin___maxh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXL,
             "__builtin___maxl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXS,
             "__builtin___maxs",
             BT_FN_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXUH,
             "__builtin___maxuh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXUL,
             "__builtin___maxul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXUW,
             "__builtin___maxuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MAXW,
             "__builtin___maxw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MIND,
             "__builtin___mind",
             BT_FN_DOUBLE_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINH,
             "__builtin___minh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINL,
             "__builtin___minl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINS,
             "__builtin___mins",
             BT_FN_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINUH,
             "__builtin___minuh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINUL,
             "__builtin___minul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINUW,
             "__builtin___minuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MINW,
             "__builtin___minw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODFCH,
             "__builtin___modfch",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODFCM,
             "__builtin___modfcm",
             BT_FN_INT_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODFCW,
             "__builtin___modfcw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODH,
             "__builtin___modh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODL,
             "__builtin___modl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODM,
             "__builtin___modm",
             BT_FN_INT_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODUH,
             "__builtin___moduh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODUL,
             "__builtin___modul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODUM,
             "__builtin___modum",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODUW,
             "__builtin___moduw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MODW,
             "__builtin___modw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MPFCW,
             "__builtin___mpfcw",
             BT_FN_INT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MPFCWL,
             "__builtin___mpfcwl",
             BT_FN_LONGLONG_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MPFML,
             "__builtin___mpfml",
             BT_FN_LONGLONG_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MPFRCH,
             "__builtin___mpfrch",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MPML,
             "__builtin___mpml",
             BT_FN_LONGLONG_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MPUML,
             "__builtin___mpuml",
             BT_FN_ULONGLONG_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MSUBS,
             "__builtin___msubs",
             BT_FN_FLOAT_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULD,
             "__builtin___muld",
             BT_FN_DOUBLE_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULFCH,
             "__builtin___mulfch",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULFCM,
             "__builtin___mulfcm",
             BT_FN_INT_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULFCW,
             "__builtin___mulfcw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULH,
             "__builtin___mulh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULHH,
             "__builtin___mulhh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULHUH,
             "__builtin___mulhuh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULHUW,
             "__builtin___mulhuw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULHW,
             "__builtin___mulhw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULL,
             "__builtin___mull",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULM,
             "__builtin___mulm",
             BT_FN_INT_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULN,
             "__builtin___muln",
             BT_FN_LONGLONG_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULS,
             "__builtin___muls",
             BT_FN_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULUH,
             "__builtin___muluh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULUL,
             "__builtin___mulul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULUM,
             "__builtin___mulum",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULUN,
             "__builtin___mulun",
             BT_FN_ULONGLONG_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULUW,
             "__builtin___muluw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_MULW,
             "__builtin___mulw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEARCLW,
             "__builtin___nearclw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEARCWH,
             "__builtin___nearcwh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEARLW,
             "__builtin___nearlw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEARWH,
             "__builtin___nearwh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NED,
             "__builtin___ned",
             BT_FN_INT_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEGCH,
             "__builtin___negch",
             BT_FN_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEGCL,
             "__builtin___negcl",
             BT_FN_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEGCW,
             "__builtin___negcw",
             BT_FN_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEGL,
             "__builtin___negl",
             BT_FN_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEGUL,
             "__builtin___negul",
             BT_FN_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEL,
             "__builtin___nel",
             BT_FN_INT_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NES,
             "__builtin___nes",
             BT_FN_INT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NEUL,
             "__builtin___neul",
             BT_FN_INT_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NORMH,
             "__builtin___normh",
             BT_FN_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NORML,
             "__builtin___norml",
             BT_FN_SHORT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NORMW,
             "__builtin___normw",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PRIORH,
             "__builtin___priorh",
             BT_FN_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PRIORL,
             "__builtin___priorl",
             BT_FN_SHORT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PRIORW,
             "__builtin___priorw",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PUTHL,
             "__builtin___puthl",
             BT_FN_LONGLONG_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PUTHW,
             "__builtin___puthw",
             BT_FN_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PUTLL,
             "__builtin___putll",
             BT_FN_LONGLONG_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_PUTLW,
             "__builtin___putlw",
             BT_FN_INT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ROTLH,
             "__builtin___rotlh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ROTLW,
             "__builtin___rotlw",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ROUNDCLW,
             "__builtin___roundclw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ROUNDCWH,
             "__builtin___roundcwh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ROUNDLW,
             "__builtin___roundlw",
             BT_FN_INT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ROUNDWH,
             "__builtin___roundwh",
             BT_FN_SHORT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHLCH,
             "__builtin___shlch",
             BT_FN_SHORT_SHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHLCW,
             "__builtin___shlcw",
             BT_FN_INT_INT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHLL,
             "__builtin___shll",
             BT_FN_LONGLONG_LONGLONG_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHLUL,
             "__builtin___shlul",
             BT_FN_ULONGLONG_ULONGLONG_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHLW,
             "__builtin___shlw",
             BT_FN_INT_INT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHRL,
             "__builtin___shrl",
             BT_FN_LONGLONG_LONGLONG_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHRRH,
             "__builtin___shrrh",
             BT_FN_SHORT_SHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHRRW,
             "__builtin___shrrw",
             BT_FN_INT_INT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHRUL,
             "__builtin___shrul",
             BT_FN_ULONGLONG_ULONGLONG_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHRUW,
             "__builtin___shruw",
             BT_FN_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SHRW,
             "__builtin___shrw",
             BT_FN_INT_INT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SQRTD,
             "__builtin___sqrtd",
             BT_FN_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SQRTS,
             "__builtin___sqrts",
             BT_FN_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_STOD,
             "__builtin___stod",
             BT_FN_DOUBLE_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_STOL,
             "__builtin___stol",
             BT_FN_LONGLONG_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_STOUL,
             "__builtin___stoul",
             BT_FN_ULONGLONG_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_STOUW,
             "__builtin___stouw",
             BT_FN_UNSIGNED_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_STOW,
             "__builtin___stow",
             BT_FN_INT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBCH,
             "__builtin___subch",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBCL,
             "__builtin___subcl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBCW,
             "__builtin___subcw",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBD,
             "__builtin___subd",
             BT_FN_DOUBLE_DOUBLE_DOUBLE,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBL,
             "__builtin___subl",
             BT_FN_LONGLONG_LONGLONG_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBS,
             "__builtin___subs",
             BT_FN_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SUBUL,
             "__builtin___subul",
             BT_FN_ULONGLONG_ULONGLONG_ULONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SWAPBH,
             "__builtin___swapbh",
             BT_FN_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SWAPBW,
             "__builtin___swapbw",
             BT_FN_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SWAPHW,
             "__builtin___swaphw",
             BT_FN_UNSIGNED_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ULTOD,
             "__builtin___ultod",
             BT_FN_DOUBLE_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ULTOS,
             "__builtin___ultos",
             BT_FN_FLOAT_LONGLONG,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_UWTOD,
             "__builtin___uwtod",
             BT_FN_DOUBLE_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_UWTOS,
             "__builtin___uwtos",
             BT_FN_FLOAT_UNSIGNED,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_WTOD,
             "__builtin___wtod",
             BT_FN_DOUBLE_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_WTOS,
             "__builtin___wtos",
             BT_FN_FLOAT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_XSHLH,
             "__builtin___xshlh",
             BT_FN_USHORT_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_XSHLW,
             "__builtin___xshlw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_XSHRH,
             "__builtin___xshrh",
             BT_FN_USHORT_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_XSHRW,
             "__builtin___xshrw",
             BT_FN_UNSIGNED_UNSIGNED_UNSIGNED_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NMADDS,
             "__builtin___nmadds",
             BT_FN_FLOAT_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_NMSUBS,
             "__builtin___nmsubs",
             BT_FN_FLOAT_FLOAT_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_SQUARES,
             "__builtin___squares",
             BT_FN_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_RECIPS,
             "__builtin___recips",
             BT_FN_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_RSQRTS,
             "__builtin___rsqrts",
             BT_FN_FLOAT_FLOAT,
             ATTR_PURE_NOTHROW_LIST)

DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_0,
             "__builtin___asm_0",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_1,
             "__builtin___asm_1",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_2,
             "__builtin___asm_2",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_3,
             "__builtin___asm_3",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_4,
             "__builtin___asm_4",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_5,
             "__builtin___asm_5",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_6,
             "__builtin___asm_6",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_7,
             "__builtin___asm_7",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_8,
             "__builtin___asm_8",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_9,
             "__builtin___asm_9",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_10,
             "__builtin___asm_10",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_11,
             "__builtin___asm_11",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_12,
             "__builtin___asm_12",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_13,
             "__builtin___asm_13",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_14,
             "__builtin___asm_14",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_ASM_15,
             "__builtin___asm_15",
             BT_FN_INT_INT_INT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__DIVH,
             "__builtin__divh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__DIVUH,
             "__builtin__divuh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__MODH,
             "__builtin__modh",
             BT_FN_SHORT_SHORT_SHORT,
             ATTR_PURE_NOTHROW_LIST)
DEF_EXT_LIB_BUILTIN (BUILT_IN_BUILTIN__MODUH,
             "__builtin__moduh",
             BT_FN_USHORT_USHORT_USHORT,
             ATTR_PURE_NOTHROW_LIST)
#define INTRN_GFECC_BUILTINS_DEF
#ifdef INTRN_EXPAND

/* Check_Opd_Enum
 *   Generates the proper enum tn.
 */
static inline TN*
Check_Opd_Enum(TN *opd, INT enum_base, SRCPOS srcpos) {
  if (TN_is_rematerializable(opd))  {
    WN *wn = TN_remat(opd);
    opd = Gen_Enum_TN(WN_const_val(wn)+enum_base);
  } else {
    opd = Gen_Enum_TN(TN_value(opd)+enum_base);
  }
  return opd;
}

/* Check_Opd_RClass
 *   Expand a copy if source tn is not of given register class.
 */
static inline TN*
Check_Opd_RClass(TN *opd, ISA_REGISTER_CLASS rc, OPS *ops) {
  if(TN_is_register(opd) && TN_register_class(opd) != rc) {
    TN *tn_rc = Build_RCLASS_TN(rc);
    Expand_Copy(tn_rc,True_TN,opd,ops);
    return tn_rc;
  }
  else {
    return opd;
  }
}

/* Check_Opd_Literal
 *  Check the availability of TN value and if it fit in
 *  corresponding literal class range.
 *  Return literal TN.
 */
static inline TN*
Check_Opd_Literal( TN *tn_opnd, ISA_LIT_CLASS lc, INT32 arg_idx, const char *intrn_name, SRCPOS srcpos  ) {
  char error_msg[256];

  TN *current_value = Gen_Literal_TN(-1,4);

  if (TN_is_rematerializable(tn_opnd)) {
    WN *wn = TN_home(tn_opnd);
    if (WN_operator_is(wn, OPR_INTCONST)) {
      TN *cunknown = Gen_Literal_TN(WN_const_val(wn), 4);
      current_value = cunknown;
    } else {
      DevWarn("%s::Expand_Extension_intrinsic:"
	" TN_is_rematerializable BUT WN_operator_is *not* OPR_INTCONST.", __FILE__);
    }
  }
  else if (TN_is_zero(tn_opnd)) {
    current_value = Gen_Literal_TN(0,4);
  }
  else if (TN_has_value(tn_opnd)) {
   current_value = tn_opnd;
  }
  else {
    sprintf(error_msg, "Immediate value expected for arg %d of intrinsic call '%s'",
                       arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }

  // Check immediate value is in range
  if (!ISA_LC_Value_In_Class(TN_value(current_value), lc)) {
    sprintf(error_msg, "Immediate value '%lld' of arg %d out of bounds"
          " for intrinsic call '%s'.",
          TN_value(current_value), arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }
  return current_value;
}

/* Check_Opd_Literal_For_Dedicated
 *  Check the availability of TN value and if it fit in
 *  given register idx range [0..reg_file_size[.
 *  Return literal TN.
 */
static inline TN*
Check_Opd_Literal_For_Dedicated( TN *tn_opnd, INT32 reg_file_size, INT32 arg_idx, const char *intrn_name, SRCPOS srcpos  ) {
  char error_msg[256];

  TN *current_value = Gen_Literal_TN(-1,4);

  if (TN_is_rematerializable(tn_opnd)) {
    WN *wn = TN_home(tn_opnd);
    if (WN_operator_is(wn, OPR_INTCONST)) {
      INT64 value = WN_const_val(wn);
        current_value = Gen_Literal_TN(WN_const_val(wn), 4);
    } else {
      DevWarn("%s::Expand_Extension_intrinsic:"
	" TN_is_rematerializable BUT WN_operator_is *not* OPR_INTCONST.", __FILE__);
    }
  }
  else if (TN_is_zero(tn_opnd)) {
    current_value = Gen_Literal_TN(0,4);
  }
  else if (TN_has_value(tn_opnd)) {
   current_value = tn_opnd;
  }
  else {
    sprintf(error_msg, "Immediate value expected for arg %d of intrinsic call '%s'",
                       arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }

  // Check immediate value is in range
  if ( ! (0 <= TN_value(current_value) && TN_value(current_value) < reg_file_size) ) {
    sprintf(error_msg, "Immediate value '%lld' of arg %d out of bounds"
          " or wrong alignment for intrinsic call '%s'.",
          TN_value(current_value), arg_idx, intrn_name);
    if (SRCPOS_linenum(srcpos)>0) {
      ErrMsgLine(EC_CG_Generic_Fatal, SRCPOS_linenum(srcpos), error_msg);
    }
    else {
      ErrMsg(EC_CG_Generic_Fatal, error_msg);
    }
  }
  return current_value;
}

#endif /* INTRN_EXPAND */

#ifdef INTRN_SWITCH

#endif /* INTRN_SWITCH */

#ifdef INTRN_DEFINES

INTRINSIC_TARG_LAST	=	INTRINSIC_GENERIC_LAST+0,
#endif /* INTRN_DEFINES */

#ifdef INTRN_INFO

#endif /* INTRN_INFO */

#ifdef INTRN_WUTIL

#endif /* INTRN_WUTIL */

#ifdef INTRN_GFEC_WFE_EXPR

#endif /* INTRN_GFEC_WFE_EXPR */

#ifdef INTRN_GFECC_BUILTINS_DEF

#endif /* INTRN_GFECC_BUILTINS_DEF */

#ifdef INTRN_GFEC_BUILTINS_DEF

#endif /* INTRN_GFEC_BUILTINS_DEF */

#ifdef INTRN_GFEC_BUILTINS_H

#endif /* INTRN_GFEC_BUILTINS_H */

#undef INTRN_GFECC_BUILTINS_DEF
#endif /*defined(TARG_ST)*/
#endif


  /* Upper bound on non-language-specific builtins.  */
  END_BUILTINS
};
// BUILT_IN_STATIC_COUNT is the count of staticaly defined builtins
#define BUILT_IN_STATIC_COUNT END_BUILTINS
// BUILT_IN_STATIC_LAST is the last builtin staticaly known
#define BUILT_IN_STATIC_LAST (END_BUILTINS-1)
extern enum built_in_function BUILT_IN_COUNT; 

#undef DEF_BUILTIN

#endif


#endif /* EXTENSION_INCLUDE_H */
