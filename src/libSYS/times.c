/*
 * times.c
 *
 * This file defines the times() function support available
 * from <times.h> on some systems.
 * The implementation supports MINGW32 host.
 * It has been tested with x-mingw32-gcc-3.4.2.
 */
#include "W_times.h"

#ifdef NEED_TIMES
#ifdef __MINGW32__
#include "W_times.h"
#include <sys/types.h>
#include <time.h>
#include <windows.h>

/*
 * Debug support
 */
#ifdef Is_True_On
#define TIMES_DEBUG
#endif
#ifdef TIMES_DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#define times_debug(x) do { if (getenv("TIMES_DEBUG")) printf x; } while(0)
#else
#define times_debug(x) (void)0
#endif


/*
 * Static children times in 1e-7 sec.
 * Used to accumulate child times information.
 * Note that only direct child times are accumulated, the
 * times of a granchild (child of a child) cannot be retrieved
 * and thus will not be reflected in the times() call.
 */
static unsigned long long children_utime;
static unsigned long long children_stime;


/*
 * Extracted from cygwin sources.
 */
#define NSPERSEC 10000000LL
static unsigned long long 
ft_to_clock_t (FILETIME * src)
{
  unsigned long long total = ((unsigned long long) src->dwHighDateTime << 32) +
    ((unsigned)src->dwLowDateTime);
  times_debug (("total (ft) %"PRIu64"\n", total));

  /* Convert into clock ticks - the total is in 10ths of a usec.  */
  total /= (NSPERSEC/CLOCKS_PER_SEC);
  times_debug (("total (clocks)%"PRIu64"\n", total));
  return total;
}

/*
 * Accumulate the times for the given child handle into the current 
 * process times: This function is called from win32 version of sys_pwait when
 * a child process terminates.
 */
void
sys_update_child_times(HANDLE child)
{
  FILETIME creation_time, exit_time, kernel_time, user_time;
  
  if (GetProcessTimes (child, &creation_time, &exit_time,
		       &kernel_time, &user_time) != 0) {
    unsigned long long stime, utime;
    stime = ((unsigned long long) kernel_time.dwHighDateTime << 32) +
      ((unsigned)kernel_time.dwLowDateTime);
    utime = ((unsigned long long) user_time.dwHighDateTime << 32) +
      ((unsigned)user_time.dwLowDateTime);
    times_debug(("child time: %"PRIu64"u %"PRIu64"s\n", utime, stime));
    children_stime += stime; 
    children_utime += utime;
  } else {
#ifdef TIMES_DEBUG
    times_debug(("*** Failed times\n"));
#endif
  }
}

/*
 * Returns the tms time information for the current process.
 * Replacement for posix times() function.
 */
clock_t
times (struct tms *buf)
{
  static struct tms last_tms;
  struct tms tmp_tms;
  FILETIME creation_time, exit_time, kernel_time, user_time;
  HANDLE hMainProc = GetCurrentProcess ();
  DWORD ticks = GetTickCount ();
  DWORD adjustment, increment;
  BOOL disabled;
  clock_t adjust = 0;
  
  /* The precision of the timer is retrieved into adjustment. */
  if (GetSystemTimeAdjustment(&adjustment, &increment, &disabled) &&
      disabled == 0) {
    adjust = ((long long) adjustment * CLOCKS_PER_SEC) / NSPERSEC;
    times_debug (("adjust: %ld\n", adjust));
  }

  /* Convert from milliseconds to CLOCKS_PER_SEC. */
  clock_t tc = (clock_t) (((long long) ticks * CLOCKS_PER_SEC) / 1000);
  times_debug (("CLOCKS_PER_SEC %ld\n", CLOCKS_PER_SEC));
  if (GetProcessTimes (hMainProc, &creation_time, &exit_time,
		       &kernel_time, &user_time) != 0) {
    buf->tms_stime = ft_to_clock_t (&kernel_time);
    buf->tms_utime = ft_to_clock_t (&user_time);
    buf->tms_cstime = children_stime / (NSPERSEC/CLOCKS_PER_SEC);
    buf->tms_cutime = children_utime / (NSPERSEC/CLOCKS_PER_SEC);
  } else {
    times_debug(("*** Failed times\n"));
  }
  /* Perform time adjustment to ensure that we never have
     user/kernel times > elapsed time. We substract to
     any time diff 2 time the adjustment given by GetSystemtimeAdjustment. */
  tmp_tms = *buf;
  if (buf->tms_stime - last_tms.tms_stime >= 2*adjust)
    buf->tms_stime -= 2*adjust;
  else 
    buf->tms_stime = last_tms.tms_stime;
  if (buf->tms_utime - last_tms.tms_utime >= 2*adjust)
    buf->tms_utime -= 2*adjust;
  else
    buf->tms_utime = last_tms.tms_utime;
  if (buf->tms_cstime - last_tms.tms_cstime >= 2*adjust)
    buf->tms_cstime -= 2*adjust;
  else
    buf->tms_cstime = last_tms.tms_cstime;
  if (buf->tms_cutime - last_tms.tms_cutime >= 2*adjust)
    buf->tms_cutime -= 2*adjust;
  else
    buf->tms_cutime = last_tms.tms_cutime;
  last_tms = tmp_tms;
  return tc;
}

#else /* !defined(__MINGW32__) */
#error "Unsupported platform"
#endif
#endif /* NEED_TIMES */

#ifdef TEST_TIMES
static int a[10];
int main(int argc, char *argv[])
{
  int count, i;
  struct tms t0, t1;
  clock_t c0, c1;
  double utime, stime, etime;
  if (argc < 2) {
    fprintf(stderr, "usage: %s <count>\n", argv[0]);
    exit(1);
  }
  count = atoi(argv[1]);
  c0 = times(&t0);
  for (i = 0; i < count; i++) {
    a[i%10] +=i;
  }
  c1 = times(&t1);
  utime = (double)(t1.tms_utime + t1.tms_cutime -
		   t0.tms_utime - t0.tms_cutime) / CLOCKS_PER_SEC;
  stime = (double)(t1.tms_stime + t1.tms_cstime -
		   t0.tms_stime - t0.tms_cstime) / CLOCKS_PER_SEC;
  etime =  (double)(c1 - c0)/ CLOCKS_PER_SEC;
  printf("cloks/sec: %ld\n", CLOCKS_PER_SEC);
  printf("%.02fu %.02fs %.02fe\n", utime, stime, etime);
  return 0;
}

#endif
  
