
string(REPLACE " " ";" c_flags "${CMAKE_C_FLAGS}")
set(c_flags ${c_flags} -m32)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/x8664/32/crtbegin.o
        COMMAND ${CMAKE_C_COMPILER} ${c_flags} -c ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.S -o ${Path64_BINARY_DIR}/lib/x8664/32/crtbegin.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.S
	VERBATIM
)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/x8664/32/crtbeginS.o
        COMMAND ${CMAKE_C_COMPILER} ${c_flags} -DSHARED -c ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.S -o ${Path64_BINARY_DIR}/lib/x8664/32/crtbeginS.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.S
	VERBATIM
)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/x8664/32/crtend.o
        COMMAND ${CMAKE_C_COMPILER} ${c_flags} -c ${CMAKE_CURRENT_SOURCE_DIR}/crtend.S -o ${Path64_BINARY_DIR}/lib/x8664/32/crtend.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtend.S
	VERBATIM
)

add_custom_command(
        OUTPUT ${Path64_BINARY_DIR}/lib/x8664/32/crtendS.o
        COMMAND ${CMAKE_C_COMPILER} ${c_flags} -DSHARED -c ${CMAKE_CURRENT_SOURCE_DIR}/crtend.S -o ${Path64_BINARY_DIR}/lib/x8664/32/crtendS.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crtend.S
	VERBATIM
)

add_custom_target(crtbegin-x8664_32 ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/x8664/32/crtbegin.o
)

add_custom_target(crtbeginS-x8664_32 ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/x8664/32/crtbeginS.o
)

add_custom_target(crtend-x8664_32 ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/x8664/32/crtend.o
)

add_custom_target(crtendS-x8664_32 ALL
    DEPENDS ${Path64_BINARY_DIR}/lib/x8664/32/crtendS.o
)

install(FILES ${Path64_BINARY_DIR}/lib/x8664/32/crtbegin.o
              ${Path64_BINARY_DIR}/lib/x8664/32/crtbeginS.o
              ${Path64_BINARY_DIR}/lib/x8664/32/crtend.o
              ${Path64_BINARY_DIR}/lib/x8664/32/crtendS.o
        DESTINATION ${PATH64_LIB_PATH}/x8664/32)
