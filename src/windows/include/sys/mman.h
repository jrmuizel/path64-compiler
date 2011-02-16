#ifndef _MMAN_H_
#define _MMAN_H_

#include <sys/types.h>

#define PROT_READ       0x1             /* page can be read */
#define PROT_WRITE      0x2             /* page can be written */
#define PROT_EXEC       0x4             /* page can be executed */
#define PROT_NONE       0x0             /* page can not be accessed */

#define MAP_SHARED      0x01            /* Share changes */
#define MAP_PRIVATE     0x02            /* Changes are private */
#define MAP_FIXED       0x10            /* Interpret addr exactly */

#define MAP_FAILED ((void*)-1)

#ifdef __cplusplus
extern "C" {
#endif

void *mmap(void*, size_t, int, int, int, off_t);
int	munmap(void*, size_t);

#ifdef __cplusplus
}
#endif

#endif
