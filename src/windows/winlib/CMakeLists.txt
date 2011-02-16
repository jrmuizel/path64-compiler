
set(winlib_SRC	
				"mmap.c"
				"rawmemchr.c"
				"readlink.c"
				"rlimit.c"
				"strtok_r.c"
				"stubs.c"
				"symlink.c"
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

ADD_LIBRARY(winlib STATIC ${winlib_SRC})
target_link_libraries(winlib ws2_32)

