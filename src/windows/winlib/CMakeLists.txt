
set(winlib_SRC	"mkstemp.c"
                    "rawmemchr.c"
                    "readlink.c"
                    "rlimit.c"
                    "strtok_r.c"
                    "stubs.c"
                    "tempname.c"
                    "times.c"
                    "win2unixpath.c")

ADD_DEFINITIONS(
	-D_WINLIB_
	-DHAVE_CONFIG_H
	-D__MSVCRT_VERSION__=0x0601
)

include_directories(BEFORE .)

ADD_LIBRARY(winlib STATIC ${winlib_SRC})

# SET_TARGET_PROPERTIES(winlib PROPERTIES OUTPUT_NAME "winlib")

install(TARGETS winlib
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

