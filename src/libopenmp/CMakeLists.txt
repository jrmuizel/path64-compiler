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




path64_set_multitarget_sources(openmp COMMON ${PATHSCALE_OPENMP_SRCS})
path64_set_multitarget_property_(openmp COMPILE_FLAGS "-std=gnu99 -D_GNU_SOURCE -fPIC")
path64_set_multitarget_property_(openmp LINK_FLAGS
                                "-Wl,--version-script=\"${libopenmp_SOURCE_DIR}/libopenmp.vs\"")
path64_add_multitarget_library(openmp SHARED)
path64_add_multitarget_dependencies(openmp compiler-stage-libs-C)

path64_set_multitarget_sources(openmp-static COMMON ${PATHSCALE_OPENMP_SRCS})
path64_set_multitarget_property_(openmp-static COMPILE_FLAGS "-std=gnu99 -D_GNU_SOURCE")
path64_set_multitarget_property_(openmp-static OUTPUT_NAME "openmp")
path64_add_multitarget_library(openmp-static STATIC)
path64_add_multitarget_dependencies(openmp-static compiler-stage-libs-C)

if(PATH64_ENABLE_PROFILING)
    path64_set_multitarget_sources(openmp_p COMMON ${PATHSCALE_OPENMP_SRCS})
    path64_set_multitarget_property_(openmp_p COMPILE_FLAGS "-pg -std=gnu99 -D_GNU_SOURCE -fPIC")
    path64_add_multitarget_library(openmp_p STATIC)
    path64_add_multitarget_dependencies(openmp_p compiler-stage-libs-C)
endif()

install(FILES omp.h DESTINATION include/${PSC_FULL_VERSION})

if(PATH64_ENABLE_FORTRAN)
    install(FILES ../include/omp/omp_lib.f DESTINATION include/${PSC_FULL_VERSION})
endif()

foreach(targ ${PATH64_ENABLE_TARGETS})
    set(targ_flag ${_PATH64_TARGET_FLAG_${targ}})
    set(targ_arch ${_PATH64_TARGET_ARCH_${targ}})
    set(targ_bits ${_PATH64_TARGET_BITS_${targ}})


    if(PATH64_ENABLE_FORTRAN)
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${targ}
                           COMMAND ${CMAKE_COMMAND} -E make_directory
                                   ${CMAKE_CURRENT_BINARY_DIR}/${targ})

        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/omp_lib-${targ}.f.o
                                  ${CMAKE_CURRENT_BINARY_DIR}/${targ}/OMP_LIB.mod
                                  ${CMAKE_CURRENT_BINARY_DIR}/${targ}/OMP_LIB_KINDS.mod
                           COMMAND ${CMAKE_Fortran_COMPILER} ${CMAKE_Fortran_FLAGS} ${targ_flag}
                                   -c ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f
                                   -o ${CMAKE_CURRENT_BINARY_DIR}/omp_lib-${targ}.f.o
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f
                                   ${CMAKE_CURRENT_BINARY_DIR}/${targ}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${targ}
                           VERBATIM
                          )
    
        add_custom_target(OMP_LIB-${targ} ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${targ}/OMP_LIB.mod
                                  ${CMAKE_CURRENT_BINARY_DIR}/${targ}/OMP_LIB_KINDS.mod
                         )
        add_dependencies(OMP_LIB-${targ} switch_fc mfef95-tmp compiler-stage-Fortran)


        set(install_mod_dir ${PATH64_LIB_PATH}/${targ_arch}/${targ_bits})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${targ}/OMP_LIB.mod DESTINATION ${install_mod_dir})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${targ}/OMP_LIB_KINDS.mod DESTINATION ${install_mod_dir})
    endif()

endforeach()

