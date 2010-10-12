
SET(PATHSCALE_ASSIGN_SRCS "assign.c" )

INCLUDE_DIRECTORIES(
    ../../clibinc
)

ADD_DEFINITIONS(
    -D_LITTLE_ENDIAN
)

if(PATH64_ENABLE_FORTRAN)
add_executable (assign ${PATHSCALE_ASSIGN_SRCS})
set_property(TARGET assign PROPERTY LINK_FLAGS "-static")
target_link_libraries(assign pathfortran-host)

install(TARGETS assign
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${PATH64_LIB_PATH})

endif()
