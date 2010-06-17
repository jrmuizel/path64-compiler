
set(WINDOWS_CRT_SRC	"mkstemp.c"
                    "readlink.c"
                    "strtok_r.c"
                    "vasprintf.c")

ADD_DEFINITIONS(
	-DWINDOWS_CRT
)

ADD_LIBRARY(windows_crt STATIC ${WINDOWS_CRT_SRC})

# SET_TARGET_PROPERTIES(windows_crt PROPERTIES OUTPUT_NAME "windows_crt")

install(TARGETS windows_crt
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

