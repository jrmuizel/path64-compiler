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

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/omp_lib.f.o
        COMMAND ${CMAKE_Fortran_COMPILER} ${CMAKE_Fortran_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f -o ${Path64_BINARY_DIR}/lib/omp_lib.f.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/omp/omp_lib.f
	VERBATIM
)

ADD_DEFINITIONS(
	-std=gnu99
	-D_GNU_SOURCE)

IF(PATH64_ENABLE_OPENMP)
add_library (openmp-static STATIC ${PATHSCALE_OPENMP_SRCS})
add_library (openmp SHARED ${PATHSCALE_OPENMP_SRCS})
set_property(TARGET openmp PROPERTY SOVERSION 1)
set_property(TARGET openmp PROPERTY LINK_FLAGS -Wl,--version-script="${libopenmp_SOURCE_DIR}/libopenmp.vs")
SET_TARGET_PROPERTIES(openmp-static PROPERTIES OUTPUT_NAME "openmp")

IF(PATH64_ENABLE_PROFILING)
add_library(openmp_p STATIC ${PATHSCALE_OPENMP_SRCS})
set_property(TARGET openmp_p PROPERTY COMPILE_FLAGS -pg)
ENDIF(PATH64_ENABLE_PROFILING)

IF(${PATH64_ENABLE_FORTRAN})
add_custom_target(OMP_LIB ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/omp_lib.f.o
)
add_dependencies(OMP_LIB switch_fc mfef95-tmp)
ENDIF()
install(TARGETS openmp openmp-static
        LIBRARY DESTINATION ${PATH64_LIB_PATH}
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

IF(PATH64_ENABLE_PROFILING)
install(TARGETS openmp_p
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})
ENDIF(PATH64_ENABLE_PROFILING)


install(FILES omp.h DESTINATION ${PATH64_LIB_PATH}/include)
IF(${PATH64_ENABLE_FORTRAN})
install(FILES ../include/omp/omp_lib.f DESTINATION ${PATH64_LIB_PATH}/include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OMP_LIB.mod DESTINATION ${PATH64_LIB_PATH}/include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OMP_LIB_KINDS.mod DESTINATION ${PATH64_LIB_PATH}/include)
ENDIF()
ENDIF(PATH64_ENABLE_OPENMP)
