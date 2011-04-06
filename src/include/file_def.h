#ifndef FILE_DEF_H
#define FILE_DEF_H

#ifdef _WIN32
#define DIR_SLASH '\\'
#define DIR_SLASH_STR "\\"

#define IS_DIR_SLASH(c) ((c) == '\\' || (c) == '/')
#define IS_ABSOLUTE_PATH(name)  ((name)[0] != '\0' && (name)[1] == ':' && \
	                             (IS_DIR_SLASH((name)[2])))

#else

#define DIR_SLASH '/'
#define DIR_SLASH_STR "/"

#define IS_DIR_SLASH(c) ((c) == '/')
#define IS_ABSOLUTE_PATH(name)  (IS_DIR_SLASH((name)[0]))
#endif

#ifdef _WIN32
#define EXT_DSO    ".dll"
#define EXT_EXE    ".exe"
#define EXT_OBJ    ".o"
#define EXT_LIB    ".lib"
#else
#define EXT_DSO    ".so"
#define EXT_EXE    ""
#define EXT_OBJ    ".o"
#define EXT_LIB    ".a"
#endif

#define FN_DSO(n)  n EXT_DSO
#define FN_EXE(n)  n EXT_EXE
#define FN_OBJ(n)  n EXT_OBJ
#define FN_LIB(n)  n EXT_LIB

#endif
