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

# Macro to add openmp libraries for given target architecture.
# The logical CMake library names are mangled by the architecture, but
# their OUTPUT_NAME on disk is not.  The build output and install
# locations include the architecture.
macro(libopenmp_add target)
  # Compiler ABI.
  set(arch ${_PATH64_TARGET_ARCH_${target}})
  set(bits ${_PATH64_TARGET_BITS_${target}})
  set(arch_flag ${_PATH64_TARGET_FLAG_${target}})
  set(build_lib_dir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${arch}/${bits})
  set(install_lib_dir ${PATH64_LIB_PATH}/${arch}/${bits})
  set(install_inc_dir ${install_lib_dir}/include)

  add_custom_command(
    OUTPUT ${build_lib_dir}/omp_lib.f.o
    COMMAND ${CMAKE_Fortran_COMPILER} ${CMAKE_Fortran_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f -o ${build_lib_dir}/omp_lib.f.o
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f
    VERBATIM
    )

  add_definitions(-std=gnu99 -D_GNU_SOURCE )

  add_library (openmp-${target}-static STATIC ${PATHSCALE_OPENMP_SRCS})
  add_library (openmp-${target} SHARED ${PATHSCALE_OPENMP_SRCS})
  set_property(TARGET openmp-${target} PROPERTY SOVERSION 1)
  set_property(TARGET openmp-${target} PROPERTY LINK_FLAGS "-Wl,--version-script=\"${libopenmp_SOURCE_DIR}/libopenmp.vs\" ${arch_flag}")
  set_property(TARGET openmp-${target} openmp-${target}-static PROPERTY OUTPUT_NAME "openmp")
  set_property(TARGET openmp-${target} openmp-${target}-static PROPERTY COMPILE_FLAGS ${arch_flag})

  set_property(TARGET openmp-${target}        PROPERTY LIBRARY_OUTPUT_DIRECTORY ${build_lib_dir})
  set_property(TARGET openmp-${target}-static PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})

  install(TARGETS openmp-${target} openmp-${target}-static
    LIBRARY DESTINATION ${install_lib_dir}
    ARCHIVE DESTINATION ${install_lib_dir})

  if(PATH64_ENABLE_PROFILING)
    add_library(openmp-${target}_p STATIC ${PATHSCALE_OPENMP_SRCS})
    set_property(TARGET openmp-${target}_p PROPERTY COMPILE_FLAGS "-pg ${arch_flag}")
    set_property(TARGET openmp-${target}_p PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})
    set_property(TARGET openmp-${target}_p PROPERTY OUTPUT_NAME "openmp_p")
    install(TARGETS openmp-${target}_p ARCHIVE DESTINATION ${install_lib_dir})
  endif()

  if(PATH64_ENABLE_FORTRAN)
    add_custom_target(OMP_LIB-${target} ALL
      DEPENDS ${build_lib_dir}/omp_lib.f.o
      )
    add_dependencies(OMP_LIB-${target} switch_fc mfef95-tmp)
  endif()

  install(FILES omp.h DESTINATION ${install_inc_dir})
  if(PATH64_ENABLE_FORTRAN)
    install(FILES ../include/omp/omp_lib.f DESTINATION ${install_inc_dir})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OMP_LIB.mod DESTINATION ${install_inc_dir})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OMP_LIB_KINDS.mod DESTINATION ${install_inc_dir})
  endif()
endmacro(libopenmp_add)

if(PATH64_ENABLE_OPENMP)
  # Loop over target architectures.
  # Currently we support at most one architecture, but multiple ABIs.
  foreach(target ${PATH64_ENABLE_TARGETS})
    libopenmp_add(${target})
  endforeach()
endif()
