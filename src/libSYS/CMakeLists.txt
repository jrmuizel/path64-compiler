# build libSYS.a and a couple custom commands
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")


SET(PATHSCALE_SYS_SRCS        "SYS.c"
                                "sys_process_unix.c")

INCLUDE_DIRECTORIES(
	.
	../../GCC/include)


add_library(SYS STATIC ${PATHSCALE_SYS_SRCS})

