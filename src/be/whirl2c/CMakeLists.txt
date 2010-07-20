set(WHIRL2C_SRC
            init2c.cxx
			PUinfo.cxx
			st2c.cxx
			stab_attr.cxx
			tcon2c.cxx
			token_buffer.cxx
			ty2c.cxx
			w2c_driver.cxx
			w2cf_parentize.cxx
			w2cf_symtab.cxx 
			whirl2c_common.cxx 
			wn_attr.cxx
			wn2c.cxx
			wn2c_pragma.cxx
			init.cxx)

ADD_DEFINITIONS(
	-DBUILD_WHIRL2C
	-DMONGOOSE_BE)

path64_set_multiarch_sources(whirl2c_sources COMMON ${WHIRL2C_SRC})
path64_add_multiarch_library(whirl2c SHARED whirl2c_sources)
path64_add_multiarch_dependencies(whirl2c generate_preg_list)
path64_set_multiarch_property(whirl2c PREFIX "")
path64_set_multiarch_property(whirl2c COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    path64_set_multiarch_sources(whirl2c-exec_sources COMMON whirl2c_main.cxx)
    path64_add_multiarch_executable(whirl2c-exec whirl2c-exec_sources)
    path64_set_multiarch_property(whirl2c-exec OUTPUT_NAME "whirl2c")
    path64_add_multiarch_multiarch_dependencies(whirl2c-exec be-static)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"be\" \"${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/${arch}/whirl2c_be\")")
    endforeach()
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

