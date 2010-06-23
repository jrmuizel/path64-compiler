
set(WINDOWS_CRT_SRC	"mkstemp.c"
                    "rawmemchr.c"
                    "readlink.c"
                    "strtok_r.c"
                    "stubs.c"
                    "tempname.c"
                    "times.c"
                    "win2unixpath.c")

ADD_DEFINITIONS(
	-DWINDOWS_CRT
	-DHAVE_CONFIG_H
	-D__MSVCRT_VERSION__=0x0601
)

include_directories(BEFORE .)

ADD_LIBRARY(windows_crt STATIC ${WINDOWS_CRT_SRC})

# SET_TARGET_PROPERTIES(windows_crt PROPERTIES OUTPUT_NAME "windows_crt")

install(TARGETS windows_crt
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

