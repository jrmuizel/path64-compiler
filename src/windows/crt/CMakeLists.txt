add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/mkstemp.o
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/mkstemp.c -o ${Path64_BINARY_DIR}/lib/mkstemp.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mkstemp.c
	VERBATIM
)


add_custom_target(mkstemp ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/mkstemp.o
)


install(FILES ${Path64_BINARY_DIR}/lib/mkstemp.o  DESTINATION ${PATH64_LIB_PATH})

