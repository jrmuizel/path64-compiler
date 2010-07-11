# build SYS.a and a couple custom commands

SET(PATHSCALE_SYS_SRCS        "SYS.c"
                                "sys_process_unix.c")

INCLUDE_DIRECTORIES(
	.
	../../GCC/include)


add_library(SYS STATIC ${PATHSCALE_SYS_SRCS})

