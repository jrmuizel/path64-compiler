project(libinstr2)

set(PATHSCALE_INSTR2_SRCS       "dump.cxx"
				"profile.cxx"
				"../../common/instrument/profile_errors.cxx"
				"profile_interface.cxx")

include_directories(
	.
	../../common/instrument
	../../common/com
	../include)

add_definitions(${PATH64_STDC_FORMAT_MACROS})


path64_set_multiarch_sources(instr_sources COMMON ${PATHSCALE_INSTR2_SRCS})
path64_add_multiarch_library(instr-static STATIC instr_sources)
path64_set_multiarch_property(instr-static OUTPUT_NAME "instr2")
path64_install_multiarch(instr-static)

path64_add_multiarch_library(instr-shared SHARED instr_sources)
path64_set_multiarch_property(instr-shared SOVERSION 1)
if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "SunOS")
    path64_set_multiarch_property(instr-shared LINK_FLAGS -Wl,--version-script="${libinstr2_SOURCE_DIR}/libinstr.vs")
endif()
path64_set_multiarch_property(instr-shared OUTPUT_NAME "instr2")
path64_install_multiarch(instr-shared)

if(PATH64_ENABLE_PROFILING)
    path64_add_multiarch_library(instr_p STATIC instr_sources)
    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")   # temporary workaround for bug in gcc on osol
        path64_set_multiarch_property(instr_p COMPILE_FLAGS -pg)
    endif()
    path64_set_multiarch_property(instr_p OUTPUT_NAME "instr2_p")
    path64_install_multiarch(instr_p)
endif()

