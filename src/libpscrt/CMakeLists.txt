project(libpscrt)


set(pscrt_sources
  malloc_opt.c
      )

set(pscrt_static_sources
  empty.c
  )

set(pscrt_sources_x64
  memset_em64t.s
  memset_opteron.s
  malloc_opt.c
  )

set(pscrt_static_sources_x64
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



# Static library
function(add_pscrt_static_library name src_name)
    set(srcs ${ARGV})
    list(REMOVE_AT srcs 0)


    foreach(targ ${PATH64_ENABLE_TARGETS})
        if(${targ} MATCHES "x8664_64")
            set(srcs ${${src_name}_x64})
        else()
            set(srcs ${${src_name}})
        endif()

        path64_add_library_for_target(${name} ${targ} STATIC ${srcs})
    endforeach()
endfunction()


add_pscrt_static_library(pscrt-static pscrt_static_sources)
path64_set_target_library_property(pscrt-static COMPILE_FLAGS -fpic)
path64_set_target_library_property(pscrt-static OUTPUT_NAME pscrt)


# Profiling library
if(PATH64_ENABLE_PROFILING)
    add_pscrt_static_library(pscrt_p pscrt_sources)
    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")   # temporary workaround for bug in gcc on osol
        path64_set_target_library_property(pscrt_p COMPILE_FLAGS -pg)
    endif()
endif()

#add_library(pscrt SHARED ${pscrt_sources})
#set_property(TARGET pscrt PROPERTY SOVERSION 1)
#set_property(TARGET pscrt PROPERTY LINK_FLAGS -Wl,--version-script="${libpscrt_SOURCE_DIR}/libpscrt.vs")


