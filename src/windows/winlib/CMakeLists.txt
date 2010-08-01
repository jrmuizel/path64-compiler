
set(winlib_SRC	
				"mkstemp.c"
				"mmap.c"
				"mmap64.c"
				"munmap.c"
				"rawmemchr.c"
				"readlink.c"
				"rlimit.c"
				"strtok_r.c"
				"stubs.c"
				"symlink.c"
				"tempname.c"
				"times.c"
				"timersub.c"
				"uname.c"
				"unix2winpath.c"
				"wait.c"
				"win2unixpath.c")

ADD_DEFINITIONS(
	-D_WINLIB_
	-DHAVE_CONFIG_H
	-D__MSVCRT_VERSION__=0x0601
)

include_directories(BEFORE .)

ADD_LIBRARY(winlib SHARED ${winlib_SRC})
target_link_libraries(winlib ws2_32)

# SET_TARGET_PROPERTIES(winlib PROPERTIES OUTPUT_NAME "winlib")

install(TARGETS winlib
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

