project(libinstr2)

SET(PATHSCALE_INSTR2_SRCS       "dump.cxx"
				"profile.cxx"
				"../../common/instrument/profile_errors.cxx"
				"profile_interface.cxx")

INCLUDE_DIRECTORIES(
	.
	../../common/instrument
	../../common/com
	../include)

add_library(instr-static STATIC ${PATHSCALE_INSTR2_SRCS})

add_library(instr-shared SHARED ${PATHSCALE_INSTR2_SRCS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
target_link_libraries(instr-shared winlib)
endif()

set_property(TARGET instr-shared PROPERTY SOVERSION 1)
set_property(TARGET instr-shared PROPERTY LINK_FLAGS -Wl,--version-script="${libinstr2_SOURCE_DIR}/libinstr.vs")
SET_TARGET_PROPERTIES(instr-shared PROPERTIES OUTPUT_NAME "instr2")
SET_TARGET_PROPERTIES(instr-static PROPERTIES OUTPUT_NAME "instr2")

IF(PATH64_ENABLE_PROFILING)
add_library(instr_p STATIC ${PATHSCALE_INSTR2_SRCS})
set_property(TARGET instr_p PROPERTY COMPILE_FLAGS -pg)
SET_TARGET_PROPERTIES(instr_p PROPERTIES OUTPUT_NAME "instr2_p")
install(TARGETS instr_p DESTINATION ${PATH64_LIB_PATH})
ENDIF()

install(TARGETS instr-shared instr-static DESTINATION ${PATH64_LIB_PATH})
