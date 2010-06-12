add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/crtbegin.o
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.c -o ${Path64_BINARY_DIR}/lib/crtbegin.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.c
	VERBATIM
)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/crtbeginS.o
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -fPIC -c ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.c -o ${Path64_BINARY_DIR}/lib/crtbeginS.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.c
	VERBATIM
)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/crtend.o
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/crtend.c -o ${Path64_BINARY_DIR}/lib/crtend.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtend.c
	VERBATIM
)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/crtendS.o
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -fPIC -c ${CMAKE_CURRENT_SOURCE_DIR}/crtend.c -o ${Path64_BINARY_DIR}/lib/crtendS.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtend.c
	VERBATIM
)

add_custom_target(crtbegin ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/crtbegin.o
)

add_custom_target(crtbeginS ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/crtbeginS.o
)

add_custom_target(crtend ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/crtend.o
)

add_custom_target(crtendS ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/crtendS.o
)

install(FILES ${Path64_BINARY_DIR}/lib/crtbegin.o ${Path64_BINARY_DIR}/lib/crtbeginS.o ${Path64_BINARY_DIR}/lib/crtend.o ${Path64_BINARY_DIR}/lib/crtendS.o DESTINATION ${PATH64_LIB_PATH})
