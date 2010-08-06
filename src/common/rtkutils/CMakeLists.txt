# build rtkutils.a and a couple custom commands

SET(PATHSCALE_RTKUTILS_SRCS   	"utils_memory.c"
	"utils_print.c"
	"utils_debug.c"
	"utils_option.c"
	"utils_string.c"
	"utils_version_id.c"
	"libUtils.c"
	"libUtilsOptions.c"
	"libUtilsVersion.c")

INCLUDE_DIRECTORIES(
	.
	)


add_library(rtkutils STATIC ${PATHSCALE_RTKUTILS_SRCS})

