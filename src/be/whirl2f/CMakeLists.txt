set(WHIRL2F_SRC         init2f.cxx
			../whirl2c/diagnostics.cxx
			../whirl2c/PUinfo.cxx
			../whirl2c/stab_attr.cxx
			../whirl2c/token_buffer.cxx
			../whirl2c/w2cf_parentize.cxx
			../whirl2c/w2cf_symtab.cxx
			../whirl2c/wn_attr.cxx
			st2f.cxx
			tcon2f.cxx
			ty2f.cxx
			whirl2f_common.cxx
			wn2f.cxx
			wn2f_expr.cxx
			wn2f_io.cxx
			wn2f_load_store.cxx
			wn2f_stmt.cxx
			wn2f_pragma.cxx
			w2f_driver.cxx
			init.cxx)

ADD_DEFINITIONS(
	-DBUILD_WHIRL2F
	-DMONGOOSE_BE)

add_library(whirl2f SHARED ${WHIRL2F_SRC})
add_dependencies(whirl2f generate_preg_list)
set_property(TARGET whirl2f PROPERTY PREFIX "")
install(TARGETS whirl2f LIBRARY DESTINATION ${PATH64_LIB_PATH})

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_EXECUTABLE(whirl2f-exec whirl2f_main.cxx)
    SET_TARGET_PROPERTIES(whirl2f-exec PROPERTIES OUTPUT_NAME "whirl2f")
    add_dependencies(whirl2f-exec be-static)
    install(TARGETS whirl2f-exec
        RUNTIME DESTINATION ${PATH64_LIB_PATH})
    foreach(arch ${PATH64_ENABLE_ARCHES})
        install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"be\" \"${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/${arch}/whirl2f_be\")")
    endforeach()

ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
