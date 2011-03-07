
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
                          "targets.c"
                          "${CMAKE_CURRENT_BINARY_DIR}/targ_info.c"
                          ${CMAKE_CURRENT_BINARY_DIR}/version_hg.c
                          "license.c"
   )

set(PATHSCALE_TABLE_SRCS  ${PATHSCALE_COMMON_DRIVER_SRCS}
                          "table.c"
   )

set(SORT_FLAGS)

option(PATH64_ENABLE_PATHAS "Enables using pathas assembler" OFF)
if(PATH64_ENABLE_PATHAS)
    add_definitions(-DPATH64_ENABLE_PATHAS)
    list(APPEND SORT_FLAGS "-DPATH64_ENABLE_PATHAS")
endif()

option(PATH64_ENABLE_DEFAULT_PATHAS "Enables using pathas as default assembler" OFF)
if(PATH64_ENABLE_DEFAULT_PATHAS)
    add_definitions(-DPATH64_ENABLE_DEFAULT_PATHAS)
    list(APPEND SORT_FLAGS "-DPATH64_ENABLE_DEFAULT_PATHAS")
endif()


option(PATH64_ENABLE_PSCLANG "Enables using psclang front-end" OFF)
if(PATH64_ENABLE_PSCLANG)
    add_definitions(-DPATH64_ENABLE_PSCLANG)
    list(APPEND SORT_FLAGS "-DPATH64_ENABLE_PSCLANG")
endif()

option(PATH64_ENABLE_DEFAULT_PSCLANG "Enables using psclang as default front-end" OFF)
if(PATH64_ENABLE_DEFAULT_PSCLANG)
    add_definitions(-DPATH64_ENABLE_DEFAULT_PSCLANG)
    list(APPEND SORT_FLAGS "-DPATH64_ENABLE_DEFAULT_PSCLANG")
endif()


option(PATH64_NO_LICENSE_CHECK "Disables license check in driver" ON)
if(PATH64_NO_LICENSE_CHECK)
    add_definitions(-DNO_LICENSE_CHECK)
endif()


# Adding definitions for target architectures
set(ARCH_DEFS)
foreach(arch ${PATH64_ENABLE_ARCHES})
    set(ARCH_DEFS ${ARCH_DEFS} ${_PATH64_ARCH_FLAGS_${arch}})
endforeach()
add_definitions(${ARCH_DEFS})


add_executable(table ${PATHSCALE_TABLE_SRCS})

string(REGEX MATCH "^[^-]+" targ_arch ${PSC_TARGET})
path64_canonical_arch(targ_arch ${targ_arch})
path64_get_host_target(host_arch)
if(NOT ${host_arch} STREQUAL ${targ_arch})
    add_definitions(-DPATH64_CROSS)
endif()

# Detecting gcc and configuring sort_options script
set(SORT_OPTIONS ${CMAKE_CURRENT_BINARY_DIR}/sort_options.sh)

set(PATH64_GCC ${CMAKE_C_COMPILER})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sort_options.sh.cmake.in
               ${SORT_OPTIONS})


if(PATH64_ENABLE_PSCRUNTIME)
    list(APPEND SORT_FLAGS "-DPATH64_ENABLE_PSCRUNTIME")
else()
    list(APPEND SORT_FLAGS "-DPATH64_NO_PSCRUNTIME")
endif()


add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        COMMAND ${SORT_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR} OPTIONS ${ARCH_DEFS} ${SORT_FLAGS} -DPSC_PRODNAME='"Compiler"' -DKEY   -DLITTLE_ENDIAN_HOST -DHOST_IA32 -DNOOPT > ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P 
        DEPENDS ${SORT_OPTIONS} OPTIONS
        )

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/check_combos.c ${CMAKE_CURRENT_BINARY_DIR}/implicits.c
        COMMAND table < ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        DEPENDS table ${CMAKE_CURRENT_BINARY_DIR}/OPTIONS.P
        )


add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version_hg.c
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_version.sh > ${CMAKE_CURRENT_BINARY_DIR}/version_hg.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_version.sh
        )


# Generating target definitions
set(TARGET_DEFINITIONS "")
set(first true)
foreach(targ ${PATH64_ENABLE_TARGETS})
    set(arch ${_PATH64_TARGET_ARCH_${targ}})
    set(abi_name ${_PATH64_TARGET_BITS_${targ}})
    set(abi ${_PATH64_TARGET_ABI_${targ}})
    set(crt_path ${PSC_CRT_PATH_${targ}})
    set(crtbegin_path ${PSC_CRTBEGIN_PATH_${targ}})

    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(dyn_linker ${PSC_DYNAMIC_LINKER_${targ}})
        if(NOT dyn_linker)
            message(FATAL_ERROR "Dynamic linker for ${targ} target is not specified. Please specify PSC_DYNAMIC_LINKER_${targ} variable")
        else()
            message(STATUS "Dynamic linker for ${targ} target: ${dyn_linker}")
        endif()
    endif()

    if(NOT first)
        set(TARGET_DEFINITIONS "${TARGET_DEFINITIONS},\n")
    else()
        set(first false)
    endif()

    set(TARGET_DEFINITIONS "${TARGET_DEFINITIONS}\t{\"${targ}\",\t\"${arch}\",\t\"${abi_name}\",\t${abi},\t\"${crt_path}\",\t\"${crtbegin_path}\"")

    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(TARGET_DEFINITIONS "${TARGET_DEFINITIONS},\t\"${dyn_linker}\"")
    endif()

    if(NOT PATH64_ENABLE_PSCRUNTIME)
        set(supcpp_path ${PSC_LIBSUPCPP_PATH_${targ}})
        set(stdcpp_path ${PSC_LIBSTDCPP_PATH_${targ}})
        set(gcc_path ${PSC_LIBGCC_PATH_${targ}})
        set(gcc_eh_path ${PSC_LIBGCC_EH_PATH_${targ}})
        set(gcc_s_path ${PSC_LIBGCC_S_PATH_${targ}})

        if(NOT supcpp_path)
            message(FATAL_ERROR "Path to libsupc++ for target ${targ} is not specified. Please specify PSC_LIBSUPCPP_PATH_${targ} variable")
        endif()

        if(NOT stdcpp_path)
            message(FATAL_ERROR "Path to libstdc++ for target ${targ} is not specified. Please specify PSC_LIBSTDCPP_PATH_${targ} variable")
        endif()

        if(NOT gcc_path)
            message(FATAL_ERROR "Path to libgcc for target ${targ} is not specified. Please specify PSC_LIBGCC_PATH_${targ} variable")
        endif()

        if(NOT gcc_eh_path)
            message(FATAL_ERROR "Path to libgcc_eh for target ${targ} is not specified. Please specify PSC_LIBGCC_EH_PATH_${targ} variable")
        endif()

        if(NOT gcc_s_path)
            message(FATAL_ERROR "Path to libgcc_s for target ${targ} is not specified. Please specify PSC_LIBGCC_S_PATH_${targ} variable")
        endif()

        set(TARGET_DEFINITIONS "${TARGET_DEFINITIONS},\t\"${supcpp_path}\",\t\"${stdcpp_path}\",\t\"${gcc_path}\",\t\"${gcc_eh_path}\",\t\"${gcc_s_path}\"")
    endif()

    set(TARGET_DEFINITIONS "${TARGET_DEFINITIONS}}")
endforeach()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/targ_info.c.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/targ_info.c)

include_directories(.
                    ../common/com
                    ${CMAKE_CURRENT_BINARY_DIR})


# TODO: set proper build date and time 
add_definitions(-std=gnu99
                -DLITTLE_ENDIAN_HOST
                -DHOST_IA32
                -DNOOPT
                -DPSCSUB
                )

# TODO : This isn't correct.. The prefix only has to affect the install since driver changes depending on what it's named
set(exe_prefix )

add_executable(${exe_refix}pathcc ${PATHSCALE_DRIVER_SRCS})
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set_property(TARGET ${exe_prefix}pathcc PROPERTY LINK_FLAGS "-static")
endif()
install(TARGETS ${exe_prefix}pathcc
        RUNTIME DESTINATION bin)

add_custom_command(
    OUTPUT ${Path64_BINARY_DIR}/bin/pathCC ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION}
    DEPENDS pathcc
# Paths in symbolic links should be relative. Otherwise bootstrap will not work
    COMMAND ${CMAKE_COMMAND} -E create_symlink pathcc ${Path64_BINARY_DIR}/bin/pathCC
)

add_custom_target(
    stageit-driver ALL
    DEPENDS ${Path64_BINARY_DIR}/bin/pathCC
)

add_dependencies(compiler-stage stageit-driver)

install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathCC${CMAKE_EXECUTABLE_SUFFIX}\")")
if(PATH64_ENABLE_FORTRAN)
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathf95${CMAKE_EXECUTABLE_SUFFIX}\")")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathf90${CMAKE_EXECUTABLE_SUFFIX}\")")
endif()
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathcc${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathcc-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")

install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathCC${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathCC-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")
if(PATH64_ENABLE_FORTRAN)
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathf95${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathf95-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"pathf90${CMAKE_EXECUTABLE_SUFFIX}\" \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/pathf90-${PSC_FULL_VERSION}${CMAKE_EXECUTABLE_SUFFIX}\")")
endif()
