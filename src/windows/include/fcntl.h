
#include_next <fcntl.h>

struct flock {
    short  l_type;
    short  l_whence;
    off_t  l_start;
    off_t  l_len;
    pid_t  l_pid;
};

#define F_WRLCK 1
#define F_SETLKW 2
#define PROT_READ 4
#define PROT_WRITE 8
#define MAP_SHARED 16
#define MAP_PRIVATE 32
