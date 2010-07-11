
ADD_DEFINITIONS(
	-D_WINLIB_
	-DHAVE_CONFIG_H
)

include_directories(BEFORE .)
include_directories(BEFORE ..)

ADD_LIBRARY(regex STATIC "regex.c")

# SET_TARGET_PROPERTIES(winlib PROPERTIES OUTPUT_NAME "winlib")

install(TARGETS regex
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

