project(libpscrt)

set(pscrt_sources
  memset_em64t.s
  memset_opteron.s
  malloc_opt.c
  )

set(pscrt_static_sources
  memcpy_em64t.c
  memcpy_opteron.c
  )

# TODO: Enable ASM language.  For now we just tell CMake to run the C
# compiler (gcc) and add some flags.
set_source_files_properties(
  memset_em64t.s
  memset_opteron.s
  PROPERTIES
  LANGUAGE C
  COMPILE_FLAGS "-x assembler-with-cpp"
  )

add_library(pscrt-static STATIC ${pscrt_sources} ${pscrt_static_sources})
set_property(TARGET pscrt-static PROPERTY COMPILE_FLAGS -fpic)
set_property(TARGET pscrt-static PROPERTY OUTPUT_NAME pscrt)

IF(PATH64_ENABLE_PROFILING)
add_library(pscrt_p STATIC ${pscrt_sources} ${pscrt_static_sources})
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")   # temporary workaround for bug in gcc on osol
set_property(TARGET pscrt_p PROPERTY COMPILE_FLAGS -pg)
endif()
ENDIF()

#add_library(pscrt SHARED ${pscrt_sources})
#set_property(TARGET pscrt PROPERTY SOVERSION 1)
#set_property(TARGET pscrt PROPERTY LINK_FLAGS -Wl,--version-script="${libpscrt_SOURCE_DIR}/libpscrt.vs")


install(TARGETS pscrt-static
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

IF(PATH64_ENABLE_PROFILING)
install(TARGETS pscrt_p
        LIBRARY DESTINATION ${PATH64_LIB_PATH}
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})
ENDIF()

