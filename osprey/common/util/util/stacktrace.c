
/*
  Implementation of stack tracing facility which:
  1) unwinds the stack 
  2) parses the debug symbol table for the process and all its
  libraries
  3) prints library, function name, source file name and src file line no. for
  each entry in the call stack
  Handles elf System V  executable formats
  Relies on stabs informations.
*/

#if defined(TARG_ST) && defined(STACK_TRACE)

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define OMIT_STACKTRACE

#ifdef __SVR4
#undef OMIT_STACKTRACE

/* Arch is SPARC */
#define ARCH_IS_SPARC 1

/* Do we have snprintf() funtion. */
#define HAVE_SNPRINTF	1

/* found Dl_info in dlfcn.h */
#define HAVE_DL_INFO 1

/* A string that contains the shared library extention (e.g. .so or .sl) */
#define SHLIB_EXT ".so"

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <libgen.h> header file.  */
#define HAVE_LIBGEN_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <sys/frame.h> header file.  */
#define HAVE_SYS_FRAME_H 1

/* Define if you have the <sys/link.h> header file.  */
#define HAVE_SYS_LINK_H 1

/* Define if you have the <link.h> header file.  */
/*#define HAVE_LINK_H 1*/

/* Define if you have the <sys/reg.h> header file.  */
#define HAVE_SYS_REG_H 1

/* Define if you have the <sys/regset.h> header file.  */
#define HAVE_SYS_REGSET_H 1

/* Define if you have the <ucontext.h> header file.  */
#define HAVE_UCONTEXT_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the demangle library (-ldemangle).  */
/*#define HAVE_LIBDEMANGLE 1*/

/* Define if you have definition for dladdr.  */
#define HAVE_DLADDR 1

/* found dladdr declaration in dlfcn.h */
#define HAVE_DLADDR_DECL 1

/* Define if you have the dl library (-ldl).  */
#define HAVE_LIBDL 1

/* Define if you have the elf library (-lelf).  */
#define HAVE_LIBELF 1

/* Define if you have the iberty library (-liberty).  */
#define HAVE_LIBIBERTY

/* Define to use the local elfr ELF reader.  */
/*#define HAVE_ELFR_H*/

/* Define to use DT_DEBUG entry to find library bases. */
/*#define USE_DT_DEBUG 1*/

#endif

#ifdef __linux
#undef OMIT_STACKTRACE

/* Arch is X86 */
#define ARCH_IS_X86 1

/* Do we have snprintf() funtion. */
/* On linux (Red Hat 7.3) snprintf is flawed. Use own version. */
/*#define HAVE_SNPRINTF	1*/

/* found Dl_info in dlfcn.h */
/*#define HAVE_DL_INFO 1*/

/* A string that contains the shared library extention (e.g. .so or .sl) */
#define SHLIB_EXT ".so"

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <libgen.h> header file.  */
#define HAVE_LIBGEN_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <sys/frame.h> header file.  */
/*#define HAVE_SYS_FRAME_H 1*/

/* Define if you have the <sys/link.h> header file.  */
/*#define HAVE_SYS_LINK_H 1*/

/* Define if you have the <link.h> header file.  */
#define HAVE_LINK_H 1

/* Define if you have the <sys/reg.h> header file.  */
/*#define HAVE_SYS_REG_H 1*/

/* Define if you have the <sys/regset.h> header file.  */
/*#define HAVE_SYS_REGSET_H 1*/

/* Define if you have the <ucontext.h> header file.  */
#define HAVE_UCONTEXT_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the demangle library (-ldemangle).  */
/*#define HAVE_LIBDEMANGLE 1*/

/* Define if you have definition for dladdr.  */
#define HAVE_DLADDR 1

/* found dladdr declaration in dlfcn.h */
/*#define HAVE_DLADDR_DECL 1*/

/* Define if you have the dl library (-ldl).  */
#define HAVE_LIBDL 1

/* Define if you have the elf library (-lelf).  */
/*#define HAVE_LIBELF 1*/

/* Define if you have the iberty library (-liberty).  */
#define HAVE_LIBIBERTY

/* Define to use the local elfr ELF reader.  */
#define HAVE_ELFR_H

/* Define to use DT_DEBUG entry to find library bases. */
/*#define USE_DT_DEBUG 1*/

#endif

#ifdef OMIT_STACKTRACE

const char * C_StackTrace(int pathInTrace,
			  int printLongStack,
			  int  numLibs,
			  const char **libsPP,
			  int debug)
{
  return "Stack Trace not yet implemented for this platform.\n";
}

const char * C_ContextStackTrace(void *uap,
				 int pathInTrace,
				 int printLongStack,
				 int  numLibs,
				 const char **libsPP,
				 int debug)
{
  return "Context Stack Trace not yet implemented for this platform.\n";
}


#else


/*
 * snprintf
 * Define own version if not present.
 */
#ifdef HAVE_SNPRINTF 
#include <stdio.h>
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#else
#include <stdarg.h>
static int 
own_vsnprintf(char *buffer, int max, const char *fmt, va_list args)
{
  int n;
  static char large_buffer[8096];
  n = vsprintf(buffer, fmt, args);
  assert (n < sizeof(large_buffer));
  return n;
}
static int 
own_snprintf(char *buffer, int max, const char *fmt, ...)
{
  int n;
  va_list args;
  va_start(args, fmt);
  
  n = own_vsnprintf(buffer, max, fmt, args);
  va_end(args);
  return n;
}
#define SNPRINTF own_snprintf
#define VSNPRINTF own_vsnprintf
/* Force error if using directly snprintf. */
#define snprintf undefined_snprintf
#define vsnprintf undefined_vsnprintf
#endif

/*
 * array and hash container support.
 */
#include "hash.h"
#include "array.h"

#define NEW_ARRAY(type, size) (type *)malloc(sizeof(type)*(size))

#if defined(HAVE_LIBDEMANGLE)
#include <demangle.h>
#elif defined(HAVE_LIBIBERTY)
extern char * cplus_demangle(const char *, int);
#endif

typedef unsigned long AddressT;

static int debugFlag;

typedef struct {
  const char* srcPathNameP;
  const char* srcFileNameP;
  const char* funcMangledNameP;
  const char* funcDemangledNameP;
  int lineNum;
} sourceInfoT;

static void findFileAndLineNo(void* pc, const char* lib, sourceInfoT *info);

static void printFunction(void *pc,
			  long *args, 
			  char *messageString,
			  int messageLen,
			  int *state,
			  const int printLongStack,
			  const int  numLibs,
			  const char **libsPP,
			  const int pathInTrace);


/* Message buffer */
#define MESSAGE_LEN 4096
static char messageString[MESSAGE_LEN];

typedef hash_t hash_ptr;

static hash_ptr hash_ptr_new(int hint)
{
  return hash_new(hint, sizeof(void *));
}

static hash_ptr hash_ptr_del(hash_ptr hash)
{
  return hash_del(hash);
}

static void hash_ptr_insert(hash_ptr hash, void *key, void *value)
{
  void *key_dup = malloc(sizeof(void *));
  hash_bucket_t bucket = hash_insert_bucket(hash, (const char *)memcpy(key_dup, (void *)&key, sizeof(void *)), sizeof(void *));
  void *data = hash_bucket_data(bucket);
  *((void **)data) = value;
}

static void *hash_ptr_get(hash_ptr hash, void *key)
{
  hash_bucket_t bucket = hash_access_bucket(hash, (const char *)&key, sizeof(void *));
  void *data;
  if (bucket == NULL) return NULL;
  data = hash_bucket_data(bucket);
  return *((void **)data);
}


typedef hash_t hash_str;

static hash_str hash_str_new(void)
{
  return hash_new(512, sizeof(void *));
}

static hash_str hash_str_del(hash_str hash)
{
  return hash_del(hash);
}

static void hash_str_insert(hash_str hash, const char *key, void *value)
{
  const char *key_dup = strdup(key);
  hash_bucket_t bucket = hash_insert_bucket(hash, key_dup, strlen(key_dup));
  void *data = hash_bucket_data(bucket);
  *((void **)data) = value;
}

static void *hash_str_get(hash_ptr hash, const char *key)
{
  hash_bucket_t bucket = hash_access_bucket(hash, key, strlen(key));
  void *data;
  if (bucket == NULL) return NULL;
  data = hash_bucket_data(bucket);
  return *((void **)data);
}

typedef struct PCMapS {
  hash_ptr hash;
  array_t sorted;
} PCMapS;
    
typedef PCMapS *PCMapT;
typedef struct pair_ptr
{
  void *ptr1;
  void *ptr2;
} pair_ptr;

#ifdef __GNUC__
#define PCMapT_new() ({ PCMapT map = (PCMapT)malloc(sizeof(PCMapS)); map->hash = hash_ptr_new(131072); map->sorted = NULL; map;})
#define PCMapT_del(x) ({ if (x->hash != NULL) hash_ptr_del(x->hash); if (x->sorted) array_del(x->sorted); free(x); (PCMapT)NULL;})
#endif

static void PCMapT_insert(PCMapT x, void *k, void *v) 
{
  hash_ptr_insert(x->hash, k, v); 
}

static int compare_pair_ptr(const pair_ptr *pair1, const pair_ptr *pair2)
{
  if (pair1->ptr1 < pair2->ptr1) return -1;
  if (pair1->ptr1 > pair2->ptr1) return 1;
  return 0;
}

static void PCMapT_sort(PCMapT map)
{
  int count = hash_count(map->hash);
  array_t array = array_new(count, sizeof(pair_ptr));
  hash_iterator_t iterator;

  for(iterator = hash_iterator_new(map->hash); 
      !hash_iterator_at_end(iterator); 
      hash_iterator_advance(iterator)) {
    hash_bucket_t bucket = hash_iterator_current(iterator);
    pair_ptr *pair = (pair_ptr *)array_append(array);
    pair->ptr1 = *(void **)hash_bucket_key(bucket);
    pair->ptr2 = *(void **)hash_bucket_data(bucket);
  }
  hash_iterator_del(iterator);
  hash_ptr_del(map->hash);
  map->hash = NULL;
  array_qsort(array, (int (*)(const void *, const void *))compare_pair_ptr);
  map->sorted = array;
}

typedef hash_str exeFileMapT;
#define exeFileMapT_new() hash_str_new()
#define exeFileMapT_del(x) hash_str_del(x)
#define exeFileMapT_insert(x, k, v) hash_str_insert(x, k, v)
#define exeFileMapT_get(x, k) hash_str_get(x, k)

static void *xfree(void *ptr)
{
  if (ptr != NULL) free(ptr);
  return NULL;
}

static PCMapT buildPCMap(const char * exeFileName);

void dumpExeFileMap(exeFileMapT exeFileMap)
{
  fprintf(stderr, "dump of exe file map: not implemented\n");
}

void dumpPCMap(PCMapT PCMapP)
{
  int i;
  for (i = array_first(PCMapP->sorted); i <= array_last(PCMapP->sorted); i++) {
    pair_ptr *pair;
    pair = (pair_ptr *)array_get_at(PCMapP->sorted, i);
    if (pair == NULL) { fprintf(stderr, "ERROR: null pair at %d\n", i); continue; }
    else {
      void *pc = pair->ptr1;
      sourceInfoT *infoP = (sourceInfoT *)pair->ptr2;
      if (infoP) {
	const char *srcPath = infoP->srcPathNameP ? infoP->srcPathNameP: "";
	const char* srcFile = infoP->srcFileNameP ? infoP->srcFileNameP: "";
	const char* funcName = infoP->funcDemangledNameP ? infoP->funcDemangledNameP: 
	  infoP->funcMangledNameP ? infoP-> funcMangledNameP: "<unknown>";
	fprintf(stderr, "at 0x%lx file %s%s, function %s\n", (AddressT)pc, srcPath, srcFile, funcName);
      }
    }
  }
}

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#else
#error "requires dflcn.h"
#endif

#ifndef HAVE_DLADDR
#error "requires dladdr()"
#endif

#ifndef HAVE_DL_INFO
typedef struct {
  const char *    dli_fname;      /* file containing address range */
  void *          dli_fbase;      /* base address of file image */
  const char *    dli_sname;      /* symbol name */
  void *          dli_saddr;      /* symbol address */
} Dl_info;
#endif
#ifndef HAVE_DLADDR_DECL
extern int dladdr(const void *pc, Dl_info *dlInfo);
#endif


static int 
dladdrWrapper(void * pc, Dl_info *passedInfo)
{
  char *funcNameFromDladdrP;
  char *fileNameFromDladdrP;
  static Dl_info localInfo;
  if (!dladdr(pc, &localInfo)) {
    return 0;
  }
  if (localInfo.dli_fname) {
    /* never deleted */
    fileNameFromDladdrP = NEW_ARRAY(char,strlen(localInfo.dli_fname) + 1);
    strcpy(fileNameFromDladdrP, localInfo.dli_fname);
  } else {
    fileNameFromDladdrP = NULL;
  }
  passedInfo->dli_fname = fileNameFromDladdrP;
  
  if (localInfo.dli_sname) {
    /* never deleted */
    funcNameFromDladdrP = NEW_ARRAY(char,strlen(localInfo.dli_sname) + 1);
    strcpy(funcNameFromDladdrP, localInfo.dli_sname);
  } else {
    funcNameFromDladdrP = NULL;
  }
  passedInfo->dli_sname = funcNameFromDladdrP;
  passedInfo->dli_fbase = localInfo.dli_fbase;
  passedInfo->dli_saddr = localInfo.dli_saddr;
  return 1;
}

static char *oldLibraryP;

static void printFunctionInit(void) 
{
  oldLibraryP = NULL;
}
static void printFunction(void *pc,
			  long *args,
			  char *messageString,
			  int messageLen,
			  int *state,
			  const int printLongStack,
			  const int  numLibs,
			  const char **libsPP,
			  const int pathInTrace)
{
  static Dl_info dladdrInfo;
  char *libraryP = 0;
  char *fullyDemangledFuncNameP = 0;
  int doPrint;
  int inTBCode;
  int i;

  if (debugFlag) 
    fprintf(stderr, "PrintFunction(pc:%p, args:%p, state:%d)\n", pc, args, *state);

  /* Use "dladdr()" to search the dynamic loader symbol table. */
  if(dladdrWrapper(pc, &dladdrInfo) == 0) {
    return;  
  }

  if (!dladdrInfo.dli_fname) {
    return;
  }

  /*
    Unless printLongStack is true, turn off the trace for libs
    specified in libsPP.
  */
  inTBCode = 0;
  for (i = 0; i < numLibs; i++) {
    if (strstr(dladdrInfo.dli_fname, libsPP[i])) {
      inTBCode = 1;
      break;
    }
  }
  /* libraryP needs to be persistent since it is the index in exeFileMap. */
  if (pathInTrace) {
    libraryP = NEW_ARRAY(char,strlen(dladdrInfo.dli_fname) + 1);
    strcpy(libraryP, dladdrInfo.dli_fname);
  }
  else {
    for (i = strlen(dladdrInfo.dli_fname) - 1; i >= 0; i--)
    {
      if (dladdrInfo.dli_fname[i] == '/')
      {
	libraryP = NEW_ARRAY(char, strlen(&dladdrInfo.dli_fname[i+1]) + 1);
	strcpy(libraryP, &dladdrInfo.dli_fname[i+1]);
	break;
      }
    }
  }
  switch(*state) {
    case 0:
      if (inTBCode) *state = 1;
      else *state = 2;
      break;
    case 1:
      if (!inTBCode) *state = 2;
      break;
    case 2:
      if (inTBCode) *state = 3;
      break;
    default:
      break;
  }

  doPrint = printLongStack || (*state == 2);

  if (doPrint) {
    sourceInfoT PCMapInfo;
    findFileAndLineNo(pc, dladdrInfo.dli_fname, &PCMapInfo);

    if (debugFlag && PCMapInfo.funcMangledNameP)
      fprintf(stderr, "Found symbol: %s\n", PCMapInfo.funcMangledNameP);
    if (debugFlag && PCMapInfo.funcDemangledNameP)
      fprintf(stderr, "demangled symbol: %s\n", PCMapInfo.funcDemangledNameP);

#ifdef HAVE_LIBIBERTY
    if (PCMapInfo.funcMangledNameP) fullyDemangledFuncNameP = cplus_demangle(PCMapInfo.funcMangledNameP, 3);
    if (debugFlag && fullyDemangledFuncNameP)
      fprintf(stderr, "Demangled name: %s\n", fullyDemangledFuncNameP);
#elif defined(HAVE_LIBDEMANGLE)
    if (PCMapInfo.funcMangledNameP) {
      const int NAME_SIZE = 1000;

      /* use malloc to be consistent with libiberty's cplus_demangle() */
      fullyDemangledFuncNameP = (char *) malloc(NAME_SIZE);  
      if (cplus_demangle(PCMapInfo.funcMangledNameP, fullyDemangledFuncNameP, NAME_SIZE)) {
	xfree(fullyDemangledFuncNameP);
	fullyDemangledFuncNameP = 0;
      }
    }
#endif
    if (oldLibraryP == NULL || strcmp(oldLibraryP, libraryP) != 0) {
      SNPRINTF(messageString, messageLen, "%sfile %s\n", messageString, libraryP);
    }
    SNPRINTF(messageString, messageLen, "%s  -- ", messageString);
    
    if (fullyDemangledFuncNameP == 0) {
      /* We don't have a a fully demangled name, probably because libiberty.a
	 is not working or name is a C name. */
      if (PCMapInfo.funcDemangledNameP) {
	SNPRINTF(messageString, messageLen, "%s in %s()", messageString, PCMapInfo.funcDemangledNameP);
      }
      else if (PCMapInfo.funcMangledNameP) {
	SNPRINTF(messageString, messageLen, "%s in %s", messageString, PCMapInfo.funcMangledNameP);
      }
      else {
	/* No name was found in the PCMap */
	SNPRINTF(messageString, messageLen, "%s at %s 0x%lx + 0x%lx", messageString, 
		 dladdrInfo.dli_sname == NULL ? "<unknown>" : dladdrInfo.dli_sname, 
		 (AddressT) dladdrInfo.dli_saddr, (AddressT) pc - (AddressT) dladdrInfo.dli_saddr);
      }
    }
    else {
      SNPRINTF(messageString, messageLen, "%s in %s", messageString, fullyDemangledFuncNameP);
      xfree(fullyDemangledFuncNameP); /* was malloc'ed by cplus_demangle()*/
    }

    /*
      srcPathNameP could be empty and the path could be in srcFileNameP.
      Sometimes the stabs have the full path in the SO type string.
      It's simpler to handle that here rather than in findFileAndLineNo()
      where copies of strings would have to be made to tease out the path from
      the file name.
    */
    if (pathInTrace)
    {
      if (PCMapInfo.srcPathNameP) {
	SNPRINTF(messageString, messageLen, "%s at %s%s", messageString, 
		 PCMapInfo.srcPathNameP, PCMapInfo.srcFileNameP ? PCMapInfo.srcFileNameP: "");
      }
    }
    else
    {
      if (PCMapInfo.srcFileNameP) {
	for (i = strlen(PCMapInfo.srcFileNameP) - 1; i >= 0; i--) {
	  if (PCMapInfo.srcFileNameP[i] == '/')
	  {
	    PCMapInfo.srcFileNameP = &PCMapInfo.srcFileNameP[i+1];
	    break;
	  }
	}
	SNPRINTF(messageString, messageLen, "%s at %s", messageString, PCMapInfo.srcFileNameP);
      }
    }
    if (PCMapInfo.lineNum) {
      SNPRINTF(messageString, messageLen, "%s line %d", messageString, PCMapInfo.lineNum);
    } else {
      SNPRINTF(messageString, messageLen, "%s (pc = 0x%lx)", messageString, (AddressT) pc);
    }
    SNPRINTF(messageString, messageLen, "%s\n", messageString);
    messageString[messageLen-1] = '\0';
  } /* if (doPrint) */
  else {
    SNPRINTF(messageString, messageLen, "%s skipped\n", messageString);
  }
  oldLibraryP = libraryP;
  return;
}

void findFileAndLineNo(void* pc,
		       const char*  exeFileNameP,
		       sourceInfoT *outputInfo)
{
  static sourceInfoT notFoundInfo = {0,0,0,0,0};
  static PCMapT     notFoundPCMapP;
  static exeFileMapT exeFileMap;
  PCMapT PCMapP;

  if (debugFlag) fprintf(stderr, "findFileAndLineNo(%p, %s)\n",
			 pc, exeFileNameP);

  if (exeFileMap == NULL) exeFileMap = exeFileMapT_new();
  /*
    Look up the executable file name in a map that contains a pointer to another map that,
    based on a PC, returns source file info and a line number.
  */
  PCMapP = (PCMapT)exeFileMapT_get(exeFileMap, exeFileNameP);
  
  /*
    exeFileMap[exeFileName] returns 0 if there was no match and creates an entry with a
    value of 0.
  */
  if (!PCMapP)
  {
    /*
      No PCMap found for this executable file.  Create one and then get what was created.
    */
    PCMapP = buildPCMap(exeFileNameP);  // returns NULL if an error occurs
    if (!PCMapP)
    {
      /*
	Trouble.
	Don't keep trying to build a map for this file name if it occurs again.  Use empty PCMap.
      */
      if (!notFoundPCMapP)
      {
	notFoundPCMapP = PCMapT_new();
	PCMapT_insert(notFoundPCMapP, 0, &notFoundInfo);
	PCMapT_sort(notFoundPCMapP);
	exeFileMapT_insert(exeFileMap, exeFileNameP, notFoundPCMapP);
      }
      else
      {
	exeFileMapT_insert(exeFileMap, exeFileNameP, notFoundPCMapP);
      }
      *outputInfo = notFoundInfo;
      return;
    }
    else
    {
      exeFileMapT_insert(exeFileMap, exeFileNameP, PCMapP);
    }
    if (debugFlag >= 2) dumpPCMap(PCMapP);
  } /* if (!PCMapP) */
  


  {
    sourceInfoT *infoP;
    int f, l, m , i;
    pair_ptr *last_pair;
    last_pair = NULL;
    /* dichotomic search in sorted stabs list. */
    f = 1;
    l = array_last(PCMapP->sorted);
    i = 0;
    while (f <= l) {
      pair_ptr *pair;
      i++;
      m = (l+f)>>1;
      pair = (pair_ptr *)array_get_at(PCMapP->sorted, m);
      if (pair->ptr1 > pc) { l = m-1; }
      else if (pair->ptr1 <= pc) { last_pair = pair; f = m+1; }
    }

    if (last_pair == NULL) {
      /* The address could be too big or too small id the PCMap is
	 incomplete. The file was stripped or no compiled with -g. 
      */
      *outputInfo = notFoundInfo;
      if (debugFlag) fprintf(stderr, "NOT FOUND\n");
    } else if (last_pair->ptr2) {
      infoP = (sourceInfoT *)last_pair->ptr2;
      *outputInfo = *infoP;
    } else {
      *outputInfo = notFoundInfo;
      if (debugFlag) fprintf(stderr, "NOT FOUND\n");
    }
  }
  if (debugFlag)
    fprintf(stderr, "END findFileAndLineNo\n");
  return;
}

/*
  This structure and the following defines are used on Solaris and Linux
*/
struct stab {
  unsigned	n_strx;		/* index into file string table */
  unsigned char n_type;		/* type flag (N_TEXT,..)  */
  char		n_other;	/* used by N_SLINE stab */
  short		n_desc;		/* see stabs documentation */
  unsigned	n_value;	/* value of symbol (or sdb offset) */ 
};

#define N_UNDF 0
#define N_BINCL 0x82
#define N_EINCL 0xA2
#define N_FUN   0x24
#define N_SLINE 0x44
#define N_SO    0x64
#define N_SOL   0x84

static PCMapT processStabs(const struct stab *stabP, 
			   int stabSize,
			   const char *stringPoolP,
			   void *dynLibHandle,
			   AddressT exeBase,
			   int typeIsExe)
{
  int stringPoolOffset = 0;
  int numStabs = stabP->n_desc;
  int fileCount = -1;
  int pathCount = -1;
  int stabCount = 0;
  //
  // This array of pointers is used for the nested filenames used by the BINCL/EINCL algorithm
  // 100 should be enough for file nesting depth
  //
  const char *fileStackP[100];
  const char *pathStackP[100];
  static char emptyStringP[] = "";
  char *curFuncNameP = 0;
  int binclAlgo = 0;
  void *funcBaseP = 0;
  AddressT funcEnd = 0;
  PCMapT PCMapP;
  int i;

  PCMapP = PCMapT_new();
  //
  // Mark the beginning of the array with a dummy entry;
  //
  PCMapT_insert(PCMapP, 0, 0);

  for (i = 0; (unsigned int)i < (stabSize/(sizeof (struct stab))) - 1; i++)
  {
    const char *string = stringPoolP + stabP->n_strx;
    stabCount++;
    switch(stabP->n_type)
    {
      case N_UNDF:
#if !defined(HAVE_LIBDEMANGLE)
	if ((stabCount != 1) && ((stabCount -1) != numStabs))
	{
	  PCMapT_del(PCMapP);
	  return (PCMapT) NULL;
	}
#endif
	pathCount = -1;
	fileCount = -1;
	numStabs         = stabP->n_desc;
	stabCount        = 0;
	stringPoolP      = stringPoolP + stringPoolOffset;
	stringPoolOffset = stabP->n_value;
	break;
      case N_BINCL:
	binclAlgo = 1;
	//
	// The string for this stabs type usually includes the file name and the file path, e.g.:
	// /hm/jlp/tb/sand_tmp_sun/TestBuilder/include/TestBuilder.h
	// I think when it does not the path is just the current working directory, so no
	// path is necessary.
	//
	pathStackP[++pathCount] = emptyStringP;
	fileStackP[++fileCount] = string;
	//
	// This is similar to what is done in printFunction() but in that
	// case we sometimes use the file path, whereas here we never do.
	//
	{ 
	  int i;
	  for (i = strlen(string) - 1; i >= 0; i--)
	    {
	      if (string[i] == '/') 
		{
		  fileStackP[fileCount] = &string[i+1];
		  break;
		}
	    }
	}
	break;
      case N_EINCL:
	pathCount--;
	fileCount--;
	break;
      case N_FUN:
	if (stabP->n_strx)
	{
	  int i;
	  int strLen = strlen(string);
	  if (strLen == 0) break;                // seems to happen sometimes with /usr/ccs/bin/ld on Solaris
	  curFuncNameP = NEW_ARRAY(char,strLen + 1);  // more than I need but not by much
	  for (i = 0;  (i < strLen) && (string[i] != ':') && (string[i] != '('); i++) {
	    curFuncNameP[i] = string[i];
	  }
	  curFuncNameP[i] = '\0';
	  if (debugFlag) fprintf(stderr, "found function name %s, len %d: %s\n", string, strLen, curFuncNameP);
	  //
	  // Usually n_value is 0 for a dynamic library symbol if the linker is GNU ld.
	  // In this case dlsym will work to get the base address of the function of interest here.
	  // When using g++ with /usr/ccs/bin/ld on Solaris then n_value is some offset
	  // probably from the address at which this library was loaded.  In this case as well
	  // dlsym will work to get the function of interest's base address.  I could somehow
	  // detect which linker was used and do the calculation of base + offset, but why not
	  // just use dlsym with both linkers and have one solution?  Seems simpler if slightly
	  // more inefficient.
	  // If the symbol is static, n_value will likely not be zero, and
	  // dlsym won't work for it.  In the case of g++ with GNU ld, n_value is a valid
	  // function address for static functions in the statically linked executable.  I'm guessing that GNU ld fixed up the
	  // stabs symbol.  For a static symbol in a dynamic library we need to add exeBase.
	  // So the simplest way to do this is to call dlsym.  If it works we've got a good
	  // address.  If it doesn't work use n_value.  
	  // Note that dynLibHandle is for the currently running image since dlopen was called with
	  // a zero first argument.
	  //
	  funcBaseP = dlsym(dynLibHandle, curFuncNameP);  
	  if (!funcBaseP) 
	  {
#if defined(HAVE_LIBDEMANGLE)
	    //
	    // This code is conditionally compiled for the SunWorks C++ compiler only.
	    // That compiler creates FUN stabs for C/C++ library routines but uses strange, abreviated names for these functions.  E.g.:
	    // 68     FUN    0      0      0000000000000000 7778   cI__insert6Mpn0HO__rb_tree_node_7rkn0D__n0HIiterator_(0,59):P(0,75);
	    // 69     FUN    0      0      0000000000000000 7898   cG__copy6Mpn0HO__rb_tree_node_7_7(0,59):P(0,74);(0,94);(0,74);(0,74)
	    // 70     FUN    0      0      0000000000000000 7967   cH__erase6Mpn0HO__rb_tree_node__v(0,59):P(0,13);(0,94);(0,74)
	    // 71     FUN    0      0      0000000000000000 8029   c2G6Mrk5_r5(0,59):P(0,97)=&(0,59);(0,94);(0,98)=&(0,99)=k(0,59)
	    //
	    //Ignore any function with name matching: c[A-Z0-9]
	    //
	    if ((curFuncNameP[0] == 'c') && (isupper(curFuncNameP[1]) || isdigit(curFuncNameP[1]))) {
	    }
	    else {
#endif
	      //
	      // This is a 32-bit assignment, and not portable.
	      // jlp: Wouldn't exeBase be zero if e_type were ET_EXEC?  Or perhaps 0x1000...?
	      //
	      if (typeIsExe) {
		funcBaseP = (void *) stabP->n_value;
	      }
	      else {
		funcBaseP = (void *) ((AddressT) stabP->n_value + exeBase);
	      }
#if defined(HAVE_LIBDEMANGLE)
	    }
#endif
	  }
	  else {   
	  }
	}
	else { // strx is null -- the is the end of a function definition
	  if (((AddressT) funcBaseP + stabP->n_value + 4) > funcEnd) {
	    funcEnd = (AddressT) funcBaseP + stabP->n_value + 4;
	  }
	}
	break;
      case N_SLINE:
      {
	sourceInfoT *infoP = (sourceInfoT *)malloc(sizeof(sourceInfoT));

	infoP->srcPathNameP       = pathStackP[pathCount];
	infoP->srcFileNameP       = fileStackP[fileCount];
	//
	// The g++ stabs data has only one function name, unlike the HP aCC debug symbol table.
	//
	infoP->funcMangledNameP   = curFuncNameP;
	infoP->funcDemangledNameP = 0;
	infoP->lineNum            = stabP->n_desc;
	//
	// Note the 32-bit only address arithmetic below.  stabP->n_value is currently unsigned
	// so I'm casting funcBaseP to that as well.  This will need to be redone to support
	// 64-bit addressing.
	//
	//(*PCMapP)[(void *) ((unsigned) funcBaseP + stabP->n_value)] = infoP;  
	PCMapT_insert(PCMapP, (void *) ((unsigned) funcBaseP + stabP->n_value), infoP);
	break;
      }
      case N_SO:
	if (string && strlen(string)) {
	  if (string[strlen(string) - 1] == '/') {
	    pathStackP[++pathCount] = string;
	  } else {
	    //
	    // Sometimes the stab string for this type includes the path.  Ignore the path if it is there.
	    //
	    fileStackP[++fileCount] = string;
	    {
	      int i;
	      for (i = strlen(string) - 1; i >= 0; i--) {
		if (string[i] == '/') {
		  fileStackP[fileCount] = &string[i+1];
		  break;
		}
	      }
	    }
	  }
	} else {
	  if (pathCount) --pathCount;
	  fileCount--;
	}
	break;
      case N_SOL: 
	fileCount = 0;
	pathStackP[0] = emptyStringP;  // I guess the string includes the path in this case.
	fileStackP[0] = string;        // As far as I've seen it does.
	break;
      default:
	break;
    }
    stabP++;
  }
  //
  // Put in dummy entry at end with the largest pc for the stabs we've seen.
  //
  PCMapT_insert(PCMapP, (void *)funcEnd, (void *)0);

  PCMapT_sort(PCMapP);
  return PCMapP;
}

#ifdef ARCH_IS_SPARC

#include <stddef.h>
#include <sys/reg.h>
#include <sys/frame.h>
#include <sys/regset.h>
#include <ucontext.h>
#include <unistd.h>
#include <libgen.h>
/*#include <libelf.h>*/
#include </usr/include/libelf.h>

#ifdef __sparcv9
#define FRAME_PTR_REGISTER         REG_SP
#define ARGUMENT_PTR(p)            ((long*)((p)->fr_arg))
#define BIAS                       2047


#elif defined(__sparc)
#define FRAME_PTR_REGISTER         REG_SP
#define ARGUMENT_PTR(p)            ((long*)((p)->fr_arg))
#define BIAS                       0

#elif defined(__i386)
#define FRAME_PTR_REGISTER         EBP
#define ARGUMENT_PTR(p)            (((long*)&((p)->fr_savfp)) + 2)
#define BIAS                       0
#endif

//  
//     Walk the stack printing each calling function
//   
//     For the call chain:         main() ==> one() ==> two() ==> tb()
//   
//     The stack, on Sparc, looks like:
//       .--------------------------------------------------------------------.
//       |  frame for tb  |  frame for two |  frame for one |  frame for main |
//       `--------------------------------------------------------------------'
//       ^                ^
//       |                |
//       SP               FP
//   
//     Each frame looks like:
//       .--------------------------------------------------------------------.
//       | register save area | struct ret ptr[1]  | outgoing args | locals   |
//       `--------------------------------------------------------------------'
//          [1] Note: This field does not exist in the V9 ABI
//     
//     The register save area looks like:
//       .--------------------------------------------------------------------.
//       | local regs 0-7  | arg regs 0-5  | frame pointer  |  return address |
//       `--------------------------------------------------------------------'
//   
/*{{{  const char * ContextStackTrace*/
const char * C_ContextStackTrace(void *uap,
				 int pathInTrace,
				 int printLongStack,
				 int  numLibs,
				 const char **libsPP,
				 int debug,
				 int max_level)
{
  struct frame *curr_frameP;
  void* return_address;
  int state = 0; // controls whether to print a line or not.
  int level = 0;
  
  debugFlag = debug;
  
  //
  //  Initialize with the SP for this function.
  //
  curr_frameP = (struct frame*)((long)(((ucontext_t*)uap)->uc_mcontext.gregs[FRAME_PTR_REGISTER]) + BIAS);
  
  printFunctionInit();

  SNPRINTF(messageString, MESSAGE_LEN, "Stack trace:\n");

  //
  //  Walk the stack and dump a traceback along the way
  //
  while ((return_address = (void*)(curr_frameP->fr_savpc)) != 0) 
  {
    curr_frameP = (struct frame*)((long)(curr_frameP->fr_savfp) + BIAS);
    if (!curr_frameP) break;
    if (curr_frameP->fr_savpc)  /* skip "_start()" function */
    {
      printFunction(return_address, 
		    ARGUMENT_PTR(curr_frameP), 
		    messageString,
		    MESSAGE_LEN,
		    &state, 
		    printLongStack, 
		    numLibs, 
		    libsPP,
		    pathInTrace);
    }
    level++;
    if (max_level > 0 && level >= max_level) break;
  }
  return messageString;
}
/*}}}*/
/*{{{  const char * StackTrace*/
const char * C_StackTrace(int pathInTrace,
			  int printLongStack,
			  int  numLibs,
			  const char **libsPP,
			  int debug,
			  int max_level)
{
  ucontext_t u;
  
  //
  //  Since getcontext() is a syscall, it will flush the current register
  //  window on a SPARC, so the stack memory image will be up-to-date.
  //
  //  This function is available on HPUX and Linux.
  //
  (void) getcontext(&u);
  return C_ContextStackTrace((void*) &u,pathInTrace,printLongStack,numLibs,libsPP,debug,max_level);
}
/*}}}*/

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(HAVE_SYS_LINK_H)
#include <sys/link.h>
#endif

int errno;

static void elfError(const char *funcNameP);

//
// Take an executable name and build a PCMap (addr to file name and line number map)
//
/*{{{  static PCMapT* buildPCMap(const char * exeFileNameP)*/
static PCMapT buildPCMap(const char * exeFileNameP)
{
  Elf32_Shdr    *shdrP;
  Elf32_Ehdr    *ehdrP;
  Elf           *elfP;
  Elf_Scn       *strScnP, *scnP, *stabScnP = 0, *stabStrScnP = 0;
  Elf_Data      *strDataP, *stabDataP, *stabStrDataP;
  int fd;
  AddressT  exeBase=0;
  struct stab *stabP;
  char *stringPoolP;
  PCMapT rtnMapP;
  static void *dynLibHandle = 0;

  if (!dynLibHandle)  // i.e. first time through
  {
    dynLibHandle = dlopen(0, RTLD_LAZY | RTLD_GLOBAL);
    if (!dynLibHandle)
    {
      return (PCMapT) NULL;
    }
  }

  if ((fd = open(exeFileNameP, O_RDONLY)) == -1)
  {
    return (PCMapT) NULL;
  }

  //
  // Obtain the ELF descriptor
  //
  if (elf_version(EV_CURRENT) == EV_NONE) 
  {
    return (PCMapT) NULL;
  }
  
  if ((elfP = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
  {
    elfError("elf_begin");
    return (PCMapT) NULL;
  }
  
  if ((ehdrP = elf32_getehdr(elfP)) == NULL) 
  {
    elfError("elf32_getehdr");
    return (PCMapT) NULL;
  }

  //
  // Get the base address of this library or exe.
  // Do this for every call of this function even though the link map can only be changed
  // by a dlopen().  Call dlinfo() every time through here in case exeFileNameP was
  // dlopen'ed since a previous call to this function.
  //
#if defined(HAVE_SYS_LINK_H) && defined(HAVE_DL_INFO)
  {
    Link_map   *link_mapP = 0;  // This structure is defined in <sys/link.h>
    int dlinfoRtn = dlinfo(dynLibHandle, RTLD_DI_LINKMAP, (void *) &link_mapP);
    // 
    // If dlinfo fails we don't get the exeBase and tracebacks with static functions in them
    // will be wrong about the static function.  But that's not a fatal problem.  So keep going.
    //
    if (!dlinfoRtn) {
      while (link_mapP) {
	if (strstr(exeFileNameP, link_mapP->l_name)) {
	  exeBase = link_mapP->l_addr;
	} // if strings match
	link_mapP = link_mapP->l_next;
      }  // while
    }    // if dlinfo() worked
    else {
      exeBase = 0;
    }
  }
#else
    exeBase = 0;
#endif

  // 
  // ehdrP->e_shstrndx is the size_t index into the section
  // header table.  Get the section header string table at
  // this index.
  //
  if ((strScnP = elf_getscn(elfP, ehdrP->e_shstrndx)) == NULL)
  {
    elfError("elf32_getscn");
    return (PCMapT) NULL;
  }

  //
  // Get the section header string table data based on the 
  // section handle gotten just above.  This region is accessed
  // for the names of all the sections.  The NULL 2nd argument
  // below means get the first buffer associated with the
  // section.
  //
  strDataP = elf_getdata(strScnP, NULL);
  if (!strDataP)
  {
      elfError("elf_getdata for str");
      return (PCMapT) NULL;
  }
  
  /* With scn == NULL we get the section descriptor for index == 1 */
  
  scnP = NULL;
  
  while((scnP = elf_nextscn(elfP, scnP)))
  {
    char *section_name;
    if ((shdrP = elf32_getshdr(scnP)) == NULL)
    {
      elfError("elf32_getshdr");
      return (PCMapT) NULL;
    }
    section_name = (char *)strDataP->d_buf + shdrP->sh_name;
    if (strcmp(section_name, ".stab") == 0) 
    {
      stabScnP = scnP;
    }
    else if (strcmp(section_name, ".stabstr") == 0) 
    {
      stabStrScnP = scnP;
    }
  }
  stabDataP = elf_getdata(stabScnP, NULL);
  if (!stabDataP)
  {
      elfError("elf_getdata for stab");
      return (PCMapT) NULL;
  }
  stabP = (struct stab *) stabDataP->d_buf;
  stabStrDataP = elf_getdata(stabStrScnP, NULL);
  if (!stabStrDataP)
  {
      elfError("elf_getdata for stab str");
      return (PCMapT) NULL;
  }
  stringPoolP = (char *) stabStrDataP->d_buf;
  rtnMapP = processStabs(stabP, stabDataP->d_size, stringPoolP, dynLibHandle, exeBase, ehdrP->e_type == ET_EXEC);
  return rtnMapP;
}

static void elfError(const char *fname)
{
  if (debugFlag) {
    char tmp[1000];
    const char *msgP;
    sprintf(tmp, "%s failed.\n", fname);
    if ((msgP = elf_errmsg(elf_errno())) != NULL)
      {
	sprintf(tmp, "%s", elf_errmsg(elf_errno()));
      }
    fprintf(stderr, "%s\n", tmp);
  }
}


#elif defined(ARCH_IS_X86)

#include <ucontext.h>


/* Register number containing frame base. */
#define FRAME_PTR_REGISTER	6 /* REG_EBP */
/* Offset from frame base of store location of caller PC. */
#define OFFSET_OF_CALLER_PC	4
/* Offset from frame base of store location of caller frame. */
#define OFFSET_OF_PREV_FRAME	0

const char * C_ContextStackTrace(void *uap,
				 int pathInTrace,
				 int printLongStack,
				 int  numLibs,
				 const char **libsPP,
				 int debug,
				 int max_level)
{
  int state = 0;
  int level = 0;
  AddressT ebp;
  void *caller_pc;
  
  if (debugFlag) 
    fprintf(stderr, "C_ContextStackTrace(%p, %d, %d, %d, %p, %d, %d)\n", 
	    uap, pathInTrace, printLongStack, numLibs, libsPP, debug, 
	    max_level);
  
  ebp = ((ucontext_t *)uap)->uc_mcontext.gregs[FRAME_PTR_REGISTER];
  
  printFunctionInit();
  SNPRINTF(messageString, MESSAGE_LEN, "Stack trace:\n");

  while (!(ebp & 1ULL) && *((AddressT *)ebp)) {
    caller_pc = (void *) *((AddressT *)(ebp+OFFSET_OF_CALLER_PC));
    printFunction(caller_pc,
		  0,
		  messageString,
		  MESSAGE_LEN,
		  &state,
		  printLongStack,
		  numLibs,
		  libsPP,
		  pathInTrace);
    level++;
    if (max_level > 0 && level >= max_level) break;
    if (*((AddressT *)ebp) < ebp) break;  // because it looks like we've gotten to an illegal address
    ebp = *((AddressT *)(ebp+OFFSET_OF_PREV_FRAME));
  }
  if (debugFlag) fprintf(stderr, "END C_ContextStackTrace\n");
  return messageString;
}


//
// In several tests (e.g. BarrierT) after a number of stack unwinds ebp comes
// out as an odd number, e.g.: ebp = (AddressT *) 0x83e58955
// In bug91 ebp goes to a weird illegal address, e.g. 0x81000
// I thought ebp should be null at the end of the stack.  It is clearly illegal to dereference this type
// of address, so we need a way to reliably know when to stop.
// Each successive value of ebp should be greater than the previous one because of the direction that
// the stack grows on x86/Linux.  So use that heuristic.  And as an extra check, make sure the address
// is even.
//

const char * C_StackTrace(int pathInTrace,
			  int printLongStack,
			  int  numLibs,
			  const char **libsPP,
			  int debug,
			  int max_level)
{
  ucontext_t u;
  const char *message;
  debugFlag = debug;
  
  if (debugFlag) fprintf(stderr, "C_StackTrace(%d, %d, %d, %p, %d, %d)\n", 
			 pathInTrace, printLongStack, numLibs, libsPP, debug, max_level);


  getcontext(&u);
  
  message = C_ContextStackTrace(&u, pathInTrace, printLongStack,
			  numLibs, libsPP, debug, max_level);

  if (debugFlag) fprintf(stderr, "END C_StackTrace\n");
  return message;
}

#if defined(HAVE_ELFR_H)

#include <string.h>
#include <errno.h>
#include "elfr.h"

#if defined(HAVE_LINK_H)
#include <link.h>
#endif
static PCMapT buildPCMap(const char *exeFileNameP) 
{
  FILE *elf_file;
  elfr_handle_t elf_h;
  static void *dynLibHandle = 0;
  const char *stab_name = ".stab";
  const char *stabstr_name = ".stabstr";
  int stab_id, stabstr_id;
  int stab_size;
  const char *stab_data, *stabstr_data;
  PCMapT rtnMapP;
  AddressT exeBase;
  ElfW(Dyn) *dynP;
  struct r_debug *r_debugP;  

  if (!dynLibHandle)  {
    // i.e. first time through
    dynLibHandle = dlopen(0, RTLD_LAZY | RTLD_GLOBAL);
    if (!dynLibHandle) return NULL;
  }
  
  elf_file = fopen(exeFileNameP, "rb");
  if (elf_file == NULL) {
    fprintf(stderr, "Can't open %s: %s\n", exeFileNameP, strerror(errno));
    goto fopen_failed;
  }
  
  elf_h = elfr_new(elf_file);
  if (elf_h == NULL) {
    fprintf(stderr, "Can't elfr_new(): %s\n", elfr_strerror(elf_h));
    goto elfr_new_failed;
  }

  if (elfr_read(elf_h) == -1) {
    fprintf(stderr, "Can't read elf file: %s\n", elfr_strerror(elf_h));
    goto elfr_read_failed;
  }

  stab_id = elfr_get_section(elf_h, stab_name);
  if (stab_id == -1) {
    fprintf(stderr, "Couldn't find %s\n", stab_name);
    goto elfr_read_failed;
  }
  if (elfr_section_data(elf_h, stab_id, &stab_data) == -1) {
    fprintf(stderr, "Couldn't load data for %s\n", stab_name);
    goto elfr_read_failed;
  }
  stab_size = elfr_section_size(elf_h, stab_id);
  
  stabstr_id = elfr_get_section(elf_h, stabstr_name);
  if (stabstr_id == -1) {
    fprintf(stderr, "Couldn't find %s\n", stabstr_name);
    goto elfr_read_failed;
  }
  if (elfr_section_data(elf_h, stabstr_id, &stabstr_data) == -1) {
    fprintf(stderr, "Couldn't load data for %s\n", stabstr_name);
    goto elfr_read_failed;
  }

  /* Get base address of this library from DT_DEBUG entry. */
  exeBase = 0;
#if defined(USE_DT_DEBUG)
  dynP = _DYNAMIC;  /* get _DYNAMIC section */
  if (_DYNAMIC) {
    for (dynP = _DYNAMIC; dynP->d_tag != DT_NULL; ++dynP) {
      if (dynP->d_tag == DT_DEBUG) {
	r_debugP = (struct r_debug *) dynP->d_un.d_ptr;
	if (r_debugP) {
	  struct link_map *lmP = r_debugP->r_map;
	  while (lmP) {
	    if (lmP->l_name && *lmP->l_name && strstr(exeFileNameP, lmP->l_name)) {
	      exeBase = lmP->l_addr;
	      break;
	    }
	    lmP = lmP->l_next;
	  }
	}
      }
    }
  }
#endif
  
  rtnMapP = processStabs((struct stab *) stab_data, stab_size, stabstr_data,
			 dynLibHandle, exeBase, 0);

  return rtnMapP;

 elfr_read_failed:
  elfr_del(elf_h);
 elfr_new_failed:
  fclose(elf_file);
 fopen_failed:
  return NULL;
}

/* end of HAVE_ELFR_H */
#else 
#error "no support for reading elf file"
#endif 

#endif

#endif /* !OMIT_STACKTRACE */

#endif /* defined(TARG_ST) && defined(STACK_TRACE) */
