
Set(PATHSCALE_COMMON_DRIVER_SRCS "errors.c"
                                 "file_utils.c"
                                 "lang_defs.c"
                                 "string_utils.c"
   )

Set(PATHSCALE_DRIVER_SRCS ${PATHSCALE_COMMON_DRIVER_SRCS}
                          ${CMAKE_CURRENT_BINARY_DIR}/check_combos.c
                          ${CMAKE_CURRENT_BINARY_DIR}/implicits.c
                          "main.c"
                          "options.c"
                          "option_seen.c"
                          "get_options.c"
                          "opt_actions.c"
                          "file_names.c"
                          "objects.c"
                          "phases.c"
                          "run.c"
                          "special_options.c"
                          ${CMAKE_CURRENT_BINARY_DIR}/version_hg.c
                          "license.c"
   )

set(PATHSCALE_TABLE_SRCS  ${PATHSCALE_COMMON_DRIVER_SRCS}
                          "table.c"
   )

add_executable(table ${PATHSCALE_TABLE_SRCS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_executable(sort_options sort_options.cpp)
GET_TARGET_PROPERTY(sort_options_exe sort_options LOCATION)

add_custom_command(
        OUTPUT tmp.options.cpp
        COMMAND cpp -traditional -P -DX8664 -DPSC_PRODNAME='"Compiler"' -DKEY -DLITTLE_ENDIAN_HOST -DHOST_IA32 -DNOOPT ${CMAKE_CURRENT_SOURCE_DIR}/OPTIONS > tmp.options.cpp
        DEPENDS OPTIONS
        )

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        # COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/sort_options.sh ${CMAKE_CURRENT_SOURCE_DIR} OPTIONS -DX8664 -DPSC_PRODNAME='"Compiler"' -DKEY   -DLITTLE_ENDIAN_HOST -DHOST_IA32 -DNOOPT > ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P 
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/sort_options.${SHELL_EXT} ${sort_options_exe} > ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P 
        DEPENDS sort_options.${SHELL_EXT} OPTIONS sort_options tmp.options.cpp
        )

else()

# Detecting gawk and gcc and configuring sort_options script
set(SORT_OPTIONS ${CMAKE_CURRENT_BINARY_DIR}/sort_options.sh)
set(PATH64_GCC ${CMAKE_C_COMPILER})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sort_options.sh.cmake.in
               ${SORT_OPTIONS})


add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        COMMAND ${SORT_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR} OPTIONS -DX8664 -DPSC_PRODNAME='"Compiler"' -DKEY   -DLITTLE_ENDIAN_HOST -DHOST_IA32 -DNOOPT > ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P 
        DEPENDS ${SORT_OPTIONS} OPTIONS
        )

endif()


add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/check_combos.c ${CMAKE_CURRENT_BINARY_DIR}/implicits.c
        COMMAND table < ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        DEPENDS table ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        )


add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version_hg.c
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_version.${SHELL_EXT} > ${CMAKE_CURRENT_BINARY_DIR}/version_hg.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_version.${SHELL_EXT}
        )


include_directories(.
                    ../common/com
                    ${CMAKE_CURRENT_BINARY_DIR})


# TODO: set proper build date and time 
add_definitions(-std=gnu99
                -DLITTLE_ENDIAN_HOST
                -DHOST_IA32
                -DNOOPT
		-DNO_LICENSE_CHECK
                )

# TODO : This isn't correct.. The prefix only has to affect the install since driver changes depending on what it's named
set(exe_prefix )

add_executable(${exe_refix}pathcc ${PATHSCALE_DRIVER_SRCS})
install(TARGETS ${exe_prefix}pathcc
        RUNTIME DESTINATION bin)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(table iberty winlib)
target_link_libraries(${exe_refix}pathcc iberty winlib ws2_32)

add_custom_target(
    stageit-driver ALL
    DEPENDS pathcc
)

else()

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/bin/pathCC ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION} ${Path64_BINARY_DIR}/lib/be
    DEPENDS pathcc
    COMMAND ${SYMLINK_COMMAND} pathcc ${CMAKE_BINARY_DIR}/bin/pathCC
    COMMAND ${SYMLINK_COMMAND} . ${CMAKE_BINARY_DIR}/lib/${PSC_FULL_VERSION}
    COMMAND ${SYMLINK_COMMAND} ../bin/be ${CMAKE_BINARY_DIR}/lib/be
)

add_custom_target(
    stageit-driver ALL
    DEPENDS ${CMAKE_BINARY_DIR}/bin/pathCC ${CMAKE_BINARY_DIR}/lib/be ${CMAKE_BINARY_DIR}/lib/${PSC_FULL_VERSION}
)

install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathCC${CMAKE_EXECUTABLE_SUFFIX}\")")

endif()

if(PATH64_ENABLE_FORTRAN)
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathf95${CMAKE_EXECUTABLE_SUFFIX}\")")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathf90${CMAKE_EXECUTABLE_SUFFIX}\")")
endif()
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathcc-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")

install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathCC${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathCC-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")
if(PATH64_ENABLE_FORTRAN)
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathf95${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathf95-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathf90${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/bin/pathf90-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")
endif()
