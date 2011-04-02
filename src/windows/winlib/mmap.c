#include <stdio.h>
#include <errno.h>
#include <io.h>
#include <sys/mman.h>
#include <windows.h>

static DWORD granularity = 0;

#define ALIGN_OFFSET(i)  (((DWORD)(i)) & (granularity - 1))

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    HANDLE file_handle, map_handle;
    DWORD page_access, map_access;
    DWORD delta;
    void *map_addr;

    if (granularity == 0) {
        SYSTEM_INFO i;

        GetSystemInfo(&i);
        granularity = i.dwAllocationGranularity;
    }

    switch (prot) {
        case PROT_NONE:
            page_access = PAGE_NOACCESS;
            map_access  = FILE_MAP_READ;
            break;

        case PROT_READ:
            page_access = PAGE_READONLY;
            map_access  = FILE_MAP_READ;
            break;

        case PROT_WRITE:
            page_access = PAGE_READWRITE;
            map_access  = FILE_MAP_WRITE;
            break;

        case PROT_EXEC:
            page_access = PAGE_EXECUTE;
            map_access  = FILE_MAP_READ;
            break;
            
        case PROT_READ | PROT_WRITE:
            page_access = PAGE_READWRITE;
            map_access  = FILE_MAP_ALL_ACCESS;
            break;
            
        case PROT_READ | PROT_EXEC:
            page_access = PAGE_EXECUTE_READ;
            map_access  = FILE_MAP_READ;
            break;
            
        case PROT_EXEC | PROT_WRITE:
        case PROT_READ | PROT_WRITE | PROT_EXEC:
            page_access = PAGE_EXECUTE_READWRITE;
            map_access  = FILE_MAP_ALL_ACCESS;
            break;

        default:
            errno = EINVAL;
            return MAP_FAILED;
    }

    if (flags & MAP_PRIVATE) {
        map_access = FILE_MAP_COPY;
    }

    if (flags & MAP_FIXED) {
        if (ALIGN_OFFSET(addr)) {
            errno = EINVAL;
            return MAP_FAILED;
        }
    } else {
        addr -= ALIGN_OFFSET(addr);
    }

    file_handle = (HANDLE)_get_osfhandle(fd);
    if (file_handle == INVALID_HANDLE_VALUE ) {
        errno = EBADF;
        return MAP_FAILED;
    }

    map_handle = CreateFileMapping(file_handle, NULL, page_access, 
                                   0, (DWORD)(offset + len), NULL);
    if (!map_handle) {
        fprintf(stderr, "CreateFileMapping Status = 0x%08X\n", GetLastError());
        errno = EACCES;
        return MAP_FAILED;
    }

    delta = ALIGN_OFFSET(offset);

    map_addr = MapViewOfFileEx(map_handle, map_access, 0, ((DWORD)offset) - delta,
                               (SIZE_T)(len + delta), addr);

    if (map_addr == NULL) {
        fprintf(stderr, "MapViewOfFileEx Status = 0x%08X\n", GetLastError());
        errno = EACCES;
        map_addr = MAP_FAILED;
    }
    else {
        map_addr += delta;
    }
    
    CloseHandle(map_handle);

    return map_addr;
}

int	munmap(void *addr, size_t size)
{
    if (granularity == 0) {
        SYSTEM_INFO i;

        GetSystemInfo(&i);
        granularity = i.dwAllocationGranularity;
    }

	addr -= ALIGN_OFFSET(addr);

    if (!UnmapViewOfFile(addr)) {
		fprintf(stderr, "UnmapViewOfFile Status = 0x%08X\n", GetLastError());
		errno = EINVAL;
		return -1;
    }
	return 0;
}
