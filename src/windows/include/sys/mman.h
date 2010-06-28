
#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_

#define PROT_NONE 0x00
#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define PROT_EXEC 0x04

#define MAP_SHARED 0x0001
#define MAP_PRIVATE 0x0002
#define MAP_COPY MAP_PRIVATE
#define MAP_AUTOGROW 0x0040

#define MAP_FAILED ((void*)(-1))

void *mmap(void *, size_t, int, int, int, off_t);
int	munmap(void *, size_t);

#endif
