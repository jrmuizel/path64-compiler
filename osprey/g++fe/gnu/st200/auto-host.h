/* since we do not use the regular gcc configure way, we need to
   distinguish here between the different host platforms... */

#ifdef linux
# include "auto-host_linux.h"
#endif
#ifdef sun
# include "auto-host_sun.h"
#endif
#ifdef __CYGWIN__
# include "auto-host_cygwin.h"
#endif
#ifdef __MINGW32__
# include "auto-host_mingw32.h"
#endif

