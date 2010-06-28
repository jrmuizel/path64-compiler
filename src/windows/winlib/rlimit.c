
#include <sys/resource.h>

int getrlimit(int resource, struct rlimit *rlp)
{
    rlp->rlim_cur = RLIM_INFINITY;
    rlp->rlim_max = RLIM_INFINITY;
    return 0;
}

int setrlimit(int resource, const struct rlimit *rlp)
{
    return 0;
}
