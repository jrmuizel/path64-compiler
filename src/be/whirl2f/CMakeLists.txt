set(WHIRL2F_SRC 
            init2f.cxx
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

path64_set_multiarch_sources(whirl2f_sources COMMON ${WHIRL2F_SRC})
path64_add_multiarch_library(whirl2f SHARED whirl2f_sources)
path64_add_multiarch_dependencies(whirl2f generate_preg_list)
path64_set_multiarch_property(whirl2f PREFIX "")
path64_set_multiarch_property(whirl2f COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")
path64_install_multiarch(whirl2f)
path64_multiarch_link_multiarch_libraries(whirl2f comutil be)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    path64_set_multiarch_sources(whirl2f-exec_sources COMMON whirl2f_main.cxx)
    path64_add_multiarch_executable(whirl2f-exec whirl2f-exec_sources)
    path64_set_multiarch_property(whirl2f-exec OUTPUT_NAME "whirl2f")
    path64_add_multiarch_multiarch_dependencies(whirl2f-exec be-static)
    path64_install_multiarch(whirl2f-exec)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"be\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/${arch}/whirl2f_be\")")
    endforeach()
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

