project(libopenmp)

SET(PATHSCALE_OPENMP_SRCS       "affinity.c"
				"atomic.c"
				"cpuid.c"
				"lock.c"
				"loop.c"
				"manager.c"
				"omp.c"
				"schedule.c"
				"stack.c"
				"team.c"
				"thread.c"
				"utils.c")

SET(PATHSCALE_OPENMP_HEADER_SRCS "../include/omp/omp_lib.f" )

# Macro to add openmp libraries for given target architecture bits.
# The logical CMake library names are mangled by the bit count, but
# their OUTPUT_NAME on disk is not.  The build output and install
# locations include the bit count.
macro(libopenmp_add bits)
  # Compiler flag selecting this many bits.
  if(bits)
    set(bits_flag -m${bits})
  else()
    set(bits_flag "")
  endif()

  add_custom_command(
    OUTPUT ${Path64_BINARY_DIR}/lib${bits}/omp_lib.f.o
    COMMAND ${CMAKE_Fortran_COMPILER} ${CMAKE_Fortran_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f -o ${Path64_BINARY_DIR}/lib${bits}/omp_lib.f.o
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f
    VERBATIM
    )

  add_definitions(-std=gnu99)

  add_library (openmp${bits}-static STATIC ${PATHSCALE_OPENMP_SRCS})
  add_library (openmp${bits} SHARED ${PATHSCALE_OPENMP_SRCS})
  set_property(TARGET openmp${bits} PROPERTY SOVERSION 1)
  set_property(TARGET openmp${bits} PROPERTY LINK_FLAGS "-Wl,--version-script=\"${libopenmp_SOURCE_DIR}/libopenmp.vs\" ${bits_flag}")
  set_property(TARGET openmp${bits} openmp${bits}-static PROPERTY OUTPUT_NAME "openmp")
  set_property(TARGET openmp${bits} openmp${bits}-static PROPERTY COMPILE_FLAGS ${bits_flag})

  set_property(TARGET openmp${bits}        PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${bits})
  set_property(TARGET openmp${bits}-static PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}${bits})

  install(TARGETS openmp${bits} openmp${bits}-static
    LIBRARY DESTINATION ${PATH64_LIB_PATH}${bits}
    ARCHIVE DESTINATION ${PATH64_LIB_PATH}${bits})

  if(PATH64_ENABLE_PROFILING)
    add_library(openmp${bits}_p STATIC ${PATHSCALE_OPENMP_SRCS})
    set_property(TARGET openmp${bits}_p PROPERTY COMPILE_FLAGS "-pg ${bits_flag}")
    set_property(TARGET openmp${bits}_p PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}${bits})
    set_property(TARGET openmp${bits}_p PROPERTY OUTPUT_NAME "openmp_p")
    install(TARGETS openmp${bits}_p ARCHIVE DESTINATION ${PATH64_LIB_PATH}${bits})
  endif()

  if(PATH64_ENABLE_FORTRAN)
    add_custom_target(OMP_LIB${bits} ALL
      DEPENDS ${Path64_BINARY_DIR}/lib${bits}/omp_lib.f.o
      )
    add_dependencies(OMP_LIB${bits} switch_fc mfef95-tmp)
  endif()

  install(FILES omp.h DESTINATION ${PATH64_LIB_PATH}${bits}/include)
  if(PATH64_ENABLE_FORTRAN)
    install(FILES ../include/omp/omp_lib.f DESTINATION ${PATH64_LIB_PATH}${bits}/include)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OMP_LIB.mod DESTINATION ${PATH64_LIB_PATH}${bits}/include)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OMP_LIB_KINDS.mod DESTINATION ${PATH64_LIB_PATH}${bits}/include)
  endif()
endmacro(libopenmp_add)

if(PATH64_ENABLE_OPENMP)
  if(PATH64_ENABLE_MULTILIB)
    # For now we hard-code 32-bit and 64-bit architectures.
    # This can be a loop later.
    libopenmp_add(32)
    libopenmp_add(64)
  else()
    # Use the native tool default architecture.
    libopenmp_add("")
  endif()
endif()
