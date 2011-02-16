set(WHIRL2C_SRC         init2c.cxx
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

add_library(whirl2c SHARED ${WHIRL2C_SRC})
ADD_DEPENDENCIES(whirl2c generate_preg_list)
set_property(TARGET whirl2c PROPERTY PREFIX "")
target_link_libraries(whirl2c be)
# install(TARGETS whirl2c LIBRARY DESTINATION ${PATH64_LIB_PATH})

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ADD_EXECUTABLE(whirl2c-exec whirl2c_main.cxx)
SET_TARGET_PROPERTIES(whirl2c-exec PROPERTIES OUTPUT_NAME "whirl2c")
add_dependencies(whirl2c-exec be-static)
install(TARGETS whirl2c-exec
    RUNTIME DESTINATION ${PATH64_LIB_PATH})
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"be\" \"${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/whirl2c_be\")")

ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
