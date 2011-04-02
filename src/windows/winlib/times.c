
#include <time.h>
#include <sys/times.h>

clock_t times(struct tms *buffer)
{
    buffer->tms_utime = 0;
    buffer->tms_stime = 0;
    buffer->tms_cutime = 0;
    buffer->tms_cstime = 0;
    return clock();
}
