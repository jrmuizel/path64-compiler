#include "W_mman.h"

#if defined(NEED_MMAP) && defined(NEED_MUNMAP)
/*
 * Debug support
 */
#define MMAP_DEBUG
#ifdef MMAP_DEBUG
#include <stdio.h>
#include <stdlib.h>
#define mmap_debug(x) do { if (getenv("MMAP_DEBUG")) printf x; } while(0)
#else
#define mmap_debug(x) (void)0
#endif


/* The following code is intended to be used with either cross
compilers or pure VC++ compilers */
#if defined(__MINGW32__) || ( defined(_WIN32) && !defined(__CYGWIN__))

/* Will exlude rarely used stuff from Win32 headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <unistd.h>


char *last_error(void)
{
  static char error_message[1024];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)error_message,sizeof(error_message),NULL);
  return error_message;
}

/* local record of the memory mapped area. */
typedef struct {    
    void *mmapped ; 		/* The actual return value that includes offset correction */
    void *lpBaseAddress ; 	/* The MapViewOfFile return value */
    size_t len ; 		/* The len that was requested by mmap */
    HANDLE hFileMappingObject ; /* The result of CreateFileMapping */
} MMAPRECORD ;

enum { MAX_MMAP_RECORD = 20 } ;
static MMAPRECORD mmapRecord[MAX_MMAP_RECORD]  ;

int setMmapRecord(void *addr, void *lpBaseAddress, int len, HANDLE hmap)
{
    int mmaprecord_ = -1 ;
    int i ;
    for(i = 0; i<sizeof(mmapRecord)/sizeof(mmapRecord[0]); ++i) {
	if (mmapRecord[i].mmapped==NULL) {
	    mmapRecord[i].mmapped = addr ;
	    mmapRecord[i].lpBaseAddress = lpBaseAddress ;
	    mmapRecord[i].len = len ;
	    mmapRecord[i].hFileMappingObject = hmap ;
	    mmaprecord_ = 0 ;
	    break ;
	}
    }
    return mmaprecord_ ;
}

MMAPRECORD *getMmapRecord(void *addr, size_t len) 
{
    MMAPRECORD *mmaprecord_ = NULL ; 
    int i ;
    for( i=0; i<sizeof(mmapRecord)/sizeof(mmapRecord[0]); ++i) {
	/* We insist on having same @ and size, but have no way to unmap less...*/
	if (mmapRecord[i].mmapped==addr && mmapRecord[i].len==len) { 
	    mmaprecord_ = &mmapRecord[i] ;
	    break ;
	}
    }
    return mmaprecord_ ;
}

/* local helper functions */
static DWORD getAllocationGranularity () __attribute__((const)) ;
static DWORD getAllocationGranularity ()
{
    static DWORD dwAllocationGranularity = 0 ;
    if (!dwAllocationGranularity) {
	SYSTEM_INFO si ;
	GetSystemInfo(&si) ;
	dwAllocationGranularity = si.dwAllocationGranularity ;
    }
    return dwAllocationGranularity ;
}

/* 
 * Creates a mmaped area a la Unix
 * Note that for primary target that is to support the open64
 * We have only 2 situations : 
 * mmap(0, fl->mapped_size, PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0) 
 * mmap(0, stat_buf.st_size, PROT_READ|PROT_WRITE,MAP_PRIVATE, fd, 0)
 * Can we ignore the MAP_PRIVATE situation : we cannot emulate it under Win32 ?
 *
 * errno : at some points, the sys_errlist[errno] is accessed
*/
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    LPVOID mmap_ = MAP_FAILED ;

    /* FIXME? Check if the area is already mmaped with the same parameters ? */
    
    /* Form the Win32's handle from the file descriptor */
    HANDLE hFile = (HANDLE)_get_osfhandle(fildes) ;

    mmap_debug(("mmap: addr:%p, len:%u, prot:%d, flags:%x, f:%d, off:%ld\n",
		addr, len, prot, (unsigned)flags, fildes, off));
    
    if (hFile && (hFile != INVALID_HANDLE_VALUE)) {	
	
	/* FIMXE? */
	DWORD flProtect = PROT_NONE ;	
	if (prot & PROT_WRITE) flProtect = PAGE_READWRITE ;
	else if (prot & PROT_READ) flProtect = PAGE_READONLY ; 
	if (flags & MAP_PRIVATE) flProtect = PAGE_WRITECOPY;
	
	HANDLE hFileMappingObject = CreateFileMapping(hFile , NULL, flProtect, 0, 0, NULL);

	if (hFileMappingObject && (hFileMappingObject != INVALID_HANDLE_VALUE)) {
	    DWORD dwDesiredAccess = (prot & PROT_WRITE) ? FILE_MAP_WRITE : FILE_MAP_READ  ;	  
	    if (flags &  MAP_PRIVATE) dwDesiredAccess = FILE_MAP_COPY;

	    // Offsets must be corrected by file allocation granularity
	    DWORD dwAllocationGranularity = getAllocationGranularity() ;
	    off64_t phstart = (off / dwAllocationGranularity ) * dwAllocationGranularity ;
	    off_t offset = off - phstart  ;
	    size_t size = len + offset ;
	    DWORD dwFileOffsetHigh =  phstart >> 32 ;
	    DWORD dwFileOffsetLow =  phstart & 0xffffffff ;
	    SIZE_T dwNumberOfBytesToMap = size ;

	    HANDLE MapViewOfFile_ = MapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap) ; 
	    if (MapViewOfFile_) {
		// MapViewOfFile success, record a new entry to use when unmapping
		mmap_ = MapViewOfFile_ + offset ;
		if (setMmapRecord(mmap_, MapViewOfFile_, len, hFileMappingObject)) {
		    // Failure to record the mapping
		}
	    } else {
		// We mock the unix return value */
		MapViewOfFile_ = MAP_FAILED ;
		// We had acquired some ressources 
		if (!CloseHandle(hFileMappingObject)) {
		    // CloseHandle failure
		}
		mmap_debug(("MapViewOfFile failed: %s\n", last_error()));
	    }
	} else {
	    // CreateFileMapping failure
	  mmap_debug(("CreateFilemapping failed: %s\n", last_error()));
	}
    }
    mmap_debug(("mmap -> %p\n", mmap_));

    return mmap_ ;
}

/* Removes a previously mmaped area */
int munmap(void *addr, size_t len)
{
    int munmap_ = -1 ;
    /* Look for a matching address */
    MMAPRECORD *mmaprec;
    mmap_debug(("munmap: addr:%p, len:%u\n", addr, len));

    mmaprec = getMmapRecord(addr, len) ;
    if (mmaprec && mmaprec->lpBaseAddress) {
	if (UnmapViewOfFile(mmaprec->lpBaseAddress)) {
	    mmaprec->lpBaseAddress = NULL ;
	    if (mmaprec && mmaprec->hFileMappingObject && (mmaprec->hFileMappingObject != INVALID_HANDLE_VALUE)) {
		if (CloseHandle(mmaprec->hFileMappingObject)) {
		    mmaprec->hFileMappingObject = INVALID_HANDLE_VALUE ;
		    munmap_ = 0 ;
		} else {
		    // CloseHandle failure
		}
	    } else {
		// Invalid handle in record
	    }
	} else {
	    // UnMapViewOfFile failure 
	}
    } else {
	// The region does not seem to be mmaped
    }   	   
    mmap_debug(("munmap -> %d\n", munmap_));
    return munmap_ ;
}

#elif defined(__sun__) || defined(__linux__) || defined(__CYGWIN__)
/* These guys have an implementation. If I need to intercept, I need to use syscall */
#else
#error "Unsupported platform"
#endif
#endif /* defined(NEED_MMAP) && defined(NEED_MUNMAP) */

