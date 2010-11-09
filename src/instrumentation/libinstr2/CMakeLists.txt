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


path64_set_multitarget_sources(instr-static COMMON ${PATHSCALE_INSTR2_SRCS})
path64_set_multitarget_property_(instr-static OUTPUT_NAME "instr2")
path64_add_multitarget_library(instr-static STATIC)
path64_add_multitarget_dependencies(instr-static compiler-stage-libs-CXX)

path64_set_multitarget_sources(instr-shared COMMON ${PATHSCALE_INSTR2_SRCS})
path64_set_multitarget_property_(instr-shared SOVERSION 1)
path64_set_multitarget_property_(instr-shared COMPILE_FLAGS -fPIC)
if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "SunOS")
    path64_set_multitarget_property_(instr-shared LINK_FLAGS "-Wl,--version-script=\"${libinstr2_SOURCE_DIR}/libinstr.vs\" -nodefaultlibs")
else()
    path64_set_multitarget_property_(instr-shared LINK_FLAGS "-nodefaultlibs")
endif()
if(PATH64_ENABLE_PSCRUNTIME)
    path64_multitarget_link_libraries(instr-shared stl cxxrt eh gcc)
endif()
path64_multitarget_link_libraries(instr-shared c)
path64_set_multitarget_property_(instr-shared OUTPUT_NAME "instr2")
path64_add_multitarget_library(instr-shared SHARED)
path64_add_multitarget_dependencies(instr-shared compiler-stage-libs-CXX)

if(PATH64_ENABLE_PROFILING)
    path64_set_multitarget_sources(instr_p COMMON ${PATHSCALE_INSTR2_SRCS})
    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")   # temporary workaround for bug in gcc on osol
        path64_set_multitarget_property_(instr_p COMPILE_FLAGS -pg)
    endif()
    path64_set_multitarget_property_(instr_p OUTPUT_NAME "instr2_p")
    path64_add_multitarget_library(instr_p STATIC)
    path64_add_multitarget_dependencies(instr_p compiler-stage-libs-CXX)
endif()

