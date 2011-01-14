
set(HEADERS
    ammintrin.h
    cpuid.h
	decfloat.h
	emmintrin.h
	float.h
	iso646.h
	limits.h
	mm3dnow.h
	mmintrin.h
	mm_malloc.h
    nmmintrin.h
	pmmintrin.h
    smmintrin.h
	stdarg.h
	stdbool.h
	stddef.h
	syslimits.h
    tmmintrin.h
	unwind.h
	varargs.h
	xmmintrin.h
)

set(header_outputs)
foreach(header ${HEADERS})
	add_custom_command(OUTPUT ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION}/include/${header}
	                   COMMAND ${CMAKE_COMMAND} -E copy
                                   ${CMAKE_CURRENT_SOURCE_DIR}/${header}
                                   ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION}/include/${header}
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${header})
	list(APPEND header_outputs ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION}/include/${header})
endforeach()

add_custom_target(headers-stage ALL true DEPENDS ${header_outputs})
add_dependencies(compiler-stage-C headers-stage)

install(FILES ${HEADERS}
	DESTINATION lib/${PSC_FULL_VERSION}/include)

