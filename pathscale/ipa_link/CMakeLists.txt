# Do not inherit library building rules from the parent project.
SET(BUILD_SHARED_LIBS 0)

IF(APPLE)
  IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -no-cpp-precomp")
  ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(APPLE)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ANSI_CFLAGS}")

SET(BFD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bfd")

ADD_DEFINITIONS(-DIPA_LINK
		-DKEY
                -DHAVE_CONFIG_H
                -W
                -Wall
                -Wwrite-strings
                -Wstrict-prototypes
                -Wmissing-prototypes)

SUBDIRS(bfd ld)
